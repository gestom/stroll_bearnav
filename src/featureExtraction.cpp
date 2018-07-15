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
#include <std_msgs/Int32.h>
using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;
static const std::string OPENCV_WINDOW = "Image window";

typedef enum
{
	DET_NONE = 0,
	DET_AGAST,
	DET_SURF,
	DET_UPSURF
}EDetectorType;

typedef enum
{
	DES_NONE = 0,
	DES_BRIEF,
	DES_SURF
}EDescriptorType;


image_transport::Subscriber image_sub_;
image_transport::Publisher image_pub_;
stroll_bearnav::FeatureArray featureArray;
stroll_bearnav::Feature feature;
ros::Publisher feat_pub_;

/* image feature parameters */
float detectionThreshold = 0;
vector<KeyPoint> keypoints; 
Mat descriptors;
Mat img;

/*kokoti hlava, co delala OCV3, me donutila delat to uplne debilne*/
Ptr<AgastFeatureDetector> agastDetector = AgastFeatureDetector::create(detectionThreshold);
Ptr<BriefDescriptorExtractor> briefDescriptor = BriefDescriptorExtractor::create();
Ptr<SURF> surf = SURF::create(detectionThreshold);
Ptr<SURF> upSurf = SURF::create(detectionThreshold,4,3,false,true);

EDetectorType usedDetector = DET_NONE;
EDescriptorType usedDescriptor = DES_NONE;
NormTypes featureNorm = NORM_INF;

/* optimization parameters */
bool optimized = false;
clock_t t;

/* adaptive threshold parameters */
bool adaptThreshold = true;
int targetKeypoints = 100;
float featureOvershootRatio = 0.3;
float maxLine = 0.5;
int target_over;
void adaptive_threshold(vector<KeyPoint>& keypoints);

int detectKeyPoints(Mat &image,vector<KeyPoint> &keypoints)
{
	cv::Mat img;
	if (maxLine < 1.0) img = image(cv::Rect(0,0,image.cols,(int)(image.rows*maxLine))); else img = image;
	if (usedDetector==DET_AGAST) agastDetector->detect(img,keypoints, Mat () );
	if (usedDetector==DET_SURF) upSurf->detect(img,keypoints, Mat () );
	if (usedDetector==DET_UPSURF) upSurf->detect(img,keypoints, Mat () );
}

int describeKeyPoints(Mat &image,vector<KeyPoint> &keypoints,Mat &descriptors)
{
	if (usedDescriptor==DES_BRIEF) briefDescriptor->compute(img,keypoints,descriptors);
	if (usedDescriptor==DES_SURF) surf->compute(img,keypoints,descriptors);
}

int detectAndDescribe(Mat &image,vector<KeyPoint> &keypoints,Mat &descriptors)
{
	if (usedDescriptor==DES_SURF && usedDetector == DET_SURF) surf->detectAndCompute(img,Mat(),keypoints,descriptors);
	else if (usedDescriptor==DES_SURF && usedDetector == DET_UPSURF) upSurf->detectAndCompute(img,Mat(),keypoints,descriptors);
	else {
		detectKeyPoints(image,keypoints);
		describeKeyPoints(image,keypoints,descriptors);
	} 
}

int setThreshold(int thres)
{
	if (usedDetector==DET_AGAST) agastDetector->setThreshold(thres);
	if (usedDetector==DET_SURF) surf->setHessianThreshold(thres);
	if (usedDetector==DET_UPSURF) upSurf->setHessianThreshold(thres);
}

/* dynamic reconfigure of surf threshold and showing images */
void callback(stroll_bearnav::featureExtractionConfig &config, uint32_t level)
{
	adaptThreshold = config.adaptThreshold;
	if (!adaptThreshold) detectionThreshold=config.thresholdParam;
	targetKeypoints = config.targetKeypoints;
	featureOvershootRatio = config.featureOvershootRatio;
	target_over = targetKeypoints + featureOvershootRatio/100.0 * targetKeypoints;
	maxLine = config.maxLine;

	/* optimize detecting features and measure time */
	optimized = config.optimized;
	usedDescriptor = (EDescriptorType) config.descriptor;
	usedDetector = (EDetectorType) config.detector;
	switch (usedDescriptor)
	{
		case DES_BRIEF:featureNorm = NORM_HAMMING;break;
		case DES_SURF:featureNorm = NORM_L2;break;
	}
	setThreshold(detectionThreshold);

	ROS_DEBUG("Changing feature featureExtraction to %.3f, keypoints %i", detectionThreshold, targetKeypoints);
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
		detectKeyPoints(img,keypoints);
		sort(keypoints.begin(),keypoints.end(),compare_response);
		/* determine the next threshold */
		adaptive_threshold(keypoints);
	
		/* reduce keypoints size to desired number of keypoints */
		keypoints.erase(keypoints.begin()+ min(targetKeypoints,(int)keypoints.size()),keypoints.end());

		/* then compute descriptors only for desired number of keypoints */
		describeKeyPoints(img,keypoints,descriptors);

	} else {

		/* detect keypoints and compute descriptors for all keypoints*/
		detectAndDescribe(img, keypoints,descriptors);

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
		feature.class_id=featureNorm;
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
	ROS_INFO("Provided features: %i",(int)featureArray.feature.size());
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
		detectionThreshold =  ((keypoints[target_over].response + keypoints[target_over + 1].response) / 2);
		float crisp = ((keypoints[targetKeypoints].response + keypoints[targetKeypoints + 1].response) / 2);
		ROS_INFO("Keypoints %ld over  %i, missing %4ld, set threshold %.3f between responses %.3f %.3f. Crisp threshold %.3f",keypoints.size(),target_over, target_over - keypoints.size(),detectionThreshold,keypoints[target_over].response,keypoints[target_over + 1].response,crisp);
	} else {
			/* compute average difference between responses of n last keypoints */
			if( keypoints.size() > 7) {
				int n_last = (int) round(keypoints.size()/5);
				float avg_dif = 0;

				for (int j = (keypoints.size() - n_last); j < keypoints.size() - 1; ++j) {
					avg_dif += keypoints[j].response - keypoints[j+1].response;
				}

			detectionThreshold -= avg_dif/(n_last-1)*(target_over - keypoints.size());
			ROS_INFO("Keypoints %ld under %i, missing %4ld, set threshold %.3f from %i last features with %.3f difference",keypoints.size(),target_over,target_over - keypoints.size(),detectionThreshold,n_last,avg_dif);

		} else {
			detectionThreshold = 0;
			ROS_INFO("Keypoints %ld under %i, missing %4ld, set threshold %.3f ",keypoints.size(),target_over,target_over - keypoints.size(),detectionThreshold);
		}
	}
	detectionThreshold = fmax(detectionThreshold,0);
	setThreshold(detectionThreshold);
}

void keypointCallback(const std_msgs::Int32::ConstPtr& msg)
{
    targetKeypoints = msg->data;
    target_over = targetKeypoints + featureOvershootRatio/100.0 * targetKeypoints;
    ROS_INFO("targetKeypoints set to %i, overshoot: %i",targetKeypoints,target_over);

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
	ros::Subscriber key_sub = nh_.subscribe("/targetKeypoints", 1, keypointCallback);
	image_pub_ = it_.advertise("/image_with_features", 1);


	ros::spin();
	return 0;
}
