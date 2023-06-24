#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

#if defined(ARDUINO_FEATHER_ESP32) // Feather Huzzah32
  #define TFT_CS         14
  #define TFT_RST        15
  #define TFT_DC         32

#elif defined(ESP8266)
  #define TFT_CS         4
  #define TFT_RST        16                                            
  #define TFT_DC         5

#else
  // For the breakout board, you can use any 2 or 3 pins.
  // These pins will also work for the 1.8" TFT shield.
  #define TFT_CS         7
  #define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
  #define TFT_DC         8
#endif

// OPTION 1 (recommended) is to use the HARDWARE SPI pins, which are unique
// to each board and not reassignable. For Arduino Uno: MOSI = pin 11 and
// SCLK = pin 13. This is the fastest mode of operation and is required if
// using the breakout board's microSD card.

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// array of PID's to print values of
String INIT = "0100";
String COOLANT_TEMPERATURE = "0105";
String OIL_TEMPERATURE = "015C";
String FUEL = "012F";

String NewLine = "\r\n";

int ledPin = 5;
int state = 2;
int Delay = 100;

int Water = 80;
int Oil = 90;
int Diesel = 100;

const int MAX_BYTES = 43;
byte bytes[MAX_BYTES] = {0};
int integers[MAX_BYTES] = {0};

void setup() {

  analogWrite(ledPin, 255);
  
  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab

  // OR use this initializer if using a 1.8" TFT screen with offset such as WaveShare:
  // tft.initR(INITR_GREENTAB);      // Init ST7735S chip, green tab

  tft.fillScreen(ST7735_BLACK);
  
  tft.setRotation(1);
  
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.setTextSize(1);
  tft.print(F("Waiting for Bluetooth connection ... "));

  delay(500);
  
  Serial.begin(38400);
  
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.setTextSize(1);
  tft.print(F("Attempting to connect to ELM327 ... "));
  Serial.print("ATZ");
  Serial.print(NewLine);

  String ELM;
  String Init;
  String Cool;

  while(true){
   if(Serial.available()){
    ELM = Serial.readString();
    break;
   }
  }
  
    Serial.print("0100");
    Serial.print(NewLine);
    
  while(true){
   if(Serial.available()){
    Init = Serial.readString();
    break;
   }
  }
  
    Serial.print(COOLANT_TEMPERATURE);
    Serial.print(NewLine);

  while(true){
   if(Serial.available()){
    Cool = Serial.readString();
    break;
   }
  }

 Serial.println("ELM: ");
 Serial.println(ELM);

 Serial.println("Init: ");
 Serial.println(Init);

 Serial.println("Cool: ");
 Serial.println(Cool);
  
}

void loop(){
  
}
