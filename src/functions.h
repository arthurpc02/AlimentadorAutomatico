#include <Arduino.h>

#include <personalHeader.h>
#include <Thread.h>

//////////////////////////////////////////////////////////////////
// Hardware:
#define PIN_INTERRUPT0 2
#define PIN_INTERRUPT1_PWM2 3
#define PIN_4 4
#define PIN_5_PWM0 5
#define PIN_6_PWM0 6
#define PIN_7 7
#define PIN_8 8
#define PIN_PWM1 9
#define PIN_SS_PWM1 10
#define PIN_MOSI_PWM2 11
#define PIN_MISO 12
#define PIN_SCK 13
#define PIN_ANALOG0 A0
#define PIN_ANALOG1 A1
#define PIN_ANALOG2 A2
#define PIN_ANALOG3 A3
#define PIN_SDA A4
#define PIN_SCL A5

//inputs:

//outputs:


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
unsigned int variavel = 16;



//////////////////////////////////////////////////////////////////////
// Objetos:
Thread T_debug = Thread();


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


void F_debug()
{
    Serial.print(" variavel: ");
    Serial.print(variavel);

    Serial.println();
}

void pin_mode()
{
    pinMode(PIN_5_PWM0, OUTPUT);
    digitalWrite(PIN_5_PWM0, LOW); //
}