/***************************************************
  This is a library for the AZ-Delivery 1.8" TFT module 
  Or similar SPI displays together with ESP 32
  Other boards should work too but need different pins

  TFT Display -> ESP32
  CS -> GPIO5
  RESET -> GPIO22
  A0 -> GPIO21
  SDA -> GPIO23 (MOSI)
  SCK -> GPIO18 (SCK)+

  VCC -> 3.3 V
  LED -> 3.3 V
  
  SD-Card Reader -> ESP32
  CD-CS -> GPIO16
  CD-MOSI -> GPIO23 (MOSI)
  CD-MISO -> GPIO19 (MISO)
  CD-SCK -> GPIO18 (SCK)

  For input a thumb joystick will be used

  Joy-Stick -> ESP32
  SW -> GPIO4
  VRy -> GPIO14
  VRx -> GPIO15

  Written by Gerald Lechner
  MIT license, all text above must be included in any redistribution
 ****************************************************/




#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library

#include <HCScreen.h>

#include <SPI.h>
#include <SD.h>

//Definition of used pins
#define TFT_CS  5  // Chip select line for TFT display
#define TFT_RST  22  // Reset line for TFT 
#define TFT_DC   21  // Data/command line for TFT

#define SD_CS    16  // Chip select line for SD card

#define JOY_BTN  4 //Button of joy stick
#define JOY_Y  14 //Y-pos of joy stick
#define JOY_X  15 //X-pos of joy stick

volatile int tmp = 0; //to read joy values;
uint8_t wait;



//tft instance
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
//HC sreen library
HCScreen screen = HCScreen(tft);

//callback function called if the enter key was clicked
void enter(int mode) {
  //Send the result to the serial monitor
  Serial.println("Ergebnis: "+screen.getResult());
}

void setup() {
  Serial.begin(115200);
  //important initialize the display
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);
  tft.fillScreen(ST7735_BLACK);
  screen.init();
  //set colors for keyboard black for font, light gray for background
  //and red for selection and cursor
  screen.setKeyboardColor(0x0,0xF0F0F0,0xFF0000);
  //initialize keyboard set result to "Test"
  screen.initKeyboard("Test");
  screen.setLineHeight(10);
  //init joy stick handler
  screen.initJoy(JOY_X,JOY_Y,JOY_BTN,enter);

}

void loop() {
  //react on joystick changes
  screen.handleJoy();
}
