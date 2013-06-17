/*
 *  Created by Brad on 2/19/12.
 */
#pragma once

#include "projectsettings.h"

#define MASTERTIMER 0 // timer 0 is always reserved for the master timer which counts milliseconds since start

void lib_timers_init();
void lib_timers_starttimer(int timernum);
void lib_timers_updatealltimers();
long lib_timers_gettimerticks(int timernum);
long lib_timers_gettimermilliseconds(int timernum);
long lib_timers_gettimerticksandreset(int timernum);
long lib_timers_gettimermicroseconds(int timernum);
unsigned long lib_timers_millisecondssincestart();
void lib_timers_delaymilli(long delaymilliseconds);
