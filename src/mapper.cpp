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
//#include <stroll_bearnav/Speed.h>
//#include <stroll_bearnav/SpeedArray.h>
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
EMappingState state = IDLE;

/* Action server parameters */
typedef actionlib::SimpleActionServer<stroll_bearnav::mapperAction> Server;
Server *server;
stroll_bearnav::mapperResult result;
stroll_bearnav::mapperFeedback feedback;

/* Image feature variables */
char name[100];
string fileName;
Mat img,descriptors;
Mat descriptor;
vector<KeyPoint> keypoints;
vector<float> path;
KeyPoint keypoint;

/* Feature messages */
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
float distanceTotalEvent=0;
float distanceTravelled=0;
float flipperPosition=0;


void distanceEventCallback(const std_msgs::Float32::ConstPtr& msg);
void featureCallback(const stroll_bearnav::FeatureArray::ConstPtr& msg);

/* Total distance travelled recieved from the event */ 
void distanceEventCallback(const std_msgs::Float32::ConstPtr& msg)
{   
	if(state == MAPPING){
		distanceTotalEvent=msg->data;
		state = SAVING;
	}
}
/*distance currently travelled */
void distanceCallback(const std_msgs::Float32::ConstPtr& msg)
{   
	distanceTravelled=msg->data;
}

/*Assign current image to variable */
void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
	if(state != IDLE){
		ROS_INFO("Catching image");
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
		ROS_INFO("Image caought");
	}	
}
/*Action server */
void executeCB(const stroll_bearnav::mapperGoalConstPtr &goal, Server *serv)
{
	fileName=goal->fileName;
	state = SAVING;
	while(state == MAPPING || state == SAVING){

		/*on preempt request end mapping and save current map */
		if(server->isPreemptRequested())
		{
			ROS_INFO("Map complete, flushing maps.");
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
		usleep(200000);
	}
	/* stop robot at the end of mapping*/
	while(state == TERMINATING){
		ROS_INFO("Mapping complete, stopping robot.");
		usleep(200000);
	}
	forwardSpeed = angularSpeed = flipperSpeed = 0.0;
}

/*receiving joystick data*/
void joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
{    
	angularSpeed = maxAngularSpeed*forwardSpeed*0.5*joy->axes[angularAxis];
	forwardAcceleration = maxForwardAcceleration*joy->axes[linearAxis];;
	flipperSpeed = maxFlipperSpeed*joy->axes[flipperAxis];
	if  (joy->buttons[stopButton]) angularSpeed = forwardSpeed = flipperSpeed = 0;
	ROS_INFO("Joystick pressed");
} 

/*flipper position -- for stair traverse*/
void flipperCallback(const std_msgs::Float32::ConstPtr& msg)
{
	flipperPosition = msg->data;   
}

/* save features and image recieved from camera as a local map*/
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
		sprintf(name,"%s_%.3f.yaml",fileName.c_str(),distanceTotalEvent);
		ROS_INFO("Saving map to %s",name);
		FileStorage fs(name,FileStorage::WRITE);
		//write(fs, "Image", img);
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
	/* joystick params */
	nh.param("axis_linear", linearAxis, 1);
	nh.param("axis_angular", angularAxis, 0);
	nh.param("axis_flipper", flipperAxis, 4);
	nh.param("stopButton", stopButton, 1);

	/* robot speed limits */
	nh.param("angularSpeed", maxAngularSpeed, 0.2);
	nh.param("forwardSpeed", maxForwardSpeed, 0.3);
	nh.param("flipperSpeed", maxFlipperSpeed, 0.5);
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
	
	/* Initiate action server */
	server = new Server (nh, "mapping", boost::bind(&executeCB, _1, server), false);
	server->start();

	path.clear();
	while (ros::ok()){
		if (state == MAPPING)
		{   /* speed limits */
			forwardSpeed += forwardAcceleration;
			forwardSpeed = fmin(fmax(forwardSpeed,-maxForwardSpeed),maxForwardSpeed);
			twist.linear.x =  forwardSpeed;
			angularSpeed = fmin(fmax(angularSpeed,-maxAngularSpeed),maxAngularSpeed);
			twist.angular.z =  angularSpeed;;
			flipperSpeed = fmin(fmax(flipperSpeed,-maxFlipperSpeed),maxFlipperSpeed);
			twist.angular.y =  flipperSpeed;
			vel_pub_.publish(twist);

			/* saving path profile */
			if (lastForwardSpeed != forwardSpeed || lastAngularSpeed != angularSpeed || lastFlipperSpeed != flipperSpeed)
			{
				path.push_back(distanceTravelled);
				path.push_back(forwardSpeed);
				path.push_back(angularSpeed);
				path.push_back(flipperSpeed);
				printf("%.3f %.3f %.3f %.3f\n",distanceTravelled,forwardSpeed,angularSpeed,flipperSpeed);
			}
			lastForwardSpeed = forwardSpeed;
			lastAngularSpeed = angularSpeed;
			lastFlipperSpeed = flipperSpeed;
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

