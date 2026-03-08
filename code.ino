/*************************************************
 * Smart Onion Storage System - ESP32
 * Sensors: DHT22, BH1750, HX711
 * Actuators: Fan, Humidifier (via MOSFET)
 *************************************************/

#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>
#include "HX711.h"

/* ---------- Pin Definitions ---------- */
#define DHTPIN 4
#define DHTTYPE DHT22

#define HX711_DOUT 19
#define HX711_SCK  18

#define FAN_PIN 26
#define HUMIDIFIER_PIN 27

/* ---------- Objects ---------- */
DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter;
HX711 scale;

/* ---------- Thresholds ---------- */
float TEMP_THRESHOLD = 30.0;     // °C
float HUM_THRESHOLD  = 65.0;     // %

/* ---------- Setup ---------- */
void setup() {
  Serial.begin(115200);

  pinMode(FAN_PIN, OUTPUT);
  pinMode(HUMIDIFIER_PIN, OUTPUT);

  digitalWrite(FAN_PIN, LOW);
  digitalWrite(HUMIDIFIER_PIN, LOW);

  // Start sensors
  dht.begin();

  Wire.begin(21, 22); // SDA, SCL
  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);

  scale.begin(HX711_DOUT, HX711_SCK);
  scale.set_scale();     // Calibrate later
  scale.tare();          // Zero the scale

  Serial.println("Smart Onion Storage System Started");
}

/* ---------- Main Loop ---------- */
void loop() {
  /* ---- Read DHT22 ---- */
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Celsius

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  /* ---- Read Light ---- */
  float lux = lightMeter.readLightLevel();

  /* ---- Read Load Cell ---- */
  float weight = scale.get_units(5); // average of 5 readings

  /* ---- Control Fan ---- */
  if (temperature > TEMP_THRESHOLD) {
    digitalWrite(FAN_PIN, HIGH);
  } else {
    digitalWrite(FAN_PIN, LOW);
  }

  /* ---- Control Humidifier ---- */
  if (humidity < HUM_THRESHOLD) {
    digitalWrite(HUMIDIFIER_PIN, HIGH);
  } else {
    digitalWrite(HUMIDIFIER_PIN, LOW);
  }

  /* ---- Serial Monitor Output ---- */
  Serial.println("-------- System Status --------");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");

  Serial.print("Weight: ");
  Serial.print(weight);
  Serial.println(" units");

  Serial.print("Fan: ");
  Serial.println(digitalRead(FAN_PIN) ? "ON" : "OFF");

  Serial.print("Humidifier: ");
  Serial.println(digitalRead(HUMIDIFIER_PIN) ? "ON" : "OFF");

  Serial.println("--------------------------------\n");

  delay(2000);
}
