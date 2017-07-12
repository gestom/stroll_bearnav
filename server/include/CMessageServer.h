/*
 * File name: CMessageServer.h
 * Date:      2006/10/12 12:18
 * Author:    
 */

#ifndef __CMESSAGESERVER_H__
#define __CMESSAGESERVER_H__

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>
#include <list>

#include <semaphore.h>
#include <pthread.h>

#include "CMessage.h"

#define NUM_CONNECTIONS 100

class CMessageServer;

typedef struct 
{
	bool write,odometry,rotation,buttons;
	CMessage message;
	int time;
	int socket;
	CMessageServer * server;
}SClientInfo;

void* serverLoop(void* clientInfo);

class CMessageServer{
	public:

		CMessageServer();
		~CMessageServer();
		int initServer(const char* port);
		void update(double odo[],bool buttons[],int rotation[]);
		CMessage getMessage(int timeout = 0);
		int getMessageReadCount();

		bool getClientInfo(int socket,bool data[]);
		CMessage checkForMessage(int socket);
		void incomingMessage(CMessage message);
		int sendHeader();
		int sendStatus(CStatusMessage & status);
		int sendPosition(int socket,double buffer[]);
		int sendDoubles(int socket,double buffer[],int len);
		int sendInts(int socket,int buffer[],int len);
		int sendBools(int socket,bool buffer[],int len);
		int closeConnection(int socket);
		int connected;
		int serverSocket;
		sem_t connectSem;
		pthread_mutex_t dataMutex;
		pthread_cond_t dataEvent;
		std::list<CMessage> incomingMessages;
		double odometry[10];
		bool buttons[10];
		int rotation[1];
		int messageRead;
		std::vector<int> clientSockets;
};

#endif

/* end of CMessageServer.h */
