# blinky

MQTT controllable ESP8266

## Arduino IDE Setup

-   Add additional boards by following the Instructions at https://github.com/esp8266/Arduino#installing-with-boards-manager.
-   Select your board under Tools -> Board (NodeMCU 0.9 or 1.0 should be fine)
-   Make sure to select at least 1MB Flash Size under Tools -> Flash Size
-   Downlaod and install the async-mqtt-client libarary https://github.com/marvinroger/async-mqtt-client/blob/master/docs/1.-Getting-started.md
-   Download and install the ESPAsyncTCP library by downloadiing the .zip folder and import it with Sketch -> Include Library -> Add .ZIP Library
-   Install the ArduinoJson Library by following the Instructions at https://arduinojson.org/v6/doc/installation/
-   Install the WiFiManager Library by following the Instructions at https://github.com/tzapu/WiFiManager#install-through-library-manager
-   Install the Adafruit NeoPixel Library by following the Instructions at https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-installation

## Flashing ESP8266

-   Flashing is possible using the Arduino ide and connection via USB (NodeMCU 0.9 and 1.0)
-   When flashed initial via USB "Over the Air" Updates are possible

## Setup

After the first flash the ESP will open a WiFi Hotspot. When connected to this WiFi a splash page should show up. Click on "Configure WiFi" to set the relevant parameter.

-   SSID Name of your WiFi
-   Password Password of your WiFi
-   Server IP or FQDN of your MQTT Server
-   Port Server Port used for MQTT
-   Nodename Name of the Node (i.e. light1)
-   Devicetype Type is as the root for mqtt topic for this device

Click Save and the Node will restart and should connect to the configured MQTT Server.

## MQTT Commands

All commands and responses are json encoded. Possible commands are:

- Reset Wifi
```javascript
{
"type": "cmd",
"cmd": "wifiReset"
}
```
Resets the WiFi config on the Node and starts the Configuration Web Portal.

- Debug mode
```javascript
{
"type": "cmd",
"cmd": "debug",
"value": "on"
}
```
Activates debug messages over mqtt. Value can be on or off.

- Activate Ball Led Animation
```javascript
{
"type": "cmd",
"cmd": "setProgram",
"value": "BALL"
}
```

- Activate Blink Led Animation
```javascript
{
"type": "cmd",
"cmd": "setProgram",
"value": "BLINK"
}
```

- Activate a specific color
```javascript
{
"type": "cmd",
"cmd": "setProgram",
"value": "MANUAL",
"red": "255",
"green": "125",
"blue": "50"
}
```
Red,green and blue can be values between 0 and 255

- Activate Blink Led Animation
```javascript
{
"type": "req",
"req": "wtf",
"uid": "1234"
}
```
Returns a respons with messages that can be send to the device.

