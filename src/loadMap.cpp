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
#include <stroll_bearnav/PathProfile.h>
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
ros::Publisher feat_pub_;
ros::Subscriber dist_sub_;
ros::Publisher pathPub;
Mat img,img2;
string folder;
int numberOfUsedMaps=0;
float mapDistances[1000];
int mapIndex = 0;
int numMaps = 1;
bool stop = false;
int numFeatures;
float distanceT;
string prefix;

typedef enum
{
	IDLE,
	ACTIVE,
	COMPLETE
}EMapLoaderState;
EMapLoaderState state = IDLE;

int loadMaps()
{
	numMaps = 0;
	DIR *dir;
	struct dirent *ent;
	cout << folder << endl;
	if ((dir = opendir (folder.c_str())) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) 
		{
			char filter[strlen(prefix.c_str())+10];
			sprintf(filter,"%s_",prefix.c_str());
			if (strstr(ent->d_name,"yaml") != NULL){
				if (strncmp(ent->d_name,filter,strlen(filter)) == 0)  mapDistances[numMaps++] = atof(&ent->d_name[strlen(filter)]);
			}
		}
		closedir (dir);
	} else {
		/* could not open directory */
		ROS_ERROR("Could not open folder %s with maps.",folder.c_str());
	}
	feedback.fileName =  "";
	feedback.numberOfMaps = numMaps;
	server->publishFeedback(feedback);
//	ROS_INFO("There are %i maps present.",numMaps);
	std::sort(mapDistances, mapDistances + numMaps, std::less<float>());
	mapDistances[0] = mapDistances[numMaps] = mapDistances[numMaps-1];
	return numMaps;
}

void loadMap(int index)
{
	char fileName[1000];
	sprintf(fileName,"%s/%s_%.3f.yaml",folder.c_str(),prefix.c_str(),mapDistances[index]); 
	printf("Loading %s/%s_%.3f.yaml\n",folder.c_str(),prefix.c_str(),mapDistances[index]); 
	FileStorage fs(fileName, FileStorage::READ);
	if(fs.isOpened()){
		fs["Keypoints"]>>keypoints_1;
		fs["Descriptors"]>>descriptors_1;
		fs["Image"]>>img;
		fs.release();
		numFeatures+=keypoints_1.size();
		numberOfUsedMaps++;
		feedback.fileName=fileName;
		feedback.numFeatures=keypoints_1.size();
		feedback.mapIndex=index+1;
		server->publishFeedback(feedback);
	}
}

int loadPath()
{	
	char fileName[1000];
	sprintf(fileName,"%s/%s.yaml",folder.c_str(),prefix.c_str()); 
	printf("Loading %s/%s.yaml\n",folder.c_str(),prefix.c_str()); 
	FileStorage fsp(fileName, FileStorage::READ);
	vector<float> path;
	path.clear();
	if(fsp.isOpened()){
		fsp["Path"]>>path;
		fsp.release();
	}
	stroll_bearnav::PathProfile pathProfile;
	for(int i=0;i<path.size()/4;i++){
		pathProfile.distance.push_back(path[4*i+0]);
		pathProfile.forwardSpeed.push_back(path[4*i+1]);
		pathProfile.angularSpeed.push_back(path[4*i+2]);
		pathProfile.flipper.push_back(path[4*i+3]);
	}
	pathPub.publish(pathProfile);
	return path.size();
}

void executeCB(const stroll_bearnav::loadMapGoalConstPtr &goal, Server *serv)
{

	stroll_bearnav::loadMapFeedback feedback;
	prefix = goal->prefix;
	if (loadMaps() > 0 && loadPath() > 0){
			 state = ACTIVE;
	}else{
		result.distance=0;
		result.numFeatures=0;
		result.numMaps=0;
		server->setAborted(result);
	}
	while(state != IDLE){
		usleep(200000);

		if(server->isPreemptRequested()){
			result.distance=distanceT;
			result.numFeatures=numFeatures;
			result.numMaps=mapIndex;
			server->setPreempted(result);
			state = IDLE;
		}
		if(state == COMPLETE){
			result.distance=distanceT;
			result.numFeatures=numFeatures;
			result.numMaps=mapIndex;
			server->setSucceeded(result);
			state = IDLE;
		}
	}
}

void distCallback(const std_msgs::Float32::ConstPtr& msg)
{	
	if(state == ACTIVE){
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
			if (mapIndex+1 >= numMaps) state = COMPLETE; 
		}
	}
}

int main(int argc, char** argv)
{ 
	ros::init(argc, argv, "feature_load");
	ros::NodeHandle nh_;
	ros::param::get("~folder", folder);
	cmd_pub_ = nh_.advertise<geometry_msgs::Twist>("cmd",1);
	pathPub = nh_.advertise<stroll_bearnav::PathProfile>("/load/path",1);
	feat_pub_ = nh_.advertise<stroll_bearnav::FeatureArray>("/load/features",1);
	dist_sub_ = nh_.subscribe<std_msgs::Float32>( "/distance", 1,distCallback);
	server = new Server (nh_, "loader", boost::bind(&executeCB, _1, server), false);
	server->start();
	ros::spin();
	return 0;
}
