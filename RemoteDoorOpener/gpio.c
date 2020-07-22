#include "gpio.h"

void led1On() {
	PIN_ON(PORT_LED1, PIN_LED1);
}

void led1Off() {
	PIN_OFF(PORT_LED1, PIN_LED1);
}

void led1Toggle() {
	PIN_TOGGLE(PORT_LED1, PIN_LED1);
}

void led2On() {
	PIN_ON(PORT_LED2, PIN_LED2);
}

void led2Off() {
	PIN_OFF(PORT_LED2, PIN_LED2);
}

void led2Toggle() {
	PIN_TOGGLE(PORT_LED2, PIN_LED2);
}