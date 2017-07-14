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
using namespace std;
using namespace cv;

ros::Publisher cmd_pub_;
ros::Subscriber featureSub_;
ros::Subscriber distEventSub_;
image_transport::Subscriber image_sub_;
image_transport::Publisher image_pub_;
float distanceTotalEvent=0;
char filename[100];
String name="/home/parallels/catkin_ws/datasets/images_cameleon/";
Mat img,descriptors;
Mat descriptor;
vector<KeyPoint> keypoints;
KeyPoint keypoint;
bool start=true;
stroll_bearnav::FeatureArray featureArray;
stroll_bearnav::Feature feature;
void distanceEventCallback(const std_msgs::Float32::ConstPtr& msg);
void featureCallback(const stroll_bearnav::FeatureArray::ConstPtr& msg);

void distanceEventCallback(const std_msgs::Float32::ConstPtr& msg){
	
	distanceTotalEvent=msg->data;
	sprintf(filename,"%simage_features%.3f.yaml",name.c_str(),distanceTotalEvent);
	FileStorage fs(filename,FileStorage::WRITE);
	write(fs, "Image", img);
	write(fs, "Keypoints", keypoints);
	write(fs, "Descriptors",descriptors);
	fs << "Distance" << distanceTotalEvent;
	fs.release();
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
}


void featureCallback(const stroll_bearnav::FeatureArray::ConstPtr& msg){
	
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
	if(start){
		sprintf(filename,"%simage_features0.yaml",name.c_str());
		FileStorage fs(filename,FileStorage::WRITE);
		write(fs, "Image", img);
		write(fs, "Keypoints", keypoints);
		write(fs, "Descriptors",descriptors);
		fs.release();
		start=false;

}
}


int main(int argc, char** argv)
{ 
	ros::init(argc, argv, "mapper");
	ros::NodeHandle nh_;
	image_transport::ImageTransport it_(nh_);
	image_sub_ = it_.subscribe( "/stereo/left/image_raw", 1,imageCallback);
	featureSub_ = nh_.subscribe<stroll_bearnav::FeatureArray>("/features",1,featureCallback);
    distEventSub_=nh_.subscribe<std_msgs::Float32>("/event/meter",1,distanceEventCallback);
    ros::spin();
    return 0;
}

