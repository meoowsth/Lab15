/*	Author: Tinghui Song
 *  Partner(s) Name: 
 *	Lab Section: 24
 *	Assignment: Lab #15  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo link: https://drive.google.com/file/d/1TQVRcdnLuowcfqep01WGdtRAlaN_yRlV/view?usp=sharing
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "bit.h"
#include "keypad.h"
#include "scheduler.h"
#include "Timer.h"
#endif
#include <avr/interrupt.h>
unsigned char received = 0;


void SPI_SlaveInit(void) {
	DDRB = 0x40;
	SPCR |= 0xC0;
	SREG |= 0x80;
}

ISR(SPI_STC_vect) {
    received = SPDR;
}

int main(void) {
    /* Insert DDR and PORT initializations */
	   DDRA = 0xFF; PORTA = 0x00;

    /* Insert your solution below */
    SPI_SlaveInit();
    TimerSet(1);
    TimerOn();

	while (1) {
		PORTA = received;
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 1;
}

