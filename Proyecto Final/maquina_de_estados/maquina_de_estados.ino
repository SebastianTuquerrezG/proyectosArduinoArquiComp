/**
 * @file maquina_de_estados.ino
 * @author Sebastian Tuquerrez, Brayan Majin, Freider Escobar
 * @title Proyecto Final Arquitectura Computacional
 * @brief Sistema de Gestión y Monitoreo de Temperatura y Luz, con maquinas de estados en Arduino
 * @version 1.0
 * @date 2023-01-30
 *
 * @section description Descripcion
 * Este proyecto está echo con el objetivo de utilizar sensores para el monitoreo de la temperatura y utilizar máquinas de Mealy
 * o automatas para realizar funciones de seguridad, configuración, monitoreo y alarmas en un sistema basado en Arduino.
 *
 * @copyright Copyright Unicauca (c) 2022
 */

#pragma region libraries
  #include <Keypad.h>
  #include <LiquidCrystal.h>
  #include <LiquidMenu.h>
  #include <EEPROM.h>
#pragma endregion libraries

/**
 * @defgroup set_pins
 * @brief Seccion para definir los pines que se van a usar del arduino para el uso del LED RGB,
 * el buzzer pasivo y los sensores de temperatura y la fotocelda.
 */
#pragma region set_pins 
  #define LED_BLUE 3
  #define LED_RED 2
  #define LED_GREEN 4
  #define BUZZER_PASIVO  6
  #define SENSOR_TEMP 5
  #define SENSOR_LUZ 0
#pragma endregion set_pins

/**
 * @defgroup melodia_alarma
 * @brief Esta seccion de codigo define las notas que van a sonar al momento de que entre en un estado de alarma
 * y se incluyen todas estas notas en un array para el facil uso con la funcion tone() y se definen las duraciones que van
 * a tener cada una de las notas, entonando la melodia de Happy Birthday.
 */
#pragma region melodia_alarma
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
#pragma end region melodia_alarmas

/**
 * @defgroup keypad_set
 * @brief Esta region define la configuracion del teclado(keypad 4x4), cuantas filas y columnas, tendra, significado que cada uno de los 
 * espacios tendra en keys[][], los pines del arduino que va a utilizar y la instancion del constructor de la libreria keypad.h 
 * key una variable que sera la que guarde lo que se ingresa por el teclado.
 */
#pragma region keypad_set
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
  char key;
#pragma endregion 

/**
 * @defgroup lcd_set
 * @brief Esta region define la configuracion del lcd, los pines que va a utilizar y la instancion del constructor de la libreria LiquidCrystal.h 
 * lcd una variable que sera la que guarde lo que se ingresa por el teclado.
 */
#pragma region lcd_set
  const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 8, d7 = 7;
  LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#pragma endregion lcd_set
/**
 * @defgroup variables
 * @brief Esta region define las variables que se van a utilizar en el programa, como las variables de los umbrales,
 * los valores por defecto de la temperatura y la luz su maximo y minimo que pueden alcanzar, y son editables en el estado de menu,
 * TEST_TEMP, variable que controla los rangos en el que puede estar la temperatura, y las variables tipo float que guardaran los valores
 * que detecte el sensor de temperatura y la fotocelda.
 */
#pragma region variables
  int umbrTempHigh , umbrTempLow , umbrLuzHigh , umbrLuzLow ;
  #define DEFAULT_TEMPHIGH 29
  #define DEFAULT_TEMPLOW 26
  #define DEFAULT_LUZHIGH 800
  #define DEFAULT_LUZLOW 300
  #define TEST_TEMP 20
  #define MAX_TEMP 125
  #define MAX_LIGTH 1023
  float temperatura = 0;
  float luz = 0;
  const float BETA = 3950;
#pragma endregion variables

/**
 * @defgroup secutiry_set
 * @brief Esta region define las variables que se van a utilizar en el programa, como las variables de la seguridad,
 * como lo son la contraseña, los intentos, y contadores los intentos, y variables para el cambio de maquina de estado de seguridad.
 */
#pragma region secutiry_set
  char password[4] = {'1','2','3','4'};
  char passwordAux[4];
  int trys = 1, cont = 0, iden = 0, cont2 = 0, mon = 0;
  unsigned long inicio = 0;
  unsigned long constante = 0;
#pragma endregion secutiry_set

/**
 * @defgroup variables_alarma
 * @brief Esta region define las variables que se van a utilizar en la alarma, y se define el ingreso al estado de alarma
 */
#pragma region variables_alarma
  float sinVal;
  int toneVal;
  boolean stateAlarm;
#pragma endregion variables_alarma

/**
 * @defgroup Screens_set
 * @brief Cadenas de texto para el menu, configuracion de cada una las pantallas que se van a mostrar en el menu con el constructor
 * LiquidScreen y definir las lineas que se mostraran en cada pantalla con LiquidLine, y instancion el constructor de la libreria LiquidMenu.
 */
#pragma region Screens_set
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

/**
 * @defgroup maquinas_de_estado_funciones
 * @brief Esta region contiene las estructura de los estados, su definicion de logica y de espacio en memoria, y de las transiciones que
 * pueden haber entre maquinas de estado.
 * Y la definicion de las funciones que ejecutaran las transiciones que hara cada maquina de estado segun la entrada.
 */
#pragma region maquinas_de_estado_funciones
/**
 * @struct estados
 * @brief Definicion de los estados en los que se puede encontrar el sistema y el espacio en memoria que ocupara
 * @section description Descripcion
 * Esta definicion representa los diferentes estados en los que se puede encontrar el sistema.
 * - estado_contrasenia: Estado inicial del sistema, en el que se realizan las configuraciones iniciales y se solicita la contraseña.
 * - estado_menu: Estado en el que se permite al usuario configurar las opciones del sistema y observar que se tiene guardado en cada umbral.
 * - estado_monitoreo: Estado en el que el sistema se encarga de monitorear continuamente los sensores.
 * - estado_alarma: Estado en el que se activa la alarma debido a una condición determinada.
 */
enum estados {
  estado_contrasenia,
  estado_menu,
  estado_monitoreo,
  estado_alarma
};

/**
 * @struct transiciones
 * @brief Definicion de las entradas a las maquinas de estados y su espacio en memoria
 * @section description Descripcion
 * Esta enumeración representa las diferentes entradas a la máquina de estados.
 * - Unknown: Entrada no reconocida.
 * - input_Clave: Entrada para cambiar al estado de configuración.
 * - input_Temp: Entrada para cambiar al estado de monitoreo.
 * - input_Boton: Entrada que indica que se ha ingresado un valor y pasa a estado de menu.
 * - input_Time: Entrada que indica que el tiempo de monitoreo ha terminado y entra en estado de alarma.
 */
enum transiciones {
  Unknown,
  input_Clave,
  input_Temp,
  input_Boton,
  input_Time
};

/**
 * @defgroup estate_of_system
 * @brief define variables del tipo de estructura estados y transiciones, donde se guarda el ultimo estado que estuvo la maquina
 * y la ultima transicion que hizo, un auxiliar reset de la placa 
 */
#pragma region estate_of_system
estados ultimoEstado;
transiciones ultimaTransicion;
void (*resetSoftware)(void) = 0;
#pragma endregion estate_of_system

/**
 * @brief estadoSeguridad
 * Este estado monitorea, que mientras no se ingrese la clave correcta o no haya una entrada conocida, se va a mantener en el estado
 * contrasenia, si se ingresa la clave correcta, cambia de estado a estado menu.
 */
void estadoSeguridad() {
  if (ultimaTransicion == transiciones::input_Clave)
    cambioDeEstado(estados::estado_menu);
  if (ultimaTransicion == transiciones::Unknown)
    cambioDeEstado(estados::estado_contrasenia);
}

/**
 * @brief estadoMenuConfig
 * Este estado monitorea que al detectar una entrada de temperatura, cambia al estado de monitoreo
 */
void estadoMenuConfig() {
  if (ultimaTransicion == transiciones::input_Temp)
    cambioDeEstado(estados::estado_monitoreo);
}

/**
 * @brief estadoMonitoreo
 * El estado de monitoreo, controla que al detectar cierto tiempo en el de temperatura no regular, cambia al estado de alarma,
 * si se ingresa algo por teclado, cambia al estado de menu y si la temperatura esta bien, se queda en estado de monitoreo
 */
void estadoMonitoreo() {
  if (ultimaTransicion == transiciones::input_Time)
    cambioDeEstado(estados::estado_alarma);
  if (ultimaTransicion == transiciones::input_Boton)
    cambioDeEstado(estados::estado_menu);
  if(ultimaTransicion == transiciones::input_Temp)
    cambioDeEstado(estados::estado_monitoreo);
}

/**
 * @brief estadoAlarma 
 * Este estado monitorea que al detectar que la temperatura esta bien regulada, se cambia al estado de monitoreo
 */
void estadoAlarma() {
  if (ultimaTransicion == transiciones::input_Temp)
    cambioDeEstado(estados::estado_monitoreo);
}
#pragma endregion maquinas_de_estado_funciones

/**
 * @brief alarmaBloqueo
 * Genera una serie de tonos con frecuencias que varían siguiendo una forma de onda senoidal.
 */
void alarmaBloqueo() {
  for (int i=0; i <180; i++){
    sinVal = (sin(i*(3.1412/180)));
    toneVal = 2000+(int(sinVal*1000));
    tone(6, toneVal);
    tiempoEspera(2);
  }
}

/**
 * @brief tiempoEspera
 * Función que realiza una espera por un tiempo determinado,
 * utilizando la función millis() de Arduino.
 *
 * @param tiempo Tiempo de espera en milisegundos
 */
void tiempoEspera(int tiempo) {
  inicio = millis();
  constante = millis();
  while ((constante-inicio) <= (tiempo) ) {
    constante= millis();
  }
}

/**
 * @brief seguridad
 * Función de seguridad para proteger el sistema.
 * @section description Descripcion
 * Esta función permite al usuario ingresar una clave de seguridad y verifica si es la clave correcta.
 * Si la clave es incorrecta, se muestra el número de intentos restantes y se incrementa el contador de intentos.
 * Si se alcanzan 4 intentos fallidos, el sistema se bloqueará y se activará una alarma.
 * La función también se encarga de reiniciar el sistema en caso de bloqueo.
 */
void Seguridad() {
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

/**
 * @brief MenuConfig
 * Función encargada de mostrar el menú de configuración en la pantalla. 
 * 
 * @section description Descripcion
 * se utiliza la librería "Keypad" para leer la entrada del teclado y realizar acciones en el menú.
 * Se pueden navegar entre las diferentes pantallas definidas con la libreria "LiquidMenu", utilizando las teclas "A" para ir a la pantalla
 * anterior y "D" para ir a la siguiente.
 * Al presionar "#" se alterna el enfoque entre los elementos del menú y al presionar "*" se activa la función asociada al elemento enfocado.
 * Si se cambia al estado de monitoreo de temperatura, la función sale y se devuelve el control al sistema.
 *
 * @note La función se quedará en un bucle infinito hasta que se realice una acción que requiera salir de este estado.
 */
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

/**
 * @brief Monitoreo
 * Función de Monitoreo
 * @section description Descripcion
 * La función se encarga de verificar la temperatura y la luz en el ambiente.
 * Si la temperatura es mayor a 35 grados, se cambia a la transición `input_Time`.
 * Si la temperatura es menor a 35 grados y se presiona una tecla, se cambia a la transición `input_Boton`.
 * De lo contrario, se mantiene en la transición `input_Temp`.
 * @var bandera
 * @brief bandera que indica si se presionó una tecla o no
 */
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

/**
 * @brief tecla
 * Función para verificar la presión de una tecla en el keypad
 *
 * Esta función muestra un mensaje en el LCD pidiendo la acción del usuario
 * y espera la presión de una tecla en el keypad. Si la tecla presionada es "C",
 * la función regresa `true`, de lo contrario regresa `false`.
 *
 * @return `true` si se presionó la tecla "C", `false` en otro caso
 */
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

/**
 * @brief Funcion que muestra la alarma
 *
 * La funcion `Alarma` se encarga de mostrar en la pantalla LCD el valor promedio de la temperatura y la luz y además
 * ejecuta la función `sonidoBuzzer` para emitir un sonido de alarma. Al finalizar su ejecución, la variable `ultimaTransicion`
 * es actualizada a `transiciones::input_Temp`.
 *
 * @warning Esta función se ejecuta cuando se cumple alguna condición que provoca la alarma.
 *
 * @see sonidoBuzzer
 * @see temperatura
 * @see luz
 * @see ultimaTransicion
 */
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

/**
 * @brief actualizarMaquina
 * Funcion que gestiona el estado del sistema 
 * Dependiento del estado que este guardo en "UltimoEstado", va entrar al switch y ejecutara la funcion que corresponda a dicho estado.
 */
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
/**
 * @brief cambioEstado
 * Funcion que cambia de estado segun la posicion de la estructura de las maquinas de estado * 
 * @param newState recibe la posicion a la cual del estado a la que el sistema desea cambiar.
 */
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

/**
 * @brief readKeypad
 * Funcion que lee el keypad y regresa el valor ingresado por el usuario, y lo muestra en pantalla si la tecla es un numero entre 0 y 9
 * @return int regresa el numero ingreso por teclado pero en numero ya que ingresa un numero tipo char.
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
 * @brief opcionesMenu
 * Esta funcion añade las funcionalidad para cada submenu o linea creada con la funcion LiquidLine de la libreria LiquidMenu,
 * e interactua con la memoria EEPROM volver a los valores principales antes de hacer alguna configuracion.
 * @see editar_valor
 */
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

/**
 * @brief leerMemoria
 * Funcion que lee la memoria EEPROM cada vez que inicia el sistema, para setear las variables de los umbrales y llenar estas variables con 
 * lo que haya guardado en la memoria EEPROM.
 */
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

/**
 * @brief editar_valor
 * Funcion que permite editar los valores de las variables de los Umbrales
 * Esta funcion editar los valores que estan guardados en las variables por defecto de los umbrales, y depende de en que pantalla o
 * submenu se encuentre editando.
 * Al editar los valores de los umbrales los guarda en la memoria EEPROM si en algun momento la placa llega a tener un reset, los valores o 
 * acciones que hayamos configurado, no se borren.
 * @param titulo Texto que se mostrara en la parte superior del lcd al estar editando un valor
 * @param varimp valor del umbral de dicho submenu que se esta editando
 * @param posicion la pantalla en la que se encuentra en el momento
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

/**
 * @brief leerTemperatura
 * Funcion para el monitoreo del Umbral de Temperatura en el menu LCD cuando esta en estado de monitoreo
 * Esta funcion se actualiza con espera, gracias a la funcion "tiempoEspera", mostrando los valores que detecte el sensor DHT11.
 */
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

/**
 * @brief leerLuz
 * Funcion para el monitoreo del Umbral de luz en el menu LCD cuando esta en estado de monitoreo
 * Esta funcion se actualiza cada 0.7 segundos gracias a la funcion "tiempoEspera", mostrando los valores que detecte la fotocelda.
 * @see tiempoEspera 
 */
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

/**
 * @brief verificacionTemp
 * Funcion que verifica el estado de la luz con la funcion "leerluz", si esta por encima del umbral de luz alto, se enciende el led rojo,
 * si esta por debajo del umbral de luz bajo, se enciende el led azul, si esta en el rango de los umbrales, se enciende el led verde.
 * @see leerTemperatura
 */
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

/**
 * @brief verificacionLuz
 * Funcion que verifica el estado de la luz con la funcion "leerluz", si esta por encima del umbral de luz alto, se enciende el led rojo, 
 * si esta por debajo del umbral de luz bajo, se enciende el led azul, si esta en el rango de los umbrales, se enciende el led verde.
 * @see leerluz
 */
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

/**
 * @brief sonidoBuzzer
 * Funcion que reproduce un sonido de alarma
 * Reproduce la melodia que se definio en @see melodia_alarma con la funcion tone() y hace uso de la funcion "tiempoEspera" para esperar 1
 * segundo despues de cada reproduccion y para la reproduccion con la funcion noTone().
 * @see tiempoEspera
 */
void sonidoBuzzer() {
  for (int i = 0; i < 10; i++) {			
    int duracion = 500 / duraciones[i];		
    tone(BUZZER_PASIVO, melodia[i], duracion);		
    tiempoEspera(1000);				
    noTone(BUZZER_PASIVO);			
  }
}