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
    if (hr_now >= hr_inicio && hr_now < hr_fim)
    {
      estado = ATIVO;
    }
    // else
    // {
    //   contador_alimentacao = 0;
    // }
    break;
  }
  case ATIVO:
  {
    if (hr_now == hr_alimentacao && min_now == min_alimentacao)
    {
      Serial.println("boia: ");
      Serial.print(hr_now);
      Serial.print(":");
      Serial.print(min_now);
      Serial.print(" ");
      digitalWrite(PIN_RELE, HIGH);
      timer_rele = millis();
      estado = ALIMENTACAO;
    }
    else if (hr_now >= hr_fim)
    {
      estado = SLEEP;
      contador_alimentacao = 0;
    }
    break;
  }
  case ALIMENTACAO:
  {
    if (millis() - timer_rele >= (duracao * 1000))
    {
      digitalWrite(PIN_RELE, LOW);
      Serial.println("alimentou");
      // contador_alimentacao++; // quando terminar alimentacao
      estado = ATIVO;
    }
    break;
  }
  }
}
