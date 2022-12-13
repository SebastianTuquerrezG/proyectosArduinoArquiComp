/**
 * @file Entregable.ino
 * @author Sebastian Tuquerrez, Brayan Majin, Freider Escobar
 * @title Tareas Asincronicas
 * @brief Uso de tareas asincronicas, para guardar temperatura y humedad de un sensor DHT y mostrarlo en una pantalla LCD, y 
 *      darle comportamiento a un LED con la variacion de la temperatura
 * @version 0.1
 * @date 2022-12-13
 */

#include <LiquidCrystal.h>
#include "DHTStable.h"
#include "AsyncTaskLib.h"

/**
 * @brief constantes globales, pines de los leds, el sensor DHT y la pantalla LCD
 *      clase DHT para el uso de la libreria DHTStable.h
 *      variables globales para temperatura y humedad
 */
#define LED_GREEN 9
#define LED_BLUE 8
#define LED_RED 10
#define DHT11_PIN 6  
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

DHTStable DHT;
float humy, temp;

/**
 * @brief errors informa de algun posible error generado por el DHT 
 * @param chk codigo de estado que devuelve cuando se lee el DHT
 * @return mensaje de error en el monitor serial 
 */
void errors(int chk){
    switch (chk)
    {
        case DHTLIB_OK:  
            Serial.print("OK,\t"); 
        break;
        case DHTLIB_ERROR_CHECKSUM: 
            Serial.print("Checksum error,\t"); 
        break;
        case DHTLIB_ERROR_TIMEOUT: 
            Serial.print("Time out error,\t"); 
        break;
        default: 
            Serial.print("Unknown error,\t"); 
        break;
    }
}

/**
 * @brief asyncTempe cada segundo se repite, lee la temperatura del sensor DHT, verifica errores y guarda la temperatura en temp
 *      y lo muestra en el monitor serial
 */
AsyncTask asyncTempe(1000, true, []() { int chk = DHT.read11(DHT11_PIN);
                                        errors(chk);
                                        temp = DHT.getTemperature(); 
                                        Serial.println("Temp: ");
                                        Serial.println(temp);   });

/**
 * @brief asyncHumid cada 0.5 segundos se repite, lee la humedad del sensor DHT, verifica errores y guarda la humedad en temp
 *      y lo muestra en el monitor serial
 */
AsyncTask asyncHumid(500, true, []() {  int chk = DHT.read11(DHT11_PIN);
                                        errors(chk);
                                        humy = DHT.getHumidity();                                        
                                        Serial.println("Humd: ");
                                        Serial.println(humy);  });

/**
 * @brief asyncShowLCD cada 1.1 segundos se repite, muestra en la pantalla LCD los valores guardados de temperatura y humedad
 */
AsyncTask asyncShowLCD(1100, true, [](){lcd.clear();
                                        lcd.setCursor(0, 0);
                                        lcd.print("HUMEDAD: ");
                                        lcd.print(humy);
                                        lcd.setCursor(0,1);
                                        lcd.print("TEMP: ");
                                        lcd.print(temp); });

/**
 * @brief asyncLEDTemp cada 1 segundo se repite, controla el comportamiento del led segun la temperatura
 */
AsyncTask asyncLEDTemp(1000, true, [](){if(temp >= 30){
                                            digitalWrite(LED_RED, HIGH);  
                                        }else if(temp <= 29 && temp >= 10){
                                            digitalWrite(LED_BLUE, HIGH); 
                                        }else{
                                            digitalWrite(LED_GREEN, HIGH);  
                                        } 
                                        digitalWrite(LED_RED, LOW); 
                                        digitalWrite(LED_BLUE, LOW); 
                                        digitalWrite(LED_GREEN, LOW);});                                      

void setup()
{
    lcd.begin(16, 2);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    Serial.begin(9600);
    asyncTempe.Start();
    asyncHumid.Start();
    asyncLEDTemp.Start();
    asyncShowLCD.Start();
}

void loop()
{
    asyncTempe.Update();
    asyncHumid.Update();
    asyncLEDTemp.Update();
    asyncShowLCD.Update();
}