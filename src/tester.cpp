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
#include <stroll_bearnav/navigatorAction.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>

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

FILE *mapFile, *viewFile;
string mapFolder,viewFolder;
bool volatile exitting = false;
bool generateDatasets = false;
bool volatile is_working = 0;
ros::CallbackQueue* my_queue;
ros::Publisher dist_pub_;
std_msgs::Float32 dist_;
float totalDist = 0;
image_transport::Subscriber mapImageSub;
image_transport::Subscriber viewImageSub;

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
	if(exitting){
		return;
	}
	is_working = 1;
	int size = msg->mapMatchIndex.size();
	float displacementGT = 0;
	vector<MatchInfo> mi;
	if (generateDatasets){
		fprintf(mapFile,"%i %i\n",0,0);
		fprintf(viewFile,"%i %i\n",(int)msg->displacement,0);
	}else{
		int offsetMap = 0;
		int offsetView = 0;
		int dummy = 0;
		fscanf(mapFile,"%i %i\n",&offsetMap,&dummy);
		fscanf(viewFile,"%i %i\n",&offsetView,&dummy);
		displacementGT = offsetView - offsetMap;
	}
	fprintf(stdout,"DISPLACEMENT: %.3f %.3f\n",displacementGT,msg->displacement);
	if(size>0)
	{
		for(int i = 0; i<size;i++)
		{
			MatchInfo new_mi;

			sprintf(new_mi.id,"%d",i);
			strcat(new_mi.id, "_");
			strcat(new_mi.id,msg->map.id.c_str());
			strcat(new_mi.id, "\0");

			new_mi.eval = msg->mapMatchEval[i];
			new_mi.x = msg->map.feature[i].x;
			new_mi.y = msg->map.feature[i].y;
			new_mi.size = msg->map.feature[i].size;
			new_mi.angle = msg->map.feature[i].angle;
			new_mi.response = msg->map.feature[i].response;
			new_mi.octave = msg->map.feature[i].octave;
			// new_mi.time = msg->view.header.stamp.sec;
			new_mi.time = time(NULL);
			mi.push_back(new_mi);
		}

		ofstream file_content(".statistics.txt");
		//ostringstream file_content;
		string line;
		ifstream f (fname);

		if (f.is_open() && file_content.is_open())
		{
			while ( getline (f,line) )
			{
				vector<string> strings;
				string line2;
				line2.assign(line);
				istringstream l(line2);
				string s;
				int i = 0;
				int j = 0;
				ostringstream end_line;
				end_line<<endl;
				while (getline(l, s, ' ') && i == 0)
				{

					for(j = 0;j<mi.size();j++)
					{
						if(s.compare(mi[j].id) == 0){
							end_line.str("");
							end_line.clear();
							end_line << " " << mi[j].time << " " <<  mi[j].eval << endl;
							mi.erase(mi.begin() +j );
							break;
						}
					}
					file_content << line;
					file_content << end_line.str();
					i++;
				}
			}
			f.close();
		}

		if(mi.size()>0)
		{
			for(int i = 0; i<mi.size(); i++)
			{
				file_content << mi[i].id << " ";
				file_content << mi[i].x << " ";
				file_content << mi[i].y << " ";
				file_content << mi[i].size << " ";
				file_content << mi[i].angle << " ";
				file_content << mi[i].response << " ";
				file_content << mi[i].octave << " ";
				file_content << mi[i].time << " ";
				file_content << mi[i].eval << endl;
			}
		}
		mi.clear();
		file_content.close();
		if( remove( fname ))
		{
			perror( "Error deleting file" );
		}

		char oldname[] =".statistics.txt";

		if ( rename( oldname , fname ) )
		{
			perror( "Error renaming file" );
		}
	}
	is_working = 0;
	totalDist += 0.2;
	dist_.data=totalDist;
	dist_pub_.publish(dist_);
}

int numPrimaryMaps 	= 0;
int primaryMapIndex 	= 0;
int numSecondaryMaps 	= 0;
int secondaryMapIndex 	= 0;
int mapsResponded 	= 0;

/*Map loader feedback for debugging*/
void feedbackMapCb(const stroll_bearnav::loadMapFeedbackConstPtr& feedback)
{
	numPrimaryMaps = feedback->numberOfMaps;
	primaryMapIndex = feedback->mapIndex;
	//	ROS_INFO("Primary map: %s",feedback->fileName.c_str());
}

void feedbackViewCb(const stroll_bearnav::loadMapFeedbackConstPtr& feedback)
{
	numSecondaryMaps = feedback->numberOfMaps;
	secondaryMapIndex = feedback->mapIndex;
	//	ROS_INFO("Secondary map: %s",feedback->fileName.c_str());
}

void doneMapCb(const actionlib::SimpleClientGoalState& state,const stroll_bearnav::loadMapResultConstPtr& result)
{ 
	ROS_INFO("Primary map client reports %s: Map covers %.3f meters and contains %i features in %i submaps.", state.toString().c_str(),result->distance,result->numFeatures,result->numMaps);
}

void doneViewCb(const actionlib::SimpleClientGoalState& state,const stroll_bearnav::loadMapResultConstPtr& result)
{
	ROS_INFO("Secondary map client reports %s: Map covers %.3f meters and contains %i features in %i submaps.", state.toString().c_str(),result->distance,result->numFeatures,result->numMaps);
}

void activeCb()
{
	mapsResponded++;
}

void mapImageCallback(const sensor_msgs::ImageConstPtr& msg)
{
	static int mapImageNum = 0;
	cv_bridge::CvImagePtr cv_ptr;
	cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
	char fileName[1000];
	sprintf(fileName,"%s/%09i.bmp",mapFolder.c_str(),mapImageNum++);	
	imwrite(fileName,cv_ptr->image);
}

void viewImageCallback(const sensor_msgs::ImageConstPtr& msg)
{
	static int viewImageNum = 0;
	cv_bridge::CvImagePtr cv_ptr;
	cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
	char fileName[1000];
	sprintf(fileName,"%s/%09i.bmp",viewFolder.c_str(),viewImageNum++);
	imwrite(fileName,cv_ptr->image);
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

	char filename[1000];
	char mode[] = "r";
	if (generateDatasets) mode[0] = 'w';
	sprintf(filename,"%s/displacements.txt",mapFolder.c_str());
	mapFile = fopen(filename,mode);
	sprintf(filename,"%s/displacements.txt",viewFolder.c_str());
	viewFile = fopen(filename,mode);


	image_transport::ImageTransport it(n);

	ros::Subscriber sub = n.subscribe("/navigationInfo", 1000, infoMapMatch);
	dist_pub_=n.advertise<std_msgs::Float32>("/distance",1);

	if (generateDatasets){
		viewImageSub = it.subscribe( "/image_view", 1,viewImageCallback);
		mapImageSub = it.subscribe( "/map_image", 1,mapImageCallback);
	}
	actionlib::SimpleActionClient<stroll_bearnav::loadMapAction> mp_view("map_preprocessor_view", true);
	actionlib::SimpleActionClient<stroll_bearnav::loadMapAction> mp_map("map_preprocessor_map", true);
	actionlib::SimpleActionClient<stroll_bearnav::navigatorAction> nav("navigator", true);
	mp_map.waitForServer(); 
	ROS_INFO("Primary map server responding");
	mp_view.waitForServer(); 
	ROS_INFO("Secondary map server responding");
	nav.waitForServer(); 
	ROS_INFO("Navigator server responding");

	bool finished_before_timeout = true;

	stroll_bearnav::loadMapGoal mapGoal;
	stroll_bearnav::navigatorGoal navGoal;
	mapGoal.prefix = "B";
	navGoal.traversals = 1;

	mp_view.sendGoal(mapGoal,&doneMapCb,&activeCb,&feedbackMapCb);
	mp_map.sendGoal(mapGoal,&doneViewCb,&activeCb,&feedbackViewCb);

	while (mapsResponded < 2) sleep(1);

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

	ROS_INFO("Goals send");
	nav.sendGoal(navGoal);

	is_working = 0;
	totalDist = 0.0;
	dist_.data=totalDist;
	dist_pub_.publish(dist_);


	while(ros::ok && !exitting)
	{
		if(exitting && !is_working){
			return 0;
		}
		ros::spinOnce();
		usleep(100000);
	}
	fclose(mapFile);
	fclose(viewFile);
	return 0;
}
