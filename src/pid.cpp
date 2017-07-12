#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <math.h>
#include <iostream>

ros::Subscriber odometrySub;
ros::Publisher commandPub;
geometry_msgs::Twist twist;
nav_msgs::Odometry odometry;


//Pid parameters init
float desiredAngle=0;
double angularAxis=0.1;
int linearAxis=0;
float Kp=1;
float Kd=0;
float Ki=0;
float error=0;
float currentAngle=0;
float tmperror=0;
float z=0;
float w=0;
float integral=0;
float derivation=0;
float lastAngle=0;
bool start=true;
int spin=0;
float output=0;


void odomcallback(const nav_msgs::Odometry::ConstPtr& msg);


//Pid function - rotates robot by desired angle 
void odomcallback(const nav_msgs::Odometry::ConstPtr& msg){	
	z=msg->pose.pose.orientation.z;
	w=msg->pose.pose.orientation.w;
	
	lastAngle=currentAngle;
	currentAngle=atan2(2*(z*w),1-2*(z*z))+2*3.14*spin; //get angle from quaternions
	//initiate parameters
	//Enter PID parameters and desired angle of rotation
	if(start==true){
		std::cout << "Enter desired Angle (rad): ";
		std::cin >> desiredAngle; 
		desiredAngle=atan2(2*(z*w),1-2*(z*z))+desiredAngle;
		lastAngle=currentAngle;
		start=false;
	}
	//if spin are > or < than pi
	if ((lastAngle-currentAngle)>4.71){
	spin++;
	currentAngle=currentAngle+2*3.14;
	}
	if ((lastAngle-currentAngle)< -4.71){
	spin--;
	currentAngle=currentAngle-2*3.14;
	}
	//PID function
	error=desiredAngle-currentAngle;
	derivation=error-tmperror;
	integral=integral+error;
	output=Kp*error+Ki*integral+Kd*derivation;
	twist.angular.z=output*angularAxis;
	tmperror=error;
	twist.linear.x=0; //the robot wont move forward

	//printout important parameters
	ROS_INFO( "%d %f %f %f",spin,currentAngle,lastAngle,desiredAngle);
}

int main(int argc,char** argv){

	ros::init(argc, argv, "pid");
	ros::NodeHandle n("~");
	n.param("axis_angular", angularAxis, 0.1);
	n.param("axis_linear", linearAxis,0);	
	odometrySub = n.subscribe<nav_msgs::Odometry>("/odom",10 ,odomcallback);
	commandPub = n.advertise<geometry_msgs::Twist>("cmd",1);
	while(ros::ok()){
		ros::spinOnce();
		
		//slow down rotation
		if(twist.angular.z>0.4){
		twist.angular.z=0.4;
		}
		twist.angular.x=0;
		commandPub.publish(twist);
		usleep(50000);
}
}
