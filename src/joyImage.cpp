#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <geometry_msgs/Twist.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include "CTimer.h"
#include <iostream>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
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
ros::Subscriber odometrySub;
geometry_msgs::Twist twist;
nav_msgs::Odometry odometry;
image_transport::Subscriber image_sub_;
image_transport::Publisher image_pub_;
Mat img_2;
double pointDist;
int totalDist;
double startx,starty,currentx,currenty,pointx,pointy;
int meter=-1;
char filename[100];

void odomcallback(const nav_msgs::Odometry::ConstPtr& msg){
	if(meter==-1){
	startx=msg->pose.pose.position.x;
	starty=msg->pose.pose.position.y;
	pointx=startx;
	pointy=starty;
	meter=0;	
	}	
	currentx=msg->pose.pose.position.x;
	currenty=msg->pose.pose.position.y;
	
	pointDist = sqrt(pow(currentx-pointx,2)+pow(currenty-pointy,2));
	
	if(pointDist>1) {
		totaldistance+=pointDistance;
		pointx=currentx;
		pointy=currenty;
			
	  }
	 

}

void loadImage(string file){

	FileStorage fs2(file, FileStorage::READ);
	if(fs2.isOpened()){
		fs2["keypoints"]>>keypoints_3;
		fs2["descriptors"]>>descriptors_3;
		fs2["Image"]>>img_3;

		if(keypoints_3.size() > 0 && descriptors_3.rows > 0 && keypoints_3.size() == descriptors_3.rows && img_3.rows>0){
			keypoints_2=keypoints_3;
			descriptors_2=descriptors_3;
			img_2=img_3.clone();
			fs2.release();
			start=false;
		}
	}
}
void imageCallback(const sensor_msgs::ImageConstPtr& msg)
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
	
	CTimer timer;
	timer.reset();	
	timer.start();	
	transpose(cv_ptr->image,img_t1);
	if(start){
    img_2=img_t1.clone();
	start=false;
	detector->detectAndCompute(img_2, Mat (), keypoints_2,descriptors_2);
	}
	if(pointDist>1){
	sprintf(Filename,%s_
	} 

  	std::vector< DMatch > good_matches;
//	detector->detectAndCompute(img_2, Mat (), keypoints_2,descriptors_2);
	detector->detectAndCompute(img_t1, Mat (), keypoints_1,descriptors_1);
	printf("Get Time %i\n",timer.getTime());	
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
		Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(NORM_HAMMING);
		vector< vector<DMatch> > matches;

		printf("Demo\n");
		matcher->knnMatch( descriptors_1, descriptors_2, matches, 2);
		printf("Demo\n");
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
			int difference = current.y;
			int index = (difference+granularity/2)/granularity + numBins/2;
			if (current.x > 50){
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
		cout << "Rotation: "<< rotation << endl;
		cout << "Position: "<< position << endl;
		for(int i=0;i<num;i++){
			if (fabs(differences[i]-rotation) < granularity*1.5){
				sum+=differences[i];
				count++;
				best_matches.push_back(good_matches[i]);
			}
		}
		cout << "Difference Rotation: "<< sum/count << endl;
		differenceRot=sum/count;
		// TODO najit maximum histogramu a prevezt do realnych souradnic hx,hy
		// TODO najit vsechny body blizke (v sousednich chlivkach) hx,hy a spocitat jejich prumernou odchylku
		// TODO dat tyto body to best matches a zobrazit jen best matches 
		// TODO publikovat na topicu 

		cout << "Counter: " << bestc << endl;
		cout << "Vektor: " << best.x << " " << best.y << endl;

		if (num>0 && keypoints_1.size() >0&& keypoints_2.size() >0) drawMatches( img_t1, keypoints_1, img_2, keypoints_2, best_matches, img_matches, Scalar(255,0,0), Scalar::all(-1), vector<char>(), 2 );
			cv::transpose(img_matches,img_matchestr);

		// Update GUI Window
		cv::imshow(OPENCV_WINDOW, img_matchestr);
		int key = cv::waitKey(1);
		printf("KOD: %i\n",key);
		
		if (key == 32)
		{
			img_2=img_t1.clone();
			keypoints_2=keypoints_1;
			descriptors_2=descriptors_1;
			FileStorage fs("/home/parallels/catkin_ws/src/cameleon_ros_driver/features.yaml", FileStorage::WRITE);
			
			write(fs, "Image", img_2);
			fs << "keypoints" << keypoints_1;
			fs <<  "descriptors" << descriptors_1;
			fs.release();
		}
	}
	//  if (cv_ptr->image.rows > 60 && cv_ptr->image.cols > 60)
     // cv::circle(cv_ptr->image, cv::Point(50, 50), 10, CV_RGB(255,0,0));
	
 /*	if (meter){
			
			write(fs, "keypoints: ", keypoints_1);
			write(fs, "descriptors: ", descriptors_1);
	
	}*/

	twist.linear.x = twist.linear.y = twist.linear.z = 0.0;
	twist.angular.y = twist.angular.x = 0.0;

    // Output modified video stream
	twist.angular.z=-differenceRot*0.0001;
	cmd_pub_.publish(twist);
    image_pub_.publish(cv_ptr->toImageMsg());

}



int main(int argc, char** argv)
{ 
  ros::init(argc, argv, "image_converter");
  ros::NodeHandle nh_;
  loadImage("/home/parallels/catkin_ws/src/cameleon_ros_driver/features.yaml");
  image_transport::ImageTransport it_(nh_);
  cmd_pub_ = nh_.advertise<geometry_msgs::Twist>("cmd",1);
  image_sub_ = it_.subscribe( "/usb_cam/image_raw", 1,imageCallback);
  image_pub_ = it_.advertise("/image_converter/output_video", 1);
  odometrySub = nh_.subscribe<nav_msgs::Odometry>("/odom",10 ,odomcallback);
  ros::spin();
  return 0;
}
