#include "functions.h"
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(115200);
  Serial.println("328p ready.");

  if (!rtc.begin())
  {
    Serial.println(F("Couldn't find RTC"));
    while (1)
      ;
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

  if (T_rtc.shouldRun())
    T_rtc.run();

  switch (estado)
  {
  case SLEEP: 
  {

    break;
  }
  case ATIVO: 
  {
    break;
  }
  case CICLO: 
  {
    //code
    break;
  }
  }
}
