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
    PRONTO,
    CICLO,
    EMERGENCIA,
} estado;

//////////////////////////////////////////////////////////////////////
// variáveis:
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
unsigned int variavel = 16;

//////////////////////////////////////////////////////////////////////
// Objetos:
Thread T_debug = Thread();
RTC_DS3231 rtc;

//////////////////////////////////////////////////////////////////////
// prototypes:
void pin_mode();
void threads_controller();
void threads_setup();
void F_debug();

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
// Functions:
void threads_setup()
{
    T_debug.onRun(F_debug);
    T_debug.setInterval(1000);
    T_debug.enabled = false;
}

void checkRTC()
{

    // If you need to set the time of the uncomment line 34 or 37
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

    DateTime now = rtc.now();

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

    // Serial.print(" since midnight 1/1/1970 = ");
    // Serial.print(now.unixtime());
    // Serial.print("s = ");
    // Serial.print(now.unixtime() / 86400L);
    // Serial.println("d");

    // // calculate a date which is 7 days and 30 seconds into the future
    // DateTime future (now + TimeSpan(7,12,30,6));

    // Serial.print(" now + 7d + 30s: ");
    // Serial.print(future.year(), DEC);
    // Serial.print('/');
    // Serial.print(future.month(), DEC);
    // Serial.print('/');
    // Serial.print(future.day(), DEC);
    // Serial.print(' ');
    // Serial.print(future.hour(), DEC);
    // Serial.print(':');
    // Serial.print(future.minute(), DEC);
    // Serial.print(':');
    // Serial.print(future.second(), DEC);
    // Serial.println();

    Serial.print("Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");

    Serial.println();
    delay(500);
}
void F_debug()
{
    Serial.print(" variavel: ");
    Serial.print(variavel);

    Serial.println();
}

void pin_mode()
{
    pinMode(PIN_RELE, OUTPUT);
    digitalWrite(PIN_RELE, LOW); //
}