/*
  Reading CO2, humidity and temperature from the SCD30
  This example prints the current CO2 level, relative humidity, and temperature in C. and sends a red/yellow/green Signal on the M5 Matrix
  
  Hardware Connections:
  Attach RedBoard to computer using a USB cable.
  Connect SCD30 to RedBoard using Qwiic cable.
  Open Serial Monitor at 115200 baud.
*/
#define M5ATOM
#define SCD30_CONNECTED
#define SCAN_REPETITION_TIME 90000  // Wie oft gemessen wird ier alle 90000ms = 90 sec = 1.5min
//Minimale Zeit zwischen zwei Messungen 2 sec 

#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD30
SCD30 airSensor;

#ifdef M5ATOM
  #include <FastLED.h>
   // Define the array of leds
  #define NUM_LEDS 25 
  #define DATA_PIN 27
  FASTLED_USING_NAMESPACE
  #define MAX_POWER_MILLIAMPS 500
  CRGB leds[NUM_LEDS];

  #define sda 26 ///* I2C Pin Definition */
  #define scl 32 ///* I2C Pin Definition */
#endif

long lastMsg = 0;
float scd30_CO2 = 0;
float scd30_temp = 0;
float scd30_feuchte = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("SCD30 Example");
  Wire.begin(sda, scl);
  LEDInit();
  if (airSensor.begin() == false)
  {
    Serial.println("Air sensor not detected. Please check if your Sensor is connected...");
    Serial.println("Co² sensor nicht erkannt. Bitte Verkabelung prüfen...");
    while (1)
      ;
  }
  //The SCD30 has data ready every two seconds
}

void loop()
{ 
  long now = millis();
        if (now - lastMsg > SCAN_REPETITION_TIME) {
          Serial.print("Time: ");Serial.println(lastMsg);
          lastMsg = now;
          #ifdef SCD30_CONNECTED
            scd30Messung(); 
          #endif  
          #ifdef M5ATOM
            LEDUpdate();          
          #endif
        }
}

void scd30Messung(void){
  if (airSensor.dataAvailable())
  {
    Serial.print("co2(ppm):"); Serial.print(airSensor.getCO2());
    Serial.print(" Temperatur(C):"); Serial.print(airSensor.getTemperature(), 1);
    Serial.print(" Feuchte(%):"); Serial.println(airSensor.getHumidity(), 1);
    scd30_CO2 = (int)airSensor.getCO2();
    scd30_temp = (int)airSensor.getTemperature();
    scd30_feuchte = (int)airSensor.getHumidity();
  }
  else{
    Serial.println("Warten auf neue Daten");
  }
}


#ifdef M5ATOM
  /* M5 ATOM LED MATRIX ansteuern */
  void LEDUpdate(void){
    for(int i = 0; i < NUM_LEDS; i++) {
      if (scd30_CO2 < 700)      leds[i] = CRGB::Green;
      else if (scd30_CO2 < 1300) leds[i] = CRGB::Yellow;
      else                      leds[i] = CRGB::Red;
      FastLED.show(); 
    }
  }
  void LEDInit(void){
    LEDS.addLeds<WS2812,DATA_PIN,GRB>(leds,NUM_LEDS);
    LEDS.setBrightness(54);
    for(int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Blue;
    }
    FastLED.show(); 
  }
#endif
