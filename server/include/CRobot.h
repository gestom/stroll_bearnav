#ifndef __CROBOT_H__
#define __CROBOT_H__

#include "CMessage.h"

class CRobot {
public:
	CRobot();
	~CRobot();
	void setSpeeds(const int forward, const int turn, const int flipper);
	bool processMessage(const CMessage & message);
	bool receiveInfo(void);
	void printInfo(void) const;
	void getStatus(CStatusMessage & status);
protected:
	bool resetOdom(void);
	bool odom(void);
	inline int bound(int input, const int min, const int max) const;
	int port;

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
