/*
 *  Created by Brad on 2/2/12.
 */
//#include "lib_timersT1.cpp"
//#ifdef xxx


#include <avr/io.h>
#include <avr/interrupt.h>
#include "lib_timers.h"

// This code creates as many timers as needed from a single AVR 16 bit timer.  The AVR timer is cascaded in to multiple
// software timers to create many 32 bit timers.  Timers count in clock cycles/1024, which gives 2,000,000 cycles per
// second on a 16Mhz machine.  Timers are good for up to around 1000 seconds.  Timer 0 is used for delaymilli, so if you
// are going to use delaymilli, don't use timer 0.

// Usage:

// half second delay done multiple ways

// inittimers();
// starttimer(1);
//	while (gettimerticks(1)<1000000L) {} // assuming 16mhz clock

// inittimers();
// starttimer(1);
//	while (gettimermilliseconds(1)<500) {}

// inittimers();
//	delaymilli(500) {}


// NUMTIMERS needs to be declared in projectsettings.h.  Remember to include one for the master timer.
unsigned char timerstarttime[NUMTIMERS]; // the value of the avr timer when we started each timer
volatile long timeroverflowcount[NUMTIMERS]; // the number of overflows each timer has seen

void lib_timers_init()
	{ // needs to be called once in the program before timers can be used
	TCCR0B |= (1 << CS01) | (1 << CS00); // Set up timer at Fcpu/64
	// at 16mhz, this gives 250,000 counts/second or 0.000004 seconds per count
	// our 8 bit counter will overflow in 0.001024 seconds
	// We cascade this into another a long which will allow us to use timers a little over 610 hours long
	// if we need timers longer than this, we will have to do them another way

	//Enable Overflow Interrupt Enable
	TIMSK0 |= (1 << TOIE0);   // enable timer overflow interrupt
	
	// make sure interrupts are enabled
	sei();
	
	lib_timers_starttimer(MASTERTIMER);
	}

ISR(TIMER0_OVF_vect)
	{
	// This is the interrupt service routine for TIMER1 OVERFLOW Interrupt.
	// Increment the overflow count for each of our timers
	for (int x=0;x<NUMTIMERS;++x)
		{
		++timeroverflowcount[x];
		}		
	}
	
void lib_timers_starttimer(int timernum)
	{ // start a timer
	// reset the timer's high word (overflow count) and remember where the low word (avr counter) is when we started.
	cli();
	timerstarttime[timernum]=TCNT0;
	timeroverflowcount[timernum]=0;
	if (TIFR0 & (1<<TOV0))
		{ // there is an overflow interrupt waiting. 
		// There is a possibility that the timer overflowed after we read the timer value
		// get a fresh copy of the timer value to make sure we got the rolled over value
		timerstarttime[timernum]=TCNT0;
		// subtract one from overflow since we know it will increment on the overflow
		timeroverflowcount[timernum]=-1;
		}
	
	sei();
	}
	
long lib_timers_gettimerticks(int timernum)
	{ // returns the 32bit timer value
	cli();
	unsigned char countervalue=TCNT0;
	int overflowcount=timeroverflowcount[timernum];
	if (TIFR0 & (1<<TOV0))
		{ // there is an overflow interrupt waiting.  
		// There is a possibility that the timer overflowed after we read the timer value
		// get a fresh copy of the timer value to make sure we got the rolled over value
		countervalue=TCNT0;
		// add the overflow that we missed
		++overflowcount;
		}

	sei();

	return((((overflowcount)<<8) | countervalue)-timerstarttime[timernum]);
	}
	
long lib_timers_gettimerticksandreset(int timernum)
	{ // returns the 32 bit timer value and resets the timer to zero, without missing any counts.
	cli();
	unsigned char countervalue=TCNT0;
	int overflowcount=timeroverflowcount[timernum];
	timeroverflowcount[timernum]=0;
	if (TIFR0 & (1<<TOV0))
		{ // there is an overflow interrupt waiting.  
		// There is a possibility that the timer overflowed after we read the timer value
		// get a fresh copy of the timer value to make sure we got the rolled over value
		countervalue=TCNT0;
		// add the overflow that we missed
		++overflowcount;
		// subtract one from overflow since we know it will increment on the overflow
		timeroverflowcount[timernum]=-1;
		}

	sei();

	long returnvalue=(((overflowcount)<<8) | countervalue)-timerstarttime[timernum];
	timerstarttime[timernum]=countervalue;
	return(returnvalue);
	}

unsigned long lib_timers_millisecondssincestart()
	{ // return the number of milliseconds since boot up
	cli();
	unsigned char countervalue=TCNT0;
	int overflowcount=timeroverflowcount[MASTERTIMER];
	if (TIFR0 & (1<<TOV0))
		{ // there is an overflow interrupt waiting.  
		// There is a possibility that the timer overflowed after we read the timer value
		// get a fresh copy of the timer value to make sure we got the rolled over value
		countervalue=TCNT0;
		// add the overflow that we missed
		++overflowcount;
		}

	sei();
	
	#if (F_CPU==16000000)
		return((((overflowcount)<<8) | countervalue)-timerstarttime[MASTERTIMER])>>8;
	#elif (F_CPU==8000000)
		return((((overflowcount)<<8) | countervalue)-timerstarttime[MASTERTIMER])>>7;
	#elif (F_CPU==1000000)
		return((((overflowcount)<<8) | countervalue)-timerstarttime[MASTERTIMER])>>4;
	#else
		need to add code for your clock rate
	#endif
	}

void lib_timers_delaymilli(long delaymilliseconds)
	{
	unsigned long donetime=lib_timers_millisecondssincestart()+delaymilliseconds;
	while (lib_timers_millisecondssincestart()<donetime) {}
	}
	
long lib_timers_gettimermilliseconds(int timernum)
	{ // return the timer's value to the nearest millisecond
	// in order to do fast math, these values will be off by a factor of 1.024 for 16mhz
	long value=lib_timers_gettimerticks(timernum);
	
	#if (F_CPU==16000000)
		return(value>>8);
	#elif (F_CPU==8000000)
		return(value>>7);
	#elif (F_CPU==1000000)
		return(value>>4);
	#else
		need to add code for your clock rate
	#endif
	}

long lib_timers_gettimermicroseconds(int timernum)
	{ // return the timer's value to the nearest microsecond
	long value=lib_timers_gettimerticks(timernum);
	
	#if (F_CPU==16000000)
		return(value<<2);
	#elif (F_CPU==8000000)
		return(value<<3);
	#elif (F_CPU==1000000)
		return(value<<6);
	#else
		need to add code for your clock rate
	#endif
	}

//#endif