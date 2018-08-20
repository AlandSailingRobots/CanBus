# **CAN-Bus repository**

Collected most files that relates to the CAN-system used in the project. The main files used by the ASPire system can be found in the sailingrobot repo. These are adapted to specific tasks on ASPire on either arduino or raspberry pi: 

* [CanBus Arduino](https://github.com/AlandSailingRobots/sailingrobot/tree/develop/ArduinoSketches/libraries)

* [CanBus RPI](https://github.com/AlandSailingRobots/sailingrobot/tree/develop/NavigationSystem/Hardwares/CAN_Services)

* [CanBus Commons](https://github.com/AlandSailingRobots/CanBusCommon)

In this repo there are the original files created for interacting with a Canbus. 
#**Logging**
There are two logger that can be used. The first one can only be run on a RPi. It is made with
```shell
make logger
```

The other one is a python interface to the CAN-bus. Desctription in directroy pythonLogger
