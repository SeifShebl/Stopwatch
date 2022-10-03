#include <avr/io.h>
#include <avr/interrupt.h>
#include<util/delay.h>

unsigned char sec = 0; //seconds counter
unsigned char min = 0; //minutes counter
unsigned char hour = 0; //hours counter

ISR(TIMER1_COMPA_vect) {
	sec++;
	if (sec == 60) {
		sec = 0;
		min++;
	}
	if (min == 60) {
		min = 0;
		hour++;
	}
}

ISR(INT0_vect) { //interupt 0 handler ( RESET )
	sec = 0;
	min = 0;
	hour = 0;
}

ISR(INT1_vect) { //interrupt 1 handler ( PAUSE )
	TCCR1B &= ~((1 << CS12) | (1 << CS10)); //stop clock
}

ISR(INT2_vect) { //interrupt 2 handler ( RESUME )
	TCCR1B |= (1 << CS12) | (1 << CS10); //resume clock
}
void Int0_Init() {
	DDRD &= ~(1 << PD2); //PD2 input
	PORTD |= (1 << PD2); //enable internal pull up
	MCUCR |= (1 << ISC01); //falling edge
	GICR |= (1 << INT0); //external interrupt request 0 enable
}

void Int1_Init() {
	DDRD &= ~(1 << PD3); //PD3 input
	MCUCR |= (1 << ISC10) | (1 << ISC11); //rising edge
	GICR |= (1 << INT1); //external interrupt request 1 enable
}
void Int2_Init() {
	DDRB &= ~(1 << PB2); //PB2 input
	PORTB |= (1 << PB2); //enable internal pull up
	MCUCSR &= ~(1 << ISC2); //falling edge
	GICR |= (1 << INT2); //external interrupt request 2 enable

}

void Timer1_CTC_Init(void) {
	TCCR1A |= (1 << FOC1A); //Non PWM mode
	TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10); //CTC Mode And Prescaler=1024
	TCNT1 = 0; //begin counting from zero
	OCR1A = 976; // (1 sec)/ (1024/1MHz)= 976
	TIMSK |= (1 << OCIE1A); // Output compare A match Interrupt enable
}

//function to adjust the output of the six seven segments
void adjust_seven_segments(char pin, char time) {
	PORTA = (1 << pin);
	PORTC = time % 10;
	;
	_delay_ms(2);
}

int main(void) {
	SREG |= (1 << 7); //I bit enable
	DDRC |= 0X0F; //First 4 pins in PortC are Output to 7447 decoder
	PORTC &= 0xF0; //initialize them with zero
	DDRA |= 0x3F; //First 6 pins of PortA are Output to on/off 7 segments
	PORTA &= 0xC0; //initialize them with zero
	Timer1_CTC_Init();
	Int0_Init();
	Int1_Init();
	Int2_Init();
	while (1) {
		adjust_seven_segments(PA0, sec); //1st 7 segments
		adjust_seven_segments(PA1, sec / 10); //2nd 7 segments
		adjust_seven_segments(PA2, min); //3rd 7 segments
		adjust_seven_segments(PA3, min / 10); //4th 7 segments
		adjust_seven_segments(PA4, hour); //5th 7 segments
		adjust_seven_segments(PA5, hour / 10); //6th 7 segments

	}
}
