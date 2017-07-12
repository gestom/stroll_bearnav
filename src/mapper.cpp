#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <stdio.h>
#include <iostream>
#include <nav_msgs/Odometry.h>
#include <cmath>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;


ros::Publisher cmd_pub_;
ros::Subscriber odometrySub;
nav_msgs::Odometry odometry;
image_transport::Subscriber image_sub_;
image_transport::Publisher image_pub_;
double pointDist=0;
float totalDist=0;
double startx,starty,currentx,currenty,pointx,pointy;
int meter=-1;
double diffM=0;
char filename[100];
String name="/home/parallels/catkin_ws/datasets/images_cameleon/";
Mat img;


void odomcallback(const nav_msgs::Odometry::ConstPtr& msg){
	if(meter==-1){
	startx=msg->pose.pose.position.x;
	starty=msg->pose.pose.position.y;
	pointx=startx;
	pointy=starty;
	meter=0;	
	}	

	currentx=msg->pose.pose.position.x;
	currenty=msg->pose.pose.position.y;
	pointDist = sqrt(pow(currentx-pointx,2)+pow(currenty-pointy,2)); 
}

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
	img=cv_ptr->image;	

	//Save image from camera after meter
	if(pointDist+diffM>1){
		totalDist+=pointDist;
		diffM=pointDist+diffM-1;
		pointx=currentx;
		pointy=currenty;
		sprintf(filename,"%simage_%.3f.yaml",name.c_str(),totalDist);
		FileStorage fs(filename,FileStorage::WRITE);
		write(fs, "Image", img);
		fs.release();
	} 


}

int main(int argc, char** argv)
{ 
  ros::init(argc, argv, "image_converter");
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_(nh_);
  image_sub_ = it_.subscribe( "/stereo/left/image_raw", 1,imageCallback);
  odometrySub = nh_.subscribe<nav_msgs::Odometry>("/odom",10 ,odomcallback);
  ros::spin();
  return 0;
}

