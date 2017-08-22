#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <geometry_msgs/Twist.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <iostream>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
#include <stroll_bearnav/FeatureArray.h>
#include <stroll_bearnav/Feature.h>
#include <stroll_bearnav/PathProfile.h>
#include <cmath>
#include <std_msgs/Float32.h>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/features2d.hpp>
#include <stroll_bearnav/navigatorAction.h>
#include <stroll_bearnav/SetDistance.h>
#include <actionlib/server/simple_action_server.h>
#include <dynamic_reconfigure/server.h>
#include <stroll_bearnav/navigatorConfig.h>
#include <stroll_bearnav/NavigationInfo.h>
using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;
static const std::string OPENCV_WINDOW = "Image window";


ros::Publisher cmd_pub_;
ros::Publisher info_pub_;
ros::Subscriber featureSub_;
ros::Subscriber loadFeatureSub_;
ros::Subscriber speedSub_;
ros::Subscriber distSub_;
image_transport::Subscriber image_sub_;
image_transport::Publisher image_pub_;

/* Service for set/reset distance */
stroll_bearnav::SetDistance srv;
ros::ServiceClient client;

/* Action server */
typedef actionlib::SimpleActionServer<stroll_bearnav::navigatorAction> Server;
Server *server;
stroll_bearnav::navigatorResult result;
stroll_bearnav::navigatorFeedback feedback;
stroll_bearnav::FeatureArray mapFeatures;


geometry_msgs::Twist twist;
nav_msgs::Odometry odometry;

/* Image features parameters */
vector<KeyPoint> keypoints_1, keypoints_2,keypointsGood,keypointsBest;
Mat descriptors_1, descriptors_2;
Mat img_goodKeypoints_1,img_keypoints_1;
KeyPoint keypoint,keypoint2;
float ratioMatchConstant = 0.7;
int currentPathElement = 0;
float currentDistance = 0;
int minGoodFeatures = 2;
float pixelTurnGain = 0.0001;
float differenceRot=0;
float minimalAdaptiveSpeed = 1.0;
float maximalAdaptiveSpeed = 1.0;
bool imgShow;

/* Feature message */
stroll_bearnav::FeatureArray featureArray;
stroll_bearnav::Feature feature;
 
typedef struct
{
	float distance;
	float forward;
	float angular;
	float flipper;
}SPathElement;

typedef enum
{
	IDLE,
	NAVIGATING,
	PREEMPTED,
	COMPLETED
}ENavigationState;

ENavigationState state = IDLE;
vector<SPathElement> path;
float overshoot = 0;
double velocityGain=0;
int maxVerticalDifference = 0;

void pathCallback(const stroll_bearnav::PathProfile::ConstPtr& msg)
{
	SPathElement a;
	path.clear();
	/* save path profile to variable */
	for (int i = 0;i<msg->distance.size();i++)
	{
		a.distance = msg->distance[i];
		a.forward = msg->forwardSpeed[i];
		a.angular = msg->angularSpeed[i];
		a.flipper = msg->flipper[i];
		path.push_back(a);
	}
	for (int i = 0;i<path.size();i++) printf("%.3f %.3f %.3f %.3f\n",path[i].distance,path[i].forward,path[i].angular,path[i].flipper);
}

/* dynamic reconfigure of showing images, velocity gain and matching ratio constant */
void callback(stroll_bearnav::navigatorConfig &config, uint32_t level)
{
	imgShow=config.showImageMatches;
	velocityGain=config.velocityGain;
	ratioMatchConstant=config.matchingRatio;
	maxVerticalDifference = config.maxVerticalDifference;
	minGoodFeatures = config.minGoodFeatures;
	pixelTurnGain = config.pixelTurnGain;
	minimalAdaptiveSpeed = config.adaptiveSpeedMin;
	maximalAdaptiveSpeed = config.adaptiveSpeedMax;
}

/* reference map received */
void loadFeatureCallback(const stroll_bearnav::FeatureArray::ConstPtr& msg)
{	 
	mapFeatures = *msg;
	ROS_INFO("Received a new reference map");
	keypoints_1.clear();
	descriptors_1=Mat();
	for(int i=0; i<msg->feature.size();i++){
		keypoint.pt.x=msg->feature[i].x;
		keypoint.pt.y=msg->feature[i].y;
		keypoint.size=msg->feature[i].size;
		keypoint.angle=msg->feature[i].angle;
		keypoint.response=msg->feature[i].response;
		keypoint.octave=msg->feature[i].octave;
		keypoint.class_id=msg->feature[i].class_id;
		keypoints_1.push_back(keypoint);
		int size=msg->feature[i].descriptor.size();
		Mat mat(1,size,CV_32FC1,(void*)msg->feature[i].descriptor.data());
		descriptors_1.push_back(mat);
	}
}

void actionServerCB(const stroll_bearnav::navigatorGoalConstPtr &goal, Server *serv)
{
	state = NAVIGATING;
	int traversals = goal->traversals;
	currentPathElement = 0;

	/* reset distance using service*/
	srv.request.distance=overshoot=0;
	if (!client.call(srv)) ROS_ERROR("Failed to call service SetDistance provided by odometry_monitor node!");
	result.success = false;
	while(state != IDLE){
		if(server->isPreemptRequested()){
			state = PREEMPTED;
			server->setPreempted(result);
			state = IDLE;
		}
		if (result.success == false && state == COMPLETED)
		{
			if (traversals<=1){
				result.success = true;
				state = IDLE;
				server->setSucceeded(result);
			}else{
				sleep(2);
				srv.request.distance=overshoot;
				if (!client.call(srv)) ROS_ERROR("Failed to call service SetDistance provided by odometry_monitor node!");
				sleep(2);
				currentPathElement = 0;
				state = NAVIGATING;
				traversals--;
			}
		}
		usleep(200000);
	}
	twist.linear.x = twist.linear.y = twist.linear.z = twist.angular.z = twist.angular.y = twist.angular.x = 0.0;	
	cmd_pub_.publish(twist);
}

/* Extract features from image recieved from camera */
void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{

	cv_bridge::CvImagePtr cv_ptr;
	try
	{
		cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
	}
	catch (cv_bridge::Exception& e)
	{
		ROS_ERROR("cv_bridge exception: %s", e.what());
		return;
	}
	img_keypoints_1=cv_ptr->image;
}
void featureCallback(const stroll_bearnav::FeatureArray::ConstPtr& msg)
{
	if(state == NAVIGATING){
		keypoints_2.clear();
		keypointsBest.clear();
		keypointsGood.clear();
		descriptors_2=Mat();

		/*reconstitute features from the incoming message*/
		for(int i=0; i<msg->feature.size();i++)
		{
			keypoint.pt.x=msg->feature[i].x;
			keypoint.pt.y=msg->feature[i].y;
			keypoint.size=msg->feature[i].size;
			keypoint.angle=msg->feature[i].angle;
			keypoint.response=msg->feature[i].response;
			keypoint.octave=msg->feature[i].octave;
			keypoint.class_id=msg->feature[i].class_id;
			keypoints_2.push_back(keypoint);

			int size=msg->feature[i].descriptor.size();
			Mat mat(1,size,CV_32FC1,(void*)msg->feature[i].descriptor.data());
			descriptors_2.push_back(mat);
		}

		std::vector< DMatch > good_matches;

		int numBins = 21;
		int histogram[numBins];
		for (int i = 0;i<numBins;i++) histogram[i] = 0;

		/*establish correspondences, build the histogram and determine robot heading*/
		int count=0,bestc=0;
		std::vector< DMatch > best_matches;
		if (keypoints_1.size() >0 && keypoints_2.size() >0){

			/*feature matching*/
			Ptr<DescriptorMatcher> matcher = BFMatcher::create(NORM_L2);
			vector< vector<DMatch> > matches;
			matcher->knnMatch( descriptors_1, descriptors_2, matches, 2);

			/*perform ratio matching*/ 
			good_matches.reserve(matches.size());  
			for (size_t i = 0; i < matches.size(); i++)
			{ 
				if (matches[i][0].distance < ratioMatchConstant*matches[i][1].distance) good_matches.push_back(matches[i][0]);
			}

			/*building histogram*/	
			int num=good_matches.size();
			vector<Point2f> matched_points1;
			vector<Point2f> matched_points2;
			Point2f current;
			Point2f best, possible;
			count=0;
			bestc=0;
			int granularity = 20;
			int differences[num];

			for (int i=0;i<num;i++){

				int idx2=good_matches[i].trainIdx;
				int idx1=good_matches[i].queryIdx;
				matched_points1.push_back(keypoints_1[idx1].pt);
				matched_points2.push_back(keypoints_2[idx2].pt);
				keypointsGood.push_back(keypoints_2[idx2]);
				/*difference in x and y positions*/
				current.x=round(matched_points1[i].x-matched_points2[i].x);	
				current.y=round(matched_points1[i].y-matched_points2[i].y);
				int difference = current.x;
				int index = (difference+granularity/2)/granularity + numBins/2;
				if (fabs(current.y) > maxVerticalDifference){
					differences[i] = -1000000;
				}else{
					differences[i] = difference;
					if (index <= 0) index = 0;
					if (index >= numBins) index = numBins-1;
					histogram[index]++;
				}
				count=0; 
			}

			/*histogram printing*/
			int max=0;
			int position=0;
			printf("Bin: ");
			for (int i = 0;i<numBins;i++) {
				
				printf("%i ",histogram[i]);
				if (histogram[i]>max)
				{
					max=histogram[i];
					position=i;
				}
			}
			feedback.max = max;

			/* rotation between features based on histogram voting */
			int rotation=(position-numBins/2)*granularity;
			printf("\n");
			float sum=0;
			keypointsBest.clear();	
			/* take only good correspondences */
			for(int i=0;i<num;i++){
				if (fabs(differences[i]-rotation) < granularity*1.5){
					sum+=differences[i];
					count++;
					best_matches.push_back(good_matches[i]);
					keypointsBest.push_back(keypointsGood[i]);
				}
			}
			/* publish statistics */
			feedback.correct = best_matches.size();
			feedback.outliers = good_matches.size() - best_matches.size();
			feedback.keypoints_avg = (keypoints_1.size() + keypoints_2.size() )/2;
			feedback.matches = good_matches.size();
			/*difference between features */
			differenceRot=sum/count; 

			//cout << "Vektor: " << count << " " << differenceRot << endl;
		}
		velocityGain = fmin(fmax(count/20.0,minimalAdaptiveSpeed),maximalAdaptiveSpeed);
		stroll_bearnav::NavigationInfo info;

				
		feedback.histogram.clear();
		if (count<minGoodFeatures) differenceRot = 0;
		for (int i = 0;i<numBins;i++) feedback.histogram.push_back(histogram[i]);

		/*forming navigation info messsage*/
		//info.mapID = currentMapID;
		info.histogram = feedback.histogram;
		info.map = mapFeatures;
		info.view = *msg;
		info.ratio = ratioMatchConstant;
		info.mapMatchIndex.clear();
		vector<int> mapIndex(mapFeatures.feature.size());
		vector<int> mapEval(mapFeatures.feature.size());
		std::fill(mapIndex.begin(),mapIndex.end(),-1); 
		std::fill(mapEval.begin(),mapEval.end(),0); 
		for (int i = 0;i<good_matches.size();i++)
		{
			mapIndex[good_matches[i].queryIdx] = good_matches[i].trainIdx;
			mapEval[good_matches[i].queryIdx] = -1;
		}	
		for (int i = 0;i<best_matches.size();i++) mapEval[best_matches[i].queryIdx] = 1;
		
		info.mapMatchIndex = mapIndex;
		info.mapMatchEval = mapEval;
		info_pub_.publish(info);

		/*Show good image features (Green) */
		Mat haha; 
		if(imgShow)
		{
			//drawKeypoints(img_keypoints_1,keypointsBest,img_goodKeypoints_1,Scalar(0,255,0), DrawMatchesFlags::DEFAULT );
		//	drawMatches(img_keypoints_1,keypoints_1,img_keypoints_1,keypoints_2,good_matches,haha,Scalar(0,0,255),Scalar(0,0,255),vector<char>(),0);
			//imshow("Good Keypoints",img_goodKeypoints_1);
			waitKey(1);
		}

		/* publish statistics */
		std::vector<int> stats;
		feedback.stats.clear();
		stats.push_back(feedback.keypoints_avg);
		feedback.stats.push_back(feedback.keypoints_avg);
		stats.push_back(feedback.matches);
		feedback.stats.push_back(feedback.matches);
		stats.push_back(feedback.correct);
		feedback.stats.push_back(feedback.correct);
		stats.push_back(feedback.outliers);
		feedback.stats.push_back(feedback.outliers);

		server->publishFeedback(feedback);
	}
}

void distanceCallback(const std_msgs::Float32::ConstPtr& msg)
{	

	if (state == NAVIGATING){
		/* check for end of path profile */
		feedback.distance = currentDistance = msg->data;
		if (currentPathElement+2 <= path.size())
		{
			if (path[currentPathElement+1].distance < msg->data){
				//ROS_INFO("Next %i %f",currentPathElement,path[currentPathElement].forward);
				 currentPathElement++;
			}
		}else{
			state = COMPLETED;
		}
		if (path.size()>currentPathElement)
		{
			//ROS_INFO("MOVE %i %f",currentPathElement,path[currentPathElement].forward);
			twist.linear.x = twist.linear.y = twist.linear.z = 0.0;
			if (fabs(path[currentPathElement].angular) > 0.001) velocityGain = 1.0;
			twist.linear.x = path[currentPathElement].forward*velocityGain; 
			twist.angular.y = twist.angular.x = 0.0;
			twist.angular.z=path[currentPathElement].angular*velocityGain;
			twist.angular.z+=differenceRot*pixelTurnGain;
			cmd_pub_.publish(twist);
		}
	}
	if (state == COMPLETED){
		if (path.size() > 0) overshoot = msg->data-path[path.size()-1].distance;
		twist.linear.x = twist.linear.y = twist.linear.z = 0.0;
		twist.angular.z = twist.angular.y = twist.angular.x = 0.0;
		cmd_pub_.publish(twist);
	}
}

int main(int argc, char** argv)
{
	ros::init(argc, argv, "angle_from_features");

	ros::NodeHandle nh;
	image_transport::ImageTransport it_(nh);
	image_sub_ = it_.subscribe( "/image_with_features", 1,imageCallback);
	cmd_pub_ = nh.advertise<geometry_msgs::Twist>("cmd",1);
	info_pub_ = nh.advertise<stroll_bearnav::NavigationInfo>("navigationInfo",1);
	featureSub_ = nh.subscribe( "/features", 1,featureCallback);
	loadFeatureSub_ = nh.subscribe("/load/features", 1,loadFeatureCallback);
	distSub_=nh.subscribe<std_msgs::Float32>("/distance",1,distanceCallback);
	speedSub_=nh.subscribe<stroll_bearnav::PathProfile>("/load/path",1,pathCallback);
  	/* Initiate action server */
	server = new Server (nh, "navigator", boost::bind(&actionServerCB, _1, server), false);
	server->start();

	/* Initiate service */
	client = nh.serviceClient<stroll_bearnav::SetDistance>("setDistance");
	/* Initiate dynamic reconfiguration */
	dynamic_reconfigure::Server<stroll_bearnav::navigatorConfig> server;
	dynamic_reconfigure::Server<stroll_bearnav::navigatorConfig>::CallbackType f = boost::bind(&callback, _1, _2);
	server.setCallback(f);

	ros::spin();
	return 0;
}
