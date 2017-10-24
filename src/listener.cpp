#include <ros/ros.h>
#include <stdio.h>
#include <iostream>

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
  int id;
  int eval;
  float x;
  float y;
  uint64_t time;
  string t;
};

void infoMapMatch(const stroll_bearnav::NavigationInfo::ConstPtr& msg)
 {
   int size = msg->mapMatchIndex.size();
   vector<MatchInfo> mi;
   if(size>0){
     for(int i = 0; i<size;i++){
         MatchInfo new_mi;
         new_mi.id = i;
         new_mi.eval = msg->mapMatchEval[i];
         new_mi.x = msg->map.feature[i].x;
         new_mi.y = msg->map.feature[i].y;
         new_mi.t = msg->view.header.stamp.sec;
         mi.push_back(new_mi);

     }

     FILE * f;


     if((f = fopen(fname,"r+"))!= NULL){
       char * line = NULL;
       size_t len = 0;
       ssize_t read;
       int id = 0;
       long int pos_now=0;
       fseek(f, 0L, SEEK_END);
       int length = ftell(f);;
       length = std::max(1000,length);
       fseek(f, 0L, SEEK_SET);
       char* rest_f = (char *)malloc(sizeof(char)*length);

       while ((read = getline(&line, &len, f)) != -1) {
         pos_now += read;
         sscanf(line,"%d ",&id);
         for(int i=0;i<mi.size();i++){
           if(id==mi[i].id){
             int rest_l = 0;
             while((read = getline(&line, &len, f)) != -1){
               strcpy(&rest_f[rest_l], line);
               rest_l+=read;
             }
             rest_f[rest_l+1] = '\0';
             fseek(f,(pos_now-1),SEEK_SET);
             pos_now += fprintf(f," %ld %d\n",mi[i].time, mi[i].eval)-1;
             fputs(rest_f,f);
             fseek(f,pos_now,SEEK_SET);
             mi.erase(mi.begin()+i);
             i--;
           }
         }
       }
       free(rest_f);
       fclose(f);
      }
      //pridat vypsani stejnejch;
     if(mi.size()>0){
       f = fopen(fname,"a+");
       while(mi.size()>0){
         int id = mi[0].id;
         fprintf(f,"%d %.02f %.02f %ld %d",mi[0].id, mi[0].x, mi[0].y,mi[0].time, mi[0].eval);
         mi.erase(mi.begin());
         for(int j=0;j<mi.size();j++){
            if(mi[j].id == id){
              fprintf(f," %d",mi[j].eval);
              mi.erase(mi.begin()+j);
              j--;
            }
         }
         fprintf(f,"\n");
       }
       fclose(f);
     }
     mi.clear();
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
