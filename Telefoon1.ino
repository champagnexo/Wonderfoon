/***************************************************
TELEFOON I - BEIGE - CODERDOJO TIENEN
****************************************************/

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

/**
Draaischijf
*/
#define ROOD 2
#define BLAUW 3

/**
Hoorn -> 5V naar (1) op telefoon, uitlezen op (5)
HIGH = hoorn van de haak, LOW = hoorn op de haak
*/
#define ORANJE 5

#define VOLUME 30
#define COUNTER_MODIFYER 1

long pin2Counter = 0;
long pin3Counter = 0;
long idleCounter = 0;
long antiDubbel = 500;
boolean pin2LastState;
boolean pin3LastState;
boolean pin5LastState;
int pin2Value = 0;
int pin3Value = 0;
int pin5Value = 0;

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

void setup()
{
  mySoftwareSerial.begin(9600);
  Serial.begin(9600);
  
  Serial.println();
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.volume(30);  //Set volume value. From 0 to 30
  // myDFPlayer.play(1);  //Play the first mp3  
  
  // Initialiseer draaischijf
  pinMode(ROOD, INPUT_PULLUP);
  pinMode(BLAUW, INPUT_PULLUP);
  pin2LastState = digitalRead(ROOD);
  pin3LastState = digitalRead(BLAUW);

  // initialiseer hoorn
  pinMode(ORANJE, INPUT_PULLUP);
  pin5LastState = digitalRead(ORANJE);
  
  idleCounter = 0;
  pin2Counter = 0;
  pin3Counter = 0;
  pin2Value = 0;
  pin3Value = 0;
  pin5Value = 0;
}

void loop()
{  
  boolean pin2State = digitalRead(ROOD);
  boolean pin3State = digitalRead(BLAUW);
  boolean pin5State = digitalRead(ORANJE);
  
  idleCounter++;
  if (idleCounter==0x30000)
  {
    idleCounter = 0;
    Serial.println("");
    pin2Value = 0;
    pin3Value = 0;
  }

  if (pin2LastState != pin2State)
  {
    if (pin2Counter < antiDubbel)
      pin2Counter++;
    else {
      if (pin2State) {
        Serial.println("io2 0 naar 1");
      }
      else {
        if (!pin3State)
          pin2Value++;
        Serial.println("io2 1 naar 0");
        Serial.println(pin2Value);
      }
      pin2LastState = pin2State;
      pin2Counter = 0;
    }
    idleCounter = 0;
  }

  if (pin3LastState != pin3State)
  {
    if (pin3Counter < antiDubbel)
      pin3Counter++;
    else {
      if (pin3State) {
        Serial.println("io3 0 naar 1");
        Serial.println("Gedraaid nummer: ");
        Serial.println((pin2Value + 1) % 10);
        myDFPlayer.stop();
        myDFPlayer.playFolder(1,(pin2Value + 1) % 10);
        pin2Value = 0;
      }
      else {
        Serial.println("io3 1 naar 0");
      }
      pin3LastState = pin3State;
      idleCounter = 0;
      pin3Counter = 0;
    }
    idleCounter = 0;
  }

  if (pin5LastState != pin5State)
  {
  	if (pin3State) {
    	Serial.println("io5 0 naar 1");
    	Serial.println("Hoorn op de haak");
    	pin5Value = 1;
  	}
  	else {
    	Serial.println("io5 1 naar 0");
      Serial.println("Hoorn van de haak");
  	}
    pin5LastState = pin5State;
    idleCounter = 0;
  }
  
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
}


void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F(""));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
