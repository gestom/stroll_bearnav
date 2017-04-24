
#include "CJoystick.h"
#include "CMessage.h"
#include "CMessageClient.h"
#include "CConsole.h"
#include "GamePad.h"

#include <iostream>

#ifdef ENABLE_POSITION_SERVER
#include <math.h>
#include "pos_server.h"
#endif

using namespace std;

enum ControlState {
	HAND = 0,
	QUIT,
	RESET_ODOM,
	AUTOMATIC
};

ControlState state = HAND;
CJoystick joystick;

ControlState switcher(void) {

	if (joystick.buttonPressed(GP_R2))
		state = HAND;
	else if (joystick.buttonPressed(GP_9))
		state = AUTOMATIC;
	else if (joystick.buttonPressed(GP_10))
		return RESET_ODOM;

	return state;
}

void automatic(CMessage & message) {
	message.type = MSG_SPEED;
	message.forward = 400;
	message.turn = 0;
	message.flipper = 0;
}

void quit(CMessage & message) {
	message.type = MSG_QUIT;
}

#if 0
void hand(CMessage & message) {
	message.type = MSG_SPEED;
	const int alpha = (joystick.buttonPressed(GP_L2) || joystick.buttonPressed(GP_R1)) ? 8 : 2;
	if (joystick.axis(GP_LEFT_VERTICAL))
		message.forward = -joystick.axis(GP_LEFT_VERTICAL)/100 * alpha;
	else if (joystick.buttonPressed(GP_3))
		message.forward = -500 * alpha / 2;
	else if (joystick.buttonPressed(GP_1))
		message.forward =  500 * alpha / 2;
	else
		message.forward =  0;

	if (joystick.axis(GP_LEFT_HORIZONTAL))
		message.turn    = -joystick.axis(GP_LEFT_HORIZONTAL)/300;
	else if (joystick.buttonPressed(GP_4))
		message.turn =  50;
	else if (joystick.buttonPressed(GP_2))
		message.turn = -50;
	else
		message.turn =  0;

	message.flipper = joystick.axis(GP_RIGHT_VERTICAL)/500;
}
#endif

const char * serverHostname = "localhost";
int serverPort = 50004;
bool joystickTestOnly = false;

int parseArguments(int argc, char ** argv)
{
	int c;
	while ((c = getopt (argc, argv, "j")) != -1) {
		switch (c)
		{
			case 'j':
				joystickTestOnly = true;
				break;
			case '?':
				//if (optopt == 'c')
					//fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				//else
				if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
				return 1;
			default:
				abort ();
		}
		return 0;
	}

	int nonOptionIndex = 0;
	for (int index = optind; index < argc; index++) {
		switch (nonOptionIndex) {
			case 0:
				serverHostname = argv[index];
				break;
			default:
				warn; printf ("Unhandled non-option argument %s\n", argv[index]);
		}
		nonOptionIndex++;
	}
	return 0;
}

// joystick control version 2 - HCH
void hand2(CMessage & message)
{
	message.type = MSG_SPEED;
	if (joystick.status()) {
		bool turboMode = (joystick.buttonPressed(GP_L2));
		bool slowMode = (joystick.buttonPressed(GP_L1));
		//int alpha = (joystick.buttonPressed(GP_L2) || joystick.buttonPressed(GP_R1)) ? 8 : 2;
		int alpha = (turboMode) ? 6 : 2;
		if (slowMode) alpha = alpha / 2;
		if (joystick.axis(GP_RIGHT_VERTICAL))
			message.forward = -joystick.axis(GP_RIGHT_VERTICAL)/100 * alpha;
		else
			message.forward =  0;

		if (joystick.axis(GP_RIGHT_HORIZONTAL)) {
			float a = -joystick.axis(GP_RIGHT_HORIZONTAL);
			float halfFactor = (!turboMode) ? 0.3 : 0.2; // velocity factor in the middle of stick range
			float halfInput = 0x7FFF / 2;
			if (a < halfInput && a > -halfInput) {
				message.turn = (a * halfFactor) / 300.0;
			} else {
				int sgn = (a>0) ? 1 : -1;
				message.turn = sgn * ((sgn*a-halfInput) * (1.0f-0.0) + halfFactor*halfInput) / 300.0;
			}
			if (slowMode) message.turn /= 2;
			if (turboMode) message.turn *= 2;
			if (abs(joystick.axis(GP_LEFT_HORIZONTAL)) > halfInput) {
				// add 1-2 * turn factor by left stick
				message.turn = abs(joystick.axis(GP_LEFT_HORIZONTAL)) * message.turn / halfInput;
			}
			//printf("a=%.3f   %d\n", a, message.turn);
		} else {
			message.turn =  0;
		}
		message.flipper = joystick.axis(GP_LEFT_VERTICAL)/500;
	} else {
		fprintf(stderr, "joystick not active\n");
		message.forward = 0;
		message.turn = 0;
		message.flipper = 0;
	}

}

void * odometry_thread(void *);

int main(int argc, char ** argv) {
	CMessage message;
	CMessageClient client;
	bool requier[2] = {true, true};

	int rc = parseArguments(argc, argv);
	if (rc != 0) {
		return rc;
	}

	if (joystickTestOnly) {
		if (joystick.initialized()) {
			fprintf(stdout, "joystick detected\n");
			return 0;
		} else {
			fprintf(stdout, "joystick not detected\n");
			return 1;
		}
	}

	msg; fprintf(stdout, "Connecting to %s:%d\n", serverHostname, serverPort);
	client.init(serverHostname, serverPort, requier);

	message.type = MSG_EMPTY;

	CStatusMessage status;

	pthread_t odo_thread_id;
	pthread_create(&odo_thread_id, NULL, odometry_thread, &client);

	while (state != QUIT) {
		client.sendMessage(message);
		fprintf(stdout,"Command: %s % 5i%c % 5i % 5i\n", message.getStrType(),
			message.forward,
			joystick.buttonPressed(GP_L2) ? '*' : ' ',
			message.turn, message.flipper);
		usleep(50 * 1000);
		if (client.checkForStatus(status) == 0) { // TODO: should not be called here, as it is called from odometry_thread
			fprintf(stdout, " Status: %d battery=%3d%%\n", status.status, status.batteryLevel);
		}
		switch (switcher()) {
			case AUTOMATIC:
				automatic(message);
				continue;
			case RESET_ODOM:
				dbg << "reset odom" << endl;
				message.type = MSG_RESET;
				continue;
			case QUIT:
				quit(message);
				continue;
			case HAND:
			default:
				//hand(message);
				hand2(message);
		}
	}
	return EXIT_SUCCESS;
}

#ifdef ENABLE_POSITION_SERVER
static uint32_t getTimeMs()
{
	uint32_t t;
	struct timeval now;
	gettimeofday(&now, NULL);
	t = now.tv_sec * 1000 + now.tv_usec / 1000;
	return t;
}

void control_callback(CControlMessage * ctrl);
#endif

void * odometry_thread(void * arg)
{
	CMessageClient * client = (CMessageClient*) arg;
#ifdef ENABLE_POSITION_SERVER
	socketDisableSigPipe();
	int psPort = 5566;
	CPositionServer server(psPort, control_callback);
	fprintf(stderr, "position server started on port %d\n", psPort);
	double x = 0;
	double y = 0;
	double z = 0;
	double heading = 0;
	double lastOdoLeft = 0;
	double lastOdoRight = 0;
	bool lastOdoValid = false;
#endif
	int counter = 0;
	while (1) {
		CStatusMessage status;
		if (client->checkForHeader() == 0) {
			if (client->checkForStatus(status) == 0) {
				if ((counter ++) % 50 == 0) {
					printf("Received cameleon robot status: battery=%d%%\n", status.batteryLevel);
				}
				// update odometry
#ifdef ENABLE_POSITION_SERVER
				double odoLeft = 0.001 * (double) status.odoLeft;
				double odoRight = 0.001 * (double) status.odoRight;
				if (lastOdoValid) {
					double wheelBase = 0.45;
					double dLeft = odoLeft - lastOdoLeft;
					double dRight = odoRight - lastOdoRight;
					double dForward = 0.5 * ( dLeft + dRight );
					double dHeading = (dRight - dLeft) / wheelBase;

					heading += dHeading;
					x += dForward * cos(heading);
					y += dForward * sin(heading);

					CPositionMessage pos;
					pos.flags = 0;
					pos.x = 1000.0 * x;
					pos.y = 1000.0 * y;
					pos.z = 1000.0 * z;
					pos.a = 1000.0 * heading;
					pos.b = 0;
					pos.c = 0;
					pos.timestamp = getTimeMs();
					server.sendPosition(&pos);
				}
				lastOdoLeft = odoLeft;
				lastOdoRight = odoRight;
				lastOdoValid = true;
#endif
			}
		}
		usleep(1000);
	}
	return 0;
}

#ifdef ENABLE_POSITION_SERVER
void control_callback(CControlMessage * ctrl)
{
	//remoteControlForward = ctrl->forwardVelocity;
	//remoteControlAngular = ctrl->angularVelocity;
}
#endif
