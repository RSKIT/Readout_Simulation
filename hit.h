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

	void AddAddress(std::string name, int addr);
	int  GetAddress(std::string name);
	bool SetAddress(std::string name, int addr);
    int  AddressSize();

	void ClearAddress();

	std::string GenerateString(bool compact = false);
private:
	int timestamp;
	int eventindex;

	std::map<std::string, int> address;
};

#endif //_HIT
