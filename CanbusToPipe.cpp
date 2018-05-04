/****************************************************************************************
 *
 * File:
 * 		CanbusToPipe.cpp
 *
 * Purpose:
 *      Sends all CAN-messages that is received from the bus to standard output
 *      that is interpreted as a pipe connection in the python script. Can be used with
 *      mockCanbus
 *
 *
 * Developer Notes:
 *
 ***************************************************************************************/




#if MOCK == 1
	#include "mockCanbus.h"
#else
	#include "CanbusRPi/mcp2515.h"
	#include "CanbusRPi/Canbus.h"
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

void printCanData(CanMsg canMsg){
	for (int i; i < 8; i++)
		std::cout << canMsg.data[i];
}


int main()
{
	uint8_t msgWrapper[3];
	msgWrapper[0] = 0xfb;
	msgWrapper[1] = 0xfa;
	msgWrapper[2] = 0xcb;
	
	std::cout << "***Started CAN-bus pipe***\n";
	CanbusClass canbus;
	CanMsg canMsg;
	if (canbus.Init(1000000)){
		std::cout << "Canbus init successfull\n";

	}



	while (true){

		if (canbus.CheckForMessages()){
			canbus.GetMessage(&canMsg);
			std::cout << msgWrapper[0] << msgWrapper[1] << msgWrapper[2];
			printIntAsBytes(canMsg.id);
			printCanData(canMsg);
			std::cout << "Sent MSG";
			std::cout << "\n";
		}
		fflush(stdout);

	}



}


