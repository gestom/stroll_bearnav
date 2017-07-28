#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <stroll_bearnav/FeatureArray.h>
#include <stroll_bearnav/Feature.h>
#include <std_msgs/Float32.h>
#include <actionlib/server/simple_action_server.h>
#include <stroll_bearnav/mapperAction.h>
#include <stroll_bearnav/Speed.h>
#include <stroll_bearnav/SpeedArray.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>

using namespace std;
using namespace cv;

typedef enum
{
	IDLE,
	MAPPING,
	SAVING,
	TERMINATING
}EMappingState;

ros::Publisher cmd_pub_;
ros::Subscriber speed_sub_;
ros::Subscriber featureSub_;
ros::Subscriber distEventSub_;
ros::Subscriber distSub_;
image_transport::Subscriber image_sub_;
image_transport::Publisher image_pub_;
float distanceTotalEvent=0;
float distanceTravelled=0;
float flipperPosition=0;
EMappingState state = IDLE;

typedef actionlib::SimpleActionServer<stroll_bearnav::mapperAction> Server;
Server *server;
stroll_bearnav::mapperResult result;
stroll_bearnav::mapperFeedback feedback;
stroll_bearnav::Speed speed;
stroll_bearnav::SpeedArray	speedArray;
char name[100];
string fileName;
Mat img,descriptors;
Mat descriptor;
vector<KeyPoint> keypoints;
vector<float> path;
KeyPoint keypoint;
stroll_bearnav::FeatureArray featureArray;
stroll_bearnav::Feature feature;

/*joystick input parameters - axes that correspond to forward, turning and flipper speeds*/ 
int stopButton = 1;
int linearAxis = 1;
int angularAxis = 0;
int flipperAxis = 4;

/*these constants determine how quickly the robot moves based on the joystick input*/ 
double maxForwardSpeed = 0.2;
double maxAngularSpeed = 0.2;
double maxFlipperSpeed = 0.2;
double maxForwardAcceleration = 0.01;

/*listening to joystick and flipperVelocity, publishing commands*/
ros::Subscriber flipperSub;
ros::Subscriber joy_sub_;
ros::Publisher vel_pub_;

/*state variables - twist is the message that eventually gets to the ROS driver of the robot, other are obvious*/
geometry_msgs::Twist twist,lastTwist;
bool teleoperated = false;
double forwardAcceleration= 0;
double forwardSpeed = 0;
double flipperSpeed = 0;
double angularSpeed = 0;
double lastForwardSpeed = 0;
double lastFlipperSpeed = 0;
double lastAngularSpeed = 0;


void distanceEventCallback(const std_msgs::Float32::ConstPtr& msg);
void featureCallback(const stroll_bearnav::FeatureArray::ConstPtr& msg);

void distanceEventCallback(const std_msgs::Float32::ConstPtr& msg)
{   
	if(state == MAPPING){
		distanceTotalEvent=msg->data;
		state = SAVING;
	}
}

void distanceCallback(const std_msgs::Float32::ConstPtr& msg)
{   
	if(state != IDLE) distanceTravelled=msg->data;
}

void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
	if(state != IDLE){
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
		img=cv_ptr->image;
	}	
}

void executeCB(const stroll_bearnav::mapperGoalConstPtr &goal, Server *serv)
{
	state = MAPPING;
	fileName=goal->fileName;
	while(state == MAPPING || state == SAVING){
		usleep(200000);
		if(server->isPreemptRequested())
		{
			while(state == SAVING) usleep(200000);
			sprintf(name,"%s_%.3f.yaml",fileName.c_str(),distanceTotalEvent);
			ROS_INFO("Saving map to %s",name);
			FileStorage fs(name,FileStorage::WRITE);
			write(fs, "Image", img);
			write(fs, "Keypoints", keypoints);
			write(fs, "Descriptors",descriptors);
			fs.release();
		
			sprintf(name,"%s.yaml",fileName.c_str());
			ROS_INFO("Saving path profile to %s",name);
			FileStorage pfs(name,FileStorage::WRITE);
			write(pfs, "Path", path);
			pfs.release();
			server->setPreempted(result);
			state = TERMINATING;
		}
	}
	while(state == TERMINATING){
		ROS_INFO("Mapping complete, stopping robot.");
		usleep(200000);
	}
	forwardSpeed = angularSpeed = flipperSpeed = 0.0;
}

/*receiving joystick data*/
void joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
{     
	angularSpeed = forwardSpeed*maxAngularSpeed*joy->axes[angularAxis];
	forwardAcceleration = maxForwardAcceleration*joy->axes[linearAxis];;
	flipperSpeed = maxFlipperSpeed*joy->axes[flipperAxis];
	if  (joy->buttons[stopButton]) angularSpeed = forwardSpeed = flipperSpeed = 0;
} 

/*flipper position -- for stair traverse*/
void flipperCallback(const std_msgs::Float32::ConstPtr& msg)
{
	flipperPosition = msg->data;   
}

void featureCallback(const stroll_bearnav::FeatureArray::ConstPtr& msg)
{
	if(state == SAVING){
		keypoints.clear();
		descriptors=Mat();

		for(int i=0; i<msg->feature.size();i++){

			keypoint.pt.x=msg->feature[i].x;
			keypoint.pt.y=msg->feature[i].y;
			keypoint.size=msg->feature[i].size;
			keypoint.angle=msg->feature[i].angle;
			keypoint.response=msg->feature[i].response;
			keypoint.octave=msg->feature[i].octave;
			keypoint.class_id=msg->feature[i].class_id;
			keypoints.push_back(keypoint);
			int size=msg->feature[i].descriptor.size();
			Mat mat(1,size,CV_32FC1,(void*)msg->feature[i].descriptor.data());
			descriptors.push_back(mat);
		}
		sprintf(name,"%s/%.3f.yaml",fileName.c_str(),distanceTotalEvent);
		ROS_INFO("Saving map to %s",name);
		FileStorage fs(name,FileStorage::WRITE);
		write(fs, "Image", img);
		write(fs, "Keypoints", keypoints);
		write(fs, "Descriptors",descriptors);
		fs.release();
		state = MAPPING;
		feedback.fileName=name;
		server->publishFeedback(feedback);
	}
}


int main(int argc, char** argv)
{ 
	ros::init(argc, argv, "mapper");
	ros::NodeHandle nh;
	image_transport::ImageTransport it_(nh);
	ros::param::get("~fileName", fileName);

	nh.param("axis_linear", linearAxis, 1);
	nh.param("axis_angular", angularAxis, 0);
	nh.param("axis_flipper", flipperAxis, 4);
	nh.param("stopButton", stopButton, 1);

	nh.param("angularSpeed", maxAngularSpeed, 0.2);
	nh.param("forwardSpeed", maxForwardSpeed, 0.2);
	nh.param("flipperSpeed", maxFlipperSpeed, 0.2);
	nh.param("forwardAcceleration", maxForwardAcceleration, 0.01);

	vel_pub_ = nh.advertise<geometry_msgs::Twist>("cmd", 1);
	flipperSub = nh.subscribe("/flipperPosition", 1, flipperCallback);
	joy_sub_ = nh.subscribe<sensor_msgs::Joy>("joy", 10, joyCallback);

	image_sub_ = it_.subscribe( "/stereo/left/image_raw", 1,imageCallback);
	featureSub_ = nh.subscribe<stroll_bearnav::FeatureArray>("/features",1,featureCallback);
	distEventSub_=nh.subscribe<std_msgs::Float32>("/distance/events",1,distanceEventCallback);
	distSub_=nh.subscribe<std_msgs::Float32>("/distance",1,distanceCallback);
	cmd_pub_ = nh.advertise<geometry_msgs::Twist>("cmd",1);
	ROS_INFO( "%s", fileName.c_str());
	server = new Server (nh, "mapping", boost::bind(&executeCB, _1, server), false);
	server->start();

	while (ros::ok()){
		if (state == MAPPING)
		{
			lastForwardSpeed = forwardSpeed;
			lastAngularSpeed = angularSpeed;
			lastFlipperSpeed = flipperSpeed;
			forwardSpeed += forwardAcceleration;
			forwardSpeed = fmin(fmax(forwardSpeed,-maxForwardSpeed),maxForwardSpeed);
			twist.linear.x =  forwardSpeed;
			angularSpeed = fmin(fmax(angularSpeed,-maxAngularSpeed),maxAngularSpeed);
			twist.linear.z =  angularSpeed;;
			flipperSpeed = fmin(fmax(flipperSpeed,-maxFlipperSpeed),maxFlipperSpeed);
			twist.linear.y =  flipperSpeed;
			vel_pub_.publish(twist);
			if (lastForwardSpeed != forwardSpeed || lastAngularSpeed != angularSpeed || lastFlipperSpeed != flipperSpeed)
			{
				path.push_back(distanceTravelled);
				path.push_back(forwardSpeed);
				path.push_back(angularSpeed);
				path.push_back(flipperSpeed);
			}
		}
		if (state == TERMINATING){
			twist.linear.x = twist.angular.z = twist.angular.y = 0;
			vel_pub_.publish(twist);
			state = IDLE;
		}
		ros::spinOnce();
		usleep(50000);
	}
	return 0;
}

