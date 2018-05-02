import time
from PipeConection import *
from ArduinoConection import *
from CanMsg import *

class CANInterface():

    def __init__(self):
        self.buff = b''
        self.canIds =[]
        self.connection = PipeConection()
        #self.connection = ArduinoConection("/dev/ttyACM0")
        self.filter = [701, 167576096]
        self.filtering = False
        self.startSeq = ["fb", "fa", "cb"]
        self.stopSeq = ["cb", "fa", "fb"]
        self.initLog()
        
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
        
            
        
    def processCanMsg (self, message):
        if (message.id not in self.canIds):
            self.canIds.append(message.id)
            print (self.canIds)
        self.logCanMsg(message)
        if((self.filtering == False) or (message.id in self.filter)):
            print(message.id)
        
    def isStart(self, seq):
        for i in range(len(seq)):
            if (seq[i].encode("hex") != self.startSeq[i]):
                return False
        #print ("IDENTFEIED START")
        return True
      
    
    def initLog(self):
        self.fileName = "logs/CanBus_" + time.strftime("%Y%b%d%H%M%S", time.localtime()) + ".log"
        file = open(self.fileName, 'w')
        file.write("[Time HH:MM:SS    messageID    dataBytes[7 ... 0]]\n")
        file.close()
    
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
        

    
    def run(self):
        
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

        
    

pipe = CANInterface()
pipe.run()


        