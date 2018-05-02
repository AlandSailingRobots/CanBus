
#if MOCK == 1
	#include "mockCanbus.h"
#else
	#include "mcp2515.h"
	#include "Canbus.h"
#endif
#include <iostream>
#include <fstream>
#include <csignal>




void printIntAsBytes(uint32_t integer){
	uint8_t x;
	for (int n= 0; n < 4; n ++){
		x = (integer >> (8*n)) & 0xff;
		std::cout << std::dec << x;
	}

}


int main()
{
	std::cout << "***Started CAN-bus pipe***\n";
	CanbusClass canbus;
	CanMsg canMsg;
	if (canbus.Init(1000000)){
		std::cout << "Canbus init successfull\n";

	}

	uint8_t msgWrapper[3];
	msgWrapper[0] = 0xfb;
	msgWrapper[1] = 0xfa;
	msgWrapper[2] = 0xcb;

	while (true){

		if (canbus.CheckForMessages()){
			canbus.GetMessage(&canMsg);
			std::cout << msgWrapper[0] << msgWrapper[1] << msgWrapper[2];
			printIntAsBytes(canMsg.id);
			std::cout << "Sent ID";
			std::cout << "\n";
		}
		fflush(stdout);

	}



}


