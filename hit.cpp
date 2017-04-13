#include "hit.h"

Hit::Hit() : eventindex(-1), timestamp(-1), charge(-1), deadtimeend(-1)
{
	address = std::map<std::string, int>();
}

Hit::Hit(const Hit& hit) : timestamp(hit.timestamp), eventindex(hit.eventindex), 
		deadtimeend(hit.deadtimeend), charge(hit.charge)
{
	for(auto it = hit.address.begin(); it != hit.address.end(); ++it)
		address.insert(*it);
	for(auto it = hit.readouttimestamps.begin(); it != hit.readouttimestamps.end(); ++it)
		readouttimestamps.insert(*it);
}

Hit::Hit(std::string hitdata) : timestamp(-1), eventindex(-1), deadtimeend(-1), charge(-1)
{
	std::stringstream s("");
	s << hitdata;

	std::string part;
	s >> part;

	if(part.compare("Event") != 0)
		return;
	else
		s >> eventindex >> part;
	if(part.compare("Timestamp") != 0)
		return;
	else
		s >> timestamp >> part;
	if(part.compare("DeadTimeEnd") != 0)
		return;
	else
		s >> deadtimeend >> part;
	if(part.compare("Charge") != 0)
		return;
	else
		s >> charge >> part;

	//Address parts:
	if(part.compare(";") != 0)
		return;
	else
	{
		s >> part;
		if(part.compare("Address:") != 0)
			return;
		s >> part;

		int address;
		while(part.compare(";") != 0)
		{
			part = part.substr(1, part.length()-2);
			s >> address;
			AddAddress(part, address);
			s >> part;
		}
	}

	s >> part;
	if(part.compare("Readout:") != 0)
		return;
	else
	{
		int readtime;
		while(s >> part)
		{
			s >> readtime;
			part = part.substr(1, part.length()-2);
			AddReadoutTime(part, readtime);
		}
	}
}

bool Hit::is_valid()
{
	return (timestamp >= 0 && eventindex >= 0 && charge >= 0 && address.size() > 0);
}

double  Hit::GetTimeStamp()
{
	return timestamp;
}

void Hit::SetTimeStamp(double timestamp)
{
	if(timestamp >= 0)
		this->timestamp = timestamp;
	else
		this->timestamp = -1;
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

double Hit::GetCharge()
{
	return charge;
}

void Hit::SetCharge(double charge)
{
	if(charge >= 0)
		this->charge = charge;
	else
		this->charge = -1;
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
		s << "Event " << eventindex << " Timestamp " << timestamp
		  << " DeadTimeEnd " << deadtimeend << " Charge " << charge
		  << " ; Address:";

		for(auto it = address.begin(); it != address.end(); ++it)
			s << " (" << it->first << ") " << it->second;

		s << " ; Readout:";

		for(auto it = readouttimestamps.begin(); it != readouttimestamps.end(); ++it)
			s << " (" << it->first << ") " << it->second;
	}
	else
	{
		s << eventindex << " " << timestamp << " "
		  << deadtimeend << " " << charge << " ;";

		//address:
		for(auto it = address.begin(); it != address.end(); ++it)
			s << " " << it->second;

		//readouttimestamps:
		s << " ;";
		for(auto it = readouttimestamps.begin(); it != readouttimestamps.end(); ++it)
			s << " " << it->second;
	}

	return s.str();
}
