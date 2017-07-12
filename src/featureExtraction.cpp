#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <iostream>
#include <geometry_msgs/Twist.h>
#include <stroll_bearnav/FeatureArray.h>
#include <stroll_bearnav/Feature.h>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/features2d.hpp>
using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;
static const std::string OPENCV_WINDOW = "Image window";

Ptr<SURF> detector = SURF::create(100);
vector<KeyPoint> keypoints_1; 
Mat descriptors_1;
Mat img_matches, img_t1,img_t2,img_matchestr,img_keypoints_1,img_3;

image_transport::Subscriber image_sub_;
image_transport::Publisher image_pub_;
stroll_bearnav::FeatureArray featureArray;
stroll_bearnav::Feature feature;
ros::Publisher feat_pub_;
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
	
	transpose(cv_ptr->image,img_t1);
	detector->detectAndCompute(img_t1, Mat (), keypoints_1,descriptors_1);
	descriptors_1.convertTo(descriptors_1,CV_32FC1);
	

	if (keypoints_1.size() > 0){
	for(int i=0;i<keypoints_1.size();i++){
	    feature.x=keypoints_1[i].pt.x;
		feature.y=keypoints_1[i].pt.y;
		feature.size=keypoints_1[i].size;
		feature.angle=keypoints_1[i].angle;
		feature.response=keypoints_1[i].response;
		feature.octave=keypoints_1[i].octave;
		feature.class_id=keypoints_1[i].class_id;
		feature.descriptor=descriptors_1.row(i);
	//	cout << "testuju descriptory" << descriptors_1.row(i) << endl;
		
    featureArray.feature.push_back(feature);
	}
	}
	feat_pub_.publish(featureArray);

}

int main(int argc, char** argv)
{ 
  ros::init(argc, argv, "image_converter");
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_(nh_);
  feat_pub_ = nh_.advertise<stroll_bearnav::FeatureArray>("/features",1);
  image_sub_ = it_.subscribe( "/stereo/left/image_raw", 1,imageCallback);
  image_pub_ = it_.advertise("/image_converter/output_video", 1);
  ros::spin();
  return 0;
}
