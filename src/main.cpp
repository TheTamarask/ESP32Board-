#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


#define WIFI_SSID "2.4G-Vectra-WiFi-8FFD5A"
//#define WIFI_SSID "Hotspot"
#define WIFI_PASSWORD "9364C817ABBE3E49"
//#define WIFI_PASSWORD "Hotspot1!"
#define API_KEY "PNHYZHY120FBZWPR"
#define DEEP_SLEEP_S_MULTIPLIER 1000000
#define DEEP_SLEEP_SECONDS 900
Adafruit_BME280 Sensor;

IPAddress HostIP(192, 168, 0, 200);
IPAddress SubnetMask(255, 255, 255, 0);
IPAddress Gateway(192, 168, 0, 1);
IPAddress DNS(8, 8, 8, 8); 
WiFiClient ThingspeakConnectionWrite;
IPAddress Thingspeak(184, 106, 153, 149); 
uint16_t ThingspeakPort(80);

/**
 * @brief Setup WiFi interface as client device
 *
 * Configure the WiFi interface in Station mode with provided IP address.
 * Sets the provided subnet and gateway  address, as wel as DNS server IP.
 *
 * @param HostIP  Station designated IP address
 * @param SubnetMask  Subnet mask for provided IP address
 * @param Gateway  default gateway address
 * @param DNS  Default DNS server address
 */
void WiFiClientSetup(IPAddress HostIP, IPAddress SubnetMask, IPAddress Gateway, IPAddress DNS)
{
  //WiFi.config(HostIP, Gateway, SubnetMask, DNS);
  WiFi.mode(WIFI_MODE_STA);
  WiFi.setAutoReconnect(true);
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

}


/**
 * @brief Sends the sensor readouts to Thingspeak
 *
 * Function reads out the BME280 sensor.
 * Then the HTTPS request is prepared in order to send data to server.
 */
void SendDataToCloud()
{
  float DataToSend[3]= {0};
  char Message[200]={0};
  char tempMessage[40]={0};
  DataToSend[0]=Sensor.readTemperature();
  DataToSend[1]=Sensor.readPressure()/100.0F;
  DataToSend[2]=Sensor.readHumidity();
  ThingspeakConnectionWrite.flush();
  if(!ThingspeakConnectionWrite.connected())
  {
    if(ThingspeakConnectionWrite.connect(Thingspeak, ThingspeakPort))
    {
      Serial.println("Connected to Thingspeak!");
    }
    else
    {
      Serial.println("An error occurred!");
      return;
    }
  }

  /*Prepare Thingspeak HTTP request to send data*/
  sprintf(Message, "GET https://api.thingspeak.com/update?api_key=%s", API_KEY);

  for(int i=1;i<=3;i++)
  {
    sprintf(tempMessage, "&field%d=%.2f", i, DataToSend[i-1]);
    strcat(Message,tempMessage);
  }
  ThingspeakConnectionWrite.println(Message);
  ThingspeakConnectionWrite.println("Connection: close");
  ThingspeakConnectionWrite.flush();
}

/**
 * @brief Prints the sensor readout through serial interface.
 */
void PrintValues() {
  Serial.print("Temperature = ");
  Serial.print(Sensor.readTemperature());
  Serial.println(" *C");
  
  Serial.print("Pressure = ");
  Serial.print(Sensor.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Humidity = ");
  Serial.print(Sensor.readHumidity());
  Serial.println(" %");

  Serial.println();
}

/**
 * @brief Setup the peripherals, read out the sensor ans send data to Thingspeak, then enhter deep sleep for set amount of time.
 */
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

  //PrintValues();
  if(WiFi.isConnected())
  {
    SendDataToCloud();
  }
  else
  {
    Serial.println("No WiFi connection!");
  }

  delay(500); 
  esp_deep_sleep(DEEP_SLEEP_SECONDS*DEEP_SLEEP_S_MULTIPLIER);
}

void loop() {

}