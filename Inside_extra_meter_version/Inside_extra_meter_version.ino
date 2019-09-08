/***********************************************************************************************************
 * WEATHERSTATION V2 - INSIDE EXTRA METER VERSION
 * Author                 : The Big Site / Jeroen Maathuis (Joennuh)
 * E-mail                 : j [dot] maathuis [at] gmail [dot] com / software [at] thebigsite [dot] nl)
 * Intitial creation date : August 8th, 2019
 * 
 * This software is part of the Weatherstation V2 project of The Big Site / Jeroen Maathuis (aka Joennuh).
 * This part is for the Inside extra meter: the device you can put everywhere inside your house with an
 * wire and waterproof DS18B20 temperature sensor on it so that you can let it measure the temperature
 * of outside the house.
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
 * -  ArduinoMenu: https://github.com/neu-rah/ArduinoMenu
 * -  TFT_eSPI: https://github.com/Bodmer/TFT_eSPI
 * -  Button2: https://github.com/LennartHennigs/Button2
 * And probably I've forgotten to mention sources. In that case: I'm sorry or that.
 * 
 * Hardware configuration:
 * - Wemos D1 mini
 * - ST7738S display
 * - DS18B20 temperature sensor
 * - 2 tactile pushbuttons
 * 
 * Preparations:
 * -  Do not forget to upload the font files in the data directory to the SPIFFS with the ESP8266 Sketch
 *    data uploader prior to uploading the sketch!
 * -  Do not forget to include the correct User Setup file in the TFT_eSPI configuration before compiling
 *    and uploading the sketch!
 ***********************************************************************************************************/
 
#define PROG_NAME "Weatherstation V2"
#define PROG_HARDWARE "Inside extra meter"
#define PROG_VERSION "0.1.0000"
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

TFT_eSPI gfx = TFT_eSPI(80, 160);
TFT_eSprite spr = TFT_eSprite(&gfx); // Sprite class needs to be invoked
TFT_eSprite sprIcons = TFT_eSprite(&gfx); // Sprite class needs to be invoked
TFT_eSprite sprTime = TFT_eSprite(&gfx); // Sprite class needs to be invoked

#define BTN_UP D2
#define BTN_DWN D3

Button2 btnUp(BTN_UP);
Button2 btnDwn(BTN_DWN, INPUT_PULLUP);

// Define pinnumbers for leds
#define LED_TFT D6

int TFTled = 150; // Screen at full brightness

#define PIN_DS18B20 D1

OneWire oneWire(PIN_DS18B20);  // on pin 14 (a 4.7K resistor is necessary)
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

result doAlert(eventMask e, prompt &item);

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
    //return out.printRaw(F("special prompt!"),len);;
    return out.print(F("special prompt!"));
  }
};

MENU(systemMenu,"System",doNothing,anyEvent,wrapStyle
  ,OP("WiFi settings",doNothing,noEvent)
  ,OP("Authorization code",doNothing,noEvent)
  ,EXIT("<Back")
);

MENU(subMenu,"Sub-Menu",doNothing,noEvent,noStyle
  ,altOP(altPrompt,"",doNothing,noEvent)
  ,OP("Op",doNothing,noEvent)
  ,EXIT("<Back")
);

char* constMEM hexDigit MEMMODE="0123456789ABCDEF";
char* constMEM hexNr[] MEMMODE={"0","x",hexDigit,hexDigit};
char buf1[]="0x11";

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",doNothing,noEvent)
  ,OP("Op2",doNothing,noEvent)
  // ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent,wrapStyle)
  ,SUBMENU(subMenu)
  ,SUBMENU(systemMenu)
  ,BARFIELD(TFTled,"Backlight: ","",0,255,10,1,doNothing,enterEvent,wrapStyle)//numeric field with a bar
  //,SUBMENU(setLed)
  //,OP("Demo led On",myLedOn,enterEvent)
  //,OP("Demo led Off",myLedOff,enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  //,OP("Alert test",doAlert,enterEvent)
  ,EDIT("Hex",buf1,hexNr,doNothing,noEvent,noStyle)
  ,EXIT("<Back")
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

#define MAX_DEPTH 3

serialIn serial(Serial);

//MENU_INPUTS(in,&serial);its single, no need to `chainStream`

//define serial output device
idx_t serialTops[MAX_DEPTH]={0};
serialOut outSerial(Serial,serialTops);

#define GFX_WIDTH 160
#define GFX_HEIGHT 80
#define fontW 6
#define fontH 9

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
  if(e==idleStart){
    gfx.fillScreen(TFT_BLACK);
    //gfx.pushImage(0, 0,  240, 22, titleBar);
  }
  else if (e==idling) {
    //Serial.println("IDLE: e==idling, before timestamp check");
    if(millis()-idleTimestamp > 1000){
      //Serial.println("IDLE: e==idling, within timestamp check");
  
      int xpos = gfx.width() / 2; // Half the screen width
      int ypos = gfx.height() / 2;

      // Time upper left corner
      sprTime.createSprite(30,11);
      sprTime.fillSprite(TFT_BLACK);
      sprTime.setTextColor(TFT_WHITE,TFT_BLACK); // Set the sprite font colour and the background colour
      sprTime.setTextDatum(TL_DATUM); // Top Left datum
      sprTime.drawString(String("16:51"), 2, 2);
      gfx.setSwapBytes(false);
      sprTime.pushSprite(0, 0, TFT_BLACK); // Push to TFT screen coord 10, 10. Blue is transparent
      gfx.setSwapBytes(true);
      sprTime.unloadFont(); // Remove the font from sprite class to recover memory used
      sprTime.deleteSprite();
      
      displayShowTemp(30,20);
  
      //gfx.println("This is the idle function");
      idleTimestamp = millis();
    }
    //Serial.println("IDLE: e==idling, after timestamp check");
  }
  nav.idleChanged=true; // Keep on calling the idle function. For documentation see: https://github.com/neu-rah/ArduinoMenu/wiki/Idling
  //Serial.println("IDLE: right before return proceed");
  return proceed;
}

//config myOptions('*','-',defaultNavCodes,false);

inline bool checkIdling() {return nav.sleepTask;}

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
   pinMode(LED_TFT,OUTPUT);
   delay(50);
   analogWrite(LED_TFT, TFTled);

  //SPI.begin(); // Disabled for the LilyGo TTGO T-display: with this line enabled nothing gets displayed on the ST7789 screen
  gfx.init();

  gfx.setRotation(3);
  
  spr.setColorDepth(16); // 16 bit colour needed to show antialiased fonts
  
  gfx.setTextWrap(false);
  gfx.fillScreen(White);
  //gfx.setTextColor(Red,Black);
  //Serial.println("Menu 4.x test on GFX");
  //gfx.println("Menu 4.x test on GFX");

  delay(250);
  
  //Serial.print("Showing bootlogo... ");
  gfx.setSwapBytes(true);
  gfx.pushImage(0, 0,  160, 80, bootlogo);
  delay(500);
  //Serial.println("DONE");

  //Trying again to adjust backlight intensity...
  analogWrite(LED_TFT, TFTled);
  delay(100);

  gfx.drawXBitmap(139, 57, iconSerial, 16, 18, Black, White);
  
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

  gfx.drawXBitmap(139, 57, iconDisk, 16, 18, Black, White);

  Serial.println("SPIFFS initialisation...");
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    gfx.drawXBitmap(122, 57, iconWarning, 16, 18, Red, White);
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
    gfx.drawXBitmap(122, 57, iconWarning, 16, 18, Red, White);
    while(1) yield();
  }
  else Serial.println("\r\nFonts found OK.");

  delay(1000);

  Serial.print("Printing software version... ");
  gfx.setTextSize(1);
  gfx.setCursor(60,35);
  
  gfx.setTextColor(Black,White);
  
  gfx.setTextDatum(TR_DATUM);
  //char swVersion[] = "SW ";
  //swVersion += PROG_VERSION;
  gfx.drawString(String("SW ") + String(PROG_VERSION), 155, 35);
  //gfx.setTextSize(2);
  Serial.println("DONE");

  delay(500);
  
  Serial.print("Initialize buttons... ");
  gfx.drawXBitmap(139, 57, iconButton, 16, 18, Black, White);
  button_init();
  delay(1000);
  Serial.println("DONE");

  gfx.drawXBitmap(139, 57, iconLedOutline, 16, 18, Black, White);

  delay(1000);
  
  //setting some plugins options
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
  
  analogWrite(LED_TFT, TFTled);
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
    spr.createSprite(130, 40);
    spr.fillSprite(TFT_BLACK);
    //spr.drawRect(0, 0, 150, 50, TFT_WHITE);
    spr.setTextColor(TFT_WHITE, TFT_BLACK); // Set the sprite font colour and the background colour
    spr.setTextDatum(MC_DATUM); // Middle Centre datum
    spr.drawString(String(printtemperature2) + String("\xB0") + String("C"), 65, 20 ); // Coords of middle of 130 x 40 Sprite
    gfx.setSwapBytes(false);
    spr.pushSprite(xpos, ypos); // Push to TFT screen coord 10, 10. Blue is transparent
    gfx.setSwapBytes(true);
    spr.unloadFont(); // Remove the font from sprite class to recover memory used
    spr.deleteSprite();
  }
}
