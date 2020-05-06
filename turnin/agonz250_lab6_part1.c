/*	Author: agonz250
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab # 6  Exercise # 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	LINK: 
 *
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0; //stuff added 

//enum States {Start, PB0, PB1, PB2} state; 

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	//AVR timer/counter controller register ....
	TCCR1B = 0x0B;

	//AVR output compare register ....
	OCR1A = 125;

	//AVR timer interrupt mask register
	TIMSK1 = 0x02;

	//initialize avr counter
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	//TimerIRS called every _avr ... milliseconds
	//
	
	//enable global interrupts
	SREG |= 0x80; //0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00; //timer off
}

void TimerISR() {
	TimerFlag = 1;
}

//TimerISR() calls this 
ISR(TIMER1_COMPA_vect) {
	//cpu calls when TCNT1 == OCR1
	_avr_timer_cntcurr--;	//counts down to 0

	if (_avr_timer_cntcurr == 0) {
		TimerISR();	//calls ISR that user uses
		_avr_timer_cntcurr = _avr_timer_M;

	}
}

//Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

//unsigned char tmpB;

enum States {Start, PBX, PBY, PBZ} state; 
unsigned char tmpB;

void Tick() {
	switch(state) { //transitions:
		case Start: 
			state = PBX;
			break;

		case PBX: 
			state = PBY;
			break;

		case PBY:
			state = PBZ;
			break;

		case PBZ:
			state = PBX;
			break;


		default: 
			state = PBX;
			break;
	}

	switch (state) { //actions
		case PBX:  	// ... 001
			tmpB = (tmpB & 0x00) | 0x01; 
			break;

		case PBY: 
			tmpB = (tmpB & 0x00) | 0x02;
			break;

		case PBZ: 
			tmpB = (tmpB & 0x00) | 0x04;
			break;

		default: 
			break;

	}
}


int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF; PORTB = 0x00; //Makes all of Port B as output 
	TimerSet(1000);
	TimerOn();

	tmpB = 0x00;
	state = Start;

    /* Insert your solution below */
	while (1) {
	    	//tmpB = ~tmpB;	//temporary bad programming style 
	    	//PORTB = tmpB; 

	   	// tmpB = 
	 

		Tick();
		PORTB = tmpB; //not sure if should just put above 
	
	    	while (!TimerFlag) {} 
	    	TimerFlag = 0;

	}
    return 1;
}
