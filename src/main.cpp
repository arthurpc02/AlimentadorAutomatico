#include "functions.h"
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(115200);
  Serial.println("328p ready.");

  if (! rtc.begin()) {
    Serial.println(F("Couldn't find RTC"));
    while (1);
  }
  
  checkRTC();

  pin_mode();

  threads_setup();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void loop()
{

  if (T_debug.shouldRun())
    T_debug.run();



  switch (estado)
  {

  case PRONTO:
  {
    //code
    break;
  }
  case CICLO:
  {
    //code
    break;
  }
  case EMERGENCIA:
  {
    //code
    break;
  }
  }
}
