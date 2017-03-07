#include "hit.h"

Hit::Hit() : eventindex(-1), timestamp(-1)
{
	address = std::map<std::string, int>();
}

Hit::Hit(Hit& hit) : timestamp(hit.timestamp), eventindex(hit.eventindex)
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

void Hit::ClearAddress()
{
	address.clear();
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
	}
	else
	{
		s << eventindex << " " << timestamp;

		for(auto it : address)
		{
			s << " " << it.second;
		}
	}

	return s.str();
}
