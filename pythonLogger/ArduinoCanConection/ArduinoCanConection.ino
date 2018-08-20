#include <SoftwareSerial.h> 
#include <Canbus.h>
#include <MsgParsing.h>


CanbusClass Canbus;
CanMsg msg;
int chipSelect = 10;
uint8_t messageWrapper[3] = {0xfb, 0xfa, 0xcb};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("STARTED");
  
  if(Canbus.Init(chipSelect)) {
    Serial.println("CAN bus initialized.");
  }
  Serial.println("SETUP COMPLETE");  

    
}

void loop() {
    if (Canbus.CheckForMessages()) {
      Canbus.GetMessage(&msg);
      processCANMessage (msg);
    }
}

void writeStart(){
  Serial.write(messageWrapper[0]);
  Serial.write(messageWrapper[1]);
  Serial.write(messageWrapper[2]);
}

void processCANMessage (CanMsg& msg){
  writeStart();
  printIntAsBytes(msg.id);
  for (int i; i < 8; i++){
    Serial.write(msg.data[i]);
  }
  
  
}


void printIntAsBytes(uint32_t integer){
  uint8_t x;
  for (int n= 0; n < 4; n ++){
    x = (integer >> (8*n)) & 0xff;
    Serial.write(x);
  }

}
