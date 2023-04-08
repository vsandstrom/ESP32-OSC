#include "Arduino.h"
#include "WiFi.h" // <-------------------- beware!!! W i F i . h, remember the capital 'F'
#include <OSCMessage.h>
#include "../credentials.c" // <---------- set wifi credentials here!

#define LED 2
#define CV 23
// #define Wifi WiFi


#ifdef DEBUG
  #define D(x) x
#else 
  #define D(x)
#endif
  
WiFiUDP udp;
const int update_rate = 8;
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

// Callback for variable voltage output. NEEDS TESTING!
void setCV(OSCMessage &msg) {
  // the voltage 0V - 3.3V is represented by 0 - 255 
  // to get up to the voltage required, perhaps a voltage regulator is required.
  float cv = msg.getInt(0);
  // float inc = 255.0 / 100.0; // converts 0 - 255 values to a scale of 0 - 100% of 3.3V
  // int value = (int)(inc * cv + 0.5);
  D(Serial.printf("Dimmer value: %i\n", value);)
  ledcWrite(channel, cv); 
}

void receiveMessage() {
  OSCMessage msg;
  int size = udp.parsePacket();
  // read 4 bytes at a time from osc message
  if (size > 0) {
    while (size--) {
      msg.fill(udp.read());
    }
    Serial.println("");
    if (!msg.hasError()) {
      msg.dispatch("/cv", setCV);
      msg.dispatch("/led", ledToggle);
      // finetune messages to just do one speed, reverse etc.
    } 
    //else { auto error = inmsg.getError(); }
  }
}

void connected() {
  Serial.printf("Connected!\n");// %s", WiFi.status());
  Serial.println(WiFi.localIP());
  int on[5] = {300, 100, 100, 100, 300};
  int off[5] = {150, 100, 100, 100, 150};
  for (int i = 0; i < 5; ++i) {
    digitalWrite(LED, HIGH);
    delay(on[i]);
    digitalWrite(LED, LOW);
    delay(off[i]);
  }
}

void setup() {
  // put your setup code here, to run once:
  WiFi.mode(WIFI_STA);
  pinMode(LED, OUTPUT);
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(CV, channel);
  const char* ssid = SSID;
  const char* password = PASS;
  unsigned int port = 8888;
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Retry connection\n");
    delay(500);
  }

  // If WiFi.status = 
  connected();
  udp.begin(port);
}

void loop() {
  receiveMessage();
  delay(update_rate);
}
