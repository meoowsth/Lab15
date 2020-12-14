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

unsigned char cData = 0x00;
unsigned char keypadOutput = 0x01;

unsigned char speed = 0x01;
unsigned char pattern = 0x01;

unsigned char column;
unsigned char row;

unsigned char letters[4][5] = {
{6, 9, 15, 9, 9},
{14, 9, 14, 9, 14},
{15, 8, 8, 8, 15},
{14, 9, 9, 9, 14}};

unsigned char numbers[6][5] = {
{2, 2, 2, 2, 2},
{7, 1, 7, 4, 7},
{7, 1, 7, 1, 7},
{5, 5, 7, 1, 1},
{7, 4, 7, 1, 7},
{7, 4, 7, 5, 7}};

unsigned char rowArray[5] = {0x01, 0x02, 0x04, 0x08, 0x10};

void SPI_MasterStart(void) {
	DDRB = 0xB0;
	SPCR |= 0x51;
	SREG |= 0x80;
}

void SPI_MasterTransmit(unsigned char cData) {
	SPDR = cData;
	PORTB = 0xA0;
	while(!(SPSR & (1<<SPIF))) {
	}
	DDRB = 0xB0;
}


enum States{Increment};

int transmitTick(int ){
	switch(state){
		case Increment:
			break;
		default:
			state = Increment;
			break;
	}
	switch(state){
		case Increment:
			if(cData == 0xFF){
				cData = 0;
			}
			else{
				cData++;
			}
			SPI_MasterTransmit(cData);
			break;
		default:
			state = Increment;
			break;
	}
	return state;
}

enum Matrix_States {Start, Seq1, Seq2, Seq3, Seq4, Seq5};

int Matrix_Tick(int state) {
	switch(state){
		case Start:
			row = rowArray[0];
			column = (letters[pattern-1][0] << 4) | (numbers[speed-1][0]);
			state = Seq1;
			break;
		case Seq1:
			row = rowArray[0];
			column = (letters[pattern-1][0] << 4) | (numbers[speed-1][0]);
			state = Seq2;
			break;
		case Seq2:
			row = rowArray[1];
			column = (letters[pattern-1][1] << 4) | (numbers[speed-1][1]);
			state = Seq3
			break;
		case Seq3:
			row = rowArray[2];
			column = (letters[pattern-1][2] << 4) | (numbers[speed-1][2]);
			state = Seq4;
			break;
		case Seq4:
			row = rowArray[3];
			column = (letters[pattern-1][3] << 4) | (numbers[speed-1][3]);
			state = Seq5;
			break;
		case Seq5:
			row = rowArray[4];
			column = (letters[pattern-1][4] << 4) | (numbers[speed-1][4]);
			state = Seq1;
			break;
		default:
			state = Start;
			break;
	}
	switch (state) {
		default:
		break;
	}
	PORTC = column;
	PORTD = ~row;
	return state;
}

enum keypad_States {keypad};
int keypadSMTick(int keypadState) {
	unsigned char x;
	switch (keypadState) {
		case keypad:
			x = GetKeypadKey();
			switch (x) {
				case '\0': keypadOutput = 0x1F; break;
				case '1': keypadOutput = 0x01; speed = 0x01; break;
				case '2': keypadOutput = 0x02; speed = 0x02; break;
				case '3': keypadOutput = 0x03; speed = 0x03; break;
				case '4': keypadOutput = 0x04; speed = 0x04; break;
				case '5': keypadOutput = 0x05; speed = 0x05; break;
				case '6': keypadOutput = 0x06; speed = 0x06; break;
				case '7': keypadOutput = 0x07; break;
				case '8': keypadOutput = 0x08; break;
				case '9': keypadOutput = 0x09; break;
				case 'A': keypadOutput = 0x0A; pattern = 0x01; break;
				case 'B': keypadOutput = 0x0B; pattern = 0x02; break;
				case 'C': keypadOutput = 0x0C; pattern = 0x03; break;
				case 'D': keypadOutput = 0x0D; pattern = 0x04; break;
				case '*': keypadOutput = 0x0E; break;
				case '0': keypadOutput = 0x00; break;
				case '#': keypadOutput = 0x0F; break;
				default: keypadOutput = 0x1B; break;
			}
			keypadState = keypad;
			break;
		default:
			keypadState = keypad;
			break;
	    }
	switch (keypadState) {
		default:
		break;
	}
	return keypadState;
}

int main(void) {
    /* Insert DDR and PORT Startializations */
	DDRA = 0xF0; PORTA = 0x0F;
	DDRD = 0xFF; PORTD = 0x00;
	DDRC = 0xFF; PORTC = 0x00;

    /* Insert your solution below */
    static task task1, task2, task3;
  	task *tasks[] = {&task1, &task2, &task3};
  	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

  	const char start = -1;

  	task1.state = start;
  	task1.period = 50;
  	task1.elapsedTime = task1.period;
  	task1.TickFct = &keypadSMTick;

  	task2.state = 0;
  	task2.period = 1;
  	task2.elapsedTime = task2.period;
  	task2.TickFct = &Matrix_Tick;

  	task3.state = 1;
  	task3.period = 1000;
  	task3.elapsedTime = task3.period;
  	task3.TickFct = &transmitTick;

  	unsigned long GCD = tasks[0]->period;
  	unsigned short i;
  	for (i = 1; i < numTasks; i++)
  	{
  		GCD = findGCD(GCD, tasks[i]->period);
  	}
  	SPI_MasterStart();
  	TimerSet(GCD);
  	TimerOn();

	while (1) {
		for (i = 0; i < numTasks; i++){
			if (tasks[i]->elapsedTime == tasks[i]->period){
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += GCD;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 1;
}
