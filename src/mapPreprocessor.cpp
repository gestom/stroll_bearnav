#include <ros/ros.h>
#include "strategies/CStrategy.h"
#include "t_models/CTemporal.h"
#include "std_msgs/Float32.h"
#include "std_msgs/String.h"
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
#include <time.h>
#include <string>
#include <stroll_bearnav/listenerConfig.h>
#include <ros/callback_queue.h>
#include <dynamic_reconfigure/server.h>


using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;
static const std::string OPENCV_WINDOW = "Image window";

ros::Publisher cmd_pub_;
ros::Publisher feat_pub_;
ros::Publisher dist_view_pub_;
ros::Subscriber dist_sub_;
std_msgs::Float32 dist_;
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
vector<float> ratings;
float stc_strategy_param=100;
float stc_model_param = 0;
string stc_model_type = "Sum";
string stc_strategy_type = "Best";
Mat img,img2;
vector<KeyPoint> keypoints_1,keypoints_2;
string currentMapName;
float currentDistance = -1.0;
Mat descriptors_1,descriptors_2;
Mat currentImage;
string folder;
int numberOfUsedMaps=0;
int lastLoadedMap=0;
float mapDistances[10000];
int numProcessedMaps = 0;
int numMaps = 1;
int numFeatures;
float distanceT;
string prefix;
bool stop = false;
std::vector<CTemporal*> models;
string stc_fname;
string tmp_param="shit";
bool statistics = false;
int f_index = 0;
int last_size = 0;
uint32_t t = time(NULL);

/*map to be preloaded*/
vector<vector<KeyPoint> > keypointsMap;
vector<Mat> descriptorMap;
vector<float> distanceMap;
vector<string> namesMap;
vector<Mat> imagesMap;
vector<vector<float> > ratingsMap;


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
	//server->publishFeedback(feedback);

	std::sort(mapDistances, mapDistances + numMaps, std::less<float>());
	mapDistances[numMaps] = mapDistances[numMaps-1];

	/*preload all maps*/
	imagesMap.clear();
	keypointsMap.clear();
	descriptorMap.clear();
	distanceMap.clear();
	namesMap.clear();
	ratingsMap.clear();
	char fileName[1000];

	numFeatures=0;
	for (int i = 0;i<numMaps;i++){
		sprintf(fileName,"%s/%s_%.3f.yaml",folder.c_str(),prefix.c_str(),mapDistances[i]);
		ROS_INFO("Preloading %s/%s_%.3f.yaml",folder.c_str(),prefix.c_str(),mapDistances[i]);
		FileStorage fs(fileName, FileStorage::READ);
		if(fs.isOpened())
		{
			img.release();
			descriptors_1.release();
			fs["Keypoints"]  >> keypoints_1;
			fs["Descriptors"]>>descriptors_1;
			fs["Image"]>>img;
			ratings.clear();
			fs["Ratings"]>>ratings;
			for (int j = ratings.size(); j < keypoints_1.size(); j++) ratings.push_back(0);
			fs.release();
			keypointsMap.push_back(keypoints_1);
			descriptorMap.push_back(descriptors_1);
			distanceMap.push_back(mapDistances[i]);
			namesMap.push_back(fileName);
			ratingsMap.push_back(ratings);
			if (image_pub_.getNumSubscribers()>0) imagesMap.push_back(img);
			numFeatures+=keypoints_1.size();
			sprintf(fileName,"Loading map %i/%i",i+1,numMaps);
			feedback.fileName = fileName;
			feedback.distance = mapDistances[i];
			feedback.numFeatures=numFeatures;
			feedback.mapIndex=i;
			feedback.mapLoaded=false;
			server->publishFeedback(feedback);
		}

	}

	/* feedback returns name of loaded map, number of features in it and index */
	sprintf(fileName,"%i features loaded from %i maps",numFeatures,numMaps);
	feedback.fileName = fileName;
	feedback.distance = mapDistances[numMaps-1];
	feedback.numFeatures=numFeatures;
	feedback.mapIndex=numMaps;
	feedback.mapLoaded=true;
	server->publishFeedback(feedback);
	return numMaps;
}

void callback(stroll_bearnav::listenerConfig &config, uint32_t level)
{
	t=config.currentTime;
  ROS_ERROR("mapPreprocessor setting time to %u ",t);
	f_index = 0;
}

/* load map based on distance travelled  */
void loadMap(int index)
{
	lastLoadedMap = index;
	keypoints_1 = keypointsMap[index];
	descriptors_1 = descriptorMap[index];
	currentMapName = namesMap[index];
	currentDistance = distanceMap[index];
	ratings = ratingsMap[index];
	numFeatures=keypoints_1.size();
	char fileName[1000];
	sprintf(fileName,"%i features loaded from %ith map at %.3f",numFeatures,index,distanceMap[index]);
	feedback.fileName=fileName;
	feedback.distance = currentDistance;
	feedback.numFeatures=keypoints_1.size();
	feedback.mapIndex=index;
	feedback.mapLoaded=true;
	/* feedback returns name of loaded map, number of features in it and index */
	server->publishFeedback(feedback);
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



void distCallback(const std_msgs::Float32::ConstPtr& msg)
{
	if(state == ACTIVE){
		distanceT=msg->data;
		featureArray.feature.clear();

		//find the closest map
		int mindex = -1;
		float minDistance = FLT_MAX;
		for (int i = 0;i<numMaps;i++){
			if (fabs(distanceT-mapDistances[i]) < minDistance){
				minDistance = fabs(distanceT-mapDistances[i]);
				mindex = i;
			}
		}

		//and publish it
		int j = 0;
		if (mindex > -1 && mindex != lastLoadedMap){
			ROS_INFO("Current distance is %.3f Closest map found at %i, last was %i",distanceT,mindex,lastLoadedMap);
			loadMap(mindex);
//			ROS_INFO("Sending a map %i features with %i descriptors",(int)keypoints_1.size(),descriptors_1.rows);
			bool with_stcs = false;
			int size = keypoints_1.size();
			int len =max(size,1);
			double stcs[len];
			if(keypoints_1.size()>0 && statistics)
			{
				ROS_INFO("Index = %d time = %u",f_index,t);

				ifstream f(stc_fname.c_str());
				if (f.is_open())
				{
					string type;
					bool map_models_found = false;
					vector<double> scores;
					scores.clear();
					for (size_t i = 0; i < keypoints_1.size(); i++) {
						scores.push_back(0);
					}

					string f_id = to_string(0) + "_" + currentMapName;
					while(f_index<models.size() && !map_models_found){
						if(f_id.compare(models[f_index]->fid)==0){
							map_models_found = true;
							ROS_WARN("yes f_index =%d fid = %s mid = %s",f_index,f_id.c_str(),models[f_index]->fid.c_str());
						}else{
							f_index += last_size;
							continue;
						}
						for(int j = 0; j<keypoints_1.size() && map_models_found;j++){
							CTemporal* model = models[f_index+j];
							model->update(stc_model_param);
							scores[j] = model->predict(t);
						}

					}
					if(!map_models_found){
						string line;
						bool id_found = false;
						CTemporal* model;
						int i = 0;
						int l_index =-1;


						while ( getline (f,line))
						{
							l_index++;
							if(f_index > l_index){
								continue;
							}
							if(f_index+keypoints_1.size()<=l_index){
								break;
							}

							istringstream l(line);
							string s;
							string id = to_string(i) + "_" + currentMapName;
							if(getline(l, s, ' ')){
								id_found = id.compare(s)==0;
							}
							if(!id_found){
								continue;
							}
							models.push_back(spawnTemporalModel(stc_model_type.c_str(), id, stc_model_param));
							model = models[f_index + i];
							for(int j = 0; j<6;j++){
								getline(l, s, ' ');
							}
							while (getline(l, s, ' '))
							{
								uint32_t t = atoi(s.c_str());

								getline(l, s, ' ');
								float state = (float)atoi(s.c_str());
								model->add(t,state);
							}
							id_found = false;
							model->update(stc_model_param);
							model->print(true);
							double score = model->predict(t);
							scores[i] = score;
							i++;
						}

					}


					last_size = keypoints_1.size();

					f.close();

					Mat tmp_mat = descriptors_1.clone();
					descriptors_1.release();

					vector<KeyPoint> tmp(keypoints_1);
					keypoints_1.clear();

					// ROS_ERROR("ARGUMENT %f",stc_strategy_param);

					// ROS_ERROR("key size: %lu score size %lu\n",keypoints_1.size(),scores.size());
					CStrategy* strategy = spawnStrategy(stc_strategy_type.c_str(),stc_strategy_param);
					// ROS_ERROR("size before %lu model %s param %f stategy %s param %f",tmp.size(),stc_model_type.c_str(),stc_model_param,stc_strategy_type.c_str(),stc_strategy_param);
					strategy->filterFeatures(&keypoints_1,&descriptors_1,&tmp,&tmp_mat, scores);

					ROS_ERROR("size after %lu",keypoints_1.size());
				}
			}

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
				feature.rating=ratings[i];
				featureArray.feature.push_back(feature);
			}
			featureArray.distance = currentDistance;
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
		dist_.data=distanceT;
		dist_view_pub_.publish(dist_);
	}
}

int main(int argc, char** argv)
{
	ros::init(argc, argv, "map_preprocessor");
	ros::NodeHandle nh_;
	image_transport::ImageTransport it_(nh_);
	ros::param::get("~folder", folder);
	if(ros::param::get("~stc_file", stc_fname)){
		ros::param::get("~stc_file", stc_fname);
		ifstream f( stc_fname.c_str());
		statistics = f.good();
		ros::param::get("~stc_model_type", stc_model_type);
		ros::param::get("~stc_strategy_type", stc_strategy_type);
		ros::param::get("~stc_model_param", stc_model_param);
		ros::param::get("~stc_strategy_param", stc_strategy_param);
	}
	cmd_pub_ = nh_.advertise<geometry_msgs::Twist>("/cmd",1);
	pathPub = nh_.advertise<stroll_bearnav::PathProfile>("/pathProfile",1);
	dist_view_pub_=nh_.advertise<std_msgs::Float32>("/distance_done",1);


	dist_sub_ = nh_.subscribe<std_msgs::Float32>( "/distance", 1,distCallback);

	image_pub_ = it_.advertise("/map_image", 1);
	feat_pub_ = nh_.advertise<stroll_bearnav::FeatureArray>("/localMap",1);

	/* Initiate action server */
	dynamic_reconfigure::Server<stroll_bearnav::listenerConfig> server2;
	dynamic_reconfigure::Server<stroll_bearnav::listenerConfig>::CallbackType clb = boost::bind(&callback, _1, _2);
	server2.setCallback(clb);

	server = new Server (nh_, "map_preprocessor", boost::bind(&executeCB, _1, server), false);
	server->start();
	ros::spin();
	return 0;
}
