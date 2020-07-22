#ifndef DOORS_H_INCLUDED
#define DOORS_H_INCLUDED

#include "protocol.h"

typedef enum _DoorState {
	IDLE,
	DELAY_BEFORE,
	QUEUED,
	OPENING,
	DELAY_AFTER
} DoorState;

typedef struct _Door {
	const RemoteProtocol* protocol;
	const char* code;
	unsigned int repeats;
	unsigned char delayBeforeSetting;
	unsigned char delayAfterSetting;
	char delayBeforeLeft;
	char delayAfterLeft;
	DoorState state;
} Door;

extern volatile Door door1, door2;

#endif // DOORS_H_INCLUDED