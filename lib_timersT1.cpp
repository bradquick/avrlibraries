/*
 *  Created by Brad on 2/2/12.
 */
 
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
unsigned int timerstarttime[NUMTIMERS]; // the value of the avr timer when we started each timer
volatile int timeroverflowcount[NUMTIMERS]; // the number of overflows each timer has seen
volatile int timermasterdoubleoverflowcount=0; // the number of overflows of the master's overflow

void lib_timers_init()
	{ // needs to be called once in the program before timers can be used
	TCCR1B |= (1 << CS11); // Set up timer at Fcpu/8
	// at 16mhz, this gives 2,000,000 counts/second or 0.0000005 seconds per count
	// our 16 bit counter will overflow in 0.032768 seconds
	// We cascade this into another int which will allow us to use timers a little over 1000 seconds long
	// For the master timer, we overflow the overflow at 20,000 counts.  This allows us to do over 1000 hours in milliseconds

	//Enable Overflow Interrupt Enable
	TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
	
	// make sure interrupts are enabled
	sei();
	
	lib_timers_starttimer(MASTERTIMER);
	}

ISR(TIMER1_OVF_vect)
	{
	// This is the interrupt service routine for TIMER1 OVERFLOW Interrupt.
	// Increment the overflow count for each of our timers
	for (int x=0;x<NUMTIMERS;++x)
		{
		++timeroverflowcount[x];
		}
	if (timeroverflowcount[MASTERTIMER]>=20000)
		{
		timeroverflowcount[MASTERTIMER]=0;
		++timermasterdoubleoverflowcount;
		}
		
	}
	
void lib_timers_starttimer(int timernum)
	{ // start a timer
	// reset the timer's high word (overflow count) and remember where the low word (avr counter) is when we started.
	cli();
	timerstarttime[timernum]=TCNT1;
	timeroverflowcount[timernum]=0;
	if (TIFR1 & (1<<TOV1))
		{ // there is an overflow interrupt waiting. 
		// There is a possibility that the timer overflowed after we read the timer value
		// get a fresh copy of the timer value to make sure we got the rolled over value
		timerstarttime[timernum]=TCNT1;
		// subtract one from timerhighword since we know it will increment on the overflow
		timeroverflowcount[timernum]=-1;
		}
	
	sei();
	}
	
long lib_timers_gettimerticks(int timernum)
	{ // returns the 32bit timer value
	cli();
	unsigned int countervalue=TCNT1;
	int overflowcount=timeroverflowcount[timernum];
	if (TIFR1 & (1<<TOV1))
		{ // there is an overflow interrupt waiting.  
		// There is a possibility that the timer overflowed after we read the timer value
		// get a fresh copy of the timer value to make sure we got the rolled over value
		countervalue=TCNT1;
		// add the overflow that we missed
		++overflowcount;
		}

	sei();

	return(((((long)overflowcount)<<16) | countervalue)-timerstarttime[timernum]);
	}
	
long lib_timers_gettimerticksandreset(int timernum)
	{ // returns the 32 bit timer value and resets the timer to zero, without missing any counts.
	cli();
	unsigned int countervalue=TCNT1;
	int overflowcount=timeroverflowcount[timernum];
	timeroverflowcount[timernum]=0;
	if (TIFR1 & (1<<TOV1))
		{ // there is an overflow interrupt waiting.  
		// There is a possibility that the timer overflowed after we read the timer value
		// get a fresh copy of the timer value to make sure we got the rolled over value
		countervalue=TCNT1;
		// add the overflow that we missed
		++overflowcount;
		// subtract one from timerhighword since we know it will increment on the overflow
		timeroverflowcount[timernum]=-1;
		}

	sei();

	long returnvalue=((((long)overflowcount)<<16) | countervalue)-timerstarttime[timernum];
	timerstarttime[timernum]=countervalue;
	return(returnvalue);
	}

unsigned long lib_timers_millisecondssincestart()
	{ // return the number of milliseconds since boot up
	cli();
	unsigned int countervalue=TCNT1;
	int overflowcount=timeroverflowcount[MASTERTIMER];
	if (TIFR1 & (1<<TOV1))
		{ // there is an overflow interrupt waiting.  
		// There is a possibility that the timer overflowed after we read the timer value
		// get a fresh copy of the timer value to make sure we got the rolled over value
		countervalue=TCNT1;
		// add the overflow that we missed
		++overflowcount;
		}

	unsigned int doubleoverflowcount=timermasterdoubleoverflowcount;
	sei();
	
	#if (F_CPU==16000000)
		return(doubleoverflowcount*655360L+(((((long)overflowcount)<<16) | countervalue)-timerstarttime[MASTERTIMER])>>11);
	#elif (F_CPU==8000000)
		return(doubleoverflowcount*1310720L+(((((long)overflowcount)<<16) | countervalue)-timerstarttime[MASTERTIMER])>>10);
	#elif (F_CPU==1000000)
		return(doubleoverflowcount*10485760L+(((((long)overflowcount)<<16) | countervalue)-timerstarttime[MASTERTIMER])>>7);
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
		return(value>>11);
	#elif (F_CPU==8000000)
		return(value>>10);
	#elif (F_CPU==1000000)
		return(value>>7);
	#else
		need to add code for your clock rate
	#endif
	}

long lib_timers_gettimermicroseconds(int timernum)
	{ // return the timer's value to the nearest microsecond
	long value=lib_timers_gettimerticks(timernum);
	
	#if (F_CPU==16000000)
		return(value>>1);
	#elif (F_CPU==8000000)
		return(value);
	#elif (F_CPU==1000000)
		return(value<<2);
	#else
		need to add code for your clock rate
	#endif
	}
