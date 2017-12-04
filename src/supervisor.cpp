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

#include <stroll_bearnav/supervisorConfig.h>
#include <stroll_bearnav/navigatorActionResult.h>
//#include "actionlib_msgs/GoalID.h"
//#include <actionlib/server/simple_action_server.h>
#include <stroll_bearnav/navigatorAction.h>
#include <actionlib/client/simple_action_client.h>


#include <opencv2/opencv.hpp>

#include <std_msgs/Int32.h>

using namespace cv;
using namespace std;

typedef actionlib::SimpleActionClient<stroll_bearnav::navigatorAction> navigatorClient;
//navigatorClient ac("navClient", true);
stroll_bearnav::navigatorActionGoal goal;
stroll_bearnav::navigatorGoal navgoal;

/* parameters from one view obtained from navigator */
struct ViewInfo {
    float ratio;
    std::vector<KeyPoint> keypoints;
    Mat descriptors;
    std::string id;
    float distance_view;
    std::vector<int> histogram;
    std::vector<int> mapMatchIndex;
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

/* for changing dynamic reconfigure parameters */
dynamic_reconfigure::Config config;
dynamic_reconfigure::IntParameter int_msg;
dynamic_reconfigure::ReconfigureRequest srv_req;
dynamic_reconfigure::ReconfigureResponse srv_resp;
dynamic_reconfigure::IntParameter thre_msg;
dynamic_reconfigure::BoolParameter adapt_msg;

/* for new image from rosbag */
ros::Publisher bag_pub_;
std_msgs::Int32 bag_msg;

/* parameters of optimization of targetKeypoints */
int value;
bool paused=true;
bool view_saving = false;
int range_max = 5000;
float accuracy = 0.3;

/* signal for feature holder node to send features */
ros::Publisher fh_pub_;
std_msgs::Int32 fh_msg;

/* signal for rosbag node to publish the image */
ros::Publisher img_pub_;
std_msgs::Int32 img_msg;

/* clock for time measuring - what slows down the algorithm */
clock_t t;


/* when all maps are loaded */
void loaderCallback(const stroll_bearnav::PathProfile::ConstPtr& msg)
{
    is_loaded = true;

    ROS_INFO("Maps are loaded.");

   /* ac.sendGoal(navgoal);

    ac.waitForResult();

    if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
        ROS_INFO("Success");
    else
        ROS_INFO("Fail");*/
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

        ROS_INFO("New view: %.0f", msg->view.distance);


        /* wait for message with requested targetKeypoints */
        ROS_INFO("targetKeyponts is %i = msg is %ld", int_msg.value, msg->view.feature.size());
        /* number of extracted features is equal to requested */
        if (msg->view.feature.size() == int_msg.value ) {
            paused = false;
        } else {
            /* threshold is 0, no more features */
            if( msg->view.feature.size() > 0 // at least 1 feature to obtain threshold
                    && abs( msg->view.feature[msg->view.feature.size() - 1].response - 0) < accuracy // threshold == 0
                    && int_msg.value > msg->view.feature.size() ){ // requested more than can be extracted
                ROS_INFO("cannot reach targetKeypoints, more than can be extracted, threshold is 0: %.3f",msg->view.feature[msg->view.feature.size() - 1].response);
                paused = false;
            } else {
                if(msg->view.feature.size() > 0) ROS_INFO("threshold is %.3f",msg->view.feature[msg->view.feature.size() - 1].response);
                if((msg->view.feature.size() <= 0 && int_msg.value > 0) // zero features but not requested
                    || (msg->view.feature.size() > 0 // at least 1 feature to obtain threshold
                        && abs( msg->view.feature[msg->view.feature.size() - 1].response - 0) >= accuracy && int_msg.value == range_max) ){ // threshold !=0 but requested maximum keypoints

                    /* take features from feature extractor - publish img from rosbag */
                    ROS_INFO("extracting...");
                    img_pub_.publish(img_msg);
                    ros::spinOnce();
                } else {
                    // requested less keypoints than obtained
                    ROS_INFO("feature holder publishing... requested less");
                    fh_msg.data = true;
                    fh_pub_.publish(fh_msg);
                    ros::spinOnce();
                }
            }
        }

        /* if requested targetKeypoints, update view variables */
        if (!paused) {
            targetKeypoints = msg->view.feature.size();
            /* feature holder node sends new features */
            fh_msg.data = true;
            fh_pub_.publish(fh_msg);
            ros::spinOnce();

            /* save keypoints and descriptors */
            view.keypoints.clear();
            view.descriptors = Mat();
            for (int i = 0; i < msg->view.feature.size(); i++) {
                keypoint.pt.x = msg->view.feature[i].x;
                keypoint.pt.y = msg->view.feature[i].y;
                keypoint.size = msg->view.feature[i].size;
                keypoint.angle = msg->view.feature[i].angle;
                keypoint.response = msg->view.feature[i].response;
                keypoint.octave = msg->view.feature[i].octave;
                keypoint.class_id = msg->view.feature[i].class_id;
                view.keypoints.push_back(keypoint);
                int size = msg->view.feature[i].descriptor.size();
                Mat mat(1, size, CV_32FC1, (void *) msg->view.feature[i].descriptor.data());
                view.descriptors.push_back(mat);
            }

            /* save also current parameters */
            view.distance_view = msg->view.distance;
            //view.id = msg->view.id;
            sprintf(name, "%.0f", msg->view.distance);
            view.id = name;
            if (msg->view.feature.size() != 0) {
                view.threshold = msg->view.feature[msg->view.feature.size() - 1].response;
            } else {
                view.threshold = 10000;
            }
            view.targetKeypoints = targetKeypoints;
            view.targetBrightness = targetBrightness;
            view.velocityGain = velocityGain;
            view.ratio = msg->ratio;
            view.histogram = msg->histogram;
            view.mapMatchIndex = msg->mapMatchIndex;
            view.mapMatchEval = msg->mapMatchEval;

            /* save this view with the best targetKeypoints*/
            if (view_saving) mapInfo.viewInfo.push_back(view);
        }

}

/* dynamic reconfigure of navigator */
void navigatorCallback(const dynamic_reconfigure::Config::ConstPtr& msg)
{
    velocityGain = msg->doubles[0].value;
    printf("navigatorCallback, velocityGain %.3f\n",velocityGain);
}

/* dynamic reconfigure of feature extractor */
void extractorCallback(const dynamic_reconfigure::Config::ConstPtr& msg)
{
    targetKeypoints = msg->ints[1].value;
    printf("extractorCallback, targetKeypoints %i\n",targetKeypoints);
}

/* dynamic reconfigure of tara camera */
void taraCallback(const dynamic_reconfigure::Config::ConstPtr& msg)
{
    targetBrightness = msg->ints[2].value;
    ROS_INFO("taraCallback, targetBrightness %i",targetBrightness);

}

/* gain time from image header timestamp (from tara camera) */
void timeCallback(const sensor_msgs::CameraInfo::ConstPtr& msg)
{
    std_msgs::Header header = msg->header;
    ros::Time timestamp = header.stamp;
    boost::posix_time::ptime my_posix_time = timestamp.toBoost();
    std::string iso_time_str = boost::posix_time::to_iso_extended_string(my_posix_time);
    current_time = iso_time_str;
    ROS_INFO("timeCallback, time %s",current_time.c_str());
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
                sprintf(name, "MapMatchIndex_%s", mapsInfo[k].viewInfo[i].id.c_str());
                write(fs, name, mapsInfo[k].viewInfo[i].mapMatchIndex);
                sprintf(name, "MapMatchEval_%s", mapsInfo[k].viewInfo[i].id.c_str());
                write(fs, name, mapsInfo[k].viewInfo[i].mapMatchEval);
            }
            write(fs, "Note", mapsInfo[k].note.c_str());

            fs.release();
        }

        ROS_INFO("Saving maps finished. ");

}

/* dynamic reconfigure of supervisor */
void callback(stroll_bearnav::supervisorConfig &config, uint32_t level)
{
    // accuracy of zero threshold
    accuracy = config.accuracy;

    ROS_INFO("accuracy: %.3f",accuracy);
}

void resultCallback(const stroll_bearnav::navigatorActionResult::ConstPtr& msg)
{
    bool result = msg->result.success;

    if(result) {
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
                sprintf(name, "MapMatchIndex_%s", mapsInfo[k].viewInfo[i].id.c_str());
                write(fs, name, mapsInfo[k].viewInfo[i].mapMatchIndex);
                sprintf(name, "MapMatchEval_%s", mapsInfo[k].viewInfo[i].id.c_str());
                write(fs, name, mapsInfo[k].viewInfo[i].mapMatchEval);
            }
            write(fs, "Note", mapsInfo[k].note.c_str());

            fs.release();
        }

        ROS_INFO("Saving maps finished. ");
    }
}

int main(int argc, char** argv) 
{
    ros::init(argc, argv, "supervisor");
    ros::NodeHandle nh_;

    ros::Subscriber tara_sub = nh_.subscribe("/stereo/tara_camera/parater_updates", 1, taraCallback);
    ros::Subscriber time_sub = nh_.subscribe("/stereo/left/camera_info", 1, timeCallback);
    ros::Subscriber loader_sub = nh_.subscribe("/pathProfile", 1, loaderCallback);
    ros::Subscriber map_sub = nh_.subscribe("/localMap", 1, mapCallback);
    ros::Subscriber inf_sub = nh_.subscribe("/navigationInfo", 1, infoCallback);
    ros::Subscriber nav_sub = nh_.subscribe("/navigator/parameter_updates", 1, navigatorCallback);
    ros::Subscriber feat_sub = nh_.subscribe("/feature_extraction/parameter_updates", 1, extractorCallback);
    ros::Subscriber cancel_sub = nh_.subscribe("/navigator/cancel", 1, cancelCallback);
    ros::Subscriber result_sub = nh_.subscribe("/navigator/result", 1, resultCallback);

    bag_pub_ = nh_.advertise<std_msgs::Int32>("/rosbag/pause", 1);
    fh_pub_ = nh_.advertise<std_msgs::Int32>("/featureHolder/start_img", 1);
    img_pub_ = nh_.advertise<std_msgs::Int32>("/rosbag/publish_img", 1);

    /* Initiate dynamic reconfiguration */
    dynamic_reconfigure::Server<stroll_bearnav::supervisorConfig> server;
    dynamic_reconfigure::Server<stroll_bearnav::supervisorConfig>::CallbackType f = boost::bind(&callback, _1, _2);
    server.setCallback(f);


    /*while(!ac.waitForServer(ros::Duration(5.0))){
        ROS_INFO("Waiting for the navClient action server to come up");
    }*/

    /* optimization function to find the best targetKeypoints of each image according to correct matches */
    /* optimization parameters */
    int range_min = 0;
    int min_step = 4;

    int granularity = 10;
    int bins[granularity];
    for (int l = 0; l < 10; ++l) bins[l] = 0;

    int j = 0;
    int start = range_min;
    int end = range_max;
    int step = (int) round( (end-start)/granularity );
    int max_ = -1;
    int max_ind = -1;
    int count = 0;
    int_msg.name = "targetKeypoints";
    ROS_INFO("start");

    /* set up dynamic reconfigurator (to gain maximum of features quickly) */
    thre_msg.name = "thresholdParam";
    thre_msg.value = 0;
    adapt_msg.name = "adaptThreshold";
    adapt_msg.value = false;
    int_msg.value = range_max;
    config.ints.clear();
    config.ints.push_back(int_msg);
    config.ints.push_back(thre_msg);
    config.bools.clear();
    config.bools.push_back(adapt_msg);
    srv_req.config = config;
    ROS_INFO("Setting threshold to %i",thre_msg.value);
    t = clock();
    if (ros::service::call("/feature_extraction/set_parameters", srv_req, srv_resp)) {
        ROS_INFO("Call to set feature_extraction parameters succeeded.");
    } else {
        ROS_INFO("Call to set feature_extraction parameters failed.");
    }
    ROS_INFO("Service Time taken: %.4f s", (float)(clock() - t)/CLOCKS_PER_SEC);
    config.ints.clear();
    config.bools.clear();
    ros::spinOnce();

    /* waiting for loaded maps */
    while(!is_loaded && ros::ok()) ros::spinOnce();
    /* publish the first image from rosbag */
    img_pub_.publish(img_msg);

    while(ros::ok()) {
        ROS_INFO("Running next iteration with step %i",step);
        /* change targetKeypoints in iteration's interval with iteration's step and count correct matches*/
        for (int m = end; m > start; m -= step) {
            /* change targetKeypoints */
            int_msg.value=m;
            config.ints.clear();
            config.ints.push_back(int_msg);
            srv_req.config = config;
            /* update change of targetKeypoints through dynamic reconfigure service */
            ROS_INFO("Setting targetKeypoints to %i",int_msg.value);
            t = clock();
            if (ros::service::call("/feature_extraction/set_parameters", srv_req, srv_resp)) {
                ROS_INFO("Call to set feature_extraction parameters succeeded.");
            } else {
                ROS_INFO("Call to set feature_extraction parameters failed.");
            }
            ROS_INFO("Service Time taken: %.4f s", (float)(clock() - t)/CLOCKS_PER_SEC);

            /* update parameters through callbacks, wait for requested targetKeypoints */
            t = clock();
            while(paused && ros::ok()) ros::spinOnce();
            ROS_INFO("Time taken: %.4fs", (float)(clock() - t)/CLOCKS_PER_SEC);
            paused=true;

            ROS_INFO("Actual targetKeypoints %i, step %i, bin %i",m,step,j);
            ROS_INFO("Counting correct matches from evaluation size %ld",view.mapMatchEval.size());
            /* count correct matches */
            for (int i = 0; i < view.mapMatchEval.size(); i++) {
                if (view.mapMatchEval[i] == 1) bins[j]++;
            }
            j++;
            if(!ros::ok()) break;
        }
        j = 0;

        /* find maximum of correct matches */
        for (int k = 0; k < granularity; ++k) {
            ROS_INFO("Finding max of correct matches %i %i",k,bins[k]);
            if(bins[k] > max_){
                max_ = bins[k];
                max_ind = k;
                count=0;
            } else if(bins[k] == max_){
                count++;
            }
        }
        /* if there are more indexes with max, then take the center */
        if(count > 0){
            ROS_INFO("%i indexes with the same max value, starting %i",count,max_ind);
            max_ind+= (int) round(count/2);
        }
        count = 0;

        /* reduce step and range */
        value = end - max_ind*step;
        start = max(value - step, range_min);
        end = min(value + step, range_max);
        step = (int) round( (end-start)/granularity );
        ROS_INFO("Max is %i on index %i, step %i, start %i, end %i", value, max_ind, step, start, end);

        /* save optimal targetKeypoints and request for the next image */
        if(step < min_step){
            ROS_INFO("The best value of targetKeypoints is %i",value);

            /* save view info of the best value */
            ROS_INFO("Saving info of the best value %i",value);
            view_saving = true;
            /* change targetKeypoints */
            int_msg.value=value;
            config.ints.clear();
            config.ints.push_back(int_msg);
            srv_req.config = config;
            /* update change of targetKeypoints through dynamic reconfigure service */
            ROS_INFO("Setting targetKeypoints to %i",int_msg.value);
            t = clock();
            if (ros::service::call("/feature_extraction/set_parameters", srv_req, srv_resp)) {
                ROS_INFO("Call to set feature_extraction parameters succeeded");
            } else {
                ROS_INFO("Call to set feature_extraction parameters failed");
            }
            ROS_INFO("Service Time taken: %.4f s", (float)(clock() - t)/CLOCKS_PER_SEC);

            /* update parameters through callbacks, wait for requested targetKeypoints */
            t = clock();
            while(paused && ros::ok()) ros::spinOnce();
            ROS_INFO("Time taken: %.4fs", (float)(clock() - t)/CLOCKS_PER_SEC);
            paused=true;
            view_saving = false;

            ROS_INFO("\nRequest for the next image.");
            /* continue rosbag */
            bag_msg.data = false;
            bag_pub_.publish(bag_msg);
            /* reset variables */
            start = range_min;
            end = range_max;
            step = (int) round( (end-start)/granularity );
            ROS_INFO("Step %i, start %i, end %i", step, start, end);

            /* set up dynamic reconfigurator */
            int_msg.value = range_max;
            config.ints.clear();
            config.ints.push_back(int_msg);
            config.ints.push_back(thre_msg);
            srv_req.config = config;
            ROS_INFO("Setting threshold to %i",thre_msg.value);
            t = clock();
            if (ros::service::call("/feature_extraction/set_parameters", srv_req, srv_resp)) {
                ROS_INFO("Call to set feature_extraction parameters succeeded.");
            } else {
                ROS_INFO("Call to set feature_extraction parameters failed.");
            }
            ROS_INFO("Service Time taken: %.4f s", (float)(clock() - t)/CLOCKS_PER_SEC);

            /* send image from rosbag */
            img_pub_.publish(img_msg);
            /* update callbacks */
            ros::spinOnce();
        }

        /* reset variables */
        for (int l = 0; l < 10; ++l) bins[l] = 0;
        max_ = -1;
        max_ind = -1;
    }
    return 0;
}
