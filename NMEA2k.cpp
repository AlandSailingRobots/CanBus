#include "mcp2515.h"
#include "Canbus.h"
#include <iostream>
#include <fstream>

int main()
{
	std::cout << "NMEA2000 test\n";
	
#ifdef VERBOSE
	std::cout << "VERBOSE mode\n";
#endif

	NMEA2000Bus N2kBus;
	if(N2kBus.Init(1000000))		//Initialise SPI interface and CAN controller
	{
		std::cout << "NMEA2000 Init ok\n";
	}
	else
	{
		std::cout << "Can't init NMEA2000\n";
		return 0;
	} 
	uint8_t MF = (1<<7)|(1<<6);
	//N2kBus.Canbus.SetFilterAndMask(0, 0, (MF << 21), (MF << 21));		//only priority 6 and up in buffer 0
	//N2kBus.Canbus.SetFilterAndMask(1, 2, (MF << 21), (MF << 21));		//only priority 6 and up in buffer 1

	while(true)
	{
		std::cout << "[S]end, [R]eceive, [L]og or [C]heck status?\n";
		int c = std::cin.get();
		std::cin.ignore(200,'\n');
		if(c == 'c' || c == 'C')
		{
			MCP2515_OutputInfo();
		}
		if(c == 's' || c == 'S')
		{
			N2kMsg NMsg;
			int i = 0;
			int In;
		 	std::cout << "insert message PGN:\n";
		 	std::cin >> std::dec >> NMsg.PGN;
		 	//std::cout << "insert destination:\n";
		 	//std::cin >> std::dec >> NMsg.Destination;
		 	std::cin.ignore(200,'\n');

		 	N2kBus.CreateN2kMsg(NMsg, 255);

		 	std::cout << "insert message data: "<<NMsg.DataLen<<"bytes\n";
		 	while(i < NMsg.DataLen)
		 	{
		 		std::cin >> std::hex >> In;
		 		NMsg.Data[i] = In;			
		 		++i;
		 	}
		 	std::cin.ignore(200,'\n');

		 	if(N2kBus.SendMessage(NMsg))
		 	{
		 		std::cout << "***Message sent***\n";
		 		PrintNMEAMsg(NMsg);
		 		int Num = 0;
		 		int Res = N2kBus.GetN2kMsg();
		 		N2kBus.MessageQue_.reserve(100);
				//while(Num > 0)
				while(Num < 100)
				//while(Res != -1)
				//while(1)
				{
					if(Res == 1)
					{
						//N2kMsg NMsg = N2kBus.MessageQue_[0];
						//N2kBus.MessageQue_.pop_back();
						//PrintNMEAMsg(NMsg);
						++Num;
						//break;
					}
					// else if(Res == 0)
					// {
					// 	//std::cout <<"Part of fast package\n";
					// }
//					else
//					{
//					 	std::cout <<"no message available\n";
//					}
					Res = N2kBus.GetN2kMsg();
				}
				std::cout << "Num: "<< Num << std::endl;

				std::ofstream out("log.txt", std::ofstream::out);
				std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
				std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
				for(auto it = N2kBus.MessageQue_.begin(); it != N2kBus.MessageQue_.end(); ++it)
				{
					PrintNMEAMsg(*it);
				}
				std::cout.rdbuf(coutbuf); //reset to standard output again
				out.close();
		 	}
		 	else
		 	{
		 		std::cout << "ERROR message could not send\n";
		 		continue;
		 	}
		 	std::cin.ignore(200,'\n');
		}
		if(c == 'r' || c == 'R')
		{
			int Res = N2kBus.GetN2kMsg();
			if(Res == 1)
			{
				std::cout << "***Message received***\n";
				N2kMsg NMsg = N2kBus.MessageQue_[0];
				N2kBus.MessageQue_.pop_back();
				PrintNMEAMsg(NMsg);
			}
			else if(Res == 0)
			{
				std::cout <<"Part of fast package\n";
			}
			else if(Res == -1)
			{
				std::cout <<"No messages found\n";
			}
		}
		if(c == 'l' || c == 'L')
		{
			int Num;
			std::cout << "how many?\n";
			std::cin >> std::dec >> Num;
			std::cin.ignore(200,'\n');

			std::ofstream out("log.txt", std::ofstream::out);
			std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
			std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!

			while(Num >= 0)
			{
				int Res = N2kBus.GetN2kMsg();
				if(Res == 1)
				{
					N2kMsg NMsg = N2kBus.MessageQue_[0];
					N2kBus.MessageQue_.pop_back();
					PrintNMEAMsg(NMsg);
					--Num;
				}
				else if(Res == 0)
				{
					std::cout <<"Part of fast package\n";
				}
			}
			std::cout.rdbuf(coutbuf); //reset to standard output again
			out.close();
		}
	}
}
