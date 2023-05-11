// Install SHT31 sensor library and MKR WAN 1310 board library
// Install libraries MRK WAN 1310, SHT31 sensor. 
#include <MKRWAN.h>
#include <DFRobot_SHT3x.h>
#include <ArduinoLowPower.h>
#include "arduino_secrets.h"

LoRaModem modem;

String appEui = SECRET_APP_EUI;
String appKey = SECRET_APP_KEY;

DFRobot_SHT3x sht3x(&Wire,/*address=*/0x44,/*RST=*/4);
float temperature;
float humidity; 

void setup(){

  Serial.begin(112500);
  Serial.println("Starting the scetch");

  // Start the STH31 sensor
  sht3x.begin();
  
  //Connect to the LoRa gateway
    if (!modem.begin(EU868)) {
    Serial.println("Failed to start module");
    while (1) {}
  };

  int connected = modem.joinOTAA(appEui, appKey);
  if (!connected) {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    while (1) {}
  }
  // Set digital pins to input to reduce battery consumption
  // for (int i=0; i < 15; i++) {
  //   pinMode(i, INPUT_PULLUP);
  // }
  // pinMode(LED_BUILTIN, OUTPUT);

  // detach USB so chip is asleep
  //USBDevice.detach();

  delay(100); 
  Serial.println("  - Connected to TTN ");

}

void loop() {
  
  // Read the sensor values
  temperature = sht3x.getTemperatureC(); 
  humidity = sht3x.getHumidityRH();

  // Print the sensor values
  Serial.print("Temperature in °C: ");
  Serial.println(temperature);
  Serial.print("Humidity %RH: ");
  Serial.println(humidity); 

  // Send the sensor values
  Serial.println("  - Sending message ... ");
  //modem.setPort(3); 
  modem.beginPacket();  
  modem.write<float>(temperature);
  modem.write<float>(humidity);

  int error = modem.endPacket(true);
 
  if (error > 0) {
    Serial.println("  - Message sent correctly!");
  } else {
    Serial.println("Error sending message :(");
  }

  delay(1000);
  LowPower.deepSleep(300000);

}
