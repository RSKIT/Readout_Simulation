#include "pixel.h"

Pixel::Pixel() : position(TCoord<double>{0,0,0}), size (TCoord<double>{0,0,0}), 
	threshold(0), efficiency(0), hit(Hit()), hitflag1(false), hitflag2(false), 
	addressname(""), address(0), deaduntil(0.)
{
	
}

Pixel::Pixel(TCoord <double> position, TCoord <double> size, 
	std::string addressname, int address, double threshold) : 
	hitflag1(false), hitflag2(false), efficiency(1.0), deaduntil(0.)
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
	
double Pixel::GetDeadTimeEnd()
{
	return deaduntil;
}

void Pixel::SetDeadTimeEnd(double enddeadtime)
{
	deaduntil = enddeadtime;
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
    if (hitflag2)
	return hit;
    else
        return Hit();
}

bool Pixel::CreateHit(Hit hit, double deaduntil)
{
	if(hit.GetTimeStamp() <= this->deaduntil)
	{
		this->deaduntil = deaduntil;
		return false;
	}
	else if (!hitflag1)
	{
        this->hit = hit;
		SetHitFlag1(true);
		this->deaduntil = deaduntil;
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
