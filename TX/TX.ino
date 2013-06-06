#include <Servo.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//TX
#define CEPIN 48
#define CSNPIN 49
RF24 radio(CEPIN, CSNPIN);
const uint64_t pipes[1] = {0xF0F0F0F0E1LL};
typedef struct{
  int CAMH;
  int CAMV;
  int MOTOR1;
  int MOTOR2;
  int DIRECTION1;
  int DIRECTION2;
}DataSend;
DataSend data;

#define DELAY 0
#define TIMEOUT 0

//Joysticks
#define STICKH1 A0
#define STICKV1 A1
#define STICKH2 A3
#define STICKV2 A4
#define HIGHSTICK 600
#define LOWSTICK 450

//Servos camara
#define SERVO1 A8
#define SERVO2 A9

//LCD
LiquidCrystal_I2C lcd(0x27,16,2); 

void setup() {
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("CONECTANDO...");
    delay(500);
    //TX
    radio.begin();
    radio.openWritingPipe(pipes[0]);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("CONECTADO");
}

void loop() {
    
    //Movimiento camara
    int valueStickH2 = analogRead(STICKH2);
    int valueStickV2 = analogRead(STICKV2);
    moveCamera(valueStickH2, valueStickV2);
    
    //Movimiento Rover
    int valueStickH1 = analogRead(STICKH1);
    int valueStickV1 = analogRead(STICKV1);
    moveRover(valueStickH1, valueStickV1);
        //Inicialización radio
        radio.stopListening();
        bool ok = radio.write( &data, sizeof(data) );
        radio.startListening();
        unsigned long started_waiting_at = millis();
        bool timeout = false;
        while ( ! radio.available() && ! timeout ){
            if (millis() - started_waiting_at > TIMEOUT ){
                timeout = true;
            }
        }
        // serial print received data
      Serial.print("DataSend.MOTOR1GAS = ");
      Serial.print(data.MOTOR1);
      Serial.print(" DataSend.MOTOR2GAS = ");
      Serial.println(data.MOTOR2);
      Serial.print("DataSend.DIRECTION1 = ");
      Serial.print(data.DIRECTION1);
      Serial.print(" DataSend.DIRECTION2 = ");
      Serial.println(data.DIRECTION2);
    // end of serial printing
    
    delay(DELAY);
}

void moveCamera(int valueStickH2, int valueStickV2){
    //Aceleración y dirección
    if(valueStickV2 > HIGHSTICK && 
        valueStickH2 > LOWSTICK && valueStickH2 < HIGHSTICK){//Arriba
      data.CAMH = 0;
      data.CAMV = 1;
    }else if(valueStickV2 < LOWSTICK&& 
        valueStickH2 > LOWSTICK && valueStickH2 < HIGHSTICK){//Abajo
      data.CAMH = 0;
      data.CAMV = -1;
    }else if(valueStickH2 > HIGHSTICK){//Derecha
      data.CAMH = 1;
      data.CAMV = 0;
    }else if(valueStickH2 < LOWSTICK){//Izquierda
      data.CAMH = -1;
      data.CAMV = 0;
    }else{
      data.CAMH = 0;
      data.CAMV = 0;
    }
}

void moveRover(int valueStickH1, int valueStickV1){
    int valueGas = abs((valueStickV1-512)/2) - 20;
    int valueTurn = abs((valueStickH1-512)/2) - 20;
    //Aceleración y dirección
    if(valueStickV1 > HIGHSTICK && 
        valueStickH1 > LOWSTICK && valueStickH1 < HIGHSTICK){//Adelante
      data.MOTOR1 = valueGas;
      data.MOTOR2 = valueGas;
      data.DIRECTION1 = 1;
      data.DIRECTION2 = 1;
    }else if(valueStickV1 < LOWSTICK&& 
        valueStickH1 > LOWSTICK && valueStickH1 < HIGHSTICK){//Atrás
      data.MOTOR1 = valueGas;
      data.MOTOR2 = valueGas;
      data.DIRECTION1 = 0;
      data.DIRECTION2 = 0;
    }else if(valueStickH1 > HIGHSTICK){//Derecha
      data.MOTOR1 = valueTurn;
      data.MOTOR2 = valueTurn;
      data.DIRECTION1 = 0;
      data.DIRECTION2 = 1;
    }else if(valueStickH1 < LOWSTICK){//Izquierda
      data.MOTOR1 = valueTurn;
      data.MOTOR2 = valueTurn;
      data.DIRECTION1 = 1;
      data.DIRECTION2 = 0;
    }else{
      data.MOTOR1 = 0;
      data.MOTOR2 = 0;
      data.DIRECTION1 = 0;
      data.DIRECTION2 = 0;
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("V1=");
    lcd.print(valueStickV1);
    lcd.setCursor(0,1);
    lcd.print("H1=");
    lcd.print(valueStickH1);
    lcd.setCursor(8,0);
    lcd.print("G=");
    lcd.print(valueGas);
    lcd.setCursor(8,1);
    lcd.print("T=");
    lcd.print(valueTurn);
}

