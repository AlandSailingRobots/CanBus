#ifndef canbus__h
#define canbus__h

#include "mcp2515.h"
#include "MsgParsing.h"
#include <vector>
#include <map>
#include <tuple>

typedef std::tuple<uint32_t,uint8_t> IDsID;

class CanbusClass
{
public:
	CanbusClass() {};
	bool Init(int SPISpeed);
	uint8_t CheckForMessages();

	bool SendMessage(CanMsg *Msg);
	bool GetMessage(CanMsg *Msg);
	
	
	void SetNormalMode();
	void SetSleepMode();
	void SetLoopBackMode();
	void SetListenOnlyMode();
	void SetConfigMode();
	void SetMasksAndActivateFilters(uint32_t Mask1, uint32_t Mask2, bool RollOver);
	void SetFilter(int FilterIndex, uint32_t Filter);
	void SetFilterAndMask(int ReceiveBuffer, int FilterIndex, uint32_t Filter, uint32_t Mask);
};

class NMEA2000Bus
{
public:
	NMEA2000Bus() {};
	bool Init(int SPISpeed);
	uint8_t CheckForMessages();

	void CreateN2kMsg(N2kMsg &NMsg, uint8_t Dest);
	bool SendMessage(N2kMsg &NMsg);
	int GetN2kMsg();

	bool IsFastPackage(const N2kMsg &Msg);
	bool ParseFastPKG(CanMsg &Msg, N2kMsg &NMsg);
	
	//void SendISORequest(uint32_t PGN, uint8_t Destination);
	//void SendISOAddressClaim(uint8_t Address, uint8_t Destination);

	std::map<IDsID, N2kMsg> FastPKG_;
	std::map<IDsID, int> BytesLeft_;		//< <MessageId, sequence id>, Bytes left>
	std::vector<N2kMsg> MessageQue_;
	std::vector<CanMsg> MessageQue2_;
	CanbusClass Canbus;


	///Address claim
	int SourceAddress_ = 1;
	uint32_t UniqueNumber_ = 1; 		//21bits
	uint16_t ManufacturerCode_ = 2017; 	//11bits
	uint8_t DeviceInstance_ = 0;
	uint8_t DeviceFunction_ = 150; 		//Bridge 
	uint8_t DeviceClass_ = 25;			//Inter/Intranetwork Device
	uint8_t IndustryCode_ = 4;			//marine
	uint8_t SystemInstance_ = 0;
	bool ArbitraryAddressCapable_ = false;

	///Product information
	// uint16_t NMEA2000Version_;
	// uint16_t ProductCode_;
	// uint8_t ModelID_[32];
	// uint8_t SoftwareVersionCode_[32];
	// uint8_t ModelVersion_[32];
	// uint8_t ModelSerialCode_[32];
	// uint8_t CertificationLevel_;
	// uint8_t LoadEquivalency_;
};

#endif
