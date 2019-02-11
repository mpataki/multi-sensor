# multi-sensor

This project is a work in progress.

This project connects an number of inexpensive sensors (light, motion, temperature/humidy so far) to an MQTT broker via an ESP8266 based board. It leverages PlatformIO, and some Arduino libraries.

When starting up for the first time the module will open an access point that you can connect to. Depending the the device that you connect with, it should prompt you to "sign in", which directs your browser to an HTTP server running on the device which exposes a form that you can configure the unit with, including WIFI and MQTT credentials, as well as a poll frequency in milliseconds.

To reset your config, hold the flash button. How long you have to hold the button depends on your poll frequency, as the whole unit will sleep for this long between loop iterations. All settings will be cleared and a new access point will be started when the unit notices the flash button is pressed.

