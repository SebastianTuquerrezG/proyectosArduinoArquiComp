#include <Keypad.h>
#include <LiquidCrystal.h>
#include "DHTStable.h"
#include "AsyncTaskLib.h"

#define NOTE_B0  31 // notas y frecuencias
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

int melodia[] = {   // array con las notas de la melodia
NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_F4, NOTE_E4, NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4, NOTE_C4, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_AS4, NOTE_AS4, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_F4};

int duraciones[] = {     // array con la duracion de cada nota
8, 8, 4, 4, 4, 2, 8, 8, 4, 4, 4, 2, 8, 8, 4, 4, 4, 4, 4, 8, 8, 4, 4, 4, 2};

const byte ROWS = 4;
//four rows
const byte COLS = 4;
//four columns
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
byte rowPins[ROWS] = {51, 49, 47, 45};
//connect to the row pinouts of the keypad
byte colPins[COLS] = {43, 41, 39, 37};
//connect to the column pinouts of the keypad
char password[4] = "1234";
char cantidad[4];
char key;
int trys = 1, cont = 0;
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
#define LED_BLUE 8
#define LED_RED 9
#define LED_GREEN 10
#define BUZZER 7
#define DHT11_PIN 6
const int rs = 12, en = 11, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

AsyncTask asyncGetContra(1000, true, [](){  key = keypad.getKey(); });

AsyncTask ascynCompararContra(1000, true, [](){
                                if (key != NO_KEY)
                                {
                                    cantidad[cont] = key;
                                    Serial.println(cantidad[cont]);
                                    lcd.setCursor(cont, 0);
                                    lcd.print(cantidad[cont]);
                                    cont++;
                                    if (cont == 4)
                                    {
                                        if (strncmp(password, cantidad, 4) == 0)
                                        {
                                            Serial.println("Contra correcta");
                                            digitalWrite(LED_GREEN, HIGH);
                                            lcd.clear();
                                            lcd.setCursor(0, 0);
                                            lcd.print("Correcto");
                                            delay(3000);
                                            trys = 1;
                                            lcd.clear();
                                        }
                                        else
                                        {
                                            Serial.println("Contra incorrecta");
                                            lcd.clear();
                                            lcd.setCursor(0, 0);
                                            lcd.print("Incorrecto");
                                            lcd.setCursor(0, 1);
                                            lcd.print(trys);
                                            trys++;
                                            if (trys == 4)
                                            {
                                                digitalWrite(LED_RED, HIGH);
                                                digitalWrite(BUZZER, HIGH);
                                                lcd.clear();
                                                lcd.setCursor(0, 0);
                                                lcd.print("System Block");
                                                for (int i = 0; i < 25; i++)
                                                {
                                                    // bucle repite 25 veces
                                                    int duracion = 1000 / duraciones[i];
                                                    // duracion de la nota en milisegundos
                                                    tone(BUZZER, melodia[i], duracion); // ejecuta el tono con la duracion
                                                    int pausa = duracion * 1.30;
                                                    // calcula pausa
                                                    delay(pausa);
                                                    // demora con valor de pausa
                                                    noTone(BUZZER);
                                                    // detiene reproduccion de tono         
                                                }
                                                trys = 1;
                                                lcd.clear();
                                            }
                                            else
                                            {
                                                digitalWrite(LED_BLUE, HIGH);
                                                delay(500);
                                                lcd.clear();
                                                lcd.setCursor(0, 0);
                                            }
                                        }
                                        digitalWrite(LED_GREEN, LOW);
                                        digitalWrite(LED_BLUE, LOW);
                                        digitalWrite(LED_RED, LOW);
                                        digitalWrite(BUZZER, LOW);
                                        cont = 0;
                                    }
                                }
                            });

void setup()
{
    Serial.begin(9600);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    lcd.begin(16, 2);
    pinMode(BUZZER, OUTPUT);
    digitalWrite(BUZZER, LOW);
}

void loop()
{
    char key = keypad.getKey();

    if (key != NO_KEY)
    {
        cantidad[cont] = key;
        Serial.println(cantidad[cont]);
        lcd.setCursor(cont, 0);
        lcd.print(cantidad[cont]);
        cont++;
        if (cont == 4)
        {
            if (strncmp(password, cantidad, 4) == 0)
            {
                Serial.println("Contra correcta");
                digitalWrite(LED_GREEN, HIGH);
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Correcto");
                delay(3000);
                trys = 1;
                lcd.clear();
            }
            else
            {
                Serial.println("Contra incorrecta");
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Incorrecto");
                lcd.setCursor(0, 1);
                lcd.print(trys);
                trys++;
                if (trys == 4)
                {
                    digitalWrite(LED_RED, HIGH);
                    digitalWrite(BUZZER, HIGH);
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("System Block");
                    for (int i = 0; i < 25; i++)
                    {
                        // bucle repite 25 veces
                        int duracion = 1000 / duraciones[i];
                        // duracion de la nota en milisegundos
                        tone(BUZZER, melodia[i], duracion); // ejecuta el tono con la duracion
                        int pausa = duracion * 1.30;
                        // calcula pausa
                        delay(pausa);
                        // demora con valor de pausa
                        noTone(BUZZER);
                        // detiene reproduccion de tono         
                    }
                    trys = 1;
                    lcd.clear();
                }
                else
                {
                    digitalWrite(LED_BLUE, HIGH);
                    delay(500);
                    lcd.clear();
                    lcd.setCursor(0, 0);
                }
            }
            digitalWrite(LED_GREEN, LOW);
            digitalWrite(LED_BLUE, LOW);
            digitalWrite(LED_RED, LOW);
            digitalWrite(BUZZER, LOW);
            cont = 0;
        }
    }
}
