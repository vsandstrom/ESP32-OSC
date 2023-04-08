#include "Arduino.h"
#include "WiFi.h" // <-------------------- beware!!! W i F i . h, remember the capital 'F'
#include <OSCMessage.h>
#define LED 2
#define CV 16
// #define Wifi WiFi

WiFiUDP udp;
const int update_rate = 16;
const int channel = 0; // <--- chooses which of the PWM units on the ESP32 to use
const int freq = 5000; // <--- the frequency of the pulse
const int resolution = 8; // <--- on ESP32 the useful bits are 16.

// Change to analogWrite message and set to different OSC messages, "fwd" / "bck"
void ledToggle(OSCMessage &msg) {
  switch (msg.getInt(0)) {
    case 0:
      digitalWrite(LED, LOW);
      break;
    case 1:
      digitalWrite(LED, HIGH);
      break;
  }
}

// Perhaps set this to respond to different messages
void setCV(OSCMessage &msg) {
  // the voltage 0V - 3.3V is represented by 0 - 255 
  // to get up to the voltage required, perhaps a voltage regulator is required.
  float cv = msg.getFloat(0);
  float inc = 255.0 / 100.0; // converts 0 - 255 values to a scale of 0 - 100% of 3.3V
  int value = (int)(inc * cv + 0.5);
  ledcWrite(CV, value); 
}

void receiveMessage() {
  OSCMessage msg;
  int size = udp.parsePacket();

  if (size > 0) {
    while (size--) {
      msg.fill(udp.read());
    }
    if (!msg.hasError()) {
      msg.dispatch("/led", ledToggle);
      // finetune messages to just do one speed, reverse etc.
      msg.dispatch("/cv", setCV);
    } 
    //else { auto error = inmsg.getError(); }
  }
}

void setup() {
  // put your setup code here, to run once:
  WiFi.mode(WIFI_STA);
  pinMode(LED, OUTPUT);
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(CV, channel);
  const char* ssid = "iPhone (122)";
  const char* password = "testing999";
  unsigned int port = 8888;
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Retry connection\n");
    delay(500);
  }

  // If WiFi.status = 
  Serial.printf("Connected!\n");// %s", WiFi.status());
  Serial.println(WiFi.localIP());
  udp.begin(port);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Serial.printf("Hello\n");
  // digitalWrite(LED, 1);
  // delay(250);
  // digitalWrite(LED, 0);
  // delay(250);
  receiveMessage();
  delay(update_rate);
}
