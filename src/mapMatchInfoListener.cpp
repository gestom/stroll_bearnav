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
#include <actionlib/server/simple_action_server.h>
#include <signal.h>
#include <ros/xmlrpc_manager.h>
#include <ros/callback_queue.h>
#include <opencv2/opencv.hpp>
#include <dynamic_reconfigure/server.h>
#include <stroll_bearnav/listenerConfig.h>

#include <std_msgs/Int32.h>

using namespace cv;
using namespace std;
char* fname;
uint32_t currentTime;
vector<string> f_ids;

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

bool volatile exitting = false;
bool volatile is_working = 0;
ros::CallbackQueue* my_queue;

void mySigHandler(int sig)
{
  my_queue = ros::getGlobalCallbackQueue();
  exitting = true;

  printf("exitting\n");
}

void callback(stroll_bearnav::listenerConfig &config, uint32_t level)
{
	currentTime=config.currentTime;
  printf("%u\n", currentTime);
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
   vector<MatchInfo> mi;
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
         new_mi.time = currentTime;
         //TODO do navigation info pridat cas explicitne
         // new_mi.time = time(NULL);
         mi.push_back(new_mi);
     }
     //TODO predelat na vlozeny s teckou vpredu
     char oldname[] =".statistics.txt";
     ofstream file_content(oldname);
     //ostringstream file_content;
     string line;
     ifstream f (fname);
     int f_index = -1;

     for (int i = 0; i < f_ids.size(); i++) {
        if(f_ids[i].compare(mi[0].id)==0){
          f_index = i;
          break;
        }
     }

     if (f.is_open()  && file_content.is_open())
     {
       int i_line = -1;
       while ( getline (f,line) )
       {
         i_line++;
         if(f_index < 0 || f_index>i_line || i_line> f_index+f_ids.size()-1){
           file_content << line <<endl;
           continue;
         }
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
         string id(mi[i].id);
         f_ids.push_back(id);
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



     if ( rename( oldname , fname ) )
     {
       perror( "Error renaming file" );
     }
   }
  is_working = 0;
  ROS_INFO("%.3f",msg->distance);
}


int main(int argc, char **argv)
{
	if(argc<2){
		perror("You must enter file Name");
		return 0;
	}else{
		fname = argv[1];
	}
	signal(SIGINT, mySigHandler);
	ros::init(argc, argv, "listener", ros::init_options::NoSigintHandler);

	ros::XMLRPCManager::instance()->unbind("shutdown");
	ros::XMLRPCManager::instance()->bind("shutdown", shutdownCallback);

	ros::NodeHandle n;

	ros::Subscriber sub = n.subscribe("/navigationInfo", 1000, infoMapMatch);
	dynamic_reconfigure::Server<stroll_bearnav::listenerConfig> server;
	dynamic_reconfigure::Server<stroll_bearnav::listenerConfig>::CallbackType f = boost::bind(&callback, _1, _2);
	server.setCallback(f);
	while(ros::ok && !exitting)
	{
		if(exitting && !is_working){
			return 0;
		}
		ros::spinOnce();
	}


	return 0;
}
