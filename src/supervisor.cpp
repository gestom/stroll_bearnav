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

#include <std_msgs/Int32.h>
//#include <stroll_bearnav/featureExtractionConfig.h>

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

bool saving = false;
//dynamic_reconfigure::Config::ConstPtr nav_msg;
//stroll_bearnav::featureExtractionConfig config;
dynamic_reconfigure::Config config2;

/*vector <dynamic_reconfigure::BoolParameter> bools;
vector <dynamic_reconfigure::IntParameter> ints;
vector <dynamic_reconfigure::StrParameter> strs;
vector <dynamic_reconfigure::DoubleParameter> doubles;
vector <dynamic_reconfigure::GroupState> groups;*/

//dynamic_reconfigure::BoolParameter bool_msg;
dynamic_reconfigure::IntParameter int_msg;
/*dynamic_reconfigure::StrParameter str_msg;
dynamic_reconfigure::DoubleParameter double_msg;
dynamic_reconfigure::GroupState group_msg;*/

dynamic_reconfigure::ReconfigureRequest srv_req;
dynamic_reconfigure::ReconfigureResponse srv_resp;

ros::Publisher bag_pub_;
ros::Publisher key_pub_;
std_msgs::Int32 msg_bag;
std_msgs::Int32 key_msg;
int counter=0;
int iter=0;
bool paused=true;
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
    key_pub_.publish(key_msg);
    ROS_INFO("targetKeyponts published %i",key_msg.data);
    if(msg->view.feature.size() == key_msg.data || iter>=5) paused = false;
    iter++;
    if(iter==6) {
        ROS_INFO("cannot reach targetKeypoints");
        iter = 0;
    }
    //msg_bag.data = 1;
    /*if(msg_bag.data == 1 && counter>5){
        msg_bag.data=0;
        counter=0;
    } else msg_bag.data=1;counter++;
    bag_pub_.publish(msg_bag);*/

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
    if(msg->view.feature.size() != 0) {
        view.threshold = msg->view.feature[msg->view.feature.size() - 1].response;
    } else {
        view.threshold = 10000;
    }
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
   // nav_msg = *msg;
    //config.targetKeypoints = targetKeypoints;
    //config2 = *msg;
    //config2.ints = msg->ints;
    printf("targetKeypoints %i\n",msg->ints[1].value);
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

    if(saving) {

        /* save last map info into memory */
        mapsInfo.push_back(mapInfo);
        mapInfo.viewInfo.clear();

        /* save parameters of all maps into files */
        for (int k = 0; k < mapsInfo.size(); ++k) {
            sprintf(name, "%s%s", mapsInfo[k].id.c_str(), suffix.c_str());
            ROS_INFO("Saving map to %s", name);
            FileStorage fs(name, FileStorage::WRITE);

            write(fs, "Time", mapsInfo[k].time.c_str());
            /* save parameters of all views of the current map */
            for (int i = 0; i < mapsInfo[k].viewInfo.size(); ++i) {
                sprintf(name, "TargetBrightness_%s", mapsInfo[k].viewInfo[i].id.c_str());
                write(fs, name, mapsInfo[k].viewInfo[i].targetBrightness);
                sprintf(name, "TargetKeypoints_%s", mapsInfo[k].viewInfo[i].id.c_str());
                write(fs, name, mapsInfo[k].viewInfo[i].targetKeypoints);
                sprintf(name, "Keypoints_%s", mapsInfo[k].viewInfo[i].id.c_str());
                write(fs, name, mapsInfo[k].viewInfo[i].keypoints);
                sprintf(name, "Descriptors_%s", mapsInfo[k].viewInfo[i].id.c_str());
                write(fs, name, mapsInfo[k].viewInfo[i].descriptors);
                sprintf(name, "Ratio_%s", mapsInfo[k].viewInfo[i].id.c_str());
                write(fs, name, mapsInfo[k].viewInfo[i].ratio);
                sprintf(name, "Histogram_%s", mapsInfo[k].viewInfo[i].id.c_str());
                write(fs, name, mapsInfo[k].viewInfo[i].histogram);
                sprintf(name, "VelocityGain_%s", mapsInfo[k].viewInfo[i].id.c_str());
                write(fs, name, mapsInfo[k].viewInfo[i].velocityGain);
                sprintf(name, "Threshold_%s", mapsInfo[k].viewInfo[i].id.c_str());
                write(fs, name, mapsInfo[k].viewInfo[i].threshold);
            }
            write(fs, "Note", mapsInfo[k].note.c_str());

            fs.release();
        }

        ROS_INFO("Saving maps finished. ");
    }
}

int main(int argc, char** argv) {
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

    bag_pub_ = nh_.advertise<std_msgs::Int32>("/rosbag/pause", 1);
    key_pub_ = nh_.advertise<std_msgs::Int32>("/targetKeypoints", 1);

    //ros::Publisher feat_pub_ = nh_.advertise<dynamic_reconfigure::Config>("/feature_extraction/parameter_updates",1);
    //ROS_INFO("should work");
    int bin_count = 10;
    int bins[bin_count];
    for (int l = 0; l < 10; ++l) {
        bins[l] = 0;
    }
    //ROS_INFO("work1");

    /*int_msg.name="thresholdParam";
    int_msg.value=400;
    ints.push_back(int_msg);*/
    int_msg.name = "targetKeypoints";
    int_msg.value = 0;

    key_msg.data = 0;
    key_pub_.publish(key_msg);
    //ints.push_back(int_msg);
    //config2.ints = ints;
    config2.ints.push_back(int_msg);

    srv_req.config = config2;

    /*if (ros::service::call("/feature_extraction/set_parameters", srv_req, srv_resp)) {
        ROS_INFO("call to set feature_extraction parameters succeeded");
    } else {
        ROS_INFO("call to set feature_extraction parameters failed");
    }*/


    //ROS_INFO("work2");
    // config.targetKeypoints = 0;
    //if(config2.ints == NULL){
    //    printf("config2 is null\n");
    //} else {
    //printf("config2 is not null, %ld\n",config2.ints.size());
    //}
    //config2.ints[1].value = 0;
    //nav_msg->ints[1].value = 0;
    //ROS_INFO("work2.5");
    //feat_pub_.publish(nav_msg);
    // feat_pub_.publish(config2);
    //ROS_INFO("work3");
    //for (int in = 0; in < 5; ++in) ros::spinOnce();
    //ROS_INFO("work4");
    int j = 0;
    int start = 0;
    int end = 5000;
    int increment = end / bin_count; //500
    int max = 0;
    int max_ind = -1;
    int count = 0;
    int center = 0;
    int size = 0;
    ROS_INFO("start");
    int ind = 0;
    /*while(ros::ok()) {
        msg_bag.data = false;
        bag_pub_.publish(msg_bag);
        ros::spinOnce();
    }*/
    /*while(ros::ok()) {
            //for (; ; ) {

            //config2.ints[1].value = ind;
                int_msg.value=ind;

                config2.ints.clear();
                config2.ints.push_back(int_msg);

                srv_req.config = config2;

                if (ros::service::call("/feature_extraction/set_parameters", srv_req, srv_resp)) {
                    ROS_INFO("call to set feature_extraction parameters succeeded");
                } else {
                    ROS_INFO("call to set feature_extraction parameters failed");
                }

            ind++;
            if(ind > 5000){
                ind=0;
                ROS_INFO("parameter reaches 5000, request for new frame");
                msg_bag.data = false;
                bag_pub_.publish(msg_bag);
            }
            //feat_pub_.publish(config2);


            ros::spinOnce();
           //}

    }*/
    //ros::spin();
    key_msg.data = 0;
    key_pub_.publish(key_msg);
    ROS_INFO("targetKeyponts published");
    ros::spinOnce();

    while(paused && ros::ok()) ros::spinOnce();
    paused=true;
    while(ros::ok()) {
        ROS_INFO("new while");
        for (int m = start; m < end; m += increment) {
            ROS_INFO("eval size %ld increment %i m %i",view.mapMatchEval.size(), increment,m);
            //usleep(2000);
            // count correct matches
            for (int i = 0; i < view.mapMatchEval.size(); i++) {
                if (view.mapMatchEval[i] == 1) bins[j]++;
                //ROS_INFO("counting correct matches");
            }
            ROS_INFO("m %i j %i",m,j);
            j++;

            // change targetKeypoints
            //nav_msg.ints[1].value += increment; // targetKeypoints
            //feat_pub_.publish(nav_msg);
            int_msg.value+=increment;
            key_msg.data+=increment;
            key_pub_.publish(key_msg);

            config2.ints.clear();
            config2.ints.push_back(int_msg);

            srv_req.config = config2;

           /* if (ros::service::call("/feature_extraction/set_parameters", srv_req, srv_resp)) {
                ROS_INFO("call to set feature_extraction parameters succeeded");
            } else {
                ROS_INFO("call to set feature_extraction parameters failed");
            }*/

            // update params through callbacks
            while(paused && ros::ok()) ros::spinOnce();
            paused=true;
        }
        j = 0;

        // find max of correct matches
        for (int k = 0; k < bin_count; ++k) {
            ROS_INFO("finding max of correct matches %i %i",k,bins[k]);
            if(bins[k] > max){
                max = bins[k];
                max_ind = k;
            } else if(bins[k] == max){
                count++;
            }
            // if there are more indexes with max, then take the center
            if(count > 0){
                max_ind+= (int) round(count/2);
            }
        }
        count = 0;

        // reduce step
        center = (max_ind+1)*increment;
        increment = (int) round(increment/2);
        size = increment*bin_count;
        start = center - (int) round(size/2);
        if(start<0) start = 0;
        end = center + (int) round(size/2);
        if(end > 5000) end = 5000;
        for (int l = 0; l < 10; ++l) {
            bins[l]=0;
        }
        ROS_INFO("max is %i on index %i, increment %i, start %i, end %i, size is %i",center,max_ind,increment,start,end,size);

        int_msg.value=0;

        config2.ints.clear();
        config2.ints.push_back(int_msg);

        srv_req.config = config2;

        /*if (ros::service::call("/feature_extraction/set_parameters", srv_req, srv_resp)) {
            ROS_INFO("call to set feature_extraction parameters succeeded");
        } else {
            ROS_INFO("call to set feature_extraction parameters failed");
        }*/
        key_msg.data=0;
        key_pub_.publish(key_msg);

        // update callbacks
        while(paused && ros::ok()) ros::spinOnce();
        paused=true;


        // next image
        if(increment < 250){
            ROS_INFO("next image request");
            //usleep(1000000);
            //continue rosbag
            msg_bag.data = false;
            bag_pub_.publish(msg_bag);
            // set targetKeypoints to 0
            //nav_msg.ints[1].value = 0;
            //feat_pub_.publish(nav_msg);
            int_msg.value=0;

            config2.ints.clear();
            config2.ints.push_back(int_msg);

            srv_req.config = config2;

            /*if (ros::service::call("/feature_extraction/set_parameters", srv_req, srv_resp)) {
                ROS_INFO("call to set feature_extraction parameters succeeded");
            } else {
                ROS_INFO("call to set feature_extraction parameters failed");
            }*/
            //key_msg.data=0;
            //key_pub_.publish(key_msg);

            // update callbacks
            ros::spinOnce();
            paused=true;
            // reset variables
            j = 0;
            start = 0;
            end = 5000;
            increment = end/bin_count; //500
            max = 0;
            max_ind = -1;
            count = 0;
            center = 0;
            size = 0;
        }
    }

    return 0;
}

