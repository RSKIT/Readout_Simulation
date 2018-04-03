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
    rocreadout(0), pixelreadout(0), readoutdelay(0), triggered(false), 
    position(TCoord<double>::Null), size(TCoord<double>::Null), delayreference(""), sampledelay(0)
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
        triggered(false), position(TCoord<double>::Null), size(TCoord<double>::Null),
        delayreference(""), sampledelay(0)
{
	this->addressname = addressname;
	this->address = address;
	this->hitqueuelength = hitqueuelength;

    SetConfiguration(configuration);
}

ReadoutCell::ReadoutCell(const ReadoutCell& roc) : addressname(roc.addressname), 
        address(roc.address), hitqueue(std::vector<Hit>()), hitqueuelength(roc.hitqueuelength),
        pixelvector(std::vector<Pixel>()), rocvector(std::vector<ReadoutCell>()), buf(0), 
        rocreadout(0), pixelreadout(0), zerosuppression(roc.zerosuppression), 
        readoutdelay(roc.readoutdelay), triggered(roc.triggered), 
        position(roc.position), size(roc.size), delayreference(roc.delayreference),
        sampledelay(0)
{
    SetConfiguration(roc.configuration);

    //use the same merging address name as before in case of MergingReadout scheme:
    if(roc.configuration & MERGINGREADOUT)
        static_cast<MergingReadout*>(rocreadout)->SetMergingAddressName(
                    static_cast<MergingReadout*>(roc.rocreadout)->GetMergingAddressName());

    if(roc.pixelreadout->NeedsROCReset())
    {
        ComplexReadout* ro = new ComplexReadout(this);
        ro->SetPixelLogic(new PixelLogic(static_cast<ComplexReadout*>(roc.pixelreadout)
                                                                            ->GetPixelLogic()));
        pixelreadout = ro;
        ro = 0;
    }

    hitqueue.insert(hitqueue.end(), roc.hitqueue.begin(), roc.hitqueue.end());

    pixelvector.insert(pixelvector.end(), roc.pixelvector.begin(), roc.pixelvector.end());
    rocvector.insert(rocvector.end(), roc.rocvector.begin(), roc.rocvector.end());
}

ReadoutCell::~ReadoutCell()
{
    Cleanup();
    
    /*pixelvector.clear();

    for(auto& it : rocvector)
        it.Cleanup();

    rocvector.clear();
    */

    /*if(buf != 0)
        delete buf;
    if(rocreadout != 0)
        delete rocreadout;
    if(pixelreadout != 0)
        delete pixelreadout;
    */
}

void ReadoutCell::Cleanup()
{
    pixelvector.clear();

    for(auto& it : rocvector)
        it.Cleanup();

    rocvector.clear();

    if(pixelreadout != 0)
    {
        delete pixelreadout;
        pixelreadout = 0;
    }
    if(buf != 0)
    {
        delete buf;
        buf = 0;
    }
    if(rocreadout != 0)
    {
        delete rocreadout;
        rocreadout = 0;
    }

}

int ReadoutCell::GetConfiguration()
{
    return configuration;
}

void ReadoutCell::SetConfiguration(int newconfig)
{
    configuration = newconfig;

    zerosuppression = newconfig & ZEROSUPPRESSION;

    if(pixelreadout != 0)
        delete pixelreadout;

    if(newconfig & PPTBORBEFOREEDGE)
        pixelreadout = new PPtBReadoutOrBeforeEdge(this);
    //else if(newconfig & PPTB)
    else
        pixelreadout = new PPtBReadout(this);
    
    if(buf != 0)
        delete buf;

    if(newconfig & PRIOBUFFER)
        buf = new PrioBuffer(this);
    //not necessary, as no third alternative and this is the default choice:
    //else if(newconfig & FIFOBUFFER)
    //    buf = new FIFOBuffer(this);
    else
        buf = new FIFOBuffer(this);

    if(rocreadout != 0)
        delete rocreadout;

    //not necessary as it is the default choice:
    //if(newconfig & NOREADONFULL)
    //    rocreadout = new NoFullReadReadout(this);
    /*else*/ if(newconfig & NOOVERWRITE)
        rocreadout = new NoOverWriteReadout(this);
    else if(newconfig & OVERWRITEONFULL)
        rocreadout = new OverWriteReadout(this);
    else if(newconfig & ONEBYONEREADOUT)
        rocreadout = new OneByOneReadout(this);
    else if(newconfig & TOKENREADOUT)
        rocreadout = new TokenReadout(this);
    else if(newconfig & SORTEDROCREADOUT)
        rocreadout = new SortedROCReadout(this);
    else if(newconfig & MERGINGREADOUT)
        rocreadout = new MergingReadout(this);
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
    readoutdelay = delay;
}

std::string ReadoutCell::GetReadoutDelayReference()
{
    return delayreference;
}

void ReadoutCell::SetReadoutDelayReference(std::string tsname)
{
    delayreference = tsname;
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

TCoord<double> ReadoutCell::GetPosition()
{
    return position;
}

void ReadoutCell::SetPosition(TCoord<double> newposition)
{
    position = newposition;
}

TCoord<double> ReadoutCell::GetSize()
{
    return size;
}

void ReadoutCell::SetSize(TCoord<double> newsize)
{
    size = newsize;
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

    //if this is the first element to be added, its position and size are the ones of the readout
    //  cell:
    if(position == TCoord<double>::Null && size == TCoord<double>::Null)
    {
        position = pixel.GetPosition();
        size     = pixel.GetSize();
    }
    //if this is not the first element, the size has to be tested whether the new pixel is inside
    //  the current readout cell volume:
    else
    {
        //update position and size (use only the new pixel):
        for(int i=0;i<3;++i)
        {
            if(position[i] > pixel.GetPosition()[i])
            {
                size[i] += position[i] - pixel.GetPosition()[i];
                position[i] = pixel.GetPosition()[i];
            }
            if(position[i] + size[i] < pixel.GetPosition()[i] + pixel.GetSize()[i])
                size[i] = pixel.GetPosition()[i] - position[i] + pixel.GetSize()[i];
        }
    }
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

bool ReadoutCell::UpdateSize()
{
    //old values as reference whether something changed:
    TCoord<double> oldposition = position;
    TCoord<double> oldsize     = size;

    //try to load first values from the subordinate objects:
    if(pixelvector.size() > 0)
    {
        position = pixelvector.begin()->GetPosition();
        size     = pixelvector.begin()->GetSize();
    }
    else if(rocvector.size() > 0)
    {
        rocvector.begin()->UpdateSize();
        position = rocvector.begin()->GetPosition();
        size     = rocvector.begin()->GetSize();
        //if first readout cell is empty, fall back on impossible parameters:
        if(position == TCoord<double>::Null && size == TCoord<double>::Null)
        {
            position = TCoord<double>{1e10,1e10,1e10};
            size = TCoord<double>{-1e10,-1e10,-1e10};
        }
    }
    else    //a readout cell without contents does not have an extent
    {
        position = TCoord<double>::Null;
        size     = TCoord<double>::Null;
    }

    for(auto& it : rocvector)
    {
        it.UpdateSize();
        if(it.size.volume() == 0)
            continue;

        for(int i = 0; i < 3; ++i)
        {
            if(it.position[i] < position[i])
            {
                size[i] += position[i] - it.position[i];
                position[i] = it.position[i];
            }
            if(it.position[i] + it.size[i] > position[i] + size[i])
                size[i] = it.position[i] - position[i] + it.size[i];
        }
    }

    for(auto& it : pixelvector)
    {
        for(int i = 0; i < 3; ++i)
        {
            if(it.GetPosition()[i] < position[i])
            {
                size[i] += position[i] - it.GetPosition()[i];
                position[i] = it.GetPosition()[i];
            }
            if(it.GetPosition()[i] + it.GetSize()[i] > position[i] + size[i])
                size[i] = it.GetPosition()[i] - position[i] + it.GetSize()[i];
        }
    }

    return ((position == oldposition) && (size == oldsize));
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

    //Update position and size:
    readoutcell.UpdateSize();

    if(position == TCoord<double>::Null && size == TCoord<double>::Null)
    {
        position = readoutcell.GetPosition();
        size     = readoutcell.GetSize();
    }
    else
    {
        for(int i = 0; i < 3; ++i)
        {
            if(readoutcell.position[i] < position[i])
            {
                size[i] += position[i] - readoutcell.position[i];
                position[i] = readoutcell.position[i];
            }
            if(readoutcell.position[i] + readoutcell.size[i] > position[i] + size[i])
                size[i] = readoutcell.position[i] - position[i] + readoutcell.size[i];
        }        
    }
}

void ReadoutCell::ClearROCVector()
{
	rocvector.clear();
}

bool ReadoutCell::PlaceHit(Hit hit, int timestamp, std::string* out)
{
    if (rocvector.size() > 0)
    {
        std::string addressname = rocvector.front().GetAddressName();
        for (auto &it : rocvector)
        {
            int address = hit.GetAddress(addressname);
            if (it.GetAddress() == address)
                return it.PlaceHit(hit, timestamp, out);
        }
    }
    
    if (pixelvector.size() > 0)
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
                    *out += hit.GenerateString() + "\n";
                }
                return result;
            }
        }

        //if the hit was valid, the execution would not be reach this point, so the hit is invalid
        if(out != 0)
        {
            hit.AddReadoutTime("PixelNotFound", hit.GetTimeStamp() + 1);
            *out += hit.GenerateString() + "\n";
        }
        return false;
    }
    else
    {
        if(out != 0)
        {
            hit.AddReadoutTime(GetAddressName(), timestamp);
            hit.AddReadoutTime("EmptyROC", timestamp);
            *out += hit.GenerateString() + "\n";
        }
        return false;
    }
}

bool ReadoutCell::LoadPixel(int timestamp, std::string* out)
{
    bool result = false;
    for(auto it = rocvector.begin(); it != rocvector.end(); ++it)
        result |= it->LoadPixel(timestamp, out);

    result |= pixelreadout->Read(timestamp, out);

    return result;
}

bool ReadoutCell::LoadCell(std::string addressname, int timestamp, std::string* out)
{
    bool result = false;
    for(auto it = rocvector.begin(); it != rocvector.end(); ++it)
        result |= it->LoadCell(addressname, timestamp, out);

    if(addressname.compare(this->addressname) == 0)
        result |= rocreadout->Read(timestamp, out);

    return result;
}

Hit ReadoutCell::ReadCell(int timestamp, bool remove)
{
    return buf->GetHit(timestamp, remove);
}

int ReadoutCell::HitsAvailable(std::string testaddressname)
{
    int result = 0;

    for(auto it = rocvector.begin(); it != rocvector.end(); ++it)
        result += it->HitsAvailable(testaddressname);

    if(testaddressname.compare(this->addressname) == 0)
        result += buf->GetNumHitsEnqueued();
    //to count all hits in the detector:
    //  take into account that for e.g. OneByOneReadout hits can be counted 2 times
    //    (ClearChild() == true):
    else if(testaddressname.compare("") == 0 && !rocreadout->ClearChild())
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
    s << space << " ( Position: " << position << "; Size: " << size << " )\n";

	for(auto& it : rocvector)
		s << it.PrintROC(space + " ");

	for(auto& it : pixelvector)
		s << space << " Pixel " << it.GetAddress() << ": Pos: " << it.GetPosition() 
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

    UpdateSize();
}

void ReadoutCell::NoTriggerRemoveHits(int timestamp, std::string* sbadout)
{
    for(auto it = rocvector.begin(); it != rocvector.end(); ++it)
        it->NoTriggerRemoveHits(timestamp, sbadout);

    if(triggered)
        buf->NoTriggerRemoveHits(timestamp, sbadout);
}

bool ReadoutCell::CheckROCAddresses()
{
    bool changedanaddress = false;
    std::vector<int> usedaddresses;
    for(auto it = rocvector.begin(); it != rocvector.end(); ++it)
    {
        auto addrit = find(usedaddresses.begin(), usedaddresses.end(), it->GetAddress());
        while(addrit != usedaddresses.end())
        {
            it->SetAddress(it->GetAddress() + 1);
            addrit = find(usedaddresses.begin(), usedaddresses.end(), it->GetAddress());

            changedanaddress = true;
        }

        usedaddresses.push_back(it->GetAddress());

        changedanaddress |= it->CheckROCAddresses();
    }

    return changedanaddress;
}

void ReadoutCell::SetTriggerTableFrontPointer(const int* front, const int clearpattern)
{
    for(auto& it : rocvector)
        it.SetTriggerTableFrontPointer(front, clearpattern);

    auto rocr = dynamic_cast<SortedROCReadout*>(rocreadout);

    if(rocr != 0)
    {
        rocr->SetTriggerTableFrontPointer(front);
        rocr->SetTriggerPattern(clearpattern);
    }
}

int ReadoutCell::RemoveAndSaveAllHits(int timestamp, std::string* sbadout)
{
    int hitcounter = 0;

    //remove hits from subordinate readout cells:
    for(auto& it : rocvector)
        hitcounter += it.RemoveAndSaveAllHits(timestamp, sbadout);

    //remove hits from the pixels:
    for(auto& it : pixelvector)
    {
        Hit h = it.GetHit(timestamp, sbadout);
        if(h.is_valid())
        {
            h.AddReadoutTime("SimulationEnd",timestamp);
            if(sbadout != NULL)
                *sbadout += h.GenerateString() + "\n";

            ++hitcounter;
        }
    }

    //remove hits from own hitqueue:
    for(auto& it : hitqueue)
    {
        if(it.is_valid())
        {
            it.AddReadoutTime("SimulationEnd", timestamp);
            if(sbadout != NULL)
                *sbadout += it.GenerateString() + "\n";

            //avoid double counting as for OneByOne readout:
            if(!rocreadout->ClearChild())
                ++hitcounter;
        }
    }

    return hitcounter;
}

void ReadoutCell::SetMergingAddressName(std::string addressname)
{
    MergingReadout* ro = 0;

    ro = dynamic_cast<MergingReadout*>(rocreadout);

    if(ro != 0)
        ro->SetMergingAddressName(addressname);
}

double ReadoutCell::GetSampleDelay()
{
    return sampledelay;
}

bool ReadoutCell::SetSampleDelay(double delay)
{
    if(delay < readoutdelay)
    {
        sampledelay = delay;
        return true;
    }
    else
        return false;
}

