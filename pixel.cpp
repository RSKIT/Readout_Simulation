#include "pixel.h"

Pixel::Pixel() //: position(TCoord<double>())
{
	
}

Pixel::Pixel(TCoord <double> position, TCoord <double> size, 
	std::string addressname, int address, double threshold) : 
	hitflag1(false), hitflag2(false), efficiency(1.0)
{
	this->addressname = addressname;
	this->address = address;	
	hit = Hit();
	hit.AddAddress(addressname, address);
	this->position = position;
	this->size = size;
	this->threshold = threshold;
}

TCoord<double> Pixel::GetPosition()
{
	return position;
}

void Pixel::SetPosition(TCoord<double> position)
{
	this->position = position;
}
	
TCoord<double> Pixel::GetSize()
{
	return size;
}

void Pixel::SetSize(TCoord<double> size)
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
	
bool Pixel::GetHitFlag1()
{
	return hitflag1;
}

void Pixel::SetHitFlag1(bool hitflag1)
{
	this->hitflag1 = hitflag1;
}
	
bool Pixel::GetHitFlag2()
{
	return hitflag2;
}

void Pixel::SetHitFlag2(bool hitflag2)
{
	this->hitflag2 = hitflag2;
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
	
Hit Pixel::GetHit()
{
	return hit;
}

bool Pixel::CreateHit(int timestamp)
{
	if (!hitflag1)
	{
		hit.SetTimeStamp(timestamp);
		SetHitFlag1(true);
		return true;
	}
	else
		return false;
}
	
bool Pixel::LoadFlag()
{
	if (hitflag1 && !hitflag2)
	{
		hitflag2 = true;
		return true;
	}
	else
		return false;
}
	
void Pixel::ClearFlags()
{
	hitflag1 = false;
	hitflag2 = false;
}