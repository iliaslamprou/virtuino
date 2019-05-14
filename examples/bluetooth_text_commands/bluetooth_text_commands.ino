
/* Virtuino example "Send - Receive Text Commands (Bluetooth)"
 * Created by Ilias Lamprou
 * Updated AUG 01 2017
 * This is the code you need to run on your arduino board to cummunicate whith VirtUino app via bluetooth
 * Before  running this code config the settings below as the instructions on the right
 * 
 * 
 * Download latest Virtuino android app from the link: https://play.google.com/store/apps/details?id=com.virtuino_automations.virtuino
 * Getting starting link:
 * Video tutorial link: https://www.youtube.com/watch?v=CYR_jigRkgk
 * Contact address for questions or comments: iliaslampr@gmail.com
 */

/*========= VirtuinoBluetooth Class methods  
*  vPinMode(int pin, int state)                                  set pin as digital OUTPUT or digital INPUT.  (Insteed default pinMode method
*
*========= Virtuino General methods  
*  void vDigitalMemoryWrite(int digitalMemoryIndex, int value)   write a value to a Virtuino digital memory   (digitalMemoryIndex=0..31, value range = 0 or 1)
*  int  vDigitalMemoryRead(int digitalMemoryIndex)               read  the value of a Virtuino digital memory (digitalMemoryIndex=0..31, returned value range = 0 or 1)
*  void vMemoryWrite(int analogMemoryIndex, float value)         write a value to Virtuino float memory       (memoryIndex=0..31, value range as float value)
*  float vMemoryRead(int analogMemoryIndex)                      read the value of  Virtuino analog memory    (analogMemoryIndex=0..31, returned value range = 0..1023)
*  run()                                                         neccesary command to communicate with Virtuino android app  (on start of void loop)
*  void vDelay(long milliseconds);                               Pauses the program (without block communication) for the amount of time (in miliseconds) specified as parameter
*  int getPinValue(int pin)                                      read the value of a Pin. Usefull for PWM pins
*  void clearTextBuffer();                                       Clear the text received text buffer
*  int textAvailable();                                          Check if there is text in the received buffer
*  String getText(byte ID);                                      Read the text from Virtuino app
*  void sendText(byte ID, String text);                          Send text to Virtuino app  
*/




#include "VirtuinoBluetooth.h"                           // Include VirtuinoBluetooth library to your code

// Code to use SoftwareSerial
#include <SoftwareSerial.h>                              //  Disable this line if you want to use hardware serial 
SoftwareSerial bluetoothSerial =  SoftwareSerial(2,3);   // arduino RX pin=2  arduino TX pin=3    connect the arduino RX pin to bluetooth module TX pin   -  connect the arduino TX pin to bluetooth module RX pin.  Disable this line if you want to use hardware serial 
VirtuinoBluetooth virtuino(bluetoothSerial);       // Set SoftwareSerial baud rate.  -  Disable this line if you want to use hardware serial 


// Code to use HardwareSerial
// VirtuinoBluetooth virtuino(Serial1);            // enable this line and disable all SoftwareSerial lines
                                                   // Open VirtuinoBluetooth.h file on the virtuino library folder ->  disable the line: #define BLUETOOTH_USE_SOFTWARE_SERIAL 
                                                  


                                              


byte storedPinState=0;

//================================================================== setup
//==================================================================
//==================================================================
void setup(){

  virtuino.DEBUG=true;               // set this value TRUE to enable the serial monitor status
  Serial.begin(9600);                // Set serial monitor baud rate
  
  bluetoothSerial.begin(9600);         // Enable this line if you want to use software serial (UNO, Nano etc.)
  //Serial1.begin(9600);               // Enable this line if you want to use hardware serial (Mega, DUE etc.)
  
   // Use virtuino.vPinMode instead of default pinMode method for digital input or digital output pins.
   virtuino.vPinMode(6,INPUT);                                  // Don't use vPinMode for PWM pins, or pins that their values change many times per second
                                                                // Every time the value of a vPinMode  pin is changed the virtuino library sends a message and inform virtuino about the pin value.
                                                                // If an PWM pin is declared as vPinMode pin the arduino will continuously try  to send data to android device because the value of a pwm pin changes constantly  
   pinMode(13,OUTPUT);                 
   
//------ enter your setup code below
  

 
    
    
}

//================================================================== loop
//==================================================================
//==================================================================

void loop(){
   virtuino.run();           //  necessary command to communicate with Virtuino android app
   
    //------ enter your loop code below here
    //------ avoid to use delay() function in your code. Use the command virtuino.vDelay() instead of delay()

    //--------------------------------------------------------
   //----- example 1 how to read a Text Command from virtuino
   if (virtuino.textAvailable()){          // check if exist a TEXT Command from Virtuino app
     String text= virtuino.getText(0);     // Read the command by Channel ID
     //String text2= virtuino.getText(2);  // Read the command by Channel ID
     Serial.println("Text of ID 0="+text); // Print the command to serial display
     virtuino.clearTextBuffer();           // Clear all Text Commands from buffer

     if (text.equals("0")) {
        digitalWrite(13,0);                                // do something 
     }
     else 
       if (text.equals("1")){
          digitalWrite(13,1);                            // do something 
           virtuino.sendText(1,"Hello Virtuino the pin is 1");        // Write text to text channel 1
       }
   }

     
   //--------------------------------------------------------
   //----- example 2 how to send a Text Command to virtuino
   byte pinState=digitalRead(6);       // read Pin 6 state
   if (pinState!=storedPinState) {     // check if pin value has changed
      virtuino.vDelay(100);                       // wait 100 millis
      if (digitalRead(6)==1) virtuino.sendText(1,"Hello Virtuino the pin is 1");
      else virtuino.sendText(1,"Now the pin is 0");
      storedPinState=pinState;
   }
   
   
 }

