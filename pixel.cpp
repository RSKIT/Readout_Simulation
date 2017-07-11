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

#include "pixel.h"

Pixel::Pixel() : position(double3d{0,0,0}), size (double3d{0,0,0}), 
	threshold(0), efficiency(0), deadtimescaling(1), detectiondelay(0), deadtimeend(-1),
	hit(Hit()), addressname(""), address(0)
{
	
}

Pixel::Pixel(double3d position, double3d size, 
	std::string addressname, int address, double threshold) : 
	efficiency(1.0), deadtimescaling(1), detectiondelay(0), deadtimeend(-1), hit(Hit())
{
	this->addressname = addressname;
	this->address = address;	
	//hit.AddAddress(addressname, address);
	this->position = position;
	this->size = size;
	this->threshold = threshold;
}

TCoord<double> Pixel::GetPosition()
{
	return position;
}

void Pixel::SetPosition(double3d position)
{
	this->position = position;
}
	
TCoord<double> Pixel::GetSize()
{
	return size;
}

void Pixel::SetSize(double3d size)
{
	this->size = size;
}
	
double Pixel::GetThreshold()
{
	return threshold;
}

void Pixel::SetThreshold(double threshold)
{
	this->threshold = threshold;
}
	
double Pixel::GetEfficiency()
{
	return efficiency;
}

void Pixel::SetEfficiency(double efficiency)
{
	this->efficiency = efficiency;
}
	
double Pixel::GetDeadTimeScaling()
{
	return deadtimescaling;
}

bool Pixel::SetDeadTimeScaling(double factor)
{
	if(factor > 0)
	{
		deadtimescaling = factor;
		return true;
	}
	else
		return false;
}

double Pixel::GetDetectionDelay()
{
	return detectiondelay;
}

bool Pixel::SetDetectionDelay(double delay)
{
	if(delay > 0)
	{
		detectiondelay = delay;
		return true;
	}
	else
		return false;
}

double Pixel::GetDeadTimeEnd()
{
	return deadtimeend;
}

void Pixel::SetDeadTimeEnd(double deadtimeend)
{
	this->deadtimeend = deadtimeend;
}
	
std::string Pixel::GetAddressName()
{
	return addressname;
}

void Pixel::SetAddressName(std::string addressname)
{
	this->addressname = addressname;
}

int Pixel::GetAddress()
{
	return address;
}

void Pixel::SetAddress(int address)
{
	this->address = address;
}
	
bool Pixel::HitIsValid()
{
	return hit.is_valid();
}

Hit Pixel::GetHit(int timestamp, std::stringstream* sbadout)
{
	if(timestamp != -1 && timestamp >= hit.GetDeadTimeEnd())
	{
		//remove hit if it was not read out:
		if(hit.is_valid())
		{
			//write loss to lost hit file_
			hit.AddReadoutTime("NotRead", timestamp);
			if(sbadout != 0)
				*sbadout << hit.GenerateString() << std::endl;
			//remove the hit:
			ClearHit();
		}
	}

	return hit;
}

bool Pixel::CreateHit(Hit hit)
{
	if(hit.GetTimeStamp() <= deadtimeend && deadtimeend != -1)
	{
		if(deadtimeend < hit.GetDeadTimeEnd())
			deadtimeend = hit.GetDeadTimeEnd();
		return false;
	}
	else if(!HitIsValid() || this->hit.GetDeadTimeEnd() < hit.GetTimeStamp())
	{
		deadtimeend = hit.GetDeadTimeEnd();
		this->hit = hit;
		return true;
	}
	else
		return false;
}

void Pixel::ClearHit(bool resetcharge)
{
	hit.SetTimeStamp(-1);
	hit.SetEventIndex(-1);
	hit.SetDeadTimeEnd(-1);
	if(resetcharge)
		hit.SetCharge(-1);
	hit.ClearAddress();
	hit.ClearReadoutTimes();
}

Hit Pixel::LoadHit(int timestamp, std::stringstream* sbadout)
{
	Hit h = GetHit(timestamp, sbadout);
	ClearHit(false);
	return h;
}

bool Pixel::IsEmpty(int timestamp)
{
	return (timestamp >= hit.GetDeadTimeEnd());
}
