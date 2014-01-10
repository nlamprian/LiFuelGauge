/**
 * Name: LiFuelGauge
 * Author: Nick Lamprianidis { paign10.ln@gmail.com }
 * Version: 1.0
 * Description: A library for interfacing the MAXIM MAX17043/MAX17044
 * 				Li+ fuel gauges. These ICs report the relative state of charge
 * 				of the connected Lithium Ion Polymer battery, and the library 
 * 				can help you configure them and communicate with them
 * Source: https://github.com/pAIgn10/LiFuelGauge
 * License: Copyright (c) 2014 Nick Lamprianidis 
 *          This library is licensed under the MIT license
 *          http://www.opensource.org/licenses/mit-license.php
 *
 * Filename: LiFuelGauge.cpp
 * File description: Implementations of methods for the LiFuelGauge library
 */

#include "LiFuelGauge.h"

// Initializes variables and the Wire library
LiFuelGauge::LiFuelGauge(gaugeType ic) : _ic(ic), _f(NULL) { Wire.begin(); }

// Initializes varaibles and the Wire library
// Assigns ISR f to interrupt intr (for Alert Interrupt)
LiFuelGauge::LiFuelGauge(gaugeType ic, uint8_t intr, func f) : _ic(ic), _f(f)
{
    attachInterrupt(intr, f, FALLING);
    Wire.begin();
}

// Returns a measurement of the voltage of the connected LiIon Polymer battery
// 0-5V range w/ 1.25mV resolution for the MAX17043
// 0-10V range w/ 2.5mV resolution for the MAX17044
double LiFuelGauge::getVoltage()
{
	Wire.beginTransmission(MAX1704X_ADDR);
	Wire.write(MAX1704X_VCELL_ADDR);
	Wire.endTransmission(false);
	Wire.requestFrom(MAX1704X_ADDR, (uint8_t)2);
	return ( (Wire.read() << 4) + (Wire.read() >> 4) ) * 0.00125 * _ic;
}

// Returns the relative state of charge of the connected LiIon Polymer battery
// as a percentage of the full capacity w/ resolution 1/256%
double LiFuelGauge::getSOC()
{
	Wire.beginTransmission(MAX1704X_ADDR);
	Wire.write(MAX1704X_SOC_ADDR);
	Wire.endTransmission(false);
	Wire.requestFrom(MAX1704X_ADDR, (uint8_t)2);
	return Wire.read() + (double) Wire.read() / 256;
}

// Returns the production version of the IC
uint16_t LiFuelGauge::getVersion()
{
	Wire.beginTransmission(MAX1704X_ADDR);
	Wire.write(MAX1704X_VERSION_ADDR);
	Wire.endTransmission(false);
	Wire.requestFrom(MAX1704X_ADDR, (uint8_t)2);
	return ( Wire.read() << 8 ) + Wire.read();
}

// Returns a value used to optimize IC performance
// to different operating conditions
uint8_t LiFuelGauge::getCompensation()
{
	Wire.beginTransmission(MAX1704X_ADDR);
	Wire.write(MAX1704X_RCOMP_ADDR);
	Wire.endTransmission(false);
	Wire.requestFrom(MAX1704X_ADDR, (uint8_t)1);
	return Wire.read();
}

// Returns the alert threshold as a percentage 
// below which an alert interrupt is generated
uint8_t LiFuelGauge::getAlertThreshold()
{
	return ( ~getStatus() & 0x1F ) + 1;
}

// Helper method. Returns the LSByte of the CONFIG register
uint8_t LiFuelGauge::getStatus()
{
	Wire.beginTransmission(MAX1704X_ADDR);
	Wire.write(MAX1704X_ATHRD_ADDR);
	Wire.endTransmission(false);
	Wire.requestFrom(MAX1704X_ADDR, (uint8_t)1);
	return Wire.read();
}

// Sets a value to the MSByte of the CONFIG register used to optimize
// IC performance to different operating conditions
// Returns the status of transmission
uint8_t LiFuelGauge::setCompensation(uint8_t comp)
{	
	uint8_t status = getStatus();
	
	Wire.beginTransmission(MAX1704X_ADDR);
	Wire.write(MAX1704X_CONFIG_ADDR);
	Wire.write(comp);
	Wire.write(status);
	return Wire.endTransmission();
}

// Sets the alert threshold below which an alert interrupt is generated
// The acceptable range is 1-32%. Default threshold is 4%
// Returns the status of transmission
uint8_t LiFuelGauge::setAlertThreshold(uint8_t thrd)
{
	if ( thrd > 32 ) thrd = 32;
	else if ( thrd < 1 ) thrd = 1;
	thrd = ( ~thrd + 1 ) & 0x1F;
	
	uint8_t comp, sleepBit;
	Wire.beginTransmission(MAX1704X_ADDR);
	Wire.write(MAX1704X_CONFIG_ADDR);
	Wire.endTransmission(false);
	Wire.requestFrom(MAX1704X_ADDR, (uint8_t)2);
	comp = Wire.read();
	sleepBit = Wire.read() & 0x80;
	
	Wire.beginTransmission(MAX1704X_ADDR);
	Wire.write(MAX1704X_CONFIG_ADDR);
	Wire.write(comp);
	Wire.write(sleepBit | thrd);
	return Wire.endTransmission();
}

// After an alert interrupt has been generated,
// it clears the alert bit on the CONFIG register
// Returns the status of transmission
uint8_t LiFuelGauge::clearAlertInterrupt()
{
	uint8_t comp = getCompensation();
	uint8_t status = getStatus();

	Wire.beginTransmission(MAX1704X_ADDR);
	Wire.write(MAX1704X_CONFIG_ADDR);
	Wire.write(comp);
	Wire.write(0xDF & status);
	return Wire.endTransmission();
}

// It puts the MAX1704X to sleep
// All IC operations are halted
// Returns the status of transmission
uint8_t LiFuelGauge::sleep()
{
	uint8_t comp = getCompensation();
	uint8_t thrd = getAlertThreshold();
	
 	Wire.beginTransmission(MAX1704X_ADDR);
	Wire.write(MAX1704X_CONFIG_ADDR);
	Wire.write(comp);
	Wire.write(0x80 | thrd);
	return Wire.endTransmission();
}
// It wakes the MAX1704X from sleep mode
// Returns the status of transmission
uint8_t LiFuelGauge::wake()
{
	uint8_t comp = getCompensation();
	uint8_t thrd = getAlertThreshold();
	
 	Wire.beginTransmission(MAX1704X_ADDR);
	Wire.write(MAX1704X_CONFIG_ADDR);
	Wire.write(comp);
	Wire.write(0x7F & thrd);
	return Wire.endTransmission();
}

// Returns a value corresponding to
// whether the MAX1704X is in sleep mode
boolean LiFuelGauge::sleeping()
{
	return ( getStatus() & 0x80 ) == 0x80;
}

// It forces the MAX1704X to
// restart fuel-gauge calculations
// Returns the status of transmission
uint8_t LiFuelGauge::quickStart()
{
 	Wire.beginTransmission(MAX1704X_ADDR);
	Wire.write(MAX1704X_MODE_ADDR);
	Wire.write(0x40);
	Wire.write(0x00);
	return Wire.endTransmission();
}

// It forces the MAX1704X to completely reset
// Returns the status of transmission
uint8_t LiFuelGauge::reset()
{
 	Wire.beginTransmission(MAX1704X_ADDR);
	Wire.write(MAX1704X_COMMAND_ADDR);
	Wire.write(0x54);
	Wire.write(0x00);
	return Wire.endTransmission();
}
