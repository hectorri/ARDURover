#include <Servo.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

//RX
#define CEPIN 8
#define CSNPIN 7
#define DELAY 0
RF24 radio(CEPIN, CSNPIN);
const uint64_t pipes[1] = {0xF0F0F0F0E1LL};

typedef struct{
  int CAMH;
  int CAMV;
  int MOTOR1;
  int MOTOR2;
  int DIRECTION1;
  int DIRECTION2;
}DataReceived;

DataReceived data; 

//Motores Rover
#define MOTORA 5
#define MOTORB 6
#define DIRECTIONA 3
#define DIRECTIONB 4

//Servos camara
#define SERVO1 A2
#define SERVO2 A3
#define SERVOLIMIT 5
#define SERVOMARGIN 7
#define SERVOCENTERH 85
#define SERVOCENTERV 65
#define SERVOSPEED 3
Servo camServo1;
Servo camServo2;
int camServo1Pos = SERVOCENTERH;
int camServo2Pos = SERVOCENTERV;

void setup() {
  Serial.begin(9600);
  
  //Motores
  pinMode (MOTORA, OUTPUT);
  pinMode (DIRECTIONA, OUTPUT);
  pinMode (MOTORB, OUTPUT);
  pinMode (DIRECTIONB, OUTPUT);
  
  //Servos cámara
  camServo1.attach(SERVO1);
  camServo2.attach(SERVO2);
  
  //Radio
  radio.begin();
  radio.openReadingPipe(1,pipes[0]);
  radio.startListening();
}

void loop() {
  // radio stuff
  if (radio.available()){
      bool done = false;
      while (!done){
          done = radio.read( &data, sizeof(data) );
      }
      
      radio.stopListening();
      radio.startListening();
      Serial.print("DataSend.CAMH = ");
      Serial.print(data.CAMH);
      Serial.print(" DataSend.CAMV = ");
      Serial.println(data.CAMV);
      Serial.print("DataSend.MOTOR1GAS = ");
      Serial.print(data.MOTOR1);
      Serial.print(" DataSend.MOTOR2GAS = ");
      Serial.println(data.MOTOR2);
      Serial.print("DataSend.DIRECTION1 = ");
      Serial.print(data.DIRECTION1);
      Serial.print(" DataSend.DIRECTION2 = ");
      Serial.println(data.DIRECTION2);
      
      moveRover(data.MOTOR1, data.MOTOR2, data.DIRECTION1, data.DIRECTION2);
      moveCamera(data.CAMH, data.CAMV);
  }
    delay(DELAY);
}

void moveCamera(int valueStickH2, int valueStickV2){
    //Movimiento horizontal
    if(valueStickH2 < 0 && camServo1Pos <= (180-SERVOLIMIT)){
      camServo1Pos += SERVOSPEED;
    }else if (valueStickH2 && camServo1Pos >= (0 + SERVOLIMIT)){
      camServo1Pos -= SERVOSPEED;
    }
  
    //Movimiento vertical
    if(valueStickV2 < 0 && camServo2Pos <= (130-SERVOLIMIT)){
      camServo2Pos += SERVOSPEED;
    }else if (valueStickV2 > 0 && camServo2Pos >= (0 + SERVOLIMIT)){
      camServo2Pos -= SERVOSPEED;
    }
      
    camServo1.write(camServo1Pos);
    camServo2.write(camServo2Pos);
}

void moveRover(int motor1, int motor2, int direction1, int direction2){
    analogWrite (MOTORA, motor1);
    analogWrite (MOTORB, motor2);
    digitalWrite (DIRECTIONA, direction1); 
    digitalWrite (DIRECTIONB, direction2);
    
}


