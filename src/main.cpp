#include "Arduino.h"
#include "WiFi.h" // <-------------------- beware!!! W i F i . h, remember the capital 'F'
#include <OSCMessage.h>
#include "../credentials.c" // <---------- set wifi credentials here!
                            // SSID (string), 
                            // PASS (string),
                            // PORT (integer)
#define LED 2 // <--- Built-In LED
#define CV 23 // <--- PWD-capable GPIO next to GND-pin
#ifdef DEBUG
  #define D(x) x
#else 
  #define D(x)
#endif
  

WiFiUDP udp;
const int update_rate = 8;
const int channel = 0; // <----- chooses which of the PWM units on the ESP32 to use
const int FREQUENCY = 1000; // <----- the frequency of the pulse
const int RESOLUTION = 16; // <-- if the RESOLUTION is 8 bit : 0 - 255, 16 bit : 0 - 65535
const int MAX = 65535;

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

void setCV(OSCMessage &msg) {
  // the voltage 0V - 3.3V is represented by 0 - 255 
  // to get the voltage required, perhaps a voltage regulator is required.
  float cv = msg.getFloat(0);
  int value = (MAX + 0.5) * cv;

  D(Serial.printf("Dimmer value: %d\n", value);)
  ledcWrite(channel, value); 
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
      // finetune messages to just do one speed, reverse etc.
      msg.dispatch("/led", ledToggle);
    } 
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
  // Use serial for debugging purpose
  Serial.begin(115200);
  // Output pin setup
  pinMode(LED, OUTPUT);
  ledcSetup(channel, FREQUENCY, RESOLUTION);
  ledcAttachPin(CV, channel);
  // Wifi client setup
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Retry connection\n");
    delay(500);
  }
  connected();
  udp.begin(PORT);
  // Setup output pins
}

void loop() {
  receiveMessage();
  delay(update_rate);
}
