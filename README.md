LiFuelGauge
===========

This is an Arduino library for interfacing the MAXIM `MAX17043/MAX17044` Li+ fuel gauges. These ICs communicate with an I2C bus at speeds up to 400kHz, and the `LiFuelGauge` library uses the [`Wire`](http://arduino.cc/en/reference/wire) library.

![project](http://i859.photobucket.com/albums/ab154/lampnick67/fuelgauge_project_zpsfe9f4efe.png)

Interface
---------

Next follows a list of the public interface of the LiFuelGauge class.

```cpp
LiFuelGauge(gaugeType ic);  // Creates a LiFuelGauge instance. Takes as argument the type of the IC. Either MAX17043 or MAX17044
LiFuelGauge(gaugeType ic, uint8_t intr, func f);  // Same as above. Also attaches the user defined function f to interrupt number intr. These arguments associate with the ALRT interrupt. The function f must take no arguments and return no values
double getVoltage();  // Returns a measurement of the battery's voltage. MAX17043: 0-5V w/ 1.25mV res. MAX17044: 0-10V w/ 2.5mV res.
double getSOC();  // Returns the battery's state of charge as a percentage of the full capacity. The resolution is 1/256%
uint16_t getVersion();  // Returns the production version of the IC
uint8_t getCompensation();  // Returns a value used to optimize IC performance to different operating conditions
uint8_t getAlertThreshold();  // Returns the alert threshold as a percentage below which an alert interrupt is generated
uint8_t setCompensation(uint8_t comp);  // Sets a value useful for optimizing IC performance to different operating conditions
uint8_t setAlertThreshold(uint8_t thrd);  // Sets the alert threshold below which an alert interrupt is generated. The acceptable range is 1-32%. Default threshold is 4%
uint8_t clearAlertInterrupt();  // After an alert interrupt has been generated, it resets the interrupt. No interrupt will be regenerated until the battery's SOC rises above the alert threshold and then falls below that again.
uint8_t sleep();  // Forces the IC to sleep mode. All operations are halted
uint8_t wake();  // Exits the IC from sleep mode
boolean sleeping();  // Returns whether or not the IC is in sleep mode
uint8_t quickStart();  // Forces the IC to restart fuel-gauge calculations
uint8_t reset();  // Forces the IC to completely reset
```



Examples
--------

There are two basic accompanying examples that demonstrate the use of the library.
* `FuelGauge` prints battery data on the serial monitor.
* `FuelGaugeLCD` shows battery data on an LCD display.
