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
    SLEEP,       // estado SLEEP: Não está no horario de alimentação (entre 'hr_inicio' e 'hr_fim', e no intervalo certo)
    ATIVO,       // estado ATVIO: realizando alimentação, ou seja, acionando o relé pelo tempo determinado pelo parâmetro 'duracao'
    ALIMENTACAO, //
} estado;

//////////////////////////////////////////////////////////////////////
// variáveis:
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

unsigned long timer_rele = 0;
int hr_now = 6;
int min_now = 6;
int hr_alimentacao = 17;
int min_alimentacao = 40;
int contador_alimentacao = 0;

//parâmetros:
int hr_inicio = 21;
int hr_fim = 22;
int intervalo = 5; // minutos
int duracao = 10;   // segundos

//////////////////////////////////////////////////////////////////////
// Objetos:
Thread T_debug = Thread();
Thread T_rtc = Thread();
RTC_DS3231 rtc;

//////////////////////////////////////////////////////////////////////
// prototypes:
void pin_mode();
void threads_controller();
void threads_setup();
void calculaAlimentacao();
void F_debug();
void F_rtc();

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
// Functions:
void threads_setup()
{
    T_debug.onRun(F_debug);
    T_debug.setInterval(2000);
    T_debug.enabled = false;

    T_rtc.onRun(F_rtc);
    T_rtc.setInterval(50);
    // T_rtc.enabled = false;
}

void F_debug()
{
    Serial.print(" hr: ");
    Serial.print(hr_now);
    Serial.print(" min: ");
    Serial.print(min_now);
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
    calculaAlimentacao();
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