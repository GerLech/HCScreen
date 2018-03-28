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

//some constants
#define NOSELECT 0
#define MAIN_MENU 1
#define SUB_MENU 2
#define BROWSER 3
#define TEXTFILE 4

//examples of menu definition
String main_menu[] = {"Submenu","Codeset","SD-Card","Entry4","Entry5","Entry6","Entry7","Entry8","Entry9","Entry10","Entry11","Entry12","Entry13","Entry14","Entry15","Back"};
uint8_t main_menu_cnt = 16;

String sub_menu[] = {"Sub1","Sub2","Sub3","Back"};
uint8_t sub_menu_cnt = 4;

volatile int tmp = 0; //to read joy values;
volatile int whichMenu = 0; //define menu to switch handler
String lastPath = ""; //to allow .. command in browser



//tft instance
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
//HC sreen library
HCScreen screen = HCScreen(tft);

//a select event for main menu occured
void handleMainMenu(String selection, int8_t selectionIndex) {
  if (selection == "Back") {
  } else {
    switch(selectionIndex) {
      case 0: screen.setMenu(sub_menu,sub_menu_cnt);
        screen.setTitle("Sub Menu");
        whichMenu = SUB_MENU;
        break;
      case 1: screen.showCodeset();
        whichMenu = NOSELECT;
        break;
      case 2: screen.setDirectory("/DCIM",SD_CS);
        whichMenu = BROWSER;
        break;
    }
  }
}

//a select event for sub menu occured
void handleSubMenu(String selection, int8_t selectionIndex) {
  if (selection == "Back") {
      screen.setMenu(main_menu,main_menu_cnt);
      screen.setTitle("Main Menu");
      whichMenu=MAIN_MENU;
  } else {
    switch(selectionIndex) {
    }
  }
}

//a select event for browser occured
void handleBrowser(String selection, int8_t selectionIndex) {

  if (selection == "..") { //the go back entry was selected
    String path = screen.getTitle();
    Serial.println(path);
    if ((path == "Error") || (path == "/")) {
      //if error or root menu go back to parent menu
      screen.setMenu(main_menu,main_menu_cnt);
      screen.setTitle("Main Menu");
      whichMenu=MAIN_MENU;
    } else {
      //otherwise set path one step up
      uint8_t idx = path.lastIndexOf("/");
      if (idx == 0) {
        path = "/";
      } else {
        path.remove(idx);
      }
      screen.setDirectory(path,SD_CS);
    }
  } else {
    //selected entry starts with an asteriks means it is a directory
    if (selection.startsWith("*")) {
      screen.setDirectory(selection.substring(1),SD_CS);
    } else if (selection.endsWith(".txt") || selection.endsWith(".TXT")) {
      //if it is a text file we will display the content
      lastPath = screen.getTitle();
      screen.setTextFile(screen.getTitle(),selection);
      Serial.println(lastPath);
      whichMenu = TEXTFILE;
    }
  }
}


void setup() {
  Serial.begin(115200);
  //important initialize the display
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);
  tft.fillScreen(ST7735_BLACK);
  screen.init();
  screen.setBaseColor(0x9542f4,0xe3f7d9);
  screen.setMenu(main_menu,main_menu_cnt);
  screen.setTitle("Main Menu");
  screen.setLineHeight(10);
  //define input pins
  pinMode(JOY_BTN, INPUT_PULLUP);
  pinMode(JOY_Y, INPUT_PULLUP);
  pinMode(JOY_X, INPUT_PULLUP);
  while (digitalRead(JOY_BTN)==0) delay(100);

}

void loop() {
  
    //read joystick X currently not used
    tmp = analogRead(JOY_X);
    //read joystick y value is between 0 and 4096
    tmp = analogRead(JOY_Y);
    if (tmp > 3000) {
      //if value > 3000 we select the next entry of a menu
      //or scroll down if no selection active
      screen.selectNext();
    };
    if (tmp < 1000) {
      //if value < 1000 we select the previous entry of a menu
      //or scroll up if no selection active
      screen.selectPrevious();
    }
    
  //select button of the joystick
  if (digitalRead(JOY_BTN)==0) {
    //wait until button was released
    while (digitalRead(JOY_BTN) == 0) delay(10);
    //we get current selection from the library
    String selection = screen.getSelection();
    int8_t selectionIndex = screen.getSelectionIndex();
    switch (whichMenu) {
      case MAIN_MENU: handleMainMenu(selection,selectionIndex);
        break;
      case SUB_MENU: handleSubMenu(selection,selectionIndex);
        break;
      case BROWSER: handleBrowser(selection,selectionIndex);
        break;
      case TEXTFILE: screen.setDirectory(lastPath,SD_CS);
        whichMenu = BROWSER;
        break;
      default : screen.setMenu(main_menu,main_menu_cnt);
        screen.setTitle("Main menu");
        whichMenu = MAIN_MENU;
        break;
    }
  }
}
