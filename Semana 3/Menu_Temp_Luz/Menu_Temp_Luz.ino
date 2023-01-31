/**
 * @file Menu_Temp_Luz.ino
 * @author Sebastian Tuquerrez, Brayan Majin, Freider Escobar
 * @title Gestion de temperatura y luz
 * @brief Menu de getion de temperatura y luz, con un LCD I2C, un buzzer pasivo, y un teclado matricial
 * @version 0.1
 * @date 2022-12-21
 *
 * @copyright Copyright Unicauca (c) 2022 *
 */

#include <LiquidCrystal.h>
#include <LiquidMenu.h>
#include <Keypad.h>
#include <EasyBuzzer.h>
#include <EEPROM.h>

/**
 * @brief configuracion de los pines de los leds, el buzzer pasivo, el LCD y el teclado matricial
 */
#define BUZZER_PASIVO 13
#define LED_RED 10
#define LED_GREEN 9
#define LED_BLUE 8
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#pragma region teclado
  const byte ROWS = 4;
  const byte COLS = 4;
  char keys[ROWS][COLS] = {
      {'1', '2', '3', 'A'},
      {'4', '5', '6', 'B'},
      {'7', '8', '9', 'C'},
      {'*', '0', '#', 'D'}};
  byte rowPins[ROWS] = {28, 30, 32, 34};
  byte colPins[COLS] = {36, 38, 40, 42};
  Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
#pragma endregion

/**
 * @brief Valores por defecto de los umbrales de temperatura y luz, y valores maximos de temperatura y luz
 *    Valor de simulacion de sensor de temperatura DHT11
 */
int umbrTempHigh , umbrTempLow , umbrLuzHigh , umbrLuzLow ;

#define DEFAULT_TEMPHIGH 29
#define DEFAULT_TEMPLOW 26
#define DEFAULT_LUZHIGH 800
#define DEFAULT_LUZLOW 300
#define TEST_TEMP 20
#define MAX_TEMP 125
#define MAX_LIGTH 1023



int readKeypad();
void editar_valor(String titulo, byte *varimp);
void color(unsigned char red, unsigned char green, unsigned char blue);

/**
 * @brief Screens textos que se van a mostrar en cada pantalla del menu
 */
#pragma region Screens
  char messages[6][16] = { "1.UmbTempHigh", "2.UmbTempLow", "3.UmbLuzHigh", "4.UmbLuzLow", "5.Reset", "6.SalirMonitoreo" };
  LiquidScreen *lastScreen = nullptr;

  LiquidLine screen_1_line_1(0, 0, messages[0]);
  LiquidLine screen_1_line_2(0, 1, messages[1]);
  LiquidScreen screen_1(screen_1_line_1, screen_1_line_2);

  LiquidLine screen_2_line_1(0, 0, messages[1]);
  LiquidLine screen_2_line_2(0, 1, messages[2]);
  LiquidScreen screen_2(screen_2_line_1, screen_2_line_2);

  LiquidLine screen_3_line_1(0, 0, messages[2]);
  LiquidLine screen_3_line_2(0, 1, messages[3]);
  LiquidScreen screen_3(screen_3_line_1, screen_3_line_2);

  LiquidLine screen_4_line_1(0, 0, messages[3]);
  LiquidLine screen_4_line_2(0, 1, messages[4]);
  LiquidScreen screen_4(screen_4_line_1, screen_4_line_2);

  LiquidLine screen_5_line_1(0, 0, "");
  LiquidLine screen_5_line_2(0, 1, "");
  LiquidScreen screen_5(screen_5_line_1, screen_5_line_2);

  LiquidLine screen_6_line_1(0,0,messages[4]);
  LiquidLine screen_6_line_1(0,1,messages[5]);

  LiquidMenu menu(lcd, screen_1, screen_2, screen_3, screen_4);
#pragma endregion Screens

/**
 * @brief readKeypad Lee los numeros que se ingresen por el keypad lo transforma a int y lo retorna 
 * @return int numero ingresado por el keypad
 */
int readKeypad() {
  String strNumber = "";
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '*') {
        break;
      } else if (key > 'A' && key < 'Z') {
        continue;
      } else if (key >= '0' && key <= '9') {
        strNumber += key;
        lcd.print(key);
      }
    }
  }
  return strNumber.toInt();
}

/**
 * @brief color Genera el color que se le pase por parametro
 * 
 * @param red 
 * @param green 
 * @param blue 
 */
void color(unsigned char red, unsigned char green, unsigned char blue)  // the color generating function
{
  analogWrite(LED_RED, red);
  analogWrite(LED_BLUE, blue);
  analogWrite(LED_GREEN, green);
}

/**
 * @brief editar_valor Genera un submenu segun la opcion y editar el valor que este guardado en variable de dicha opcion del menu y generara un
 *  posible error si el valor ingresado no es valido o se incumplen los valores de los umbrales por defecto
 * @param titulo Titulo de la opcion del menu
 * @param varimp Variable que esta guardada y se va a editar
 */
void editar_valor(String titulo, int *varimp, int posicion) {
  char pressedKey;
  menu.change_screen(&screen_5);
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print(titulo);
  lcd.setCursor(0, 1);
  lcd.print(*varimp);
  lcd.print(" edit=Press 0");  
  while ((pressedKey = keypad.getKey()) != '*' && pressedKey == NO_KEY && pressedKey != '#') {
  }
  if (pressedKey == '#') {
    menu.change_screen(lastScreen);
    return;
  }
  int number = readKeypad();
  if (varimp == &umbrTempLow && number < umbrTempHigh || varimp == &umbrTempHigh && number > umbrTempLow && number <= MAX_TEMP) {
    *varimp = number;
    if(posicion == 2 || posicion == 3){
      EEPROM.put(posicion, *varimp);
    }else{
      EEPROM.write(posicion, *varimp);
    }
    menu.change_screen(lastScreen);
    return;
  }

  if (varimp == &umbrLuzLow && number < umbrLuzHigh || varimp == &umbrLuzHigh && number > umbrLuzLow && number <= MAX_LIGTH) {
    *varimp = number;
    if(posicion == 2 || posicion == 3){
      EEPROM.put(posicion, *varimp);
    }else{
      EEPROM.write(posicion, *varimp);
    }
    menu.change_screen(lastScreen);
    return;
  }
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("Error press \"*\"");
  while ((pressedKey = keypad.getKey()) != '*' && pressedKey == NO_KEY) {
  }

  menu.change_screen(lastScreen);
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16,2);
  menu.add_screen(screen_5);
  pinMode(BUZZER_PASIVO, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  EasyBuzzer.setPin(BUZZER_PASIVO);

  if (EEPROM.read(0) == 255){
    umbrTempHigh = DEFAULT_TEMPHIGH;
  }else{
    umbrTempHigh = EEPROM.read(0);
  }

  if (EEPROM.read(1) == 255){
    umbrTempLow = DEFAULT_TEMPLOW;
  }else{
    umbrTempLow = EEPROM.read(1);
  }

  if (EEPROM.get(2, umbrLuzHigh) == -1){
    umbrLuzHigh = DEFAULT_LUZHIGH;
  }else{
    umbrLuzHigh = EEPROM.get(2, umbrLuzHigh);
  }

  if (EEPROM.get(3, umbrLuzLow) == -1){
    umbrLuzLow = DEFAULT_LUZLOW;
  }else{
    umbrLuzLow = EEPROM.get(3, umbrLuzLow);
  }

  screen_1_line_1.attach_function(1, []() {
    editar_valor("UmbTempHigh", &umbrTempHigh, 0);
  });
  screen_1_line_2.attach_function(1, []() {
    editar_valor("UmbTempLow", &umbrTempLow, 1);
  });

  screen_2_line_1.attach_function(1, []() {
    editar_valor("UmbTempLow", &umbrTempLow, 1);
  });
  screen_2_line_2.attach_function(1, []() {
    editar_valor("UmbLuzHigh", &umbrLuzHigh, 2);
  });

  screen_3_line_1.attach_function(1, []() {
    editar_valor("UmbLuzHigh", &umbrLuzHigh, 2);
  });
  screen_3_line_2.attach_function(1, []() {
    editar_valor("UmbLuzLow", &umbrLuzLow, 3);
  });

  screen_4_line_1.attach_function(1, []() {
    editar_valor("UmbLuzLow", &umbrLuzLow, 3);
  });
  screen_4_line_2.attach_function(1, []() {
    menu.change_screen(&screen_5);
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print("\"*\" to confirm ");
    lcd.setCursor(0, 1);
    lcd.print("\"#\" to cancel  ");
    char pressedKey;
    while ((pressedKey = keypad.getKey()) != '*' && pressedKey == NO_KEY && pressedKey != '#') {
    }
    if (pressedKey == '#') {
      menu.change_screen(lastScreen);
      return;
    }
    umbrTempHigh = DEFAULT_TEMPHIGH;
    umbrTempLow = DEFAULT_TEMPLOW;
    umbrLuzHigh = DEFAULT_LUZHIGH;
    umbrLuzLow = DEFAULT_LUZLOW;
    menu.change_screen(lastScreen);
  });
  menu.update();
}

void loop() {
  EasyBuzzer.update();

  char key = keypad.getKey();
  if (key == 'A') {
    if (menu.get_currentScreen() != &screen_1) {
      menu.previous_screen();
    }
  } else if (key == 'D') {
    if (menu.get_currentScreen() != &screen_4) {
      menu.next_screen();
    }
  } else if (key == '#') {
    menu.switch_focus();
  } else if (key == '*') {
    lastScreen = menu.get_currentScreen();
    menu.call_function(1);
  }
  if (TEST_TEMP > umbrTempHigh) {
    color(255, 0, 0);
    EasyBuzzer.singleBeep(300,500);
  } else if (TEST_TEMP < umbrTempLow) {
    color(0, 0, 255);
    EasyBuzzer.stopBeep();
  } else {
    color(0, 255, 0);
    EasyBuzzer.stopBeep();
  }
}


