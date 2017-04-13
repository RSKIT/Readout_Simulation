#include "detector_base.h"

DetectorBase::DetectorBase() : 
        addressname(""), address(0), rocvector(std::vector<ReadoutCell>()), outputfile(""),
        fout(std::fstream()), badoutputfile(""), fbadout(std::fstream()), hitcounter(0), 
        position(TCoord<double>::Null), size(TCoord<double>::Null)
{
	
}

DetectorBase::DetectorBase(std::string addressname, int address) : outputfile(""), 
        fout(std::fstream()), badoutputfile(""), fbadout(std::fstream()), hitcounter(0), 
        position(TCoord<double>::Null), size(TCoord<double>::Null)
{
	this->addressname = addressname;
	this->address = address;
	rocvector = std::vector<ReadoutCell>();
}


DetectorBase::DetectorBase(const DetectorBase& templ) : addressname(templ.addressname),
        address(templ.address), rocvector(templ.rocvector),
        outputfile(templ.outputfile), fout(std::fstream()), badoutputfile(""), 
        fbadout(std::fstream()), hitcounter(0), position(templ.position), size(templ.size)
{

}


std::string DetectorBase::GetAddressName()
{
	return addressname;
}

void DetectorBase::SetAddressName(std::string addressname)
{
	if(addressname != "")
        this->addressname = addressname;
}
	
int DetectorBase::GetAddress()
{
	return address;
}

void DetectorBase::SetAddress(int address)
{
	this->address = address;
}
	
ReadoutCell* DetectorBase::GetROC(int index)
{
	if (index < rocvector.size())
		return &rocvector[index];
	else
		return NULL;
}

ReadoutCell* DetectorBase::GetROCAddress(int address)
{
	for (auto it = rocvector.begin(); it != rocvector.end(); it++)
	{
		if (it->GetAddress() == address)
			return &(*it);
	}
	return NULL;
}

void DetectorBase::AddROC(ReadoutCell readoutcell)
{
	rocvector.push_back(readoutcell);
}

void DetectorBase::ClearROCVector()
{
	rocvector.clear();
}

std::vector<ReadoutCell>::iterator DetectorBase::GetROCVectorBegin()
{
	return rocvector.begin();
}

std::vector<ReadoutCell>::iterator DetectorBase::GetROCVectorEnd()
{
	return rocvector.end();
}

TCoord<double> DetectorBase::GetPosition()
{
	return position;
}

void DetectorBase::SetPosition(TCoord<double> position)
{
	this->position = position;
}
	
TCoord<double> DetectorBase::GetSize()
{
	return size;
}

void DetectorBase::SetSize(TCoord<double> size)
{
    this->size = size;
}

bool DetectorBase::SizeOKROC(ReadoutCell* cell)
{
    for(auto it = cell->GetROCsBegin(); it != cell->GetROCsEnd(); it++)
    {
        if(!SizeOKROC(&(*it)))
            return false;
    }

    for(auto it = cell->GetPixelsBegin(); it != cell->GetPixelsEnd(); it++)
    {
        for (int i = 0; i < 3; i++)
        {
            if(it->GetPosition()[i] + it->GetSize()[i] > this->GetPosition()[i] + this->GetSize()[i])
                return false;
            if(it->GetPosition()[i] < this->GetPosition()[i])
                return false;
        }

    }
    return true;
}

bool DetectorBase::SizeOK()
{
    for(auto it = rocvector.begin(); it != rocvector.end(); it++)
    {
        if (!SizeOKROC(&(*it)))
            return false;
    }
    return true;
}

bool DetectorBase::EnlargeSizeROC(ReadoutCell *cell)
{
    //somewhere here "-nan" values are created...
    bool corrected=false;
    for(auto it = cell->GetROCsBegin(); it != cell->GetROCsEnd(); it++)
    {
        if(EnlargeSizeROC(&(*it)))
            corrected = true;
    }

    for(auto it = cell->GetPixelsBegin(); it != cell->GetPixelsEnd(); it++)
    {
        TCoord<double> parentpos = this->GetPosition();
        TCoord<double> childpos = it->GetPosition();
        TCoord<double> parentsize = this->GetSize();
        TCoord<double> childsize = it->GetSize();

        bool correction = false;

        for (int i = 0; i <3; i++)
        {
            if(childpos[i] < parentpos[i])
            {
                correction = true;
                parentsize[i] += parentpos[i] - childpos[i];
                parentpos[i] = childpos[i];
            }
            if(childpos[i] + childsize[i] > parentpos[i] + parentsize[i])
            {
                correction = true;
                parentsize[i]  = childpos[i] + childsize[i] - parentpos[i];
            }
        }
        if (correction)
        {
            this->SetPosition(parentpos);
            this->SetSize(parentsize);
            corrected = true;
        }

        std::cout << parentpos << " size: " << parentsize << std::endl;
    }
    return corrected;
}

bool DetectorBase::EnlargeSize()
{
    bool corrected = false;
    for(auto it = rocvector.begin(); it != rocvector.end(); it++)
    {
        if(EnlargeSizeROC(&(*it)))
            corrected = true;
    }
    return corrected;
}

void DetectorBase::StateMachine(int timestamp)
{
    StateMachineCkUp(timestamp);
    StateMachineCkDown(timestamp);
}

void DetectorBase::StateMachineCkUp(int timestamp)
{

}

void DetectorBase::StateMachineCkDown(int timestamp)
{

}

bool DetectorBase::PlaceHit(Hit hit)
{
    if (rocvector.size() < 1)
        return false;

    std::string addressname = rocvector.front().GetAddressName();
    for (auto &it : rocvector)
    {
        int address = hit.GetAddress(addressname);
        std::cout << "roc addressname: " << addressname << std::endl;
        std::cout << "hit getaddress: " << address << std::endl;
        if (it.GetAddress() == address)
            return it.PlaceHit(hit);
    }

    return false;
}

void DetectorBase::SaveHit(Hit hit, std::string filename, bool compact)
{
    ++hitcounter;

    std::fstream outfile;
    outfile.open(filename.c_str(), std::ios::out);
    if(!outfile.is_open())
    {
        std::cout << "Could not open outputfile \"" << filename << "\"" << std::endl;
        return;
    }

    outfile << hit.GenerateString(compact) << std::endl;
    outfile.close();
}

bool DetectorBase::SaveHit(Hit hit, bool compact)
{
    ++hitcounter;

    if(!fout.is_open())
    {
        if(outputfile == "")
            return false;
        fout.open(outputfile.c_str(), std::ios::out | std::ios::app);
        if(!fout.is_open())
        {
            std::cout << "Could not open output file \"" << outputfile << "\"." << std::endl;
            return false;
        }
    }

    fout << hit.GenerateString(compact) << std::endl;

    return true;
}

bool DetectorBase::SaveBadHit(Hit hit, bool compact)
{
    ++badhitcounter;

    if(!fbadout.is_open())
    {
        if(badoutputfile == "")
            return false;
        fbadout.open(badoutputfile.c_str(), std::ios::out | std::ios::app);
        if(!fbadout.is_open())
        {
            std::cout << "Could not open output file \"" << outputfile << "\" for bad hits." 
                      << std::endl;
            return false;
        }
    }

    fbadout << hit.GenerateString(compact) << std::endl;

    return true;
}


std::string DetectorBase::GetOutputFile()
{
    return outputfile;
}

void DetectorBase::SetOutputFile(std::string filename)
{
    CloseOutputFile();
    outputfile = filename;
}

void DetectorBase::CloseOutputFile()
{
    if(fout.is_open())
        fout.close();
}

std::string DetectorBase::GetBadOutputFile()
{
    return badoutputfile;
}

void DetectorBase::SetBadOutputFile(std::string filename)
{
    CloseBadOutputFile();
    badoutputfile = filename;
}

void DetectorBase::CloseBadOutputFile()
{
    if(fbadout.is_open())
        fbadout.close();
}

int DetectorBase::GetHitCounter()
{
    return hitcounter;
}

void DetectorBase::ResetHitCounter()
{
    hitcounter = 0;
}


std::string DetectorBase::PrintDetector()
{
	std::stringstream s("");

    s << "Detector Size: " << position << " - " << position + size << std::endl
	  << "Detector " << address << " Contents:\n";
	for(auto it : rocvector)
		s << it.PrintROC(" ");

	return s.str();
}