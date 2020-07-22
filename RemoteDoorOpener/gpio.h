#ifndef GPIO_H_INCLUDED
#define GPIO_H_INCLUDED

#include <avr/io.h>

#define DDR_RADIO_ENABLE     DDRD
#define PORT_RADIO_ENABLE    PORTD
#define PIN_RADIO_ENABLE     PD5
#define DDR_RADIO            DDRD
#define PORT_RADIO           PORTD
#define PIN_RADIO            PD4
#define DDR_LED1             DDRB
#define PORT_LED1            PORTB
#define PIN_LED1             PB0
#define DDR_LED2             DDRB
#define PORT_LED2            PORTB
#define PIN_LED2             PB1
#define DDR_BUTTON1          DDRD
#define PIN_BUTTON1          PD2
#define DDR_BUTTON2          DDRD
#define PIN_BUTTON2          PD3

#define PIN_ON(PORT,PIN)       PORT |= (1<<PIN)
#define PIN_OFF(PORT,PIN)      PORT &= ~(1<<PIN)
#define PIN_TOGGLE(PORT,PIN)   PORT ^= (1<<PIN)
#define PIN_STATE(PORT,PIN)    ((PORT >> PIN) & 1)

#define ENABLE_RADIO()         PIN_ON(PORT_RADIO_ENABLE,PIN_RADIO_ENABLE)
#define DISABLE_RADIO()        PIN_OFF(PORT_RADIO_ENABLE,PIN_RADIO_ENABLE)
#define RADIO_0()              (PIN_OFF(PORT_RADIO,PIN_RADIO))
#define RADIO_1()              (PIN_ON(PORT_RADIO,PIN_RADIO))
#define RADIO_T()              (PIN_TOGGLE(PORT_RADIO,PIN_RADIO))
#define RADIO_S(V)             ((V) ? (RADIO_1()) : (RADIO_0()))

void led1On();
void led1Off();
void led1Toggle();
void led2On();
void led2Off();
void led2Toggle();

#endif // GPIO_H_INCLUDED