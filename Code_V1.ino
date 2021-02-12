#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>
#define FIREBASE_HOST "spo2watcher.firebaseio.com/"
#define FIREBASE_AUTH "cJ5z2Mq7btANEpz77Fv0yv7A8xEHJYypFDHn0j5G"
#define WIFI_SSID "Gandotras_4G"                                             
#define WIFI_PASSWORD "shalini12345"

#define REPORTING_PERIOD_MS     2000
uint32_t tsLastReport = 0;
PulseOximeter pox;

volatile boolean heartBeatDetected = false;

void onBeatDetected()
{   
    Serial.println("Beat!");
    heartBeatDetected = true;    
}
 
void setup()
{
    Serial.begin(9600);
    delay(1000);                 
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                     //try to connect with wifi
    Serial.print("Connecting to ");
    Serial.print(WIFI_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
    }
    Serial.println();
    Serial.print("Connected to ");
    Serial.println(WIFI_SSID);
    Serial.print("IP Address is : ");
    Serial.println(WiFi.localIP());                                            //print local IP address
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                              // connect to firebase
    if (Firebase.failed()) {
        Serial.print("setting /message failed:");
        Serial.println(Firebase.error());
    }
    else {
        Serial.println("SUCCESS");
    }
    Serial.print("Initializing pulse oximeter..");
    // Initialize the PulseOximeter instance
    // Failures are generally due to an improper I2C wiring, missing power supply
    // or wrong target chip
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }
     pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
 
    // Register a callback for the beat detection
    pox.setOnBeatDetectedCallback(onBeatDetected);
}
 
void loop()
{
    // Make sure to call update as fast as possible
    pox.update();
    uint32_t ts = millis();
    if (ts - tsLastReport > REPORTING_PERIOD_MS && heartBeatDetected) {
        heartBeatDetected = false;
        float hr = pox.getHeartRate();
        float sp = pox.getSpO2();
        if(sp > 0)
        {
        Serial.print("Heart rate:");
        Serial.print(hr);
        Serial.print("bpm / SpO2:");
        Serial.print(sp);
        Serial.println("%");
 
        tsLastReport = ts;
        }
    }
}
