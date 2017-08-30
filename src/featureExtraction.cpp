#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <iostream>
#include <algorithm>
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
vector<KeyPoint> keypoints; 
Mat descriptors;
Mat img;
//Ptr<SURF> detector = SURF::create(surfThreshold);
Ptr<AgastFeatureDetector> detector = AgastFeatureDetector::create(surfThreshold,true,AgastFeatureDetector::OAST_9_16);
Ptr<BriefDescriptorExtractor> extractor = BriefDescriptorExtractor::create();

/* optimization parameters */
bool optimized = false;
clock_t t;

/* adaptive threshold parameters */
bool adaptThreshold = true;
int targetKeypoints = 100;
float featureOvershootRatio = 0.3;
int target_over;
void adaptive_threshold(vector<KeyPoint>& keypoints);

/* dynamic reconfigure of surf threshold and showing images */
void callback(stroll_bearnav::featureExtractionConfig &config, uint32_t level)
{
	adaptThreshold = config.adaptThreshold;
	if (!adaptThreshold) surfThreshold=config.thresholdParam;
	targetKeypoints = config.targetKeypoints;
	featureOvershootRatio = config.featureOvershootRatio;
	target_over = targetKeypoints + featureOvershootRatio/100.0 * targetKeypoints;

	/* optimize detecting features and measure time */
	optimized = config.optimized;

	detector->setThreshold(surfThreshold);

	ROS_DEBUG("Changing feature featureExtraction to %.3f, keypoints %i", surfThreshold, targetKeypoints);
}

/*to select most responsive features*/
bool compare_response(KeyPoint first, KeyPoint second)
{
	  if (first.response > second.response) return true; else return false;
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
	img=cv_ptr->image;

	/* Detect image features */
	t = clock();
	if(optimized && adaptThreshold){

		/* firstly only detect keypoints */
		detector->detect(img,keypoints, Mat () );
		sort(keypoints.begin(),keypoints.end(),compare_response);
		/* determine the next threshold */
		adaptive_threshold(keypoints);
	
		/* reduce keypoints size to desired number of keypoints */
		keypoints.erase(keypoints.begin()+ min(targetKeypoints,(int)keypoints.size()),keypoints.end());

		/* then compute descriptors only for desired number of keypoints */
		extractor->compute(img,keypoints,descriptors);

	} else {
		t = clock();
		/* detect keypoints and compute descriptors for all keypoints*/
		//detector->detectAndCompute(img, Mat (), keypoints,descriptors);

		if(adaptThreshold) adaptive_threshold(keypoints);
		keypoints.erase(keypoints.begin()+ min(targetKeypoints,(int)keypoints.size()),keypoints.end());

	}
	ROS_DEBUG("Time taken: %.4fs", (float)(clock() - t)/CLOCKS_PER_SEC);

	/* publish image features */
	featureArray.feature.clear();
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
	char numStr[100];
	sprintf(numStr,"Image_%09d",msg->header.seq);
	featureArray.id =  numStr;
	featureArray.distance = msg->header.seq;
	feat_pub_.publish(featureArray);

	/*and if there are any consumers, publish image with features*/
	if (image_pub_.getNumSubscribers()>0)
	{
		/* Show all detected features in image (Red)*/
		drawKeypoints( img, keypoints, cv_ptr->image, Scalar(0,0,255), DrawMatchesFlags::DEFAULT );
		/* publish image with features */
		image_pub_.publish(cv_ptr->toImageMsg());
		ROS_INFO("Features extracted %ld %ld",featureArray.feature.size(),keypoints.size());
	}
}

/* adaptive threshold - trying to target a given number of keypoints */
void adaptive_threshold(vector<KeyPoint>& keypoints)
{
	// supposes keypoints are sorted according to response (applies to surf)
	if(keypoints.size() > target_over) {
		surfThreshold =  ((keypoints[target_over].response + keypoints[target_over + 1].response) / 2);
		ROS_INFO("Keypoints %ld over  %i, missing %4ld, set threshold %.3f between responses %.3f %.3f",keypoints.size(),target_over, target_over - keypoints.size(),surfThreshold,keypoints[target_over].response,keypoints[target_over + 1].response);
	} else {
			/* compute average difference between responses of n last keypoints */
			if( keypoints.size() > 7) {
				int n_last = (int) round(keypoints.size()/5);
				float avg_dif = 0;

				for (int j = (keypoints.size() - n_last); j < keypoints.size() - 1; ++j) {
					avg_dif += keypoints[j].response - keypoints[j+1].response;
				}

			surfThreshold -= avg_dif/(n_last-1)*(target_over - keypoints.size());
			ROS_INFO("Keypoints %ld under %i, missing %4ld, set threshold %.3f from %i last features with %.3f difference",keypoints.size(),target_over,target_over - keypoints.size(),surfThreshold,n_last,avg_dif);

		} else {
			surfThreshold = 0;
			ROS_INFO("Keypoints %ld under %i, missing %4ld, set threshold %.3f ",keypoints.size(),target_over,target_over - keypoints.size(),surfThreshold);
		}
	}
	surfThreshold = fmax(surfThreshold,0);
	detector->setThreshold(surfThreshold);
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
