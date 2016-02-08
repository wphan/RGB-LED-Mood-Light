/*
 * 2016_val_rgb_light.c
 *
 * Created: 2016-01-31 1:57:38 PM
 * Author : William
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

volatile uint8_t LED_DUTY[3][2] = { {255, 0}, {255, 0}, {255, 0} };		// duty for each color: { {RED_CUR, RED_TARGET}, {GRN_CUR, GRN_TARGET}, {BLU_CUR, BLU_TARGET) }
volatile uint8_t TARGET_REACHED[3] = {1, 0, 1};							// flag for each color: {REACHED_RED, REACHED_GREEN, REACHED_BLUE} 

int main(void)
{
	// set PD5, PD6 and PB3 as outputs
	DDRD |= (1 << DDD5); //OC0B
	DDRD |= (1 << DDD6); //OC0A
	DDRB |= (1 << DDB3); //OC2A
	
	// set duty cycles
	OCR0A = LED_DUTY[0][0]; // red
	OCR0B = LED_DUTY[1][0]; // green
	OCR2A = LED_DUTY[2][0]; // blue
	
	//set none-inverting mode
	TCCR0A |= (1 << COM0A1) | (0 << COM0A0);
	TCCR0A |= (1 << COM0B1) | (0 << COM0B0);
	TCCR2A |= (1 << COM2A1);
	
	// set fast PWM Mode, 8bit  fast mode for timer1
	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	TCCR0B |= (1 << CS01);
	TCCR2A |= (1 << WGM21) | (1 << WGM20);
	
	// set prescaler to 8
	TCCR0B |= (1 << CS01);
	TCCR2B |= (1 << CS21);
	
	// Timer 1 compare value: OCR1A = (clockspeed/prescaler * time_in_sec) -1
	OCR1A = 0x04FF;		//this is a good final speed, nice and slow
	//OCR1A = 0x007F; //use this for testing, faster
	
	// mode 4, CTC on OCR1A
	TCCR1B |= (1 << WGM12);
	
	// set interrupt on compare match
	TIMSK1 |= (1 << OCIE1A);
	
	// set prescaler to 1024 and starts the timer
	TCCR1B |= (1 << CS12) | (1 << CS10);
	
	// enable interrupts
	sei();
	
    /* Replace with your application code */
    while (1) 
    {
		int i;
		
		OCR0A = LED_DUTY[0][0];
		OCR0B = LED_DUTY[1][0];
		OCR2A = LED_DUTY[2][0];
		
		//do da randoms
		srand(LED_DUTY[0][0]);
		
		for (i = 0; i < 3; i ++)
		{
			if (TARGET_REACHED[i])
			{
				LED_DUTY[i][1] = rand();
				TARGET_REACHED[i] = 0;
			}
		}
    }
	
	return 1;
}

ISR (TIMER1_COMPA_vect)
{
	int i;
	
	for (i = 0; i < 3; i ++)
	{
		if (!TARGET_REACHED[i])
		{
			if (LED_DUTY[i][1] == LED_DUTY[i][0])
			{
				TARGET_REACHED[i] = 1;
			}
			else if (LED_DUTY[i][1] > LED_DUTY[i][0])
			{
				LED_DUTY[i][0] += 1;
			}
			else
			{
				LED_DUTY[i][0] -= 1;
			}
		}
	}
}