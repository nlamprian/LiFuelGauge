/**
 * This is an example demonstrating the use of the LiFuelGauge library
 * The example uses an LCD display to show battery status information
 * When an Alert Threshold interrupt is generated, the lowPower ISR is
 * called and the a sign appears on the LCD display
 * 
 * Note:
 *   After exiting sleep mode or resetting, give the MAX17043/4 
 *   half a second to perform the first measurements
 */

#include <LiquidCrystal.h>
#include <Wire.h>
#include <LiFuelGauge.h>


// Creates a LiquidCrystal instance with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 6, 5, 4, 3);

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
    lcd.begin(16, 2);  // Sets up the LCD's number of columns and rows
    
    gauge.reset();  // Resets MAX17043
    delay(200);  // Waits for the initial measurements to be made
    
    setAThres();
    setTemplate();
}

void loop()
{
    lcd.setCursor(5,0);
    lcd.print(gauge.getSOC());  // Gets the battery's state of charge
    lcd.setCursor(7,1);
    lcd.print(gauge.getVoltage());  // Gets the battery voltage
    
    if ( alert )
    {
        lcd.clear();
        lcd.print("Beware,");
        lcd.setCursor(0,1);
        lcd.print("Low Power!");
        
        while ( true )
        {
            lcd.display();
            delay(1000);
            lcd.noDisplay();
            delay(1000);
        }
    }
    
    delay(2000);
}

void lowPower() { alert = true; }

// Sets the Alert Threshold to 10% of full capacity
// and informs on the LCD display
void setAThres()
{
    gauge.setAlertThreshold(10);
    lcd.print("Alert Threshold");
    lcd.setCursor(0,1);
    lcd.print("is set to ");
    lcd.print(gauge.getAlertThreshold());
    lcd.print('%');
    delay(4000);
}

// Prints some labels on the LCD display
void setTemplate()
{
    lcd.clear();
    lcd.print("SOC:      %");
    lcd.setCursor(0,1);
    lcd.print("VCELL:     V");
}
