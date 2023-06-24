#include <SoftwareSerial.h>
#include "ELMduino.h"


SoftwareSerial mySerial(11, 12); // RX, TX
#define ELM_PORT mySerial


ELM327 myELM327;


uint32_t rpm = 0;
float engineCoolantTemp = 0;
float oilTemp = 0;

const int statePin = 2;

void setup()
{
#if LED_BUILTIN
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
#endif

  Serial.begin(38400);
  ELM_PORT.begin(38400);

  while(true){
    if (digitalRead(statePin) == HIGH){
      break;
    }
  }

  Serial.println("Attempting to connect to ELM327...");

  if (!myELM327.begin(ELM_PORT, true, 2000))
  {
    Serial.println("Couldn't connect to OBD scanner");
    while (1);
  }

  Serial.println("Connected to ELM327");
}


void loop()
{
  float tempRPM = myELM327.rpm();
  engineCoolantTemp = myELM327.engineCoolantTemp();
  oilTemp = myELM327.oilTemp();

  if (myELM327.nb_rx_state == ELM_SUCCESS)
  {
    rpm = (uint32_t)tempRPM;
    Serial.print("RPM: "); Serial.println(rpm);
    Serial.print("Water: "); Serial.println(engineCoolantTemp);
    Serial.print("Oil: "); Serial.println(oilTemp);
  }
  else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    myELM327.printError();
}
