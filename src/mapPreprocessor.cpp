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

ros::Publisher cmd_pub_;
ros::Publisher feat_pub_;
ros::Subscriber dist_sub_;
ros::Publisher pathPub;
image_transport::Publisher image_pub_;

/* Action server */
typedef actionlib::SimpleActionServer<stroll_bearnav::loadMapAction> Server;
Server *server;
stroll_bearnav::loadMapResult result;
stroll_bearnav::loadMapFeedback feedback;

/* Feature messages */
stroll_bearnav::FeatureArray featureArray;
stroll_bearnav::Feature feature;

/* map variables */
Mat img,img2;
vector<KeyPoint> keypoints_1,keypoints_2; 
string currentMapName; 
float currentDistance = -1.0; 
Mat descriptors_1,descriptors_2;
Mat currentImage;
string folder;
int numberOfUsedMaps=0;
int lastLoadedMap=0;
float mapDistances[1000];
int numProcessedMaps = 0;
int numMaps = 1;
int numFeatures,numFeatures_2;
float distanceT;
string prefix;
bool stop = false;
float ratioMatchConstant = 0.7;

/*map to be preloaded*/
vector<vector<KeyPoint> > keypointsMap;
vector<Mat> descriptorMap;
vector<float> distanceMap;
vector<string> namesMap;
vector<Mat> imagesMap;


typedef enum
{
	IDLE,
	ACTIVE,
	COMPLETE
}EMapLoaderState;
EMapLoaderState state = IDLE;

/* Loads all maps from a folder 
   returns number of loaded maps  */
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

	/* send feedback to action server */
	feedback.fileName =  "";
	feedback.numberOfMaps = numMaps;
	server->publishFeedback(feedback);

	std::sort(mapDistances, mapDistances + numMaps, std::less<float>());
	mapDistances[numMaps] = mapDistances[numMaps-1];

	/*preload all maps*/
	imagesMap.clear();
	keypointsMap.clear();
	descriptorMap.clear();
	distanceMap.clear();
	namesMap.clear();
	char fileName[1000];
	
	numFeatures=0;
	for (int i = 0;i<numMaps;i++){
		sprintf(fileName,"%s/%s_%.3f.yaml",folder.c_str(),prefix.c_str(),mapDistances[i]); 
		ROS_INFO("Preloading %s/%s_%.3f.yaml",folder.c_str(),prefix.c_str(),mapDistances[i]); 
		FileStorage fs(fileName, FileStorage::READ);
		if(fs.isOpened()){
			img.release();
			descriptors_1.release();
			fs["Keypoints"]>>keypoints_1;
			fs["Descriptors"]>>descriptors_1;
			fs["Image"]>>img;
			fs.release();
			keypointsMap.push_back(keypoints_1);
			descriptorMap.push_back(descriptors_1);
			distanceMap.push_back(mapDistances[i]);
			namesMap.push_back(fileName);
			if (image_pub_.getNumSubscribers()>0) imagesMap.push_back(img);
			numFeatures+=keypoints_1.size();
			sprintf(fileName,"Loading map %i/%i",i+1,numMaps);
			feedback.fileName = fileName;
			feedback.numFeatures=numFeatures;
			feedback.mapIndex=numMaps;
			server->publishFeedback(feedback);
		}

	}

	/* feedback returns name of loaded map, number of features in it and index */
	sprintf(fileName,"%i features loaded from %i maps",numFeatures,numMaps);
	feedback.fileName = fileName;
	feedback.numFeatures=numFeatures;
	feedback.mapIndex=numMaps;
	server->publishFeedback(feedback);
	return numMaps;
}

/* load map two nearest maps based on distance travelled  */
void loadMap(int index,int index2)
{	
	lastLoadedMap=index;	
	keypoints_1 = keypointsMap[index];
	descriptors_1 = descriptorMap[index];
	currentMapName = namesMap[index];
	currentDistance = distanceMap[index];
	numFeatures=keypoints_1.size();
	char fileName[1000];
	sprintf(fileName,"%i features loaded from %ith map at %.3f",numFeatures,index,distanceMap[index]);
	feedback.fileName=fileName;
	feedback.numFeatures=keypoints_1.size();
	feedback.mapIndex=index;
	/* feedback returns name of loaded map, number of features in it and index */
	server->publishFeedback(feedback);
	
	//	lastLoadedMap = index;
	keypoints_2 = keypointsMap[index2];
	descriptors_2 = descriptorMap[index2];
	numFeatures_2=keypoints_2.size();
	char fileName2[1000];
	sprintf(fileName2,"Second map: %i features loaded from %ith map at %.3f",numFeatures_2,index2,distanceMap[index2]);
}

/* load path profile 
   returns size of path profile */
int loadPath()
{	
	char fileName[1000];
	sprintf(fileName,"%s/%s.yaml",folder.c_str(),prefix.c_str()); 
	ROS_DEBUG("Loading %s/%s.yaml",folder.c_str(),prefix.c_str()); 
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

/* Action server */
void executeCB(const stroll_bearnav::loadMapGoalConstPtr &goal, Server *serv)
{
	lastLoadedMap = -1;
	numProcessedMaps = 0;
	stroll_bearnav::loadMapFeedback feedback;
	
	prefix = goal->prefix;
	
	if (loadMaps() > 0 && loadPath() >= 0){
		state = ACTIVE;
	}else{
		result.distance=0;
		result.numFeatures=0;
		result.numMaps=0;
		server->setAborted(result);
	}
	/* server returns distance travelled, total number of features
	   and number of loaded map on preempt request*/
	while(state != IDLE){
		usleep(200000);
		
		if(server->isPreemptRequested()){
			result.distance=distanceT;
			result.numFeatures=numFeatures;
			result.numMaps=numProcessedMaps;
			server->setPreempted(result);
			state = IDLE;
		}
		if(state == COMPLETE){
			result.distance=distanceT;
			result.numFeatures=numFeatures;
			result.numMaps=numProcessedMaps;
			server->setSucceeded(result);
			state = IDLE;
		}
	}
}

/*listens when navigator requests map */
void distCallback(const std_msgs::Float32::ConstPtr& msg)
{	
	if(state == ACTIVE){
		distanceT=msg->data;
		featureArray.feature.clear();

		//find the two closest maps
		int mindex = -1;
		int mindex2= -1;
		float minDistance = FLT_MAX;
		for (int i = 0;i<numMaps;i++){
			if (fabs(distanceT-mapDistances[i]) < minDistance){
				minDistance = fabs(distanceT-mapDistances[i]);
				mindex = i;

				if(mapDistances[mindex]<distanceT) {
					mindex2=mindex+1;	
				}else{ 
					mindex2=mindex-1; 
				}
			}
		}

		ROS_INFO("Current distance is %.3f Closest map found at %i",distanceT,mindex);
		ROS_INFO("Current distance is %.3f 2nd Closest map found at %i",distanceT,mindex2);

			std::vector< DMatch > good_matches;
		if (mindex > -1 && mindex2> -1 && mindex!=lastLoadedMap){
			/*load two closest maps */
			loadMap(mindex,mindex2);

			/*feature matching*/
			Ptr<DescriptorMatcher> matcher = BFMatcher::create(NORM_L2);
			vector< vector<DMatch> > matches;
			matcher->knnMatch(descriptors_1, descriptors_2, matches, 2);

			/*perform ratio matching*/ 
			good_matches.reserve(matches.size());  
			for (size_t i = 0; i < matches.size(); i++)
			{ 
				if (matches[i][0].distance < ratioMatchConstant*matches[i][1].distance) good_matches.push_back(matches[i][0]);
			}
		}	
		int num=good_matches.size();
		vector<Point2f> matched_points1;
		vector<Point2f> matched_points2;

		for (int i=0;i<num;i++){

			int idx2=good_matches[i].trainIdx;
			int idx1=good_matches[i].queryIdx;
			matched_points1.push_back(keypoints_1[idx1].pt);
			matched_points2.push_back(keypoints_2[idx2].pt);

			/* linear interpolation */
			feature.x=matched_points1[i].x+((matched_points2[i].x-matched_points2[i].x)/(distanceMap[mindex2]-distanceMap[mindex]))*(distanceT-distanceMap[mindex]);	
			feature.y=matched_points1[i].y+((matched_points2[i].y-matched_points2[i].y)/(distanceMap[mindex2]-distanceMap[mindex]))*(distanceT-distanceMap[mindex]);	

			feature.size=keypoints_1[idx1].size;
			feature.angle=keypoints_1[idx1].angle;
			feature.response=keypoints_1[idx1].response;
			feature.octave=keypoints_1[idx1].octave;
			feature.class_id=keypoints_1[idx1].class_id;
			feature.descriptor=descriptors_1.row(idx1);
			featureArray.feature.push_back(feature);
		}
		featureArray.distance = distanceT;
		featureArray.id = currentMapName;
		numberOfUsedMaps++;

		/* publish loaded map */
		feat_pub_.publish(featureArray);

		/*if someone listens, then publish loaded image too*/
		if (image_pub_.getNumSubscribers()>0){
			std_msgs::Header header;
			cv_bridge::CvImage bridge(header, sensor_msgs::image_encodings::MONO8, imagesMap[mindex]);
			image_pub_.publish(bridge.toImageMsg());
		}
	}	
}

int main(int argc, char** argv)
{ 
	ros::init(argc, argv, "map_preprocessor");
	ros::NodeHandle nh_;
	image_transport::ImageTransport it_(nh_);
	ros::param::get("~folder", folder);
	cmd_pub_ = nh_.advertise<geometry_msgs::Twist>("/cmd",1);
	pathPub = nh_.advertise<stroll_bearnav::PathProfile>("/pathProfile",1);
	dist_sub_ = nh_.subscribe<std_msgs::Float32>( "/navigator/distance", 1,distCallback);
	image_pub_ = it_.advertise("/map_image", 1);
	feat_pub_ = nh_.advertise<stroll_bearnav::FeatureArray>("/localMap",1);

	/* Initiate action server */
	server = new Server (nh_, "map_preprocessor", boost::bind(&executeCB, _1, server), false);
	server->start();
	ros::spin();
	return 0;
}
