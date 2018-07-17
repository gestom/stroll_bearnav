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


vector<Point2f> matched_points1;
vector<Point2f> matched_points2;
vector< vector<DMatch> > matches,revmatches;
vector< DMatch > good_matches;
vector< DMatch > best_matches;
vector< DMatch > bad_matches;
stroll_bearnav::NavigationInfo info;
ros::Publisher cmd_pub_;
ros::Publisher info_pub_;
ros::Subscriber featureSub_;
ros::Subscriber loadFeatureSub_;
ros::Subscriber speedSub_;
ros::Subscriber distSub_;
ros::Subscriber distEventSub_;
image_transport::Subscriber image_sub_;
image_transport::Subscriber image_map_sub_;
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

bool showAllMatches=true;
bool showGoodMatches=true;
int numFeatureAdd = 50;
int numFeatureRemove = 50;
float remapRatio = 0.5;
bool plasticMap = true;

geometry_msgs::Twist twist;
nav_msgs::Odometry odometry;

/* Image features parameters */
Ptr<DescriptorMatcher> matcher;
vector<KeyPoint> mapKeypoints, currentKeypoints,keypointsGood,keypointsBest;
Mat mapDescriptors, currentDescriptors;
Mat img_goodKeypoints_1,currentImage,mapImage;
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
NormTypes featureNorm = NORM_INF;
int descriptorType = CV_32FC1;

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

/* Map features ratings parameters */
bool isRating = false;
int mapChanges=0;

/* Total distance travelled recieved from the event */
void distanceEventCallback(const std_msgs::Float32::ConstPtr& msg)
{
    isRating=true;
}

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
	//for (int i = 0;i<path.size();i++) printf("%.3f %.3f %.3f %.3f\n",path[i].distance,path[i].forward,path[i].angular,path[i].flipper);
}

/* dynamic reconfigure of showing images, velocity gain and matching ratio constant */
void callback(stroll_bearnav::navigatorConfig &config, uint32_t level)
{
	showAllMatches=config.showAllMatches;
	showGoodMatches=config.showGoodMatches;
	velocityGain=config.velocityGain;
	ratioMatchConstant=config.matchingRatio;
	maxVerticalDifference = config.maxVerticalDifference;
	plasticMap = config.plasticMap;	
	remapRatio = config.remapRatio;	
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
	mapKeypoints.clear();
	mapDescriptors.release();
	mapDescriptors = Mat();
	for(int i=0; i<msg->feature.size();i++){
		keypoint.pt.x=msg->feature[i].x;
		keypoint.pt.y=msg->feature[i].y;
		keypoint.size=msg->feature[i].size;
		keypoint.angle=msg->feature[i].angle;
		keypoint.response=msg->feature[i].response;
		keypoint.octave=msg->feature[i].octave;
		keypoint.class_id=msg->feature[i].class_id;
		mapKeypoints.push_back(keypoint);
		int size=msg->feature[i].descriptor.size();
		Mat mat(1,size,descriptorType,(void*)msg->feature[i].descriptor.data());
		mapDescriptors.push_back(mat);
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
	}
	twist.linear.x = twist.linear.y = twist.linear.z = twist.angular.z = twist.angular.y = twist.angular.x = 0.0;	
	cmd_pub_.publish(twist);
}

/* get image from camera */
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
	currentImage=cv_ptr->image;
}

/* get image from map */
void imageMapCallback(const sensor_msgs::ImageConstPtr& msg)
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
	mapImage=cv_ptr->image;
}

/*to select most rating matches*/
bool compare_rating(stroll_bearnav::Feature first, stroll_bearnav::Feature second)
{
	if (first.rating > second.rating) return true; else return false;
}


void featureCallback(const stroll_bearnav::FeatureArray::ConstPtr& msg)
{
	if(state == NAVIGATING){
		currentKeypoints.clear();
		keypointsBest.clear();
		keypointsGood.clear();
		currentDescriptors.release();
		currentDescriptors = Mat();

		/*determine the norm for feature matching*/
		if (msg->feature.size() > 0){
			ROS_INFO("Norm is %i",msg->feature[0].class_id);
			if (msg->feature[0].class_id != -1 && featureNorm != msg->feature[0].class_id)
			{
				matcher.release();
				featureNorm = (NormTypes) msg->feature[0].class_id; 
				if (featureNorm == NORM_HAMMING ||featureNorm == NORM_HAMMING2) descriptorType = CV_8U; else descriptorType = CV_32FC1;
				matcher = BFMatcher::create(featureNorm);
				ROS_INFO("Matcher switched to %i",featureNorm);
			}
		}

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
			currentKeypoints.push_back(keypoint);
			int size=msg->feature[i].descriptor.size();
			Mat mat(1,size,descriptorType,(void*)msg->feature[i].descriptor.data());
			currentDescriptors.push_back(mat);

		}

		/*eventually, recalculate map descriptors*/
		if (mapDescriptors.type() != descriptorType)
		{
			ROS_INFO("Recalculating map");
			mapDescriptors.release();
			mapDescriptors = Mat();
			for(int i=0; i<mapFeatures.feature.size();i++){
				int size=mapFeatures.feature[i].descriptor.size();
				Mat mat(1,size,descriptorType,(void*)mapFeatures.feature[i].descriptor.data());
				mapDescriptors.push_back(mat);
			}
		}
 
		good_matches.clear();

		int numBins = 41;
		int histogram[numBins];
		for (int i = 0;i<numBins;i++) histogram[i] = 0;

		best_matches.clear();
		bad_matches.clear();

		/*establish correspondences, build the histogram and determine robot heading*/
		int count=0,bestc=0;
		info.updated=false;
		info.view = *msg;
		matches.clear();
		if (mapKeypoints.size() >0 && currentKeypoints.size() >0){

			/*feature matching*/
			int knn = 5;
			try{
				 matcher->knnMatch( mapDescriptors, currentDescriptors, matches, knn);
				 //TODO crosscheck matching matcher->knnMatch( currentDescriptors,  mapDescriptors,revmatches, knn);
			}catch (Exception& e){
				matches.clear();
				ROS_ERROR("Feature desriptors from the map and in from the image are not compatible.");
			}
			/*perform ratio matching*/ 
			good_matches.reserve(matches.size());  
			for (size_t i = 0; i < matches.size(); i++)
			{
				if (matches[i][0].distance < ratioMatchConstant*matches[i][1].distance) good_matches.push_back(matches[i][0]);
				//TODO crosscheck matching //if (matches[i][0].trainIdx == revmatches[matches[i][0].trainIdx][0].queryIdx) 
			}
			
			/* rating view features	*/
			// maximasing minimal distance
			for (int i = 0; i < info.view.feature.size(); i++) {
				info.view.feature[i].rating=5000;
			}
			int nIdx=-1;
			float distance;
			for (int i = 0; i < matches.size(); i++) {
				for (int j = 0; j < matches[i].size(); j++) {
					nIdx=matches[i][j].trainIdx;
					distance=matches[i][j].distance;
					info.view.feature[nIdx].rating=fmin(info.view.feature[nIdx].rating,distance);
				}
			}
			sort(info.view.feature.begin(),info.view.feature.end(),compare_rating);
            //cout << "view: first " << info.view.feature[0].rating << " x " << info.view.feature[0].x << " last " << info.view.feature[info.view.feature.size()-1].rating  << " x " << info.view.feature[info.view.feature.size()-1].x  << endl;


			/*building histogram*/	
			int num=good_matches.size();
			matched_points1.clear();
			matched_points2.clear();
			Point2f current;
			Point2f best, possible;
			count=0;
			bestc=0;
			int granularity = 20;
			int *differences = (int*)calloc(num,sizeof(int));

			for (int i=0;i<num;i++){

				int idx2=good_matches[i].trainIdx;
				int idx1=good_matches[i].queryIdx;
				//printf("MATCH: %i %i %i\n",i,idx2,idx1);
				matched_points1.push_back(mapKeypoints[idx1].pt);
				matched_points2.push_back(currentKeypoints[idx2].pt);
				keypointsGood.push_back(currentKeypoints[idx2]);
				/*difference in x and y positions*/
				current.x=round(matched_points1[i].x-matched_points2[i].x);	
				current.y=round(matched_points1[i].y-matched_points2[i].y);
				int difference = current.x;
				int index = (difference+granularity/2)/granularity + numBins/2;
				if (fabs(current.y) > maxVerticalDifference){
					differences[i] = -1000000;
				}else{
					differences[i] = difference;
				//	if (index <= 0) index = 0;
				//	if (index >= numBins) index = numBins-1;
					if (index >= 0 || index < numBins) histogram[index]++;
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
			/* use good correspondences to determine heading */
			best_matches.clear();
			bad_matches.clear();
			/* take only good correspondences */
			for(int i=0;i<num;i++){
				if (fabs(differences[i]-rotation) < granularity*1.5){
					sum+=differences[i];
					count++;
					best_matches.push_back(good_matches[i]);
					keypointsBest.push_back(keypointsGood[i]);
				} else {
					bad_matches.push_back(good_matches[i]);
				}
			}
			free(differences);

			/* publish statistics */
			feedback.correct = best_matches.size();
			feedback.outliers = good_matches.size() - best_matches.size();
			feedback.keypoints_avg = (mapKeypoints.size() + currentKeypoints.size() )/2;
			feedback.matches = good_matches.size();
			/*difference between features */
			differenceRot=sum/count; 
			cout << "correct: " << feedback.correct << " out: " << feedback.outliers << " map " << mapKeypoints.size() << " cur " << currentKeypoints.size() << " gm " << feedback.matches << " difference " << differenceRot  << " distance " << feedback.distance << endl;
			//cout << "Vektor: " << count << " " << differenceRot << endl;
			//cout << "bm " << bad_matches.size()  << endl;
		}
		velocityGain = fmin(fmax(count/20.0,minimalAdaptiveSpeed),maximalAdaptiveSpeed);


				
		feedback.histogram.clear();
		if (count<minGoodFeatures) differenceRot = 0;
		for (int i = 0;i<numBins;i++) feedback.histogram.push_back(histogram[i]);

		/*forming navigation info messsage*/
		//info.mapID = currentMapID;
		info.histogram = feedback.histogram;
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
		for (int i = 0;i<best_matches.size();i++) {
			mapEval[best_matches[i].queryIdx] = 1;
			// rating map features
			if(isRating) mapFeatures.feature[best_matches[i].queryIdx].rating+=mapEval[best_matches[i].queryIdx];
		}
		if(isRating) 
		{
			for (int i = 0; i < bad_matches.size(); i++) {
				mapFeatures.feature[bad_matches[i].queryIdx].rating += mapEval[bad_matches[i].queryIdx];
			}

			numFeatureAdd = numFeatureRemove = best_matches.size()*remapRatio;

			// remove the worst rating from map
			sort(mapFeatures.feature.begin(), mapFeatures.feature.end(), compare_rating);
			if (numFeatureRemove >mapFeatures.feature.size()) numFeatureAdd = numFeatureRemove = mapFeatures.feature.size();
			if (numFeatureRemove > -1){
				mapFeatures.feature.erase(mapFeatures.feature.end() - numFeatureRemove, mapFeatures.feature.end());
			}else{
				mapFeatures.feature.erase(mapFeatures.feature.end() - bad_matches.size(), mapFeatures.feature.end());
			}
			if (plasticMap){
				mapFeatures.feature.clear();
				numFeatureAdd = 500;
			}
			// add the least similar features from view to map
			for (int i = 0; i < numFeatureAdd && i < info.view.feature.size(); i++) {
				info.view.feature[i].rating = 0;
				info.view.feature[i].x = info.view.feature[i].x + differenceRot;
				mapFeatures.feature.push_back(info.view.feature[i]);
				//info.view.feature.erase(info.view.feature.begin(), info.view.feature.begin() + 10);
			}

			isRating=false;
			mapChanges++;
			info.updated=true;
		}
		info.mapChanges=mapChanges;
		info.map = mapFeatures;
		info.mapMatchIndex = mapIndex;
		info.mapMatchEval = mapEval;
		info.correct = feedback.correct;
		info.matches = feedback.matches;
		info.distance = feedback.distance;
		info.diffRot = differenceRot;
		info_pub_.publish(info);

		/*Show good image features (Green) */
		Mat output,outtran; 
		if(image_pub_.getNumSubscribers()>0)
		{
			//drawKeypoints(currentImage,keypointsBest,img_goodKeypoints_1,Scalar(0,255,0), DrawMatchesFlags::DEFAULT );
			if (currentImage.rows >0 && mapKeypoints.size() >0 && currentKeypoints.size() >0)
			{
				if (mapImage.rows==0) mapImage = currentImage;
				Mat mapIm = mapImage.t();  
				Mat curIm = currentImage.t();
				vector<KeyPoint> kpMap,kpCur;
				KeyPoint tmp;
				for (int i = 0;i<mapKeypoints.size();i++)
				{
					tmp = mapKeypoints[i];	
					tmp.pt.y = mapKeypoints[i].pt.x;
					tmp.pt.x = mapKeypoints[i].pt.y;
					kpMap.push_back(tmp);
				} 
				for (int i = 0;i<currentKeypoints.size();i++)
				{
					tmp = currentKeypoints[i];	
					tmp.pt.y = currentKeypoints[i].pt.x;
					tmp.pt.x = currentKeypoints[i].pt.y;
					kpCur.push_back(tmp);
				}
				if (showAllMatches){
					drawMatches(mapIm,kpMap,curIm,kpCur,good_matches,outtran,Scalar(0,0,255),Scalar(0,0,255),vector<char>(),0);
					if (showGoodMatches) drawMatches(mapIm,kpMap,curIm,kpCur,best_matches,outtran,Scalar(0,255,0),Scalar(0,255,0),vector<char>(),3);
				}else{
					if (showGoodMatches) drawMatches(mapIm,kpMap,curIm,kpCur,best_matches,outtran,Scalar(0,255,0),Scalar(0,255,0),vector<char>(),2);
				}
				output = outtran.t();
				std_msgs::Header header;
				cv_bridge::CvImage bridge(header, sensor_msgs::image_encodings::BGR8, output);
				image_pub_.publish(bridge.toImageMsg());
			}
		}

		/* publish statistics */
		feedback.diffRot = differenceRot;
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
			/*if not in demo mode*/
			if(path.size() > 0 || showAllMatches == false  && showGoodMatches == false) state = COMPLETED;
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
		/*used for testing and demos*/
		if (path.size()==0)
		{
			twist.linear.x = twist.linear.y = twist.linear.z = 0.0;
			twist.angular.y = twist.angular.x = 0.0;
			twist.angular.z =differenceRot*pixelTurnGain;
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
	ros::init(argc, argv, "navigator");

	ros::NodeHandle nh;
	image_transport::ImageTransport it_(nh);
	image_sub_ = it_.subscribe( "/image_with_features", 1,imageCallback);
	image_map_sub_ = it_.subscribe( "/map_image", 1,imageMapCallback);
	cmd_pub_ = nh.advertise<geometry_msgs::Twist>("cmd",1);
	info_pub_ = nh.advertise<stroll_bearnav::NavigationInfo>("/navigationInfo",1);
	image_pub_ = it_.advertise("/navigationMatches", 1);

	featureSub_ = nh.subscribe( "/features", 1,featureCallback);
	loadFeatureSub_ = nh.subscribe("/localMap", 1,loadFeatureCallback);
	distSub_=nh.subscribe<std_msgs::Float32>("/distance",1,distanceCallback);
    distEventSub_=nh.subscribe<std_msgs::Float32>("/distance_events",1,distanceEventCallback);
	speedSub_=nh.subscribe<stroll_bearnav::PathProfile>("/pathProfile",1,pathCallback);
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
