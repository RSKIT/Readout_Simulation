#include "hit.h"

Hit::Hit() : eventindex(-1), timestamp(-1)
{
	address = std::map<std::string, int>();
}

Hit::Hit(const Hit& hit) : timestamp(hit.timestamp), eventindex(hit.eventindex)
{
	for(auto it : hit.address)
		address.insert(it);
}

int  Hit::GetTimeStamp()
{
	return timestamp;
}

void Hit::SetTimeStamp(int timestamp)
{
	if(timestamp >= -1)
		this->timestamp = timestamp;
}

int Hit::GetEventIndex()
{
	return eventindex;
}

void Hit::SetEventIndex(int index)
{
	if(index >= -1)
		eventindex = index;
}

double Hit::GetDeadTimeEnd()
{
	return deadtimeend;
}

void Hit::SetDeadTimeEnd(double time)
{
	deadtimeend = time;
}


void Hit::AddAddress(std::string name, int addr)
{
	address.insert(std::make_pair(name, addr));
}

int Hit::GetAddress(std::string name)
{
	auto it = address.find(name);
	if(it != address.end())
		return it->second;
	else
		return -1;
}

bool Hit::SetAddress(std::string name, int addr)
{
	auto it = address.find(name);

	if(it != address.end())
	{
		it->second = addr;
		return true;
	}
	else
		return false;		
}

int  Hit::AddressSize()
{
    return address.size();
}

void Hit::ClearAddress()
{
	address.clear();
}

void Hit::AddReadoutTime(std::string name, int timestamp)
{
	readouttimestamps.insert(std::make_pair(name, timestamp));
}

int Hit::GetReadoutTime(std::string name)
{
	auto it = readouttimestamps.find(name);

	if(it != readouttimestamps.end())
		return it->second;
	else 
		return -1;
}

bool Hit::SetReadoutTime(std::string name, int timestamp)
{
	auto it = readouttimestamps.find(name);
	if(it == readouttimestamps.end())
		return false;
	else
	{
		it->second = timestamp;
		return true;
	}
}

int Hit::ReadoutTimeSize()
{
	return readouttimestamps.size();
}

void Hit::ClearReadoutTimes()
{
	readouttimestamps.size();
}

std::string Hit::GenerateString(bool compact)
{
	std::stringstream s("");

	if(!compact)
	{
		s << "Event " << eventindex << " Timestamp " << timestamp << " Address";

		for(auto it : address)
		{
			s << " (" << it.first << ") " << it.second;
		}

		s << "; Readout:";

		for(auto it: readouttimestamps)
		{
			s << " (" << it.first << ") " << it.second;
		}
	}
	else
	{
		s << eventindex << " " << timestamp;

		//address:
		for(auto it : address)
			s << " " << it.second;

		//readouttimestamps:
		s << " ;";
		for(auto it : readouttimestamps)
			s << " " << it.second;
	}

	return s.str();
}
