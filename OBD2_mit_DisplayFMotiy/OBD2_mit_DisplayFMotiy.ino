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

#include <SoftwareSerial.h>

// OPTION 1 (recommended) is to use the HARDWARE SPI pins, which are unique
// to each board and not reassignable. For Arduino Uno: MOSI = pin 11 and
// SCLK = pin 13. This is the fastest mode of operation and is required if
// using the breakout board's microSD card.

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// array of PID's to print values of
String INIT = "0100";
String COOLANT_TEMPERATURE = "0105";
String OIL_TEMPERATURE = "015C";
//String FUEL = "012F";

String NewLine = "\r\n";

int ledPin = 5;
int state = 2;
int Delay = 100;

int Water;
int Oil;

int HIGHWATER = 80;
int HIGHOIL = 80;

const int MAX_BYTES = 43;
byte bytes[MAX_BYTES] = {0};
int integers[MAX_BYTES] = {0};

bool redW = false;
bool redO = false;

SoftwareSerial mySerial(11, 12); // RX, TX



void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(state, INPUT);

  analogWrite(ledPin, 255);
  
  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab

  tft.fillScreen(ST7735_BLACK);
  
  tft.setRotation(1);
  
  Serial.begin(38400);
  mySerial.begin(38400);

  Serial.println("START");
  
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.setTextSize(1);
  tft.print(F("Waiting for Bluetooth connection ... "));

  while(true){
    if(digitalRead(state) == HIGH) break;
  }

  delay(500);
  

  
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.setTextSize(1);
  tft.print(F("Attempting to connect to ELM327 ... "));
  mySerial.print("ATZ");
  mySerial.print(NewLine);

  String ELM;

  while (true) {
    if (mySerial.available()){
      ELM = mySerial.readString();
      ELM.toLowerCase();
      if (ELM.indexOf("elm") >= 0){
        Serial.print(ELM);
        tft.print(F("success!"));
        break;
      }
    }
  }

  delay(200);
  
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.setTextSize(1);
  tft.print(F("Attempting to connect to OBD2 CAN bus ... "));

  String SINIT;
  
  while (true) {
    mySerial.print(INIT);
    mySerial.print(NewLine);
    if (mySerial.available()){
      SINIT = mySerial.readString();
      SINIT.toLowerCase();
      Serial.println(SINIT);
      if (SINIT.indexOf("41 00") >= 0){
        tft.print(F("success!"));
        break;
      }
      else if (SINIT.indexOf("searching") >= 0){
        delay(1000);
      }
    }
  }
  
  tft.fillScreen(ST7735_BLACK);
}

void loop() {
  ReadData(COOLANT_TEMPERATURE);
  Water = integers[2] - 40;
  
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setTextSize(8);
  tft.setTextWrap(false);
  tft.setCursor(0, 5);

  if (Water < HIGHWATER && !redW){
    redW = true;
    tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
  }
  else {
    redW = false;
  }
  
  if (Water < 10) {
    String Wat = String(Water);
    tft.print("  " + Wat);
  }
  else if (Water < 100) {
    String Wat = String(Water);
    tft.print(" " + Wat);
  }
  else tft.print(Water);
  

  if (Oil < HIGHOIL && !redO){
    redO = true;
    tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
  }
  else {
    redO = false;
  }
  
  ReadData(OIL_TEMPERATURE);
  Oil = integers[2] - 40;
  
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setCursor(0, 66);
  
  if (Oil < 10) {
    String Oi = String(Oil);
    tft.print("  " + Oi);
  }
  else if (Oil < 100) {
    String Oi = String(Oil);
    tft.print(" " + Oi);
  }
  else tft.print(Oil);
}

void ReadData(String PID){
  mySerial.print(PID);
  mySerial.print(NewLine);
  byte b;
  int bread = 1;

  String Data;
  String Start = PID + "\r41";
  bool Error = false;

  memset(bytes, 0, sizeof(bytes));
  memset(integers, 0, sizeof(integers));
  
  while(true){
    if(mySerial.available()){
      Data = mySerial.readString();
      Serial.println(Data);
      if (Data.startsWith(Start)){
        break;
      }
      else {
        Error = true;
        break;
      }
    }
  }

  if(!Error) parseString(Data);
  else integers[2] = 333;
}

void parseString(String input) {
  int index = 0;
  int previousIndex = 0;
  int byteCount = 0; // Zähler für die Anzahl der Bytes
  
  // Schleife zum Extrahieren der Bytes
  while (index < input.length() && byteCount < MAX_BYTES) {
    if (input.charAt(index) == ' ' || index == input.length() - 1) {
      String byteString;
      if (index == input.length() - 1) {
        byteString = input.substring(previousIndex);
      } else {
        byteString = input.substring(previousIndex, index);
      }
      bytes[byteCount] = (byte) strtol(byteString.c_str(), NULL, 16);
      previousIndex = index + 1;
      byteCount++;
    }
    index++;
  }
  
  // Umgewandelte Bytes in Integer-Werte umwandeln
  for (int i = 0; i < byteCount; i++) {
    integers[i] = (int) bytes[i];
    //Serial.println(integers[i]);
  }
}
