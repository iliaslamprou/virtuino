/* Virtuino example  (Bluetooth)
 * Example name = "Read temperatures from two ds18b20 sensors "
 * Created by Ilias Lamprou
 * Updated 24 02 2017
 * This is the code you need to run on your arduino board to cummunicate whith VirtUino app via bluetooth
 * Before  running this code config the settings below as the instructions on the right
 * 
 * Download latest Virtuino android app from the link: https://play.google.com/store/apps/details?id=com.virtuino_automations.virtuino
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
*/


#include <OneWire.h>
#include <DallasTemperature.h>
 
// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 6       // Connect two or more ds18b20 sensors to pin 6 
 
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
 
 #include "VirtuinoBluetooth.h"                   // virtuino library 1.47 or highest
 // Code to use SoftwareSerial
 #include <SoftwareSerial.h>                              //  Disable this line if you want to use hardware serial 
 SoftwareSerial bluetoothSerial =  SoftwareSerial(2,3);   // arduino RX pin=2  arduino TX pin=3    connect the arduino RX pin to bluetooth module TX pin   -  connect the arduino TX pin to bluetooth module RX pin.  Disable this line if you want to use hardware serial 
 VirtuinoBluetooth virtuino(bluetoothSerial);       // Set SoftwareSerial baud rate.  -  Disable this line if you want to use hardware serial 

 // Code to use HardwareSerial
 // VirtuinoBluetooth virtuino(Serial1);            // enable this line and disable all SoftwareSerial lines
                                                    // Open VirtuinoBluetooth.h file on the virtuino library folder ->  disable the line: #define BLUETOOTH_USE_SOFTWARE_SERIAL 
                                                    // Connect HC-05 module to Arduino (MEGA or DUE) Serial1. (pins: 18,19)
 


 //======================================================================== setup
 //========================================================================
void setup(void){
  Serial.begin(9600);     // start monitor serial port
  
 bluetoothSerial.begin(9600);         // Disable this line if you want to use software serial (Mega, DUE etc.)
 //Serial1.begin(9600);               // Enable this line if you want to use hardware serial (Mega, DUE etc.)
  
  virtuino.DEBUG=true;               // set this value TRUE to enable the serial monitor status
 
  // Start up the library Dallas Temperature IC Control Library
  sensors.begin();
  Serial.println("Setup complete.");
}
 
//======================================================================== setup
void readTemperatures(){
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  Serial.println(" Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");

  // You can have more than one IC on the same bus. 
  // 0 refers to the first IC on the wire
  float temperature1=sensors.getTempCByIndex(0);
  float temperature2=sensors.getTempCByIndex(1);
  Serial.println("Temperature for Device 1 is: "+String(temperature1));
  Serial.println("Temperature for Device 2 is: "+String(temperature2));

  virtuino.vMemoryWrite(0,temperature1);    // write temperature 1 to virtual pin V0. On Virtuino panel add a value display or an analog instrument to pin V0
  virtuino.vMemoryWrite(1,temperature2);    // write temperature 1 to virtual pin V1. On Virtuino panel add a value display or an analog instrument to pin V1
}

//======================================================================== setup
//========================================================================
void loop(void){
  virtuino.run();               //  necessary command to communicate with Virtuino android app

    //------ enter your loop code below here
    //------ avoid to use delay() function in your code. Use the command virtuino.vDelay() instead of delay()
    //------ your code .....

   
  readTemperatures();           // Dont' read the sensors values every cicle
  virtuino.vDelay(1000);        // Add a delay at least 1 second long. 
 
}


