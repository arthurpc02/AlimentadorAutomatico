#include <Arduino.h>

#include <personalHeader.h>
#include <Thread.h>
#include <RTClib.h>

//////////////////////////////////////////////////////////////////
// defines:

//inputs:
#define PIN_BOTOES A0

//outputs:
#define PIN_RELE 11

//////////////////////////////////////////////////////////////////////
// enums e structs:
enum Estado
{
    SLEEP,       // estado SLEEP: Não está no horario de alimentação (entre 'hr_inicio' e 'hr_fim')
    ATIVO,       // estado ATVIO: Está no horario de alimentação (entre 'hr_inicio' e 'hr_fim')
    ALIMENTACAO, // estado ALIMENTACAO: realizando alimentação, ou seja, acionando o relé pelo tempo determinado pelo parâmetro 'duracao'
};
Estado estado = SLEEP;

//////////////////////////////////////////////////////////////////////
// variáveis:
unsigned long timer_rele = 0;
int hr_now = 6;
int min_now = 6;
int hr_alimentacao = 17;
int min_alimentacao = 40;
int contador_alimentacao = 0;

//parâmetros:
int hr_inicio = 10;
int hr_fim = 11;
int intervalo = 5; // minutos
int duracao = 15;  // segundos

//eeprom
const byte adr_inicio = 50; //escolhi uma localização qualquer na eeprom para começar a armazenar os valores
const byte adr_fim = adr_inicio + sizeof(hr_inicio);
const byte adr_intervalo = adr_fim + sizeof(hr_fim);
const byte adr_duracao = adr_intervalo + sizeof(intervalo);

//////////////////////////////////////////////////////////////////////
// Objetos:
Thread T_debug = Thread();
Thread T_storeParameters = Thread();
Thread T_rtc = Thread();
RTC_DS3231 rtc;

//////////////////////////////////////////////////////////////////////
// prototypes:
void F_debug();
void F_storeParameters();
void F_rtc();
void pin_mode();
void threads_controller();
void threads_setup();
void calculaAlimentacao();
void loadParametersFromEEPROM();

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
// Functions:
void threads_setup()
{
    T_debug.onRun(F_debug);
    T_debug.setInterval(60000);
    // T_debug.enabled = false;

    T_storeParameters.onRun(F_storeParameters);
    T_storeParameters.setInterval(3000);
    // T_storeParameters.enabled = false;

    T_rtc.onRun(F_rtc);
    T_rtc.setInterval(50);
    // T_rtc.enabled = false;
}

void F_debug()
{
    // Serial.print(" hr: ");
    // Serial.print(hr_now);
    // Serial.print(" min: ");
    // Serial.print(min_now);
    Serial.print("    hr_alime: ");
    Serial.print(hr_alimentacao);
    Serial.print(" min_alime: ");
    Serial.print(min_alimentacao);
    Serial.print("  contador: ");
    Serial.print(contador_alimentacao);
    Serial.println();
}

void F_rtc()
{
    DateTime now = rtc.now();
    hr_now = now.hour();
    min_now = now.minute();
    // calculaAlimentacao();
}

void F_storeParameters()
{
    EEPROM.put(adr_inicio, hr_inicio);
    EEPROM.put(adr_fim, hr_fim);
    EEPROM.put(adr_intervalo, intervalo);
    EEPROM.put(adr_duracao, duracao);
}

void loadParametersFromEEPROM()
{
    EEPROM.get(adr_inicio, hr_inicio);
    EEPROM.get(adr_fim, hr_fim);
    EEPROM.get(adr_intervalo, intervalo);
    EEPROM.get(adr_duracao, duracao);
}

// Calcula em qual hora e em qual minuto ocorrerá a próxima alimentacao
void calculaAlimentacao()
{
    int hr_intervalo = contador_alimentacao * intervalo / 60;
    int min_intervalo = contador_alimentacao * intervalo % 60;
    hr_alimentacao = hr_inicio + hr_intervalo;
    min_alimentacao = min_intervalo;

    if (hr_alimentacao <= hr_now && min_alimentacao <= min_now)
    {
        contador_alimentacao++;
    }
    // Serial.print(" hr_int: ");
    // Serial.print(hr_intervalo);
    // Serial.print(" min_int: ");
    // Serial.print(min_intervalo);
    // Serial.print(" hr_alime: ");
    // Serial.print(hr_alimentacao);
    // Serial.print(" min_alime: ");
    // Serial.print(min_alimentacao);
    // Serial.print("  contador: ");
    // Serial.print(contador_alimentacao);
    // Serial.println();
}

void pin_mode()
{
    pinMode(PIN_RELE, OUTPUT);
    digitalWrite(PIN_RELE, LOW); //
}

void checkRTC()
{

    // If you need to set the time of the rtc uncomment line 34 or 37
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

    char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

    DateTime now = rtc.now();

    hr_now = now.hour();
    min_now = now.minute();

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    Serial.print("Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");

    Serial.println();
    delay(500);
}