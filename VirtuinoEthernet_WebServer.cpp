/* Virtuino Ethernet Shield web server library
 * Created by Ilias Lamprou
 * Modify by Rico Schumann
 * Updated 16/05/2019
 * 
 * Download latest Virtuino android app from the link: https://play.google.com/store/apps/details?id=com.javapapers.android.agrofarmlitetrial ???
 * Contact address for questions or comments: iliaslampr@gmail.com
 */



//========= Virtuino General methods  

//  void vDigitalMemoryWrite(int digitalMemoryIndex, int value)       write a value to a Virtuino digital memory   (digitalMemoryIndex=0..31, value range = 0 or 1)
//  int  vDigitalMemoryRead(int digitalMemoryIndex)                   read  the value of a Virtuino digital memory (digitalMemoryIndex=0..31, returned value range = 0 or 1)
//  void vMemoryWrite(int memoryIndex, float value);                  write a value to Virtuino memory             (memoryIndex=0..31, value range as float value)
//  float vMemoryRead(int memoryIndex);                               read a value of  Virtuino memory             (memoryIndex=0..31, returned a float value
//  run();                                                            neccesary command to communicate with Virtuino android app  (on start of void loop)
//  int getPinValue(int pin);                                         read the value of a Pin. Usefull to read the value of a PWM pin
 


#include "VirtuinoEthernet_WebServer.h"
#include <SPI.h>
#include <Ethernet.h>
//#include <Ethernet2.h>


 
//====================================================================================== VirtuinoEthernet_WebServer init
//======================================================================================
  VirtuinoEthernet_WebServer::VirtuinoEthernet_WebServer(EthernetServer *VirtuinoServer){
  MyServer = VirtuinoServer;
}

 
 
 
//====================================================================================== run
//======================================================================================
 void VirtuinoEthernet_WebServer::run(){
    EthernetClient client = MyServer->available();
  if (client) {
    char c;
   if (DEBUG) Serial.println(et_clientConnected);
    
    while (client.connected()) {
      if (client.available()) {
        c = client.read();
        lineBuffer+=c;
        if (DEBUG)  Serial.write(c);
      }
      if (c == '\n' ) {
              int pos = lineBuffer.indexOf("GET /");
              if (pos!=-1){                                   // We have a GET message
                  lineBuffer.remove(0,pos+5);
                  if (DEBUG) Serial.println("\n\r LineBuffer="+lineBuffer);
                  checkVirtuinoCommand(&lineBuffer);
                  if (textToSendCommandBuffer.length()>0){
                     etResponseBuffer+=textToSendCommandBuffer;
                     textToSendCommandBuffer="";
                  }
                   if (DEBUG) Serial.println("\n\r response="+etResponseBuffer);
                  delay(10);
                  client.flush();
                  client.println("HTTP/1.1 200 OK");
                  client.println("Content-Type: text/html");
                  client.println("Connection: close");
                  client.println();
                  client.println(etResponseBuffer);
                  lineBuffer="";
                  break;
                } // if pos
             lineBuffer="";
        } // if c=='\n'
    } // if client.available
    delay(5);
	client.flush();
    client.stop();  // close the connection:
    if (DEBUG) Serial.println(et_clientDisconnected);
  }
  }
  
//====================================================================================== checkVirtuinoCommand
//======================================================================================
void   VirtuinoEthernet_WebServer::checkVirtuinoCommand(String* command){
     int pos=command->lastIndexOf(et_COMMAND_END_CHAR);                      
  if (pos>5){                                          // check the size of command
      command->remove(pos+1);     // clear the end of command buffer
     //------------------  get command password
      String commandPassword="";
      int k=command->indexOf(et_COMMAND_START_CHAR);
       if (k>0) {
        commandPassword = command->substring(0,k);
        command->remove(0,k);   // clear the start part of the command  buffer
       }
      etResponseBuffer="";
      if ((password.length()==0) || (commandPassword.equals(password))) {                       // check if password correct
           while (command->indexOf(et_COMMAND_START_CHAR)>=0){
              int cStart=command->indexOf(et_COMMAND_START_CHAR);
              int cEnd=command->indexOf(et_COMMAND_END_CHAR);
              if (cEnd-cStart>5){
                String oneCommand = command->substring(cStart+1,cEnd);                               // get one command
                char commandType = getCommandType(oneCommand.charAt(0));
                  if (commandType!='E') {
                     int pin= getCommandPin(&oneCommand);
                     if (pin!=-1){
                        boolean returnInfo=false;
                        if (oneCommand.charAt(4)=='?') returnInfo=true;
                        oneCommand.remove(0,4);
                        if (oneCommand.length()<et_TEXT_COMMAND_MAX_SIZE) executeReceivedCommand(commandType,pin,&oneCommand,returnInfo);
                        else {
                          etResponseBuffer+=getErrorCommand(et_ERROR_SIZE);
                          String s="";
                          if (pin<9) s+="0";
                          s+=String(pin);
                          etResponseBuffer+=et_COMMAND_START_CHAR;
                          etResponseBuffer+='T';
                          etResponseBuffer+=s;
                          etResponseBuffer+="= ";           // return an empty text if the size of text command is too big
                          etResponseBuffer+=et_COMMAND_END_CHAR;  
                          
                        }
                        lastCommunicationTime=millis();
                     } else  etResponseBuffer +=getErrorCommand(et_ERROR_PIN);  // response  error pin number   !E00=1$   
                  } else etResponseBuffer +=getErrorCommand(et_ERROR_TYPE);  // response  error type   !E00=3$   
          
              } else{
                etResponseBuffer+=getErrorCommand(et_ERROR_SIZE);  // response  error size of command   !E00=4$   
              }
              command->remove(0,cEnd+1-cStart);
           }  // while
      } else etResponseBuffer+=getErrorCommand(et_ERROR_PASSWORD);     // the password is not correct
  }
  else  etResponseBuffer+=getErrorCommand(et_ERROR_SIZE);         // the size of command is not correct
 
}

 //====================================================================================== getCommandType
 //======================================================================================
  //This function returns the command type which was received from app
  //The second character of command string determines the command type
  // I  digital input read command    
  // Q  digital ouput write
  // D  digital memory read - write commang
  // A  analog input analog input read command
  // O  analog output write command
  // V  float memory read - write command
  // C  connect or disconect command    
  // E  error
  
  // Other characters are recognized as an error and function returns the character E
  //This is a system fuction. Don't change this code
  
  char  VirtuinoEthernet_WebServer::getCommandType(char c){
    if (!(c== 'I' || c == 'Q' || c == 'D' ||c == 'A' ||c == 'O' || c == 'V' || c == 'C' || c == 'T')){
      c='E'; //error  command
    }
    return c;
  }


  //======================================================================================  getCommandPin
  //======================================================================================
  //This function returns the pin number of the command string which was received from app
  //Fuction checks if pin number is correct. If not it returns -1 as error code
  //This is a system fuction. Don't change this code
 
  int  VirtuinoEthernet_WebServer::getCommandPin(String* aCommand){
    char p1= aCommand->charAt(1);
    char p2= aCommand->charAt(2);
    String s="-1";
    if (isDigit(p1) && isDigit(p2)) {
       s="";
       s+=p1;
       s+=p2;
    }
     return s.toInt();
  }

//======================================================================================  getCommandValue
//======================================================================================
float VirtuinoEthernet_WebServer::getCommandValue(String* aCommand){
     boolean er=false;
     for (int i=0;i<aCommand->length();i++){
        char c=aCommand->charAt(i);
        if (! ((c=='.') || (c=='-') || (isDigit(aCommand->charAt(i))))) er=true;
       }
      if (er==false) return aCommand->toFloat();
    
    return 0;
  }
//====================================================================================== getErrorCommand
//======================================================================================
String VirtuinoEthernet_WebServer::getErrorCommand(byte code){
  String rValue="";
  rValue+=et_COMMAND_START_CHAR;
  rValue+="E00=";
  rValue+=String(code);
  rValue+=et_COMMAND_END_CHAR;
  return rValue;
}

//================================================================================== urlencode
//==================================================================================
String VirtuinoEthernet_WebServer::urlencode(String* str){
   String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str->length(); i++){
      c=str->charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
    
}

//================================================================================== h2int
//==================================================================================
unsigned char VirtuinoEthernet_WebServer::h2int(char c){
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}
//================================================================================== urldecode
//==================================================================================
String VirtuinoEthernet_WebServer::urldecode(String* str){
    String encodedString="";
    char c;
    char code0;
    char code1;
    for (int i =0; i < str->length(); i++){
        c=str->charAt(i);
      if (c == '+'){
        encodedString+=' ';  
      }else if (c == '%') {
        i++;
        code0=str->charAt(i);
        i++;
        code1=str->charAt(i);
        c = (h2int(code0) << 4) | h2int(code1);
        encodedString+=c;
      } else{
         encodedString+=c;  
      }
      yield();
    }
    
   return encodedString;

}
//======================================================================================== clearTextBuffer
//========================================================================================
void VirtuinoEthernet_WebServer:: clearTextBuffer(){textReceivedCommandBuffer="";}

//======================================================================================== clearTextBuffer
//========================================================================================
int VirtuinoEthernet_WebServer::  textAvailable(){return textReceivedCommandBuffer.length();}
  
//======================================================================================== getTextByID
//========================================================================================
String VirtuinoEthernet_WebServer:: getText(byte ID){
   String returnedText="";
   String ID_string ="";
   ID_string+=et_COMMAND_START_CHAR;
   ID_string+='T';
   if (ID<10) ID_string+='0';
   ID_string+=ID;
   ID_string+="=";       
   int  pos=textReceivedCommandBuffer.indexOf(ID_string);
   if (pos>=0) {
      pos= textReceivedCommandBuffer.indexOf("=",pos);
      int lastPos=textReceivedCommandBuffer.indexOf(et_COMMAND_END_CHAR,pos);
      returnedText= textReceivedCommandBuffer.substring(pos+1,lastPos);
      returnedText=urldecode(&returnedText);
      clearTextByID(ID,&textReceivedCommandBuffer);
   }
   return returnedText;
  }

//======================================================================================== clearTextByID
//========================================================================================
void VirtuinoEthernet_WebServer:: clearTextByID(byte ID, String* textBuffer){
   String ID_string ="";
   ID_string+=et_COMMAND_START_CHAR;
   ID_string+='T';
   if (ID<10) ID_string+='0';
   ID_string+=ID;
   ID_string+="=";                   // !ID3=
  int  pos=textBuffer->indexOf(ID_string);
   if (pos>=0) {
      int lastPos= textBuffer->indexOf(et_COMMAND_END_CHAR,pos);
      textBuffer->remove(pos,lastPos+1);
   }
}

//======================================================================================== addTextToBuffer
//========================================================================================
void VirtuinoEthernet_WebServer::addTextToReceivedBuffer(byte ID, String* text){
   clearTextByID(ID,&textReceivedCommandBuffer);
   textReceivedCommandBuffer+=et_COMMAND_START_CHAR;
   textReceivedCommandBuffer+= 'T';
   if (ID<10) textReceivedCommandBuffer+= '0';
   textReceivedCommandBuffer+=ID;
   textReceivedCommandBuffer+= "=";
   textReceivedCommandBuffer+= *text;
   textReceivedCommandBuffer+=et_COMMAND_END_CHAR;
}

//======================================================================================== addTextToBuffer
//========================================================================================
void VirtuinoEthernet_WebServer::sendText(byte ID, String text){
   clearTextByID(ID,&textToSendCommandBuffer);
   textToSendCommandBuffer+=et_COMMAND_START_CHAR;
   textToSendCommandBuffer+= 'T';
   if (ID<10) textToSendCommandBuffer+= '0';
   textToSendCommandBuffer+=ID;
   textToSendCommandBuffer+= "=";
   textToSendCommandBuffer+=urlencode(&text); 
   textToSendCommandBuffer+=et_COMMAND_END_CHAR;
}




//======================================================================================== executeCommandFromApp
//========================================================================================
 //======================================================================================== executeCommandFromApp
//========================================================================================
  void VirtuinoEthernet_WebServer::executeReceivedCommand(char activeCommandType, int activeCommandPin ,String* commandString, boolean returnInfo){
    //------------------------------------
   // Every time we have a complete command from bluetooth the global value commandState is set to 1 
   // The value activeCommandType contains command type such as Digital output, Analog output etc.
   // The value activeCommandPin contains the pin number of the command
   // The value activeCommandValue contains the value (0 or 1 for digital, 0-255 for analog outputs, 0-1023 for analog memory) 
   // In this void we have to check if the command is correct and then execute the command 
   // After executing the command we have to send a response to app
   // At last we must reset the command state value to zero 
   //This is a system fuction. Don't change this code
      
    String response=getErrorCommand(et_ERROR_UNKNOWN); 
    String pinString="";
    if (activeCommandPin<10) pinString = "0"+String(activeCommandPin);
    else pinString=String(activeCommandPin);
    float activeCommandValue=0;
     switch (activeCommandType) {
      case 'T':                         
            if ((activeCommandPin>=0) & (activeCommandPin < 100)){
                response =et_COMMAND_START_CHAR;
                response +=activeCommandType;
                response +=pinString;
                response +="=";
                response +=*commandString;
                response +=et_COMMAND_END_CHAR;  // response 
                addTextToReceivedBuffer(activeCommandPin,commandString);
            }
          break;
      case 'I':                         
            if ((activeCommandPin>=0) & (activeCommandPin < et_arduinoPinsSize))
              response =et_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(digitalRead(activeCommandPin))+et_COMMAND_END_CHAR;  // response 
            else   response =getErrorCommand(et_ERROR_PIN);  // response  error pin number   !E00=1$       
          break;
       case 'Q': 
            if ((activeCommandPin>=0) & (activeCommandPin < et_arduinoPinsSize)){
                 if (returnInfo) response =et_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(digitalRead(activeCommandPin))+et_COMMAND_END_CHAR;  // response 
                 else {
                   activeCommandValue = getCommandValue(commandString);
                   if ((activeCommandValue == 0) || (activeCommandValue == 1)) {
                          digitalWrite(activeCommandPin,activeCommandValue);
                          arduinoPinsValue[activeCommandPin]=activeCommandValue;
                          response =et_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(digitalRead(activeCommandPin))+et_COMMAND_END_CHAR;  // response 
                   } else    response =getErrorCommand(et_ERROR_VALUE);
                   }
            } else   response =getErrorCommand(et_ERROR_PIN);;  // response  error pin number   !E00=1$   
          break; 


       case 'D':
            if ((activeCommandPin>=0) & (activeCommandPin<et_virtualDigitalMemorySize)){ 
                if (returnInfo) response =et_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(VirtuinoEthernet_WebServer::vDigitalMemoryRead(activeCommandPin))+et_COMMAND_END_CHAR;  // response 
                else{
                      activeCommandValue = getCommandValue(commandString);
                      VirtuinoEthernet_WebServer::vDigitalMemoryWrite(activeCommandPin, activeCommandValue); 
                      response =et_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(VirtuinoEthernet_WebServer::vDigitalMemoryRead(activeCommandPin))+et_COMMAND_END_CHAR;  // response 
                }// else    response =getErrorCommand(et_ERROR_VALUE);
            } else   response =getErrorCommand(et_ERROR_PIN);  // response  error pin number   !E00=1$   
            break; 
      
       case 'V': 
           if ((activeCommandPin>=0) & (activeCommandPin<et_virtualAnalogMemorySize)){
               if (returnInfo) response =et_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(VirtuinoEthernet_WebServer::vMemoryRead(activeCommandPin))+et_COMMAND_END_CHAR;  // response
               else { 
                    activeCommandValue = getCommandValue(commandString);
                   	VirtuinoEthernet_WebServer::vMemoryWrite(activeCommandPin, activeCommandValue);
                    response =et_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(VirtuinoEthernet_WebServer::vMemoryRead(activeCommandPin))+et_COMMAND_END_CHAR;  // response
                    } 
           } else   response =getErrorCommand(et_ERROR_PIN);
          break;
       case 'O': 
           if ((activeCommandPin>=0) & (activeCommandPin < et_arduinoPinsSize)){
                   
                   if (returnInfo) {
                      int pwm_value = pulseIn(activeCommandPin, HIGH);
                      pwm_value = pwm_value /7.85;
                      response =et_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(pwm_value)+et_COMMAND_END_CHAR;  // response 
                  }
                   else {
                    activeCommandValue = getCommandValue(commandString); 
                          arduinoPinsValue[activeCommandPin]=(int) activeCommandValue;
                          analogWrite(activeCommandPin,(int)activeCommandValue);
                          response =et_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String((int)activeCommandValue)+et_COMMAND_END_CHAR;  // response 
                   }
                        
           } else   response =getErrorCommand(et_ERROR_PIN);;
          break;

         case 'A':                        
            if ((activeCommandPin>=0) & (activeCommandPin < arduinoAnalogPinsSize))
              response ="!"+String(activeCommandType)+pinString+"="+String(analogRead(analogInputPinsMap[activeCommandPin]))+"$";  // response 
            else   response =getErrorCommand(et_ERROR_PIN);  // response  error pin number   !E00=1$       
          break;  

          case 'C':  
                      response="";  
                       activeCommandValue= getCommandValue(commandString);
                      if (activeCommandValue==1)response =et_firmwareCode;//"!C00="+String(et_firmwareCode)"+"$";//et_COMMAND_START_CHAR+"C"+pinString+"=1"+et_COMMAND_END_CHAR;     // return firmware version
                      break;
           }
        //   Serial.println("=== response="+response);
        etResponseBuffer += response;
       
  }

//====================================================================================== getPinValue
//======================================================================================
int VirtuinoEthernet_WebServer::getPinValue(int pin){
  if (pin>=0 && pin<et_arduinoPinsSize)
	  return arduinoPinsValue[pin];
  else return 0;
  }

//====================================================================================== vDigitalMemoryWrite
//======================================================================================
 void VirtuinoEthernet_WebServer::vDigitalMemoryWrite(int digitalMemoryIndex, int value){
  if ((digitalMemoryIndex>=0) && (digitalMemoryIndex<et_virtualDigitalMemorySize)){
	  
	  // hear you can also write a Routine, to save the Value in a external SRAM and save dynamical memory!!!
	  
    virtualDigitalMemory[digitalMemoryIndex]=value;
    
  }
  
}
//====================================================================================== vDigitalMemoryRead
//======================================================================================
 int VirtuinoEthernet_WebServer::vDigitalMemoryRead(int digitalMemoryIndex){
  if ((digitalMemoryIndex>=0) & digitalMemoryIndex<et_virtualDigitalMemorySize){
	  
	   // hear you can also write a Routine, to read the Value from a external SRAM and save dynamical memory!!!
	  
    return virtualDigitalMemory[digitalMemoryIndex];
  }
  else return 0;    // error
}

//====================================================================================== vMemoryWrite
//======================================================================================
// This void must not be called more than two times per second
void VirtuinoEthernet_WebServer::vMemoryWrite(int memoryIndex, float value){
  if ((memoryIndex>=0) & memoryIndex<et_virtualAnalogMemorySize){
	  
	   // hear you can also write a Routine, to save the Value in a external SRAM and save dynamical memory!!!
	  
      virtualFloatMemory[memoryIndex]=value;
  }
}

//====================================================================================== vMemoryRead
//======================================================================================
 float VirtuinoEthernet_WebServer::vMemoryRead(int memoryIndex){
  if ((memoryIndex>=0) & memoryIndex<et_virtualAnalogMemorySize){
	  
	   // hear you can also write a Routine, to read the Value from a external SRAM and save dynamical memory!!!
	  
    return virtualFloatMemory[memoryIndex];
  }
  else return 0;    // error
}


//====================================================================================== vDelay
//======================================================================================
void VirtuinoEthernet_WebServer::vDelay(long milliseconds){
  long timeStart=millis();
  while (millis()-timeStart<milliseconds) run();
}
