

#include <Keypad.h>
#include <LiquidCrystal.h>
#include <LiquidMenu.h>
#include <EEPROM.h>

#define LED_BLUE 3
#define LED_RED 2
#define LED_GREEN 4
#define BUZZER_PASIVO  6

//Definicion de los pines de los sensores
#define SENSOR_TEMP 5
#define SENSOR_LUZ 0

//melodias
#define NOTE_B0  31 
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
int melodia[] = { 
NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_F4, NOTE_E4, NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4, NOTE_C4, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_AS4, NOTE_AS4, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_F4};
int duraciones[] = {
8, 8, 4, 4, 4, 2, 8, 8, 4, 4, 4, 2, 8, 8, 4, 4, 4, 4, 4, 8, 8, 4, 4, 4, 2};


//teclado
const byte ROWS = 4;  
const byte COLS = 4;  

char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A'},
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C'},
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = {32,34,36,38}; 
byte colPins[COLS] = {40,42,44,46}; 

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


//liquid cristal
const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 8, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Valores por defecto
int umbrTempHigh , umbrTempLow , umbrLuzHigh , umbrLuzLow ;

#define DEFAULT_TEMPHIGH 29
#define DEFAULT_TEMPLOW 26
#define DEFAULT_LUZHIGH 800
#define DEFAULT_LUZLOW 300
#define TEST_TEMP 20
#define MAX_TEMP 125
#define MAX_LIGTH 1023

#define DisplayClass LiquidCrystal_I2C

//variables de la contraseña
char password[4] = {'1','2','3','4'};
char passwordAux[4] ;
char key;
int trys = 1, cont = 0, iden = 0, cont2 = 0, mon = 0;
unsigned long inicio = 0;
unsigned long constante = 0;

float temperatura = 0;  //Guarda la temperatura del sensor
float luz = 0;  //Guarda la luz del sensor
const float BETA = 3950;

//Variables para la alarma
float sinVal;
int toneVal;
boolean stateAlarm;

/**
 * @brief Screens textos que se van a mostrar en cada pantalla del menu
 */
#pragma region Screens
  char messages[6][16] = { "1.UmbTempHigh", "2.UmbTempLow", "3.UmbLuzHigh", "4.UmbLuzLow", "5.Reset", "6.Monitoreo" };
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

  LiquidLine screen_5_line_1(0, 0, messages[4]);
  LiquidLine screen_5_line_2(0, 1, messages[5]);
  LiquidScreen screen_5(screen_5_line_1, screen_5_line_2);

  LiquidLine screen_6_line_1(0, 0, "");
  LiquidLine screen_6_line_2(0, 1, "");
  LiquidScreen screen_6(screen_6_line_1, screen_6_line_2);

  LiquidMenu menu(lcd);
  
#pragma endregion Screens


enum estados
{
  estado_contrasenia,
  estado_menu,
  estado_monitoreo,
  estado_alarma
};

enum transiciones
{
  Unknown,
  input_Clave,
  input_Temp,
  input_Boton,
  input_Time
};

// Variables globales
estados ultimoEstado;
transiciones ultimaTransicion;

// Acciones de los estados y condiciones de transiciones
void estadoSeguridad()
{
  if (ultimaTransicion == transiciones::input_Clave)
    cambioDeEstado(estados::estado_menu);
  if (ultimaTransicion == transiciones::Unknown)
    cambioDeEstado(estados::estado_contrasenia);
}

void estadoMenuConfig()
{
  if (ultimaTransicion == transiciones::input_Temp)
    cambioDeEstado(estados::estado_monitoreo);
}

void estadoMonitoreo()
{
  if (ultimaTransicion == transiciones::input_Time)
    cambioDeEstado(estados::estado_alarma);
  if (ultimaTransicion == transiciones::input_Boton)
    cambioDeEstado(estados::estado_menu);
  if(ultimaTransicion == transiciones::input_Temp)
    cambioDeEstado(estados::estado_monitoreo);
}

void estadoAlarma()
{
  if (ultimaTransicion == transiciones::input_Temp)
    cambioDeEstado(estados::estado_monitoreo);
}

void(* resetSoftware)(void) = 0;

void alarmaBloqueo(){
  for (int i=0; i <180; i++){
    // convert degrees to radians then obtain sin value
    sinVal = (sin(i*(3.1412/180)));
    // generate a frequency from the sin value
    toneVal = 2000+(int(sinVal*1000));

    tone(6, toneVal);
    tiempoEspera(2);
  }
}

void tiempoEspera(int tiempo) {
  inicio = millis();
  constante = millis();
  while ((constante-inicio) <= (tiempo) ) {
    constante= millis();
  }
}

// Salidas asociadas a las transiciones
void Seguridad()
{
  Serial.println("ESTADO CONTRASENIA");
  while(true){
    if(trys == 4){
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("SISTEMA");
      lcd.setCursor(3, 1);
      lcd.print("BLOQUEADO");
      digitalWrite(LED_RED, HIGH);
      alarmaBloqueo();
      tiempoEspera(4000);
      noTone(6);
      digitalWrite(LED_RED, LOW);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("REINICIANDO...");
      tiempoEspera(2000);
      lcd.clear();
      resetSoftware();
      break;
    }
    lcd.setCursor(0, 0);
    lcd.print("INGRESE LA CLAVE");

    key = keypad.getKey();

    if(key){
      passwordAux[cont] = key;
      cont++;

      lcd.setCursor(cont2, 1);
      lcd.print("*");
      cont2++;

      if(cont == 4){

        for(int i=0; i<4; i++){
          if(passwordAux[i] != password[i]){
            iden = 1;
          }
        }

        if(iden == 1){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("CLAVE INCORRECTA");
          lcd.setCursor(0,1);
          lcd.print((3-trys));
          lcd.setCursor(1,1);
          lcd.print(" INTENTOS MAS");
          tiempoEspera(1000);
          cont = 0;
          cont2 = 0;
          iden = 0;
          trys++;
          lcd.clear();
        }else{
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("CLAVE CORRECTA");
          lcd.setCursor(0, 1);
          lcd.print("Entrando...");
          ultimaTransicion = transiciones::input_Clave;
          digitalWrite(LED_GREEN, HIGH);
          tiempoEspera(1000);
          digitalWrite(LED_GREEN, LOW);
          cont = 0;
          cont2= 0;
          iden = 0;
          trys =1;
          lcd.clear();
          return;
        }
      }
    }
  }
}
void MenuConfig()
{
  Serial.println("ESTADO MENU");
  while(true){
    char key = keypad.getKey();
    if (key == 'A') {
      if (menu.get_currentScreen() != &screen_1) {
        menu.previous_screen();
      }
    } else if (key == 'D') {
      if (menu.get_currentScreen() != &screen_5) {
        menu.next_screen();
      }
    } else if (key == '#') {
      menu.switch_focus();
    } else if (key == '*') {
      lastScreen = menu.get_currentScreen();
      menu.call_function(1);
    }
    if(mon == 1){
      ultimaTransicion = transiciones::input_Temp;
      break;
    }
  }
}

void Monitoreo()
{
  Serial.println("Estado Monitoreo");
  boolean  bandera=false;
  verificacionTemp();
  verificacionLuz();
  if (temperatura > 35) {
    ultimaTransicion=transiciones::input_Time;
  }else if(temperatura<35){
    if(tecla()){
      bandera=true;
    }else{
      ultimaTransicion=transiciones::input_Temp; 
    }
  }
  if (bandera == true) {
    ultimaTransicion=transiciones::input_Boton;
    mon=0;
  }
}

boolean tecla(){
  boolean bandera=false;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Continuar PressB");
  lcd.setCursor(0, 1);
  lcd.print("Ir Menu PressC");

  while ((key = keypad.getKey()) != 'C' && key == NO_KEY) { 
  }   
  if(key== 'C' ){
    bandera= true;
  }
  return bandera;
}


void Alarma()
{
  Serial.println("Estado Alarma");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PROM.TEMP:");
  lcd.setCursor(11, 0);
  lcd.print(temperatura);
  lcd.setCursor(0, 1);
  lcd.print("PROM.LUZ:");
  lcd.setCursor(10, 1);
  lcd.print(luz);
  sonidoBuzzer();
  ultimaTransicion=transiciones::input_Temp;
}

void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  lcd.clear();
  menu.add_screen(screen_1);
  menu.add_screen(screen_2);
  menu.add_screen(screen_3);
  menu.add_screen(screen_4);
  menu.add_screen(screen_5);
  leerMemoria();
  ultimoEstado = estado_contrasenia;
  Seguridad();
  if(ultimaTransicion == transiciones::input_Clave){
    opcionesMenu();
  }
}
void loop() 
{
  opcionesMenu();
  actualizarMaquina();
}

// Actualiza el estado de la maquina
void actualizarMaquina()
{
  switch (ultimoEstado)
  {
    case estado_contrasenia: estadoSeguridad(); break;
    case estado_menu: estadoMenuConfig(); break;
    case estado_monitoreo: estadoMonitoreo(); break;
    case estado_alarma: estadoAlarma(); break;
  }
}

// Funcion que cambia el estado y dispara las transiciones
void cambioDeEstado(int newState)
{
  ultimoEstado = newState;
  switch (ultimoEstado)
  {
    case estados::estado_contrasenia: Seguridad();   break;
    case estados::estado_menu: MenuConfig();   break;
    case estados::estado_monitoreo: Monitoreo();   break;
    case estados::estado_alarma: Alarma();   break;
    default: break;
  }
}

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

void opcionesMenu(){
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
    EEPROM.write(0, 255);
    EEPROM.write(1, 255);
    EEPROM.put(2, -1);
    EEPROM.put(3, -1);
    menu.change_screen(lastScreen);
  });
  screen_5_line_1.attach_function(1, []() {
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
    EEPROM.write(0, 255);
    EEPROM.write(1, 255);
    EEPROM.put(2, -1);
    EEPROM.put(3, -1);
    menu.change_screen(lastScreen);
  });
  screen_5_line_2.attach_function(1, []() {
    mon = 1;
  });
  menu.update();
}

void leerMemoria(){
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
}

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

void leerTemperatura(){
  float suma=0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("Temperatura: ");
  lcd.setCursor(0, 1);
  for (int a = 0; a < 10; a++) {
    int analogValue = analogRead(SENSOR_TEMP);
    float celcius =  1 / (log(1 / (1023. / analogValue - 1)) / BETA + 1.0 / 298.15) - 273.15;
    tiempoEspera(500);
    lcd.setCursor(0, 1);
    lcd.println(celcius);
    Serial.println(celcius);
    tiempoEspera(200);
    suma += celcius;
  }
  temperatura = suma / 10;
}


void leerLuz() {
  float suma = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("Luz: ");
  for (int a = 0; a < 10; a++) {
    int analogValue = analogRead(SENSOR_LUZ);
    lcd.setCursor(0, 1);
    Serial.println(analogValue );
    lcd.println(analogValue );
    tiempoEspera(700);
    suma += analogValue ;
  }
  luz = suma / 10;
}


void verificacionTemp() {
  leerTemperatura();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("Prom Temperatura:");
  lcd.setCursor(0, 1);
  lcd.println(temperatura);
  if ( temperatura >umbrTempHigh) {
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, HIGH);
  } else if (  temperatura <  umbrTempLow ) {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_BLUE, HIGH);

  } else {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_GREEN, HIGH);
  }
  tiempoEspera(3000);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_GREEN, LOW);
}

void verificacionLuz() {
  leerLuz();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("Prom Luz:");
  lcd.setCursor(0, 1);
  lcd.println(luz);
  if ( luz > umbrLuzHigh) {
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, HIGH);
  } else if (  luz  < umbrLuzLow ) {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_BLUE, HIGH);
  } else {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_GREEN, HIGH);
  }
   tiempoEspera(3000);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_GREEN, LOW);
}


void sonidoBuzzer() {
  for (int i = 0; i < 10; i++) {			
    int duracion = 500 / duraciones[i];		
    tone(BUZZER_PASIVO, melodia[i], duracion);		
    tiempoEspera(1000);				
    noTone(BUZZER_PASIVO);			
  }
}







