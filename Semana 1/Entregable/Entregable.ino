#include <LiquidCrystal.h>
#include "DHTStable.h"
#include "AsyncTaskLib.h"

#define LED_GREEN 9
#define LED_BLUE 8
#define LED_RED 10

#define DHT11_PIN 6   
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
DHTStable DHT;

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
float humy, temp;
AsyncTask asyncTask1(1000, true, []() { int chk = DHT.read11(DHT11_PIN);
                                        errors(chk);
                                        temp = DHT.getTemperature(); 
                                        Serial.println("Temp: ");
                                        Serial.println(temp);   });
AsyncTask asyncTask2(500, true, []() {  int chk = DHT.read11(DHT11_PIN);
                                        errors(chk);
                                        humy = DHT.getHumidity();                                        
                                        Serial.println("Humd: ");
                                        Serial.println(humy);  });

void setup()
{
    lcd.begin(16, 2);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    asyncTask1.Start();
    asyncTask2.Start();
}

void loop()
{
    asyncTask1.Update();
    asyncTask2.Update();
    if(temp > 30){
        digitalWrite(LED_RED, HIGH);  
    }else if(temp < 29 && temp > 10){
        digitalWrite(LED_BLUE, HIGH); 
    }else{
        digitalWrite(LED_GREEN, HIGH);  
    } 
    digitalWrite(LED_RED, LOW); 
    digitalWrite(LED_BLUE, LOW); 
    digitalWrite(LED_GREEN, LOW);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("HUMEDAD: ");
    lcd.print(humy);
    lcd.setCursor(0,1);
    lcd.print("TEMP: ");
    lcd.print(temp);
    delay(500);
}

