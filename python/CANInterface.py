import subprocess
import struct
import time


class pipeInterface():

    def __init__(self):
        self.buff = b''
        self.canIds =[]
        self.proc = subprocess.Popen("./../CANBus-pipe.run",
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE)
        self.filter = [701, 167576096]
        self.filtering = False
        self.startSeq = ["fb", "fa", "cb"]
        self.stopSeq = ["cb", "fa", "fb"]
        
    def readCanMsg (self):
        canId  = b''
        while (len (canId) < 3):
            canId += self.proc.stdout.read(1)
        while (self.isStop(canId[-3:]) == False):
            canId += self.proc.stdout.read(1)
        canId = int(canId[:-3].decode("utf-8"))
        
        if (canId not in self.canIds):
            self.canIds.append(canId)
            print (self.canIds)
        
        if((self.filtering == False) or (canId in self.filter)):
            print(canId)
        
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
                
                message = self.proc.stdout.read(1)
                self.buff += message     
                if (len (self.buff) > 2):
                    if (self.isStart(self.buff[-3:])):
                        self.buff = self.buff[:-3]
                        self.readCanMsg() 
                        
            print(self.buff[:-1]) 

        
    

pipe = pipeInterface()
pipe.run()
print (pipe.canIds)
print (len (pipe.canIds))

        