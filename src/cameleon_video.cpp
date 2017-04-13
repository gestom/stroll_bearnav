#include <ros/ros.h>
#include <image_transport/image_transport.h>

#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>

using namespace std;
using namespace cv;

VideoCapture *capture;

int imageNumber;
int key;

int main(int argc, char** argv) 
{
	ros::init(argc, argv, "cameleon_camera");
	ros::NodeHandle n = ros::NodeHandle("~");
	image_transport::ImageTransport it(n);
	VideoCapture cap("rtsp://172.43.50.194/axis-media/media.amp?camera=1"); // open the default camera
	if(!cap.isOpened())  // check if we succeeded
		return -1;

	Mat edges;
	cv_bridge::CvImagePtr cv_ptr;
	image_transport::Publisher imagePub = it.advertise("/output_video", 1);

	ros::Rate r(10); 
	Mat frame;
	while (ros::ok())
	{
		cap >> frame;
		sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame).toImageMsg();
		imagePub.publish(msg);
		ros::spinOnce();
		r.sleep();
	}
	return 0;
}

