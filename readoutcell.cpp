#include "readoutcell.h"

ReadoutCell::ReadoutCell() : addressname(""), address(0), hitflag(false), 
	hitqueuelength(0), hitqueue(std::queue<Hit>()), pixelvector(std::vector<Pixel>()),
	rocvector(std::vector<ReadoutCell>())
{
	
}
ReadoutCell::ReadoutCell(std::string addressname, int address, int hitqueuelength) : hitflag(false)	
{
	this->addressname = addressname;
	this->address = address;
	this->hitqueuelength = hitqueuelength;
	//hitqueue.clear();
	//pixelvector.clear();
	//rocvector.clear();
}

std::string ReadoutCell::GetAddressName()
{
	return addressname;
}
void ReadoutCell::SetAddressName(std::string addressname)
{
	this->addressname = addressname;
}

int ReadoutCell::GetAddress()
{
		return address;
}

void ReadoutCell::SetAddress(int address)
{
	this->address = address;
}

bool ReadoutCell::GetHitflag()
{
	return hitflag;
}

void ReadoutCell::SetHitflag(bool hitflag)
{
	this->hitflag = hitflag;
}

int ReadoutCell::GetHitqueuelength()
{
	return hitqueuelength;
}
void ReadoutCell::SetHitqueuelength(int hitqueuelength)
{
	this->hitqueuelength = hitqueuelength;
}

bool ReadoutCell::AddHit(Hit hit)
{
	if (hitqueue.size() < hitqueuelength)
	{
		hitqueue.push(hit);
		return true;
	}
	else
		return false;
}

Hit ReadoutCell::GetHit()
{
	return hitqueue.front();
}

bool ReadoutCell::PopHit()
{
	if (hitqueue.size() > 0)
	{
		hitqueue.pop();
		if (hitqueue.size() == 0)
			hitflag = false;
		return true;
	}
	else
		return false;
	
}

Pixel* ReadoutCell::GetPixel(int index)
{
	if (index < pixelvector.size())
		return &pixelvector[index];
	else
		return NULL;
}

Pixel* ReadoutCell::GetPixelAddress(int address)
{
	for (auto it = pixelvector.begin(); it != pixelvector.end(); it++)
	{
		if (it->GetAddress() == address)
			return &(*it);
	}
	return NULL;
}

void ReadoutCell::AddPixel(Pixel pixel)
{
	pixelvector.push_back(pixel);
}

void ReadoutCell::ClearPixelVector()
{
	pixelvector.clear();
}

ReadoutCell* ReadoutCell::GetROC(int index)
{
	if (index < rocvector.size())
		return &rocvector[index];
	else
		return NULL;
}

ReadoutCell* ReadoutCell::GetROCAddress(int address)
{
		for (auto it = rocvector.begin(); it != rocvector.end(); it++)
	{
		if (it->GetAddress() == address)
			return &(*it);
	}
	return NULL;
}

void ReadoutCell::AddROC(ReadoutCell readoutcell)
{
	rocvector.push_back(readoutcell);
}

void ReadoutCell::ClearROCVector()
{
	rocvector.clear();
}

bool ReadoutCell::GetNextHitflag()
{
    return nexthitflag;
}

void ReadoutCell::SetNextHitflag(bool nexthitflag)
{
    this->nexthitflag = nexthitflag;
}

Hit ReadoutCell::GetNextHit()
{
    return nexthit;
}

void ReadoutCell::SetNextHit(Hit nexthit)
{
    this->nexthit = nexthit;
}

void ReadoutCell::Apply()
{
    if (AddHit(nexthit))
        hitflag = nexthitflag;
}
