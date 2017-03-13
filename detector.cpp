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
