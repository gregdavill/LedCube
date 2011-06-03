/*----------------------------------------------------------------------*/
/* FAT file system sample project for FatFs            (C)ChaN, 2010    */
/*----------------------------------------------------------------------*/


#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include "uart.h"
#include "xitoa.h"

char Line[128];				/* Console input buffer */

volatile char ledCycle;

void setCol(char);
void setRow(char);


char lookupR[8]= {1,8,87,8,76,56,87,190};
char lookupG[8]= {2,16,72,50,12,13,94,210};
char lookupB[8]= {4,32,03,250,180,120,30,72};

#define RED (0xC0)
#define GREEN (0x30)
#define BLUE (0x0C)



//PORTC
#define ROW_CLK (1<<1)
#define ROW_DATA (1<<0)
#define ROW_RST (1<<2)

#define COL_CLK (1<<4)
#define COL_DATA (1<<3)
#define COL_RST (1<<5)

//PORTD
#define RXD (1<<0)
#define TXD (1<<1)
#define SW1 (1<<2)
#define SW2 (1<<3)



ISR(TIMER0_OVF_vect)
{
  
  setCol(0xFF);
  setRow(lookupR[ledCycle]);
setRow(lookupG[ledCycle]);
setRow(lookupB[ledCycle]);
  setCol(~(1 << ledCycle));
  
  if(ledCycle++ >= 8)
  	ledCycle = 0;
} 



void get_line (char *buff, int len)
{
	char c;
	int i = 0;


	for (;;) {
		c = uart_get();
		if (c == '\r') break;
		if ((c == '\b') && i) {
			i--;
			uart_put(c);
			continue;
		}
		if (c >= ' ' && i < len - 1) {	//Visible chars 
			buff[i++] = c;
			xputc(c);
		}
	}
	buff[i] = 0;
	uart_put('\r');
	uart_put('\n');
}



//void setCol(char);
void setCol(char data)
{
	unsigned char i;
	for(i = 0;i<8;i++){
		PORTC &= ~COL_DATA;
		if((data & 1) == 1)
			PORTC |= COL_DATA;
		PORTC |= COL_CLK;
		PORTC &= ~COL_CLK;
		data >>= 1;
	}
}

void setRow(char data)
{
	unsigned char i;
	for(i = 0;i<8;i++){
		PORTC &= ~ROW_DATA;
		if((data & 1) == 1)
			PORTC |= ROW_DATA;
		PORTC |= ROW_CLK;
		PORTC &= ~ROW_CLK;
		data >>= 1;
	}
}

static
void IoInit ()
{
	DDRC = ROW_DATA | ROW_CLK | ROW_RST | COL_DATA | COL_CLK | COL_RST;	// Port A
	PORTC = ROW_RST | COL_RST;

	DDRD = TXD;
	PORTD = RXD | TXD;
	
	

	uart_init();		// Initialize UART driver

/*
	OCR1A = 51;			// Timer1: LCD bias generator (OC1B)
	OCR1B = 51;
	TCCR1A = 0b00010000;
	TCCR1B = 0b00001010;
*/
	OCR2A = 90-1;		// Timer2: 100Hz interval (OC2)
	TCCR2A = 0b00000010;
	TCCR2B = 0b00000101;

	TCCR0A = 0;
	TCCR0B = 1<<CS01;
	TIMSK0 = 1<<TOIE0;	// Enable TC2.oc, interrupt

	//rtc_init();			// Initialize RTC

	sei();
}



#define CMD_VERSION 0x00
#define CMD_PGM_DATAFLASH 0x10
#define CMD_READ_DATAFLASH 0x11
#define CMD_GET_DATAFLASH 0x12


int main (void)
{
	

	IoInit();
	
	xfunc_out = uart_put;
	//xputs(PSTR("\rCube CLI:\n"));
	//xputs(_USE_LFN ? PSTR("LFN Enabled") : PSTR("LFN Disabled"));
	//xprintf(PSTR(", Code page: %u\n"), _CODE_PAGE);
	
	unsigned char driving = 1;

	char* ptr;
	for (;;) {
		//xputc('>');
		ptr = (char*)Line;
		get_line(ptr, sizeof(Line));
		switch (*ptr++) {
		
		case CMD_VERSION:
			xputc(10);
			break;
		
		
		}
	}


}
