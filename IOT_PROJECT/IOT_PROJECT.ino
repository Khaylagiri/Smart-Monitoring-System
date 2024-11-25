// Blynk IOT Smart Plant Monitoring System

/* Connections
Relay. D3
Btn.   D7
Soil.  A0
PIR.   D5
SDA.   D2
SCL.   D1
Temp.  D4
*/

//Include the library files
#include <Wire.h>
#include <SPI.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>



char auth[] = "MIdMMCin-6-xwaQo07S5qCFdX5giuK5z";  //Enter your Blynk Auth token
char ssid[] = "Sab";  //Enter your WIFI SSID
char pass[] = "jimin131095";  //Enter your WIFI Password

DHT dht(D4, DHT11);//(DHT sensor pin,sensor type)  D4 DHT11 Temperature Sensor
BlynkTimer timer;

//Define component pins
#define soil A0     //A0 Soil Moisture Sensor
#define PIR D5      //D5 PIR Motion Sensor
int PIR_ToggleValue;

void checkPhysicalButton();
int relay1State = LOW;
int pushButton1State = HIGH;
#define RELAY_PIN_1       D3   //D3 Relay
#define PUSH_BUTTON_1     D7   //D7 Button
#define VPIN_BUTTON_1    V12 

//Create three variables for pressure
double T, P;
char status;



void setup() {
  Serial.begin(9600);
  pinMode(PIR, INPUT);

 pinMode(RELAY_PIN_1, OUTPUT);
 digitalWrite(RELAY_PIN_1, LOW);
  pinMode(PUSH_BUTTON_1, INPUT_PULLUP);
  digitalWrite(RELAY_PIN_1, relay1State);
Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  dht.begin();

  timer.setInterval(100L, soilMoistureSensor);
  timer.setInterval(100L, DHT22sensor);
  timer.setInterval(500L, checkPhysicalButton);
}


//Get the DHT11 sensor values
void DHT22sensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);


}


//Get the soil moisture values
void soilMoistureSensor() {
  int value = analogRead(soil);
  value = map(value, 0, 1024, 0, 100);
  value = (value - 100) * -1;

  Blynk.virtualWrite(V3, value);
}

//Get the PIR sensor values
void PIRsensor() {
  bool value = digitalRead(PIR);
  if (value) {
    Blynk.logEvent("pirmotion","WARNNG! Motion Detected!"); //Enter your Event Name
    WidgetLED LED(V5);
    LED.on();
  } else {
    WidgetLED LED(V5);
    LED.off();
  }  
  }

BLYNK_WRITE(V6)
{
 PIR_ToggleValue = param.asInt();  
}


BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(VPIN_BUTTON_1);
}

BLYNK_WRITE(VPIN_BUTTON_1) {
  relay1State = param.asInt();
  digitalWrite(RELAY_PIN_1, relay1State);
}

void checkPhysicalButton()
{
  if (digitalRead(PUSH_BUTTON_1) == LOW) {
    // pushButton1State is used to avoid sequential toggles
    if (pushButton1State != LOW) {

      // Toggle Relay state
      relay1State = !relay1State;
      digitalWrite(RELAY_PIN_1, relay1State);

      // Update Button Widget
      Blynk.virtualWrite(VPIN_BUTTON_1, relay1State);
    }
    pushButton1State = LOW;
  } else {
    pushButton1State = HIGH;
  }
}


void loop() {
    if (PIR_ToggleValue == 1)
    {
      PIRsensor();
      }
     else
     {
    WidgetLED LED(V5);
    LED.off();
     }

      
  Blynk.run();//Run the Blynk library
  timer.run();//Run the Blynk timer

  }
