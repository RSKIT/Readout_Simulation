#include "readoutcell_functions.h"
#include "readoutcell.h"

ReadoutCell::ReadoutCell() : addressname(""), address(0),
	hitqueuelength(1), hitqueue(std::vector<Hit>()), pixelvector(std::vector<Pixel>()),
	rocvector(std::vector<ReadoutCell>()), zerosuppression(true), buf(0),
    rocreadout(0), pixelreadout(0)
{
	buf          = new FIFOBuffer(this);
    rocreadout   = new NoFullReadReadout(this);
    pixelreadout = new PPtBReadout(this);
}

ReadoutCell::ReadoutCell(std::string addressname, int address, int hitqueuelength, 
                            int configuration) : hitqueue(std::vector<Hit>()),
        pixelvector(std::vector<Pixel>()), rocvector(std::vector<ReadoutCell>()),
        buf(0), rocreadout(0), pixelreadout(0)
{
	this->addressname = addressname;
	this->address = address;
	this->hitqueuelength = hitqueuelength;

    SetConfiguration(configuration);
}

ReadoutCell::ReadoutCell(const ReadoutCell& roc) : addressname(roc.addressname), 
        address(roc.address), hitqueue(roc.hitqueue), hitqueuelength(roc.hitqueuelength),
        pixelvector(roc.pixelvector), rocvector(roc.rocvector), buf(0), rocreadout(0), 
        pixelreadout(0), configuration(roc.configuration), zerosuppression(roc.zerosuppression)
{
    SetConfiguration(configuration);
}

ReadoutCell::~ReadoutCell()
{
    /*
    if(buf != 0)
        delete buf;
    if(rocreadout != 0)
        delete rocreadout;
    if(pixelreadout != 0)
        delete pixelreadout;
    */
}

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
    else
        rocreadout = new NoFullReadReadout(this);    
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

    return buf->InsertHit(hit);
}

Hit ReadoutCell::GetHit()
{
	return buf->GetHit();
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

bool ReadoutCell::PlaceHit(Hit hit, std::fstream* fout)
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
                bool result = it.CreateHit(hit);
                if(fout != 0 && fout->is_open())
                {
                    hit.AddReadoutTime("PixelFull", hit.GetTimeStamp() + 1);
                    *fout << hit.GenerateString() << std::endl;
                }
                return result;
            }
        }
        return false;
    }
    else
        return false;
}

bool ReadoutCell::LoadPixel(int timestamp, std::fstream* out)
{
    bool result = false;
    for(auto it = rocvector.begin(); it != rocvector.end(); ++it)
        result |= it->LoadPixel(timestamp, out);

    result |= pixelreadout->Read(timestamp, out);

    return result;
}

bool ReadoutCell::LoadCell(std::string addressname, int timestamp, std::fstream* out)
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

Hit ReadoutCell::ReadCell()
{
    return buf->GetHit();
}

int ReadoutCell::HitsAvailable(std::string addressname)
{
    int result = 0;

    for(auto it = rocvector.begin(); it != rocvector.end(); ++it)
        result += it->HitsAvailable(addressname);

    if(addressname.compare(this->addressname) == 0 || addressname.compare("") == 0)
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