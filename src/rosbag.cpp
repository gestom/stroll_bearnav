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
using namespace cv;
//using namespace cv::xfeatures2d;
using namespace std;
Mat img_1;
int paused = false;

//callback pause/play
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

    ros::Subscriber bag_sub = nh_.subscribe("/rosbag/pause", 1, pauseCallback);

    rosbag::Bag bag;
    bag.open("/home/lucie/vision_ws/rosbagy/new_3.bag", rosbag::bagmode::Read);

    std::vector<std::string> topics;
    topics.push_back(std::string("/stereo/left/camera_info"));
    topics.push_back(std::string("/stereo/tara_camera/parater_updates"));
    /*topics.push_back(std::string("/load/path"));
    topics.push_back(std::string("/load/features"));
    topics.push_back(std::string("/navigationInfo"));
    topics.push_back(std::string("/navigator_node/parameter_updates"));
    topics.push_back(std::string("/feature_extraction_node/parameter_updates"));
    topics.push_back(std::string("/navigator/cancel"));*/
    topics.push_back(std::string("/stereo/left/image_raw"));
    topics.push_back(std::string("/odom"));

    ros::Publisher cam_info_pub_ = nh_.advertise<sensor_msgs::CameraInfo>("/stereo/left/camera_info",1);
    ros::Publisher cam_param_pub_ = nh_.advertise<dynamic_reconfigure::Config>("/stereo/tara_camera/parater_updates",1);
    image_transport::Publisher cam_img_pub_ = it_.advertise("/stereo/left/image_raw",1);
    ros::Publisher odom_pub_ = nh_.advertise<nav_msgs::Odometry>("/odom",1);

    rosbag::View view(bag, rosbag::TopicQuery(topics));

    sensor_msgs::CameraInfo::ConstPtr msg_info;
    sensor_msgs::ImageConstPtr msg_img;
    dynamic_reconfigure::Config::ConstPtr msg_param;
    nav_msgs::Odometry::ConstPtr msg_odom;

    ROS_INFO("Rosbag init done. ");

    BOOST_FOREACH(rosbag::MessageInstance const m, view)
    {

        do{
            //ROS_INFO("Rosbag publishes, pause %i ", paused);
            if ( m.getTopic() == "/stereo/left/camera_info" ) {
                msg_info = m.instantiate<sensor_msgs::CameraInfo>();
                if (msg_info != NULL) cam_info_pub_.publish(msg_info);
                std::cout << "camera info timestamp: " << msg_info->header.stamp << std::endl;
            }
            if (m.getTopic() == "/stereo/left/image_raw") {
                paused = true;
                std::cout << "img" << std::endl;
                msg_img = m.instantiate<sensor_msgs::Image>();
                if (msg_img != NULL) {
                    /*cv_bridge::CvImagePtr cv_ptr;
                    try {
                        cv_ptr = cv_bridge::toCvCopy(msg_img, sensor_msgs::image_encodings::BGR8);
                    }
                    catch (cv_bridge::Exception &e) {
                        ROS_ERROR("cv_bridge exception: %s", e.what());
                        return 1;
                    }
                    img_1 = cv_ptr->image;
                    imshow("Rosbag", img_1);
                    waitKey(0);*/
                    cam_img_pub_.publish(msg_img);
                }
            }
            if (m.getTopic() == "/stereo/tara_camera/parater_updates") {
                std::cout << "upd" << std::endl;
                msg_param = m.instantiate<dynamic_reconfigure::Config>();
                if(msg_param != NULL) cam_param_pub_.publish(msg_param);
            }
            if (m.getTopic() == "/odom") {
                std::cout << "odom" << std::endl;
                msg_odom = m.instantiate<nav_msgs::Odometry>();
                if(msg_odom != NULL) odom_pub_.publish(msg_odom);
            }
            //ROS_INFO("Rosbag once");
            ros::spinOnce();

        }while(paused && ros::ok());
        if(!ros::ok()){
            ROS_INFO("Rosbag interrupted. ");
            break;
        }
        //ROS_INFO("Sleep");
        //sleep(1);
        //paused = true;

        ROS_INFO("Rosbag publishes next frame. ");
    }

    bag.close();

    //ros::spin();
    return 0;
}


