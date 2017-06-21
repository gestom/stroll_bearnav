#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include "CTimer.h" 
#include <iostream>
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
vector<KeyPoint> keypoints_1, keypoints_2;
Mat descriptors_1, descriptors_2;
Mat img_matches, img_t1,img_t2,img_matchestr,img_keypoints_1;
class ImageConverter
{
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;
  Mat img_2;
public:
  ImageConverter()
    : it_(nh_)
  {
    // Subscrive to input video feed and publish output video feed
    image_sub_ = it_.subscribe("/output_video", 1,
      &ImageConverter::imageCb, this);
    image_pub_ = it_.advertise("/image_converter/output_video", 1);

    cv::namedWindow(OPENCV_WINDOW);
  }

  ~ImageConverter()
  {
    cv::destroyWindow(OPENCV_WINDOW);
  }
 
  void imageCb(const sensor_msgs::ImageConstPtr& msg)
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
//	Mat img_t1;
//	transpose(cv_ptr->image,img_t1);
	if(tenCounter % 10==0){
	CTimer timer;
	timer.reset();	
	timer.start();	
	if(start){
    img_2=cv_ptr->image;
	start=false;
	detector->detectAndCompute(img_2, Mat (), keypoints_2,descriptors_2);
	} 
	img_t1=cv_ptr->image;
	//transpose(cv_ptr->image,img_t1);
	//transpose(img2,img_t2);

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
  	if (num>0 && keypoints_1.size() >0&& keypoints_2.size() >0)drawMatches( img_t1, keypoints_1, img_2, keypoints_2, good_matches, img_matches, Scalar::all(-1), Scalar::all(-1), vector<char>(), 2 );
//	cv::transpose(img_matches,img_matchestr);
	
    vector<Point2f> matched_points1;
    vector<Point2f> matched_points2;
    int count=0,bestc=0;
    Point2f current;
   	Point2f best, possible;


	//ransac 
	for (int i=0;i<num;i++){

	    int idx2=good_matches[i].trainIdx;
   		int idx1=good_matches[i].queryIdx;
    	matched_points1.push_back(keypoints_1[idx1].pt);
    	matched_points2.push_back(keypoints_2[idx2].pt);
   		current.x=round(matched_points1[i].x-matched_points2[i].x);	
    	current.y=round(matched_points1[i].y-matched_points2[i].y);
    	int length=sqrt(pow(matched_points2[i].x-matched_points1[i].x,2)+pow(matched_points2[i].y-matched_points1[i].y,2));
    cout << "Matched_point1 " << matched_points1[i] <<  "Matched_point2 " << matched_points2[i] <<"length: " << length << endl;
		cout <<"Matched points size: " << matched_points1.size() << endl;
		for(int j=0;j<num-1;j++){
    	
        	possible.x=round(matched_points1[j].x-matched_points2[j].x);	
        	possible.y=round(matched_points1[j].y-matched_points2[j].y);
			//cout << "Possible" << possiblex << endl;	
			if (current.x==	possible.x && current.y==possible.y){
     			count++;
				
			}
		}
  
    	if(count>bestc){
			best=current;
			bestc=count;		
		}
 
		count=0; 
	}
		
	cout << "Counter: " << bestc << endl;
    cout << "Vektor: " << best.x << " " << best.y << endl;
    // Update GUI Window
	cv::imshow(OPENCV_WINDOW, img_matches);
    cv::waitKey(1);
}
	//  if (cv_ptr->image.rows > 60 && cv_ptr->image.cols > 60)
     // cv::circle(cv_ptr->image, cv::Point(50, 50), 10, CV_RGB(255,0,0));
	img_2=cv_ptr->image;
	keypoints_2=keypoints_1;
	descriptors_2=descriptors_1;
	

    // Output modified video stream
    image_pub_.publish(cv_ptr->toImageMsg());
  } tenCounter++;
}
};


int main(int argc, char** argv)
{
  ros::init(argc, argv, "image_converter");
  ImageConverter ic;
  ros::spin();
  return 0;
}
