import time
from PipeConection import *
from ArduinoConection import *
from CanMsg import *
import sys

#Interface class for logging the CAN-bus can be used with arduino or on a RPi
class CANInterface():

    def __init__(self, decierdedInterface= 1, usbPort = "/dev/ttyUSB0"):
        self.buff = b''
        self.canIds =[]
        if decierdedInterface == "1":
            self.connection = PipeConection()
        elif decierdedInterface == "2":
            self.connection = ArduinoConection(usbPort)
        else:
            self.connection="NONE"
        self.printFilter = [701, 167576096]
        self.printFiltering = False
        self.logFilter = []
        self.logFiltering = True
        self.startSeq = ["fb", "fa", "cb"]
        self.initLog()
        
        
    #Reads a Msg from the selected conection. Reads 4 bytes that are the ID and 8 that are the raw data.    
    def readCanMsg (self):
        byteId = ""
        data = []
        for i in range(4):
            byteId = self.connection.read(1) + byteId
        intId = int (byteId.encode("hex"), 16)
        for n in range (8):
            data.append(self.connection.read(1).encode("hex"))
        message = CanMsg(intId, data)
        self.processCanMsg(message)
        
            
    #Deides if the message is to be printed and/or logged dependig on filter settings.     
    def processCanMsg (self, message):
        if (message.id not in self.canIds): #If new message ID
            self.canIds.append(message.id)
            print (self.canIds)
        if ((self.logFiltering == False) or (message.id in self.logFilter)):
            self.logCanMsg(message)
        if((self.printFiltering == False) or (message.id in self.printFilter)):
            print(message.id)
        
      
     #Identifes if a sequesnse is a predetermined start sequesne   
    def isStart(self, seq):
        for i in range(len(seq)):
            if (seq[i].encode("hex") != self.startSeq[i]):
                return False
        #print ("IDENTFEIED START")
        return True
      
    #Inlitialise log file
    def initLog(self):
        self.fileName = "logs/CanBus_" + time.strftime("%Y%b%d%H%M%S", time.localtime()) + ".log"
        file = open(self.fileName, 'w')
        file.write("[Time HH:MM:SS    messageID    dataBytes[7 ... 0]]\n")
        file.close()
    
    #Logs message with timestamp
    def logCanMsg(self, message):
        file = open(self.fileName, 'a')
        toWrite = time.strftime("%H:%M:%S", time.localtime())
        file.write(toWrite.rjust(14))
        toWrite = str (message.id)
        file.write(toWrite.rjust(13))
        toWrite = ""
        for b in message.data:
            toWrite = b + toWrite
        file.write(toWrite.rjust(22))    
        file.write('\n')
        file.close()
        
        
    #Logs witch messages was found. Used when program terminates    
    def logCanIds(self):
        file = open(self.fileName, 'a')
        file.write("Got the folowing Can ids: ")
        file.write(str(self.canIds))
        file.close()
    
    
    #Main loop
    def run(self):
        if self.connection == "NONE":
            print("INVALID CONECTION VALUE! Try 1 for RPI or 2 for Arduino")
            return

        startTime = time.time()
        while(True):
        #while (time.time() < startTime + 5):
            self.buff = b''
            message = b''
            while(message != b'\n'):
                
                message = self.connection.read(1)
                self.buff += message     
                if (len (self.buff) > 2):
                    if (self.isStart(self.buff[-3:])):
                        self.buff = self.buff[:-3]
                        self.readCanMsg() 
                        
            print(self.buff[:-1]) 

        
    

def main(decierdedInterface= 1, usbPort = "/dev/ttyUSB0"):
    pipe = CANInterface(decierdedInterface, usbPort)
    try:
        pipe.run()
    finally:
        pipe.logCanIds()

if __name__ == '__main__':
    if len(sys.argv) == 1:
        print('''USEAGE: python2 CANInterface.py conectionType [usbPort]
        conectionType == 1 -> cpp-pipe conection (Rpi) 
        conectionType == 2 -> Arduino conection
        usbPort = usbport of arudino''')
    if len(sys.argv) == 2:
        main(sys.argv[1])
    if len(sys.argv) >= 3:
        main(sys.argv[1], sys.argv[2])





        
        
        
