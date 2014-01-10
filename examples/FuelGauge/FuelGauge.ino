/**
 * This is an example demonstrating the use of the LiFuelGauge library
 * The example prints battery status information on the serial monitor
 * When an Alert Threshold interrupt is generated, the lowPower ISR is
 * called and afterwards (hypothetically) any running operations are 
 * getting finalized before the system enters sleep mode
 * 
 * Note:
 *   After exiting sleep mode or resetting, give the MAX17043/4 
 *   half a second to perform the first measurements
 */

#include <Wire.h>
#include <LiFuelGauge.h>

// LiFuelGauge constructor parameters
// 1. IC type, MAX17043 or MAX17044
// 2. Number of interrupt to which the alert pin is associated (Optional) 
// 3. ISR to call when an alert interrupt is generated (Optional)
//
// Creates a LiFuelGauge instance for the MAX17043 IC
// and attaches lowPower to INT0 (PIN2 on most boards, PIN3 on Leonardo)
LiFuelGauge gauge(MAX17043, 0, lowPower);

// A flag to indicate a generated alert interrupt
volatile boolean alert = false;


void setup()
{
    Serial.begin(9600); // Initializes serial port
    // Waits for serial port to connect. Needed for Leonardo only
    while ( !Serial ) ;
    
    gauge.reset();  // Resets MAX17043
    delay(200);  // Waits for the initial measurements to be made
    
    // Sets the Alert Threshold to 10% of full capacity
    gauge.setAlertThreshold(10);
    Serial.println(String("Alert Threshold is set to ") + 
                   gauge.getAlertThreshold() + '%');
}

void loop()
{
    Serial.print("SOC: ");
    Serial.print(gauge.getSOC());  // Gets the battery's state of charge
    Serial.print("%, VCELL: ");
    Serial.print(gauge.getVoltage());  // Gets the battery voltage
    Serial.println('V');
    
    if ( alert )
    {
        Serial.println("Beware, Low Power!");
        Serial.println("Finalizing operations...");
        gauge.clearAlertInterrupt();  // Resets the ALRT pin
        alert = false;
        Serial.println("Storing data...");
        Serial.println("Sending notification...");
        Serial.println("System operations are halted...");
        gauge.sleep();  // Forces the MAX17043 into sleep mode
        while ( true ) ;
    }
    
    delay(2000);
}

void lowPower() { alert = true; }
