#include <ros/ros.h>
#include <std_msgs/Float32.h>
#include <image_transport/image_transport.h>
#include <geometry_msgs/Twist.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <iostream>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
#include <stroll_bearnav/FeatureArray.h>
#include <stroll_bearnav/Feature.h>
#include <stroll_bearnav/PathProfile.h>
#include <cmath>
#include <std_msgs/Float32.h>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/features2d.hpp>
#include <stroll_bearnav/navigatorAction.h>
#include <stroll_bearnav/SetDistance.h>
#include <actionlib/server/simple_action_server.h>
#include <dynamic_reconfigure/server.h>
#include <stroll_bearnav/navigatorConfig.h>
#include <stroll_bearnav/NavigationInfo.h>
#include <nn_matcher/NNImageMatching.h>
#include <string>

using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;
static const std::string OPENCV_WINDOW = "Image window";


vector<Point2f> matched_points1;
vector<Point2f> matched_points2;
vector< vector<DMatch> > matches,revmatches;
vector< DMatch > good_matches;
vector< DMatch > best_matches;
vector< DMatch > bad_matches;
stroll_bearnav::NavigationInfo info;
ros::Publisher cmd_pub_;
ros::Publisher info_pub_;
ros::Subscriber featureSub_;
ros::Subscriber loadFeatureSub_;
ros::Subscriber speedSub_;
ros::Subscriber distSub_;
ros::Subscriber distEventSub_;
image_transport::Subscriber image_sub_;
image_transport::Subscriber image_map_sub_;
image_transport::Publisher image_pub_;

/* Service for set/reset distance */
stroll_bearnav::SetDistance srv;
ros::ServiceClient client, client2;

/* Action server */
typedef actionlib::SimpleActionServer<stroll_bearnav::navigatorAction> Server;
Server *server;
stroll_bearnav::navigatorResult result;
stroll_bearnav::navigatorFeedback feedback;
stroll_bearnav::FeatureArray mapFeatures;

bool showAllMatches=true;
bool showGoodMatches=true;
int numFeatureAdd = 50;
int numFeatureRemove = 50;

geometry_msgs::Twist twist;
nav_msgs::Odometry odometry;

/* Image features parameters */
Ptr<DescriptorMatcher> matcher;
vector<KeyPoint> mapKeypoints, currentKeypoints,keypointsGood,keypointsBest;
Mat mapDescriptors, currentDescriptors;
Mat img_goodKeypoints_1,currentImage,mapImage;
KeyPoint keypoint,keypoint2;
float ratioMatchConstant = 0.7;
int currentPathElement = 0;
float currentDistance = 0;
int minGoodFeatures = 2;
float pixelTurnGain = 0.001;
float pixelTurnGainInt = 0;
float differenceRot=0;
float differenceRotInt=0;
float minimalAdaptiveSpeed = 1.0;
float maximalAdaptiveSpeed = 1.0;
float maximalCurvature = 1.0;
bool imgShow;
NormTypes featureNorm = NORM_INF;
int descriptorType = CV_32FC1;

/* Feature message */
stroll_bearnav::FeatureArray featureArray;
stroll_bearnav::Feature feature;

/*PID Control*/
float PID_Kp, PID_Ki, PID_Kd;
float error_accumlation;
float last_error;
std::vector<float> offset_queue;
ros::Publisher kp_pub, ki_pub, control_output;

/*Testing Log*/
bool write_log=false;
string folder;
std::vector<float> log_distances;
std::vector<float> log_offsets;
std::vector<int> log_num_inliners;
 
typedef struct
{
	float distance;
	float forward;
	float angular;
	float flipper;
}SPathElement;

typedef enum
{
	IDLE,
	NAVIGATING,
	PREEMPTED,
	COMPLETED
}ENavigationState;

ENavigationState state = IDLE;
vector<SPathElement> path;
float overshoot = 0;
double velocityGain=0;
int maxVerticalDifference = 0;

/* Map features ratings parameters */
bool isRating = false;
int mapChanges=0;

/* Total distance travelled recieved from the event */
void distanceEventCallback(const std_msgs::Float32::ConstPtr& msg)
{
    isRating=true;
}

void pathCallback(const stroll_bearnav::PathProfile::ConstPtr& msg)
{
	SPathElement a;
	path.clear();
	/* save path profile to variable */
	for (int i = 0;i<msg->distance.size();i++)
	{
		a.distance = msg->distance[i];
		a.forward = msg->forwardSpeed[i];
		a.angular = msg->angularSpeed[i];
		a.flipper = msg->flipper[i];
		path.push_back(a);
	}
	//for (int i = 0;i<path.size();i++) printf("%.3f %.3f %.3f %.3f\n",path[i].distance,path[i].forward,path[i].angular,path[i].flipper);
}

/* dynamic reconfigure of showing images, velocity gain and matching ratio constant */
void callback(stroll_bearnav::navigatorConfig &config, uint32_t level)
{
	showAllMatches=config.showAllMatches;
	showGoodMatches=config.showGoodMatches;
	velocityGain=config.velocityGain;
	ratioMatchConstant=config.matchingRatio;
	maxVerticalDifference = config.maxVerticalDifference;
	
	minGoodFeatures = config.minGoodFeatures;
	pixelTurnGain = config.pixelTurnGain;
	minimalAdaptiveSpeed = config.adaptiveSpeedMin;
	maximalAdaptiveSpeed = config.adaptiveSpeedMax;
	maximalCurvature = 0;//config.maximalCurvature;
	pixelTurnGainInt = 0;//config.pixelTurnGainInt;
}

/* reference map received */
void loadFeatureCallback(const stroll_bearnav::FeatureArray::ConstPtr& msg)
{	
	mapFeatures = *msg;
	ROS_INFO("Received a new reference map");
	mapKeypoints.clear();
	mapDescriptors.release();
	mapDescriptors = Mat();
	for(int i=0; i<msg->feature.size();i++){
		keypoint.pt.x=msg->feature[i].x;
		keypoint.pt.y=msg->feature[i].y;
		keypoint.size=msg->feature[i].size;
		keypoint.angle=msg->feature[i].angle;
		keypoint.response=msg->feature[i].response;
		keypoint.octave=msg->feature[i].octave;
		keypoint.class_id=msg->feature[i].class_id;
		mapKeypoints.push_back(keypoint);
		int size=msg->feature[i].descriptor.size();
		Mat mat(1,size,CV_32FC1,(void*)msg->feature[i].descriptor.data());
		if (descriptorType != CV_32FC1) mat.convertTo(mat,descriptorType);
		mapDescriptors.push_back(mat);
	}
}

void actionServerCB(const stroll_bearnav::navigatorGoalConstPtr &goal, Server *serv)
{
	state = NAVIGATING;
	int traversals = goal->traversals;
	currentPathElement = 0;
	differenceRotInt = 0;

	/* reset distance using service*/
	srv.request.distance=overshoot=0;
	if (!client.call(srv)) ROS_ERROR("Failed to call service SetDistance provided by odometry_monitor node!");
	result.success = false;
	while(state != IDLE){
		if(server->isPreemptRequested()){
			state = PREEMPTED;
			server->setPreempted(result);
			state = IDLE;
		}
		if (result.success == false && state == COMPLETED)
		{
			if (traversals<=1){
				result.success = true;
				state = IDLE;
				server->setSucceeded(result);
			}else{
				sleep(2);
				srv.request.distance=overshoot;
				if (!client.call(srv)) ROS_ERROR("Failed to call service SetDistance provided by odometry_monitor node!");
				sleep(2);
				currentPathElement = 0;
				state = NAVIGATING;
				traversals--;
			}
		}
	}
	twist.linear.x = twist.linear.y = twist.linear.z = twist.angular.z = twist.angular.y = twist.angular.x = 0.0;	
	cmd_pub_.publish(twist);
}


/* get image from map */
void imageMapCallback(const sensor_msgs::ImageConstPtr& msg)
{

	cv_bridge::CvImagePtr cv_ptr;
	try
	{
		cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
	}
	catch (cv_bridge::Exception& e)
	{
		ROS_ERROR("cv_bridge exception: %s", e.what());
		return;
	}
    cvtColor(cv_ptr->image, mapImage, cv::COLOR_BGR2GRAY);
}

/*to select most rating matches*/
bool compare_rating(stroll_bearnav::Feature first, stroll_bearnav::Feature second)
{
	if (first.rating > second.rating) return true; else return false;
}

float PID(float error) {
    // if(abs(error)<2)    offset_queue.resize(0);
    // if(isnan(error))    error=0;

    // int k = 10000;
    // offset_queue.push_back(error);
    // while(offset_queue.size()>k)    offset_queue.erase(offset_queue.begin());
    // float sum = 0;
    // for(std::vector<float>::iterator it=offset_queue.begin(); it!=offset_queue.end(); ++it)    sum+=*it;
    // int len = offset_queue.size();

    // float delta = 0;
    // if(len > 1)     delta = offset_queue[len-1] - offset_queue[len-2];

    // return max(min(PID_Kp*error + PID_Ki*sum + PID_Kd*delta, float(500)), float(-500));

    if(isnan(error))    error=0;
    if(abs(error)<2)    error_accumlation = 0;

    error_accumlation += error;

    float delta = error - last_error;

    std_msgs::Float32 msg;

    msg.data = PID_Kp*error;
    kp_pub.publish(msg);

    msg.data = PID_Ki*error_accumlation;
    ki_pub.publish(msg);

    msg.data = max(min(PID_Kp*error + PID_Ki*error_accumlation + PID_Kd*delta, float(500)), float(-500));
    control_output.publish(msg);

    return max(min(PID_Kp*error + PID_Ki*error_accumlation + PID_Kd*delta, float(500)), float(-500));
}

void writeLog() {
    /*save the path profile as well*/
    char name[100];
    sprintf(name,"%s/log_%f.yaml", folder.c_str(), ros::Time::now().toSec());
    ROS_INFO("saving test log to %s",name);
    FileStorage pfs(name,FileStorage::WRITE);
    write(pfs, "distance", log_distances);
    write(pfs, "offset", log_offsets);
    write(pfs, "num_inliners", log_num_inliners);
    pfs.release();
    ROS_INFO("done!");
}

void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{;

    //state = NAVIGATING;
    if(state == NAVIGATING){

        currentKeypoints.clear();
        keypointsBest.clear();
        keypointsGood.clear();
        currentDescriptors.release();
        currentDescriptors = Mat();
        good_matches.clear();

        nn_matcher::NNImageMatching srv;

        cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);

        cvtColor(cv_ptr->image, currentImage, cv::COLOR_BGR2GRAY);

        srv.request.image_camera = *(cv_bridge::CvImage(std_msgs::Header(), "mono8", currentImage).toImageMsg());

        //imwrite( "/home/kevin/currentImage"+to_string(time)+".jpg", currentImage);
        
        srv.request.image_map = *(cv_bridge::CvImage(std_msgs::Header(), "mono8", mapImage).toImageMsg());

        //imwrite( "/home/kevin/mapImage" + to_string(time) + ".jpg", mapImage);

        // call feature maching service
        if (client2.call(srv))
        {
            //ROS_INFO("Flag is: %ld", (long int)srv.response.flag);
        }
        else
        {
            ROS_ERROR("Failed to call service to match two images.");
            return;
        }

        good_matches.clear();

        int numBins = srv.response.differences.size();
        int granularity = 20;
        int count = 0;
        int num = srv.response.differences.size();
        int *differences = (int*)calloc(num,sizeof(int));
        int histogram[numBins];
        for (int i = 0;i<numBins;i++) histogram[i] = 0;

        best_matches.clear();
        bad_matches.clear();

        for(int i=0; i<srv.response.differences.size(); i++)
            differences[i] = int(srv.response.differences[i]);

        /*building histogram*/
        for (int i=0;i<num;i++){

            int index = (differences[i]+granularity/2)/granularity + numBins/2;

            if (index >= 0 && index < numBins) histogram[index]++;
        }

        /*histogram printing*/
        int max=0;
        int position=0;
        printf("Bin: ");
        for (int i = 0;i<numBins;i++) {

            printf("%i ",histogram[i]);
            if (histogram[i]>max)
            {
                max=histogram[i];

                position=i;
            }
        }
        feedback.max = max;

        /* rotation between features based on histogram voting */
        int rotation=(position-numBins/2)*granularity;
        printf("\n");
        float sum=0;
        int num_inliners = 0;
        int num_outliners = 0;
        keypointsBest.clear();
        /* use good correspondences to determine heading */
        best_matches.clear();
        bad_matches.clear();
        /* take only good correspondences */
        for(int i=0;i<num;i++){
            if (fabs(differences[i]-rotation) < granularity*1.5){
                sum+=differences[i];
                count++;
                num_inliners++;
                //best_matches.push_back(good_matches[i]);
                //keypointsBest.push_back(keypointsGood[i]);
            } else {
                num_outliners++;
                //bad_matches.push_back(good_matches[i]);
            }
        }
        free(differences);

        /* publish statistics */
        feedback.correct = num_inliners;
        feedback.outliers = num_outliners;
        feedback.keypoints_avg = 0;
        feedback.matches = num;
        /*difference between features */
        differenceRot=sum/count;
        cout << "correct: " << feedback.correct << " out: " << feedback.outliers << " map " << mapKeypoints.size() << " cur " << currentKeypoints.size() << " gm " << feedback.matches << " difference " << differenceRot  << " distance " << feedback.distance << endl;
        //cout << "Vektor: " << count << " " << differenceRot << endl;
        //cout << "bm " << bad_matches.size()  << endl;
        float differenceRot_raw = differenceRot;
        differenceRot = PID(differenceRot);
        ROS_ERROR("PID control, before %f after %f", differenceRot_raw, differenceRot);

        velocityGain = fmin(fmax(count/20.0,minimalAdaptiveSpeed),maximalAdaptiveSpeed);

        feedback.histogram.clear();
        if (count<minGoodFeatures) differenceRot = 0;
        for (int i = 0;i<numBins;i++) feedback.histogram.push_back(histogram[i]);

        /*forming navigation info messsage*/
        //info.mapID = currentMapID;
        info.histogram = feedback.histogram;
        info.ratio = ratioMatchConstant;
        info.mapMatchIndex.clear();
        vector<int> mapIndex(mapFeatures.feature.size());
        vector<int> mapEval(mapFeatures.feature.size());
        std::fill(mapIndex.begin(),mapIndex.end(),-1);
        std::fill(mapEval.begin(),mapEval.end(),0);

        //Output the log
        if(write_log) {
            log_distances.push_back(currentDistance);
            log_offsets.push_back(differenceRot);
            log_num_inliners.push_back(num_inliners);
        }

        // Rating
        isRating = false;

        if(isRating)
        {
            for (int i = 0;i<good_matches.size();i++)
            {
                mapIndex[good_matches[i].queryIdx] = good_matches[i].trainIdx;
                mapEval[good_matches[i].queryIdx] = -1;
            }
            for (int i = 0;i<best_matches.size();i++) {
                mapEval[best_matches[i].queryIdx] = 1;
                // rating map features
                if(isRating) mapFeatures.feature[best_matches[i].queryIdx].rating+=mapEval[best_matches[i].queryIdx];
            }

            for (int i = 0; i < bad_matches.size(); i++) {
                mapFeatures.feature[bad_matches[i].queryIdx].rating += mapEval[bad_matches[i].queryIdx];
            }

            numFeatureAdd = numFeatureRemove = best_matches.size()/2;

            // remove the worst rating from map
            sort(mapFeatures.feature.begin(), mapFeatures.feature.end(), compare_rating);
            if (numFeatureRemove >mapFeatures.feature.size()) numFeatureAdd = numFeatureRemove = mapFeatures.feature.size();
            if (numFeatureRemove > -1){
                mapFeatures.feature.erase(mapFeatures.feature.end() - numFeatureRemove, mapFeatures.feature.end());
            }else{
                mapFeatures.feature.erase(mapFeatures.feature.end() - bad_matches.size(), mapFeatures.feature.end());
            }

            //mapFeatures.feature.clear();
            //numFeatureAdd = 500;
            // add the least similar features from view to map
            for (int i = 0; i < numFeatureAdd && i < info.view.feature.size(); i++) {
                info.view.feature[i].rating = 0;
                info.view.feature[i].x = info.view.feature[i].x + differenceRot;
                mapFeatures.feature.push_back(info.view.feature[i]);
                //info.view.feature.erase(info.view.feature.begin(), info.view.feature.begin() + 10);
            }

            isRating=false;
            mapChanges++;
            info.updated=true;
        }
        info.mapChanges=mapChanges;
        info.map = mapFeatures;
        info.mapMatchIndex = mapIndex;
        info.mapMatchEval = mapEval;
        info.correct = feedback.correct;
        info.matches = feedback.matches;
        info.distance = feedback.distance;
        info.diffRot = differenceRot;
        info_pub_.publish(info);

        /*Show good image features (Green) */
        Mat output,outtran;
        if(image_pub_.getNumSubscribers()>0)
        {
            //drawKeypoints(currentImage,keypointsBest,img_goodKeypoints_1,Scalar(0,255,0), DrawMatchesFlags::DEFAULT );
            if (currentImage.rows >0 && mapKeypoints.size() >0 && currentKeypoints.size() >0)
            {
                if (mapImage.rows==0) mapImage = currentImage;
                Mat mapIm = mapImage.t();
                Mat curIm = currentImage.t();
                vector<KeyPoint> kpMap,kpCur;
                KeyPoint tmp;
                for (int i = 0;i<mapKeypoints.size();i++)
                {
                    tmp = mapKeypoints[i];
                    tmp.pt.y = mapKeypoints[i].pt.x;
                    tmp.pt.x = mapKeypoints[i].pt.y;
                    kpMap.push_back(tmp);
                }
                for (int i = 0;i<currentKeypoints.size();i++)
                {
                    tmp = currentKeypoints[i];
                    tmp.pt.y = currentKeypoints[i].pt.x;
                    tmp.pt.x = currentKeypoints[i].pt.y;
                    kpCur.push_back(tmp);
                }
                if (showAllMatches){
                    drawMatches(mapIm,kpMap,curIm,kpCur,good_matches,outtran,Scalar(0,0,255),Scalar(0,0,255),vector<char>(),0);
                    if (showGoodMatches) drawMatches(mapIm,kpMap,curIm,kpCur,best_matches,outtran,Scalar(0,255,0),Scalar(0,255,0),vector<char>(),3);
                }else{
                    if (showGoodMatches) drawMatches(mapIm,kpMap,curIm,kpCur,best_matches,outtran,Scalar(0,255,0),Scalar(0,255,0),vector<char>(),2);
                }
                output = outtran.t();
                std_msgs::Header header;
                cv_bridge::CvImage bridge(header, sensor_msgs::image_encodings::BGR8, output);
                image_pub_.publish(bridge.toImageMsg());
            }
        }

        /* publish statistics */
        std::vector<int> stats;
        feedback.stats.clear();
        feedback.diffRot = differenceRot;
        stats.push_back(feedback.diffRot);
        feedback.stats.push_back(feedback.diffRot);
        stats.push_back(feedback.keypoints_avg);
        feedback.stats.push_back(feedback.keypoints_avg);
        stats.push_back(feedback.matches);
        feedback.stats.push_back(feedback.matches);
        stats.push_back(feedback.correct);
        feedback.stats.push_back(feedback.correct);
        stats.push_back(feedback.outliers);
        feedback.stats.push_back(feedback.outliers);

        server->publishFeedback(feedback);
    }
}


void distanceCallback(const std_msgs::Float32::ConstPtr& msg)
{	

	if (state == NAVIGATING){
		/* check for end of path profile */
		feedback.distance = currentDistance = msg->data;
		if (currentPathElement+2 <= path.size())
		{
			while (path[currentPathElement+1].distance < msg->data){
				//ROS_INFO("Next %i %f",currentPathElement,path[currentPathElement].forward);
				 currentPathElement++;
			}
		}else{
			/*if not in demo mode*/
			if(path.size() > 0 || showAllMatches == false  && showGoodMatches == false) state = COMPLETED;
		}
		if (path.size()>currentPathElement)
		{
			//ROS_INFO("MOVE %i %f",currentPathElement,path[currentPathElement].forward);
			twist.linear.x = twist.linear.y = twist.linear.z = 0.0;
			twist.angular.z = twist.angular.y = twist.angular.x = 0.0;
			if (fabs(path[currentPathElement].angular) > 0.001) velocityGain = 1.0;
			twist.linear.x = path[currentPathElement].forward*velocityGain; 
			twist.angular.z = path[currentPathElement].angular*velocityGain;
			//differenceRotInt+=differenceRot;
            ROS_INFO("distance %f", path[currentPathElement].distance);
            ROS_INFO("pvelocityGain %f", velocityGain);
            if(path[currentPathElement].angular==0)
                ROS_INFO("%ith  path angular event: %f redian/s", currentPathElement, path[currentPathElement].angular);
            else
                ROS_ERROR("%ith  path angular event: %f redian/s", currentPathElement, path[currentPathElement].angular);

            ROS_INFO("angular vel %f", twist.angular.z);
            ROS_INFO("differenceRot %f", differenceRot);
            ROS_INFO("pixelTurnGain %f", pixelTurnGain);
            
			twist.angular.z+=differenceRot*pixelTurnGain;
			//if (twist.angular.z > +twist.linear.x*maximalCurvature) twist.angular.z  = +twist.linear.x*maximalCurvature; 
			//if (twist.angular.z < -twist.linear.x*maximalCurvature) twist.angular.z  = -twist.linear.x*maximalCurvature; 
 
            // Control the robot
			cmd_pub_.publish(twist);
            
		}
		/*used for testing and demos*/
		if (path.size()==0)
		{
			twist.linear.x = twist.linear.y = twist.linear.z = 0.0;
			twist.linear.y = twist.linear.z = twist.angular.y = twist.angular.x = 0.0;
			twist.angular.z =differenceRot*pixelTurnGain;
			cmd_pub_.publish(twist);
		}
	}
	if (state == COMPLETED){
		if (path.size() > 0) overshoot = msg->data-path[path.size()-1].distance;
		twist.linear.x = twist.linear.y = twist.linear.z = 0.0;
		twist.angular.z = twist.angular.y = twist.angular.x = 0.0;
		cmd_pub_.publish(twist);

        if(write_log) {
            writeLog();
            write_log = false;
        }
	}
}

int main(int argc, char** argv)
{
	ros::init(argc, argv, "navigator");

	ros::NodeHandle nh;
	image_transport::ImageTransport it_(nh);
    ros::param::get("~PID_Kp", PID_Kp);
    ros::param::get("~PID_Ki", PID_Ki);
    ros::param::get("~PID_Kd", PID_Kd);
    ros::param::get("~write_log", write_log);
    ros::param::get("~folder", folder);

	image_sub_ = it_.subscribe( "/image_with_features", 1,imageCallback);
	image_map_sub_ = it_.subscribe( "/map_image", 1,imageMapCallback);
	cmd_pub_ = nh.advertise<geometry_msgs::Twist>("cmd",1);
	info_pub_ = nh.advertise<stroll_bearnav::NavigationInfo>("/navigationInfo",1);
	image_pub_ = it_.advertise("/navigationMatches", 1);

    kp_pub = nh.advertise<std_msgs::Float32>("p_val",1);
    ki_pub = nh.advertise<std_msgs::Float32>("i_val",1);
    control_output = nh.advertise<std_msgs::Float32>("pid_control_output",1);

	//featureSub_ = nh.subscribe( "/features", 1,featureCallback);
	//loadFeatureSub_ = nh.subscribe("/localMap", 1,loadFeatureCallback);
	distSub_=nh.subscribe<std_msgs::Float32>("/distance",1,distanceCallback);
    distEventSub_=nh.subscribe<std_msgs::Float32>("/distance_events",1,distanceEventCallback);
	speedSub_=nh.subscribe<stroll_bearnav::PathProfile>("/pathProfile",1,pathCallback);
  	/* Initiate action server */
	server = new Server (nh, "navigator", boost::bind(&actionServerCB, _1, server), false);
	server->start();

	/* Initiate service */
	client = nh.serviceClient<stroll_bearnav::SetDistance>("setDistance");
    client2 = nh.serviceClient<nn_matcher::NNImageMatching>("NN_Image_Matching");
	/* Initiate dynamic reconfiguration */
	dynamic_reconfigure::Server<stroll_bearnav::navigatorConfig> server;
	dynamic_reconfigure::Server<stroll_bearnav::navigatorConfig>::CallbackType f = boost::bind(&callback, _1, _2);
	server.setCallback(f);

	ros::spin();
	return 0;
}
