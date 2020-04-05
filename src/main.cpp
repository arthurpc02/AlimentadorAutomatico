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

  loadParametersFromEEPROM();
  Serial.print(" hr_inicio: ");
  Serial.print(hr_inicio);
  Serial.print("    hr_fim: ");
  Serial.print(hr_fim);
  Serial.print(" intervalo: ");
  Serial.print(intervalo);
  Serial.print("  duracao: ");
  Serial.print(duracao);
  Serial.println();

  checkRTC();

  pin_mode();

  threads_setup();

  // ihm.configDefaultMsg(F("Engelfish")); // escolhe mensagem inicial do display
  ihm.setup();                                   // inicia o display e imprime mensagem inicial
  delay(2000);
  ihm.addMenuToIndex(&menu_hr_inicio); // coloca os menus em ordem na ihm
  ihm.addMenuToIndex(&menu_hr_fim);
  ihm.addMenuToIndex(&menu_intervalo);
  ihm.addMenuToIndex(&menu_duracao);
  ihm.addMenuToIndex(&menu_principal);

  ihm.focus(&menu_principal);
  // ihm.focus(&menu_produto); // direciona a ihm para iniciar nesse menu
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void loop()
{
  ihm.task();

  if (T_debug.shouldRun())
    T_debug.run();
  if (T_inputs.shouldRun())
    T_inputs.run();
  if (T_rtc.shouldRun())
    T_rtc.run();
  if (T_storeParameters.shouldRun())
    T_storeParameters.run();

  switch (estado)
  {
  case SLEEP:
  {
    if (hr_now >= hr_inicio && hr_now < hr_fim)
    {
      // Serial.println("hr_now");
      // Serial.print(hr_now);
      // Serial.print(" hr_inicio");
      // Serial.print(hr_inicio);
      // Serial.print(" hr_fim");
      // Serial.print(hr_fim);
      // Serial.println();
      ihm.focus(&menu_principal);
      ihm.changeCustomString(&menu_principal, 2,"ATIVO");
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
    calculaAlimentacao();
    if (hr_now == hr_alimentacao && min_now == min_alimentacao)
    {
      Serial.println("boia: ");
      Serial.print(hr_now);
      Serial.print(":");
      Serial.print(min_now);
      Serial.print(" ");
      digitalWrite(PIN_RELE, HIGH);
      timer_rele = millis();
      ihm.focus(&menu_principal);
      ihm.changeCustomString(&menu_principal, 2,"ALIMENTANDO");
      estado = ALIMENTACAO;
    }
    else if (hr_now >= hr_fim)
    {
      estado = SLEEP;
      ihm.focus(&menu_principal);
      ihm.changeCustomString(&menu_principal, 2,"INATIVO");
      contador_alimentacao = 0;
    }
    break;
  }
  case ALIMENTACAO:
  {
    if (millis() - timer_rele >= ((unsigned int)duracao * 1000))
    {
      digitalWrite(PIN_RELE, LOW);
      Serial.println("alimentou");
      ihm.focus(&menu_principal);
      ihm.changeCustomString(&menu_principal, 2,"ATIVO");
      // contador_alimentacao++; // quando terminar alimentacao
      estado = ATIVO;
    }
    break;
  }
  }
}
