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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//unsigned char tmpB;

enum States {Start, A, B, C, D, buttPress, Wait} state; 
unsigned char tmpA; 
unsigned char tmpB;
unsigned char ableToPause = 1; //Bool value. Used to determine if able to be paused  

void Tick() {
	switch(state) { //transitions:
		case Start: 
			state = A;
			break;

		case A: 
			if ( ((tmpA & 0x01) == 0x01) && (ableToPause == 1)  ) { //button pressed 1st time
				state = buttPress;
				ableToPause = 0;	
			}

			//After releasing the button after the second time, able to be paused  normally as above  
			else if ( ((tmpA & 0x01) == 0x00) && (ableToPause == 0)) {
				//state = buttPress;
				state = B;
				ableToPause = 1;
				
			}
			
			else { //else go to next part of sequence
				state = B;
				//firstPress = 1; //ADDED 
			}
			break;

		case B:   
			if ( ((tmpA & 0x01) == 0x01) && (ableToPause == 1)  ) { //button pressed 1st time
				state = buttPress;
				ableToPause = 0;	
			}

			//After releasing the button after the second time, able to be paused  normally as above  
			else if ( ((tmpA & 0x01) == 0x00) && (ableToPause == 0)) {
				//state = buttPress;
				state = C;
				ableToPause = 1;	
			}
			
			else { //else go to next part of sequence
				state = C;
			}
			break;

		case C:      
			if ( ((tmpA & 0x01) == 0x01) && (ableToPause == 1)  ) { //button pressed 1st time
				state = buttPress;
				ableToPause = 0;	
			}

			//After releasing the button after the second time, able to be paused  normally as above  
			else if ( ((tmpA & 0x01) == 0x00) && (ableToPause == 0)) {
				//state = buttPress;
				state = D;
				ableToPause = 1;	
			}
			
			else { //else go to next part of sequence
				state = D;
			}
			break;

		case D:     
			if ( ((tmpA & 0x01) == 0x01) && (ableToPause == 1)  ) { //button pressed 1st time
				state = buttPress;
				ableToPause = 0;	
			}

			//After releasing the button after the second time, able to be paused  normally as above  
			else if ( ((tmpA & 0x01) == 0x00) && (ableToPause == 0)) {
				//state = buttPress;
				state = D;
				ableToPause = 1;	
			}
			
			else { //else go to next part of sequence
				state = A;
			}
			break;

		case buttPress:
			if ( (tmpA & 0x01) == 0x01) {
				state = buttPress;
			}
			else { 
				state = Wait;	
			}
			break;

		case Wait:  
			if ( (tmpA & 0x01) == 0x01 ) { //If button is pressed 
				state = A;
				//tmpC = 0x01;	
			}
			else { //else if button not pressed, just stay 
				state = Wait;
			}
			break;
			
		default: 
			state = A;
			break;
	}

	switch (state) { //actions
		case A:  	// ... 001
			tmpB = (tmpB & 0x00) | 0x01; 
			break;

		case B: 	//010
			tmpB = (tmpB & 0x00) | 0x02;
			break;

		case C: 	//100 
			tmpB = (tmpB & 0x00) | 0x04;
			break;

		case D: 	//010
			tmpB = (tmpB & 0x00) | 0x02;
			break;

		case buttPress: 
			//tmpB = (tmpB & 0x0F) | 0x08; //for debugging 
						    //Sets PB3 to 1 
			break;

		case Wait: 
			break;

		default: 
			break;

	}
}


int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF; //Makes all of port a as input 
	DDRB = 0xFF; PORTB = 0x00; //Makes all of Port B as output 
	TimerSet(300); //300 ms period 
	TimerOn();

	tmpA = 0x00;
	tmpB = 0x00;
	state = Start;

    /* Insert your solution below */
	while (1) {
		tmpA = ~PINA;

		Tick();
		PORTB = tmpB;  
	
	    	while (!TimerFlag) {} 
	    	TimerFlag = 0;

	}
    return 1;
}
