#include "Arduino.h"
#include "WiFi.h" // <-------------------- beware!!! W i F i . h, remember the capital 'F'
#include <OSCMessage.h>
#define LED 2
#define CV 0
// #define Wifi WiFi

WiFiUDP udp;
int update_rate = 16;

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
  float cv = msg.getFloat(0);
  analogWrite(CV, cv);
}

void receiveMessage() {
  OSCMessage inmsg;
  int size = udp.parsePacket();

  if (size > 0) {
    while (size--) {
      inmsg.fill(udp.read());
    }
    if (!inmsg.hasError()) {
      inmsg.dispatch("/led", ledToggle);
      // finetune messages to just do one speed, reverse etc.
      inmsg.dispatch("/cv", setCV);
    } 
    //else { auto error = inmsg.getError(); }
  }
}

void setup() {
  // put your setup code here, to run once:
  WiFi.mode(WIFI_STA);
  pinMode(LED, OUTPUT);
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
