/*
 * File name: CMessage.h
 * Date:      2006/10/12 11:56
 * Author:    
 */

#ifndef __CMESSAGE_H__
#define __CMESSAGE_H__

#define MESSAGE_LENGTH 10

extern const int messageHeaderLength;
extern const unsigned char messageHeaderData[];

typedef enum {
	MSG_NONE = 0,
	MSG_START,
	MSG_SPEED,
	MSG_EMPTY,
	MSG_RESET,
	MSG_STOP,
	MSG_QUIT,
	MSG_STATUS,
	MSG_NUMBER 
} TMessageType;

class CMessage {
	public:
		CMessage();
		~CMessage();

		unsigned char buf[MESSAGE_LENGTH+1];
		void pack();
		void unpack();
		const char* getStrType();

		TMessageType type;

		int forward;
		int turn;
		int flipper;
};

class CStatusMessage
{
public:
	TMessageType type;

	int status;
	int odoLeft;
	int odoRight;
	int pitch;
	int roll;
	int flipperPos;
	int batteryLevel;

	CStatusMessage();
	void pack();
	void unpack();
	unsigned char * getSerializedData() { return buffer; }
	int getSerializedLength() { return serializedLength; }
	int getExpectedLength() { return 29; }

protected:
	void packInt(int value);
	void unpackInt(int & value, int & ptr);
private:
	unsigned char buffer[32];
	int serializedLength;
};

#endif

/* end of CMessage.h */
