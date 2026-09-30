#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

// ### DHT module
#define DHTPIN 4  // Set the pin connected to the DHT11 data pin
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);
const long DHT_INTERVAL = 5000; 
unsigned long previousDthMillis = 0;

// ### termistor
#define analogPin  A0 //the thermistor attach to 
#define beta 3950 //the beta of the thermistor
#define resistance 10 //the value of the pull-up resistor

// ### LC Display module
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
bool backlightEnable = true;

// ### backlight button
const int BACKLIGHT_BUTTON_PIN = 2;    
int backlightButtonState = 0;         
int backlightButtonLastState = 0;    

// ### led indicator
const int LED_INDICATOR_PIN = 6;  

void setup()
{
  pinMode(BACKLIGHT_BUTTON_PIN, INPUT);
  pinMode(LED_INDICATOR_PIN, OUTPUT);

  dht.begin();

  lcd.init();                      // initialize the lcd 
  if (backlightEnable) {
    lcd.backlight();
  }

  Serial.begin(9600);
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousDthMillis >= DHT_INTERVAL) {
    digitalWrite(LED_INDICATOR_PIN, HIGH);
    previousDthMillis = currentMillis;         // save the last state
    
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
    float humidity = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float temperature = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    //read thermistor value
    long termistorValue = analogRead(analogPin);
    //the calculating formula of temperature
    float termistorTemperature = beta / (log((1025.0 * 10 / termistorValue - 10) / 10) + beta / 298.0) - 273.0;

    String temperatureString = "";
    temperatureString += (int)temperature;
    temperatureString += (char)223;
    temperatureString += "/";
    temperatureString += (int)termistorTemperature;
    temperatureString += (char)223;

    String humidityString = "";
    humidityString += (int)humidity;
    humidityString += "%";

    lcd.clear();

    lcd.print("Tmp");
    lcd.setCursor(13, 0);
    lcd.print("Hum");
    lcd.setCursor(0, 1);
    lcd.print(temperatureString);
    lcd.setCursor(13, 1);
    lcd.print(humidityString);

    digitalWrite(LED_INDICATOR_PIN, LOW);
  }

  backlightButtonState = digitalRead(BACKLIGHT_BUTTON_PIN);
  if (backlightButtonState != backlightButtonLastState) {
    if (backlightButtonState == HIGH) {
      backlightEnable = !backlightEnable;
      if (backlightEnable) {
        lcd.backlight();
      } else {
        lcd.noBacklight();
      }
    } 
    delay(50);
  }
  backlightButtonLastState = backlightButtonState;
}
