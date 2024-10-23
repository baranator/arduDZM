#include <Arduino.h>

#include <Servo.h>

Servo myservo;  // create Servo object to control a servo

void setup() {
  Serial.begin(9600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(PB3, OUTPUT);
  myservo.attach(PB4);
}

void loop() {
  // put your main code here, to run repeatedly:
    digitalWrite(PB3, HIGH);   // turn the LED on (HIGH is the voltage level)
  myservo.write(0);
  delay(2000);                       // wait for a second
  Serial.println("switchi");
  digitalWrite(PB3, LOW);    // turn the LED off by making the voltage LOW
  myservo.write(180);
  delay(2000);                       // wait for a second
}
