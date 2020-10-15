#include <ros/ros.h>
#include "tf/transform_broadcaster.h"
#include <tf/transform_listener.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <nav_msgs/Odometry.h>
#include <sensor_msgs/JointState.h>
#include <std_msgs/Float32.h>
#include <dynamic_reconfigure/server.h>
#include <stroll_bearnav/distanceConfig.h>
#include <stroll_bearnav/SetDistance.h>


using namespace std;

ros::Publisher dist_pub_;
ros::Publisher distEvent_pub_;
ros::Subscriber odometrySub;
ros::Subscriber jointSub;
nav_msgs::Odometry odometry;
std_msgs::Float32 dist_;
std_msgs::Float32 distEvent_;
stroll_bearnav::distanceConfig config;

/* distance parameters */
int eventCounter=0;
float totalDist=0;
double currentX, currentY, currentRot, currentTranslation;
double lastX=FLT_MAX;
double lastY=FLT_MAX;
float distanceEvent=0;
float distanceThreshold=0.2;
double angularVelThreshold=4;
double robotRadius=0.25; // half of the robot width
double deltaTime=0.05; // encoder odometry 1/fps


/* service for set/reset the distance */
bool setDistance(stroll_bearnav::SetDistance::Request &req, stroll_bearnav::SetDistance::Response &res)
{   
	res.distance=req.distance;
	totalDist=req.distance;
	lastX = currentX;
	lastY = currentY;
	eventCounter = totalDist/distanceThreshold-1;
	ROS_INFO("Setting current travelled distance to %.3f, event counter to %i",(float)req.distance,eventCounter);
	dist_.data=totalDist;
	dist_pub_.publish(dist_);
	return true;
}

/*allow the distanceThreshold to be changed online*/
void callback(stroll_bearnav::distanceConfig &config, uint32_t level)
{	
	 eventCounter = eventCounter*distanceThreshold/config.distance_param;
	 distanceThreshold=config.distance_param;
 	 ROS_INFO("Changing distance event threshold: %f", config.distance_param);
}

/*FOR thorvald*/
void jointcallback(const sensor_msgs::JointState::ConstPtr& msg)
{
	/*if lastX and lastY are not initialized*/
	if (lastX == FLT_MAX && lastY == FLT_MAX){
		lastX=msg->position[5]/10;
		lastY=0;
	}	

	/*calculate eucledian distance from the last odometric position*/
	currentX=msg->position[5]/10;
	currentY=0;
	totalDist += sqrt(pow(currentX-lastX,2)+pow(currentY-lastY,2));
	lastX=currentX;
	lastY=currentY;

	/*if it exceeds distanceThreshold from the last send position, then send an event*/
	if (totalDist-eventCounter*distanceThreshold >= distanceThreshold)
	{
		eventCounter++;
		distEvent_.data=totalDist;
		distEvent_pub_.publish(distEvent_);
	} 
	dist_.data=totalDist;
	dist_pub_.publish(dist_);
}

void odomcallback(const nav_msgs::Odometry::ConstPtr& msg)
{
	/*if lastX and lastY are not initialized*/
	if (lastX == FLT_MAX && lastY == FLT_MAX){
		lastX=msg->pose.pose.position.x;
		lastY=msg->pose.pose.position.y;
	}	

	/*calculate eucledian distance from the last odometric position*/
	currentX=msg->pose.pose.position.x;
	currentY=msg->pose.pose.position.y;
	currentRot=msg->twist.twist.angular.z;
	currentTranslation=sqrt(pow(currentX-lastX,2)+pow(currentY-lastY,2));
	totalDist += currentTranslation;

	ROS_INFO("distance now: %f", totalDist);

	/*if(currentTranslation<0.01&&fabs(currentRot)>0.008) {
		totalDist += 0.1; //fabs(robotRadius)*fabs(currentRot)*deltaTime;
		ROS_WARN("add angular distance compensation: %f", totalDist);
	}
	else
		ROS_ERROR("translation: %f, rotation %f", currentTranslation, currentRot);*/

	// the incoming geometry_msgs::Quaternion is transformed to a tf::Quaterion
    tf::Quaternion quat;
    tf::quaternionMsgToTF(msg->pose.pose.orientation, quat);

    // the tf::Quaternion has a method to acess roll pitch and yaw
    double roll, pitch, yaw;
    tf::Matrix3x3(quat).getRPY(roll, pitch, yaw);

    if(yaw >= angularVelThreshold && sqrt(pow(currentX-lastX,2)+pow(currentY-lastY,2)) < distanceThreshold) {
    	totalDist += distanceThreshold;
    	ROS_INFO("Added distance compensation.");
    }


	lastX=currentX;
	lastY=currentY;

	/*if it exceeds distanceThreshold from the last send position, then send an event*/
	if (totalDist-eventCounter*distanceThreshold >= distanceThreshold)
	{
		eventCounter++;
		distEvent_.data=totalDist;
		distEvent_pub_.publish(distEvent_);
	} 
	dist_.data=totalDist;
	dist_pub_.publish(dist_);
}


int main(int argc, char** argv)
{ 
	ros::init(argc, argv, "odometry_monitor");
	ros::NodeHandle nh("~");

	nh.param("maxAngularSpeed", angularVelThreshold, 2.0); // override to record the rotation event

	//initiate action server
	dynamic_reconfigure::Server<stroll_bearnav::distanceConfig> server;
	dynamic_reconfigure::Server<stroll_bearnav::distanceConfig>::CallbackType f;
	f = boost::bind(&callback, _1, _2);
	server.setCallback(f);

	/* initiate service */
	ros::ServiceServer service = nh.advertiseService("/setDistance", setDistance);

	odometrySub = nh.subscribe<nav_msgs::Odometry>("/odom", 10 ,odomcallback);
	jointSub = nh.subscribe<sensor_msgs::JointState>("/joint_states",10 ,jointcallback);
	dist_pub_=nh.advertise<std_msgs::Float32>("/distance",1);
	distEvent_pub_=nh.advertise<std_msgs::Float32>("/distance_events",1);
	ros::spin();
	return 0;
}

