#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include "mockCanbus.h"

#define SEND_INTERVAL 0.1

bool CanbusClass::Init(int SPISpeed){
	std::cout << "USING MOCK\n";
	return true;
}

bool CanbusClass::CheckForMessages(){
	float currentTime = ((float)clock())/CLOCKS_PER_SEC;
	if ((currentTime - m_lastMessageTime)   > SEND_INTERVAL){
		m_lastMessageTime = ((float)clock())/CLOCKS_PER_SEC;
		return true;
	}
	else
	{
		return false;
	}
}

bool CanbusClass::GetMessage(CanMsg *Msg){
	int randomValue = rand() % 100;
	if (randomValue <50){
		Msg->id = 701;
	}
	else
	{
		Msg->id = 1244343;
	}
	Msg->data[0] = 0xcb;
	Msg->data[3] = 0x92;
	Msg->data[5] = 0x75;
	return true;

}

