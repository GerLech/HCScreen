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

//some menu entries to demenstrate the use of markup
//markup starts with a control character 
//\2 = font color followed by 3 bytes for red, green and blue dont use 0 for color values !
//\3 = background color followed by 3 bytes for red, green and blue dont use 0 for color values !
//\4 = position text followed by 1 byte for absolute position. Dont use 0 !
//the markup will be endet with \1
String menu[] = {
  "\2\xff\1\1Red\1 Entry", // font color ff,1,1 = red
  "\2\1\xff\1Green\1 Entry", // font color 1,ff,1 = green
  "\2\1\1\xffBlue\1 Entry", // font color 1,1,ff = blue
  "\2\xff\xff\xff\3\xff\1\1White on Red\1", //font color ff,ff,ff = white and background ff,1,1 = red
  "\2\1\1\1\3\1\xff\1Black on Green\1", //font color 1,1,1 = black and background 1,ff,1 green
  "\3\xff\1\1X\3\xff\x7f\1X\3\xff\xff\1X\3\x7f\xff\1X\3\1\xff\1X\3\1\xff\x7fX\3\1\xff\xffX\3\1\x7f\xffX\3\1\1\xffX\1",
  //background colors ff,1,1 - ff,7f,1 - ff,ff,1 - 7f,ff,1 - 1,ff,1 - 1,ff,7f - 1,ff,ff - 1,7f,ff - 1,1,ff
  //like a rainbow from red to blue
  "7\4\5Entry", //position 5
  "8 \4\6Entry", //position 6
  "9\4\7Entry", //position 7
  "10\4\10Entry", //position 10 
  "Entry\4\1XXXXXXXXX", //position 1 overwrite "ntry" by "XXXXXXXXXX"
  "Entry12",
  "Entry13",
  "Entry14",
  "Entry15",
  "Back"
  };
uint8_t menu_cnt = 16;
volatile int tmp = 0; //to read joy values;
uint8_t wait = 0;



//tft instance
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
//HC sreen library
HCScreen screen = HCScreen(tft);

//callback for joy button event
void enter(int mode) {
  int index = screen.getSelectionIndex();
  Serial.print(index);
  Serial.println(" was selected");
}


void setup() {
  Serial.begin(115200);
  //important initialize the display
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);
  tft.fillScreen(ST7735_BLACK);
  screen.init();
  screen.setLineHeight(10);
  screen.setBaseColor(0x9542f4,0xe3f7d9);
  screen.setMenu(menu,menu_cnt);
  //init joy stick
  screen.initJoy(JOY_X,JOY_Y,JOY_BTN,enter);

}

void loop() {
  //react on joy stick changes
  screen.handleJoy();
}
