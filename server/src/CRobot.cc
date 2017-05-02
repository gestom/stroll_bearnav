#include <cmath>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "CRobot.h"
#include "CConsole.h"

CRobot::CRobot() {
	//	const char * name = "/home/petr/cameleon/file";
	const char * name = "/dev/ttyS0";
	port = open(name, O_RDWR | O_NONBLOCK);
	if (port == -1) {
		error;
		perror("Cannot open port");
	} else
		msg << "Opened port " << name << std::endl;

	struct termios ctrlStruct;
	tcgetattr(port, &ctrlStruct);
	ctrlStruct.c_oflag = 0;
	ctrlStruct.c_iflag = 0;
	ctrlStruct.c_lflag = 0;
	ctrlStruct.c_cflag = B115200 | CS8 | CREAD;
	tcsetattr(port, TCSANOW, &ctrlStruct);
	tcflush(port, TCSANOW);

	oldROdom = 0;
	oldLOdom = 0;
	theta0 = 0;

	dataValid = false;

	odomLeft = 0;
	odomRight = 0;
	flipperPose = 0;
	batteryLevel = 0;
	pitch = 0;
	roll = 0;
	distance = 0;
	theta = 0;
	x = 0;
	y = 0;

}

CRobot::~CRobot() {
	close(port);
}

bool CRobot::processMessage(const CMessage & message) {
	bool result = false;
	switch (message.type) {
		case MSG_STOP:
			setSpeeds(0,0,0);
			result = true;
			break;

		case MSG_SPEED:
			setSpeeds(message.forward, message.turn, message.flipper);
			result = true;
			break;

		case MSG_RESET:
			dbg << "Reset: " <<
				" OLO " << oldLOdom << " ORO " << oldROdom << std::endl;
			resetOdom();
			result = true;
			break;

		default:
			break;
	}
	return result;
}

void CRobot::setSpeeds(const int forward, const int turn, const int flip) {
	char serialFrame[32];
	sprintf(serialFrame, "V%+05dP%+04dF%+03dOVNNNNVXXX\n",
			forward,
			turn,
			bound(flip, -47, 47));
	unsigned char checkSum = 0;

	for (unsigned short i = 0; i < 22; i++)
		checkSum += serialFrame[i];

	sprintf(serialFrame + 22, "%03u\n", checkSum);
	msg << serialFrame << std::endl;
	ssize_t wn = write(port, serialFrame, 26);
	if (wn == -1) {
		error;
		perror("Write failed");
	}

	if (receiveInfo()) {
		odom();
	}
}

bool CRobot::odom(void) {
	const double base = 0.45; // m
	const int rOdom = odomRight - oldROdom;
	const int lOdom = odomLeft  - oldLOdom;
	dbg << "rodom " << oldROdom << " lodom " << oldLOdom << std::endl;
	dbg << "rodom " << rOdom << " lodom " << lOdom << std::endl;
	const int sum = (rOdom + lOdom);
	const int subst = (rOdom - lOdom);
	distance = sum / 2;
	theta = subst / 1000.0 / base;
	if (subst > 1.0e-5) {
		x = sum * base / subst / 2 * (sin(theta));
		y = sum * base / subst / 2 * (cos(theta) - 1);
	} else
		x = sum * base / 2;

	msg << "Distance " << distance / 1000.0 << " Theta " << theta << " x " << x << " y " << y << std::endl;
	return true;
}

bool CRobot::receiveInfo(void) {
	static char buffer[200];
	int ret = read(port, buffer, 200);
	if (ret == 64) {
		sscanf(buffer, "%06d%06d", &odomLeft, &odomRight);
		sscanf(buffer + 41, "%03d%03d%03d", &batteryLevel, &pitch, &roll);
		sscanf(buffer + 54, "%03d", &flipperPose);
		dbg << (buffer);
		dataValid = true;
	} else {
		warn;
		fprintf(stdout, "Suspicious CPF frame - dropping\n");
		dataValid = false;
	}
	msg << ret << std::endl;

	return (ret == 64);
}

bool CRobot::resetOdom(void) {
	dbg << "Reset odom function" << std::endl;
	odom();
	oldLOdom = odomLeft;
	oldROdom = odomRight;
	dbg << "rodom " << oldROdom << " lodom " << oldLOdom << std::endl;
	return true;
}

void CRobot::printInfo(void) const {
	dbg << "Print Info ";
	fprintf(stdout, "LROdom % 2.3f % 2.3f Pitch %1.3f Roll %1.3f Flipper %1.3f Battery % 3d%%\n",
			odomLeft / 1000.0,        // m
			odomRight / 1000.0,       // m
			pitch * M_PI / 180,       // degree
			roll * M_PI / 180,        // degree
			flipperPose * M_PI / 180, // degree
			batteryLevel);            // %
}

void CRobot::getStatus(CStatusMessage & status)
{
	status.status = (dataValid) ? 1 : 0;
	status.odoLeft = odomLeft;
	status.odoRight = odomRight;
	status.pitch = pitch;
	status.roll = roll;
	status.flipperPos = flipperPose;
	status.batteryLevel = batteryLevel;
}

inline int CRobot::bound(int input, const int min, const int max) const {
	input = input > min ? input : min;
	return input < max ? input : max;
}
