#include <ros/ros.h>
#include <stdio.h>
#include <iostream>

#include "std_msgs/Float32.h"
#include <stroll_bearnav/PathProfile.h>
#include <stroll_bearnav/FeatureArray.h>
#include <stroll_bearnav/NavigationInfo.h>

#include <dynamic_reconfigure/server.h>
#include <stroll_bearnav/navigatorConfig.h>
#include "sensor_msgs/CameraInfo.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <actionlib/server/simple_action_server.h>

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

/* parameters from one view obtained from navigator */
struct ViewInfo {
    float ratio;
    std::vector<KeyPoint> keypoints;
    Mat descriptors;
    std::string id;
    std::vector<int> histogram;
    std::vector<int> mapMatchEval;

    float threshold;
    int targetKeypoints;
    int targetBrightness;
    float velocityGain;
};

/* parameters of one saved map */
struct OneMapInfo{
    std::string id;
    std::string time;
    std::vector<ViewInfo> viewInfo;
    std::string note;
};

/* maps container for writing to disc */
std::vector<OneMapInfo> mapsInfo;
/* helper variables */
OneMapInfo mapInfo;
ViewInfo view;

/* obtained parameters from callbacks */
int targetKeypoints;
int targetBrightness;
float velocityGain;
std::string current_time;
vector<KeyPoint> keypoints;
Mat descriptors;
KeyPoint keypoint;
char name[100];
bool is_loaded = false;

/* suffix of newly saved maps of parameters */
std::string suffix = ".sup";


/* time when all maps are loaded */
void loaderCallback(const stroll_bearnav::PathProfile::ConstPtr& msg)
{
    is_loaded = true;

    ROS_INFO("Maps are loaded.");
}

/* map from loader */
void mapCallback(const stroll_bearnav::FeatureArray::ConstPtr& msg)
{
    /* save previous map*/
    mapsInfo.push_back(mapInfo);

    /* print info */
    ROS_INFO("Pushing map %s",mapInfo.id.c_str());
    printf("time: %s\n", mapInfo.time.c_str());
    for (int i = 0; i < mapInfo.viewInfo.size(); ++i) {
        printf("targetBrightness: %i\n", mapInfo.viewInfo[i].targetBrightness);
        printf("targetKeypoints: %i\n", mapInfo.viewInfo[i].targetKeypoints);
        printf("features size: %ld\n",mapInfo.viewInfo[i].keypoints.size());
        printf("ratio: %.3f\n",mapInfo.viewInfo[i].ratio);
        for (int j = 0; j < mapInfo.viewInfo[i].histogram.size(); ++j) {
            printf("%i ",mapInfo.viewInfo[i].histogram[j]);
        }
        printf("\nvelocityGain: %.3f\n", mapInfo.viewInfo[i].velocityGain);
        printf("threshold: %.3f\n", mapInfo.viewInfo[i].threshold);
    }
    printf("note: %s\n", mapInfo.note.c_str());

    /* save parameters of new map */
    mapInfo.viewInfo.clear();
    mapInfo.id = msg->id;
    mapInfo.note = "";
    mapInfo.time = current_time;

    ROS_INFO("New map: %s",msg->id.c_str());

}

/* save information from view from navigator */
void infoCallback(const stroll_bearnav::NavigationInfo::ConstPtr& msg)
{
    ROS_INFO("New view: %.0f",msg->view.distance);

    /* save keypoints and descriptors */
    view.keypoints.clear();
    view.descriptors=Mat();
    for(int i=0; i<msg->view.feature.size();i++){
        keypoint.pt.x=msg->view.feature[i].x;
        keypoint.pt.y=msg->view.feature[i].y;
        keypoint.size=msg->view.feature[i].size;
        keypoint.angle=msg->view.feature[i].angle;
        keypoint.response=msg->view.feature[i].response;
        keypoint.octave=msg->view.feature[i].octave;
        keypoint.class_id=msg->view.feature[i].class_id;
        view.keypoints.push_back(keypoint);
        int size=msg->view.feature[i].descriptor.size();
        Mat mat(1,size,CV_32FC1,(void*)msg->view.feature[i].descriptor.data());
        view.descriptors.push_back(mat);
    }

    /* save also current parameters */

    //view.id = msg->view.id;
    //printf("view id: %s\n",msg->view.id.c_str());
    char tmp2[100];
    sprintf(tmp2,"%.0f",msg->view.distance);
    //printf("distance: %s\n", tmp2);
    view.id = tmp2;
    view.threshold = msg->view.feature[msg->view.feature.size()-1].response;
    view.targetKeypoints = targetKeypoints;
    view.targetBrightness = targetBrightness;
    view.velocityGain = velocityGain;
    view.ratio = msg->ratio;
    view.histogram = msg->histogram;
    view.mapMatchEval = msg->mapMatchEval;

    /* save this view */
    mapInfo.viewInfo.push_back(view);
}

/* dynamic reconfigure of navigator */
void navigatorCallback(const dynamic_reconfigure::Config::ConstPtr& msg)
{
    velocityGain = msg->doubles[0].value;

}

/* dynamic reconfigure of feature extractor */
void extractorCallback(const dynamic_reconfigure::Config::ConstPtr& msg)
{
    targetKeypoints = msg->ints[1].value;
}

/* dynamic reconfigure of tara camera */
void taraCallback(const dynamic_reconfigure::Config::ConstPtr& msg)
{
    targetBrightness = msg->ints[2].value;
    ROS_INFO("tara %i",targetBrightness);

}

/* gain time from image header timestamp (from tara camera) */
void timeCallback(const sensor_msgs::CameraInfo::ConstPtr& msg)
{
    std_msgs::Header header = msg->header;
    ros::Time timestamp = header.stamp;
    boost::posix_time::ptime my_posix_time = timestamp.toBoost();
    std::string iso_time_str = boost::posix_time::to_iso_extended_string(my_posix_time);
    current_time = iso_time_str;
}

/* cancel of goal of action server */
void cancelCallback(const actionlib_msgs::GoalID::ConstPtr& msg)
{
    ROS_INFO("Navigator goal canceled.");

    /* save last map info into memory */
    mapsInfo.push_back(mapInfo);
    mapInfo.viewInfo.clear();

    /* save parameters of all maps into files */
    for (int k = 0; k < mapsInfo.size(); ++k) {
        sprintf(name,"%s%s", mapsInfo[k].id.c_str(),suffix.c_str());
        ROS_INFO("Saving map to %s", name);
        FileStorage fs(name, FileStorage::WRITE);

        write(fs, "Time", mapsInfo[k].time.c_str());
        /* save parameters of all views of the current map */
        for (int i = 0; i < mapsInfo[k].viewInfo.size(); ++i) {
            sprintf(name,"TargetBrightness_%s", mapsInfo[k].viewInfo[i].id.c_str());
            write(fs, name,mapsInfo[k].viewInfo[i].targetBrightness);
            sprintf(name,"TargetKeypoints_%s", mapsInfo[k].viewInfo[i].id.c_str());
            write(fs, name, mapsInfo[k].viewInfo[i].targetKeypoints);
            sprintf(name,"Keypoints_%s", mapsInfo[k].viewInfo[i].id.c_str());
            write(fs, name, mapsInfo[k].viewInfo[i].keypoints);
            sprintf(name,"Descriptors_%s", mapsInfo[k].viewInfo[i].id.c_str());
            write(fs, name,mapsInfo[k].viewInfo[i].descriptors);
            sprintf(name,"Ratio_%s", mapsInfo[k].viewInfo[i].id.c_str());
            write(fs, name, mapsInfo[k].viewInfo[i].ratio);
            sprintf(name,"Histogram_%s", mapsInfo[k].viewInfo[i].id.c_str());
            write(fs, name, mapsInfo[k].viewInfo[i].histogram);
            sprintf(name,"VelocityGain_%s", mapsInfo[k].viewInfo[i].id.c_str());
            write(fs, name, mapsInfo[k].viewInfo[i].velocityGain);
            sprintf(name,"Threshold_%s", mapsInfo[k].viewInfo[i].id.c_str());
            write(fs, name, mapsInfo[k].viewInfo[i].threshold);
        }
        write(fs, "Note", mapsInfo[k].note.c_str());

        fs.release();
    }

    ROS_INFO("Saving maps finished. ");
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "supervisor");
    ros::NodeHandle nh_;

    ros::Subscriber tara_sub = nh_.subscribe("/stereo/tara_camera/parater_updates", 1, taraCallback);
    ros::Subscriber time_sub = nh_.subscribe("/stereo/left/camera_info", 1, timeCallback);
    ros::Subscriber loader_sub = nh_.subscribe("/load/path", 1, loaderCallback);
    ros::Subscriber map_sub = nh_.subscribe("/load/features", 1, mapCallback);
    ros::Subscriber inf_sub = nh_.subscribe("/navigationInfo", 1, infoCallback);
    ros::Subscriber nav_sub = nh_.subscribe("/navigator_node/parameter_updates", 1, navigatorCallback);
    ros::Subscriber feat_sub = nh_.subscribe("/feature_extraction_node/parameter_updates", 1, extractorCallback);
    ros::Subscriber cancel_sub = nh_.subscribe("/navigator/cancel", 1, cancelCallback);

    ros::spin();
    return 0;
}

