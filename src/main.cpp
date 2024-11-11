#include <Arduino.h>




#define DEBUG
#define RPM_MAX 8000
#define RPM_CRIT 6000
#define RPM_MIN 200
#define WELCOME_MSG "Hidiho!"
#define DSP_REFRESH_MS 100
#define OLED_DISPLAY
#undef SERVO_DISPLAY


#ifdef SERVO_DISPLAY
  #include <Servo.h>
#endif
#ifdef OLED_DISPLAY
  #include <TinyWireM.h>
  #include <Tiny4kOLED.h>
  #include "m38_font.h"
#endif


#ifdef SERVO_DISPLAY
  #ifdef OLED_DISPLAY
    #error Choose either OLED or SERVO, not both!
  #endif
#endif


const uint8_t oledWidth = 128;
const uint8_t oledHeight = 64;
#define ADC_PIN 5
uint16_t adcMax=0;
uint16_t adcMin=1023;
//low voltage count to detect spark safely
const uint8_t lvths=5;
uint8_t lvcount=0;
#define EMA_ALPHA 0.3

unsigned long lastSpark=0;
uint16_t rpm_EMA=0;
unsigned long lastDisplayRefresh=0;


#ifdef OLED_DISPLAY
void oledSetup(){
  TinyWireM.begin();
  oled.begin(oledWidth, oledHeight, sizeof(tiny4koled_init_128x64br), tiny4koled_init_128x64br);
  oled.clear();
  oled.setFont(FONT6X8);
  oled.on();
    oled.setCursor(30, 4); // x: pixel / y: line
  oled.print(WELCOME_MSG);
}

void drawBar(uint16_t rpm){
  uint8_t e=0b00000000;
  uint8_t f=0b11111111;
  uint16_t segs=(RPM_MAX/1000.0);
  uint16_t segw=128/segs;

  uint16_t segf=round(rpm/1000.0);


    if(segw%2!=0){
      segw--;
    }
    oled.setCursor(0, 0);
    oled.startData();
    for (uint8_t x=0; x<segs; x++) {
      //invert colors for reached ticks
      if(segf>x){
        e=0b00000000;
        f=0b11111111;
      }else{
        f=0b00000000;
        e=0b11111111;
      }
      //mark critical rpms
      if(x>RPM_CRIT/1000){
          f=f^0b00011000;
          e=e^0b00011000;
      }
      for(uint16_t i=0;i<segw-1;i++){
        oled.sendData(f);
      }
      //tick
      oled.sendData(e);
    }
    oled.endData();

}


void oledShowRPM(uint16_t rpm){
  //BAR
  drawBar(rpm);
  //TEXT
  oled.setFontX2(FONTM38);
  if(rpm>RPM_CRIT){
    oled.setCursor(5, 2);
    oled.print("(!)");
  }
  oled.setCursor(32, 2);
  oled.print(rpm);
  
}

void oledDebugLine(){
  oled.setCursor(1, oledHeight/8-1); // x: pixel / y: line
  oled.setFont(FONT6X8);
  String d="min: ";
  d+=adcMin;
  d+=", max: ";
  d+=adcMax;
  oled.print(d.c_str());
}

void oledRefresh(){
  oled.clear();
  oledShowRPM(rpm_EMA);
  #ifdef DEBUG
    oledDebugLine();
  #endif
}
#endif

#ifdef SERVO_DISPLAY

Servo myservo;  // create Servo object to control a servo

void servoSetup(){
  pinMode(PB3, OUTPUT);
  myservo.attach(PB4);
}

#endif


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

  #ifdef OLED_DISPLAY
  oledSetup();
  #endif

  #ifdef SERVO_DISPLAY
  servoSetup();
  #endif


  lastSpark=micros();
}

void loop() {
  updateMinMax();
  
  if(adcMax>adcMin){
    //if range was not initialized
    return;
  }
  //only detect a new spark if the last one is at least half of the shortest period of highest rpm ago
  if(micros()-lastSpark < (1.0/RPM_MAX)*60*1000*1000/2){
    return;
  }

  uint8_t val=analogRead(ADC_PIN);
  if(val < adcMax-0.2*(adcMax-adcMin)){
    lvcount++;
  }else{
    lvcount--;
  }
  if(lvcount<0){
    lvcount=0;
  }
  if(lvcount>=lvths){
    //spark detected, calc momentary rpm
    unsigned long currentSpark=micros();
    unsigned long periodLength=currentSpark-lastSpark;
    lastSpark=currentSpark;
    lvcount=0;

    //flatten by exponential moving average (EMA)
    uint16_t rpm=60*1.0*1000*1000/(periodLength);
    rpm_EMA= EMA_ALPHA * rpm + (1-EMA_ALPHA)*rpm_EMA;
    
  }

  //if there was not spark detected for a timeperiod that implicates less than RPM_MIN rpm, assume motor is off/at zero rpm
  if(micros()-lastSpark >  (1.0/(RPM_MIN*60))*1000*1000){
    rpm_EMA=0;
  }

  unsigned long now=millis();
  if(now-lastDisplayRefresh>DSP_REFRESH_MS){
    #ifdef OLED_DISPLAY
      oledRefresh();
    #endif
    
    lastDisplayRefresh=now;
  }


  // // put your main code here, to run repeatedly:
  //   digitalWrite(PB3, HIGH);   // turn the LED on (HIGH is the voltage level)
  // myservo.write(0);
  // delay(2000);                       // wait for a second
  // Serial.println("switchi");
  // digitalWrite(PB3, LOW);    // turn the LED off by making the voltage LOW
  // myservo.write(180);
}
