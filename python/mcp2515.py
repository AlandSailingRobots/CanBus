from mcp2515_defs import *
from MsgParsing import *

import time
from wiringpi import *


def MCP2515_SendByte(Data):
    if type(Data) != type(b'x00'):
        print("Note in byte")
        return
    ReadWrite = bytes();
    ReadWrite += Data
    retLen, retData = wiringPiSPIDataRW(CHANNEL,ReadWrite)
    
def MCP2515_Read(Address):
    if type(Address) != type(b'x00'):
        print("Note in byte")
        return
    ReadWrite = bytes();
    ReadWrite += SPI_READ + Address + b'x00'
    retLen, retData = wiringPiSPIDataRW(CHANNEL,ReadWrite)
    return retData

def MCP2515_Write(Address, Data):
    if (type(Address) != type(b'x00')):
        print("Note in byte")
        return
    ReadWrite = bytes();
    ReadWrite += SPI_READ + Address + Data
    retLen, retData = wiringPiSPIDataRW(CHANNEL,ReadWrite)
    
def MCP2515_ReadStatus():
    ReadWrite = bytes()
    ReadWrite += SPI_READ_STATUS +  b'x00'
    retLen, retData = wiringPiSPIDataRW(CHANNEL,ReadWrite)
    return retData

def MCP2515_RXStatus():
    ReadWrite = bytes()
    ReadWrite += SPI_RX_STATUS +  b'x00'
    retLen, retData = wiringPiSPIDataRW(CHANNEL,ReadWrite)
    return retData
    
def MCP2515_BitModify(Address, Mask, Data):
    if (type(Address) != type(b'x00')) or (type(Data) != type(b'x00')) or (type(Mask) != type(b'x00')):
        print("Note in byte")
        return
    ReadWrite = bytes()
    ReadWrite += SPI_BIT_MODIFY + Address + Mask + Data
    retLen, retData = wiringPiSPIDataRW(CHANNEL,ReadWrite)
    
def MCP_Init():
    MCP2515_SendByte(SPI_RESET)
    print("Waiting for CAN bus to restart")
    time.sleep(0.01)
    
    ReadWrite = bytes()
    ReadWrite += SPI_WRITE + CNF3 + b'x05' + b'xf1' + b'x41' + bytes([((1<<RX1IE)|(1<<RX0IE))])
    retLen, retData = wiringPiSPIDataRW(CHANNEL,ReadWrite)
    if(MCP2515_Read(CNF1) != b'x41'):
        print ("Chip not accessible")
        return False
    
    #deactivate the RXnBF Pins (High Impedance State)
    MCP2515_Write(BFPCTRL, b'x00')
    MCP2515_Write(TXRTSCTRL, b'x00')
    MCP2515_Write(RXB0CTRL, bytes([(1<<RXM1) | (1<<RXM0) | (1<<BUKT)]))
    MCP2515_Write(RXB1CTRL, bytes([(1<<RXM1) | (1<<RXM0)]))
    MCP2515_Write(CANCTRL, b'x00')
    return True

PickFirst = True

def MCP2515_CheckMessage():
    RXStatus = toInt(MCP2515_RXStatus())
    if((RXStatus & (1 << 6)) or (RXStatus & (1 << 7))):
        if((RXStatus & (1 << 6)) and (RXStatus & (1 << 7))):
            if(PickFirst):                        
                MsgAddr = SPI_READ_RX          
                PickFirst = False
            else:
                MsgAddr = bytes([toInt(SPI_READ_RX)| 0x04])
                PickFirst = True
        else:
            PickFirst = Trues
            if(RXStatus & (1 << 6)):
                MsgAddr = SPI_READ_RX
            else:
                MsgAddr = bytes([toInt(SPI_READ_RX) | 0x04])
    else:
        return 0
    return MsgAddr

def MCP2515_GetMessage(MsgAddress):
    message = CanMsg()
    
    if(MsgAddress != 0):
        MsgAddr = MsgAddress
        pass
    else:
        MsgAddr = MCP2515_CheckMessage()
        if(MsgAddr==0):
            return False
    
    ReadWrite = bytes()
    ReadWrite += MsgAddr + bytes(13)
    retLen, retData = wiringPiSPIDataRW(CHANNEL, ReadWrite)
    message.ide = (toInt(retData[2])>>IDE) & 0x01
    message.id = toInt(retData[1] << 3)
    message.id = message.id | (toInt(retData[2])>>5)
    if (message.ide != 0):
        message.id = (message.id << 18)
        message.id = message.id | ((toInt(retData[2]) & 0x03) << 16)
        message.id = message.id | (toInt(retData[3]) << 8)
        message.id = message.id | (toInt(retData[4]))
    
    message.lenght = toInt(retData[5])& 0x0f
    for i in range(message.lenght):
        message.data[i] = toInt(retData[6+i])
    
    return message


        
    
        
    
                
            
    
def toInt (Byte):
    int.from_bytes(Byte, byteorder='big', signed=False)
    
    
    


    
    
    
