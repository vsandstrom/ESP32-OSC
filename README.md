# ESP32-OSC

The ESP32 uC is designed to handle recieving OSC (Open Sound Control) messages over wifi, and outputing a variable voltage, or an on/off state. The intended purpose is to be able to remote control custom devices from QLab in a theater setting.

The platformio project is set up so that you need to supply the credentials for the wifi network you want to work with inside a file called ***credentials.c*** in the root of the project:
```c
// inside credentials.c
#define SSID (string) // name of the wifi router
#define PASS (string) // password to router
#define PORT (integer) // port that the esp32 listens to incoming udp / osc messages at
```

## Compile & Upload:
Connect the esp32 to a USB port on your host computer
```zsh
# will compile the source files, similar to verify in the arduino IDE
$ pio run 
```

```zsh
# will compile, then search for a esp32 device connected to any USB port on your computer.
$ pio run -t upload
```
## Get device IP
The esp32-device will display a short blinking sequence to signal that it has connected a network.
```zsh
# this will monitor the esp32 serial output, 
# this tells you the esp32 own IP, needed for sending OSC-messages to the device
$ pio device monitor -b 115200
```
## Usage
The default behaviour is using the pin **23** as a varable voltage output, using PWM to ajust its output. The signal is between 0V - +3.3V and is set using an 8bit value, 0 - 255. It is the one next to the corner GND pin.<br>
```lua
-- The OSC address of the default variable voltage output:
/cv -- (values 0 - 255)
-- The address of the built-in LED
/led -- (values 0 - 1)
```

 This should be easy to configure and expand upon. 
