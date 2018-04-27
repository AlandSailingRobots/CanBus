#include "mcp2515.h"
#include "Canbus.h"
#include <iostream>
#include <fstream>
#include <csignal>


#define MSG_START_SEQ

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

	int x = (number >> (8*n)) & 0xff;

	while (true){

		if (canbus.CheckForMessages()){
			canbus.GetMessage(&canMsg);
			std::cout << msgWrapper[0] << msgWrapper[1] << msgWrapper[2];
			std::cout << std::dec << *idPtr;
			std::cout << msgWrapper[2] << msgWrapper[1] << msgWrapper[0];
			//std::cout << "Sent ID";
			//std::cout << "\n";
			fflush(stdout);
		}

	}



}


