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

Hit Pixel::GetHit(int timestamp)
{
	if(timestamp != -1 && timestamp >= hit.GetDeadTimeEnd())
		ClearHit();

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
	else if(!HitIsValid())
	{
		deadtimeend = hit.GetDeadTimeEnd();
		this->hit = hit;
		return true;
	}
	else
		return false;
}

void Pixel::ClearHit()
{
	hit.SetTimeStamp(-1);
	hit.SetEventIndex(-1);
	hit.SetDeadTimeEnd(-1);
	hit.SetCharge(-1);
	hit.ClearAddress();
	hit.ClearReadoutTimes();
}

Hit Pixel::LoadHit(int timestamp)
{
	Hit h = GetHit(timestamp);
	ClearHit();
	return h;
}
