#include <ros/ros.h>
#include <stdio.h>
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

bool conti = true;
bool stop = false;
ros::CallbackQueue* my_queue;
float griefScore[256];

void mySigHandler(int sig)
{
  conti = false;
  if (sig == 15) stop = true;
}

void infoMapMatch(const stroll_bearnav::NavigationInfo::ConstPtr& msg)
{
	//is_working = 1;
	int size = msg->mapMatchIndex.size();
	vector<MatchInfo> mi;
	if(size>0)
	{
		if (msg->map.feature.size() != 	msg->mapMatchIndex.size()){
			//printf("Trouble - map and index size mismatch!\n");
			size = 0; 
		}
		float posCount = 0;
		float negCount = 0;
		for(int i = 0; i<size;i++){
			 if (msg->mapMatchEval[i] > 0) posCount++;
			 if (msg->mapMatchEval[i] < 0) negCount--;
		}
		for(int i = 0; i<size ;i++)
		{
			float eval = msg->mapMatchEval[i];
			if (eval != 0){
				if (eval < 0 && negCount > 0) eval = eval/negCount;
				if (eval > 0 && posCount > 0) eval = eval/posCount;
				Mat mapDes(1,32,  CV_32FC1,(void*)msg->map.feature[i].descriptor.data());
				Mat viewDes(1,32,CV_32FC1,(void*)msg->view.feature[msg->mapMatchIndex[i]].descriptor.data());
				mapDes.convertTo(mapDes,CV_8U);
			//	cout << mapDes << endl;
				viewDes.convertTo(viewDes,CV_8U);
			//	cout << viewDes << endl;
				for (int o=0;o<32;o++){
					unsigned char chunk0 = mapDes.at<uchar>(0,o);
					unsigned char chunk1 = viewDes.at<uchar>(0,o);
					unsigned char chunk = chunk0^chunk1;
					unsigned char rotBit = 128;
					for (int p = 0;p<8;p++){
						if ((chunk&rotBit) > 0)  griefScore[8*o+p]-=eval; else griefScore[8*o+p]+=eval;
						//printf("%i %i %i %i %i\n",i,chunk,rotBit,griefScore[8*o+p],eval);
						rotBit=rotBit/2;
					}
				}
			}
		}
	}
}

typedef struct
{
	int id,x0,x1,y0,y1;
	float score;
}STracia;

int compareScore(const void * a, const void * b)
{
  if ( (*(STracia*)a).score <  (*(STracia*)b).score ) return +1;
  if ( (*(STracia*)a).score >  (*(STracia*)b).score ) return -1;
  return 0;
}

int compareID(const void * a, const void * b)
{
  if ( (*(STracia*)a).id >  (*(STracia*)b).id ) return +1;
  if ( (*(STracia*)a).id <  (*(STracia*)b).id ) return -1;
  return 0;
}

STracia tracia[256];

int main(int argc, char **argv)
{
	srand (time(NULL));
	signal(SIGTERM, mySigHandler);
	signal(SIGUSR1, mySigHandler);
	ros::init(argc, argv, "listener");
	ros::NodeHandle n;
	ros::Subscriber sub = n.subscribe("/navigationInfo", 1000, infoMapMatch);
	int round = 0;
	while (stop == false){
		conti = true;
		memset(griefScore,0,sizeof(float)*256);
		while( ros::ok && conti)
		{
			ros::spinOnce();
			usleep(100000);
		}

		float fit = 0;
		char filename[1000];

		FILE* file = fopen("/home/gestom/test_pairs.grief","r");
		sprintf(filename,"/home/gestom/test_pairs_%i.grief",round++);
		FILE* evalFile = fopen(filename,"w");

		for (int i = 0;i<256;i++){
			fscanf(file,"%i %i %i %i\n",&tracia[i].x0,&tracia[i].x1,&tracia[i].y0,&tracia[i].y1);
			tracia[i].id = i;
			tracia[i].score = griefScore[i];
			fprintf(evalFile,"%i %i %i %i %i %.3f\n",tracia[i].id,tracia[i].x0,tracia[i].x1,tracia[i].y0,tracia[i].y1,tracia[i].score);
			fit += tracia[i].score;	
		}
		fprintf(evalFile,"FIT: %.3f\n",fit);
		fclose(file);
		fclose(evalFile);
		printf("FIT: %.3f\n",fit);
		qsort (tracia,256,sizeof(STracia),compareScore);

		int xWindow = 48;
		int yWindow = 48;
		for (int i = 256-atoi(argv[1]);i<256;i++){
			tracia[i].x0 = rand()%xWindow-xWindow/2;
			tracia[i].x1 = rand()%xWindow-xWindow/2;
			tracia[i].y0 = rand()%yWindow-yWindow/2;
			tracia[i].y1 = rand()%yWindow-yWindow/2;
		}
		qsort (tracia,256,sizeof(STracia),compareID);

		file = fopen("/home/gestom/test_pairs.grief","w");
		for (int i = 0;i<256;i++){
			fprintf(file,"%i %i %i %i\n",tracia[i].x0,tracia[i].x1,tracia[i].y0,tracia[i].y1);
		}
		fclose(file);
	}
	return 0;
}
