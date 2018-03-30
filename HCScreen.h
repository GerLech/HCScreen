/*
||
|| @file HCScreen.h
|| @version 1.0
|| @author Gerald Lechner
|| @contact lechge@gmail.com
||
|| @description
|| | HCScreen class is used to control a line oriented display
|| | on a TFT color display. It requires the libraries
|| |  Adafruit_ST7735 and Adafruit_GFX
|| #
||
|| @license
|| | This library is free software; you can redistribute it and/or
|| | modify it under the terms of the GNU Lesser General Public
|| | License as published by the Free Software Foundation; version
|| | 2.1 of the License.
|| |
|| | This library is distributed in the hope that it will be useful,
|| | but WITHOUT ANY WARRANTY; without even the implied warranty of
|| | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
|| | Lesser General Public License for more details.
|| |
|| | You should have received a copy of the GNU Lesser General Public
|| | License along with this library; if not, write to the Free Software
|| | Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
|| #
||
*/
#ifndef HCScreen_h
#define HCScreen_h

#include <Arduino.h>
#include <Adafruit_ST7735.h>
#include <FS.h>

#define	HC_BLACK   0x000000
#define HC_GRAY    0xC0C0C0
#define	HC_BLUE    0x0000FF
#define	HC_RED     0xFF0000
#define	HC_GREEN   0x00FF00
#define HC_CYAN    0x00FFFF
#define HC_MAGENTA 0xFF00FF
#define HC_YELLOW  0xFFFF00
#define HC_WHITE   0xFFFFFF


typedef struct { // Data stored for ICON
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
  unsigned char	 pixel_data[31*31*3+1];
} HCIcon;

class HCScreen {
public:
  //public functions
  HCScreen(Adafruit_ST7735& tft);
//set the font color and background color for standard lines
//default is font in black and background white
//color values are 3 bytes RGB as used in WEB
//colors will be converted to the 16 bit format required
//by tft display
void init();
void showContent(); //refresh display with current content
void setTitle(String title);
void setTitle(String title, uint16_t fontColor, uint16_t bgColor);
void setMenu(String menu[],uint8_t entries);
void setBaseColor(unsigned long font_color, unsigned long bg_color);
void setSelectionColor(unsigned long font_color, unsigned long bg_color);
void setTitleColor(unsigned long font_color, unsigned long bg_color);
void setGridColor(unsigned long font_color, unsigned long bg_color);
void selectNext();
void selectPrevious();
void moveRight(); //move selection to right in grid mode
void moveLeft(); //move selection to left in grid mode
String getSelection();
int8_t getSelectionIndex();
void showCodeset();
void setLineHeight(uint8_t height);
void setDirectory(String path, uint8_t SDcs);
String getTitle();
void setTextFile(String path, String fileName);
void initIconGrid();
void showIcon(uint8_t x, uint8_t y, const HCIcon *icon=NULL);
void showIcon(uint8_t index, const HCIcon *icon=NULL);

private:
  //private members
  Adafruit_ST7735& _tft;
  uint8_t _screenLines; //number of lines available
  uint8_t _leftMargin; //space pixels left
  uint8_t _rightMargin; //space pixels right
  uint8_t _topMargin; //space pixels top
  uint8_t _bottomMargin; //space pixels bottom
  uint8_t _lineLength; //characters in a line
  int8_t _selectedLine; //selected line -1 = none
  int8_t _selectedColumn; //xy mode only
  uint16_t _fontColor; //color for font 16 bit tft-format
  uint16_t _bgColor; //color for background 16 bit tft-format
  uint16_t _selFontColor; //color for font in selected line 16 bit tft-format
  uint16_t _selBgColor; //color for background in selected line 16 bit tft-format
  uint16_t _titleFontColor; //color for font in title line 16 bit tft-format
  uint16_t _titleBgColor; //color for background in title line 16 bit tft-format
  uint16_t _gridSelColor; //color for icon selection 16 bit tft-format
  uint16_t _gridBgColor; //color for background of icon grid line 16 bit tft-format
  uint8_t _showTitle=0; //if 1 title will be displayed
  String _title; //title to be displayed
  String _content[100]; //content to be displayed
  uint8_t _contentLines = 0; //number of lines in content
  uint8_t _startLine; //first line of content to be displayed
  uint8_t _lineHeight  = 8; //height of a line in pixels 8= minimal value
  uint8_t _rows = 4; //rows for icon grid
  uint8_t _columns = 5; //columns for icon grid
  uint8_t _gridMode = 0; //if 1 grid mode is aktive

  uint16_t convertColor(unsigned long webColor );
  void showLine(uint8_t lin, String txt, uint16_t font, uint16_t bg);
  uint8_t loadDir(fs::FS &fs, String path, uint8_t cnt);
  void showGridSelection(uint16_t color);
  void showOneLine(uint8_t lin, uint16_t fnt, uint16_t bg);
};


#endif
