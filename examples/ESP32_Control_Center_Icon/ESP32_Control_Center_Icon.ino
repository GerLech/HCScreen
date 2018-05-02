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
//icons to be used
#include "Icons/icon_home.c"
#include "Icons/icon_ok.c"
#include "Icons/icon_cancel.c"
#include "Icons/icon_exit.c"
#include "Icons/icon_info.c"
#include "Icons/icon_help.c"
#include "Icons/icon_new.c"
#include "Icons/icon_add.c"
#include "Icons/icon_delete.c"
#include "Icons/icon_close.c"
#include "Icons/icon_register.c"
#include "Icons/icon_open.c"
#include "Icons/icon_edittxt.c"
#include "Icons/icon_network.c"
#include "Icons/icon_wifi.c"
#include "Icons/icon_search.c"
#include "Icons/icon_setup.c"
#include "Icons/icon_save.c"
#include "Icons/icon_check.c"
#include "Icons/icon_reload.c"



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

//some constants
#define MAIN  0
#define FORM  1
#define DIRECTORY 2


//tft instance
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
//HC sreen library
HCScreen screen = HCScreen(tft);

//global variables
uint8_t whichDisplay = 0; //variable to hold current display usage

//show a grid with 20 icons
void showIcons() {
  screen.initIconGrid();
  screen.showIcon(0,&icon_home);
  screen.showIcon(1,&icon_ok);
  screen.showIcon(2,&icon_cancel);
  screen.showIcon(3,&icon_exit);
  screen.showIcon(4,&icon_info);
  screen.showIcon(5,&icon_help);
  screen.showIcon(6,&icon_new);
  screen.showIcon(7,&icon_add);
  screen.showIcon(8,&icon_delete);
  screen.showIcon(9,&icon_close);
  screen.showIcon(10,&icon_register);
  screen.showIcon(11,&icon_open);
  screen.showIcon(12,&icon_edittxt);
  screen.showIcon(13,&icon_network);
  screen.showIcon(14,&icon_wifi);
  screen.showIcon(15,&icon_search);
  screen.showIcon(16,&icon_setup);
  screen.showIcon(17,&icon_save);
  screen.showIcon(18,&icon_check);
  screen.showIcon(19,&icon_reload);
  whichDisplay = MAIN;
}

//show some text and a grid with two icons bottom left
void showForm() {
  tft.fillScreen(0xFFFF);
  tft.setCursor(5,5);
  tft.print("This is a Form");
  screen.initIconGrid(96,96,2,1);
  screen.showIcon(0,&icon_ok);
  screen.showIcon(1,&icon_cancel); 
  whichDisplay = FORM; 
}

//callback to handle selection
//click on the open icon opens a SD card directory
//click on setup icon open a test form
void clicked(int8_t scmode) {
  int selection = screen.getSelectionIndex();
  switch(whichDisplay) {
    case MAIN:
      switch(selection) {
        case 11: screen.setDirectory("/",SD_CS);
          whichDisplay = DIRECTORY;
        break;
        case 16: showForm();
          break;
      }
      break;
    case FORM: showIcons();
      break;
    case DIRECTORY: showIcons();
      break;
  }
}


//setup
void setup() {
  Serial.begin(115200);
  //important initialize the display
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);
  tft.fillScreen(ST7735_BLACK);
  //init HCScreen library
  screen.setLineHeight(10);
  screen.init();
  //init joystick and set callback function
  screen.initJoy(JOY_X,JOY_Y,JOY_BTN,clicked);

  //show main icon grid
  showIcons();

}

void loop() {
  //react on joystick
  screen.handleJoy();
}
