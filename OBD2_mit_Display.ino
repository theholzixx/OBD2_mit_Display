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

  while(true){
    if(digitalRead(state) == HIGH) break;
  }

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

  while (true) {
    if (Serial.available()){
      ELM = Serial.readString();
      Serial.print(ELM);
      ELM = Serial.readString();
      Serial.print(ELM);
      ELM = Serial.readString();
      Serial.print(ELM);
      ELM = Serial.readString();
      Serial.print(ELM);
      //ELM = ".ATZ\r\n\r\n\r\nELM327 v2.1\r\n\r\n>\r\n";
      if (ELM.indexOf("ELM") >= 0){
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
  Serial.print(INIT);
  Serial.print(NewLine);
  
  while (true) {
    if (Serial.available()){
      if (Serial.readString().indexOf("41 00") >= 0){
        tft.print(F("success!"));
        break;
      }
    }
  }
  
  tft.fillScreen(ST7735_BLACK);
  ReadData(COOLANT_TEMPERATURE);
  Water = integers[2] - 40;
  ReadData(OIL_TEMPERATURE);
  Oil = integers[2] - 40;
  ReadData(FUEL);
  Diesel = integers[2];
  
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.setTextSize(3);
  tft.print(F("Water: "));
  
  if (Water < 10) {
    String Wat = String(Water);
    tft.print("  " + Wat);
  }
  else if (Water < 100) {
    String Wat = String(Water);
    tft.print(" " + Wat);
  }
  else tft.print(Water);
  
  
  tft.setCursor(0, 52);
  tft.print(F("Oil: "));
  
  if (Oil < 10) {
    String Oi = String(Oil);
    tft.print("  " + Oi);
  }
  else if (Oil < 100) {
    String Oi = String(Oil);
    tft.print(" " + Oi);
  }
  else tft.print(Oil);
  
  
  tft.setCursor(0, 80);
  tft.print(F("Diesel: "));
  
  if (Diesel < 10) {
    String Dies = String(Diesel);
    tft.print("  " + Dies);
  }
  else if (Diesel < 100){
    String Dies = String(Diesel);
    tft.print(" " + Dies);
  }
  else tft.print(Diesel);
  
}

void loop() {
  tft.setTextColor(ST77XX_BLUE, ST77XX_BLACK);
  tft.setCursor(126, 0);
  
  if (Water < 10) {
    String Wat = String(Water);
    tft.print("  " + Wat);
  }
  else if (Water < 100) {
    String Wat = String(Water);
    tft.print(" " + Wat);
  }
  else tft.print(Water);
  
  tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
  tft.setCursor(90, 52);
  
  if (Oil < 10) {
    String Oi = String(Oil);
    tft.print("  " + Oi);
  }
  else if (Oil < 100) {
    String Oi = String(Oil);
    tft.print(" " + Oi);
  }
  else tft.print(Oil);
  
  tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
  tft.setCursor(0, 104);
  
  if (Diesel < 10) {
    String Dies = String(Diesel);
    tft.print("  " + Dies);
  }
  else if (Diesel < 100){
    String Dies = String(Diesel);
    tft.print(" " + Dies);
  }
  else tft.print(Diesel);

  ReadData(COOLANT_TEMPERATURE);
  Water = integers[2] - 40;
  ReadData(OIL_TEMPERATURE);
  Oil = integers[2] - 40;
  ReadData(FUEL);
  Diesel = integers[2];
  
}

void ReadData(String PID){
  Serial.print(PID);
  Serial.print(NewLine);
  byte b;
  int bread = 1;

  String Data;

  memset(bytes, 0, sizeof(bytes));
  memset(integers, 0, sizeof(integers));
  
  while(true){
    if(Serial.available()){
      Data = Serial.readString();
      if (Data.startsWith("41")){
        break;
      }
    }
  }

  parseString(Data);
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
