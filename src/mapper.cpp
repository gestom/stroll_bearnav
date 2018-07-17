#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <stroll_bearnav/FeatureArray.h>
#include <stroll_bearnav/Feature.h>
#include <std_msgs/Float32.h>
#include <actionlib/server/simple_action_server.h>
#include <stroll_bearnav/mapperAction.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>
#include <stroll_bearnav/SetDistance.h>
#include <stroll_bearnav/NavigationInfo.h>

using namespace std;
using namespace cv;

typedef enum
{
	IDLE,
	PREPARING,
	MAPPING,
	SAVING,
	TERMINATING
}EMappingState;

ros::Publisher cmd_pub_;
ros::Subscriber speed_sub_;
ros::Subscriber featureSub_;
ros::Subscriber distEventSub_;
ros::Subscriber distSub_;
ros::Subscriber infoSub_; 
image_transport::Subscriber image_sub_;
image_transport::Publisher image_pub_;

/* Service for set/reset distance */
ros::ServiceClient client;
stroll_bearnav::SetDistance srv;

/* Action server parameters */
typedef actionlib::SimpleActionServer<stroll_bearnav::mapperAction> Server;
Server *server;
stroll_bearnav::mapperResult result;
stroll_bearnav::mapperFeedback feedback;

/* Image feature variables */
char name[100];
char lastID[100];
string folder,baseName;
Mat img;
vector<Mat> imgQueue;
vector<string> idQueue;
Mat descriptors;
Mat descriptor;
vector<KeyPoint> keypoints;
vector<float> path;
KeyPoint keypoint;
float rating;
vector<float> ratings;

vector<vector<KeyPoint> > keypointsMap;
vector<Mat> descriptorMap;
vector<float> distanceMap;
vector<Mat> imagesMap;
vector<vector<float> > ratingsMap;

/* Feature messages */
stroll_bearnav::FeatureArray featureArray;
stroll_bearnav::Feature feature;

/*joystick input parameters - axes that correspond to forward, turning and flipper speeds*/ 
int stopButton = 2;
int pauseButton = 0;
int linearAxis = 1;
int angularAxis = 0;
int flipperAxis = 4;

/*these constants determine how quickly the robot moves based on the joystick input*/ 
double maxForwardSpeed = 0.2;
double maxAngularSpeed = 0.2;
double maxFlipperSpeed = 0.2;
double maxForwardAcceleration = 0.01;

/*listening to joystick and flipperVelocity, publishing commands*/
ros::Subscriber flipperSub;
ros::Subscriber joy_sub_;
ros::Publisher vel_pub_;

/*state variables - twist is the message that eventually gets to the ROS driver of the robot, other are obvious*/
geometry_msgs::Twist twist,lastTwist;
double forwardAcceleration= 0;
double forwardSpeed = 0;
double flipperSpeed = 0;
double angularSpeed = 0;
double lastForwardSpeed = 0;
double lastFlipperSpeed = 0;
double lastAngularSpeed = 0;
float distanceTotalEvent=0;
float distanceTravelled=0;
float flipperPosition=0;
bool userStop = false;
EMappingState state = IDLE;

/* plastic map parameters*/
int mapChanges=0;
int lastMapChanges=-1;
bool isPlastic=true;
bool isUpdated=false;


void distanceEventCallback(const std_msgs::Float32::ConstPtr& msg);
void featureCallback(const stroll_bearnav::FeatureArray::ConstPtr& msg);

/* select an appropriate image according to feature array ID*/ 
void imageSelect(const char *id)
{
	if (idQueue.size() > 0){
		int imageIndex = idQueue.size()-1;
		for (int i = 0;i<idQueue.size();i++){
			ROS_INFO("IDS: %s %s",id,idQueue[i].c_str());
			if (strcmp(id,idQueue[i].c_str())==0) imageIndex = i;
		}
		imgQueue[imageIndex].copyTo(img);
		imgQueue.clear();
		idQueue.clear();
		strcpy(lastID,id);
	}
}

/* Total distance travelled recieved from the event */ 
void distanceEventCallback(const std_msgs::Float32::ConstPtr& msg)
{
	if(state == MAPPING){
		distanceTotalEvent=msg->data;
		if(!isPlastic) state = SAVING;
	}
}

/*distance currently travelled */
void distanceCallback(const std_msgs::Float32::ConstPtr& msg)
{   
	distanceTravelled=msg->data;
}

/*Assign current image to variable */
void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
	if(state != IDLE){
		cv_bridge::CvImagePtr cv_ptr;
		try
		{
			cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::MONO8);
		}
		catch (cv_bridge::Exception& e)
		{
			ROS_ERROR("cv_bridge exception: %s", e.what());
			return;
		}
		imgQueue.push_back(cv_ptr->image);
		if (state == PREPARING && imgQueue.size() > 10) state = SAVING;
		char tmpID[100];
		sprintf(tmpID,"Image_%09d",msg->header.seq);
		idQueue.push_back(tmpID);
	}	
}

/*Action server */
void executeCB(const stroll_bearnav::mapperGoalConstPtr &goal, Server *serv)
{
	imagesMap.clear();
	keypointsMap.clear();
	descriptorMap.clear();
	distanceMap.clear();
	ratingsMap.clear();

	/* reset distance using service*/
	srv.request.distance = distanceTravelled = distanceTotalEvent = 0;
	userStop = false;
	baseName = goal->fileName;
	path.clear();
	state = PREPARING;
	if (isPlastic) state = SAVING;
	//TODO if plastic, listen to navigator and then terminate mapping

	if (!client.call(srv)) ROS_ERROR("Failed to call service SetDistance provided by odometry_monitor node!");
	while(state == MAPPING || state == SAVING || state == PREPARING){

		/*on preempt request end mapping and save current map */
		if(server->isPreemptRequested() || userStop)
		{
			ROS_INFO("Map complete, flushing maps.");
			while(state == SAVING) usleep(20000);


			/*add last data to the map*/
			imageSelect(lastID);
			imagesMap.push_back(img);
			keypointsMap.push_back(keypoints);
			descriptorMap.push_back(descriptors);
			distanceMap.push_back(distanceTravelled);
			ratingsMap.push_back(ratings);

			/*and flush it to the disk*/
			for (int i = 0;i<distanceMap.size();i++){
				sprintf(name,"%s/%s_%.3f.yaml",folder.c_str(),baseName.c_str(),distanceMap[i]);
				ROS_INFO("Saving map to %s",name);
				FileStorage fs(name,FileStorage::WRITE);
				write(fs, "Image", imagesMap[i]);
				write(fs, "Keypoints", keypointsMap[i]);
				write(fs, "Descriptors",descriptorMap[i]);
				write(fs, "Ratings",ratingsMap[i]);
				fs.release();
			}
			result.fileName=name;

			/*save the path profile as well*/
			sprintf(name,"%s/%s.yaml",folder.c_str(),baseName.c_str());
			ROS_INFO("Saving path profile to %s",name);
			FileStorage pfs(name,FileStorage::WRITE);
			write(pfs, "Path", path);
			pfs.release();
			if (server->isPreemptRequested()) server->setPreempted(result); else  server->setSucceeded(result);
			userStop = false;
			state = TERMINATING;
		}
		usleep(200000);
	}
	/* stop robot at the end of mapping*/
	while(state == TERMINATING){
		ROS_INFO("Mapping complete, stopping robot.");
		usleep(200000);
	}
	forwardSpeed = angularSpeed = flipperSpeed = 0.0;
}

/*receiving joystick data*/
void joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
{    
	angularSpeed = maxAngularSpeed*forwardSpeed*0.5*joy->axes[angularAxis];
	forwardAcceleration = maxForwardAcceleration*joy->axes[linearAxis];;
	flipperSpeed = maxFlipperSpeed*joy->axes[flipperAxis];
	if  (joy->buttons[stopButton] || joy->buttons[pauseButton]) angularSpeed = forwardSpeed = flipperSpeed = 0;
	if  (joy->buttons[stopButton]) userStop = true;
	ROS_DEBUG("Joystick pressed");
} 

/*flipper position -- for stair traverse*/
void flipperCallback(const std_msgs::Float32::ConstPtr& msg)
{
	flipperPosition = msg->data;   
}

/* save features and image recieved from camera as a local map*/
void featureCallback(const stroll_bearnav::FeatureArray::ConstPtr& msg)
{
    if(!isPlastic) {
	ROS_INFO("Features received\n");
        if (state == SAVING) {
            keypoints.clear();
            descriptors.release();
	    ratings.clear();
	    img.release();

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
                rating=msg->feature[i].rating;
                ratings.push_back(rating);
            }

            /*store in memory rather than on disk*/
	    imageSelect(msg->id.c_str());
            imagesMap.push_back(img);
	  
            keypointsMap.push_back(keypoints);
            descriptorMap.push_back(descriptors);
            distanceMap.push_back(distanceTotalEvent);
            ratingsMap.push_back(ratings);
		
            /* publish feedback */
            sprintf(name, "%i keypoints stored at distance %.3f", (int) keypoints.size(), distanceTotalEvent);
            ROS_INFO("%i keypoints stored at distance %.3f", (int) keypoints.size(), distanceTotalEvent);
            state = MAPPING;
            feedback.fileName = name;
            server->publishFeedback(feedback);
        }
    }
}



void infoMapMatch(const stroll_bearnav::NavigationInfo::ConstPtr& msg)
{
	isUpdated = msg->updated;

	if(isPlastic && isUpdated) {
		/*plastic map saves only map with rating*/
		state = SAVING;

		lastMapChanges = mapChanges;
		mapChanges = msg->mapChanges;

		if (state == SAVING) {
			keypoints.clear();
			descriptors.release();
			ratings.clear();
			img.release();

			//rating
			for (int i = 0; i < msg->map.feature.size(); i++) {
				stroll_bearnav::Feature feature = msg->map.feature[i];
				keypoint.pt.x = feature.x;
				keypoint.pt.y = feature.y;
				keypoint.size = feature.size;
				keypoint.angle = feature.angle;
				keypoint.response = feature.response;
				keypoint.octave = feature.octave;
				keypoint.class_id = feature.class_id;
				keypoints.push_back(keypoint);

				int size = feature.descriptor.size();
				Mat mat(1, size, CV_32FC1, (void *) feature.descriptor.data());
				descriptors.push_back(mat);

				rating = feature.rating;
				ratings.push_back(rating);
			}
			//cout << "mapper: first " << msg->map.feature[0].rating << " x " << msg->map.feature[0].x << " last " << msg->map.feature[msg->map.feature.size()-1].rating  << " x " << msg->map.feature[msg->map.feature.size()-1].x  << endl;


			/*store in memory rather than on disk*/
			imageSelect(msg->view.id.c_str());
			imagesMap.push_back(img);
			keypointsMap.push_back(keypoints);
			descriptorMap.push_back(descriptors);
			distanceMap.push_back(msg->map.distance);
			ratingsMap.push_back(ratings);

			/* publish feedback */
			sprintf(name, "%i keypoints stored at distance %.3f", (int) keypoints.size(), distanceTotalEvent);
			ROS_INFO("%i keypoints stored at distance %.3f", (int) keypoints.size(), distanceTotalEvent);
			state = MAPPING;
			feedback.fileName = name;
			server->publishFeedback(feedback);
		}
	}
}

int main(int argc, char** argv)
{ 
	ros::init(argc, argv, "mapper");
	ros::NodeHandle nh("~");
	image_transport::ImageTransport it_(nh);
	ros::param::get("~folder", folder);
	/* joystick params */
	nh.param("axis_linear", linearAxis, 1);
	nh.param("axis_angular", angularAxis, 0);
	nh.param("axis_flipper", flipperAxis, 4);
	nh.param("stopButton", stopButton, 2);
	nh.param("pauseButton", pauseButton, 0);

	/* robot speed limits */
	nh.param("angularSpeed", maxAngularSpeed, 0.5);
	nh.param("forwardSpeed", maxForwardSpeed, 1.5);
	nh.param("flipperSpeed", maxFlipperSpeed, 0.5);
	nh.param("forwardAcceleration", maxForwardAcceleration, 0.01);

	if (isPlastic == false) vel_pub_ = nh.advertise<geometry_msgs::Twist>("/cmd", 1);
	flipperSub = nh.subscribe("/flipperPosition", 1, flipperCallback);
	joy_sub_ = nh.subscribe<sensor_msgs::Joy>("/joy", 10, joyCallback);
	infoSub_ = nh.subscribe("/navigationInfo", 1000, infoMapMatch);

	image_sub_ = it_.subscribe( "/image", 1,imageCallback);					//THIS IS A PROBLEM WHEN GENERATING GROUND TRUTH
	if(!isPlastic) featureSub_ = nh.subscribe<stroll_bearnav::FeatureArray>("/features",1,featureCallback);
	distEventSub_=nh.subscribe<std_msgs::Float32>("/distance_events",1,distanceEventCallback);
	distSub_=nh.subscribe<std_msgs::Float32>("/distance",1,distanceCallback);
	cmd_pub_ = nh.advertise<geometry_msgs::Twist>("/cmd",1);
	ROS_INFO( "Map folder is: %s", folder.c_str());

	/* Initiate action server */
	server = new Server (nh, "/mapper", boost::bind(&executeCB, _1, server), false);
	server->start();

	/* Initiate service */
	client = nh.serviceClient<stroll_bearnav::SetDistance>("/setDistance");

	path.clear();
	while (ros::ok()){
		if (state == MAPPING)
		{   /* speed limits */
			forwardSpeed += forwardAcceleration;
			forwardSpeed = fmin(fmax(forwardSpeed,-maxForwardSpeed),maxForwardSpeed);
			twist.linear.x =  forwardSpeed;
			angularSpeed = fmin(fmax(angularSpeed,-maxAngularSpeed),maxAngularSpeed);
			twist.angular.z =  angularSpeed;;
			flipperSpeed = fmin(fmax(flipperSpeed,-maxFlipperSpeed),maxFlipperSpeed);
			twist.angular.y =  flipperSpeed;
			if (isPlastic == false) vel_pub_.publish(twist);

			/* saving path profile */
			if (lastForwardSpeed != forwardSpeed || lastAngularSpeed != angularSpeed || lastFlipperSpeed != flipperSpeed)
			{
				path.push_back(distanceTravelled);
				path.push_back(forwardSpeed);
				path.push_back(angularSpeed);
				path.push_back(flipperSpeed);
				//printf("%.3f %.3f %.3f %.3f\n",distanceTravelled,forwardSpeed,angularSpeed,flipperSpeed);
			}
			lastForwardSpeed = forwardSpeed;
			lastAngularSpeed = angularSpeed;
			lastFlipperSpeed = flipperSpeed;
		}
		if (state == TERMINATING){
			twist.linear.x = twist.angular.z = twist.angular.y = 0;
			if (isPlastic == false) vel_pub_.publish(twist);
			state = IDLE;
		}
		ros::spinOnce();
		usleep(50000);
	}
	return 0;
}

