/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "bit.h"
#include "timer.h"
#endif



unsigned char GetKeypadKey()
{
	PORTC = 0xEF;
	asm("nop");
	if (GetBit(PINC,0) == 0) {return ('1');}
	if (GetBit(PINC,1) == 0) {return ('4');}
	if (GetBit(PINC,2) == 0) {return ('7');}
	if (GetBit(PINC,3) == 0) {return ('*');}

	PORTC = 0xDF;
	asm("nop");
	if (GetBit(PINC,0) == 0) {return ('2');}
	if (GetBit(PINC,1) == 0) {return ('5');}
	if (GetBit(PINC,2) == 0) {return ('8');}
	if (GetBit(PINC,3) == 0) {return ('0');}

	PORTC = 0xBF;
	asm("nop");

	if (GetBit(PINC,0) == 0) {return ('3');}
	if (GetBit(PINC,1) == 0) {return ('6');}
	if (GetBit(PINC,2) == 0) {return ('9');}
	if (GetBit(PINC,3) == 0) {return ('#');}

	PORTC = 0x7F;
	asm("nop");

	if (GetBit(PINC,0) == 0) {return ('A');}
	if (GetBit(PINC,1) == 0) {return ('B');}
	if (GetBit(PINC,2) == 0) {return ('C');}
	if (GetBit(PINC,3) == 0) {return ('D');}
		
	return('\0');

}

typedef struct _task
{
	signed char state;
	unsigned long int period;
	unsigned long int elapsedTime;
	int (*TickFct)(int);
}task;

unsigned char led7_output = 0x00;

enum display_States { display_display };

int displaySMTick(int state)
{
	unsigned char output;
	
	switch(state)
	{
		case (display_display):
			state = display_display;
			break;
		default:
			state = display_display;
			break;
	}

	switch(state)
	{
		case display_display:
			output = led7_output << 7;
			break;
	}

	PORTB = output;
	return state;
}

enum keypad_light { buttonPressed, noButtonPressed};

int keypadLightSMTick(int state)
{
	unsigned char x = GetKeypadKey();
	
	switch(state)
	{
		case(noButtonPressed):
			if(x != '\0') //if a button is pressed
			{
				state = buttonPressed;
			}
			else
			{
				state = noButtonPressed;
			}
			break;
		case(buttonPressed):
			if(x != '\0')
			{
				state = buttonPressed;
			}
			else
			{
				state = noButtonPressed;
			}
			break;
		default:
			state = noButtonPressed;
			break;
	}


	switch(state)
	{
		case(noButtonPressed):
			led7_output = 0x00;
			break;
		case(buttonPressed):
			led7_output = 0x01;
			break;
		default:
			break;
	}

	return state;
}

unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
	unsigned long int c;
	while(1)
	{
		c = a%b;
		if(c==0) {return b;}
		a = b;
		b = c;
	}

	return 0;
}



int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xF0; PORTC = 0x0F;
    /* Insert your solution below */

	static task task1, task2;
	task *tasks[] = {&task1, &task2};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;

	task1.state = start;
	task1.period = 50;
	task1.elapsedTime = task1.period;
	task1.TickFct = &keypadLightSMTick;

	task2.state = start;
	task2.period = 10;
	task2.elapsedTime = task2.period;
	task2.TickFct = &displaySMTick;

	
	unsigned long GCD = tasks[0]->period;
	unsigned short i;

	for(i = 0; i < numTasks; i++)
	{
		GCD = findGCD(GCD,tasks[i]->period);
	}

	TimerSet(GCD);
	TimerOn();


    while (1)
    {
	for(i = 0; i < numTasks; i++)
	{
		if(tasks[i]->elapsedTime == tasks[i]->period)
		{
			tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
			tasks[i]->elapsedTime = 0;
		}
		tasks[i]->elapsedTime += GCD;
	}

	while(!TimerFlag);
	TimerFlag = 0;
   
}

	return 0;
}
