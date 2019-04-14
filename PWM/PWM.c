#include <avr/io.h>
#include <stdint.h>
#include "util/delay.h"


unsigned char number[] = 
{
	0b01111111, 
	0b00001110, 
	0b10110111, 
	0b10011111, 

};

unsigned char i = 0; 
unsigned char j =0;
void init_pwm (void)
{
	TCCR1A=(1<<COM1A1)|(1<<WGM10); /
	TCCR1B=(1<<CS10);		 
	OCR1A=0x00;			
}

int main(void)
{
	
	 DDRB  = 0b00111111;  
	 PORTB = 0b11000010;  
	 init_pwm();
	 
	 DDRD  = 0b11111011;  
	 PORTD = number[0];
	  
	   
	
    while(1) 
    {
	  if ((PIND&(1<< PD2)) == 0){  
		    i=0; 
		    _delay_ms(300); 
			OCR1A=0x00; 
			}
	   else if ((PINB&(1<< PB7)) == 0 && (PINB&(1<< PD6)) == 0 ){ 		  
	   }
	  else if ((PINB&(1<< PB7)) == 0){ 
		  
		  if (i<3){
			  
		      i=i+1;
			  for (j=0; j<85; j++)
			  {
				OCR1A++; _delay_ms(10);  
			  }
	          _delay_ms(300);
			  }
	      }
	  else if ((PINB&(1<< PB6)) == 0){
		  if (i>0)
		  {
			   for (j=0; j<85; j++)
			   {
				   OCR1A--; _delay_ms(10);
			   }
			   i=i-1;
			   _delay_ms(300);
		  }
	  		 
	  }
	 
	PORTD = number[i];
	  }

   
}

