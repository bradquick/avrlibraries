/*
 *  analogio.h
 *  Project
 *
 *  Created by Brad on 7/28/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

// ADC Functions:
//
// To use the ADC functions:
//
//    adcsetup(ADCREFVCC,0);         // initializes the ADC using Vcc as the reference
//                                        // special channel ADCCHANREF1POINT1 reads the 1.1V internal reference
//                                        // special channel ADCCHANSLEEP puts the adc to sleep to conserve power
//		adcsetchannel(0);							// sets which channel to read
//    adcstartreading();                  // starts a reading
//		while (!adcreadingdone()) {}			// wait for the reading to finish
//    unsigned int value=adcgetreading(); // gets the value when the reading is completed.
//
// 
//		or
//
//
//
//    adcsetup(ADCREFVCC,1);         // initializes the adc using Vcc as the reference with auto trigger
//                                        // special channel ADCCHANREF1POINT1 reads the 1.1V internal reference
//                                        // special channel ADCCHANSLEEP puts the adc to sleep to conserve power
//		adcsetchannel(0);							// sets which channel to read
//  
//		while ()
//			{
//			unsigned int value=adcgetreading(); // gets the value when the reading is completed.
//			}

// reference voltages:
#define ADCREFVCC 0
#define ADCREF1POINT1 1
#define ADCREFEXT 2

// channels: (channels 0 throuh ? can also be used)
#define ADCCHANREF1POINT1 -1
#define ADCCHANSLEEP -2

void adcsetup(int reference,char autotrigger);
void adcsetchannel(int channel);
void adcstartreading();
unsigned int adcgetreading();
char adcreadingdone();
unsigned int adcallinone(int channel,int reference);

#ifdef LOADCPPFROMHEADERS
#include "analogio.cpp"
#endif
