#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>

#include "CMessage.h"

const int messageHeaderLength = 3;
const unsigned char messageHeaderData[] = {0xF0, 0xAF, 0xFA};

const char* StrMessage[] ={
	"None",
	"Start",
	"Speed set",
	"Empty message",
	//"Set turn speed",
	//"Set forward speed",
	//"Turn by angle",
	//"Forward by distance",
	"Reset",
	"Stop",
	"Quit",
	"Status",
	"Number"
};

CMessage::CMessage() {
	type = MSG_NONE;
	forward = turn = flipper = 0;
}

CMessage::~CMessage() { }

const char * CMessage::getStrType() {
	return StrMessage[type];
}

void CMessage::pack() {
	buf[0] = type;
	if (forward < 0) {
		forward = - forward;
		buf[3] = 1;
	}else{
		buf[3] = 0;
	}
	buf[1] = forward%256; 
	buf[2] = forward/256;
	if (turn < 0) {
		turn = - turn;
		buf[6] = 1;
	}else{
		buf[6] = 0;
	}
	buf[4] = turn%256; 
	buf[5] = turn/256;

	if (flipper < 0) {
		flipper = - flipper;
		buf[9] = 1;
	}else{
		buf[9] = 0;
	}
	buf[7] = flipper%256; 
	buf[8] = flipper/256;
}

void CMessage::unpack() {
	type = (TMessageType)buf[0];
	forward = buf[1] + buf[2]*256; 
	turn = buf[4] + buf[5]*256; 
	flipper = buf[7] + buf[8]*256; 
	if (buf[3] > 0) forward = - forward;
	if (buf[6] > 0) turn = - turn;
	if (buf[9] > 0) flipper = - flipper;
}

CStatusMessage::CStatusMessage()
{
	serializedLength = 0;
	type = MSG_STATUS;
	status = 0;
	odoLeft = 0;
	odoRight = 0;
	pitch = 0;
	roll = 0;
	flipperPos= 0;
	batteryLevel = 0;
}

void CStatusMessage::pack()
{
	serializedLength = 0;
	buffer[serializedLength++] = type;
	packInt(status);
	packInt(odoLeft);
	packInt(odoRight);
	packInt(pitch);
	packInt(roll);
	packInt(flipperPos);
	packInt(batteryLevel);
}

void CStatusMessage::unpack()
{
	int ptr = 0;
	type = (TMessageType) buffer[ptr++];
	unpackInt(status, ptr);
	unpackInt(odoLeft, ptr);
	unpackInt(odoRight, ptr);
	unpackInt(pitch, ptr);
	unpackInt(roll, ptr);
	unpackInt(flipperPos, ptr);
	unpackInt(batteryLevel, ptr);
}

void CStatusMessage::packInt(int value)
{
	uint32_t v = htonl((uint32_t)value);
	memcpy(buffer + serializedLength, &v, 4);
	serializedLength += 4;
}

void CStatusMessage::unpackInt(int & value, int & ptr)
{
	uint32_t v;
	memcpy(&v, buffer + ptr, 4);
	ptr += 4;
	value = (int32_t) ntohl(v);
}

