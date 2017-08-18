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
float surfThreshold = 400;
vector<KeyPoint> keypoints_1; 
Mat descriptors_1;
Mat img_1;
Ptr<SURF> detector = SURF::create(surfThreshold);
bool imgShow;
bool publishImg;

/* adaptive threshold parameters */
bool adaptThreshold;
int targetKeypoints;
float gain;
struct desc_resp
{
	template<class T>
	bool operator()(T const &a, T const &b) const { return a.response > b.response; }
};
int target_over;
int n_last;

/* dynamic reconfigure of surf threshold and showing images */
void callback(stroll_bearnav::featureExtractionConfig &config, uint32_t level)
{
    adaptThreshold = config.adaptThreshold;
    if (!adaptThreshold) surfThreshold=config.thresholdParam;
    targetKeypoints = config.targetKeypoints;
    gain = config.gain;
	target_over = targetKeypoints + gain * targetKeypoints;
	
    detector->setHessianThreshold(surfThreshold);

    ROS_DEBUG("Changing feature featureExtraction to %.3f, keypoints %i", surfThreshold, targetKeypoints);
	/* show image with features */
	imgShow=config.showImage;
	publishImg=config.publishImgMsg;
}

/* Extract features from image recieved from camera */
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
	img_1=cv_ptr->image;

	/* adaptive surf threshold - trying to target a given number of keypoints */
	if (adaptThreshold){
		std::sort(keypoints_1.begin(), keypoints_1.end(), desc_resp());
		if(keypoints_1.size() > target_over) {
			surfThreshold =  ((keypoints_1[target_over].response + keypoints_1[target_over + 1].response) / 2);
			ROS_INFO("Keypoints %ld over %i , set threshold %.3f between responses %.3f %.3f, missing %ld",keypoints_1.size(),target_over,surfThreshold,keypoints_1[target_over].response,keypoints_1[target_over + 1].response, target_over - keypoints_1.size());
		} else {
            /* compute average difference between responses of n last keypoints */
			n_last = keypoints_1.size()/5;
			float avg_dif = 0;
			if( keypoints_1.size() > 1) {
				for (int j = (keypoints_1.size() - n_last); j < keypoints_1.size() - 1; ++j) {
					avg_dif += keypoints_1[j].response - keypoints_1[j+1].response;
				}
			} else {
				avg_dif = 1000;
                n_last = 2;
			}
			surfThreshold -= avg_dif/(n_last-1)*(target_over - keypoints_1.size());
			ROS_INFO("Keypoints %ld under %i , set threshold %.3f, n_last %i, avg_dif %.3f, missing %ld ",keypoints_1.size(),target_over,surfThreshold,n_last,avg_dif,target_over - keypoints_1.size());

		}
		surfThreshold = fmax(surfThreshold,0);
		if(keypoints_1.size() == 0) surfThreshold = 0;
		detector->setHessianThreshold(surfThreshold);
	}

	/* Detect image features */
	detector->detectAndCompute(img_1, Mat (), keypoints_1,descriptors_1);
	featureArray.feature.clear();

	/* Show all detected features in image (Red)*/
	if(imgShow)
	{
		//image_pub_.publish(cv_ptr->toImageMsg());
		drawKeypoints( img_1, keypoints_1, cv_ptr->image, Scalar(0,0,255), DrawMatchesFlags::DEFAULT );
		imshow("Keypoints",cv_ptr->image);
		waitKey(1);
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
		if (adaptThreshold){
			if(i < targetKeypoints) featureArray.feature.push_back(feature);
		}else{
			featureArray.feature.push_back(feature);
		}

	}
		
	featureArray.id = "image_" + msg->header.seq;
	featureArray.distance = msg->header.seq;

	/* publish image features */
	feat_pub_.publish(featureArray);
	/* publish image with features */
	if(publishImg)
	{
		image_pub_.publish(cv_ptr->toImageMsg());
	}
	ROS_INFO("Features extracted %ld",featureArray.feature.size());	
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
	image_pub_ = it_.advertise("/image_with_features", 1);

	ros::spin();
	return 0;
}
