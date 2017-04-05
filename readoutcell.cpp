#include "readoutcell.h"

ReadoutCell::ReadoutCell() : addressname(""), address(0), hitflag(false), 
	hitqueuelength(1), hitqueue(std::queue<Hit>()), pixelvector(std::vector<Pixel>()),
	rocvector(std::vector<ReadoutCell>()), ispptb(false)
{
	
}
ReadoutCell::ReadoutCell(std::string addressname, int address, int hitqueuelength, bool pptb) 
    : hitflag(false), ispptb(pptb)
{
	this->addressname = addressname;
	this->address = address;
	this->hitqueuelength = hitqueuelength;
        this->hitflag = false;
        while (hitqueue.size() != 0)
            hitqueue.pop();
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
	if(hitqueuelength > 0)
        this->hitqueuelength = hitqueuelength;
}

bool ReadoutCell::GetPPtBState()
{
    return ispptb;
}

void ReadoutCell::SetPPtBState(bool pptb)
{
    ispptb = pptb;
}


bool ReadoutCell::AddHit(Hit hit, int timestamp)
{
	if (hitqueue.size() < hitqueuelength)
	{
		hit.AddReadoutTime(addressname, timestamp);
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

int ReadoutCell::GetNumPixels()
{
	return pixelvector.size();
}

std::vector<Pixel>::iterator ReadoutCell::GetPixelsBegin()
{
	return pixelvector.begin();
}

std::vector<Pixel>::iterator ReadoutCell::GetPixelsEnd()
{
	return pixelvector.end();
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

void ReadoutCell::SetNextHit(Hit nexthit, int timestamp)
{
    this->nexthit = nexthit;
    this->nexthitTS = timestamp;
}

void ReadoutCell::Apply()
{
    if (AddHit(nexthit,nexthitTS))
        hitflag = nexthitflag;
}

bool ReadoutCell::PlaceHit(Hit hit)
{
    if (rocvector.size() > 0)
    {
        std::string addressname = rocvector.front().GetAddressName();
        for (auto &it : rocvector)
        {
            int address = hit.GetAddress(addressname);
            std::cout << "roc addressname: " << addressname << std::endl;
            std::cout << "hi getaddress: " << address << std::endl;
            if (it.GetAddress() == address)
                return it.PlaceHit(hit);
        }
        return false;
    }
    else if (pixelvector.size() > 0)
    {
        for (auto &it : pixelvector)
        {
            std::string addressname = it.GetAddressName();
            int address = hit.GetAddress(addressname);
            if (it.GetAddress() == address)
            {
                return it.CreateHit(hit);
            }
        }
        return false;
    }
    else
        return false;
}

bool ReadoutCell::LdPix()
{
    bool returnval = false;
    for (auto it = rocvector.begin(); it!= rocvector.end(); it++)
    {
        if (it->LdPix())
            returnval = true;
    }

    for (auto it = pixelvector.begin(); it != pixelvector.end(); it++)
    {
        if (it->LoadFlag(-1))
        {
            returnval = true;
            std::cout << "flag1: " <<it->GetHitFlag1();
            std::cout << " flag2: " << it->GetHitFlag2() << std::endl;

        }
        if(it->GetHit().GetTimeStamp() != -1)
            std::cout << it->GetHit().GenerateString() << std::endl;
    }

    return returnval;

}

bool ReadoutCell::LdCol()
{
    bool returnval = false;
    for (auto it = rocvector.begin(); it!= rocvector.end(); it++)
    {
        if (it->LdCol())
            return true;
    }

    if(!ispptb)
    {
        for (auto it = pixelvector.begin(); it != pixelvector.end(); ++it)
        {
            if (it->GetHitFlag2())
            {
                std::cout << "addhit to roc" << std::endl;
                std::cout << it->GetHit().GenerateString()<< std::endl;
                if(this->AddHit(it->GetHit()))
                {
                    this->SetHitflag(true);
                    it->ClearFlags();
                    std::cout << "###flags cleared" << std::endl;
                }
                break;
            }
        }
    }
    else    //is a PPtB chip
    {
        int pixeladdress = 0;
        std::string addrname;
        Hit hit;
        for(auto it = pixelvector.begin(); it != pixelvector.end(); ++it)
        {
            if(it->GetHitFlag2())
            {
                addrname = it->GetAddressName();
                hit = it->GetHit();
                pixeladdress |= hit.GetAddress(addrname);
                this->SetHitflag(true);
                it->ClearFlags();
            }
        }
        if(GetHitflag())
        {
            hit.SetAddress(addrname, pixeladdress);
            AddHit(hit);
        }
    }
    return returnval;
}

Hit ReadoutCell::RdCol()
{
    for (auto it = rocvector.begin(); it!= rocvector.end(); it++)
    {
        if (it->GetHitflag())
        {
            Hit hit = it->GetHit();
            it->PopHit();
            return hit;
        }
    }

    std::cout << "roc hitflag: " << this->hitflag << std::endl;
    if (this->hitflag == true)
    {

        Hit hit = hitqueue.front();//this->GetHit();
        if(this->PopHit())
            std::cout << "popped" << std::endl;
        else
            std::cout << "notpopped" << std::endl;
        return hit;
    }

    Hit hit;
    //std::cout << "meep " << this->hitqueue.size() << std::endl;
    return hit;
}

bool ReadoutCell::LoadPixelFlag(int timestamp, std::fstream* out)
{
    bool returnval = false;
    //check child readoutcells for pixels:
    for (auto it = rocvector.begin(); it!= rocvector.end(); it++)
        returnval |= it->LoadPixelFlag(timestamp);

    //check the pixels for hits:
    for (auto it = pixelvector.begin(); it != pixelvector.end(); it++)
    {
        returnval |= it->LoadFlag(timestamp);

        if(it->GetHit().GetTimeStamp() != -1)
            std::cout << it->GetHit().GenerateString() << std::endl;
    }

    return returnval;
}

bool ReadoutCell::LoadPixel(int timestamp, std::fstream* out)
{
    bool returnval = false;

    //check child readout cells for pixels:
    for(auto it = rocvector.begin(); it != rocvector.end(); ++it)
        returnval |= it->LoadPixel(timestamp);

    //check the pixels:
    for(auto it = pixelvector.begin(); it != pixelvector.end(); ++it)
    {
        Hit hit = it->GetHit();
        if(hit.GetAddress(it->GetAddressName()) != -1)
        {
            returnval = true;
            if(!AddHit(hit, timestamp))
            {
                //TODO: save not detected hit
            }
            it->ClearFlags();
        }
    }

    return returnval;
}

bool ReadoutCell::LoadCell(std::string addressname, int timestamp, std::fstream* out)
{

}

Hit ReadoutCell::ReadCell()
{

}

int ReadoutCell::GetNumROCs()
{
	return rocvector.size();
}

std::vector<ReadoutCell>::iterator ReadoutCell::GetROCsBegin()
{
	return rocvector.begin();
}

std::vector<ReadoutCell>::iterator ReadoutCell::GetROCsEnd()
{
	return rocvector.end();

}

std::string ReadoutCell::PrintROC(std::string space)
{
	std::stringstream s("");

	s << space << "ROC " << address << " contents:\n";

	for(auto it : rocvector)
		s << it.PrintROC(space + " ");

	for(auto it : pixelvector)
		s << space << " " << "Pixel " << it.GetAddress() << ": Pos: " << it.GetPosition() 
			<< "; Size: " << it.GetSize() << " Thr: " << it.GetThreshold() << "; Eff: "
            << it.GetEfficiency() << std::endl;

	return s.str();
}

void ReadoutCell::ShiftCell(TCoord<double> distance)
{
    for(auto it = rocvector.begin(); it != rocvector.end(); ++it)
        it->ShiftCell(distance);

    for(auto it = pixelvector.begin(); it != pixelvector.end(); ++it)
        it->SetPosition(it->GetPosition() + distance);
}