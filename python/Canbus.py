from mcp2515 import *

class Canbus ():
    def __init__(self):
        pass
    
    def SendMessage(self, Msg):
        pass
    
    def GetMessage(self):
        return MCP2515_GetMessage(0)
    
    def CheckForMessages(self):
        return MCP2515_CheckMessage()
    
    def Init(self, SPISpeed):
        wiringPiSetup()
        if(wiringPiSPISetup(CHANNEL, SPISpeed) == -1):
            print ("SPI setup failed")
            return False
        else:
            print ("SPI Setup successfull")
            return MCP2515_Init()
        