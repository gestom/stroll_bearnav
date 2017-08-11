#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <iostream>
#include <stroll_bearnav/FeatureArray.h>
#include <stroll_bearnav/Feature.h>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/features2d.hpp>
#include <dynamic_reconfigure/server.h>
#include <stroll_bearnav/featureExtractionConfig.h>
using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;
static const std::string OPENCV_WINDOW = "Image window";

image_transport::Subscriber image_sub_;
image_transport::Publisher image_pub_;
stroll_bearnav::FeatureArray featureArray;
stroll_bearnav::Feature feature;
ros::Publisher feat_pub_;

/* image feature parameters */
int surfThreshold = 1000;
vector<KeyPoint> keypoints_1; 
Mat descriptors_1;
Mat img_1;
Ptr<SURF> detector = SURF::create(surfThreshold);
bool imgShow;

/* adaptive threshold parameters */
bool adaptThreshold;
int targetKeypoints;
int gain;

/* dynamic reconfigure of surf threshold and showing images */
void callback(stroll_bearnav::featureExtractionConfig &config, uint32_t level)
{
    adaptThreshold = config.adaptThreshold;
    if (!adaptThreshold) surfThreshold=config.thresholdParam;
    targetKeypoints = config.targetKeypoints;
    gain = config.gain;
	
    detector->setHessianThreshold(surfThreshold);
    ROS_DEBUG("Changing feature featureExtraction to %d, keypoints %i", surfThreshold, targetKeypoints);
	/* show image with features */
	imgShow=config.showImage;
}

/* Extract features from image recieved from camera */
void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{

	cv_bridge::CvImagePtr cv_ptr;
	try
	{
		cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::MONO8);
	}
	catch (cv_bridge::Exception& e)
	{
		ROS_ERROR("cv_bridge exception: %s", e.what());
		return;
	}
	img_1=cv_ptr->image;
	/* Detect image features */
	detector->detectAndCompute(img_1, Mat (), keypoints_1,descriptors_1);
	featureArray.feature.clear();

	/* Show all detected features in image (Red)*/
	if(imgShow)
	{
		drawKeypoints( img_1, keypoints_1, cv_ptr->image, Scalar(0,0,255), DrawMatchesFlags::DEFAULT );
		imshow("Keypoints",cv_ptr->image);
		image_pub_.publish(cv_ptr->toImageMsg());
	}
	/* Save image features to variables */	
	for(int i=0;i<keypoints_1.size();i++){
		feature.x=keypoints_1[i].pt.x;
		feature.y=keypoints_1[i].pt.y;
		feature.size=keypoints_1[i].size;
		feature.angle=keypoints_1[i].angle;
		feature.response=keypoints_1[i].response;
		feature.octave=keypoints_1[i].octave;
		feature.class_id=keypoints_1[i].class_id;
		descriptors_1.row(i).copyTo(feature.descriptor);
		featureArray.feature.push_back(feature);
	}
	/* publish image features */
	feat_pub_.publish(featureArray);

}

int main(int argc, char** argv)
{ 
	ros::init(argc, argv, "feature_extraction");
	ros::NodeHandle nh_;
	image_transport::ImageTransport it_(nh_);

	/* Initiate dynamic reconfiguration */
	dynamic_reconfigure::Server<stroll_bearnav::featureExtractionConfig> server;
	dynamic_reconfigure::Server<stroll_bearnav::featureExtractionConfig>::CallbackType f = boost::bind(&callback, _1, _2);
	server.setCallback(f);

	feat_pub_ = nh_.advertise<stroll_bearnav::FeatureArray>("/features",1);
	image_sub_ = it_.subscribe( "/stereo/left/image_raw", 1,imageCallback);
	image_pub_ = it_.advertise("/image_converter/output_video", 1);

    //adaptive surf threshold - trying to target a given number of keypoints
    if (adaptThreshold){
            surfThreshold += gain*(targetKeypoints - keypoints_1.size()); // p regulator
            detector->setHessianThreshold(surfThreshold);
    }

	ros::spin();
	return 0;
}
