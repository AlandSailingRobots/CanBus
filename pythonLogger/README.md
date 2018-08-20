# ** Logger using python**

Python logger that can log all CAN messages it detects or filter out messages. Can be used with arduino interface or rpi interface. Comment in the desired connection. 

* RPi pipe:
Needs to have a cpp pipe conection. Crate with make in above directory:
```shell
make pipe USE_MOCK=0
```
If USE_MOCK=1 then the logger is runed with a mock canbus for debugging the logger.

* Arduino:

Needs a arduino with can-shield connected and flashed with ArduinoCanConection script. A string with the port of the arduino is added in the constructor of the connection.

