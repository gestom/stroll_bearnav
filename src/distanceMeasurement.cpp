#include <ros/ros.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <nav_msgs/Odometry.h>
#include <std_msgs/Float32.h>
using namespace std;


ros::Publisher dist_pub_;
ros::Publisher distEvent_pub_;
ros::Subscriber odometrySub;
nav_msgs::Odometry odometry;
std_msgs::Float32 dist_;
std_msgs::Float32 distEvent_;
double pointDist=0;
float totalDist=0;
double startx,starty,currentx,currenty,pointx,pointy;
float distanceEvent=0;
double diffM=0;
bool start=true;

void odomcallback(const nav_msgs::Odometry::ConstPtr& msg){
	if(start){
		pointx=msg->pose.pose.position.x;
		pointy=msg->pose.pose.position.y;
		start=false;	
	}	
	currentx=msg->pose.pose.position.x;
	currenty=msg->pose.pose.position.y;
	pointDist = sqrt(pow(currentx-pointx,2)+pow(currenty-pointy,2));
   
	
	if(pointDist+diffM>0.1){
		totalDist+=pointDist+diffM;
		diffM=pointDist+diffM-0.1;
		pointx=currentx;
		pointy=currenty;
		distanceEvent=totalDist;
		distEvent_.data=distanceEvent;
		distEvent_pub_.publish(distEvent_);
		
 		dist_.data=totalDist;
	}  else {
		dist_.data=totalDist+pointDist+diffM;
	}
	dist_pub_.publish(dist_);
	
}



int main(int argc, char** argv)
{ 
  ros::init(argc, argv, "distance");
  ros::NodeHandle nh_;
  odometrySub = nh_.subscribe<nav_msgs::Odometry>("/odom",10 ,odomcallback);
  dist_pub_=nh_.advertise<std_msgs::Float32>("/distance",1);
  distEvent_pub_=nh_.advertise<std_msgs::Float32>("/event/meter",1); 
  ros::spin();
  return 0;
}

