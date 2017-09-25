#include <ros/ros.h>
#include <stdio.h>
#include <iostream>

#include <rosbag/bag.h>
#include <rosbag/view.h>
#include <std_msgs/Int32.h>
#include <std_msgs/String.h>

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include "sensor_msgs/CameraInfo.h"
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>
#include <opencv2/opencv.hpp>
#include <nav_msgs/Odometry.h>
#include <dynamic_reconfigure/server.h>
#include "boost/date_time/posix_time/posix_time.hpp"
using namespace cv;
using namespace std;

int paused = false;
string folder;
string current_time;

/* pause/continue rosbag */
void pauseCallback(const std_msgs::Int32::ConstPtr& msg)
{
    paused = msg->data;

    ROS_INFO("Rosbag: %i",paused);
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "rosbag_node");
    ros::NodeHandle nh_;
    image_transport::ImageTransport it_(nh_);
    //ros::param::get("~folder", folder);
    if(argc < 2){
        ROS_INFO("No argument with path to rosbag.");
        return 1;
    }
    folder=argv[1];

    ros::Subscriber bag_sub = nh_.subscribe("/rosbag/pause", 1, pauseCallback);

    /* to publish messages from rosbag */
    ros::Publisher cam_info_pub_ = nh_.advertise<sensor_msgs::CameraInfo>("/stereo/left/camera_info",1);
    ros::Publisher cam_param_pub_ = nh_.advertise<dynamic_reconfigure::Config>("/stereo/tara_camera/parameter_updates",1);
    image_transport::Publisher cam_img_pub_ = it_.advertise("/stereo/left/image_raw",1);
    ros::Publisher odom_pub_ = nh_.advertise<nav_msgs::Odometry>("/odom",1);
    /* messages for publishers */
    sensor_msgs::CameraInfo::ConstPtr msg_info;
    sensor_msgs::ImageConstPtr msg_img;
    dynamic_reconfigure::Config::ConstPtr msg_param;
    nav_msgs::Odometry::ConstPtr msg_odom;

    /* open rosbag */
    rosbag::Bag bag;
    ROS_INFO("Opening rosbag %s",folder.c_str());
    bag.open(folder.c_str(), rosbag::bagmode::Read);
    /* add tracked topics */
    std::vector<std::string> topics;
    topics.push_back(std::string("/stereo/left/camera_info"));
    topics.push_back(std::string("/stereo/tara_camera/parameter_updates"));
    topics.push_back(std::string("/stereo/left/image_raw"));
    topics.push_back(std::string("/odom"));
    rosbag::View view(bag, rosbag::TopicQuery(topics));

    ROS_INFO("Rosbag init done. ");

    /* iterate all messages from tracked topics */
    BOOST_FOREACH(rosbag::MessageInstance const m, view)
    {
        /* repeating last message till request to continue */
        do{
            /* if message from the specified topic, then create this message and publish it next */
            if ( m.getTopic() == "/stereo/left/camera_info" ) {
                msg_info = m.instantiate<sensor_msgs::CameraInfo>();
                if (msg_info != NULL) cam_info_pub_.publish(msg_info);
                /* readable format of time */
                ros::Time timestamp = msg_info->header.stamp;
                boost::posix_time::ptime my_posix_time = timestamp.toBoost();
                std::string iso_time_str = boost::posix_time::to_iso_extended_string(my_posix_time);
                current_time = iso_time_str;
                ROS_INFO("/stereo/left/camera_info timestamp: %s",current_time.c_str());
            }
            if (m.getTopic() == "/stereo/left/image_raw") {
                /* pause rosbag on images to repeat publishing them */
                paused = true;
                ROS_INFO("/stereo/left/image_raw");
                msg_img = m.instantiate<sensor_msgs::Image>();
                if (msg_img != NULL) cam_img_pub_.publish(msg_img);
            }
            if (m.getTopic() == "/stereo/tara_camera/parameter_updates") {
                ROS_INFO("/stereo/tara_camera/parameter_updates");
                msg_param = m.instantiate<dynamic_reconfigure::Config>();
                if(msg_param != NULL) cam_param_pub_.publish(msg_param);
            }
            if (m.getTopic() == "/odom") {
                ROS_INFO("/odom");
                msg_odom = m.instantiate<nav_msgs::Odometry>();
                if(msg_odom != NULL) odom_pub_.publish(msg_odom);
            }
            ros::spinOnce();

        }while(paused && ros::ok());
        if(!ros::ok()){
            ROS_INFO("Rosbag interrupted. ");
            break;
        }

        ROS_INFO("Rosbag publishes next message. ");
    }

    bag.close();

    return 0;
}


