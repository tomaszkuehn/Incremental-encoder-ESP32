/*
 Inremental encoder
 GPIO14 - pulse1
 GPIO12 - pulse2
 GND - GND

 */
#include <Arduino.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();  // pins defined in User_Setup.h

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
uint8_t knobstack = 0xA0;
 
void IRAM_ATTR onTimer() {
  uint8_t kcopy, p1;
  
  portENTER_CRITICAL_ISR(&timerMux);
  kcopy = knobstack;
  portEXIT_CRITICAL_ISR(&timerMux);
  
  if ((kcopy!=0x30)&&(kcopy!=0xC0)) {
    //read knob GPIO
    p1 = 0;
    p1 = p1 | digitalRead(14)<<4;
    p1 = p1 | digitalRead(12)<<5;
    
    if (p1!=(kcopy & 0x30)) { //galka ruszona
        kcopy=kcopy<<2;
        kcopy=kcopy|p1;
        portENTER_CRITICAL_ISR(&timerMux);
        knobstack = kcopy;
        portEXIT_CRITICAL_ISR(&timerMux);
        Serial.print(p1, HEX);
    }
  }
 
}

void setup(void) {
  pinMode(12, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
  
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_YELLOW, TFT_BLACK); 
  tft.drawString("00",6,0,7);

  Serial.begin(9600);

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000, true);
  timerAlarmEnable(timer);
}


char knob() {
  char p,p1;
  // 11 00 01 11 00 01 11 01 00 11

  p=0;
  portENTER_CRITICAL_ISR(&timerMux);
  p1 = knobstack;
  portEXIT_CRITICAL_ISR(&timerMux);

  switch (p1)
  {
   case 0xC0: p=1;break;
   case 0x30: p=2;break;
  }
  if (p>0) {
    portENTER_CRITICAL_ISR(&timerMux);
    knobstack = 0xA0;
    portEXIT_CRITICAL_ISR(&timerMux);
  }
  return(p);
}

void loop() {
  int k = knob();
  while(k == 0) {
    k = knob();
  }
  tft.setCursor (8, 52);
  tft.setTextColor(TFT_BLACK, TFT_BLACK);
  tft.drawString("88",6,0,7);
  tft.setTextColor(0xFBE0); // Orange
  if( k == 1 ) {
    tft.drawString("11",6,0,7);  
  }
  else
  {
    tft.drawString("22",6,0,7);  
  }
}
