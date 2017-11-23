/*
    ROME (ReadOut Modelling Environment)
    Copyright © 2017  Rudolf Schimassek (rudolf.schimassek@kit.edu),
                      Felix Ehrler (felix.ehrler@kit.edu),
                      Karlsruhe Institute of Technology (KIT)
                                - ASIC and Detector Laboratory (ADL)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3 as 
    published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    This file is part of the ROME simulation framework.
*/

#include "hit.h"

Hit::Hit() : eventindex(-1), timestamp(-1), charge(-1), deadtimeend(-1), availablefrom(-1)
{
	#ifndef PREFERWRITE
		address 			= std::map<std::string, int>();
		readouttimestamps 	= std::map<std::string, int>();
	#else
		address 			= std::vector<std::pair<std::string, int> >();
		readouttimestamps	= std::vector<std::pair<std::string, int> >();
	#endif
}

Hit::Hit(const Hit& hit) : timestamp(hit.timestamp), eventindex(hit.eventindex), 
		deadtimeend(hit.deadtimeend), charge(hit.charge), availablefrom(hit.availablefrom)
{
	#ifndef PREFERWRITE
		std::map<std::string, int>::const_iterator it;
		for(it = hit.address.begin(); it != hit.address.end(); ++it)
			address.insert(*it);
		for(it = hit.readouttimestamps.begin(); it != hit.readouttimestamps.end(); ++it)
			readouttimestamps.insert(*it);
	#else
		address.insert(address.end(), hit.address.begin(), hit.address.end());
		readouttimestamps.insert(readouttimestamps.end(), hit.readouttimestamps.begin(),
			hit.readouttimestamps.end());
	#endif
}

Hit::Hit(std::string hitdata) : timestamp(-1), eventindex(-1), deadtimeend(-1), charge(-1),
		availablefrom(-1)
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

bool Hit::is_available(int timestamp)
{
	if(timestamp > availablefrom)
		return true;
	else
		return false;
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

int Hit::GetAvailableTime()
{
	return availablefrom;
}

void Hit::SetAvailableTime(int timestamp)
{
	availablefrom = timestamp;
}


void Hit::AddAddress(std::string name, int addr)
{
	#ifndef PREFERWRITE
		address.insert(std::make_pair(name, addr));
	#else
		address.push_back(std::make_pair(name, addr));
	#endif
}

int Hit::GetAddress(std::string name)
{
	#ifndef PREFERWRITE
		std::map<std::string, int>::iterator it = address.find(name);
		if(it != address.end())
			return it->second;
		else
			return -1;
	#else
		std::vector<std::pair<std::string, int> >::iterator it = address.begin();
		while(it != address.end())
		{
			if(it->first.compare(name) == 0)
				return it->second;

			++it;
		}
		return -1;
	#endif
}

bool Hit::SetAddress(std::string name, int addr)
{
	#ifndef PREFERWRITE
		std::map<std::string, int>::iterator it = address.find(name);

		if(it != address.end())
		{
			it->second = addr;
			return true;
		}
		else
			return false;
	#else
		std::vector<std::pair<std::string, int> >::iterator it = address.begin();
		while(it != address.end())
		{
			if(it->first.compare(name) == 0)
			{
				it->second = addr;
				return true;
			}

			++it;
		}
		return false;
	#endif
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
	#ifndef PREFERWRITE
		readouttimestamps.insert(std::make_pair(name, timestamp));
	#else
		readouttimestamps.push_back(std::make_pair(name, timestamp));
	#endif
}

int Hit::GetReadoutTime(const std::string name)
{
	#ifndef PREFERWRITE
		std::map<std::string, int>::iterator it = readouttimestamps.find(name);

		if(it != readouttimestamps.end())
			return it->second;
		else 
			return -1;
	#else
		std::vector<std::pair<std::string, int> >::iterator it = readouttimestamps.begin();
		while(it != readouttimestamps.end())
		{
			if(it->first.compare(name) == 0)
				return it->second;

			++it;
		}
		return -1;
	#endif
}

std::string Hit::FindReadoutTime(std::string namepart)
{
	#ifndef PREFERWRITE
		std::map<std::string, int>::iterator it = readouttimestamps.begin();

		while(it != readouttimestamps.end())
		{
			if(it->first.find(namepart) != std::string::npos)
				return it->first;

			++it;
		}

		return "";
	#else
		std::vector<std::pair<std::string, int> >::iterator it = readouttimestamps.begin();
		while(it != readouttimestamps.end())
		{
			if(it->first.find(namepart) != std::string::npos)
				return it->first;

			++it;
		}
		return "";
	#endif
}

bool Hit::SetReadoutTime(std::string name, int timestamp)
{
	#ifndef PREFERWRITE
		std::map<std::string, int>::iterator it = readouttimestamps.find(name);
		if(it == readouttimestamps.end())
			return false;
		else
		{
			it->second = timestamp;
			return true;
		}
	#else
		std::vector<std::pair<std::string, int> >::iterator it = readouttimestamps.begin();
		while(it != readouttimestamps.end())
		{
			if(it->first.compare(name) == 0)
			{
				it->second = timestamp;
				return true;
			}

			++it;
		}
		return false;
	#endif
}

int Hit::ReadoutTimeSize()
{
	return readouttimestamps.size();
}

void Hit::ClearReadoutTimes()
{
	readouttimestamps.clear();
}

std::string Hit::GenerateTitleString()
{
	std::stringstream s("");

	s << "Event; Timestamp; DeadTimeEnd; Charge; Address: ";

	#ifndef PREFERWRITE
		for(std::map<std::string, int>::iterator it = address.begin(); it != address.end(); ++it)
			s << "(" << it->first << ") ";
	#else
		for(std::vector<std::pair<std::string, int> >::iterator it = address.begin();
				it != address.end(); ++it)
			s << "(" << it->first << ") ";
	#endif

	s << "; Readout Times: ";

	#ifndef PREFERWRITE
		for(std::map<std::string, int>::iterator it = readouttimestamps.begin(); 
				it != readouttimestamps.end(); ++it)
			s << "(" << it->first << ") ";
	#else
		for(std::vector<std::pair<std::string, int> >::iterator it = readouttimestamps.begin();
				it != readouttimestamps.end(); ++it)
			s << "(" << it->first << ") ";
	#endif

	return s.str();
}

std::string Hit::GenerateString(bool compact)
{
	std::stringstream s("");

	static std::streamsize defaultprecision = s.precision();

	if(!compact)
	{
		s << "Event " << eventindex << " Timestamp " << std::setprecision(3) << std::fixed 
		  << timestamp << " DeadTimeEnd " << std::setprecision(3) << std::fixed << deadtimeend;
		s.unsetf(std::ios_base::fixed);
		s << " Charge " << std::setprecision(defaultprecision) << charge << " ; Address:";

		#ifndef PREFERWRITE
			for(std::map<std::string, int>::iterator it = address.begin(); 
						it != address.end(); ++it)
				s << " (" << it->first << ") " << it->second;
		#else
			for(std::vector<std::pair<std::string, int> >::iterator it = address.begin();
						it != address.end(); ++it)
				s << " (" << it->first << ") " << it->second;
		#endif

		s << " ; Readout:";

		#ifndef PREFERWRITE
			for(std::map<std::string, int>::iterator it = readouttimestamps.begin(); 
						it != readouttimestamps.end(); ++it)
				s << " (" << it->first << ") " << it->second;
		#else
			for(std::vector<std::pair<std::string, int> >::iterator it = readouttimestamps.begin(); 
						it != readouttimestamps.end(); ++it)
				s << " (" << it->first << ") " << it->second;
		#endif
	}
	else
	{
		s << eventindex << " " << timestamp << " "
		  << deadtimeend << " " << charge << " ;";

		//address:
		 #ifndef PREFERWRITE
			for(std::map<std::string, int>::iterator it = address.begin(); 
					it != address.end(); ++it)
				s << " " << it->second;
		#else
			for(std::vector<std::pair<std::string, int> >::iterator it = address.begin(); 
					it != address.end(); ++it)
				s << " " << it->second;
		#endif			

		//readouttimestamps:
		s << " ;";
		#ifndef PREFERWRITE
			for(std::map<std::string, int>::iterator it = readouttimestamps.begin(); 
					it != readouttimestamps.end(); ++it)
				s << " " << it->second;
		#else
			for(std::vector<std::pair<std::string, int> >::iterator it = readouttimestamps.begin(); 
					it != readouttimestamps.end(); ++it)
				s << " " << it->second;
		#endif			
	}

	return s.str();
}

bool Hit::operator<(const Hit& second)
{
	return timestamp < second.timestamp;
}