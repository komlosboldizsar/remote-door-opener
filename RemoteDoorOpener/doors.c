#include "doors.h"
#include "protocol.h"
#include "codes.h"

volatile Door door1 = { &MY_PROTOCOL, CODE_DOOR1, 8, 30, 5, 0, 0, IDLE };
volatile Door door2 = { &MY_PROTOCOL, CODE_DOOR2, 8, 40, 5, 0, 0, IDLE };