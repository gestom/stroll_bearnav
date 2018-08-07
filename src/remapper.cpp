#include <ros/ros.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include "std_msgs/Float32.h"
#include <stroll_bearnav/NavigationInfo.h>
#include <stroll_bearnav/FeatureArray.h>
#include <signal.h>
#include <ros/xmlrpc_manager.h>
#include <ros/callback_queue.h>
#include <opencv2/opencv.hpp>
#include <actionlib/client/simple_action_client.h>
#include <stroll_bearnav/loadMapAction.h>
#include <stroll_bearnav/mapperAction.h>
#include <stroll_bearnav/navigatorAction.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <dynamic_reconfigure/IntParameter.h>
#include <dynamic_reconfigure/Reconfigure.h>
#include <dynamic_reconfigure/Config.h>
#include <std_msgs/Int32.h>

using namespace cv;
using namespace std;
char* fname;

struct MatchInfo{
  char id[300];
  int eval;
  float x;
  float y;
  float size;
  float angle;
  float response;
  int octave;
  uint32_t time;
  string t;
};

int numPrimaryMaps 	= 0;
int primaryMapIndex 	= 0;
int numSecondaryMaps 	= 0;
int secondaryMapIndex 	= 0;
int clientsResponded 	= 0;

float statSumCorrect = 0;
float statSumMatches = 0;
float statSumOutliers = 0;
float statNumMaps = 0;

stroll_bearnav::loadMapGoal mapGoal;
stroll_bearnav::loadMapGoal viewGoal;
stroll_bearnav::mapperGoal mapperGoal;
stroll_bearnav::navigatorGoal navGoal;

FILE *mapFile, *viewFile,*logFile;
string mapFolder,viewFolder;
bool volatile exitting = false;
bool generateDatasets = true;
bool volatile is_working = 0;
ros::CallbackQueue* my_queue;
ros::Publisher dist_pub_;
ros::Publisher distEvent_pub_;
std_msgs::Float32 dist_;
float totalDist = 0;
image_transport::Subscriber mapImageSub;
image_transport::Subscriber viewImageSub;
vector<float> distanceMap;


void mySigHandler(int sig)
{
  my_queue = ros::getGlobalCallbackQueue();
  exitting = true;

  printf("exitting\n");
}


void shutdownCallback(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
{
	int num_params = 0;
	if (params.getType() == XmlRpc::XmlRpcValue::TypeArray)
		num_params = params.size();
	if (num_params > 1)
	{
		exitting = true;

		printf("exitting from second\n");
	}

	result = ros::xmlrpc::responseInt(1, "", 0);
}

void infoMapMatch(const stroll_bearnav::NavigationInfo::ConstPtr& msg)
{
	is_working = 1;
	int size = msg->mapMatchIndex.size();
	float displacementGT = 0;
	vector<MatchInfo> mi;
	
	is_working = 0;
	/*maps processed*/
	if (primaryMapIndex < numPrimaryMaps-1){
		totalDist = distanceMap[primaryMapIndex+1];
		dist_.data=totalDist;
		dist_pub_.publish(dist_);
		distEvent_pub_.publish(dist_);
	}else{
		 exitting = 1;
	}
}

/*Map loader feedback for debugging*/
void feedbackMapCb(const stroll_bearnav::loadMapFeedbackConstPtr& feedback)
{
	distanceMap.push_back(feedback->distance);
	numPrimaryMaps = feedback->numberOfMaps;
	primaryMapIndex = feedback->mapIndex;
	ROS_INFO("Primary map: %s %i %f",feedback->fileName.c_str(),primaryMapIndex,feedback->distance);
}

void feedbackViewCb(const stroll_bearnav::loadMapFeedbackConstPtr& feedback)
{
	numSecondaryMaps = feedback->numberOfMaps;
	secondaryMapIndex = feedback->mapIndex;
	ROS_INFO("Secondary map: %s",feedback->fileName.c_str());
}

void doneMapCb(const actionlib::SimpleClientGoalState& state,const stroll_bearnav::loadMapResultConstPtr& result)
{ 
	ROS_INFO("Primary map client reports %s: Map covers %.3f meters and contains %i features in %i submaps.", state.toString().c_str(),result->distance,result->numFeatures,result->numMaps);
	clientsResponded++;
}

void doneViewCb(const actionlib::SimpleClientGoalState& state,const stroll_bearnav::loadMapResultConstPtr& result)
{
	ROS_INFO("Secondary map client reports %s: Map covers %.3f meters and contains %i features in %i submaps.", state.toString().c_str(),result->distance,result->numFeatures,result->numMaps);
	clientsResponded++;
}

void activeCb()
{
	clientsResponded++;
}

void doneMapperCb(const actionlib::SimpleClientGoalState& state,const stroll_bearnav::mapperResultConstPtr& result)
{ 
	ROS_INFO("Navigator client reports %s.",state.toString().c_str());
	clientsResponded++;
}

void feedbackMapperCb(const stroll_bearnav::mapperFeedbackConstPtr& feedback)
{
	ROS_INFO("MAPPER report: %s",feedback->fileName.c_str());
}

void doneNavCb(const actionlib::SimpleClientGoalState& state,const stroll_bearnav::navigatorResultConstPtr& result)
{ 
	ROS_INFO("Navigator client reports %s.",state.toString().c_str());
	clientsResponded++;
}

void feedbackNavCb(const stroll_bearnav::navigatorFeedbackConstPtr& feedback)
{
	int offsetMap = 0;
	int offsetView = 0;
	int dummy = 0;
	int mapA = 0;
	int mapB = 0;
	//fscanf(mapFile, "%i %i\n",&offsetMap,&dummy);
	//fscanf(viewFile,"%i %i\n",&offsetView,&dummy);
	float displacementGT = offsetView - offsetMap;

	ROS_INFO("Navigation reports %i correct matches and %i outliers out of %i matches at distance %.3f with maps %s %s. Displacement %.3f GT %.3f",feedback->correct,feedback->outliers,feedback->matches,feedback->distance,mapGoal.prefix.c_str(),viewGoal.prefix.c_str(),feedback->diffRot,displacementGT);
	fprintf(logFile,"Navigation reports %i correct matches and %i outliers out of %i matches at distance %.3f with maps %s %s. Displacement %.3f GT %.3f\n",feedback->correct,feedback->outliers,feedback->matches,feedback->distance,mapGoal.prefix.c_str(),viewGoal.prefix.c_str(),feedback->diffRot,displacementGT);
	statSumCorrect += feedback->correct;
	statSumMatches += feedback->matches;
	statSumOutliers += feedback->outliers;
	statNumMaps++;
}

void mapImageCallback(const sensor_msgs::ImageConstPtr& msg)
{
	static int mapImageNum = 0;
	cv_bridge::CvImagePtr cv_ptr;
	cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
	//char fileName[1000];
	//sprintf(fileName,"%s/%09i.bmp",mapFolder.c_str(),mapImageNum++);	
	//imwrite(fileName,cv_ptr->image);
}

void viewImageCallback(const sensor_msgs::ImageConstPtr& msg)
{
	static int viewImageNum = 0;
	cv_bridge::CvImagePtr cv_ptr;
	cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
	//char fileName[1000];
	//sprintf(fileName,"%s/%09i.bmp",viewFolder.c_str(),viewImageNum++);
	//imwrite(fileName,cv_ptr->image);
}

int configureFeatures(int detector,int descriptor)
{
	dynamic_reconfigure::ReconfigureRequest srv_req;
	dynamic_reconfigure::ReconfigureResponse srv_resp;
	dynamic_reconfigure::IntParameter param;
	dynamic_reconfigure::Config conf;

	param.name = "detector";
	param.value = detector;
	conf.ints.push_back(param);
	param.name = "descriptor";
	param.value = descriptor;
	conf.ints.push_back(param);
	srv_req.config = conf;

	if (ros::service::call("/feature_extraction/set_parameters", srv_req, srv_resp) == false) ROS_WARN("Feature extraction module not configured.");
}


int main(int argc, char **argv)
{
	if(argc<2){
		perror("You must enter file Name");
		fname = "bla";
	}else{
		fname = argv[1];
	}
	signal(SIGINT, mySigHandler);
	ros::init(argc, argv, "listener", ros::init_options::NoSigintHandler);

	ros::XMLRPCManager::instance()->unbind("shutdown");
	ros::XMLRPCManager::instance()->bind("shutdown", shutdownCallback);

	ros::NodeHandle n;
	ros::param::get("~folder_view", viewFolder);
	ros::param::get("~folder_map", mapFolder);

	/*char filename[1000];
	char mode[] = "r";
	if (generateDatasets) mode[0] = 'w';
	sprintf(filename,"%s/displacements.txt",mapFolder.c_str());
	mapFile = fopen(filename,mode);
	sprintf(filename,"%s/displacements.txt",viewFolder.c_str());
	viewFile = fopen(filename,mode);*/
	logFile = fopen("Results.txt","w");

	configureFeatures(3,2);
	image_transport::ImageTransport it(n);

	distEvent_pub_=n.advertise<std_msgs::Float32>("/distance_events",1);
	ros::Subscriber sub = n.subscribe("/navigationInfo", 1000, infoMapMatch);
	dist_pub_=n.advertise<std_msgs::Float32>("/distance",1);

	if (generateDatasets){
		viewImageSub = it.subscribe( "/image_view", 1,viewImageCallback);
		mapImageSub = it.subscribe( "/map_image", 1,mapImageCallback);
	}

	actionlib::SimpleActionClient<stroll_bearnav::loadMapAction> mp_view("map_preprocessor_view", true);
	actionlib::SimpleActionClient<stroll_bearnav::loadMapAction> mp_map("map_preprocessor_map", true);
	actionlib::SimpleActionClient<stroll_bearnav::mapperAction> mapper("mapper", true);
	actionlib::SimpleActionClient<stroll_bearnav::navigatorAction> nav("navigator", true);
	mp_map.waitForServer(); 
	ROS_INFO("Primary map server responding");
	mp_view.waitForServer(); 
	ROS_INFO("Secondary map server responding");
	mapper.waitForServer(); 
	ROS_INFO("Mapping server responding");
	nav.waitForServer(); 
	ROS_INFO("Navigator server responding");


	bool finished_before_timeout = true;

	//const char *viewNames[] = {"P1","P2","P3","P4","P5","P6","P7","P8","P9","P10","P11","P12","P13","P14","P15","P16","P17"};
	const char *viewNames[] = {"A1","A2","A3","A4","A5","A6","A7","A8","A9","A10","A11","A12","A13","A14","A15","A16"};
	const char *mapNames[] = {"E0","E1","E2","E3","E4","E5","E6","E7","E8","E9","E10","E11","E12","E13","E14","E15","E16"};

	//const char *viewNames[] = {"X1","X2","X3","X4","X5","X6","X7","X8","X9","X10","X11","X12","X13","X14","X15","X16","X17"};
	//const char *mapNames[] = {"A0","Y1","Y2","Y3","Y4","Y5","Y6","Y7","Y8","Y9","Y10","Y11","Y12","Y13","Y14","Y15","Y16"};
	int numGlobalMaps = 9;
	for (int globalMapIndex = 0;globalMapIndex<numGlobalMaps;globalMapIndex++)
	{
		/*set map and view info */
		clientsResponded = 0;
		navGoal.traversals = 1;

		viewGoal.prefix = viewNames[globalMapIndex];
		mapGoal.prefix = mapNames[globalMapIndex];
		mapperGoal.fileName = mapNames[globalMapIndex+1];
		mp_map.sendGoal(mapGoal,&doneMapCb,&activeCb,&feedbackMapCb);
		mp_view.sendGoal(viewGoal,&doneViewCb,&activeCb,&feedbackViewCb);
		mapper.sendGoal(mapperGoal,&doneMapperCb,&activeCb,&feedbackMapperCb);

		/*wait for maps to load*/
		while (clientsResponded < 3) sleep(1);

		while (primaryMapIndex != numPrimaryMaps)
		{
			sleep(1);
			ROS_INFO("Waiting for primary map load %i of %i.",primaryMapIndex,numPrimaryMaps);
		}
		while (secondaryMapIndex != numSecondaryMaps)
		{
			sleep(1);
			ROS_INFO("Waiting for secondary map load %i of %i.",secondaryMapIndex,numSecondaryMaps);
		}

		/*initiate navigation*/
		ROS_INFO("Goals send");
		nav.sendGoal(navGoal,&doneNavCb,&activeCb,&feedbackNavCb);
		while (clientsResponded < 3) sleep(1);

		/*send first odometry info*/
		is_working = 0;
		totalDist = 0.0;
		dist_.data=totalDist;
		dist_pub_.publish(dist_);
		distEvent_pub_.publish(dist_);

		/*perform navigation*/
		while(ros::ok && !exitting)
		{
			if(exitting && !is_working){
				return 0;
			}
			ros::spinOnce();
			usleep(10000);
		}

		/*terminate navigation, unload maps*/
		exitting = 0;
		clientsResponded = 0;
		nav.cancelGoal();
		mapper.cancelGoal();
		mp_view.cancelGoal();
		mp_map.cancelGoal();
		while (clientsResponded < 4) sleep(1);
	
		/*Flush statistics*/
		ROS_INFO("Map test %s %s summary: %.3f %.3f %.3f",mapGoal.prefix.c_str(),viewGoal.prefix.c_str(),statSumMatches/statNumMaps,statSumCorrect/statNumMaps,statSumOutliers/statNumMaps);
		fprintf(logFile,"Map test %s %s summary: %.3f %.3f %.3f\n",mapGoal.prefix.c_str(),viewGoal.prefix.c_str(),statSumMatches/statNumMaps,statSumCorrect/statNumMaps,statSumOutliers/statNumMaps);
		statSumCorrect = statSumMatches = statSumOutliers = statNumMaps = 0;
	}
	fclose(logFile);
	return 0;
}
