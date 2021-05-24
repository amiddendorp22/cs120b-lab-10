/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *	Video Demo: https://youtu.be/ug0bTLsCk9U
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
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

unsigned char led0_output = 0x00;
unsigned char led1_output = 0x00;
unsigned char led2_output = 0x00;
unsigned char led3_output = 0x00;

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
			output = led0_output | led1_output << 1 | led2_output << 2| led3_output << 3;
			break;
	}

	PORTB = output;
	return state;
}

enum keypad_light { init_state, start_state, one_state, two_state, three_state, four_state, five_state};

int keypadLightSMTick(int state)
{
	unsigned char x = GetKeypadKey();
	
	switch(state)
	{
		case(init_state):
			if(x == '#')
			{
				state = start_state;
			}
			else
			{
				state = init_state;
			}
			break;
		case(start_state):
			if(x == '#')
			{
				state = start_state;
			}
			else if(x == '1')
			{
				state = one_state;
			}
			else if(x == '\0')
			{
				state = start_state;
			}
			else
			{
				state = init_state;
			}
			break;
		case(one_state):
			if(x == '#')
			{
				state = start_state;
			}
			else if(x == '2')
			{
				state = two_state;
			}
			else if(x == '\0')
			{
				state = one_state;
			}
			else
			{
				state = init_state;
			}
			break;
		case(two_state):
			if(x == '#')
			{
				state = start_state;
			}
			else if(x == '3')
			{
				state = three_state;
			}
			else if(x == '\0')
			{
				state = two_state;
			}
			else
			{
				state = init_state;
			}
			break;
		case(three_state):
			if(x == '#')
			{
				state = start_state;
			}
			else if(x == '4')
			{
				state = four_state;
			}
			else if(x == '\0')
			{
				state = three_state;
			}
			else
			{
				state = init_state;
			}
			break;
		case(four_state):
			if(x == '#')
			{
				state = start_state;
			}
			else if(x == '5')
			{
				state = five_state;
			}
			else if(x == '\0')
			{
				state = four_state;
			}
			else
			{
				state = init_state;
			}
			break;
		case(five_state):
			state = init_state;
			break;
		default:
			state = init_state;
			break;
	}


	switch(state)
	{
		case(init_state):
			led2_output = 0x01;
			break;
		case(start_state):
			led1_output = 0x01;
			led2_output = 0x00;
			break;
		case(one_state):
			led1_output = 0x00;
			break;
		case(two_state):
			led1_output = 0x01;
			break;
		case(three_state):
			led1_output = 0x00;
			break;
		case(four_state):
			led1_output = 0x01;
			break;
		case(five_state):
			led0_output = 0x01;
			led1_output = 0x00;
			break;
	}


	return state;
}

enum locking_mechanism { buttonPressed, buttonNotPressed};

int lockingSMFct(int state)
{

	unsigned char pin7 = ~PINA & 0x01; //grabs PA0 (button inside house)
	switch(state)
	{
		case(buttonPressed):
			if(pin7 == 0x01)
			{
				state = buttonPressed;
			}
			else
			{
				state = buttonNotPressed;
			}
			break;
		case(buttonNotPressed):
			if(pin7 == 0x01)
			{
				state = buttonPressed;
			}
			else
			{
				state = buttonNotPressed;
			}
			break;
		default:
			state = buttonNotPressed;
			break;
	}

	switch(state)
	{
		case(buttonPressed):
			led0_output = 0x00;
			break;
		case(buttonNotPressed):
			break;

	}

	return state;
}



void set_PWM(double frequency)
{
	static double current_frequency;
	if(frequency != current_frequency)
	{
		if(!frequency)
		{
			TCCR3B &= 0x08;
		}
		else
		{
			TCCR3B |= 0x03;
		}
	
		if(frequency < 0.954)
		{
			OCR3A = 0xFFFF;
		}
		else if(frequency > 31250)
		{
			OCR3A = 0x0000;
		}
		else
		{
			OCR3A = (short)(8000000 / (128 * frequency)) - 1;
		}
		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on()
{
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off()
{
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum Melody_States {Mel_Init, Mel_1, Mel_2, Mel_3, Mel_4, Mel_5, Mel_6, Mel_7, Mel_8, Mel_9, Mel_10, Mel_11, Mel_12, Mel_13, Mel_14, Mel_15};

int TickFct_Melody(int state)
{
	unsigned char tmpA = ~PINA & 0x80;
	switch(state)
	{
		case(Mel_Init):
			if(tmpA == 0x80)
			{
				state = Mel_1;
			}
			else
			{
				state = Mel_Init;
			}
			break;
		case(Mel_1):
			state = Mel_2;
			break;
		case(Mel_2):
                        state = Mel_3;
                        break;
		case(Mel_3):
                        state = Mel_4;
                        break;
		case(Mel_4):
                        state = Mel_5;
                        break;
		case(Mel_5):
                        state = Mel_6;
                        break;
		case(Mel_6):
                        state = Mel_7;
                        break;
		case(Mel_7):
                        state = Mel_8;
                        break;
		case(Mel_8):
                        state = Mel_9;
                        break;
		case(Mel_9):
                        state = Mel_10;
                        break;
		case(Mel_10):
                        state = Mel_11;
                        break;
		case(Mel_11):
			state = Mel_12;
			break;
		case(Mel_12):
			state = Mel_13;
			break;
		case(Mel_13):
			state = Mel_14;
			break;
		case(Mel_14):
			state = Mel_15;
			break;
		case(Mel_15):
			state = Mel_Init;
			break;
		default:
			state = Mel_Init;
			break;
	}

	switch(state)
	{
		case(Mel_Init):
			set_PWM(0);
			break;
		case(Mel_1):
			led3_output = 0x01;
			set_PWM(261.63);
			break;
		case(Mel_2):
			set_PWM(349.23);
			break;
		case(Mel_3):
			set_PWM(293.66);
			break;
		case(Mel_4):
			set_PWM(329.63);
			break;
		case(Mel_5):
			set_PWM(392.00);
			break;
		case(Mel_6):
			set_PWM(523.25);
			break;
		case(Mel_7):
			set_PWM(440.00);
			break;
		case(Mel_8):
			set_PWM(493.88);
			break;
		case(Mel_9):
			set_PWM(261.63);
			break;
		case(Mel_10):
			set_PWM(523.25);
			break;
		case(Mel_11):
			set_PWM(493.88);
			break;
		case(Mel_12):
			set_PWM(349.23);
			break;
		case(Mel_13):
			set_PWM(440.00);
			break;
		case(Mel_14):
			set_PWM(329.63);
			break;
		case(Mel_15):
			set_PWM(392.00);
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
	DDRB = 0xFF; PORTB = 0x00; //all but last pin is output, last pin is input (button inside house)
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xF0; PORTC = 0x0F;
    /* Insert your solution below */

	static task task1, task2, task3, task4;
	task *tasks[] = {&task1, &task2, &task3, &task4};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;

	task1.state = start;
	task1.period = 300;
	task1.elapsedTime = task1.period;
	task1.TickFct = &keypadLightSMTick;

	task2.state = start;
	task2.period = 200;
	task2.elapsedTime = task2.period;
	task2.TickFct = &lockingSMFct;

	task3.state = start;
	task3.period = 200;
	task3.elapsedTime = task3.period;
	task3.TickFct = &TickFct_Melody;


	task4.state = start;
	task4.period = 10;
	task4.elapsedTime = task4.period;
	task4.TickFct = &displaySMTick;

	
	unsigned long GCD = tasks[0]->period;
	unsigned short i;

	for(i = 0; i < numTasks; i++)
	{
		GCD = findGCD(GCD,tasks[i]->period);
	}

	TimerSet(GCD);
	TimerOn();
	PWM_on();
//	PORTB = 0x01;
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
