#ifndef IHMNATIVA
#define IHMNATIVA

// to do:
//     - flag i2c busy
//     - setas
//     - aceitar ints também como parâmetro no addMenu();
//     - eeprom
//     - selecao de produtos
//     - biblioteca parametros (com template)
// done:
//     - digitalReadFast
//     - colocar macro F nas strings
//     - menu status (menu type)
//     - structParametro = r/w, eeprom, casas decimais, etc
//     - quanto aumenta da variavel a cada pressionada de botão
//     - colocar menus na ordem
//     - tela de abertura com contato da sunnytec
//     - mudar struct menu para classe menu
//     - implementar restante dos botões
//     - próximo menu
//     - apenas uma função executa todas as tasks da library
//     - adicionar unidade de medida, mínimo, máximo e setas para os menus
//     - criar menus e variaveis
//     - struct "menu"
//     - volta a usar classe
//     - biblioteca para o display
//     - atualizar display
//     - ler botões
//     - intervalo para ler botões
//     - intervalo para atualizar ações e display

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);

enum TipoMenu
{
    PARAMETRO,
    STATUS,
    MONITOR,
    READONLY,
};

class Menu
{
public:
//constructor:
    Menu(String par_name = "def",
         TipoMenu par_tipo = READONLY,
         int *par_variavel = NULL,
         String par_unidade = " ",
         int par_acrescimo = 1,
         int par_minimo = 0,
         int par_maximo = 9999,
         int * par_produto = NULL)
    {
        name = par_name;
        tipo = par_tipo;
        p_variavel = par_variavel;
        unidade = par_unidade;
        acrescimo = par_acrescimo;
        minimo = par_minimo;
        maximo = par_maximo;
        produto = par_produto;
    }

    Menu(String par_name = "def", TipoMenu par_tipo = STATUS, String par_msg = " ", int * par_produto = NULL)
    {
        name = par_name;
        tipo = par_tipo;
        message = par_msg;
        produto = par_produto;
    }

    void changeMsg(String msg)
    {
        message = msg;
    }

    String getName()
    {
        return name;
    }

    int *getVariavel()
    {
        return p_variavel;
    }

    String getUnidade()
    {
        return unidade;
    }

    int getAcrescimo()
    {
        return acrescimo;
    }

    int getMinimo()
    {
        return minimo;
    }

    int getMaximo()
    {
        return maximo;
    }

    TipoMenu getTipo()
    {
        return tipo;
    }

    String getMsg()
    {
        return message;
    }

    void setMonitorMenus(Menu *par_menu1 = NULL, Menu *par_menu2 = NULL, Menu *par_menu3 = NULL, Menu *par_menu4 = NULL)
    {
        menu1 = par_menu1;
        menu2 = par_menu2;
        menu3 = par_menu3;
        menu4 = par_menu4;
    }

    int * getProduto()
    {
        return produto;
    }

    Menu *menu1 = NULL;
    Menu *menu2 = NULL;
    Menu *menu3 = NULL;
    Menu *menu4 = NULL;

private:
    String name = "def";
    int *p_variavel = NULL;
    TipoMenu tipo = READONLY;
    int type = 0;
    String unidade = " ";
    int acrescimo = 1;
    int minimo = 0;
    int maximo = 9999;
    String message = " ";
    int * produto = NULL;
};

class ihmNativa
{
public:
    //construtor:
    ihmNativa()
    {
    }

    Menu *getMenu()
    {
        return currentMenu;
    }

    void focus(Menu *menu_f)
    {
        selectMenu(menu_f);
    }

    void updateStatusMonitor(String message, Menu *p_menu)
    {
        p_menu->changeMsg(message);
        flag_variavelMudou = true;
    }

    // void updateStatus(String message)
    // {
    //     Menu_status.changeMsg(message);
    //     flag_variavelMudou = true;
    //     // selectMenu(&Menu_status);
    // }

    void signalVariableChange()
    {
        flag_variavelMudou = true;
    }

    // void updateStatusFocus(String message)
    // {
    //     Menu_status.changeMsg(message);
    //     selectMenu(&Menu_status);
    // }

    // lasts many seconds because of a delay while showing default msg
    void setup()
    {
        lcd.init();
        lcd.backlight();
        lcd.print(F("Display ready.")); //(F()) saves string to flash & keeps dynamic memory free

        // addMenuToIndex(&defaultMenu);
        displayDefaultMsg();
        // addMenuToIndex(&Menu_status);
        // selectMenu(&Menu_status);

        task();
        // estruturaMenu_parametros();
    }

    void task();

    void addMenuToIndex(Menu *menu)
    {
        menuList[menuCount] = menu;
        menuCount++;
    }

    void removeMenuFromIndex()
    {
        menuCount--;
        menuList[menuCount] = NULL;
        // selectMenu(menuList[menuCount - 1]);
    }

    void configDefaultMsg(String msg)
    {
        default_msg = msg;
    }

    void showStatus2msg(String msg) // muda a quarta linha do display para a mensagem enviada, independente de qual menu está ativo
    {
        // if (currentMenu != &Menu_status)
        // {
            status2 = msg;
            flag_status2 = true;
        // }
    }

#define NEXT 1
#define PREVIOUS -1
    void changeMenu(int direction) // passa para o anterior ou próximo menu, dependendo das posições dos Menus no index
    {
        static int index = 0;
        index += constrain(direction, -1, 1);
        if (index >= menuCount)
        {
            index = 0;
        }
        else if (index < 0)
        {
            index = menuCount - 1;
        }

        selectMenu(menuList[index]);
    }

    void addVar(MaisOuMenos maisOuMenos) // incrementa ou decrementa a variável associada ao Menu atual
    {
        maisOuMenos = static_cast<MaisOuMenos>(constrain(maisOuMenos, -1, 1));
        *(currentMenu->getVariavel()) += maisOuMenos * (currentMenu->getAcrescimo());
        *(currentMenu->getVariavel()) = constrain(*(currentMenu->getVariavel()), currentMenu->getMinimo(), currentMenu->getMaximo());
        // Serial.print("min: "); Serial.print(currentMenu->min);
        // Serial.println("  add");
        //currentMenu->var = currentMenu->var + maisOuMenos*(currentMenu->passo);
    }

private:
    Menu *menuList[9] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
    Menu *currentMenu = NULL;
    // Menu Menu_status = Menu("Status", STATUS, "ok");

    unsigned long timer_eventos = 0;
    unsigned int intervalo_eventos = 50; //miliseconds

    // String asdf = " ";

    // variáveis ihm:
    String line1 = "z";
    String line2 = "z";
    String line3 = "z";
    String line4 = "z";

    String oldLine1 = "z";
    String oldLine2 = "z";
    String oldLine3 = "z";
    String oldLine4 = "z";

    String status2 = " ";

    enum Setas
    {
        nenhuma,
        linha1,
        linha2,
        linha3,
        linha4
    } setas;

    String default_msg = "Controlador";
    int menuCount = 0;

    bool flag_menuChanged = false;
    bool flag_variavelMudou = false;
    bool flag_status2 = false;

    void selectMenu(Menu *menu)
    {
        flag_menuChanged = true;
        currentMenu = menu;
    }

    void eventProcessor_parametros()
    {
        // rotina que trata os botões acionados
        if (millis() - timer_eventos >= intervalo_eventos)
        {
            timer_eventos = millis();
            if (flag_variavelMudou == true)
            {
                flag_variavelMudou = false;
                line3 = String(*currentMenu->getVariavel());
                line3.concat(F(" "));
                line3.concat(currentMenu->getUnidade());
                display();
            }
            else if (flag_menuChanged == true)
            {
                flag_menuChanged = false;
                estruturaMenu_parametros();
                display();
            }
            else if (flag_status2 == true)
            {
                flag_status2 = false;
                line4 = status2;
                display();
            }
        }
    }

    void eventProcessor_readOnly()
    {
        // rotina que trata os botões acionados
        if (millis() - timer_eventos >= intervalo_eventos)
        {
            timer_eventos = millis();
            if (flag_variavelMudou == true)
            {
                flag_variavelMudou = false;
                line3 = String(*currentMenu->getVariavel());
                line3.concat(F(" "));
                line3.concat(currentMenu->getUnidade());
                display();
            }
            else if (flag_menuChanged == true)
            {
                flag_menuChanged = false;
                estruturaMenu_readOnly();
                display();
            }
            else if (flag_status2 == true)
            {
                flag_status2 = false;
                line4 = status2;
                display();
            }
        }
    }

    void eventProcessor_monitor()
    {
        // rotina que trata os botões acionados
        if (millis() - timer_eventos >= intervalo_eventos)
        {
            timer_eventos = millis();
            if (flag_variavelMudou == true)
            {
                flag_variavelMudou = false;
                estruturaMenu_monitor();
                display();
            }
            else if (flag_menuChanged == true)
            {
                flag_menuChanged = false;
                estruturaMenu_monitor();
                display();
            }
            else if (flag_status2 == true)
            {
                flag_status2 = false;
                line4 = status2;
                display();
            }
        }
    }

    void eventProcessor_status()
    {
        // rotina que trata os botões acionados
        if (millis() - timer_eventos >= intervalo_eventos)
        {
            timer_eventos = millis();
            if (flag_variavelMudou == true)
            {
                flag_variavelMudou = false;
                line3 = String(currentMenu->getMsg());
                display();
            }
            else if (flag_menuChanged == true)
            {
                flag_menuChanged = false;
                estruturaMenu_status();
                display();
            }
            else if (flag_status2 == true)
            {
                flag_status2 = false;
                line4 = status2;
                display();
            }
        }
    }

    void estruturaMenu_parametros()
    {
        if (currentMenu->getProduto() == NULL)
        {
            line1 = F("--------------------");
        }
        else
        {
            line1 = F("Produto: ");
            line1.concat((String)*currentMenu->getProduto());
        }

        line2 = currentMenu->getName();
        line2.concat(F(": "));

        line3 = String(*currentMenu->getVariavel());
        line3.concat(F(" "));
        line3.concat(currentMenu->getUnidade());

        line4 = F("min: ");
        line4.concat(String(currentMenu->getMinimo()));
        line4.concat(F(" max: "));
        line4.concat(String(currentMenu->getMaximo()));
    }

    void estruturaMenu_readOnly()
    {
        line1 = F("--------------------");

        line2 = currentMenu->getName();
        line2.concat(F(": "));

        line3 = String(*currentMenu->getVariavel());
        line3.concat(F(" "));
        line3.concat(currentMenu->getUnidade());

        line4 = F("min: - ");
        // line4.concat(String(currentMenu->getMinimo()));
        line4.concat(F("  max: -"));
        // line4.concat(String(currentMenu->getMaximo()));
    }

    void estruturaMenu_monitor()
    {
        line1 = currentMenu->menu1->getName();
        line1.concat(F(": "));
        line1.concat(*currentMenu->menu1->getVariavel());
        // line1.concat(F(" "));
        // line1.concat(currentMenu->menu1->getUnidade());

        line2 = currentMenu->menu2->getName();
        line2.concat(F(": "));
        line2.concat(*currentMenu->menu2->getVariavel());
        // line2.concat(F(" "));
        // line2.concat(currentMenu->menu2->getUnidade());

        line3 = currentMenu->menu3->getName();
        line3.concat(F(": "));
        line3.concat(*currentMenu->menu3->getVariavel());
        // line3.concat(F(" "));
        // line3.concat(currentMenu->menu3->getUnidade());

        line4 = currentMenu->menu4->getMsg();
    }

    void estruturaMenu_status()
    {
        if (currentMenu->getProduto() == NULL)
        {
            line1 = F("--------------------");
        }
        else
        {
            line1 = F("Produto: ");
            line1.concat((String)*currentMenu->getProduto());
        }

        line2 = currentMenu->getName();
        line2.concat(F(": "));

        line3 = currentMenu->getMsg();

        line4 = F(" ");
    }

    void display()
    {
        if (line1 != oldLine1)
        {
            lcd.setCursor(0, 0);
            lcd.print(F("                    "));
            lcd.setCursor(0, 0);
            lcd.print(line1);

            oldLine1 = line1;
        }
        if (line2 != oldLine2)
        {
            lcd.setCursor(0, 1);
            lcd.print(F("                    "));
            lcd.setCursor(0, 1);
            lcd.print(line2);

            oldLine2 = line2;
        }
        // if (line3 != oldLine3)
        // {
        //     lcd.setCursor(0, 2);
        //     lcd.print(F("                    "));
        //     lcd.setCursor(0, 2);
        //     lcd.print(line3);

        //     oldLine3 = line3;
        // }
        // if (line4 != oldLine4)
        // {
        //     lcd.setCursor(0, 3);
        //     lcd.print(F("                    "));
        //     lcd.setCursor(0, 3);
        //     lcd.print(line4);

        //     oldLine4 = line4;
        // }
        if (setas > 0)
        {
            lcd.setCursor(0, setas - 1);
            lcd.write(0x7F); //seta para esquerda
            lcd.setCursor(19, setas - 1);
            lcd.write(0x7E); //seta para direita
            setas = static_cast<Setas>(0);
        }
    }

    void displayDefaultMsg()
    {
        line1 = F("   EngelFish");
        line2 = F("  Alimentador");
        line3 = F("     ");
        line4 = default_msg;
        display();
        delay(2000);
    }

    void eventos() // check for events
    {
        static int var = -1;
        if (currentMenu->getVariavel() != NULL)
        {
            if (var != *currentMenu->getVariavel())
            {
                var = *currentMenu->getVariavel();
                flag_variavelMudou = true;
            }
        }
    }
};

void ihmNativa::task()
{
    switch (currentMenu->getTipo())
    {
    case PARAMETRO:
    {
        eventos();
        eventProcessor_parametros();
        break;
    }
    case READONLY:
    {
        eventos();
        eventProcessor_readOnly();
        break;
    }
    case STATUS:
    {
        eventos();
        eventProcessor_status();
        break;
    }
    case MONITOR:
    {
        // eventos_monitor();
        eventProcessor_monitor();
        break;
    }
    }
}

#endif