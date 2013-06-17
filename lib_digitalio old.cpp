/*
 *  Created by Brad on 2/2/12.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "lib_digitalio.h"
#include "projectsettings.h"

// This code controls the simple digital IO

// Usage:

// #define LEDOUTPUT (DIGITALPORTB | 7)     // defines the reference for pin 7 on PORTB
// lib_digitalio_initpin(LEDOUTPUT,DIGITALOUTPUT | PULLUPRESISTOR); // set the pin as an output (also turns on the pull up resistor)
// lib_digitalio_setoutput(LEDOUTPUT, DIGITALLOW); // turn the output on by pulling it low

// #define PUSHBUTTON (DIGITALPORTB | 6)     // defines the reference for pin 6 on PORTB
// lib_digitalio_initpin(PUSHBUTTON,DIGITALINPUT);  // set the pin as an input (also turns on the pull up resistor)
// if (lib_digitalio_getinput(PUSHBUTTON)) {}       // Check the input
// lib_digitalio_setinterrupt(PUSHBUTTON,11,mypushbuttoncallback); // tell the interrupt
// void mypushbuttoncallback(char interruptnumber,char newstate) // call back will get called any time the pin changes
//		{
//		if (newstate==DIGITALON) {}
//		}


// ports to be used as digital IO need to be defined in projectsettings.h
#define SETUPDIGITALIOMACRO(myport,avrport,avrddr,pinnumber) \
	if (port==myport) \
		{ \
		if (output & DIGITALOUTPUT)\
			{ \
			if (!(output & NOPULLUPRESISTOR)) avrport |= (1 << pinnumber); /* pull up resistor on */\ 
			avrddr |= (1 << pinnumber); /* this is an output */ \
			}\
		else\
			{\
			avrddr &= ~(1 << pinnumber); /* set it to an input*/\
			if (!(output & NOPULLUPRESISTOR)) avrport |= (1 << pinnumber); /* pull up resistor on*/\
			}\
		}\

void lib_digitalio_initpin(unsigned char pinnumber,unsigned char output)
   { // set pin pinnumber to be an output if output==1, othewise set it to be an input
	unsigned char port=pinnumber & 0xf0;
	pinnumber &= 0x0f;

#ifdef USEDIGITALPORTB
	SETUPDIGITALIOMACRO(DIGITALPORTB,PORTB,DDRB,pinnumber);
#endif
#ifdef USEDIGITALPORTC
	SETUPDIGITALIOMACRO(DIGITALPORTC,PORTC,DDRC,pinnumber);
#endif
#ifdef USEDIGITALPORTD
	SETUPDIGITALIOMACRO(DIGITALPORTD,PORTD,DDRD,pinnumber);
#endif
#ifdef USEDIGITALPORTE
	SETUPDIGITALIOMACRO(DIGITALPORTE,PORTE,DDRE,pinnumber);
#endif
#ifdef USEDIGITALPORTF
	SETUPDIGITALIOMACRO(DIGITALPORTF,PORTF,DDRF,pinnumber);
#endif
#ifdef USEDIGITALPORTG
	SETUPDIGITALIOMACRO(DIGITALPORTG,PORTG,DDRG,pinnumber);
#endif
#ifdef USEDIGITALPORTH
	SETUPDIGITALIOMACRO(DIGITALPORTH,PORTH,DDRH,pinnumber);
#endif
#ifdef USEDIGITALPORTI
	SETUPDIGITALIOMACRO(DIGITALPORTI,PORTI,DDRI,pinnumber);
#endif
#ifdef USEDIGITALPORTI
	SETUPDIGITALIOMACRO(DIGITALPORTJ,PORTJ,DDRJ,pinnumber);
#endif
#ifdef USEDIGITALPORTK
	SETUPDIGITALIOMACRO(DIGITALPORTK,PORTK,DDRK,pinnumber);
#endif
#ifdef USEDIGITALPORTL
	SETUPDIGITALIOMACRO(DIGITALPORTL,PORTL,DDRL,pinnumber);
#endif

   }

#define GETDIGITALINPUTMACRO(myport,avrpin,pinnumber) \
	if (port==myport) \
		{\
		if (avrpin & (1 << pinnumber)) return(0);\
		else return(1);\
		}\
	

unsigned char lib_digitalio_getinput(unsigned char pinnumber)
   {
	unsigned char port=pinnumber & 0xf0;
	pinnumber &= 0x0f;

#ifdef USEDIGITALPORTB
	GETDIGITALINPUTMACRO(DIGITALPORTB,PINB,pinnumber);
#endif
#ifdef USEDIGITALPORTC
	GETDIGITALINPUTMACRO(DIGITALPORTC,PINC,pinnumber);
#endif
#ifdef USEDIGITALPORTD
	GETDIGITALINPUTMACRO(DIGITALPORTD,PIND,pinnumber);
#endif
#ifdef USEDIGITALPORTE
	GETDIGITALINPUTMACRO(DIGITALPORTE,PINE,pinnumber);
#endif
#ifdef USEDIGITALPORTF
	GETDIGITALINPUTMACRO(DIGITALPORTF,PINF,pinnumber);
#endif
#ifdef USEDIGITALPORTG
	GETDIGITALINPUTMACRO(DIGITALPORTG,PING,pinnumber);
#endif
#ifdef USEDIGITALPORTH
	GETDIGITALINPUTMACRO(DIGITALPORTH,PINH,pinnumber);
#endif
#ifdef USEDIGITALPORTI
	GETDIGITALINPUTMACRO(DIGITALPORTI,PINI,pinnumber);
#endif
#ifdef USEDIGITALPORTJ
	GETDIGITALINPUTMACRO(DIGITALPORTJ,PINJ,pinnumber);
#endif
#ifdef USEDIGITALPORTK
	GETDIGITALINPUTMACRO(DIGITALPORTK,PINK,pinnumber);
#endif
#ifdef USEDIGITALPORTL
	GETDIGITALINPUTMACRO(DIGITALPORTL,PINL,pinnumber);
#endif
   }

#define SETDIGITALOUTPUTMACRO(value,myport,avrport,pinnumber) \
	if (port==myport) \
		{\
		if (!value) avrport |= (1 << pinnumber);\
		else avrport &= ~(1 << pinnumber);\
		}\
		
void lib_digitalio_setoutput(unsigned char pinnumber,unsigned char value)
   {
	unsigned char port=pinnumber & 0xf0;
	pinnumber &= 0x0f;

#ifdef USEDIGITALPORTB
	SETDIGITALOUTPUTMACRO(value,DIGITALPORTB,PORTB,pinnumber);
#endif
#ifdef USEDIGITALPORTC
	SETDIGITALOUTPUTMACRO(value,DIGITALPORTC,PORTC,pinnumber);
#endif
#ifdef USEDIGITALPORTD
	SETDIGITALOUTPUTMACRO(value,DIGITALPORTD,PORTD,pinnumber);
#endif
#ifdef USEDIGITALPORTE
	SETDIGITALOUTPUTMACRO(value,DIGITALPORTE,PORTE,pinnumber);
#endif
#ifdef USEDIGITALPORTF
	SETDIGITALOUTPUTMACRO(value,DIGITALPORTF,PORTF,pinnumber);
#endif
#ifdef USEDIGITALPORTG
	SETDIGITALOUTPUTMACRO(value,DIGITALPORTG,PORTG,pinnumber);
#endif
#ifdef USEDIGITALPORTH
	SETDIGITALOUTPUTMACRO(value,DIGITALPORTH,PORTH,pinnumber);
#endif
#ifdef USEDIGITALPORTI
	SETDIGITALOUTPUTMACRO(value,DIGITALPORTI,PORTI,pinnumber);
#endif
#ifdef USEDIGITALPORTJ
	SETDIGITALOUTPUTMACRO(value,DIGITALPORTJ,PORTJ,pinnumber);
#endif
#ifdef USEDIGITALPORTK
	SETDIGITALOUTPUTMACRO(value,DIGITALPORTK,PORTK,pinnumber);
#endif
#ifdef USEDIGITALPORTL
	SETDIGITALOUTPUTMACRO(value,DIGITALPORTL,PORTL,pinnumber);
#endif
	}


#ifdef MINPCINT
#define NUMPCINTS MAXPCINT-MINPCINT+1

//typedef void (* digitalcallbackfunctptr)(char interruptnumber,char newpinstate);
unsigned char interruptportandpin[NUMPCINTS];
long interruptlastpinstate=0x0;
digitalcallbackfunctptr digitalcallback[NUMPCINTS]={0};

void lib_digitalio_setinterrupt(unsigned char pinnumber,unsigned char pcintnumber,digitalcallbackfunctptr callback)
	{ // remember which input this interrupt belongs to and it's call back function
	interruptportandpin[pcintnumber-MINPCINT]=pinnumber;
	digitalcallback[pcintnumber-MINPCINT]=callback;

	// remember the current state of this input
	if (lib_digitalio_getinput(pinnumber)) interruptlastpinstate |= (1<<(pcintnumber-MINPCINT));
	else interruptlastpinstate &= ~(1<<(pcintnumber-MINPCINT));
	
	// enable the appropriate interrupts
	if (pcintnumber<=7)
		{
		PCICR |= (1<<PCIE0);
		PCMSK0 |= (1<<pcintnumber);
		}
	else if (pcintnumber>=16)
		{
		PCICR |= (1<<PCIE2);
		PCMSK2 |= (1<<(pcintnumber-16));
		}
	else 
		{
		PCICR |= (1<<PCIE1);
		PCMSK1 |= (1<<(pcintnumber-8));
		}
	sei();  // turn on interrupts in general
	}

void checkdigitalinterrupt(unsigned char min,unsigned char max)
	{ // check a range of PCINT's to see which input state has changed
	if (min<MINPCINT) min=MINPCINT;
	if (max>MAXPCINT) max=MAXPCINT;
	
	// one of the interrupts 0-7 triggered.  Figure out which it was
	for (unsigned char x=min;x<=max;++x)
		{
		unsigned char shift=x-MINPCINT;
//		long pinstatemask=(1<<(x-MINPCINT));
		unsigned char inputstate=lib_digitalio_getinput(interruptportandpin[shift]);
		if ((inputstate!=((interruptlastpinstate >> shift) & 0x01)) && digitalcallback[shift]!=0)
			{
			if (inputstate) interruptlastpinstate |= (1<<(shift));
			else interruptlastpinstate &= ~(1<<(shift));
			(digitalcallback[shift])(x,inputstate);
//			return;
			}
		}
	}
	
#if MINPCINT<=7
ISR(PCINT0_vect)
	{ // check for PCINT0 through PCINT7
	checkdigitalinterrupt(0,7);
	}
#endif

#if (MAXPCINT>=8 && MINPCINT<=15)
ISR (PCINT1_vect)
	{
	checkdigitalinterrupt(8,15);
	}
#endif

#if (MAXPCINT>=16)
ISR (PCINT2_vect)
	{
	checkdigitalinterrupt(16,23);
	}
#endif

#endif