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

ROCBuffer::ROCBuffer(ReadoutCell* roc) : cell(roc)
{
	
}


bool ROCBuffer::InsertHit(const Hit& hit)
{
	if(cell->hitqueue.size() < cell->hitqueuelength)
	{
		cell->hitqueue.push_back(hit);
		return true;
	}
	else
		return false;
}



Hit ROCBuffer::GetHit(int timestamp, bool remove)
{
	if(cell->hitqueue.size() == 0)
		return Hit();
	else
	{
		Hit h = cell->hitqueue.front();
		if(h.is_valid() && h.is_available(timestamp))
		{
			if(remove)
			{
				cell->hitqueue.erase(cell->hitqueue.begin());
				//for OneByOneReadout also the child has to be cleared:
				if(cell->rocreadout->ClearChild())
					cell->rocvector[0].buf->GetHit(timestamp, remove);
			}
			return h;
		}
		else
			return Hit();
	}
}

bool ROCBuffer::NoTriggerRemoveHits(int timestamp, std::fstream* fbadout)
{
	return false;
}

bool ROCBuffer::is_full()
{
	return (cell->hitqueue.size() >= cell->hitqueuelength);
}

int ROCBuffer::GetNumHitsEnqueued()
{
	return -1;
}

FIFOBuffer::FIFOBuffer(ReadoutCell* roc) : ROCBuffer(roc)
{
	cell->hitqueue.clear();
}

bool FIFOBuffer::InsertHit(const Hit& hit)
{
	if(cell->hitqueue.size() < cell->hitqueuelength)
	{
		cell->hitqueue.push_back(hit);
		return true;
	}
	else
		return false;
}

Hit FIFOBuffer::GetHit(int timestamp, bool remove)
{
	if(cell->hitqueue.size() == 0)
		return Hit();
	else
	{
		Hit h = cell->hitqueue.front();
		if(h.is_valid() && h.is_available(timestamp))
		{
			if(remove)
			{
				cell->hitqueue.erase(cell->hitqueue.begin());
				//for OneByOneReadout also the child has to be cleared:
				if(cell->rocreadout->ClearChild())
					cell->rocvector[0].GetHit(timestamp, remove);
			}
			return h;
		}
		else
			return Hit();
	}
}

bool FIFOBuffer::NoTriggerRemoveHits(int timestamp, std::fstream* fbadout)
{
	bool delsomething = false;
	auto it = cell->hitqueue.begin();
	while(it != cell->hitqueue.end())
	{
		if(it->GetAvailableTime() == timestamp)
		{
			if(fbadout != 0 && fbadout->is_open())
			{
				it->AddReadoutTime("noTrigger", timestamp);
				*fbadout << it->GenerateString(false) << std::endl;
			}
			
			it = cell->hitqueue.erase(it);

			delsomething = true;
		}
		else
			++it;
	}

	return delsomething;
}

bool FIFOBuffer::is_full()
{
	return (cell->hitqueue.size() >= cell->hitqueuelength);
}

int FIFOBuffer::GetNumHitsEnqueued()
{
	return cell->hitqueue.size();
}

PrioBuffer::PrioBuffer(ReadoutCell* roc) : ROCBuffer(roc)
{
	if(cell == 0)
		return;
	else
	{
		cell->hitqueue.clear();
		for(int i = 0; i < cell->hitqueuelength; ++i)
			cell->hitqueue.push_back(Hit());
	}
}

bool PrioBuffer::InsertHit(const Hit& hit)
{
	for(int i = 0; i < cell->hitqueuelength; ++i)
	{
		if(!cell->hitqueue[i].is_valid())
		{
			cell->hitqueue[i] = hit;
			//add in which buffer the hit was put:
			cell->hitqueue[i].AddReadoutTime(cell->GetAddressName()+"_bufferNumber", i);
			return true;
		}
	}
	return false;
}

Hit PrioBuffer::GetHit(int timestamp, bool remove)
{
	for(int i = 0; i < cell->hitqueuelength; ++i)
	{
		if(cell->hitqueue[i].is_valid() && cell->hitqueue[i].is_available(timestamp))
		{
			Hit h = cell->hitqueue[i];
			if(remove)
			{
				cell->hitqueue[i] = Hit();
				//for OneByOneReadout also the child has to be cleared:
				if(cell->rocreadout->ClearChild())
					cell->rocvector[0].hitqueue[i] = Hit();
			}
			return h;
		}
	}
	return Hit();
}

bool PrioBuffer::NoTriggerRemoveHits(int timestamp, std::fstream* fbadout)
{
	bool delsomething = false;
	auto it = cell->hitqueue.begin();
	while(it != cell->hitqueue.end())
	{
		if(it->GetAvailableTime() == timestamp)
		{
			if(fbadout != 0 && fbadout->is_open())
			{
				it->AddReadoutTime("noTrigger", timestamp);
				*fbadout << it->GenerateString(false) << std::endl;
			}
			
			*it = Hit();

			delsomething = true;
		}
		
		++it;
	}

	return delsomething;
}

bool PrioBuffer::is_full()
{
	for(auto it = cell->hitqueue.begin(); it != cell->hitqueue.end(); ++it)
		if(!it->is_valid())
			return false;

	return true;
}

int PrioBuffer::GetNumHitsEnqueued()
{
	int num = 0;
	for(auto it = cell->hitqueue.begin(); it != cell->hitqueue.end(); ++it)
		if(it->is_valid())
			++num;

	return num;
}

ROCReadout::ROCReadout(ReadoutCell* roc) : cell(roc)
{

}


bool ROCReadout::Read(int timestamp, std::fstream* out)
{
	return false;
}

bool ROCReadout::ClearChild()
{
	return false;
}

NoFullReadReadout::NoFullReadReadout(ReadoutCell* roc) : ROCReadout(roc)
{

}

bool NoFullReadReadout::Read(int timestamp, std::fstream* out)
{
	//do not read at all if the buffer is already full:
	if(cell->buf->is_full())
		return false;

	//to save whether a hit was found
	bool hitfound = false;

	//check all child ROCs for hits:
	for(auto it = cell->rocvector.begin(); it != cell->rocvector.end(); ++it)
	{
		//get a hit from the respective ROC:
		Hit h  = it->buf->GetHit(timestamp);
		if((h.is_valid() && h.is_available(timestamp)) || !cell->zerosuppression)
		{
			h.AddReadoutTime(cell->addressname, timestamp);
			h.SetAvailableTime(timestamp + cell->GetReadoutDelay());
			bool result = cell->buf->InsertHit(h);

			//return on a writing error in own buffer:
			if(!result)
			{
				//log the losing of the hit:
				if(out != 0 && out->is_open())
					*out << h.GenerateString() << std::endl;
				return hitfound;
			}

			//update hit-found flag:
			hitfound |= result;

			//do not continue reading, if the buffer is full
			if(cell->buf->is_full())
				return hitfound;
		}
	}

	return hitfound;
}

bool NoFullReadReadout::ClearChild()
{
	return false;
}

NoOverWriteReadout::NoOverWriteReadout(ReadoutCell* roc) : ROCReadout(roc)
{

}

bool NoOverWriteReadout::Read(int timestamp, std::fstream* out)
{
	//to save whether a hit was found
	bool hitfound = false;

	//check all child ROCs for hits:
	for(auto it = cell->rocvector.begin(); it != cell->rocvector.end(); ++it)
	{
		//get a hit from the respective ROC:
		Hit h  = it->buf->GetHit(timestamp);
		if((h.is_valid() && h.is_available(timestamp)) || !cell->zerosuppression)
		{
			h.AddReadoutTime(cell->addressname, timestamp);
			h.SetAvailableTime(timestamp + cell->GetReadoutDelay());
			if(!cell->buf->InsertHit(h))
			{
				//log the loss of the hit:
				if(out != 0 && out->is_open())
				{
					h.AddReadoutTime("noSpace", timestamp);
					*out << h.GenerateString() << std::endl;
				}
			}
			else
				hitfound = true;
		}
	}

	return hitfound;
}

bool NoOverWriteReadout::ClearChild()
{
	return false;
}

OverWriteReadout::OverWriteReadout(ReadoutCell* roc) : ROCReadout(roc)
{

}

bool OverWriteReadout::Read(int timestamp, std::fstream* out)
{
	//to save whether a hit was found
	bool hitfound = false;

	//check all child ROCs for hits:
	for(auto it = cell->rocvector.begin(); it != cell->rocvector.end(); ++it)
	{
		//get a hit from the respective ROC:
		Hit h  = it->buf->GetHit(timestamp);
		if((h.is_valid() && h.is_available(timestamp)) || !cell->zerosuppression)
		{
			h.AddReadoutTime(cell->addressname, timestamp);
			h.SetAvailableTime(timestamp + cell->GetReadoutDelay());
			if(!cell->buf->InsertHit(h))
			{
				//replace the "oldest" hit:
				Hit oldhit = cell->buf->GetHit(timestamp);
				cell->buf->InsertHit(h);
				//log the loss of the hit:
				if(out != 0 && out->is_open())
				{
					oldhit.AddReadoutTime("overwritten", timestamp);
					*out << oldhit.GenerateString() << std::endl;
				}
			}
			else
				hitfound = true;
		}
	}

	return hitfound;
}

bool OverWriteReadout::ClearChild()
{
	return false;
}

OneByOneReadout::OneByOneReadout(ReadoutCell* roc) : ROCReadout(roc)
{

}

bool OneByOneReadout::Read(int timestamp, std::fstream* out)
{
	bool hitfound = false;

	if(cell->rocvector.size() != 1)
	{
		std::cout << "Error: Wrong number of children at " << cell->GetAddressName() << " "
				  << cell->GetAddress() << std::endl;
		return false;
	}

	if(cell->rocvector[0].hitqueuelength != cell->hitqueuelength)
	{
		std::cout << "Error: Non-Matching buffer sizes at " << cell->GetAddressName() << " "
				  << cell->GetAddress() << std::endl;
				  return false;
	}

	ReadoutCell* child = &(cell->rocvector[0]);
	for(int i = 0; i < cell->hitqueuelength; ++i)
	{
		if(!cell->hitqueue[i].is_valid() && child->hitqueue[i].is_valid() 
				&& child->hitqueue[i].is_available(timestamp))
		{
			//std::cout << "1by1 ok" << std::endl;
			cell->hitqueue[i] = child->hitqueue[i];
			cell->hitqueue[i].AddReadoutTime(cell->GetAddressName(), timestamp);
			cell->hitqueue[i].SetAvailableTime(timestamp + cell->GetReadoutDelay());
			hitfound = true;
		}
		else if(child->hitqueue[i].is_valid() && !child->hitqueue[i].is_available(timestamp))
			std::cout << "available from: " << child->hitqueue[i].GetAvailableTime() << std::endl;
	}

	return hitfound;
}

bool OneByOneReadout::ClearChild()
{
	return true;
}

PixelReadout::PixelReadout(ReadoutCell* roc) : cell(roc)
{

}


bool PixelReadout::Read(int timestamp, std::fstream* out)
{
	return false;
}


PPtBReadout::PPtBReadout(ReadoutCell* roc) : PixelReadout(roc)
{

}

bool PPtBReadout::Read(int timestamp, std::fstream* out)
{
	Hit h;

	for(auto it = cell->pixelvector.begin(); it != cell->pixelvector.end(); ++it)
	{
		Hit ph = it->GetHit(timestamp);

		if(ph.is_valid() && ph.is_available(timestamp))
		{
			ph.AddReadoutTime(cell->addressname, timestamp);
			ph.SetAvailableTime(timestamp + cell->GetReadoutDelay());

			if(!h.is_valid())
				h = ph;
			else
			{
				h.SetAddress(it->GetAddressName(),
								h.GetAddress(it->GetAddressName()) | it->GetAddress());
			}

			if(cell->GetNumPixels() > 1)
			{
				ph.AddReadoutTime("merged", timestamp);
				if(out != 0 && out->is_open())
					*out << ph.GenerateString() << std::endl;
			}

			it->ClearHit();
		}
	}

	if(h.is_valid() || !cell->zerosuppression)
	{
		return cell->buf->InsertHit(h);
	}
	else
		return false;
}
