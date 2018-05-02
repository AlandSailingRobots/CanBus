import serial

class ArduinoConection():
    
    def __init__(self,portName):
        self.portName = portName
        self.device = serial.Serial(portName)
        


    def read(self, numberOfBytes):
        return self.device.read(numberOfBytes)



        