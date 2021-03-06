/****************************************************************************************
 *
 * File:
 * 		mockCanbus.h
 *
 * Purpose:
 *      Used to emulate the a the Canbus and mcp2515 files without needing to have the hardware conected
 *
 *
 * Developer Notes:
 *		Only implemented the checkFor message and getMessage for now.
 *
 ***************************************************************************************/



#ifndef canbus__h
#define canbus__h

#include "CanbusRPi/MsgParsing.h"
#include <vector>
#include <map>
#include <tuple>
#include <time.h>



class CanbusClass
{
public:
	CanbusClass() {};
	bool Init(int SPISpeed);
	bool CheckForMessages();

	//bool SendMessage(CanMsg *Msg);
	bool GetMessage(CanMsg *Msg);

	/*
	void SetNormalMode();
	void SetSleepMode();
	void SetLoopBackMode();
	void SetListenOnlyMode();
	void SetConfigMode();
	void SetMasksAndActivateFilters(uint32_t Mask1, uint32_t Mask2, bool RollOver);
	void SetFilter(int FilterIndex, uint32_t Filter);
	void SetFilterAndMask(int ReceiveBuffer, int FilterIndex, uint32_t Filter, uint32_t Mask);
	*/

private:
	float m_lastMessageTime;
	const float m_sendIntervall = 1;

};


#endif
