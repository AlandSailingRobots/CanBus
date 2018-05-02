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
        
    def readCanMsg (self):
        byteId = ""
        for i in range(4):
            byteId = self.connection.read(1) + byteId
        intId = int (byteId.encode("hex"), 16)
        message = CanMsg(intId)
        self.processCanMsg(message)
        
            
        
    def processCanMsg (self, message):
        if (message.id not in self.canIds):
            self.canIds.append(message.id)
            print (self.canIds)
        
        if((self.filtering == False) or (message.id in self.filter)):
            print(message.id)
        
    def isStart(self, seq):
        for i in range(len(seq)):
            if (seq[i].encode("hex") != self.startSeq[i]):
                return False
        #print ("IDENTFEIED START")
        return True
      

    def isStop(self, seq):
        for i in range(len(seq)):
            if (seq[i].encode("hex") != self.stopSeq[i]):
                return False
        #print ("IDENTFEIED STOP")
        return True

    
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


        