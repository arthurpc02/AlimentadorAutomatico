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
  calculaAlimentacao();

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
    if (hr_now >= hr_inicio && hr_now <= hr_fim)
    {
      if (hr_now == hr_alimentacao && min_now == min_alimentacao)
      {
        Serial.println("hr da boia");
        digitalWrite(PIN_RELE, HIGH);
        timer_rele = millis();
        estado = ATIVO;
      }
    }
    else
    {
      counter_alimentacao = 0;
      calculaAlimentacao();
    }
    break;
  }
  case ATIVO:
  {
    if (millis() - timer_rele >= (duracao * 1000))
    {
      digitalWrite(PIN_RELE, LOW);
      Serial.println("alimentou");
      counter_alimentacao++; // quando terminar alimentacao
      calculaAlimentacao();
    }
    break;
  }
  }
}
