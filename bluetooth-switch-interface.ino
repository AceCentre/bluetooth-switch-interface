/*********************************************************************
Bluetooth Switch Interface 

 Ace Centre is a UK based charity developing solutions for Augmentative
 and Alternative Communication and Assistive Technology. We promote
 an open design approach for hardware and software. https://acecentre.org.uk/

 If you're able to support us, your donation will help fund our development.
 https://www.justgiving.com/onecheckout/donation/direct/charity/139889

 The Bluetooth Switch Interface is based on the Adafruit Feather 32u4
 Bluefruit LE development board. 
 https://learn.adafruit.com/adafruit-feather-32u4-bluefruit-le/overview
 
 MIT license, check LICENSE for more information
 All text above must be included in any redistribution
*********************************************************************/

// Set the number of switches
const int switchQuantity = 6; 

// Set which pins on the board the switches are connected to
const int switchPins[] = {18, 19, 20, 21, 22, 11};

// Set which key (or text) should be sent with each switch press
const char* switchKeys[] = {"A", "B", "C", "D", "E", "Hello"};

// Delay after switch press to avoid switch bounce
const int switchDebounceDelay = 30;

// Other variables which will may not need changing
const int ledPin = 13;  // the number of the LED pin
const char* btDeviceName = "Hello";
int switchState[switchQuantity];
int switchLastState[switchQuantity];
int switchOutput[switchQuantity];

//Need to figure out which of these are required
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif

#define FACTORYRESET_ENABLE         0
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

void error(const __FlashStringHelper*err) {

  // Flash LED if error
  digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
  delay(1000); 
  
  while (1);
}

void setup(void)
{
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  
  for (int n=0 ; n < switchQuantity ; n++)
  {
    switchState[n] = 0;
    switchLastState[n] = 0;
  }  

  // Initialise the BT module
  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  // Disable command echo from Bluefruit
  ble.echo(false);

  // Print Bluefruit information
  ble.info();

  // Change the device name to make it easier to find 
  if (! ble.sendCommandCheckOK(F( "AT+GAPDEVNAME=Ace BT Switch Interface")) ) {
    error(F("Could not set device name?"));
  }

  // Enable HID Service
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    if ( !ble.sendCommandCheckOK(F( "AT+BleHIDEn=On" ))) {
      error(F("Could not enable Keyboard"));
    }
  }else
  {
    if (! ble.sendCommandCheckOK(F( "AT+BleKeyboardEn=On"  ))) {
      error(F("Could not enable Keyboard"));
    }
  }

  // Add or remove service requires a reset
  if (! ble.reset() ) {
    error(F("Couldn't reset??"));
  }
}

void loop(void)
{

  // Detect switch press
  for (int n=0 ; n < switchQuantity ; n++)
  {
    switchState[n] = digitalRead(switchPins[n]);
    if (switchState[n] != switchLastState[n]) {
      if (switchState[n] == HIGH) { //switch leading edge

        sendCharacter(switchKeys[n]);
      } 
      else {  // switch falling edge
      }
      // slight delay to allow for switch debounce
      delay(switchDebounceDelay);
    }
  switchLastState[n] = switchState[n];
  }
}

void sendCharacter(String message)
{
  //Need to figure out this line
  char keys[BUFSIZE+1]; 
  
  //Line required to tell device that serial date to be sent as BT keytstroke
  ble.print("AT+BleKeyboard=" + message);

  //Not sure what this line does, but does not work without it!
  ble.println(keys);
}
