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
#include <actionlib/server/simple_action_server.h>
using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;
static const std::string OPENCV_WINDOW = "Image window";

Ptr<SURF> detector = SURF::create(100);
bool start=true;
int tenCounter=10;
vector<KeyPoint> keypoints_1, keypoints_2,keypoints_3;
Mat descriptors_1, descriptors_2,descriptors_3;
Mat img_matches, img_t1,img_t2,img_matchestr,img_keypoints_1,img_3;
typedef actionlib::SimpleActionServer<stroll_bearnav::navigatorAction> Server;
Server *server;
bool done=false;
stroll_bearnav::navigatorResult result;
stroll_bearnav::navigatorFeedback feedback;

ros::Publisher cmd_pub_;
ros::Subscriber featureSub_;
ros::Subscriber loadFeatureSub_;
ros::Subscriber speedSub_;
ros::Subscriber distSub_;

geometry_msgs::Twist twist;
nav_msgs::Odometry odometry;
image_transport::Subscriber image_sub_;
image_transport::Publisher image_pub_;
Mat img_2;
KeyPoint keypoint,keypoint2;
double pointDist;
int totalDist;
double startx,starty,currentx,currenty,pointx,pointy;
int meter=-1;
char filename[100];
stroll_bearnav::FeatureArray featureArray;
stroll_bearnav::Feature feature; 
float ratioMatchConstant = 0.7;
int currentPathElement = 0;
int minGoodFeatures = 0;

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

/* reference map received */
void loadFeatureCallback(const stroll_bearnav::FeatureArray::ConstPtr& msg)
{	 
	ROS_INFO("Received a new map reference map");
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
	currentPathElement = 0;
	while(state != IDLE){
		if(server->isPreemptRequested()){
			state = PREEMPTED;
			server->setPreempted(result);
		}
		if (/*server->succeeded()*/ true && state == COMPLETED){
			server->setSucceeded(result);
		}
		usleep(200000);
	}
	twist.linear.x = twist.linear.y = twist.linear.z = twist.angular.y = twist.angular.x = 0.0;	
	cmd_pub_.publish(twist);
}

void featureCallback(const stroll_bearnav::FeatureArray::ConstPtr& msg)
{
	if(state == NAVIGATING){
		keypoints_2.clear();
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
		float differenceRot=0;

		/*establish correspondences, build the histogram and determine robot heading*/
		int count=0,bestc=0;
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
			int numBins = 21;
			count=0;
			bestc=0;
			int histogram[numBins];
			int granularity = 20;
			int differences[num];
			std::vector< DMatch > best_matches;
			for (int i = 0;i<numBins;i++) histogram[i] = 0;

			for (int i=0;i<num;i++){

				int idx2=good_matches[i].trainIdx;
				int idx1=good_matches[i].queryIdx;
				matched_points1.push_back(keypoints_1[idx1].pt);
				matched_points2.push_back(keypoints_2[idx2].pt);
				/*difference in x and y positions*/
				current.x=round(matched_points1[i].x-matched_points2[i].x);	
				current.y=round(matched_points1[i].y-matched_points2[i].y);
				int difference = current.x;
				int index = (difference+granularity/2)/granularity + numBins/2;
				if (fabs(current.y) > 50){
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
	
			/* rotation between features based on histogram voting */
			int rotation=(position-numBins/2)*granularity;
			printf("\n");
			float sum=0;

			/* take only good correspondences */
			for(int i=0;i<num;i++){
				if (fabs(differences[i]-rotation) < granularity*1.5){
					sum+=differences[i];
					count++;
					best_matches.push_back(good_matches[i]);
				}
			}
			/*difference between features */
			differenceRot=sum/count; 

			cout << "Vektor: " << count << " " << differenceRot << endl;
		}
		
		if (path.size()>currentPathElement)
		{
			twist.linear.x = twist.linear.y = twist.linear.z = 0.0;
			twist.linear.x = path[currentPathElement].forward; 
			twist.angular.y = twist.angular.x = 0.0;

			twist.angular.z=path[currentPathElement].angular;
			/*check for number of good matches */
			if (count>minGoodFeatures) twist.angular.z+=differenceRot*0.0001;
			cmd_pub_.publish(twist);
		}
	}
	if (state == COMPLETED || state == PREEMPTED) state = IDLE;
}

void distanceCallback(const std_msgs::Float32::ConstPtr& msg)
{	
	/* check for end of path profile */
	if (currentPathElement+2 <= path.size())
	{
		if (path[currentPathElement+1].distance < msg->data) currentPathElement++;
	}else{
		state = COMPLETED;
	}
}

int main(int argc, char** argv)
{
	ros::init(argc, argv, "angle_from_features");

	ros::NodeHandle nh;
	image_transport::ImageTransport it_(nh);
	cmd_pub_ = nh.advertise<geometry_msgs::Twist>("cmd",1);
	featureSub_ = nh.subscribe( "/features", 1,featureCallback);
	loadFeatureSub_ = nh.subscribe("/load/features", 1,loadFeatureCallback);
	distSub_=nh.subscribe<std_msgs::Float32>("/distance",1,distanceCallback);
	speedSub_=nh.subscribe<stroll_bearnav::PathProfile>("/load/path",1,pathCallback);
  	/* Initiate action server */
	server = new Server (nh, "navigator", boost::bind(&actionServerCB, _1, server), false);
	server->start();

	ros::spin();
	return 0;
}
