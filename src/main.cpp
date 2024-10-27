#include <Arduino.h>

#include <Servo.h>

#include <TinyWireM.h>
#include <Tiny4kOLED.h>

uint8_t oledWidth = 128;
uint8_t oledHeight = 64;

#include "m38_font.h"
const DCfont *oledFont = FONTM38;

Servo myservo;  // create Servo object to control a servo


#define ADC_PIN 5
uint16_t adcMax=0;
uint16_t adcMin=1023;


void oledSetup(){


}

void oledShowRPM(){

}

void oledDebugLine(){

}

void updateMinMax(){
  uint16_t val=analogRead(ADC_PIN);
  if(val>adcMax){
    adcMax=val;
  }
  if(val<adcMin){
    adcMin=val;
  }
}

void setup() {
  //Serial.begin(9600);
  // initialize digital pin LED_BUILTIN as an output.
  //pinMode(PB3, OUTPUT);
  //myservo.attach(PB4);


   TinyWireM.begin();
  oled.begin(oledWidth, oledHeight, sizeof(tiny4koled_init_128x64br), tiny4koled_init_128x64br);
  oled.clear();
  oled.setFont(FONT6X8);
  oled.on();
  oled.setCursor(30, 4); // x: pixel / y: line
  oled.print("Hello world");
}

void loop() {
  updateMax();
  // // put your main code here, to run repeatedly:
  //   digitalWrite(PB3, HIGH);   // turn the LED on (HIGH is the voltage level)
  // myservo.write(0);
  // delay(2000);                       // wait for a second
  // Serial.println("switchi");
  // digitalWrite(PB3, LOW);    // turn the LED off by making the voltage LOW
  // myservo.write(180);
    oled.setCursor(30, 4); // x: pixel / y: line
  oled.print("Hello world");
  delay(2000);                       // wait for a second
    oled.setCursor(30, 4); // x: pixel / y: line
  oled.print("bye world");
}
