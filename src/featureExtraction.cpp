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
bool imgShow = false;
bool publishImg = false;
void extract_features(cv_bridge::CvImagePtr& cv_ptr, vector<KeyPoint>& keypoints, Mat& descriptors);

/* optimization parameters */
bool optimized = false;
bool measure_time = false;
clock_t t;

/* adaptive threshold parameters */
bool adaptThreshold = true;
int targetKeypoints = 100;
float gain = 0.3;
int target_over;
void adaptive_threshold(vector<KeyPoint>& keypoints);

/* dynamic reconfigure of surf threshold and showing images */
void callback(stroll_bearnav::featureExtractionConfig &config, uint32_t level)
{
    adaptThreshold = config.adaptThreshold;
    if (!adaptThreshold) surfThreshold=config.thresholdParam;
    targetKeypoints = config.targetKeypoints;
    gain = config.gain;
	target_over = targetKeypoints + gain * targetKeypoints;

	/* optimize detecting features and measure time */
	optimized = config.optimized;
	measure_time = config.measure_time;
	
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

	/* Detect image features */
	if(optimized && adaptThreshold){

		/* measuring time of the whole process to find out whether this option is faster */
		if(measure_time) t = clock();

		/* firstly only detect keypoints */
		detector->detect(img_1,keypoints_1, Mat () );

		/* reduce keypoints size to desired number of keypoints */
		int fend = target_over;
		if (target_over > keypoints_1.size()) fend = keypoints_1.size();
		keypoints_1.erase(keypoints_1.begin()+fend,keypoints_1.end());

		/* then compute descriptors only for desired number of keypoints */
		detector->compute(img_1,keypoints_1,descriptors_1);

		adaptive_threshold(keypoints_1);
		extract_features(cv_ptr,keypoints_1,descriptors_1);

		if(measure_time) printf("\nTime taken: %.4f s\n", (float)(clock() - t)/CLOCKS_PER_SEC);

	} else {
		if(measure_time) t = clock();
		/* detect keypoints and compute descriptors for all keypoints*/
		detector->detectAndCompute(img_1, Mat (), keypoints_1,descriptors_1);

		if(adaptThreshold) adaptive_threshold(keypoints_1);
		extract_features(cv_ptr,keypoints_1,descriptors_1);

		if(measure_time) printf("\nTime taken: %.4f s\n", (float)(clock() - t)/CLOCKS_PER_SEC);
	}

	featureArray.id = "image_" + msg->header.seq;
	featureArray.distance = msg->header.seq;
	feat_pub_.publish(featureArray);
}

void extract_features(cv_bridge::CvImagePtr& cv_ptr, vector<KeyPoint>& keypoints, Mat& descriptors){

	featureArray.feature.clear();

	/* Show all detected features in image (Red)*/
	if(imgShow)
	{
		drawKeypoints( img_1, keypoints, cv_ptr->image, Scalar(0,0,255), DrawMatchesFlags::DEFAULT );
		//image_pub_.publish(cv_ptr->toImageMsg());
		imshow("Keypoints",cv_ptr->image);
		waitKey(1);
	}
	/* Save image features to variables */
	for(int i=0;i<keypoints.size();i++){
		feature.x=keypoints[i].pt.x;
		feature.y=keypoints[i].pt.y;
		feature.size=keypoints[i].size;
		feature.angle=keypoints[i].angle;
		feature.response=keypoints[i].response;
		feature.octave=keypoints[i].octave;
		feature.class_id=keypoints[i].class_id;
		descriptors.row(i).copyTo(feature.descriptor);
		if(adaptThreshold) {
			if(i < targetKeypoints) featureArray.feature.push_back(feature);
		} else {
			featureArray.feature.push_back(feature);
		}

	}
	/* publish image with features */
	if(publishImg)
	{
		image_pub_.publish(cv_ptr->toImageMsg());
	}
	ROS_INFO("Features extracted %ld",featureArray.feature.size());
}

/* adaptive threshold - trying to target a given number of keypoints */
void adaptive_threshold(vector<KeyPoint>& keypoints){
	// supposes keypoints are sorted according to response (applies to surf)

		if(keypoints.size() > target_over) {
			surfThreshold =  ((keypoints[target_over].response + keypoints[target_over + 1].response) / 2);
			ROS_INFO("Keypoints %ld over %i , set threshold %.3f between responses %.3f %.3f, missing %ld",keypoints.size(),target_over,surfThreshold,keypoints[target_over].response,keypoints[target_over + 1].response, target_over - keypoints.size());
		} else {
			/* compute average difference between responses of n last keypoints */
			if( keypoints.size() > 1) {
				int n_last = keypoints.size()/5;
				float avg_dif = 0;

				for (int j = (keypoints.size() - n_last); j < keypoints.size() - 1; ++j) {
					avg_dif += keypoints[j].response - keypoints[j+1].response;
				}

				surfThreshold -= avg_dif/(n_last-1)*(target_over - keypoints.size());
				ROS_INFO("Keypoints %ld under %i , set threshold %.3f, n_last %i, avg_dif %.3f, missing %ld ",keypoints.size(),target_over,surfThreshold,n_last,avg_dif,target_over - keypoints.size());

			} else {
				surfThreshold = 0;
				ROS_INFO("Keypoints %ld under %i , set threshold %.3f, missing %ld ",keypoints.size(),target_over,surfThreshold,target_over - keypoints.size());
			}
		}
		surfThreshold = fmax(surfThreshold,0);
		detector->setHessianThreshold(surfThreshold);
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
	image_sub_ = it_.subscribe( "/image", 1,imageCallback);
	image_pub_ = it_.advertise("/image_with_features", 1);

	ros::spin();
	return 0;
}
