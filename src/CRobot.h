#ifndef __CROBOT_H__
#define __CROBOT_H__

class CRobot {
	public:
		CRobot(const char *port,float maxForward,float maxBackward,float maxTurn);
		~CRobot();
		void setSpeeds(float forward,float turn, float flip); 
		bool receiveInfo(void);
		bool exchangeInfo();
		void printInfo(void) const;
	protected:
		bool resetOdom(void);
		bool odom(void);
		inline int bound(int input, const int min, const int max) const;
		int port;
		
		int forwardSpeed,turnSpeed,flipSpeed;
		bool dataValid;

		int odomLeft;
		int odomRight;
		int flipperPose;
		int batteryLevel;
		int pitch;
		int roll;

		int distance;
		double theta;
		double x;
		double y;

		double theta0;
		int oldROdom;
		int oldLOdom;
};

#endif
