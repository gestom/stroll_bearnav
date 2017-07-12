#include <errno.h>
#include <string.h>
#include <time.h>

#include "CConsole.h"
#include "CMessageServer.h"

CMessageServer::CMessageServer()
{
	sem_init(&connectSem,0,1);	
	pthread_mutex_init(&dataMutex, 0);
	pthread_cond_init(&dataEvent, 0);
	messageRead = 0;
}

CMessageServer::~CMessageServer()
{
	pthread_cond_destroy(&dataEvent);
	pthread_mutex_destroy(&dataMutex);
	sem_destroy(&connectSem);
}

void* connectLoop(void *serv)
{
	struct sockaddr_in clientAddr;
	socklen_t addrLen = sizeof(clientAddr);
	CMessageServer* server = (CMessageServer*) serv;
	int newClient = 0;
	while (true)
	{
		newClient = accept(server->serverSocket, (struct sockaddr *)&clientAddr,&addrLen);
		if (newClient > -1){
			msg;
			fprintf(stdout,"Incoming connection from %s.\n",inet_ntoa(clientAddr.sin_addr));
			msg;
			fprintf(stdout,"Incoming connection accepted on socket level %i.\n",newClient);
			sem_wait(&server->connectSem);
			SClientInfo * clientInfo = new SClientInfo;
			clientInfo->socket = newClient;
			clientInfo->server = server;
			server->clientSockets.push_back(newClient);
			sem_post(&server->connectSem);
			pthread_t* thread=(pthread_t*)malloc(sizeof(pthread_t));
			pthread_create(thread,NULL,&serverLoop,(void*)clientInfo);
		}else{
			warn;
			fprintf(stderr, "Accept on listening socked failed.\n");
		}
	}
	return NULL;
}

int CMessageServer::initServer(const char* port)
{
	int used_port = atoi(port);
	struct sockaddr_in mySocketAddr;
	mySocketAddr.sin_family = AF_INET;
	mySocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	mySocketAddr.sin_port = htons(used_port);
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	int reuse = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	//setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
	if (serverSocket < 0)
	{
		error;
		perror("Cannot create socket ");
		return -1;
	}
	if (bind(serverSocket,( struct sockaddr *)&mySocketAddr,sizeof(mySocketAddr)) < 0)
	{
		error;
		perror("Cannot bind socket.");
		::close(serverSocket);
		return -2;
	}
	if (listen(serverSocket,1) < 0)
	{
		error;
		fprintf(stderr,"cannot make socket listen.\n");
	}
	msg << "Listening on port " << used_port << std::endl;
	pthread_t* thread=(pthread_t*)malloc(sizeof(pthread_t));
	pthread_create(thread,NULL,&connectLoop,(void*)this);
	return 0;
}

void* serverLoop(void* clientInfo)
{
	SClientInfo * info = (SClientInfo*) clientInfo;
	CMessage message; 
	CMessageServer* server = info->server;
	server->getClientInfo(info->socket,&info->write);
	//int dataOk = 0;
	bool connected = true;
	while (connected){
		//dataOk = 0;
		message = server->checkForMessage(info->socket);
		//fprintf(stdout,"Message received %s %i %i from %i.",message.getStrType(),message.value1,message.value2,info.socket);
#if 0
		sem_wait(info.sem);
		if (info.odometry) server->sendDoubles(info.socket,server->odometry,10);
		if (info.buttons) server->sendBools(info.socket,server->buttons,10);
		if (info.rotation) server->sendInts(info.socket,server->rotation,1);
		sem_post(info.sem);
#endif
		if (info->write){
			//fprintf(stdout,"Sending message to main server.");
			server->incomingMessage(message);
		}
		if (message.type == MSG_QUIT)
			connected = false;
	}
	server->closeConnection(info->socket);
	delete info;
	return NULL;
}

CMessage CMessageServer::getMessage(int timeout)
{
	CMessage result;
	pthread_mutex_lock(&dataMutex);
	if (incomingMessages.empty()) {
		if (timeout < 0) {
			pthread_cond_wait(&dataEvent, &dataMutex);
		} else {
			if (timeout > 0) {
				struct timespec to;
				to.tv_sec = time(NULL) + timeout;
				to.tv_nsec = 0;
				pthread_cond_timedwait(&dataEvent, &dataMutex, &to);
			}
		}
	}
	if (!incomingMessages.empty()) {
		result = incomingMessages.front();
		incomingMessages.pop_front();
		messageRead++; 
	} else {
		result.type = MSG_NONE;
	}
	pthread_mutex_unlock(&dataMutex);
	return result;
}

int CMessageServer::getMessageReadCount()
{
	int result;
	pthread_mutex_lock(&dataMutex);
	result = messageRead;
	pthread_mutex_unlock(&dataMutex);
	return result;
}

bool CMessageServer::getClientInfo(int socket,bool data[])
{
	bool result;
	char buffer[1000];
	int receiveResult = recv(socket,data,4*sizeof(bool),MSG_WAITALL); // TODO: portability is limited (struct aligning)
	if ( receiveResult >0 && connected == 1)
	{
		sprintf(buffer,"Client requires");
		if (data[0]) sprintf(buffer,"%s to drive the robot and obtain ",buffer); else sprintf(buffer,"to read ");
		if (data[1]) sprintf(buffer,"%s odometric, ",buffer);
		if (data[2]) sprintf(buffer,"%s rotation speed, ",buffer);
		if (data[3]) sprintf(buffer,"%s pressed buttons ",buffer);
		sprintf(buffer,"%sdata.",buffer);
		msg;
		fprintf(stdout,"%s\n",buffer);
		result = true;
	}
	else
	{
		warn;
		fprintf(stdout,"Disconnect detected.\n");
		result = false;
	}
	return result;
}

CMessage CMessageServer::checkForMessage(int socket)
{
	CMessage message;
	int receiveResult = recv(socket,&message.buf,MESSAGE_LENGTH,MSG_WAITALL);
	if ( receiveResult >0)
	{
		//fprintf(stdout,"Packet accepted length %i.", receiveResult);
		message.unpack();
	}
	else
	{
		//fprintf(stdout,"Disconnect detected.");
		message.type = MSG_QUIT;
	}
	return message;
}

void CMessageServer::incomingMessage(CMessage message)
{
	pthread_mutex_lock(&dataMutex);
	incomingMessages.push_back(message);
	pthread_cond_signal(&dataEvent);
	pthread_mutex_unlock(&dataMutex);
}

void CMessageServer::update(double posit[],bool butto[],int rotat[]) 
{
	pthread_mutex_lock(&dataMutex);
	memcpy(odometry,posit,10*sizeof(double));	
	memcpy(buttons,butto,10*sizeof(bool));
	memcpy(rotation,rotat,sizeof(int));
	pthread_mutex_unlock(&dataMutex);
}

int CMessageServer::sendHeader()
{
	int result = 0;
	for (unsigned int i=0; i<clientSockets.size(); i++) {
		int s = send(clientSockets[i], messageHeaderData, messageHeaderLength, MSG_NOSIGNAL);
		if (s != messageHeaderLength) result = -1;
	}
	return result;
}

int CMessageServer::sendStatus(CStatusMessage & status)
{
	if (clientSockets.empty()) {
		// not connected
		warn << "sendStatus() not connected" << std::endl;
		return 1;
	}

	status.pack();
	sendHeader();
	for (unsigned int i=0; i<clientSockets.size(); i++) {
		if (send(clientSockets[i], status.getSerializedData(), status.getSerializedLength(), MSG_NOSIGNAL) > 0) {
			return 0;
		} else {
			error;
			fprintf(stderr,"send: %s\n", strerror(errno));
			return -1;
		}
	}
	return 0;
}

int CMessageServer::sendPosition(int socket,double buffer[])
{
	if (send(socket,buffer,4*sizeof(double),MSG_NOSIGNAL) == 4*sizeof(double)) return 0; else
	{
		error;
		fprintf(stderr,"Network error\n");
		return -1;
	}
	return 0;
}

int CMessageServer::sendInts(int socket,int buffer[],int len)
{
	if (send(socket,buffer,len*sizeof(int),MSG_NOSIGNAL) == (int)(len*sizeof(int))) return 0; else
	{
		error;
		fprintf(stderr,"Network error\n");
		return -1;
	}
	return 0;
}


int CMessageServer::sendDoubles(int socket,double buffer[],int len)
{
	int a =	send(socket,buffer,len*sizeof(double),MSG_NOSIGNAL);
	if (a == (int)(len*sizeof(double)))  return 0; else
	{
		error;
		fprintf(stderr,"Network error, %i \n",a);
		return -1;
	}
	return 0;
}

int CMessageServer::sendBools(int socket,bool buffer[],int len)
{
	    if (send(socket,buffer,len*sizeof(bool),MSG_NOSIGNAL) == (int)(len*sizeof(bool))) return 0; else
		 {
			error;
			fprintf(stderr,"Network error\n");
			return -1;
		 }
		 return 0;
}

int CMessageServer::closeConnection(int socket)
{
	::close(socket);
	for (std::vector<int>::iterator it = clientSockets.begin(); it != clientSockets.end(); it++) {
		if (*it == socket) {
			clientSockets.erase(it);
			break;
		}
	}
	connected = false;
	return 0;
}

