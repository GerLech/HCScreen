//Version 0.13
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
#include "Adafruit_ST7735.h"
#include "Arduino.h"
#include "HCScreen.h"
#include "FS.h"
#include "SD.h"
#define CHAR_WIDTH 6

//This is a mapping table for special characters
const char ANSI[128] = {
  69,32,44,159,34,242,197,206,
  32,32,83,60,79,32,90,32,
  32,239,239,34,34,250,45,196,
  126,32,115,62,111,32,122,89,
  32,173,189,156,207,190,221,245,
  249,184,166,174,170,32,169,238,
  248,241,253,252,239,230,244,250,
  44,251,248,175,172,171,243,168,
  183,181,182,65,142,65,146,128,
  212,144,210,211,222,214,215,216,
  209,165,227,224,226,229,153,158,
  157,235,233,234,154,237,231,224,
  133,160,131,198,132,134,145,135,
  138,130,136,137,141,161,140,139,
  208,164,149,162,147,228,148,246,
  155,151,163,150,129,236,232,152
};



//constructor prefill private members
HCScreen::HCScreen(Adafruit_ST7735& tft): _tft(tft) {
  init();
  _selectedLine = -1; //selected line -1 = none
  _fontColor = convertColor(HC_BLACK);
  _bgColor = convertColor(HC_WHITE);
  _selFontColor = convertColor(HC_WHITE);
  _selBgColor = convertColor(HC_BLACK);
  _titleFontColor = convertColor(HC_YELLOW);
  _titleBgColor = convertColor(HC_BLUE);
  _gridSelColor = convertColor(HC_RED);
  _gridBgColor = convertColor(HC_WHITE);
  _title = "";
  _startLine = 0;
  _contentLines = 0;
  _showTitle = 0;
  _tft.fillScreen(_bgColor);
  _lineLength = 8;
}

//Initialize screen geometry this function needs to be called whenever
//orientation of the TFT display was changed
void HCScreen::init() {
  uint8_t swidth = _tft.width();
  uint8_t sheight = _tft.height();
  _lineLength = swidth / CHAR_WIDTH; //we need 6 pixels/character
  _leftMargin = (swidth-(_lineLength*CHAR_WIDTH))/2;
  _rightMargin = swidth-(_lineLength*CHAR_WIDTH)-_leftMargin;
  _screenLines = (sheight + 1 - _lineHeight) / _lineHeight; //we need 10 pixels/line
  _topMargin = (sheight-(_screenLines*_lineHeight))/2;
  _bottomMargin = sheight-(_screenLines*_lineHeight)-_topMargin;
  _tft.fillScreen(_bgColor);
}

//show all visible lines out of the content array beginning with _startLine.
//this allows to display more text which can be scrolled by modifying
//_startLine. Currently the number of lines is limited to 100
void HCScreen::showContent()
{
  //lines
  uint8_t line = _startLine; //content Line
  uint8_t dLine = 0; //display line
  if (_showTitle > 0){
    showLine(dLine,_title,_titleFontColor,_titleBgColor);
    dLine++;
  }
  String txt = "";
  String txt1 = "";
  uint8_t idx = 0;
  while (dLine < _screenLines) {
    txt = "";
    if (line < _contentLines) {
      txt = _content[line];
      if (_selectedLine == line) {
        showLine(dLine,txt,_selFontColor,_selBgColor);
      } else {
        showLine(dLine,txt,_fontColor,_bgColor);
      }
      dLine ++;
      line ++;
    } else {
      showLine(dLine,txt,_fontColor,_bgColor);
      dLine ++;
      line ++;
    }
  }
}

//display one line. Special characters will be mapped from the ANSI code
//to the ASCII code of the display
void HCScreen::showLine(uint8_t lin, String txt, uint16_t font, uint16_t bg) {
  uint8_t i;
  uint8_t y = lin * _lineHeight + _topMargin;
  uint8_t tIndex = 0;
  uint8_t tLen = txt.length();
  char c = 0;
  _tft.setCursor(_leftMargin,y+1);
  _tft.setTextColor(font,bg);
  for (i = 0; i<_lineLength; i++) {
    if (tIndex < tLen) {
      c = txt[tIndex];
      if (c>127) c=ANSI[c-128];
      if (c > 31) _tft.print(c);
      tIndex++;
    } else {
      _tft.print(" ");
    }
  }

}

//shows all available characters ordered by code number
//16 characters in a line and 16 lines
void HCScreen::showCodeset() {
  _showTitle = 0;
  _selectedLine = -1;
  _startLine = 0;
  _tft.fillScreen(_bgColor);
  uint8_t lin =0;
  uint8_t pos = 0;
  char c = 0;
  for (lin = 0; lin<16; lin++) {
    _tft.setCursor(_leftMargin,(lin * 8));
    for (pos = 0; pos < 16; pos++) {
      _tft.print(c);
      c++;
    }
  }

}
//show a 31x31 pixel icon on position calculated from index
//the bitmap is a rgb bitmap with 8-bit R, G and B
void HCScreen::showIcon(uint8_t index, const HCIcon *icon){
  if (index >= _icons) index = _icons-1;
  uint8_t row  = index / _columns;
  uint8_t column = index % _columns;
  showIcon(column,row,icon);
}

//show a 31x31 pixel icon on position x y
//the bitmap is a rgb bitmap with 8-bit R, G and B
void HCScreen::showIcon(uint8_t column, uint8_t row, const HCIcon *icon){
  if (icon) {

    uint8_t x = column * 32 +_gridX;
    uint8_t y = row * 32 + _gridY;
    if (x>(_tft.width()-32)) x = _tft.width()-32;
    if (y>(_tft.height()-32)) y = _tft.height()-32;
    uint8_t ix = 0;
    uint8_t iy = 0;
    uint16_t idx = 0;
    uint16_t color = 0;
    for (iy = 0; iy<icon->height; iy++){
      for (ix = 0; ix<icon->width; ix++) {
        color = _tft.Color565(icon->pixel_data[idx++], icon->pixel_data[idx++], icon->pixel_data[idx++]);
        _tft.drawPixel(x+ix+1,y+iy+1,color);
      }
    }
  }
}

//Allows to set a title bar on top of the display
void HCScreen::setTitle(String title){
  _showTitle = 1;
  _title = title;
  showContent();
}

//Allows to set a title bar on top of the display and define
//used font color and background color
//colors are in the 3-byte RGB format and will be converted
//to the 16-bit format used by the display
void HCScreen::setTitle(String title, uint16_t fontColor, uint16_t bgColor){
  setTitleColor(fontColor,bgColor);
  setTitle(title);
}

//fill content with a list of entries and activate selection
void HCScreen::setMenu(String menu[],uint8_t entries){
  _tft.fillScreen(_bgColor);
  uint8_t i;
  for (i=0; i<entries; i++){
    _content[i]=menu[i];
  }
  _tft.fillScreen(_bgColor);
  _selectedLine = 0;
  _contentLines = entries;
  _startLine = 0;
  _gridMode = 0;
  showContent();
}

//fill content with directory SDcs is the pin number used for
//card readers chip select
//if SD card cant be mounted, error will be displayed
//otherwise title bar shows the curent path
void HCScreen::setDirectory(String path, uint8_t SDcs){
  _tft.fillScreen(_bgColor);
  uint8_t cnt = 1;
  if (_title == "") _title = "/";
  _content[0]=".."; //to exit current menu
  if(!SD.begin(SDcs)){
      _content[cnt]="No Card";
      _title = "Error";
      cnt++;
  } else {
    Serial.println(path);
    Serial.println(_title);
    if (!path.startsWith("/") ) {
      if (_title.endsWith("/")) {
        path = _title + path;
      } else {
        path = _title + "/" + path;
      }
    }
    if (path == "") path = "/";
    _title = path;
    cnt = loadDir(SD,path,cnt);
  }
  _selectedLine = 0;
  _startLine = 0;
  _contentLines = cnt;
  _showTitle = 1;
  _gridMode = 0;
  showContent();
}

//Display the first 100 lines of a text file
//long lines will be splitted
//UTF8 codes will be converted
void HCScreen::setTextFile(String path, String fileName) {
  _tft.fillScreen(_bgColor);
  int8_t idx = 0;
  if (!path.endsWith("/")) path += "/";
  File dataFile = SD.open(path+fileName);
  _showTitle = 1;
  _title = fileName;
  _selectedLine = -1; //no line selection
  uint8_t lin = 0;
  String line = "";
  String txt = "";
  char c = 0;
  // if the file is available, write to it:
  if (dataFile) {
    while (dataFile.available() && (lin<100)) {
      line = "";
      do {
        c= dataFile.read();
        if (c > 31) {
          if (c==0xc3) {
            c=dataFile.read();
            c=c+64; //convert to ANSI Latin 1
          }
          if (c==0xc2) {
            c=dataFile.read();
          }
        line += String(c);
        }
      } while ((c != 13) && dataFile.available());
      do {
        if (line.length() >= _lineLength ) {
          idx = line.lastIndexOf(" ",_lineLength);
          if (idx >= 0) {
            txt=line.substring(0,idx);
            line = line.substring(++idx);
          } else {
            txt=line.substring(0,_lineLength);
            line = line.substring(_lineLength);
          }
        } else {
          txt = line;
          line = "";
        }
        Serial.println(txt);
        _content[lin]=txt;
        lin++;
      } while ((line != "") && (lin < 100)) ;
    }
    dataFile.close();
    _contentLines = lin;
  }  else {
    _content[0] = "File not open";
    _contentLines = 1;
  }
  _startLine = 0;
  _gridMode = 0;
  showContent();
}


//initialize icon grid full screen calculate rows and columns
//clear background
void HCScreen::initIconGrid() {
  initIconGrid(0,0,_tft.width()/32,_tft.height()/32);
}

//initialize icon grid on part of the screen
void HCScreen::initIconGrid(uint8_t x, uint8_t y, uint8_t columns, uint8_t rows) {
  if (x>(_tft.width()-32)) x = _tft.width()-32;
  if (y>(_tft.height()-32)) y = _tft.height()-32;
  if (rows > ((_tft.height()-y)/32)) rows = (_tft.height() - y) /32;
  if (columns > ((_tft.width()-x)/32)) columns = (_tft.width() - x) /32;
  _columns = columns;
  _rows = rows;
  _gridX = x;
  _gridY = y;
  _icons = _columns * _rows;
  _tft.fillRect(x,y,columns*32,rows*32,_gridBgColor);
  _showTitle = 0;
  _gridMode = 1;
  _selectedLine = 0;
  _selectedColumn = 0;
  showGridSelection(_gridSelColor);
}

//set font color and background color for normal text
//colors are in the 3-byte RGB format and will be converted
//to the 16-bit format used by the display
void HCScreen::setBaseColor(unsigned long font_color, unsigned long bg_color) {
  _fontColor = convertColor(font_color);
  _bgColor = convertColor(bg_color);
  _tft.fillScreen(_bgColor);
  showContent();
}

//set font color and background color for the selected line
//colors are in the 3-byte RGB format and will be converted
//to the 16-bit format used by the display
void HCScreen::setSelectionColor(unsigned long font_color, unsigned long bg_color) {
  _selFontColor = convertColor(font_color);
  _selBgColor = convertColor(bg_color);
  showContent();
}

//set font color and background color for the title bar
//colors are in the 3-byte RGB format and will be converted
//to the 16-bit format used by the display
void HCScreen::setTitleColor(unsigned long font_color, unsigned long bg_color) {
  _titleFontColor = convertColor(font_color);
  _titleBgColor = convertColor(bg_color);
  showContent();
}

//set selection color and background color for icon grid
//colors are in the 3-byte RGB format and will be converted
//to the 16-bit format used by the display
void HCScreen::setGridColor(unsigned long font_color, unsigned long bg_color) {
  _gridSelColor = convertColor(font_color);
  _gridBgColor = convertColor(bg_color);
  showContent();
}

//this private function converts colors from 3-byte RGB format
//to the 16-bit format (5-bit R, 6-bit G and 5-bit B)
//which is used in the display
uint16_t HCScreen::convertColor(unsigned long webColor) {
  unsigned long r = (webColor & 0xF80000) >> 8;
  unsigned long g = (webColor & 0xFC00) >> 5;
  unsigned long b = (webColor & 0xF8) >> 3;
  uint16_t tftColor = r | g | b;
  return tftColor;
}

//moves the selected line downwards if the bottom is reached and there
//are more lines in the content array, the display will be scrolled up
void HCScreen::selectNext() {
  uint8_t lin;
  if (_gridMode) {
    if (_selectedLine < (_rows-1)) {
      showGridSelection(_gridBgColor);
      _selectedLine++;
      showGridSelection(_gridSelColor);
    }
  } else if (_selectedLine<0) { //selection is not activated
      if ((_startLine + _screenLines - _showTitle) < (_contentLines-1)) {
        _startLine++;
        showContent();
      }
  } else {
    if (_selectedLine < (_contentLines - 1)) {
      showOneLine(_selectedLine,_fontColor,_bgColor);
      _selectedLine++;
      if ((_selectedLine - _startLine + _showTitle) >= _screenLines) {
        _startLine++;
        showContent();
      } else {
        showOneLine(_selectedLine,_selFontColor,_selBgColor);
      }
    }
  }
}

void HCScreen::showOneLine(uint8_t lin, uint16_t fnt, uint16_t bg) {
  showLine(lin-_startLine+_showTitle,_content[lin],fnt,bg);
}

//moves the selected line upwards if the top is reached and there
//are more lines in the content array, the display will be scrolled up
void HCScreen::selectPrevious() {
  if (_gridMode) {
    if (_selectedLine > 0) {
      showGridSelection(_gridBgColor);
      _selectedLine--;
      showGridSelection(_gridSelColor);
    }
  } else if (_selectedLine<0) { //selection is not activated
      if (_startLine  > 0) {
        _startLine--;
        showContent();
      }
  } else {
    if (_selectedLine > 0) {
      showOneLine(_selectedLine,_fontColor,_bgColor);
      _selectedLine--;
      if (_selectedLine < _startLine) {
        _startLine--;
        showContent();
      } else {
        showOneLine(_selectedLine,_selFontColor,_selBgColor);
      }
    }
  }
}

//moves the selection to the right
void HCScreen::moveRight() {
  if (_gridMode) {
    if (_selectedColumn < (_columns-1)) {
      showGridSelection(_gridBgColor);
      _selectedColumn++;
      showGridSelection(_gridSelColor);
    }
  }
}


//moves the selection to the left
void HCScreen::moveLeft() {
  if (_gridMode) {
    if (_selectedColumn > 0) {
      showGridSelection(_gridBgColor);
      _selectedColumn--;
      showGridSelection(_gridSelColor);
    }
  }
}

//return the currently selected line if selection is activated
//otherwise an empty string will be returned
String HCScreen::getSelection() {
  if ((_selectedLine < 0) || (_gridMode)) {
    return "";
  } else {
    return _content[_selectedLine];
  }
}

//return the title
String HCScreen::getTitle() {
  return _title;
}

//return the index inside content array of the currently selected line
//or -1 if selection is not activated
int8_t HCScreen::getSelectionIndex() {
  if (_gridMode) {
    return _selectedLine * _columns + _selectedColumn;
  } else {
    return _selectedLine;
  }
}

//change the used line height
//do not set smaller then 8
//10 looks the best
void HCScreen::setLineHeight(uint8_t height) {
  _lineHeight = height;
  //after setting the line height it is required to reinitialize
  //and redisplay the content
  init();
  showContent();
}


//load the first 100 entries from a given path
//if an entry is a directory an asteriks will be shown in front
uint8_t HCScreen::loadDir(fs::FS &fs, String path, uint8_t cnt){
    Serial.println(path);
    uint8_t fcnt = cnt;
    File root = fs.open(path);
    if(!root){
        _content[cnt]="Failed to open directory";
        fcnt++;
    }
    else if(!root.isDirectory()){
        _content[cnt]="No directory";
        fcnt++;
    }
    else
    {
      File file = root.openNextFile();
      while(file && (fcnt<100)){
          String name = file.name();
          //strip _path
          uint8_t e = name.lastIndexOf("/");
          name = name.substring(++e);
          if(file.isDirectory()){
              _content[fcnt]="*" + name;
          } else {
            _content[fcnt]=name;
          }
          file = root.openNextFile();
          fcnt++;
      }
    }

  return fcnt;
}

//draw rectangle around selected selected icon
void HCScreen::showGridSelection(uint16_t color) {
  uint8_t x = _selectedColumn * 32 + _gridX;
  uint8_t y = _selectedLine * 32 + _gridY;
  _tft.drawRect(x,y,33,33,color);
}
