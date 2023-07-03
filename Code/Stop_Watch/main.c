/*
 * Mini_Project_2.c
 *
 *  Created on: Sep 16, 2021
 *      Author: Hana
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
unsigned char SEC1 =0;
unsigned char SEC2 =0;
unsigned char MIN1 =0;
unsigned char MIN2 =0;
unsigned char HOUR1 =0;
unsigned char HOUR2 =0;
unsigned char flag =0;

void Timer1(void)
{
	TCNT1=0;       //Set timer1 initial count to zero
	/* Set FOC1A for non-PWM mode
	 * Set WGM12 for CTC mode
	 * Choosing prescaler = 1024 by setting CS10 & CS12
	 * Set compare value to 1000 (1 second) in OCR1A
	 * Match interrupt enable
	 */
	TCCR1A |= (1<<FOC1A);
	TCCR1B |= (1<<WGM12) |(1<<CS10) |(1<<CS12);
	OCR1A=1000;
	TIMSK |=(1<<OCIE1A);
}
ISR(TIMER1_COMPA_vect)
{
	flag=1;
}

void INT0_Init(void)
{
	DDRD &= ~(1<<PD2);
	PORTD|=(1<<PD2);     //internal pull-up
	GICR |= (1<<INT0);   //enable external interrupt INT0
	MCUCR |= (1<<ISC01); //Trigger INT0 with the falling edge
}
ISR(INT0_vect)
{
	//reset the stop watch
	SEC1=0;
	SEC2=0;
	MIN1=0;
	MIN2=0;
	HOUR1=0;
	HOUR2=0;
}
void INT1_Init(void)
{
	DDRD &= ~(1<<PD3);
	GICR |= (1<<INT1);        //enable external interrupt INT1
	MCUCR |= (1<<ISC10) | (1<<ISC11); //Trigger INT1 with the rising edge
}
ISR(INT1_vect)
{
	//Pause stop watch
	TCCR1B &=~(1<<CS10) &~(1<<CS12)	; //Counter is stopped
}
void INT2_Init(void)
{
	DDRB &= ~(1<<PB2);
	PORTB|=(1<<PB2);    //internal pull-up
	GICR |= (1<<INT2);  //enable external interrupt INT2
	MCUCSR &=~(1<<ISC2); //Trigger INT2 with the falling edge
}
ISR(INT2_vect)
{
	//resume counting
	TCCR1B |= (1<<CS10) |(1<<CS12);  //selecting clock source with prescaler = 1024
}
int main()
{
	DDRC|=0x0F; //setting first four pins in PORT C as output pins
	PORTC&=0xF0; //initialization
	DDRA|=0x3F; //setting first 6 pins in PORT A as output pins
	PORTA|=0x3F; //initialization
	SREG |= (1<<7); //Enable Global interrupts in MC
	INT0_Init(); //Enable external INT0
	INT1_Init(); //Enable external INT1
	INT2_Init(); //Enable external INT2
	Timer1();
	while(1)
	{
		/*if the flag = 1 this means that the counter counted 1 second */
		if(flag==1)
		{
			//increment 7-seg devices
			SEC1++;
			if(SEC1 ==10)
			{
				SEC1=0;
				SEC2++;
				_delay_ms(5);
			}
			if(SEC2 ==6)
			{
				SEC2=0;
				MIN1++;
				_delay_ms(5);
			}
			if(MIN1 == 10)
			{
				MIN1=0;
				MIN2++;
				_delay_ms(5);
			}
			if(MIN2 == 6)
			{
				MIN2=0;
				HOUR1++;
				_delay_ms(5);
			}
			if(HOUR1 ==10)
			{
				HOUR1=0;
				HOUR2++;
				_delay_ms(5);
			}
			if(HOUR2 ==10)
			{
				HOUR2=0;
				_delay_ms(5);
			}
			flag=0;
		}
		else
		{

			PORTA =0x01;
			PORTC = (PORTC & 0xF0) | (SEC1 & 0x0F);
			_delay_ms(5);
			PORTA =0x02;
			PORTC = (PORTC & 0xF0) | (SEC2 & 0x0F);
			_delay_ms(5);
			PORTA =0x04;
			PORTC = (PORTC & 0xF0) | (MIN1 & 0x0F);
			_delay_ms(5);
			PORTA =0x08;
			PORTC = (PORTC & 0xF0) | (MIN2 & 0x0F);
			_delay_ms(5);
			PORTA =0x10;
			PORTC = (PORTC & 0xF0) | (HOUR1 & 0x0F);
			_delay_ms(5);
			PORTA =0x20;
			PORTC = (PORTC & 0xF0) | (HOUR2 & 0x0F);
			_delay_ms(5);
		}
	}

}
