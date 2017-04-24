#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include "CMessage.h"
#include "CMessageServer.h"
#include "CRobot.h"

bool run = true;

void exitFunction(int signal) {
	run = false;
}

int main(int argc, char ** argv) {
	signal(SIGTERM, &exitFunction);

	CMessage message;
	CMessageServer server;
	CRobot robot;

	message.type = MSG_SPEED;

	while(server.initServer("50004"))
		usleep(1000000);

	int lastTurn = 0;
	int lastForward = 0;
	int lastFlipper = 0;
	int lastCommandTime = 0;

	while (run) {
		message = server.getMessage(0);
		//if (server.getMessageReadCount() > 1) {
			//fprintf(stdout, "message is not fresh\n");
			//message.type = MSG_NONE;
		//}
		fprintf(stdout, "Command: %s - % 5i % 5i % 5i\n",
			message.getStrType(),
			message.forward,
			message.turn,
			message.flipper);
		if (!robot.processMessage(message)) {
			//robot.receiveInfo(); // it is called from processMessage -> setSpeeds -> receiveInfo
			lastCommandTime++;
			if (lastCommandTime > 20) {
				lastForward = 0;
				lastTurn = 0;
				lastFlipper = 0;
			}
			robot.setSpeeds(lastForward, lastTurn, lastFlipper);
		} else {
			lastForward = message.forward;
			lastTurn = message.turn;
			lastFlipper = message.flipper;
			lastCommandTime = 0;
		}
		{
			CStatusMessage status;
			robot.getStatus(status);
			server.sendStatus(status);
		}
		robot.printInfo();
		usleep(40000);
	}

	robot.setSpeeds(0, 0, 0);
	fprintf(stdout, "Exiting\n");
	sleep(1);
	return EXIT_SUCCESS;
}

