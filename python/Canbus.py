from mcp2515 import *
from wiringpi import *

class Canbus ():
    def __init__(self):
        pass
    
    def SendMessage(self, Msg):
        pass
    
    def GetMessage(self):
        return MCP2515_GetMessage(0)
    
    def CheckForMessages(self):
        if (MCP2515_CheckMessage() == 0):
            return False
        else
            return True
        
    
    def Init(self, SPISpeed):
        wiringPiSetup()
        if(wiringPiSPISetup(CHANNEL, SPISpeed) == -1):
            print ("SPI setup failed")
            return False
        else:
            print ("SPI Setup successfull")
            return MCP2515_Init()
        


