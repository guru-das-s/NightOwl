/*********************************************************************
This is an example for our nRF8001 Bluetooth Low Energy Breakout

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/1697

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Kevin Townsend/KTOWN  for Adafruit Industries.
MIT license, check LICENSE for more information
All text above, and the splash screen below must be included in any redistribution
*********************************************************************/

// This version uses the internal data queing so you can treat it like Serial (kinda)!

#include <SPI.h>
#include "Adafruit_BLE_UART.h"
#include <SFE_MMA8452Q.h> // Includes the SFE_MMA8452Q library
#include <Wire.h>

// Connect CLK/MISO/MOSI to hardware SPI
// e.g. On UNO & compatible: CLK = 13, MISO = 12, MOSI = 11
#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RDY 2     // This should be an interrupt pin, on Uno thats #2 or #3
#define ADAFRUITBLE_RST 9

MMA8452Q accel;

Adafruit_BLE_UART BTLEserial = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);
/**************************************************************************/
/*!
    Configure the Arduino and start advertising with the radio
*/
/**************************************************************************/
void setup(void)
{ 
  Serial.begin(9600);
  while(!Serial); // Leonardo/Micro should wait for serial init
//Serial.println(F("Adafruit Bluefruit Low Energy nRF8001 Print echo demo"));
//Serial.println("MMA8452Q Test Code!");
  accel.init();
   
  // BTLEserial.setDeviceName("NEWNAME"); /* 7 characters max! */

  BTLEserial.begin();
}

/**************************************************************************/
/*!
    Constantly checks for new events on the nRF8001
*/
/**************************************************************************/
aci_evt_opcode_t laststatus = ACI_EVT_DISCONNECTED;

void loop()
{
  // Local variable declarations
  byte pl; // for storing orientation information
  float acc_x, acc_y, acc_z; // these hold the accelerometer values
  int i=0,j=0,k=0;
  
  // Tell the nRF8001 to do whatever it should be working on.
  BTLEserial.pollACI();

  // Ask what is our current status
  aci_evt_opcode_t status = BTLEserial.getState();
  // If the status changed....
  if (status != laststatus) {
    // print it out!
    if (status == ACI_EVT_DEVICE_STARTED) {
        Serial.println(F("* Advertising started"));
    }
    if (status == ACI_EVT_CONNECTED) {
        Serial.println(F("* Connected!"));
    }
    if (status == ACI_EVT_DISCONNECTED) {
        Serial.println(F("* Disconnected or advertising timed out"));
    }
    // OK set the last status change to this one
    laststatus = status;
  }

  // read the accelerometer values: x, y, z components and orientation
  if (accel.available())
  {
    // First, use accel.read() to read the new variables:
    accel.read();
    pl = accel.readPL();
    acc_x = accel.cx;
    acc_y = accel.cy;
    acc_z = accel.cz;
  }

  // Convert floating point accel values to string format, courtesy arduino-hacks.com/float-to-string-float-to-character-array-arduino/)
  char charval_x[7];
  char charval_y[7];
  char charval_z[7];
  String voltageString_x="";
  String voltageString_y="";
  String voltageString_z="";
  String voltageString_total="";
  
  dtostrf(acc_x,7,3,charval_x);
  dtostrf(acc_y,7,3,charval_y);
  dtostrf(acc_z,7,3,charval_z);

  for(i=0;i<sizeof(charval_x);i++) //Making strings
  {
    voltageString_x+=charval_x[i];
  }
  voltageString_x.trim();  
  
  for(j=0;j<sizeof(charval_y);j++) 
  {
    voltageString_y+=charval_y[j];
  }
  voltageString_y.trim();

  for(k=0;k<sizeof(charval_z);k++) 
  {
    voltageString_z+=charval_z[k];
  }
  voltageString_z.trim();

//  Serial.println(sizeof(voltageString_x)); // Testing............
//  Serial.println(sizeof(voltageString_y));
//  Serial.println(sizeof(voltageString_z));

  voltageString_total = voltageString_x+" "+voltageString_y+" "+voltageString_z;
//  Serial.println(voltageString_total);
  //Broadcast the analog voltage!
  if (status == ACI_EVT_CONNECTED) {
      uint8_t sendbuffer[20]; 
      voltageString_total.getBytes(sendbuffer,20);
      uint8_t sendbuffersize = 20;
      
      // write the data
      BTLEserial.write(sendbuffer, sendbuffersize);
    }


} //end of void loop()
