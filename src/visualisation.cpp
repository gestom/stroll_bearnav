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
#include <std_msgs/Float32.h>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/features2d.hpp>
#include <dynamic_reconfigure/server.h>
//#include <stroll_bearnav/visualisationConfig.h>
#include <stroll_bearnav/NavigationInfo.h>
using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;
static const std::string OPENCV_WINDOW = "Image window";


ros::Subscriber currentFeatureSub;
ros::Subscriber mapFeatureSub;
ros::Subscriber navigationInfoSub;
image_transport::Subscriber currentImageSub;
image_transport::Subscriber mapImageSub;

bool showAllMatches=true;
bool showGoodMatches=true;
int imageQueueLength = 10;

/* Image features parameters */
vector<KeyPoint> mapKeypoints, currentKeypoints,keypointsGood,keypointsBest;
Mat currentImage,mapImage;
vector<Mat> imageBuffer;
vector<int> idBuffer;

/* Feature message */
stroll_bearnav::FeatureArray currentFeatures;
stroll_bearnav::FeatureArray mapFeatures;
 

/* dynamic reconfigure of showing images, velocity gain and matching ratio constant */
/*void callback(stroll_bearnav::navigatorConfig &config, uint32_t level)
{
	showAllMatches=config.showAllMatches;
	showGoodMatches=config.showGoodMatches;
}*/

/* reference map received */
void mapFeatureCallback(const stroll_bearnav::FeatureArray::ConstPtr& msg)
{	 
	mapFeatures = *msg;
	ROS_DEBUG("Received a new reference map");
	mapKeypoints.clear();

	KeyPoint keypoint;
	for(int i=0; i<msg->feature.size();i++){
		keypoint.pt.x=msg->feature[i].x;
		keypoint.pt.y=msg->feature[i].y;
		keypoint.size=msg->feature[i].size;
		keypoint.angle=msg->feature[i].angle;
		keypoint.response=msg->feature[i].response;
		keypoint.octave=msg->feature[i].octave;
		keypoint.class_id=msg->feature[i].class_id;
		mapKeypoints.push_back(keypoint);
	}
}

/* get image from camera */
void currentImageCallback(const sensor_msgs::ImageConstPtr& msg)
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
	ROS_INFO("Current image ID %i",msg->header.seq);
	imageBuffer.push_back(cv_ptr->image);
	idBuffer.push_back(msg->header.seq);
	while (imageBuffer.size() > imageQueueLength) imageBuffer.erase(imageBuffer.begin()); 
	while (idBuffer.size() > imageQueueLength) idBuffer.erase(idBuffer.begin()); 
}

/* get image from map */
void mapImageCallback(const sensor_msgs::ImageConstPtr& msg)
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
	ROS_INFO("Current map ID %i",msg->header.seq);
	mapImage=cv_ptr->image;
}

void navigationInfoCallback(const stroll_bearnav::NavigationInfo::ConstPtr& msg)
{
	/*Show good image features (Green) */
	Mat output,outtran;
	/*fill the map features*/
	mapKeypoints.clear();
	KeyPoint keypoint;
	for(int i=0; i<msg->map.feature.size();i++){
		keypoint.pt.x=msg->map.feature[i].x;
		keypoint.pt.y=msg->map.feature[i].y;
		keypoint.size=msg->map.feature[i].size;
		keypoint.angle=msg->map.feature[i].angle;
		keypoint.response=msg->map.feature[i].response;
		keypoint.octave=msg->map.feature[i].octave;
		keypoint.class_id=msg->map.feature[i].class_id;
		mapKeypoints.push_back(keypoint);
	}
 	/*fill the map features*/
	currentKeypoints.clear();
	for(int i=0; i<msg->map.feature.size();i++){
		keypoint.pt.x=msg->view.feature[i].x;
		keypoint.pt.y=msg->view.feature[i].y;
		keypoint.size=msg->view.feature[i].size;
		keypoint.angle=msg->view.feature[i].angle;
		keypoint.response=msg->view.feature[i].response;
		keypoint.octave=msg->view.feature[i].octave;
		keypoint.class_id=msg->view.feature[i].class_id;
		currentKeypoints.push_back(keypoint);
	}

 	/*fill the matches */
	DMatch match;
	std::vector< DMatch > matches;
	std::vector< DMatch > goodMatches;
	for (int i = 0;i<msg->mapMatchIndex.size();i++)
	{
		if(msg->mapMatchIndex[i] >= 0)
		{
			match.queryIdx = i;
			match.trainIdx = msg->mapMatchIndex[i];
			matches.push_back(match);
			if (msg->mapMatchEval[i] == 1) goodMatches.push_back(match); 
		} 
	}
	int id = atoi(&(msg->view.id.c_str())[7]); 
	ROS_INFO("Image ID: %s %i",msg->view.id.c_str(),id);

	/*find the relevant image*/
	std::vector<int>::iterator it;
	it = find (idBuffer.begin(), idBuffer.end(), id);
	if (it != idBuffer.end()){
		int index = it-idBuffer.begin();
		currentImage.release();
		currentImage = imageBuffer[index];
		ROS_INFO("Image position is %i",index);
	}else{
		currentImage.release();
		int index = it-idBuffer.begin();
		ROS_INFO("Image position is %i",index);
		if (index>1) currentImage = imageBuffer[index-1];
	}
	if(showAllMatches || showGoodMatches)
	{
		//drawKeypoints(currentImage,keypointsBest,img_goodKeypoints_1,Scalar(0,255,0), DrawMatchesFlags::DEFAULT );
		if (currentImage.rows >0 && mapKeypoints.size() >0 && currentKeypoints.size() >0)
		{
			if (mapImage.rows==0) mapImage = currentImage;
			Mat mapIm = mapImage.t();  
			Mat curIm = currentImage.t();
			vector<KeyPoint> kpMap,kpCur;
			KeyPoint tmp;
			for (int i = 0;i<mapKeypoints.size();i++)
			{
				tmp = mapKeypoints[i];	
				tmp.pt.y = mapKeypoints[i].pt.x;
				tmp.pt.x = mapKeypoints[i].pt.y;
				kpMap.push_back(tmp);
			} 
			for (int i = 0;i<currentKeypoints.size();i++)
			{
				tmp = currentKeypoints[i];	
				tmp.pt.y = currentKeypoints[i].pt.x;
				tmp.pt.x = currentKeypoints[i].pt.y;
				kpCur.push_back(tmp);
			}
			if (showAllMatches){
				drawMatches(mapIm,kpMap,curIm,kpCur,matches,outtran,Scalar(0,0,255),Scalar(0,0,255),vector<char>(),0);
				if (showGoodMatches) drawMatches(mapIm,kpMap,curIm,kpCur,goodMatches,outtran,Scalar(0,255,0),Scalar(0,255,0),vector<char>(),3);
			}else{
				if (showGoodMatches) drawMatches(mapIm,kpMap,curIm,kpCur,goodMatches,outtran,Scalar(0,255,0),Scalar(0,255,0),vector<char>(),2);
			}
			output = outtran.t();
			std_msgs::Header header;
			cv_bridge::CvImage bridge(header, sensor_msgs::image_encodings::BGR8, output);
			imshow("Visualisation",output);
			waitKey(1);
			//image_pub_.publish(bridge.toImageMsg());
		}
	}


}

int main(int argc, char** argv)
{
	ros::init(argc, argv, "visualisation");

	ros::NodeHandle nh;
	image_transport::ImageTransport it_(nh);
	//image_transport::TransportHints("compressed");
	currentImageSub = it_.subscribe( "/image", 1,currentImageCallback);
	//mapImageSub = it_.subscribe( "/map_image", 1,mapImageCallback);
	//image_pub_ = it_.advertise("/navigationMatches", 1);
	navigationInfoSub = nh.subscribe( "/navigationInfo", 1,navigationInfoCallback);

	/* Initiate dynamic reconfiguration */
//	dynamic_reconfigure::Server<stroll_bearnav::visualisationConfig> server;
//	dynamic_reconfigure::Server<stroll_bearnav::visualisationConfig>::CallbackType f = boost::bind(&callback, _1, _2);
//	server.setCallback(f);

	ros::spin();
	return 0;
}
