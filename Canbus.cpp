//#include <stdint.h>
#include <iostream>
#include <cstdio>
#include "global.h"
#include "mcp2515.h"
#include "Canbus.h"

bool NMEA2000Bus::Init(int SPISpeed)
{
	return Canbus.Init(SPISpeed);
}

bool NMEA2000Bus::IsFastPackage(const N2kMsg &NMsg)
{
	switch(NMsg.PGN)
	{
		case 126464: return true;
		case 126996: return true;
		case 65240: return true;
		case 126208: return true;
		case 129038: return true;
		case 129039: return true;
		case 129041: return true;
		case 129793: return true;
		case 129794: return true;
		case 129798: return true;
		case 129802: return true;
		case 129809: return true;
		case 129810: return true;
	}	
	return false;
}

int NMEA2000Bus::GetN2kMsg()		//-1: no message avaliable, 0: fast pakage, 1: message in que
{
	CanMsg Msg;
	if(Canbus.GetMessage(&Msg))
	{
		N2kMsg NMsg;
		IdToN2kMsg(NMsg, Msg.id);
		if(IsFastPackage(NMsg))
		{
			bool FullMsg = ParseFastPKG(Msg, NMsg);
			if(FullMsg)							//if we have received the whole message
			{
				MessageQue_.push_back(NMsg);
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			NMsg.DataLen = Msg.header.length;		//Single frame message
			NMsg.Data.resize(Msg.header.length);
			for(int i = 0; i < 8; ++i)
			{
				NMsg.Data[i] = Msg.data[i];
			}
			MessageQue_.push_back(NMsg);
			return 1;
		}
	}
	return -1;			//no message to get
}

bool NMEA2000Bus::ParseFastPKG(CanMsg &Msg, N2kMsg &NMsg)		//return true and overwrite the NMsg if we have received the whole message
{
	IDsID Key = IDsID(Msg.id, Msg.data[0]&0xE0);		//message ID and sequence ID
	uint8_t SequenceNumber = Msg.data[0]&0x1F;

	auto it = BytesLeft_.find(Key);
	if(it != BytesLeft_.end())				//have parts of the message already
	{
#ifdef VERBOSE
		std::cout << "Fast package, SID: "<< (int)(Msg.data[0]&0xE0) <<", SequenceNumber: " << (int)SequenceNumber << ", Bytesleft: " << (int)it->second << std::endl;
#endif
		it->second -= 7;				//decrease bytes left
		for(int i = 1; i < 8; ++i)
		{
			FastPKG_[Key].Data[6+(SequenceNumber-1)*7 +i] = Msg.data[i];
		}
		if(it->second <= 0)				//have the whole message
		{
			NMsg = FastPKG_[Key];
			FastPKG_.erase(Key);
			return true;
		}
		else
		{
			return false;
		}
	}
	else									//not found, create new N2kMsg
	{
		uint8_t BytesInMsg = Msg.data[1];
		NMsg.DataLen = BytesInMsg;
		NMsg.Data.resize(BytesInMsg);
		for(int i = 2; i < 8; ++i)
		{
			NMsg.Data[i-2] = Msg.data[i];
		}
#ifdef VERBOSE
		std::cout << "New fast package, SID: "<< (int)(Msg.data[0]&0xE0) <<", SequenceNumber: " << (int)SequenceNumber << ", Bytesleft: " << (int)BytesInMsg << std::endl;
#endif
		if(BytesInMsg <= 6)
		{
			return true;
		}
		else
		{
			BytesLeft_[Key] = BytesInMsg;		//how many bytes left
			FastPKG_[Key] = NMsg;
			return false;
		}
	}
}
void NMEA2000Bus::CreateN2kMsg(N2kMsg &NMsg, uint8_t Dest)
{
	NMsg.Source = SourceAddress_;
	NMsg.Destination = Dest;
	if(NMsg.PGN == 59904)			//ISO Request
	{
		NMsg.Priority = 6;
		NMsg.DataLen = 3;
		NMsg.Data.resize(NMsg.DataLen);
	}
	else if(NMsg.PGN == 60928)
	{
		NMsg.Priority = 6;
		NMsg.DataLen = 8;
		NMsg.Data.resize(NMsg.DataLen);
		NMsg.Data[0] = UniqueNumber_ & 0xff;
		NMsg.Data[1] = (UniqueNumber_>>8) & 0xff;
		NMsg.Data[2] = (UniqueNumber_>>16) & 0x1f;

		NMsg.Data[2] |= (ManufacturerCode_<<5)&0xff;
		NMsg.Data[3] = ManufacturerCode_ >> 3;
		
		NMsg.Data[4] = DeviceInstance_;
		NMsg.Data[5] = DeviceFunction_;
		NMsg.Data[6] = (DeviceClass_ << 1) | 0x01;

		NMsg.Data[7] = (ArbitraryAddressCapable_<<7) | (IndustryCode_<<4) | SystemInstance_;
	}
	else
	{
#ifdef VERBOSE
		std::cout << "Message not implemented\n";
#endif
	}
}

bool NMEA2000Bus::SendMessage(N2kMsg &NMsg)
{
	if(!IsFastPackage(NMsg))
	{
		CanMsg Msg;
		N2kMsgToId(NMsg, Msg.id);
		Msg.header.ide = 1;
		//Msg.header.rtr = 0;
		Msg.header.length = NMsg.DataLen;
		for(int i = 0; i < NMsg.DataLen; ++i)
		{
			Msg.data[i] = NMsg.Data[i];
		}
		return Canbus.SendMessage(&Msg);
	}
	else
	{
#ifdef VERBOSE
		std::cout << "Fast package sending not implemented\n";
#endif
		return false;
	}
}
uint8_t NMEA2000Bus::CheckForMessages()			//returns address of message if there is one, otherwise returns 0
{
	return Canbus.CheckForMessages();
}


bool CanbusClass::SendMessage(CanMsg *Msg)
{
	return MCP2515_SendMessage(Msg, 0);
}
bool CanbusClass::GetMessage(CanMsg *Msg)
{					
	return MCP2515_GetMessage(Msg, 0);
}
uint8_t CanbusClass::CheckForMessages()			//returns address of messge if there is one, otherwise returns 0
{
	//return (!digitalRead(MCP2515_INT));		//use interrupt pin
	return(MCP2515_CheckMessage());
}
bool CanbusClass::Init(int SPISpeed)
{
	wiringPiSetup();
	//pinMode(MCP2515_INT, INPUT);					//set the interrupt pin to input
	if(wiringPiSPISetup(CHANNEL, SPISpeed) == -1)	//channel, SPI speed
	{
#ifdef VERBOSE
		std::cout << "SPI Setup failed\n";
#endif
		return false;
	}
	else
	{
#ifdef VERBOSE
		std::cout << "SPI Setup successfull\n";
#endif
	}
	return MCP2515_Init();
}

void CanbusClass::SetNormalMode()
{
	uint8_t Mode = 0x00;
	uint8_t Mask = (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0);
	MCP2515_BitModify(CANCTRL, Mask, Mode);
	uint8_t Status = MCP2515_Read(CANSTAT);	//wait untill it changes
	while((Status & Mask) != Mode)
	{
		Status = MCP2515_Read(CANSTAT);
	}
}
void CanbusClass::SetSleepMode()
{
	uint8_t Mode = 0x20;
	uint8_t Mask = (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0);
	MCP2515_BitModify(CANCTRL, Mask, Mode);
	uint8_t Status = MCP2515_Read(CANSTAT);	//wait untill it changes
	while((Status & Mask) != Mode)
	{
		Status = MCP2515_Read(CANSTAT);
	}
}
void CanbusClass::SetLoopBackMode()
{
	uint8_t Mode = 0x40;
	uint8_t Mask = (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0);
	MCP2515_BitModify(CANCTRL, Mask, Mode);
	uint8_t Status = MCP2515_Read(CANSTAT);	//wait untill it changes
	while((Status & Mask) != Mode)
	{
		Status = MCP2515_Read(CANSTAT);
	}
}
void CanbusClass::SetListenOnlyMode()
{
	uint8_t Mode = 0x60;
	uint8_t Mask = (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0);
	MCP2515_BitModify(CANCTRL, Mask, Mode);
	uint8_t Status = MCP2515_Read(CANSTAT);	//wait untill it changes
	while((Status & Mask) != Mode)
	{
		Status = MCP2515_Read(CANSTAT);
	}
}
void CanbusClass::SetConfigMode()
{
	uint8_t Mode = 0x80;
	uint8_t Mask = (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0);
	MCP2515_BitModify(CANCTRL, Mask, Mode);
	uint8_t Status = MCP2515_Read(CANSTAT);	//wait untill it changes
	while((Status & Mask) != Mode)
	{
		Status = MCP2515_Read(CANSTAT);
	}
}
void CanbusClass::SetFilterAndMask(int ReceiveBuffer, int FilterIndex, uint32_t Filter, uint32_t Mask)
{
	uint8_t FSIDH, FSIDL, FEID8, FEID0, MSIDH, MSIDL, MEID8, MEID0, RXBCTRL;
	switch(FilterIndex)
	{
		case 0:
			FSIDH = RXF0SIDH;
			FSIDL = RXF0SIDL;
			FEID8 = RXF0EID8;
			FEID0 = RXF0EID0;
			break;
		case 1:
			FSIDH = RXF1SIDH;
			FSIDL = RXF1SIDL;
			FEID8 = RXF1EID8;
			FEID0 = RXF1EID0;
			break;
		case 2:
			FSIDH = RXF2SIDH;
			FSIDL = RXF2SIDL;
			FEID8 = RXF2EID8;
			FEID0 = RXF2EID0;
			break;
		case 3:
			FSIDH = RXF3SIDH;
			FSIDL = RXF3SIDL;
			FEID8 = RXF3EID8;
			FEID0 = RXF3EID0;
			break;
		case 4:
			FSIDH = RXF4SIDH;
			FSIDL = RXF4SIDL;
			FEID8 = RXF4EID8;
			FEID0 = RXF4EID0;
			break;
		case 5:
			FSIDH = RXF5SIDH;
			FSIDL = RXF5SIDL;
			FEID8 = RXF5EID8;
			FEID0 = RXF5EID0;
			break;
#ifdef VERBOSE
		default: std::cout << "Not a valid filter\n";
#endif
	}
	if(ReceiveBuffer == 0)
	{
		MSIDH = RXM0SIDH;
		MSIDL = RXM0SIDL;
		MEID8 = RXM0EID8;
		MEID0 = RXM0EID0;
		RXBCTRL = RXB0CTRL;
	}
	else if(ReceiveBuffer == 1)
	{
		MSIDH = RXM1SIDH;
		MSIDL = RXM1SIDL;
		MEID8 = RXM1EID8;
		MEID0 = RXM1EID0;
		RXBCTRL = RXB1CTRL;
	}
	else
	{
#ifdef VERBOSE
		std::cout << "Not a valid buffer\n";
#endif
	}
	SetConfigMode();

	MCP2515_Write(RXB0CTRL, (0<<RXM1) | (0<<RXM0) | (1<<BUKT));	//make filters active on buffer 0
	MCP2515_Write(RXB1CTRL, (0<<RXM1) | (0<<RXM0));	//make filters active on buffer 1
//	MCP2515_Write(RXBCTRL, (0<<RXM1) | (0<<RXM0));	//make filters active on selected buffer
	MCP2515_Write(MSIDH, Mask>>21);
	MCP2515_Write(MSIDL, ((Mask>>13) & 0xE0) | ((Mask>>16) & 0x03));
	MCP2515_Write(MEID8, Mask>>8);
	MCP2515_Write(MEID0, Mask);

	MCP2515_Write(FSIDH, Filter>>21);
	MCP2515_Write(FSIDL, ((Filter>>13) & 0xE0) | (1<<EXIDE) | ((Filter>>16) & 0x03));
	MCP2515_Write(FEID8, Filter>>8);
	MCP2515_Write(FEID0, Filter);

//	MCP2515_Write(FSIDH, (1<<7)|(1<<6));
//	MCP2515_Write(FSIDL, (1<<EXIDE));
//	MCP2515_Write(FEID8, 0);
//	MCP2515_Write(FEID0, 0);
//	MCP2515_Write(MSIDH, (1<<7)|(1<<6));
//	MCP2515_Write(MSIDL, 0);
//	MCP2515_Write(MEID8, 0);
//	MCP2515_Write(MEID0, 0);

	SetNormalMode();
}
