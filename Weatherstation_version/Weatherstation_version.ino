/***********************************************************************************************************
 * WEATHERSTATION V2 - WEATHERSTATION VERSION
 * Author                 : The Big Site / Jeroen Maathuis (Joennuh)
 * E-mail                 : j [dot] maathuis [at] gmail [dot] com / software [at] thebigsite [dot] nl)
 * Intitial creation date : August 8th, 2019
 * 
 * This software is part of the Weatherstation V2 project of The Big Site / Jeroen Maathuis (aka Joennuh).
 * This part is for the Weatherstation: the device you put in your living to view all data of inside and
 * outside the house on.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * This software is based on example sketches from several sources. Mostly the examples come from library's
 * being used in this sketch.
 * Sources:
 * -  TTGO T-Display: https://github.com/Xinyuan-LilyGO/TTGO-T-Display
 * -  ArduinoMenu: https://github.com/neu-rah/ArduinoMenu
 * -  TFT_eSPI: https://github.com/Bodmer/TFT_eSPI
 * -  Button2: https://github.com/LennartHennigs/Button2
 * And probably I've forgotten to mention sources. In that case: I'm sorry for that.
 * 
 * Hardware configuration:
 * - LilyGO TTGO T-Display (with ST7789V display, 2 onboard buttons and charging interface)
 * - RGB common cathode
 * - 3 Warm white leds
 * - DS18B20 temperature sensor
 * 
 * Preparations:
 * -  Do not forget to upload the font files in the data directory to the SPIFFS with the ESP32 Sketch
 *    data uploader prior to uploading the sketch!
 * -  Do not forget to include the correct User Setup file in the TFT_eSPI configuration before compiling
 *    and uploading the sketch!
 ***********************************************************************************************************/

#include "lang_nl.h" // Which language file should be included?
 
#define PROG_NAME "Weatherstation V2"
#define PROG_HARDWARE "Weatherstation"
#define PROG_VERSION "0.1.0002"
#define PROG_MANUFACTURER "The Big Site"

#include <Arduino.h>

// Font files are stored in SPIFFS, so load the linbrary
#include <FS.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <Button2.h>
#include "bmp.h"
#include "icons.h"

#include <OneWire.h>
#include <DallasTemperature.h>

#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif

#define AA_FONT_SMALL "NotoSansBold15"
#define AA_FONT_LARGE "NotoSansBold36"

#include <menu.h>
#include <menuIO/serialIO.h>
#include <menuIO/TFT_eSPIOut.h>
// #include <menuIO/chainStream.h>
#include <menuIO/esp8266Out.h>//must include this even if not doing web output...
#include <plugin/barField.h>

using namespace Menu;

TFT_eSPI gfx = TFT_eSPI(135, 240);
TFT_eSprite spr = TFT_eSprite(&gfx); // Sprite class needs to be invoked
TFT_eSprite sprIcons = TFT_eSprite(&gfx); // Sprite class needs to be invoked
TFT_eSprite sprTime = TFT_eSprite(&gfx); // Sprite class needs to be invoked

#define BTN_UP 35 
#define BTN_DWN 0

Button2 btnUp(BTN_UP);
Button2 btnDwn(BTN_DWN);

// Define pinnumbers for leds
#define LED_RED 25
#define LED_GRN 26
#define LED_BLU 27
#define LED_TR1 33 // Torch light
#define LED_TR2 32
#define LED_TR3 12
#define LED_TFT 4

// Define default values for led brightness
int RGBledRed = 0;
int RGBledGreen = 5;
int RGBledBlue = 0;
int torch = 0; // Torch light, default is off
int TFTled = 80; // Screen at full brightness

// Setting PWM properties
const int pwmFreq = 5000;
const int pwmResolution = 8;

const int pwmLedChannelRed = 0;
const int pwmLedChannelGreen = 1;
const int pwmLedChannelBlue = 2;
const int pwmLedChannelTorch = 3;
const int pwmLedChannelTFT = 4;

#define PIN_DS18B20 15

OneWire oneWire(PIN_DS18B20);  // on pin 14 (a 4.7K resistor is necessary)
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

result doAlert(eventMask e, prompt &item);

result RGBPresetShow(eventMask event, navNode& nav, prompt &item) {
  Serial.print("selected index: ");
  Serial.println(nav.sel);
  Serial.print("selected value: ");
  Serial.println(((menuValue<int>*)&item)->target());
  int RGBPresetChoice = ((menuValue<int>*)&item)->target();
  switch(RGBPresetChoice){
    case 0: // off
      RGBledRed = 0;
      RGBledGreen = 0;
      RGBledBlue = 0;
      break;
    case 1: // red
      RGBledRed = 50;
      RGBledGreen = 0;
      RGBledBlue = 0;
      break;
    case 2: // yellow
      RGBledRed = 50;
      RGBledGreen = 50;
      RGBledBlue = 0;
      break;
    case 3: // green
      RGBledRed = 0;
      RGBledGreen = 50;
      RGBledBlue = 0;
      break;
    case 4: // marine
      RGBledRed = 0;
      RGBledGreen = 50;
      RGBledBlue = 50;
      break;
    case 5: // blue
      RGBledRed = 0;
      RGBledGreen = 0;
      RGBledBlue = 50;
      break;
    case 6: // purple
      RGBledRed = 50;
      RGBledGreen = 0;
      RGBledBlue = 50;
      break;
    case 7: // white normal
      RGBledRed = 50;
      RGBledGreen = 50;
      RGBledBlue = 50;
      break;
    case 8: // white bright
      RGBledRed = 255;
      RGBledGreen = 255;
      RGBledBlue = 255;
      break;
    default: // off
      RGBledRed = 0;
      RGBledGreen = 0;
      RGBledBlue = 0;
      break;
  }
  return proceed;
}

int displayColor = 0;
result RGBPresetMenuReset(eventMask e,navNode& nav, prompt &item) {
  displayColor = 0;
  return proceed;
}

CHOOSE(displayColor,RGBPresetMenu,LANG_RGB_PRESET_MENU_TITLE,doNothing,noEvent,wrapStyle
  ,VALUE(LANG_NONE,0,RGBPresetShow,focusEvent)
  ,VALUE(LANG_RED,1,RGBPresetShow,focusEvent)
  ,VALUE(LANG_YELLOW,2,RGBPresetShow,focusEvent)
  ,VALUE(LANG_GREEN,3,RGBPresetShow,focusEvent)
  ,VALUE(LANG_MARINE,4,RGBPresetShow,focusEvent)
  ,VALUE(LANG_BLUE,5,RGBPresetShow,focusEvent)
  ,VALUE(LANG_PURPLE,6,RGBPresetShow,focusEvent)
  ,VALUE(LANG_WHITE_NORMAL,7,RGBPresetShow,focusEvent)
  ,VALUE(LANG_WHITE_BRIGHT,8,RGBPresetShow,focusEvent)
);

MENU(RGBledMenu,LANG_RGB_MENU_TITLE,doNothing,anyEvent,wrapStyle
   //,FIELD(RGBledRed,"Red        : ","",0,255,10,5,doNothing,noEvent,wrapStyle)
   ,BARFIELD(RGBledRed,LANG_RGB_RED,"",0,255,10,1,RGBPresetMenuReset,enterEvent,wrapStyle)//numeric field with a bar
   //,FIELD(RGBledGreen,"Green      : ","",0,255,10,5,doNothing,noEvent,wrapStyle)
  ,BARFIELD(RGBledGreen,LANG_RGB_GREEN,"",0,255,10,1,RGBPresetMenuReset,enterEvent,wrapStyle)//numeric field with a bar
  //,FIELD(RGBledBlue,"Blue       : ","",0,255,10,5,doNothing,noEvent,wrapStyle)
  ,BARFIELD(RGBledBlue,LANG_RGB_BLUE,"",0,255,10,1,RGBPresetMenuReset,enterEvent,wrapStyle)//numeric field with a bar
  ,SUBMENU(RGBPresetMenu)
  ,EXIT(LANG_BACK)
);

int test=55;

int ledCtrl=HIGH;

result myLedOn() {
  ledCtrl=HIGH;
  return proceed;
}
result myLedOff() {
  ledCtrl=LOW;
  return proceed;
}

TOGGLE(ledCtrl,setLed,"Demo led: ",doNothing,noEvent,noStyle//,doExit,enterEvent,noStyle
  ,VALUE("On",HIGH,doNothing,noEvent)
  ,VALUE("Off",LOW,doNothing,noEvent)
);

int selTest=0;
SELECT(selTest,selMenu,"Select",doNothing,noEvent,noStyle
  ,VALUE("Zero",0,doNothing,noEvent)
  ,VALUE("One",1,doNothing,noEvent)
  ,VALUE("Two",2,doNothing,noEvent)
);

int chooseTest=-1;
CHOOSE(chooseTest,chooseMenu,"Choose",doNothing,noEvent,noStyle
  ,VALUE("First",1,doNothing,noEvent)
  ,VALUE("Second",2,doNothing,noEvent)
  ,VALUE("Third",3,doNothing,noEvent)
  ,VALUE("Last",-1,doNothing,noEvent)
);

//customizing a prompt look!
//by extending the prompt class
class altPrompt:public prompt {
public:
  altPrompt(constMEM promptShadow& p):prompt(p) {}
  Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t) override {
    return out.printRaw(F("special prompt!"),len);;
  }
};

MENU(subMenu,"Sub-Menu",doNothing,noEvent,noStyle
  ,altOP(altPrompt,"",doNothing,noEvent)
  ,OP("Op",doNothing,noEvent)
  ,EXIT(LANG_BACK)
);

char* constMEM hexDigit MEMMODE="0123456789ABCDEF";
char* constMEM hexNr[] MEMMODE={"0","x",hexDigit,hexDigit};
char buf1[]="0x11";

MENU(mainMenu,LANG_MAIN_MENU_TITLE,doNothing,noEvent,wrapStyle
  ,OP("Op1",doNothing,noEvent)
  ,OP("Op2",doNothing,noEvent)
  // ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent,wrapStyle)
  ,SUBMENU(subMenu)
  ,SUBMENU(RGBledMenu)
  ,BARFIELD(torch,LANG_TORCH,"",0,255,10,1,RGBPresetMenuReset,enterEvent,wrapStyle)//numeric field with a bar
  ,BARFIELD(TFTled,LANG_BACKLIGHT,"",0,255,10,1,RGBPresetMenuReset,enterEvent,wrapStyle)//numeric field with a bar
  ,SUBMENU(setLed)
  ,OP("Demo led On",myLedOn,enterEvent)
  ,OP("Demo led Off",myLedOff,enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  //,OP("Alert test",doAlert,enterEvent)
  ,EDIT("Hex",buf1,hexNr,doNothing,noEvent,noStyle)
  ,EXIT(LANG_BACK)
);

// define menu colors --------------------------------------------------------
//  {{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
//monochromatic color table


#define Black RGB565(0,0,0)
#define Red	RGB565(255,0,0)
#define Green RGB565(0,255,0)
#define Blue RGB565(0,0,255)
#define Gray RGB565(128,128,128)
#define LighterRed RGB565(255,150,150)
#define LighterGreen RGB565(150,255,150)
#define LighterBlue RGB565(150,150,255)
#define DarkerRed RGB565(150,0,0)
#define DarkerGreen RGB565(0,150,0)
#define DarkerBlue RGB565(0,0,150)
#define Cyan RGB565(0,255,255)
#define Magenta RGB565(255,0,255)
#define Yellow RGB565(255,255,0)
#define White RGB565(255,255,255)

const colorDef<uint16_t> colors[] MEMMODE={
  {
    {
      (uint16_t)Black,
      (uint16_t)Black
    },
    {
      (uint16_t)Black,
      (uint16_t)DarkerBlue,
      (uint16_t)DarkerBlue
    }
  },//bgColor
  {
    {
      (uint16_t)Gray,
      (uint16_t)Gray
    },
    {
      (uint16_t)White,
      (uint16_t)White,
      (uint16_t)White
    }
  },//fgColor
  {
    {
      (uint16_t)White,
      (uint16_t)Black
    },
    {
      (uint16_t)Yellow,
      (uint16_t)Yellow,
      (uint16_t)Red
    }
  },//valColor
  {
    {
      (uint16_t)White,
      (uint16_t)Black
    },
    {
      (uint16_t)White,
      (uint16_t)Yellow,
      (uint16_t)Yellow
    }
  },//unitColor
  {
    {
      (uint16_t)White,
      (uint16_t)Gray
    },
    {
      (uint16_t)Black,
      (uint16_t)Blue,
      (uint16_t)White
    }
  },//cursorColor
  {
    {
      (uint16_t)White,
      (uint16_t)Yellow
    },
    {
      (uint16_t)DarkerRed,
      (uint16_t)White,
      (uint16_t)White
    }
  },//titleColor
};

#define MAX_DEPTH 5

serialIn serial(Serial);

//MENU_INPUTS(in,&serial);its single, no need to `chainStream`

//define serial output device
idx_t serialTops[MAX_DEPTH]={0};
serialOut outSerial(Serial,serialTops);

#define GFX_WIDTH 240
#define GFX_HEIGHT 135
#define fontW 12
#define fontH 18

constMEM panel panels[] MEMMODE = {{0, 0, GFX_WIDTH / fontW, GFX_HEIGHT / fontH}};
navNode* nodes[sizeof(panels) / sizeof(panel)]; //navNodes to store navigation status
panelsList pList(panels, nodes, 1); //a list of panels and nodes
idx_t eSpiTops[MAX_DEPTH]={0};
TFT_eSPIOut eSpiOut(gfx,colors,eSpiTops,pList,fontW,fontH+1);
menuOut* constMEM outputs[] MEMMODE={&outSerial,&eSpiOut};//list of output devices
outputsList out(outputs,sizeof(outputs)/sizeof(menuOut*));//outputs list controller

NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);

unsigned long idleTimestamp = millis();

//when menu is suspended
result idle(menuOut& o,idleEvent e) {
//  int oldRGBledRed = RGBledRed;
//  int oldRGBledBlue = RGBledBlue;
//  int oldRGBledGreen = RGBledGreen;
  //Serial.println("IDLE: Before e==idling");
  if(e==idleStart){
    gfx.fillScreen(TFT_BLACK);
    gfx.pushImage(0, 0,  240, 22, titleBar);
  }
  else if (e==idling) {
    //Serial.println("IDLE: e==idling, before timestamp check");
    if(millis()-idleTimestamp > 1000){
//      RGBledRed = 10;
//      RGBledBlue = 0;
//      RGBledGreen = 0;
      //Serial.println("IDLE: e==idling, within timestamp check");
  
      int xpos = gfx.width() / 2; // Half the screen width
      int ypos = gfx.height() / 2;

      // Icons upper right corner
      sprIcons.createSprite(70, 22);
      spr.fillSprite(TFT_BLACK);
      sprIcons.drawXBitmap(0, 2, iconLedOutline, 16, 18, TFT_WHITE);
      if(torch>0){
        sprIcons.drawXBitmap(0, 2, iconLedFill, 16, 18, TFT_YELLOW);
      }
      else{
        sprIcons.drawXBitmap(0, 2, iconLedFill, 16, 18, TFT_WHITE);
      }
      gfx.setSwapBytes(false);
      sprIcons.pushSprite(170,0,TFT_BLACK);
      gfx.setSwapBytes(true);
      sprIcons.deleteSprite();

      // Time upper left corner
      sprTime.createSprite(50,22);
      sprTime.fillSprite(TFT_BLACK);
      sprTime.setTextColor(TFT_WHITE,TFT_BLACK); // Set the sprite font colour and the background colour
      sprTime.setTextDatum(TL_DATUM); // Top Left datum
      sprTime.drawString(String("16:51"), 3, 5); // Coords of middle of 150 x 50 Sprite
      gfx.setSwapBytes(false);
      sprTime.pushSprite(0, 0, TFT_BLACK); // Push to TFT screen coord 10, 10. Blue is transparent
      gfx.setSwapBytes(true);
      sprTime.unloadFont(); // Remove the font from sprite class to recover memory used
      sprTime.deleteSprite();
      
      displayShowTemp(88,24);
  
      //gfx.println("This is the idle function");
      idleTimestamp = millis();
    }
    //Serial.println("IDLE: e==idling, after timestamp check");
  }
  //Serial.println("IDLE: after e==idling check");
//  RGBledRed = oldRGBledRed;
//  RGBledBlue = oldRGBledBlue;
//  RGBledGreen = oldRGBledGreen;
  nav.idleChanged=true; // Keep on calling the idle function. For documentation see: https://github.com/neu-rah/ArduinoMenu/wiki/Idling
  //Serial.println("IDLE: right before return proceed");
  return proceed;
}

//config myOptions('*','-',defaultNavCodes,false);

inline bool checkIdling() {return nav.sleepTask;}

int oldTorch = 0;

void toggleTorch(){
  //Serial.print("oldTorch = ");
  //Serial.print(oldTorch);
  //Serial.print("torch = ");
  //Serial.println(torch);
  if(torch>0 && torch<255){ // Torch was dimmed, set to full on
    //Serial.println("torch>0 && torch<255");
    oldTorch = torch;
    torch = 255;    
  }
  else if(torch==255){ // Torch was full on
    //Serial.println("torch>0 && torch<255");
    if(oldTorch>0 && oldTorch<255){ // Before full on, torch was dimmed, restore to dimmed
      //Serial.println("oldTorch>0 && oldTorch<255");
      torch=oldTorch;
      oldTorch=0;
    }
    else if(oldTorch==0){ // Before full on torch was full off, restore full off
      //Serial.println("oldTorch==0");
      torch=0;
      oldTorch=0;
    } 
  }
  else if(torch==0){ // Torch was full off, set to full on
    //Serial.println("torch==0");
    torch = 255;
    oldTorch = 0;
  }
}

void button_init()
{
    // inline bool idling() {return nav.sleepTask;} could be a way to have a function that checkes whether ArduinoMenu is in the idle state or not.
    // Could be used to navigate in wheater cards in the idle state instead of navigating in the ArduinoMenu.
    // See Gitter chat ArduinoMenu/Lobby 2019-08-22 01:48 GMT+2
  
    btnUp.setLongClickHandler([](Button2 & b) {
        // Select
        unsigned int time = b.wasPressedFor();
        if (time >= 1000) {
          if(checkIdling()==false)
          {
            nav.doNav(enterCmd);
          }
          else
          {
            toggleTorch();
          }          
        }
    });
    
    btnUp.setClickHandler([](Button2 & b) {
       // Up
       nav.doNav(downCmd);
    });

    btnDwn.setLongClickHandler([](Button2 & b) {
        // Exit
        unsigned int time = b.wasPressedFor();
        if (time >= 1000) {
          nav.doNav(escCmd);
        }
    });
    
    btnDwn.setClickHandler([](Button2 & b) {
        // Down
        nav.doNav(upCmd);
    });
}

void button_loop()
{
    btnUp.loop();
    btnDwn.loop();
}

void setup() {
   // -- TFT backlight
  //Serial.print("Configuring PWM for TFT backlight... ");
  ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
  ledcAttachPin(LED_TFT, pwmLedChannelTFT);
  //Serial.println("DONE");

  //Serial.print("Setting PWM for TFT backlight to intensity 255... ");
  ledcWrite(pwmLedChannelTFT, 255);
  //Serial.println("DONE");

  //SPI.begin(); // Disabled for the LilyGo TTGO T-display: with this line enabled nothing gets displayed on the ST7789 screen
  gfx.init();

  gfx.setRotation(1);
  
  spr.setColorDepth(16); // 16 bit colour needed to show antialiased fonts
  
  gfx.setTextWrap(false);
  gfx.fillScreen(White);
  //gfx.setTextColor(Red,Black);
  //Serial.println("Menu 4.x test on GFX");
  //gfx.println("Menu 4.x test on GFX");

  delay(250);
  
  //Serial.print("Showing bootlogo... ");
  gfx.setSwapBytes(true);
  gfx.pushImage(0, 0,  240, 135, bootlogo);
  delay(500);
  //Serial.println("DONE");

  gfx.drawXBitmap(219, 112, iconSerial, 16, 18, Black, White);
  
  Serial.begin(115200);
  while(!Serial);
  Serial.flush();
  Serial.println();
  Serial.print(PROG_MANUFACTURER);
  Serial.print(" ");
  Serial.println(PROG_NAME);
  Serial.print("SW ");
  Serial.println(PROG_VERSION);
  Serial.println("BOOT OK");

  delay(2000);

  gfx.drawXBitmap(219, 112, iconDisk, 16, 18, Black, White);

  Serial.println("SPIFFS initialisation...");
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    gfx.drawXBitmap(202, 112, iconWarning, 16, 18, Red, White);
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nSPIFFS available!");

  listFiles(); // Lists the files so you can see what is in the SPIFFS

  Serial.println("Checking font availability...");
  // ESP32 will crash if any of the fonts are missing
  bool font_missing = false;
  if (SPIFFS.exists("/NotoSansBold15.vlw")    == false) font_missing = true;
  if (SPIFFS.exists("/NotoSansBold36.vlw")    == false) font_missing = true;
  if (SPIFFS.exists("/Final-Frontier-28.vlw")    == false) font_missing = true;
  if (SPIFFS.exists("/NotoSansMonoSCB20.vlw")    == false) font_missing = true;
  if (SPIFFS.exists("/OpenSans36.vlw")    == false) font_missing = true;

  if (font_missing)
  {
    Serial.println("\r\nOne or more font(s) is/are missing in SPIFFS, did you upload it?");
    gfx.drawXBitmap(202, 112, iconWarning, 16, 18, Red, White);
    while(1) yield();
  }
  else Serial.println("\r\nFonts found OK.");

  delay(1000);

  Serial.print("Printing software version... ");
  gfx.setCursor(235,50);
  
  gfx.loadFont(AA_FONT_SMALL);
  
  gfx.setTextColor(Black,White);
  
  gfx.setTextDatum(TR_DATUM);
  //char swVersion[] = "SW ";
  //swVersion += PROG_VERSION;
  gfx.drawString(String("SW ") + String(PROG_VERSION), 235, 50);

  gfx.unloadFont();
  
  gfx.setTextSize(2);
  
  Serial.println("DONE");

  delay(500);
  
  Serial.print("Initialize buttons... ");
  gfx.drawXBitmap(219, 112, iconButton, 16, 18, Black, White);
  button_init();
  delay(1000);
  Serial.println("DONE");

  gfx.drawXBitmap(219, 112, iconLedOutline, 16, 18, Black, White);

  // -- Red
  Serial.print("Configuring PWM for red led... ");
  ledcSetup(pwmLedChannelRed, pwmFreq, pwmResolution);
  ledcAttachPin(LED_RED, pwmLedChannelRed);
  Serial.println("DONE");

  Serial.print("Testing PWM for red led on intensity 100... ");
  gfx.drawXBitmap(219, 112, iconLedFill, 16, 18, Red);
  ledcWrite(pwmLedChannelRed, 100);
  Serial.println("DONE");

  delay(1000);

  Serial.print("Turning red led off... ");
  ledcWrite(pwmLedChannelRed, 0);
  gfx.drawXBitmap(219, 112, iconLedFill, 16, 18, White);
  Serial.println("DONE");

  delay(500);

  // -- Green
  Serial.print("Configuring PWM for green led... ");
  ledcSetup(pwmLedChannelGreen, pwmFreq, pwmResolution);
  ledcAttachPin(LED_GRN, pwmLedChannelGreen);
  Serial.println("DONE");

  Serial.print("Testing PWM for green led on intensity 100... ");
  gfx.drawXBitmap(219, 112, iconLedFill, 16, 18, Green);
  ledcWrite(pwmLedChannelGreen, 100);
  Serial.println("DONE");

  delay(1000);

  Serial.print("Turning green led off... ");
  gfx.drawXBitmap(219, 112, iconLedFill, 16, 18, White);
  ledcWrite(pwmLedChannelGreen, 0);
  Serial.println("DONE");

  delay(500);

  // -- Blue
  Serial.print("Configuring PWM for blue led... ");
  ledcSetup(pwmLedChannelBlue, pwmFreq, pwmResolution);
  ledcAttachPin(LED_BLU, pwmLedChannelBlue);
  Serial.println("DONE");

  Serial.print("Testing PWM for blue led on intensity 100... ");
  gfx.drawXBitmap(219, 112, iconLedFill, 16, 18, Blue);
  ledcWrite(pwmLedChannelBlue, 100);
  Serial.println("DONE");

  delay(1000);

  Serial.print("Turning blue led off... ");
  gfx.drawXBitmap(219, 112, iconLedFill, 16, 18, White);
  ledcWrite(pwmLedChannelBlue, 0);
  Serial.println("DONE");

  delay(500);

   // -- Torch
  Serial.print("Configuring PWM for torch leds... ");
  ledcSetup(pwmLedChannelTorch, pwmFreq, pwmResolution);
  ledcAttachPin(LED_TR1, pwmLedChannelTorch);
  ledcAttachPin(LED_TR2, pwmLedChannelTorch);
  ledcAttachPin(LED_TR3, pwmLedChannelTorch);
  Serial.println("DONE");

  Serial.print("Testing PWM for torch led on intensity 100... ");
  gfx.drawXBitmap(219, 112, iconLedFill, 16, 18, Yellow);
  ledcWrite(pwmLedChannelTorch, 100);
  Serial.println("DONE");

  delay(1000);

  Serial.print("Turning torch led off... ");
  gfx.drawXBitmap(219, 112, iconLedFill, 16, 18, White);
  ledcWrite(pwmLedChannelTorch, 0);
  Serial.println("DONE");

  delay(500);
  
  //setting some plugins otions
  barFieldOptions::fill="*";

  delay(500);
  
  //setting some plugins otions
  barFieldOptions::fill="*";
  
  Serial.println("- READY -");
  
  gfx.fillScreen(Black);

  delay(250);
  nav.idleTask=idle;//point a function to be used when menu is suspended
  mainMenu[1].disable();
  nav.idleOn(idle); // Directly enable idle screen with wheater cards
}

void loop() {
  button_loop();
  nav.poll();//this device only draws when needed

//    Serial.print("R: ");
//    Serial.print(RGBledRed);
//    Serial.print(", G: ");
//    Serial.print(RGBledGreen);
//    Serial.print(", B: ");
//    Serial.print(RGBledBlue);
      ledcWrite(pwmLedChannelRed, RGBledRed);
      ledcWrite(pwmLedChannelGreen, RGBledGreen);
      ledcWrite(pwmLedChannelBlue, RGBledBlue);
      ledcWrite(pwmLedChannelTorch, torch);
      ledcWrite(pwmLedChannelTFT, TFTled);
//    neopixelTimestamp=millis();
}

void displayShowTemp(int xpos, int ypos){
  float temperature;
  gfx.setTextDatum(TC_DATUM); // Top Centre datum
  // >>> Based on http://domoticx.com/arduino-temperatuur-sensor-ds18b20/
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures... ");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  
  //Serial.print("Temperature for the device 1 (index 0) is: ");
  //Serial.println(sensors.getTempCByIndex(0));

  temperature  = sensors.getTempCByIndex(0);
  Serial.println("---------------------------");
  Serial.print("temperature = ");
  Serial.println(temperature);
  if(temperature == DEVICE_DISCONNECTED_C){
    Serial.println(">> DEVICE DISCONNECTED! <<");

    spr.loadFont("OpenSans36");
    spr.createSprite(150, 50);
    spr.fillSprite(TFT_BLACK);
    //spr.drawRect(0, 0, 150, 50, TFT_WHITE);
    spr.setTextColor(TFT_WHITE, TFT_BLACK); // Set the sprite font colour and the background colour
    spr.setTextDatum(MC_DATUM); // Middle Centre datum
    spr.drawString(String("--,-") + String("\xB0") + String("C"), 75, 25 ); // Coords of middle of 150 x 50 Sprite
    gfx.setSwapBytes(false);
    spr.pushSprite(xpos, ypos, TFT_BLACK); // Push to TFT screen coord 10, 10. Blue is transparent
    gfx.setSwapBytes(true);
    spr.unloadFont(); // Remove the font from sprite class to recover memory used
    spr.deleteSprite();
    
  }

  else{
    // Arduino doesn't have rounding to 1 decimal. Workaround to convert 2 decimals to 1 decimal.
    // Partly based on https://www.reddit.com/r/arduino/comments/242c5o/how_to_round_to_nearest_tenth/
    // >>> AND STILL DOESN'T WORK! IT STILL PROVIDES 2 DECIMALS! <<<
    // temperature = 23.81
    int newtemperature1 = round(temperature * 10); // newtemperature1 =  238
    Serial.print("newtemperature1 = ");
    Serial.println(newtemperature1);
    
    float newtemperature2 = newtemperature1 / 10.0; 
    Serial.print("newtemperature2 = ");
    Serial.println(newtemperature2);

    char printtemperature[10];
    
    snprintf(printtemperature, 10, "%.1f", newtemperature2); // Documentation at http://www.cplusplus.com/reference/cstdio/snprintf/ & http://www.cplusplus.com/reference/cstdio/printf/
    String printtemperature2 = (String) printtemperature;
    printtemperature2.replace(".",",");
    Serial.print("printtemperature = ");
    Serial.println(printtemperature);
    Serial.print("printtemperature2 = ");
    Serial.println(printtemperature2);
  
    spr.loadFont("OpenSans36");
    spr.createSprite(150, 50);
    spr.fillSprite(TFT_BLACK);
    //spr.drawRect(0, 0, 150, 50, TFT_WHITE);
    spr.setTextColor(TFT_WHITE, TFT_BLACK); // Set the sprite font colour and the background colour
    spr.setTextDatum(MC_DATUM); // Middle Centre datum
    spr.drawString(String(printtemperature2) + String("\xB0") + String("C"), 75, 25 ); // Coords of middle of 150 x 50 Sprite
    gfx.setSwapBytes(false);
    spr.pushSprite(xpos, ypos); // Push to TFT screen coord 10, 10. Blue is transparent
    gfx.setSwapBytes(true);
    spr.unloadFont(); // Remove the font from sprite class to recover memory used
    spr.deleteSprite();
  }
}

  
