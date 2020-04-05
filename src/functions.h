#include <Arduino.h>

#include <personalHeader.h>
#include <Thread.h>
#include <RTClib.h>
#include <ihmNativa.h>

//////////////////////////////////////////////////////////////////
// defines:

//inputs:
#define PIN_BOTOES A0

#define PIN_DIREITA 3
#define PIN_ESQUERDA 4
#define PIN_CIMA 5
#define PIN_BAIXO 6
#define PIN_SELECT 7

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
bool flag_teste = false;
String msg_ativo = "ATIVO";
String msg_inativo = "INATIVO";
String msg_alimentando = "ALIMENTANDO";

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
Thread T_inputs = Thread();
Thread T_storeParameters = Thread();
Thread T_rtc = Thread();
RTC_DS3231 rtc;
ihmNativa ihm;

Menu menu_hr_inicio = Menu("Hora Inicio", PARAMETRO, &hr_inicio, "hr", 1, 0, 23);
Menu menu_hr_fim = Menu("Hora Fim", PARAMETRO, &hr_fim, "hr", 1, 0, 23);
Menu menu_intervalo = Menu("Intervalo", PARAMETRO, &intervalo, "min", 2, 0, 1440);
Menu menu_duracao = Menu("Duracao", PARAMETRO, &duracao, "seg", 1, 0, 300);

Menu menu_principal = Menu("Principal", CUSTOM, "Relogio: ", "Status");

//////////////////////////////////////////////////////////////////////
// prototypes:
void F_debug();
void F_inputs();
void F_inputsTemp();
void F_storeParameters();
void F_rtc();
void pin_mode();
void threads_controller();
void threads_setup();
void calculaAlimentacao();
void loadParametersFromEEPROM();
void botaoDireita();
void botaoEsquerda();
void botaoCima();
void botaoBaixo();
void botaoSelect();

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
// Functions:
void threads_setup()
{
    T_debug.onRun(F_debug);
    T_debug.setInterval(60000);
    // T_debug.enabled = false;

    T_inputs.onRun(F_inputsTemp);
    T_inputs.setInterval(300);
    // T_inputs.enabled = false;

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

void botaoDireita()
{
    Serial.println("R: next");
    ihm.changeMenu(NEXT);
}

void botaoEsquerda()
{
    Serial.println("Left: prev");
    ihm.changeMenu(PREVIOUS);
}

void botaoCima()
{
    Serial.println("Up: add");
    ihm.addVar(MAIS);
}

void botaoBaixo()
{
    Serial.println("Down: sub");
    ihm.addVar(MENOS);
}

void botaoSelect()
{
    Serial.println("b_select");
    flag_teste = true;
}

// Função de Leitura de botoes usando entradas digitais, apenas para teste
void F_inputsTemp()
{
    if (digitalRead(PIN_DIREITA) == LOW)
    {
        botaoDireita();
    }
    else if (digitalRead(PIN_ESQUERDA) == LOW)
    {
        botaoEsquerda();
    }
    else if (digitalRead(PIN_CIMA) == LOW)
    {
        botaoCima();
    }
    else if (digitalRead(PIN_BAIXO) == LOW)
    {
        botaoBaixo();
    }
    else if (digitalRead(PIN_SELECT) == LOW)
    {
        botaoSelect();
    }
}

// Função de Leitura de botoes usando entrada analógica
void F_inputs()
{
    int botoes = analogRead(PIN_BOTOES); // leitura do pino A0

    // leitura  menor que 50, botao RIGHT foi pressionado
    if (botoes < 50)
    {
        botaoDireita();
    }
    // leitura é menor que 250, botao UP foi pressionado
    else if (botoes < 250)
    {
        botaoCima();
    }

    //  leitura é menor que 450, botao DOWN foi pressionado
    else if (botoes < 450)
    {
        botaoBaixo();
    }

    //  leitura é menor que 650, botao LEFT foi pressionado
    else if (botoes < 650)
    {
        botaoEsquerda();
    }

    //  leitura é menor que 850, botao SELECT foi pressionado
    else if (botoes < 850)
    {
        botaoSelect();
    }
    // leitura  maior que 1000, significa que não foi apertado nenhum botão
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

    int totalAlimentacao = hr_alimentacao * 60 + min_alimentacao;
    int totalNow = hr_now * 60 + min_now;
    if (totalAlimentacao <= totalNow)
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
    digitalWrite(PIN_RELE, LOW); // inativo

    pinMode(PIN_BOTOES, INPUT); // botoes do LCD

    pinMode(PIN_DIREITA, INPUT_PULLUP);
    pinMode(PIN_ESQUERDA, INPUT_PULLUP);
    pinMode(PIN_CIMA, INPUT_PULLUP);
    pinMode(PIN_BAIXO, INPUT_PULLUP);
    pinMode(PIN_SELECT, INPUT_PULLUP);
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