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
 * Filename: LiFuelGauge.h
 * File description: Definitions and methods for the LiFuelGauge library
 */

#ifndef LiFuelGauge_h
#define LiFuelGauge_h

#include <Arduino.h>
#include <Wire.h>

// MAX1704X register addresses
const uint8_t MAX1704X_ADDR = 0x36;
const uint8_t MAX1704X_VCELL_ADDR = 0x02;
const uint8_t MAX1704X_SOC_ADDR = 0x04;
const uint8_t MAX1704X_MODE_ADDR = 0x06;
const uint8_t MAX1704X_VERSION_ADDR = 0x08;
const uint8_t MAX1704X_CONFIG_ADDR = 0x0C;
const uint8_t MAX1704X_RCOMP_ADDR = 0x0C;
const uint8_t MAX1704X_ATHRD_ADDR = 0x0D;
const uint8_t MAX1704X_COMMAND_ADDR = 0xFE;

// Signature of the ISR for the ALERT Interrupt
typedef void (*func)();

// Names of the two supported ICs
// Used for reporting the correct voltage measurement (see getVoltage method)
enum gaugeType
{
	MAX17043 = 1,
	MAX17044 = 2
};

// Class for interfacing the MAX17043/MAX17044 Li+ fuel gauges
class LiFuelGauge
{
public:
	LiFuelGauge(gaugeType ic);
    LiFuelGauge(gaugeType ic, uint8_t intr, func f);
    double getVoltage();
    double getSOC();
    uint16_t getVersion();
    uint8_t getCompensation();
    uint8_t getAlertThreshold();
    uint8_t setCompensation(uint8_t comp);
    uint8_t setAlertThreshold(uint8_t thrd);
    uint8_t clearAlertInterrupt();
    uint8_t sleep();
    uint8_t wake();
    boolean sleeping();
    uint8_t quickStart();
    uint8_t reset();
    
private:
    gaugeType _ic;
    func _f;
    uint8_t getStatus();
};

#endif  // LiFuelGauge
