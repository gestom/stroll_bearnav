#include "CMessageClient.h"

#define NETWORK_BLOCK MSG_WAITALL

CMessageClient::CMessageClient()
{
//	module = LOG_MODULE_ROBOT;
}


CMessageClient::~CMessageClient()
{}

int CMessageClient::sendMessage(CMessage & message)
{
  //dump->Debug(module,"Message sending %s %i %i.",message->getStrType(),message->value1,message->value2);
  message.pack();
  if (send(mySocket,message.buf,MESSAGE_LENGTH,MSG_NOSIGNAL) == MESSAGE_LENGTH) return 0; else
  {
//    dump->Error(module,"Network error");
    return -1;
  }
}

int CMessageClient::init(const char *ip,const int port,bool requirements[])
{
  int result = -1;
  mySocket = socket(AF_INET, SOCK_STREAM, 0);
  CMessage message;
  if (mySocket > 0)
  {
    struct sockaddr_in server_addr;
    struct hostent *host_info;
    host_info =  gethostbyname(ip);
    if (host_info != NULL)
    {
      server_addr.sin_family = host_info->h_addrtype;
      memcpy((char *) &server_addr.sin_addr.s_addr,host_info->h_addr_list[0], host_info->h_length);
      server_addr.sin_port = htons(port);
      result = connect(mySocket,(struct sockaddr*) &server_addr,sizeof(server_addr));
    }
    if (result == 0)
    {
	    if (send(mySocket,requirements,4*sizeof(bool),MSG_NOSIGNAL) == 4*sizeof(bool)) return 0; else
	    {
//		    dump->Error(module,"Network error when sending info");
		    return -1;
	    }
//	    dump->Inform(module,"Connection established.");
//	    message.type = MSG_START;
//	    sendMessage(&message);
    }
  }
  return result;
}

int CMessageClient::checkForHeader()
{
	int pos = 0;
	while (pos < messageHeaderLength) {
		unsigned char b;
		int r = recv(mySocket, &b, 1, NETWORK_BLOCK);
		if (r != 1) return -1;
		if (b == messageHeaderData[pos]) {
			//printf("checkForHeader(%d) %02X\n", pos, b);
			pos++;
		} else {
			//printf("checkForHeader(error) %02X\n", b);
			pos = 0;
		}
	}
	return 0;
}

int CMessageClient::checkForInts(int data[],unsigned int len)
{
  int result;
  int lengthReceived = recv(mySocket,data,len*sizeof(int),NETWORK_BLOCK);
  if ((unsigned int)lengthReceived == len*sizeof(int)) result = 0;
  return result;
}

int CMessageClient::checkForBools(bool data[],unsigned int len)
{
  int result;
  int lengthReceived = recv(mySocket,data,len*sizeof(bool),NETWORK_BLOCK);
  if (lengthReceived == (int)(len*sizeof(bool))) result = 0;
  return result;
}

int CMessageClient::checkForDoubles(double data[],unsigned int len)
{
  int result;
  int lengthReceived = recv(mySocket,data,len*sizeof(double),NETWORK_BLOCK);
  if (lengthReceived == (int)(len*sizeof(double))) result = 0;
  return result;
}

int CMessageClient::checkForData(double odo[],bool but[],int rotat[])
{
	int result = 0;
	result +=checkForDoubles(odo,10);
	result +=checkForBools(but,10);
	result +=checkForInts(rotat,1);
	return result;
}

int CMessageClient::checkForStatus(CStatusMessage & status)
{
  int result;
  int lengthReceived = recv(mySocket, status.getSerializedData(), status.getExpectedLength(), NETWORK_BLOCK);

  //printf("CMessageClient::recv %d:", lengthReceived);
  //for (int i=0; i<lengthReceived; i++) printf(" %02X", (int)(status.getSerializedData()[i])); printf("\n");

  if (lengthReceived == status.getExpectedLength()) {
	  status.unpack();
	  if (status.type == MSG_STATUS && (status.status == 0 || status.status == 1)) {
		  result = 0;
	  } else {
		  result = -1;
	  }
  } else {
	  result = -2;
  }
  return result;
}

