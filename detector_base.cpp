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

#include "detector_base.h"

DetectorBase::DetectorBase() : 
        addressname(""), address(0), rocvector(std::vector<ReadoutCell>()), outputfile(""),
        sout(std::string("")),fout(std::fstream()), badoutputfile(""), 
        sbadout(std::string("")),fbadout(std::fstream()), hitcounter(0), 
        position(TCoord<double>::Null), size(TCoord<double>::Null), 
        triggertable(std::deque<int>()), triggertabledepth(0), currenttriggerts(-1), 
        triggertablemask(0)
{
	
}

DetectorBase::DetectorBase(std::string addressname, int address) : outputfile(""),
        sout(std::string("")), fout(std::fstream()), badoutputfile(""), 
        sbadout(std::string("")),fbadout(std::fstream()), hitcounter(0), 
        position(TCoord<double>::Null), size(TCoord<double>::Null), 
        triggertable(std::deque<int>()), triggertabledepth(0), currenttriggerts(-1), 
        triggertablemask(0)
{
	this->addressname = addressname;
	this->address = address;
	rocvector = std::vector<ReadoutCell>();
}


DetectorBase::DetectorBase(const DetectorBase& templ) : addressname(templ.addressname),
        address(templ.address), rocvector(templ.rocvector),
        outputfile(templ.outputfile), fout(std::fstream()), sout(std::string("")),
        badoutputfile(templ.badoutputfile), sbadout(std::string("")),
        fbadout(std::fstream()), hitcounter(0), position(templ.position), size(templ.size),
        triggertabledepth(templ.triggertabledepth), currenttriggerts(templ.currenttriggerts),
        triggertablemask(templ.triggertablemask)
{
    triggertable.clear();
    if(templ.triggertable.size() > 0)
        triggertable.insert(triggertable.end(), templ.triggertable.begin(), 
                                templ.triggertable.end());

    for(auto& it : rocvector)
        it.SetTriggerTableFrontPointer(&currenttriggerts, triggertablemask);
}

DetectorBase::DetectorBase(const DetectorBase* templ) : addressname(templ->addressname),
        address(templ->address), rocvector(templ->rocvector), outputfile(templ->outputfile),
        fout(std::fstream()), sout(std::string("")), badoutputfile(templ->badoutputfile),
        fbadout(std::fstream()), sbadout(std::string("")), hitcounter(0), 
        position(templ->position), size(templ->size), triggertabledepth(templ->triggertabledepth),
        currenttriggerts(templ->currenttriggerts), triggertablemask(templ->triggertablemask)
{
    triggertable.clear();
    if(templ->triggertable.size() > 0)
        triggertable.insert(triggertable.end(), templ->triggertable.begin(), 
                                templ->triggertable.end());

    for(auto& it : rocvector)
        it.SetTriggerTableFrontPointer(&currenttriggerts, triggertablemask);
}

DetectorBase::~DetectorBase()
{
    CloseOutputFile();
    CloseBadOutputFile();
}

void DetectorBase::Cleanup()
{
    for(auto& it : rocvector)
        it.Cleanup();

    rocvector.clear();
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

bool DetectorBase::CheckROCAddresses()
{
    bool changewasnecessary = false;
    std::vector<int> usedaddresses;
    for(auto it = rocvector.begin(); it != rocvector.end(); ++it)
    {
        auto addrit = find(usedaddresses.begin(), usedaddresses.end(), it->GetAddress());
        while(addrit != usedaddresses.end())
        {
            it->SetAddress(it->GetAddress() + 1);
            addrit = find(usedaddresses.begin(), usedaddresses.end(), it->GetAddress());

            changewasnecessary = true;
        }

        usedaddresses.push_back(it->GetAddress());

        changewasnecessary |= it->CheckROCAddresses();
    }

    return changewasnecessary;
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
            if(it->GetPosition()[i] + it->GetSize()[i] 
                    > this->GetPosition()[i] + this->GetSize()[i])
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

bool DetectorBase::StateMachine(int timestamp, bool trigger)
{
    bool result = true;
    result &= StateMachineCkUp(timestamp, trigger);
    result &= StateMachineCkDown(timestamp, trigger);

    return result;
}

bool DetectorBase::StateMachineCkUp(int timestamp, bool trigger, bool print, int updatepitch)
{
    return false;
}

bool DetectorBase::StateMachineCkDown(int timestamp, bool trigger, bool print, int updatepitch)
{
    return false;
}

bool DetectorBase::PlaceHit(Hit hit, int timestamp)
{
    if (rocvector.size() < 1)
        return false;

    std::string addressname = rocvector.front().GetAddressName();
    for (auto &it : rocvector)
    {
        if (it.GetAddress() == hit.GetAddress(addressname))
            return it.PlaceHit(hit, timestamp, &sbadout);
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

    sout += hit.GenerateString(compact) + "\n";

    return true;
}

bool DetectorBase::SaveBadHit(Hit hit, bool compact)
{
    ++badhitcounter;

    sbadout += hit.GenerateString(compact) + "\n";

    return true;
}

bool DetectorBase::FlushOutput()
{
    //make sure that the output file is opened:
    if(sout.length() > 0 && !fout.is_open() && outputfile != "")
    {
        fout.open(outputfile.c_str(), std::ios::out | std::ios::app);
        if(!fout.is_open())
        {
            std::cout << "Could not open outputfile \"" << outputfile << "\"." << std::endl;
            return false;
        }
    }


    if(fout.is_open())
    {
        fout << sout;
        sout = "";
        return true;
    }
    else
    {
        if(sout.length() == 0)
        {
            sout = "";
            return true;
        }
        else
        {
            sout = "";
            return false;
        }
    }
}

bool DetectorBase::FlushBadOutput()
{
    //make sure that the output file is opened:    
    if(sbadout.length() > 0 && !fbadout.is_open() && badoutputfile != "")
    {
        fbadout.open(badoutputfile.c_str(), std::ios::out | std::ios::app);
        if(!fbadout.is_open())
        {
            std::cout << "Could not open outputfile \"" << badoutputfile << "\" for lost hits."
                      << std::endl;
            return false;
        }
    }

    if(fbadout.is_open())
    {
        fbadout << sbadout;
        sbadout = "";
        return true;
    }
    else
    {
        if(sbadout.length() == 0)
        {
            sbadout = "";
            return true;
        }
        else
        {
            sbadout = "";
            return false;
        }
    }
}

std::string DetectorBase::GenerateOutput()
{
    return sout;
}

void DetectorBase::ClearOutput()
{
    sout = "";
}

std::string DetectorBase::GenerateBadOutput()
{
    return sbadout;
}

void DetectorBase::ClearBadOutput()
{
    sbadout = "";
}

int DetectorBase::HitsEnqueued()
{
    int remaining = 0;
    for(auto& it : rocvector)
        remaining += it.HitsAvailable("");

    return remaining;
}

int DetectorBase::HitsAvailable(std::string addressname)
{
    int hits = 0;
    for(auto& it : rocvector)
        hits += it.HitsAvailable(addressname);

    return hits;
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
    {
        FlushOutput();
        fout.close();
    }
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
    {
        FlushBadOutput();
        fbadout.close();
    }
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

    if(triggertabledepth > 0)
        s << "  Trigger Table Depth: " << triggertabledepth << std::endl;
	for(auto it : rocvector)
		s << it.PrintROC(" ");

	return s.str();
}

int DetectorBase::GetState()
{
    return -1;
}

void DetectorBase::SetState(int index)
{

}

int DetectorBase::GetNextState()
{
    return -1;
}

std::string DetectorBase::GetCurrentStateName()
{
    return "";
}

DetectorBase* DetectorBase::Clone()
{
    return new DetectorBase(*this);
}

int DetectorBase::GetNumStates()
{
    return 0;
}

int DetectorBase::GetTriggerTableDepth()
{
    return triggertabledepth;
}

void DetectorBase::SetTriggerTableDepth(int depth)
{
    if(depth >= 0)
        triggertabledepth = depth;
}

int DetectorBase::GetTriggerTableEntries()
{
    return triggertable.size();
}

const int* DetectorBase::GetTriggerTableFrontPointer()
{
    return &currenttriggerts;
}

const int DetectorBase::GetTriggerTimeMask()
{
    return triggertablemask;
}

void DetectorBase::SetTriggerTimeMask(int pattern)
{
    triggertablemask = pattern;
}

int DetectorBase::GetTriggerTableFront()
{
    return currenttriggerts;
}

void DetectorBase::RemoveTriggerTableFront(int timestamp)
{
    if(triggertable.size() > 0)
    {
        //only use the trigger time stamp when its validity period is already over:
        if(triggertable.front() > (timestamp | triggertablemask))
            currenttriggerts = -1;
        else
        {
            currenttriggerts = triggertable.front();
            triggertable.pop_front();
        }
    }
    else
        currenttriggerts = -1;
}

void DetectorBase::ClearTriggerTable()
{
    triggertable.clear();
}

bool DetectorBase::AddTriggerTableEntry(int timestamp)
{
    if(triggertable.size() > 0 && (timestamp | triggertablemask) == triggertable.back())
    {
        std::stringstream s("");
        s << "# TriggerTable signals merged: " << timestamp << std::endl;
        sbadout += s.str();
        return true;
    }

    if(triggertable.size() < triggertabledepth)
    {
        triggertable.push_back(timestamp | triggertablemask);
        std::stringstream s("");
        s << "# TriggerTable entry added: " << timestamp << std::endl;
        sbadout += s.str();
        return true;
    }
    else
    {
        std::stringstream s("");
        s << "# TriggerTable full: " << timestamp << std::endl;
        sbadout += s.str();
        return false;
    }
}

int DetectorBase::WriteRemainingHitsToBadOut(int timestamp)
{
    int hitcounter = 0;
    for(auto& it : rocvector)
        hitcounter += it.RemoveAndSaveAllHits(timestamp, &sbadout);

    return hitcounter;
}
