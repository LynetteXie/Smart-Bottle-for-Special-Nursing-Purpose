/*
 * This is a demo of SmartBottle project.
 * It collects data of user daily water intake, 
 * providing a new revolutionary way of nursing.
 * Author: Lynette Xie
 * Date: April 6, 2018
 */

/*
  Arduino Water flow meter
  YF- S201 Hall Effect Water Flow Sensor
  Water Flow Sensor output processed to read in litres/hour
  http://www.theorycircuit.com/water-flow-sensor-yf-s201-arduino-interface/
*/

/* DS1302_Serial_Hard 
   Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
   web: http://www.RinkyDinkElectronics.com/
  
   A quick demo of how to use my DS1302-library to 
   retrieve time- and date-date for you to manipulate.
  
   I assume you know how to connect the DS1302.
   DS1302:  CE pin    -> Arduino Digital 11
            I/O pin   -> Arduino Digital 12
            SCLK pin  -> Arduino Digital 13
*/

#include <DS1302.h>

// Init the DS1302
DS1302 rtc(11, 12, 13);

// Init a Time-data structure
Time t;

volatile int flow_frequency; // Measures flow sensor pulses
unsigned int l_hour; // Calculated litres/hour
unsigned char flowsensor = 2; // Sensor Input
unsigned long currentTime;
unsigned long cloopTime;

float volume=0; // Volume of flow
void flow () // Interrupt function

#undef int TrafficLightRedPin = 7; // The red traffic light is conntected pin 7 of the Arduino.
#undef int GroundPin = 4;         // The ground is conntected pin 4 of the Arduino.

{
   flow_frequency++;
}
void setup()
{   
   pinMode(flowsensor, INPUT);
   digitalWrite(flowsensor, HIGH); // Optional Internal Pull-Up
   //initialize serial communications at 9600 bps;
   Serial.begin(9600);
   attachInterrupt(0, flow, RISING); // Setup Interrupt
   sei(); // Enable interrupts
   currentTime = millis();
   cloopTime = currentTime;

   // Set the clock to run-mode, and disable the write protection
   rtc.halt(false);
   rtc.writeProtect(false);

   // The following lines can be commented out to use the values already stored in the DS1302
   rtc.setDOW(FRIDAY);        // Set Day-of-Week to FRIDAY
   rtc.setTime(03, 00, 00);     // Set the time to 00:00:00 (24hr format)
   rtc.setDate(27, 04, 2018);   // Set the date to April 27th, 2018

   int TrafficLightRedPin = 7;
   int GroundPin = 4; 
   
   pinMode(TrafficLightRedPin, OUTPUT);
   pinMode(GroundPin, OUTPUT);
}

void loop ()
{
   currentTime = millis();
   // Every second, calculate and print litres/hour
   if(currentTime >= (cloopTime + 1000))
   {
      cloopTime = currentTime; // Updates cloopTime
      // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
      l_hour = (flow_frequency * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
      // Calculate the volume of the flow
      // Water-flow Formula: 1L = 5880 square waves
      volume= (float)flow_frequency / 5880.0 * 1000.0; 
     Serial.print("The current flow sensor pulses is: ");
     Serial.println(flow_frequency);
     Serial.println("     ");
     Serial.print("Your current water intake is: ");
     Serial.print(volume);
     Serial.println("mL");
   }

   // Get data from the real time clock DS1302
   t = rtc.getTime();

   // Send date over serial connection
   Serial.print("Today is the ");
   Serial.print(t.date, DEC);
   Serial.print(". day of ");
   Serial.print(rtc.getMonthStr());
   Serial.print(" in the year ");
   Serial.print(t.year, DEC);
   Serial.println(".");

   // Send Day-of-Week and time
   Serial.print("It is the ");
   Serial.print(t.dow, DEC);
   Serial.print(". day of the week (counting monday as the 1th), and it has passed ");
   Serial.print(t.hour, DEC);
   Serial.print(" hour(s), ");
   Serial.print(t.min, DEC);
   Serial.print(" minute(s) and ");
   Serial.print(t.sec, DEC);
   Serial.println(" second(s) since midnight.");

   // Print water intake so far
   Serial.print("You have already drunk ");
   Serial.print(volume);
   Serial.print("ml water ");
   Serial.print("within ");
   Serial.print(t.hour, DEC);
   Serial.print(" hour(s), ");
   Serial.print(t.min, DEC);
   Serial.print(" minute(s) and ");
   Serial.print(60-t.sec, DEC);
   Serial.println(" second(s) since the program starts.");

   // If the user drink less than 600ml water within 2 hours
   // the TrafficLightRed starts to blink
   // to notify the user / care giver to drink suffient water.
   if(volume < 600 && t.hour > 2)
   {
      int TrafficLightRedPin = 7;
      int GroundPin = 4; 
      
      digitalWrite(TrafficLightRedPin, HIGH);
      delay(500);
      digitalWrite(TrafficLightRedPin, LOW);
      digitalWrite(GroundPin, LOW);
   }

   // Send a divider for readability
   Serial.println("  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -");

   // Wait 3 second before repeating :)
  delay (3000);
}
