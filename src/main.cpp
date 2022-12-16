#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define WIFI_SSID "2.4G-Vectra-WiFi-8FFD5A"
#define WIFI_PASSWORD "9364C817ABBE3E49"

Adafruit_BME280 Sensor;

bool isConnected=0;
IPAddress HostIP(192, 168, 0, 200);
IPAddress SubnetMask(255, 255, 255, 0);
IPAddress Gateway(192, 168, 0, 1);
IPAddress DNS(8, 8, 8, 8); 

void WiFiClientSetup(IPAddress HostIP, IPAddress SubnetMask, IPAddress Gateway, IPAddress DNS)
{
  WiFi.config(HostIP, Gateway, SubnetMask, DNS);
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("----------");
  Serial.println("Connecting to WiFi, please wait...");
  
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.println("Connecting...");
    delay(1000);
  }
   
  Serial.println("WiFi Connected!");
  Serial.print("IPv4 address: ");
  Serial.print(WiFi.localIP());
  Serial.print("\r\n");

  Serial.print("Subnet mask: ");
  Serial.print(WiFi.subnetMask());
  Serial.print("\r\n");

  Serial.print("Local Gateway: ");
  Serial.print(WiFi.gatewayIP());
  Serial.print("\r\n");

  Serial.print("DNS server: ");
  Serial.print(WiFi.dnsIP());
  Serial.print("\r\n");
  Serial.println("----------");
  isConnected=true;
}

void WiFiClientReconnect()
{
  if(WiFi.status()!=WL_CONNECTED)
  {
    isConnected=0;
    uint8_t tempCounter=0;
    Serial.println("----------");
    Serial.println("Lost connection. Reestablishing...");
    while(WiFi.status()!=WL_CONNECTED)
    {
      tempCounter++;
      delay(1000);
      Serial.print(".");
      if(tempCounter==10)
      {
        Serial.print("\r\n");
        tempCounter=0;
      }
    }
  }
  else if(WiFi.status()==WL_CONNECTED && !isConnected)
  {
    Serial.print("\r\n");
    Serial.println("Connection reestablished!");
    Serial.println("----------");
    isConnected=true;
  }
}

void printValues() {
  Serial.print("Temperature = ");
  Serial.print(Sensor.readTemperature());
  Serial.println(" *C");
  
  // Convert temperature to Fahrenheit
  /*Serial.print("Temperature = ");
  Serial.print(1.8 * Sensor.readTemperature() + 32);
  Serial.println(" *F");*/
  
  Serial.print("Pressure = ");
  Serial.print(Sensor.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(Sensor.readAltitude(1013.25));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(Sensor.readHumidity());
  Serial.println(" %");

  Serial.println();
  delay(1000);
}

void setup() {
  Serial.begin(921600);
  Serial.println("----------");
  Serial.println("Serial initialized!");
  Serial.println("----------");
  WiFiClientSetup(HostIP, SubnetMask, Gateway, DNS);

  Serial.println("----------");
  Serial.println("BME280 initializing...");
  if(Sensor.begin(0x76))
  {
    Serial.println("BME280 initialized!");
  }
  else
  {
    Serial.println("BME280 not initialized!");
  }
  Serial.println("----------");
}

void loop() {
  WiFiClientReconnect();
  printValues();
  
}