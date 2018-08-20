import subprocess

#Conection to Canbus C++ progam that can be ran on a RPi
class PipeConection():
    
    def __init__(self):
         self.proc = subprocess.Popen("./../CANBus-pipe.run",
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE)
         
         
    def read(self, numberOfBytes):
        return self.proc.stdout.read(numberOfBytes)