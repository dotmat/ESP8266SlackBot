//  SlackBot - 9th May 2019

// The SlackBot is an Afadruit ESP8266 chip that connects to your Wifi and lets users via slack know that someone is at the front door.
// Hardware: Adafruit ESP8266, LiPoly Battery, Button. 

// Onboot the Unit announces its presence to the test room - via the test channel URL.
// When the doorbell is pressed the ESP8266 makes an API request to Slack telling everyone that someone is at the front door.

// HeaderFiles
#include <Arduino.h>

// Wifi Files
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
ESP8266WiFiMulti WiFiMulti;

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

// Pin Config
const int ledPin = LED_BUILTIN;
const int buttonPin = 14;
const int userLEDPin = 12;

// Button Config
int buttonState = 0;

void setup() {
  Serial.begin(115200);
  //Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  // Config of pins
  pinMode(ledPin, OUTPUT);
  pinMode(userLEDPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.println("ButtonPins have been configured.");

  for (uint8_t t = 5; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("Put Your Wifi SSID Here ", "Put your Wifi Password here");

  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    Serial.println("SlackBot has come online");
    sendMessageToSlack("<!here> SlackBot has come online.", "URL of channel where this alert will go to.");
  }
}

void loop() {
  // Read the state of the doorbell button
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    //handleButtonPress();
    digitalWrite(ledPin, HIGH);
    digitalWrite(userLEDPin, HIGH);
  } else {
    handleButtonRelease();
  }
}

void sendMessageToSlack(String messagePayload, String urlToSendTo) {
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure();
  HTTPClient https;

  Serial.println("Attempting HTTP Connection to Slack");
  
  https.begin(urlToSendTo, "C1:0D:53:49:D2:3E:E5:2B:A2:61:D5:9E:6F:99:0D:3D:FD:8B:B2:B3");
  https.addHeader("Content-Type", "application/json");
  String postMessage = String("{'text' : '"+messagePayload+"'}");
  int httpCode = https.POST(postMessage);
  Serial.print("http result:");
  Serial.println(httpCode);
  https.writeToStream(&Serial);
  String payload = https.getString();
  https.end();
  
}


// All the events get triggered once the button is finished being depressed. This stops continious loops of API calls and what not.
void handleButtonRelease() {
  digitalWrite(ledPin, LOW);
   digitalWrite(userLEDPin, LOW);
   Serial.println("Button was pressed");
   sendMessageToSlack("<!here> Ding Dong! Someone is at the door, you should probably get it... :door: :wave:", "URL of channel to notify people someones at the door");
   delay(1000);
}
