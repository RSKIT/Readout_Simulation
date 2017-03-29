#include "detector.h"

Detector::Detector() : 
        addressname(""), address(0), rocvector(std::vector<ReadoutCell>()), currentstate(PullDown)
{
	
}

Detector::Detector(std::string addressname, int address)
{
	this->addressname = addressname;
	this->address = address;
	rocvector = std::vector<ReadoutCell>();
        currentstate = PullDown;
}

std::string Detector::GetAddressName()
{
	return addressname;
}
void Detector::SetAddressName(std::string addressname)
{
	this->addressname = addressname;
}
	
int Detector::GetAddress()
{
	return address;
}

void Detector::SetAddress(int address)
{
	this->address = address;
}
	
ReadoutCell* Detector::GetROC(int index)
{
	if (index < rocvector.size())
		return &rocvector[index];
	else
		return NULL;
}

ReadoutCell* Detector::GetROCAddress(int address)
{
	for (auto it = rocvector.begin(); it != rocvector.end(); it++)
	{
		if (it->GetAddress() == address)
			return &(*it);
	}
	return NULL;
}

void Detector::AddROC(ReadoutCell readoutcell)
{
	rocvector.push_back(readoutcell);
}

void Detector::ClearROCVector()
{
	rocvector.clear();
}

std::vector<ReadoutCell>::iterator Detector::GetROCVectorBegin()
{
	return rocvector.begin();
}

std::vector<ReadoutCell>::iterator Detector::GetROCVectorEnd()
{
	return rocvector.end();
}

	
void Detector::SaveHit(Hit hit, std::string filename, bool compact)
{
    std::fstream outfile;
    outfile.open(filename.c_str(), std::ios::out);
    outfile << hit.GenerateString(compact) << std::endl;
    outfile.close();
}

TCoord<double> Detector::GetPosition()
{
	return position;
}

void Detector::SetPosition(TCoord<double> position)
{
	this->position = position;
}
	
TCoord<double> Detector::GetSize()
{
	return size;
}

void Detector::SetSize(TCoord<double> size)
{
    this->size = size;
}

bool Detector::SizeOKROC(ReadoutCell* cell)
{
    for(auto it = cell->GetROCsBegin(); it != cell->GetROCsEnd(); it++)
    {
        if(!SizeOKROC(&(*it)))
            return false;
    }

    for(auto it = cell->GetPixelsBegin(); it != cell->GetPixelsEnd(); it++)
    {
        for (int i = 0; i<3; i++)
        {
        if(it->GetPosition()[i] + it->GetSize()[i] > this->GetPosition()[i] + this->GetSize()[i])
            return false;
        if(it->GetPosition()[i] < this->GetPosition()[i])
            return false;
        }

    }
    return true;
}

bool Detector::SizeOK()
{
    for(auto it = rocvector.begin(); it != rocvector.end(); it++)
    {
        if (!SizeOKROC(&(*it)))
            return false;
    }
    return true;
}

bool Detector::EnlargeSizeROC(ReadoutCell *cell)
{
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
    }
    return corrected;
}

bool Detector::EnlargeSize()
{
    bool corrected = false;
    for(auto it = rocvector.begin(); it != rocvector.end(); it++)
    {
        if(EnlargeSizeROC(&(*it)))
            corrected = true;
    }
    return corrected;
}

void Detector::StateMachine()
{
    switch(currentstate)
    {
        case PullDown:

            /* generate hits
             * set flag1 in pixels
             * */
            break;

        case LdPix:
            /* iterate all pixels ín all rocs
             * flag1-->flag2*/
            break;

        case LdCol:
            /* get hit from first pix (prio logic) of each roc to rocqueue,
             * reset this pix' flags,
             * delete in pix
             * set roc flag */
            break;

        case RdCol:
            /* if flag in roc,
             * return first data (prio logic)
             * pop data in roc,
             * check if hitvector size !=0 --> hitflag
             * */

            break;
    }
    NextState();
}

void Detector::SetState(state nextstate)
{
    this->currentstate = nextstate;
}

Detector::state Detector::GetState()
{
    return currentstate;
}

Detector::state Detector::NextState()
{
    switch(currentstate)
    {
        case PullDown   : return currentstate = LdPix;
        case LdPix      : return currentstate = LdCol;
        case LdCol      : return currentstate = RdCol;
        case RdCol      : return currentstate = PullDown;
    }
}