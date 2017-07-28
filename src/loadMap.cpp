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
#include <stroll_bearnav/Speed.h>
#include <std_msgs/Float32.h>
#include <geometry_msgs/Twist.h>
#include <cmath>
#include <dirent.h>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/features2d.hpp>
#include <actionlib/server/simple_action_server.h>
#include <stroll_bearnav/loadMapAction.h>
using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;
static const std::string OPENCV_WINDOW = "Image window";

vector<KeyPoint> keypoints_1,keypoints_2; 
Mat descriptors_1,descriptors_2;
ros::Publisher cmd_pub_;
typedef actionlib::SimpleActionServer<stroll_bearnav::loadMapAction> Server;
Server *server;
geometry_msgs::Twist twist;
stroll_bearnav::loadMapResult result;
stroll_bearnav::loadMapFeedback feedback;
stroll_bearnav::FeatureArray featureArray;
stroll_bearnav::Feature feature;
stroll_bearnav::Speed speed;
ros::Publisher feat_pub_;
ros::Subscriber dist_sub_;
ros::Publisher speed_pub_;
Mat img,img2;
string folder;
int numberOfUsedMaps=0;
bool isRunning=true;
float mapDistances[1000];
int mapIndex = 0;
int numMaps = 1;
bool stop = false;
int numFeatures;
float distanceT;
vector<float> path;
void loadMaps(string folder)
{
	DIR *dir;
	struct dirent *ent;
	cout << folder << endl;
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
		numFeatures+=keypoints_1.size();
		fs.release();
		feedback.fileName=fileName;
		server->publishFeedback(feedback);
		numberOfUsedMaps++;
	}
}
void loadPath(string folder)
{	
	char fileName[1000];
	sprintf(fileName,"%s.yaml",folder.c_str()); 
	printf("Loading %s.yaml\n",folder.c_str()); 
	FileStorage fsp(fileName, FileStorage::READ);
	path.clear();
	if(fsp.isOpened()){
		fsp["Path"]>>path;
		fsp.release();
	}
	for(int i=0;i<path.size();i++){
	speed.velocity.push_back(path[i]);
	}
	speed_pub_.publish(speed);

}
void executeCB(const stroll_bearnav::loadMapGoalConstPtr &goal, Server *serv)
{
	isRunning = true;
  
    while(isRunning == true){
   		usleep(200000);
    	
		if(server->isPreemptRequested()){
     		isRunning = false;
	 		result.numberOfMaps=numberOfUsedMaps;
		 	result.distance=distanceT;
		 	result.features=numFeatures;
	
	        server->setPreempted(result);
	
			while(true){
			twist.linear.x = twist.linear.y = twist.linear.z = 0.0;
		 	twist.angular.y = twist.angular.x = 0.0;	
	 	    cmd_pub_.publish(twist);
   			}
		}
  	}
  
}

void distCallback(const std_msgs::Float32::ConstPtr& msg)
{	
	if(isRunning){
		distanceT=msg->data;
		featureArray.feature.clear();
	//if the next map is closer than the current one
		if (fabs(distanceT-mapDistances[mapIndex]) > fabs(distanceT-mapDistances[mapIndex+1]))
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
}

int main(int argc, char** argv)
{ 
	ros::init(argc, argv, "feature_load");
	ros::NodeHandle nh_;
	ros::param::get("~folder", folder);
	loadMaps(folder);
	loadPath(folder);
	cmd_pub_ = nh_.advertise<geometry_msgs::Twist>("cmd",1);
	speed_pub_ = nh_.advertise<stroll_bearnav::Speed>("speed/data",1);
	feat_pub_ = nh_.advertise<stroll_bearnav::FeatureArray>("/load/features",1);
	dist_sub_ = nh_.subscribe<std_msgs::Float32>( "/distance", 1,distCallback);
	server = new Server (nh_, "mapping", boost::bind(&executeCB, _1, server), false);
	server->start();
	ros::spin();
	return 0;
}
