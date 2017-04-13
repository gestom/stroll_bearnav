#include <ros/ros.h>
#include <tf/tf.h>
#include <geometry_msgs/Twist.h>
#include "CMessageClient.h"
#include <string.h>

ros::Subscriber commandSub;

//robot Parameters
int robotPort;
std::string robotIP;
double maxForwardSpeed,maxBackwardSpeed,maxTurnSpeed;
double forwardSpeedGain,turnSpeedGain;
CMessage message;
CMessageClient client;
CStatusMessage status;

void commandCallback(const geometry_msgs::Twist::ConstPtr& msg)
{
	message.type = MSG_SPEED;
	message.forward = (int)(msg->linear.x*1000);
	message.turn = (int)(msg->angular.z*1000);
	message.flipper = 0;

	if (message.forward > +maxForwardSpeed*1000) message.forward = maxForwardSpeed*1000;
	if (message.forward < -maxBackwardSpeed*1000) message.forward = -maxBackwardSpeed*1000;
	if (message.turn    > +maxTurnSpeed*1000) message.turn = maxTurnSpeed*1000;
	if (message.turn    < -maxTurnSpeed*1000) message.turn = -maxTurnSpeed*1000;

	client.sendMessage(message);
}
     
int main(int argc, char** argv)
{
	ros::init(argc, argv, "cameleon_ros_driver");
	ros::NodeHandle n("~");

	//initialize robot model parameters
	n.param("cameleon_port",robotPort,50004);
	n.param("cameleon_ip",robotIP,std::string("172.43.50.193"));
	n.param("cameleon_maxForwardSpeed",maxForwardSpeed,0.5);
	n.param("cameleon_maxBackwardSpeed",maxBackwardSpeed,0.5);
	n.param("cameleon_maxTurnSpeed",maxTurnSpeed,0.5);
	printf("%f",maxTurnSpeed);
	bool requier[2] = {true, true};
	client.init(robotIP.c_str(), robotPort, requier);

	//create the robot
	//cmd_vel = n.advertise<geometry_msgs::Twist>("/cmd_vel", 1);
	commandSub = n.subscribe("/cmd_vel", 1, commandCallback);

	ros::spin();
}
