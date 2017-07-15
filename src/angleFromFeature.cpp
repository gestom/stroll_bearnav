#include <ros/ros.h>
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
#include <cmath>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/features2d.hpp>
using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;
static const std::string OPENCV_WINDOW = "Image window";

Ptr<SURF> detector = SURF::create(100);
bool start=true;
int tenCounter=10;
vector<KeyPoint> keypoints_1, keypoints_2,keypoints_3;
Mat descriptors_1, descriptors_2,descriptors_3;
Mat img_matches, img_t1,img_t2,img_matchestr,img_keypoints_1,img_3;

ros::Publisher cmd_pub_;
ros::Subscriber featureSub_;
ros::Subscriber loadFeatureSub_;
geometry_msgs::Twist twist;
nav_msgs::Odometry odometry;
image_transport::Subscriber image_sub_;
image_transport::Publisher image_pub_;
Mat img_2;
KeyPoint keypoint,keypoint2;
double pointDist;
int totalDist;
double startx,starty,currentx,currenty,pointx,pointy;
int meter=-1;
char filename[100];
stroll_bearnav::FeatureArray featureArray;
stroll_bearnav::Feature feature; 

void loadFeatureCallback(const stroll_bearnav::FeatureArray::ConstPtr& msg)
{	 
	ROS_INFO("Received new map");
	keypoints_1.clear();
	descriptors_1=Mat();

	for(int i=0; i<msg->feature.size();i++){

		keypoint.pt.x=msg->feature[i].x;
		keypoint.pt.y=msg->feature[i].y;
		keypoint.size=msg->feature[i].size;
		keypoint.angle=msg->feature[i].angle;
		keypoint.response=msg->feature[i].response;
		keypoint.octave=msg->feature[i].octave;
		keypoint.class_id=msg->feature[i].class_id;
		keypoints_1.push_back(keypoint);
		int size=msg->feature[i].descriptor.size();
		Mat mat(1,size,CV_32FC1,(void*)msg->feature[i].descriptor.data());
		descriptors_1.push_back(mat);
	}
}
 

void featureCallback(const stroll_bearnav::FeatureArray::ConstPtr& msg)
{

	keypoints_2.clear();
	descriptors_2=Mat();

	for(int i=0; i<msg->feature.size();i++){

		keypoint.pt.x=msg->feature[i].x;
		keypoint.pt.y=msg->feature[i].y;
		keypoint.size=msg->feature[i].size;
		keypoint.angle=msg->feature[i].angle;
		keypoint.response=msg->feature[i].response;
		keypoint.octave=msg->feature[i].octave;
		keypoint.class_id=msg->feature[i].class_id;
		keypoints_2.push_back(keypoint);
		int size=msg->feature[i].descriptor.size();
		Mat mat(1,size,CV_32FC1,(void*)msg->feature[i].descriptor.data());
		descriptors_2.push_back(mat);


	}

	std::vector< DMatch > good_matches;
	//	detector->detectAndCompute(img_2, Mat (), keypoints_2,descriptors_2);
	//detector->detectAndCompute(img_t1, Mat (), keypoints_1,descriptors_1);
	//printf("Get Time %i\n",timer.getTime());	
	//	detector->detect(img_t1,keypoints_1);
	//	detector->detect(img_t2,keypoints_2);
	//	descriptor->compute(img_t1,keypoints_1,descriptors_1);
	//	descriptor->compute(img_t2,keypoints_2,descriptors_2);
	//	drawKeypoints( img_t1, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT );
	//	transpose(img_keypoints_1,img_t1);
	//	imshow("Keypoints 1", img_t1 );
	float differenceRot=0;
	if (keypoints_1.size() >0 && keypoints_2.size() >0){
		//FlannBasedMatcher matcher;
		//    DescriptorMatcher matcher(NORM_HAMMING,true);
		Ptr<DescriptorMatcher> matcher = BFMatcher::create(NORM_L2);
		vector< vector<DMatch> > matches;

		matcher->knnMatch( descriptors_1, descriptors_2, matches, 2);
		double max_dist = 0; double min_dist = 100;
		//-- Quick calculation of max and min distances between keypoints
		good_matches.reserve(matches.size());  
		for (size_t i = 0; i < matches.size(); ++i)
		{ 
			if (matches[i][0].distance < 0.7*matches[i][1].distance) good_matches.push_back(matches[i][0]);
		}


		int num= good_matches.size();
		vector<Point2f> matched_points1;
		vector<Point2f> matched_points2;
		int count=0,bestc=0;
		Point2f current;
		Point2f best, possible;
		int numBins = 21;
		int histogram[numBins];
		int granularity = 20;
		int differences[num];
		std::vector< DMatch > best_matches;
		for (int i = 0;i<numBins;i++) histogram[i] = 0;
		//ransac 
		for (int i=0;i<num;i++){

			int idx2=good_matches[i].trainIdx;
			int idx1=good_matches[i].queryIdx;
			matched_points1.push_back(keypoints_1[idx1].pt);
			matched_points2.push_back(keypoints_2[idx2].pt);
			current.x=round(matched_points1[i].x-matched_points2[i].x);	
			current.y=round(matched_points1[i].y-matched_points2[i].y);
			int length=sqrt(pow(matched_points2[i].x-matched_points1[i].x,2)+pow(matched_points2[i].y-matched_points1[i].y,2));
			//	cout << "Matched_point1 " << matched_points1[i] <<  "Matched_point2 " << matched_points2[i] <<"length: " << length << endl;
			//	cout <<"Matched points size: " << matched_points1.size() << endl;
			/*for(int j=0;j<num-1;j++){

			  possible.x=round(matched_points1[j].x-matched_points2[j].x);	
			  possible.y=round(matched_points1[j].y-matched_points2[j].y);
			//cout << "Possible" << possiblex << endl;	
			if (current.x==	possible.x && current.y==possible.y){
			count++;

			}
			}*/
			int difference = current.x;
			int index = (difference+granularity/2)/granularity + numBins/2;
			if (fabs(current.y) > 50){
				differences[i] = -1000000;
			}else{
				differences[i] = difference;
				if (index <= 0) index = 0;
				if (index >= numBins) index = numBins-1;
				histogram[index]++;
			}
			count=0; 
		}
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
		int rotation=(position-numBins/2)*granularity;
		printf("\n");
		float sum=0;
		//cout << "Rotation: "<< rotation << endl;
		//cout << "Position: "<< position << endl;
		for(int i=0;i<num;i++){
			if (fabs(differences[i]-rotation) < granularity*1.5){
				sum+=differences[i];
				count++;
				best_matches.push_back(good_matches[i]);
			}
		}
		//cout << "Difference Rotation: "<< sum/count << endl;
		differenceRot=sum/count;
		// TODO najit maximum histogramu a prevezt do realnych souradnic hx,hy
		// TODO najit vsechny body blizke (v sousednich chlivkach) hx,hy a spocitat jejich prumernou odchylku
		// TODO dat tyto body to best matches a zobrazit jen best matches 
		// TODO publikovat na topicu 

		//cout << "Counter: " << bestc << endl;
		cout << "Vektor: " << best.x << " " << best.y << endl;

	}
	twist.linear.x = twist.linear.y = twist.linear.z = 0.0;
	twist.linear.x = 0.1; 
	twist.angular.y = twist.angular.x = 0.0;

	// Output modified video stream
	twist.angular.z=differenceRot*0.0001;
	cmd_pub_.publish(twist);
	//image_pub_.publish(cv_ptr->toImageMsg());

}



int main(int argc, char** argv)
{ 
  ros::init(argc, argv, "angle_from_features");
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_(nh_);
  cmd_pub_ = nh_.advertise<geometry_msgs::Twist>("cmd",1);
  featureSub_ = nh_.subscribe( "/features", 1,featureCallback);
  loadFeatureSub_ = nh_.subscribe("/load/features", 1,loadFeatureCallback);
  ros::spin();
  return 0;
}
