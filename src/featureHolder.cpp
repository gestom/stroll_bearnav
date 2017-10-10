#include <ros/ros.h>
#include <stdio.h>
#include <iostream>

#include <stroll_bearnav/FeatureArray.h>
#include <stroll_bearnav/featureExtractionConfig.h>

#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>
#include <opencv2/opencv.hpp>

#include <std_msgs/Int32.h>

using namespace cv;
using namespace std;

/* variables for saving and publishing features  */
vector<KeyPoint> keypoints;
Mat descriptors;
stroll_bearnav::FeatureArray featureArray;
stroll_bearnav::Feature feature;
KeyPoint keypoint;
ros::Publisher feat_pub_;
float distance_nav;
string id;
int targetKeypoints;
int started = false;

/* image publishing */
Mat img;
image_transport::Publisher image_pub_;
cv_bridge::CvImagePtr cv_ptr;

void startCallback(const std_msgs::Int32::ConstPtr& msg)
{
    started = msg->data;

    ROS_INFO("Feature holder started: %i",started);

    /* publish image features */
    if(started) {
        /* form the message with desired number of features */
        featureArray.feature.clear();
        for(int i=0;i<keypoints.size();i++){
            feature.x=keypoints[i].pt.x;
            feature.y=keypoints[i].pt.y;
            feature.size=keypoints[i].size;
            feature.angle=keypoints[i].angle;
            feature.response=keypoints[i].response;
            feature.octave=keypoints[i].octave;
            feature.class_id=keypoints[i].class_id;
            descriptors.row(i).copyTo(feature.descriptor);
            if(i < targetKeypoints) featureArray.feature.push_back(feature); else break;
        }
        featureArray.id =  id;
        featureArray.distance = distance_nav;

        ROS_INFO("Published feature array size %ld, targetkeypoints %i", featureArray.feature.size(), targetKeypoints);
        feat_pub_.publish(featureArray);
        ros::spinOnce();
    }
}

/* clear keypoints when new image published */
void pauseCallback(const std_msgs::Int32::ConstPtr& msg)
{

    keypoints.clear();

    ROS_INFO("Keypoints cleared.");
}

/* save incoming features */
void featureCallback(const stroll_bearnav::FeatureArray::ConstPtr& msg)
{
    ROS_INFO("Feature callback keypoints size: %ld",msg->feature.size());

    if(keypoints.size() < msg->feature.size() ) {
        keypoints.clear();
        descriptors = Mat();

        /*reconstitute features from the incoming message*/
        for (int i = 0; i < msg->feature.size(); i++) {
            keypoint.pt.x = msg->feature[i].x;
            keypoint.pt.y = msg->feature[i].y;
            keypoint.size = msg->feature[i].size;
            keypoint.angle = msg->feature[i].angle;
            keypoint.response = msg->feature[i].response;
            keypoint.octave = msg->feature[i].octave;
            keypoint.class_id = msg->feature[i].class_id;
            keypoints.push_back(keypoint);

            int size = msg->feature[i].descriptor.size();
            Mat mat(1, size, CV_32FC1, (void *) msg->feature[i].descriptor.data());
            descriptors.push_back(mat);
        }
        id = msg->id;
        distance_nav = msg->distance;
    }

    /*publish image with features*/
    if(cv_ptr != NULL)
    {
        /* Show all detected features in image (Red)*/
        drawKeypoints( img, keypoints, img, Scalar(0,0,255), DrawMatchesFlags::DEFAULT );
        /* publish image with features */
        image_pub_.publish(cv_ptr->toImageMsg());
        ROS_INFO("Features extracted %ld %ld",featureArray.feature.size(),keypoints.size());
    }

    ros::spinOnce();
}

/* dynamic reconfigure of feature extractor */
void extractorCallback(const dynamic_reconfigure::Config::ConstPtr& msg)
{
    targetKeypoints = msg->ints[1].value;
    printf("extractorCallback in featureHolder, targetKeypoints %i\n",targetKeypoints);

    /* publish image features */
    if(keypoints.size() > 0 ) {
        /* form the message with desired number of features */
        featureArray.feature.clear();
        for (int i = 0; i < keypoints.size(); i++) {
            feature.x = keypoints[i].pt.x;
            feature.y = keypoints[i].pt.y;
            feature.size = keypoints[i].size;
            feature.angle = keypoints[i].angle;
            feature.response = keypoints[i].response;
            feature.octave = keypoints[i].octave;
            feature.class_id = keypoints[i].class_id;
            descriptors.row(i).copyTo(feature.descriptor);
            if (i < targetKeypoints) featureArray.feature.push_back(feature); else break;
        }
        featureArray.id = id;
        featureArray.distance = distance_nav;

        ROS_INFO("Publishing %ld features", featureArray.feature.size());
        feat_pub_.publish(featureArray);

        ros::spinOnce();
    }
}

/* save incoming image */
void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
    try {
        cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception &e) {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }
    img = cv_ptr->image;
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "feature_holder_node");
    ros::NodeHandle nh_;
    image_transport::ImageTransport it_(nh_);

    ros::Subscriber feat_sub = nh_.subscribe("/features", 1, featureCallback);
    ros::Subscriber ext_sub = nh_.subscribe("/feature_extraction/parameter_updates", 1, extractorCallback);
    feat_pub_ = nh_.advertise<stroll_bearnav::FeatureArray>("/features",1);

    image_transport::Subscriber image_sub_ = it_.subscribe( "/image", 1,imageCallback);
    image_pub_ = it_.advertise("/image_with_features", 1);

    ros::Subscriber fh_sub = nh_.subscribe("/featureHolder/start_img", 1, startCallback);
    ros::Subscriber bag_sub = nh_.subscribe("/rosbag/pause", 1, pauseCallback);

    ros::spin();

}

