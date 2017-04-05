#ifndef _HIT
#define _HIT

#include <string>
#include <sstream>
#include <map>
#include <utility>

class Hit
{
public:
	Hit();
	Hit(const Hit& hit);

	int  GetTimeStamp();
	void SetTimeStamp(int timestamp);

	int  GetEventIndex();
	void SetEventIndex(int index);

	double GetDeadTimeEnd();
	void SetDeadTimeEnd(double time);

	void AddAddress(std::string name, int addr);
	int  GetAddress(std::string name);
	bool SetAddress(std::string name, int addr);
    int  AddressSize();

	void ClearAddress();

	void AddReadoutTime(std::string name, int timestamp);
	int GetReadoutTime(std::string name);
	bool SetReadoutTime(std::string name, int timestamp);
	int ReadoutTimeSize();
	void ClearReadoutTimes();

	std::string GenerateString(bool compact = false);
private:
	int timestamp;
	int eventindex;
	double deadtimeend;

	std::map<std::string, int> address;

	std::map<std::string, int> readouttimestamps;
};

#endif //_HIT
