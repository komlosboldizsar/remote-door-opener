#define F_CPU 8000000UL
#define NULL 0

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "codes.h"
#include "gpio.h"
#include "protocol.h"
#include "pgmspace_utils.h"
#include "doors.h"

volatile unsigned long myTime = 0;

/* Handle pressing buttons */

void openDelayed(volatile Door* door) {
	door->state = DELAY_BEFORE; \
	door->delayBeforeLeft = door->delayBeforeSetting;
}

void openImmediately(volatile Door* door) {
	door->state = QUEUED;
	door->delayBeforeLeft = 0;
}

#define HANDLE_BUTTON(DOOR,BUTTON_DEBOUNCING_VAR)	if (DOOR.state == IDLE) \
														openDelayed(&DOOR); \
													else if ((DOOR.state == DELAY_BEFORE) && ((myTime - BUTTON_DEBOUNCING_VAR) > 200)) \
														openImmediately(&DOOR); \
													BUTTON_DEBOUNCING_VAR = myTime;

unsigned long button1LastPressed = 0, button2LastPressed = 0;

ISR (INT0_vect)
{
	HANDLE_BUTTON(door1, button1LastPressed);
}

ISR (INT1_vect)
{
	HANDLE_BUTTON(door2, button2LastPressed);
}

/* Protocol, radio things */

char currentCode[CODES_MAX_LENGTH+1];
volatile char repeatIndex;
volatile char pulseIndex;
volatile unsigned char pulseCurrentLength;
const unsigned char* volatile pulseNeededLength;

#define ENABLE_TIMER1()              TCCR1B |= (1<<CS11); TIMSK |= (1<<OCIE1A);
#define DISABLE_TIMER1()             TIMSK &= ~(1<<OCIE1A); TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10));

void setTimer1Interval(unsigned int usecs) {
	OCR1AH = (usecs >> 8);
	OCR1AL = usecs;
}

void initTimer1() {
	DISABLE_TIMER1();
	TCCR1A = 0x00;
	TCCR1B |= (1<<WGM12);
	setTimer1Interval(1000);
}

void initProtocol(const RemoteProtocol* protocol) {
	setTimer1Interval(protocol->baseTime);
	ENABLE_RADIO();
	RADIO_0();
	_delay_ms(10);
}

void initProtocolRun(const RemoteProtocol* protocol) {
	pulseIndex = 0;
	pulseCurrentLength = 0;
	pulseNeededLength = protocol->syncLength;
	RADIO_S(!protocol->isInverted);
}

volatile Door* volatile currentOpeningDoor;

ISR (TIMER1_COMPA_vect) {

	if (currentOpeningDoor == NULL)
		return;
	if (currentOpeningDoor->state != OPENING)
		return;
	
	pulseCurrentLength++;

	if (pulseCurrentLength >= *pulseNeededLength) {
		RADIO_T();
		pulseIndex++;
		pulseCurrentLength = 0;
		if ((pulseIndex%2) == 0) {
			char code_bit = currentCode[(pulseIndex/2)-1];
			if (code_bit != '\0') {
				pulseNeededLength = currentOpeningDoor->protocol->bitLengths[code_bit - '0'];
			} else {
				repeatIndex++;
				if (repeatIndex >= currentOpeningDoor->repeats) {
					DISABLE_RADIO();
					currentOpeningDoor->delayAfterLeft = currentOpeningDoor->delayAfterSetting;
					currentOpeningDoor->state = DELAY_AFTER;
					DISABLE_TIMER1();
				} else {
					pulseIndex = 0;
					pulseCurrentLength = 0;
					pulseNeededLength = currentOpeningDoor->protocol->syncLength;
					RADIO_S(!currentOpeningDoor->protocol->isInverted);
				}
			}
		} else {
			pulseNeededLength++;
		}
	}

}

void startOpeningDoor(volatile Door* door) {
	door->state = OPENING;
	currentOpeningDoor = door;
	repeatIndex = 0;
	strcpypgm(currentCode, door->code);
	initProtocol(door->protocol);
	initProtocolRun(door->protocol);
	ENABLE_TIMER1();
}

/* General things */

void initGpio() {
	
	DDR_LED1 |= (1<<PIN_LED1);
	DDR_LED2 |= (1<<PIN_LED2);
	DDR_RADIO_ENABLE |= (1<<PIN_RADIO_ENABLE);
	DDR_RADIO |= (1<<PIN_RADIO);
	DDR_BUTTON1 &= ~(1<<PIN_BUTTON1);
	DDR_BUTTON2 &= ~(1<<PIN_BUTTON2);
	
	DISABLE_RADIO();
	PIN_OFF(PORT_LED1,PIN_LED1);
	PIN_OFF(PORT_LED2,PIN_LED2);
	
	MCUCR = (1<<ISC11)|(1<<ISC01);
	GIMSK = (1<<INT1)|(1<<INT0);
	
}

#define MAIN_LOOP_DELAY 10
#define EMPTY_TIME_BEFORE_SLEEP 1000

typedef void (*LED_SET_FN)();
void handleDoor(volatile Door* door, unsigned char secPassed, LED_SET_FN feedbackLedOnFn, LED_SET_FN feedbackLedOffFn, LED_SET_FN feedbackLedToggleFn, unsigned char* feedBackLedBlinkVar);
void blinkLed(LED_SET_FN feedbackLedToggleFn, unsigned char* feedBackLedBlinkVar, unsigned char divider);

int main(void)
{
	
	currentOpeningDoor = NULL;
	myTime = 0;
	set_sleep_mode(SLEEP_MODE_IDLE);
    initGpio();
	initTimer1();
	
	for (int i = 0; i < 10; i++) {
		led1Toggle();
		led2Toggle();
		_delay_ms(250);
	}
	
	sei();
	
	unsigned char emptyTime = 0;
	unsigned int secTimer = 0;
	unsigned char secPassed = 0;
	unsigned char led1Blink = 0, led2Blink = 0;
	
	while(1) {
		
		ATOMIC_BLOCK(ATOMIC_FORCEON) {
			myTime += MAIN_LOOP_DELAY;
		}
		_delay_ms(MAIN_LOOP_DELAY);
		
		secTimer += MAIN_LOOP_DELAY;
		if (secTimer >= 1000) {
			secPassed = 1;
			secTimer -= 1000;
		}
		
		handleDoor(&door1, secPassed, led1On, led1Off, led1Toggle, &led1Blink);
		handleDoor(&door2, secPassed, led2On, led2Off, led2Toggle, &led2Blink);
		
		if ((door1.state == IDLE) && (door2.state == IDLE)) {
			emptyTime += MAIN_LOOP_DELAY;
			if (emptyTime >= EMPTY_TIME_BEFORE_SLEEP) {
				emptyTime = 0;
				myTime = 0;
				sleep_mode();
			}
		} else {
			emptyTime = 0;
		}
		
		secPassed = 0;
		
	}
	
}

void handleDoor(volatile Door* door, unsigned char secPassed, LED_SET_FN feedbackLedOnFn, LED_SET_FN feedbackLedOffFn, LED_SET_FN feedbackLedToggleFn, unsigned char* feedBackLedBlinkVar) {
	if (door->state == DELAY_BEFORE) {
		if (secPassed)
			door->delayBeforeLeft--;
		if (door->delayBeforeLeft <= 0)
			door->state = QUEUED;
		blinkLed(feedbackLedToggleFn, feedBackLedBlinkVar, 16);
	} else if (door->state == QUEUED) {
		if (currentOpeningDoor == NULL) {
			startOpeningDoor(door);
			feedbackLedOnFn();
		} else {
			blinkLed(feedbackLedToggleFn, feedBackLedBlinkVar, 8);
		}
	} else if (door->state == DELAY_AFTER) {
		if (secPassed)
			door->delayAfterLeft--;
		if (door->delayAfterLeft <= 0) {
			currentOpeningDoor = NULL;
			door->state = IDLE;
			feedbackLedOffFn();
		}
	}
}

void blinkLed(LED_SET_FN feedbackLedToggleFn, unsigned char* feedBackLedBlinkVar, unsigned char divider) {
	(*feedBackLedBlinkVar)++;
	if (*feedBackLedBlinkVar >= divider) {
		feedbackLedToggleFn();
		*feedBackLedBlinkVar = 0;
	}
}


