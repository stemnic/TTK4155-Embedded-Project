#include<avr/io.h>
 
int main()
{
	DDRB = 0x00;		//configure portB as input
	DDRC = 0xFF;		//configure portC as output
 
	while(1)
	{
		PORTC = PINB;
	}
	return 0;
}