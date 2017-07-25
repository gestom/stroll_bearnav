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
#include <std_msgs/Float32.h>
#include <cmath>
#include <dirent.h>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/features2d.hpp>
using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;
static const std::string OPENCV_WINDOW = "Image window";

vector<KeyPoint> keypoints_1,keypoints_2; 
Mat descriptors_1,descriptors_2;

stroll_bearnav::FeatureArray featureArray;
stroll_bearnav::Feature feature;
ros::Publisher feat_pub_;
ros::Subscriber dist_sub_;
Mat img,img2;
string folder;

float mapDistances[1000];
int mapIndex = 0;
int numMaps = 1;
bool stop = false;

void loadMaps(string folder)
{
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (folder.c_str())) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) 
		{
			if (strstr(ent->d_name,"yaml") != NULL) mapDistances[numMaps++] = atof(ent->d_name);
		}
		closedir (dir);
	} else {
		/* could not open directory */
		perror ("");
	}
	std::sort(mapDistances, mapDistances + numMaps, std::less<float>());
	mapDistances[0] = mapDistances[numMaps] = mapDistances[numMaps-1];
}

void loadMap(int index)
{
	char fileName[1000];
	sprintf(fileName,"%s/%.3f.yaml",folder.c_str(),mapDistances[index]); 
	printf("Loading %s/%.3f.yaml\n",folder.c_str(),mapDistances[index]); 
	FileStorage fs(fileName, FileStorage::READ);
	if(fs.isOpened()){
		fs["Keypoints"]>>keypoints_1;
		fs["Descriptors"]>>descriptors_1;
		fs["Image"]>>img;

		/*:if(keypoints_2.size() > 0 && descriptors_2.rows > 0 && keypoints_2.size() == descriptors_2.rows && img2.rows>0){
			keypoints_1=keypoints_2;
			descriptors_1=descriptors_2;
			img=img2.clone();
		}*/
		fs.release();
	}
}

void distCallback(const std_msgs::Float32::ConstPtr& msg)
{
	float distance=msg->data;
	featureArray.feature.clear();
	//if the next map is closer than the current one
	if (fabs(distance-mapDistances[mapIndex]) > fabs(distance-mapDistances[mapIndex+1]))
	{
		mapIndex++;
		loadMap(mapIndex);
		for(int i=0;i<keypoints_1.size();i++)
		{
			feature.x=keypoints_1[i].pt.x;
			feature.y=keypoints_1[i].pt.y;
			feature.size=keypoints_1[i].size;
			feature.angle=keypoints_1[i].angle;
			feature.response=keypoints_1[i].response;
			feature.octave=keypoints_1[i].octave;
			feature.class_id=keypoints_1[i].class_id;
			feature.descriptor=descriptors_1.row(i);
			featureArray.feature.push_back(feature);
		}
		feat_pub_.publish(featureArray);
	}
}

int main(int argc, char** argv)
{ 
  loadMaps(folder);
  ros::init(argc, argv, "feature_load");
  ros::NodeHandle nh_;
  ros::param::get("~folder", folder);

  feat_pub_ = nh_.advertise<stroll_bearnav::FeatureArray>("/load/features",1);
  dist_sub_ = nh_.subscribe<std_msgs::Float32>( "/distance", 1,distCallback);
  ros::spin();
  return 0;
}
