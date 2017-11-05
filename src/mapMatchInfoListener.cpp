#include <ros/ros.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
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

#include <opencv2/opencv.hpp>

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
  int time;
  string t;
};

void infoMapMatch(const stroll_bearnav::NavigationInfo::ConstPtr& msg)
 {
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
         new_mi.time = msg->view.header.stamp.sec;
         mi.push_back(new_mi);
     }

     ostringstream file_content;
     string line;
     ifstream f (fname);

     if (f.is_open())
     {
       while ( getline (f,line) )
       {
         vector<string> strings;
         istringstream l(line);
         string s;
         int i = 0;
         int j = 0;
         ostringstream end_line;
         //end_line  << "\n";
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
           file_content << line << end_line.str();
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

     ofstream f2 (fname);
     if((f2.is_open()))
     {
       f2 << file_content.str();
     }
     f2.close();
  }
}


int main(int argc, char **argv)
{
  if(argc<2){
    perror("You must enter file Name");
    return 1;
  }
  fname = argv[1];
  ros::init(argc, argv, "listener");

  ros::NodeHandle n;

  ros::Subscriber sub = n.subscribe("/navigationInfo", 1000, infoMapMatch);

  ros::spin();

  return 0;
}
