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

#include "readoutcell_functions.h"
#include "readoutcell.h"

ReadoutCell::ReadoutCell() : addressname(""), address(0),
	hitqueuelength(1), hitqueue(std::vector<Hit>()), pixelvector(std::vector<Pixel>()),
	rocvector(std::vector<ReadoutCell>()), zerosuppression(true), buf(0),
    rocreadout(0), pixelreadout(0), readoutdelay(0), triggered(false)
{
	buf          = new FIFOBuffer(this);
    rocreadout   = new NoFullReadReadout(this);
    pixelreadout = new PPtBReadout(this);

    configuration = PPTB | ZEROSUPPRESSION | FIFOBUFFER | NOREADONFULL;
}

ReadoutCell::ReadoutCell(std::string addressname, int address, int hitqueuelength, 
                            int configuration) : hitqueue(std::vector<Hit>()),
        pixelvector(std::vector<Pixel>()), rocvector(std::vector<ReadoutCell>()),
        buf(0), rocreadout(0), pixelreadout(0), zerosuppression(true), readoutdelay(0), 
        triggered(false)
{
	this->addressname = addressname;
	this->address = address;
	this->hitqueuelength = hitqueuelength;

    SetConfiguration(configuration);
}

ReadoutCell::ReadoutCell(const ReadoutCell& roc) : addressname(roc.addressname), 
        address(roc.address), hitqueue(roc.hitqueue), hitqueuelength(roc.hitqueuelength),
        pixelvector(roc.pixelvector), rocvector(roc.rocvector), buf(0), rocreadout(0), 
        pixelreadout(0), zerosuppression(roc.zerosuppression), readoutdelay(roc.readoutdelay),
        triggered(roc.triggered)
{
    SetConfiguration(roc.configuration);

    if(roc.pixelreadout->NeedsROCReset())
    {
        ComplexReadout* ro = new ComplexReadout(this);
        ro->SetPixelLogic(new PixelLogic(static_cast<ComplexReadout*>(roc.pixelreadout)
                                                                            ->GetPixelLogic()));
        pixelreadout = ro;
    }
}

/*ReadoutCell::~ReadoutCell()
{
    
    if(buf != 0)
        delete buf;
    if(rocreadout != 0)
        delete rocreadout;
    if(pixelreadout != 0)
        delete pixelreadout;
    
}
*/

int ReadoutCell::GetConfiguration()
{
    return configuration;
}

void ReadoutCell::SetConfiguration(int newconfig)
{
    //std::cout << "ReadoutCell Configuration: " << newconfig << std::endl;

    configuration = newconfig;

    zerosuppression = newconfig & ZEROSUPPRESSION;

    if(pixelreadout != 0)
        delete pixelreadout;

    //if(newconfig & PPTB)  //always true at the moment, as no alternative available
        pixelreadout = new PPtBReadout(this);
    
    if(buf != 0)
        delete buf;

    if(newconfig & PRIOBUFFER)
        buf = new PrioBuffer(this);
    //else if(newconfig & FIFOBUFFER)
    //    buf = new FIFOBuffer(this);
    else
        buf = new FIFOBuffer(this);

    if(rocreadout != 0)
        delete rocreadout;

    //if(newconfig & NOREADONFULL)
    //    rocreadout = new NoFullReadReadout(this);
    /*else*/ if(newconfig & NOOVERWRITE)
        rocreadout = new NoOverWriteReadout(this);
    else if(newconfig & OVERWRITEONFULL)
        rocreadout = new OverWriteReadout(this);
    else if(newconfig & ONEBYONEREADOUT)
        rocreadout = new OneByOneReadout(this);
    else
        rocreadout = new NoFullReadReadout(this);    
}

void ReadoutCell::SetComplexPPtBReadout(PixelReadout* pixelro)
{
    if(pixelro != 0)
        pixelreadout = pixelro;
}


int ReadoutCell::GetReadoutDelay()
{
    return readoutdelay;
}

void ReadoutCell::SetReadoutDelay(int delay)
{
    //if(delay <= 0)
    //    readoutdelay = 0;
    //else
        readoutdelay = delay;

}

bool ReadoutCell::GetZeroSuppression()
{
    return zerosuppression;
}

bool ReadoutCell::GetTriggered()
{
    return triggered;
}

void ReadoutCell::SetTriggered(bool triggered)
{
    this->triggered = triggered;
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

int ReadoutCell::GetHitqueuelength()
{
	return hitqueuelength;
}
void ReadoutCell::SetHitqueuelength(int hitqueuelength)
{
	if(hitqueuelength > 0)
        this->hitqueuelength = hitqueuelength;
}


bool ReadoutCell::AddHit(Hit hit, int timestamp)
{
	hit.AddReadoutTime(addressname, timestamp);
    hit.SetAvailableTime(timestamp + readoutdelay);

    return buf->InsertHit(hit);
}

Hit ReadoutCell::GetHit(int timestamp, bool remove)
{
	return buf->GetHit(timestamp, remove);
}

int ReadoutCell::GetEnqueuedHits()
{
    return buf->GetNumHitsEnqueued();
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

bool ReadoutCell::PlaceHit(Hit hit, int timestamp, std::stringstream* out)
{
    if (rocvector.size() > 0)
    {
        std::string addressname = rocvector.front().GetAddressName();
        for (auto &it : rocvector)
        {
            int address = hit.GetAddress(addressname);
            //std::cout << "roc addressname: " << addressname << std::endl;
            //std::cout << "hit address: " << address << std::endl;
            if (it.GetAddress() == address)
                return it.PlaceHit(hit, timestamp, out);
        }
        //return false; //this way a readoutcell with ROCs and pixels is possible
    }
    else if (pixelvector.size() > 0)
    {
        for (auto &it : pixelvector)
        {
            std::string addressname = it.GetAddressName();
            int address = hit.GetAddress(addressname);
            if (it.GetAddress() == address)
            {
                bool result = it.CreateHit(hit);
                if(!result)
                {
                    hit.AddReadoutTime("PixelFull", hit.GetTimeStamp() + 1);
                    *out << hit.GenerateString() << std::endl;
                }
                return result;
            }
        }

        //if the hit was valid, the execution would not be reach this point, so the hit is invalid
        if(out != 0)
        {
            hit.AddReadoutTime("PixelNotFound", hit.GetTimeStamp() + 1);
            *out << hit.GenerateString() << std::endl;
        }
        return false;
    }
    else
        return false;
}

bool ReadoutCell::LoadPixel(int timestamp, std::stringstream* out)
{
    bool result = false;
    for(auto it = rocvector.begin(); it != rocvector.end(); ++it)
        result |= it->LoadPixel(timestamp, out);

    result |= pixelreadout->Read(timestamp, out);

    return result;
}

bool ReadoutCell::LoadCell(std::string addressname, int timestamp, std::stringstream* out)
{
    bool result = false;
    for(auto it = rocvector.begin(); it != rocvector.end(); ++it)
    {
        result |= it->LoadCell(addressname, timestamp, out);
    }

    if(addressname.compare(this->addressname) == 0)
        result |= rocreadout->Read(timestamp, out);

    return result;
}

Hit ReadoutCell::ReadCell(int timestamp, bool remove)
{
    return buf->GetHit(timestamp, remove);
}

int ReadoutCell::HitsAvailable(std::string addressname)
{
    int result = 0;

    for(auto it = rocvector.begin(); it != rocvector.end(); ++it)
        result += it->HitsAvailable(addressname);

    if(addressname.compare(this->addressname) == 0)
        result += buf->GetNumHitsEnqueued();
    //take into account that for e.g. OneByOneReadout hits can be counted 2 times:
    else if(addressname.compare("") == 0 && !rocreadout->ClearChild())
        result += buf->GetNumHitsEnqueued();
    

    return result;
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

	s << space << "ROC (" << addressname << "): " << address << " contents:\n";

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

void ReadoutCell::NoTriggerRemoveHits(int timestamp, std::stringstream* sbadout)
{
    for(auto it = rocvector.begin(); it != rocvector.end(); ++it)
        it->NoTriggerRemoveHits(timestamp, sbadout);

    if(triggered)
        buf->NoTriggerRemoveHits(timestamp, sbadout);
}