#include <ros/ros.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <nav_msgs/Odometry.h>
#include <std_msgs/Float32.h>
#include <dynamic_reconfigure/server.h>
#include <stroll_bearnav/distanceConfig.h>
#include <stroll_bearnav/SetDistance.h>


using namespace std;

ros::Publisher dist_pub_;
ros::Publisher distEvent_pub_;
ros::Subscriber odometrySub;
nav_msgs::Odometry odometry;
std_msgs::Float32 dist_;
std_msgs::Float32 distEvent_;
stroll_bearnav::distanceConfig config;

/* distance parameters */
int eventCounter=0;
float totalDist=0;
double currentX,currentY;
double lastX=FLT_MAX;
double lastY=FLT_MAX;
float distanceEvent=0;
float distanceThreshold=0.2;

/* service for set/reset the distance */
bool setDistance(stroll_bearnav::SetDistance::Request &req, stroll_bearnav::SetDistance::Response &res)
{   
	res.distance=req.distance;
	totalDist=req.distance;
	ROS_INFO("Request Distance: %f",(float)req.distance);
	return true;
}

/*allow the distanceThreshold to be changed online*/
void callback(stroll_bearnav::distanceConfig &config, uint32_t level)
{	
	 eventCounter = eventCounter*distanceThreshold/config.distance_param;
	 distanceThreshold=config.distance_param;
 	 ROS_INFO("Changing distance threshold: %f", config.distance_param);
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
	totalDist += sqrt(pow(currentX-lastX,2)+pow(currentY-lastY,2));
	lastX=currentX;
	lastY=currentY;

	/*if it exceeds distanceThreshold from the last send position, then send an event*/
	if (totalDist-eventCounter*distanceThreshold > distanceThreshold)
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
	ros::init(argc, argv, "distance");
	ros::NodeHandle nh;

	//initiate action server
	dynamic_reconfigure::Server<stroll_bearnav::distanceConfig> server;
	dynamic_reconfigure::Server<stroll_bearnav::distanceConfig>::CallbackType f;
	f = boost::bind(&callback, _1, _2);
	server.setCallback(f);

	/* initiate service */
    ros::ServiceServer service = nh.advertiseService("setDistance", setDistance);

	odometrySub = nh.subscribe<nav_msgs::Odometry>("/odom",10 ,odomcallback);
	dist_pub_=nh.advertise<std_msgs::Float32>("/distance",1);
	distEvent_pub_=nh.advertise<std_msgs::Float32>("/distance/events",1);
	ros::spin();
	return 0;
}

