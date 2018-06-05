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

ROCBuffer::~ROCBuffer()
{
	cell = 0;
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

bool ROCBuffer::NoTriggerRemoveHits(int timestamp, std::string* sbadout)
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

bool FIFOBuffer::NoTriggerRemoveHits(int timestamp, std::string* sbadout)
{
	bool delsomething = false;
	auto it = cell->hitqueue.begin();
	while(it != cell->hitqueue.end())
	{
		if(it->GetAvailableTime() == timestamp)
		{
			if(sbadout != 0)
			{
				it->AddReadoutTime("noTrigger", timestamp);
				*sbadout += it->GenerateString(false) + "\n";
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

bool PrioBuffer::NoTriggerRemoveHits(int timestamp, std::string* sbadout)
{
	bool delsomething = false;
	auto it = cell->hitqueue.begin();
	while(it != cell->hitqueue.end())
	{
		if(it->GetAvailableTime() == timestamp)
		{
			if(sbadout != 0)
			{
				it->AddReadoutTime("noTrigger", timestamp);
				*sbadout += it->GenerateString(false) + "\n";
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

ROCReadout::~ROCReadout()
{
	cell = 0;
}

bool ROCReadout::Read(int timestamp, std::string* out)
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

bool NoFullReadReadout::Read(int timestamp, std::string* out)
{
	//do not read at all if the buffer is already full:
	if(cell->buf->is_full())
		return false;

	//to save whether a hit was found:
	bool hitfound = false;

	//check all child ROCs for hits:
	for(auto it = cell->rocvector.begin(); it != cell->rocvector.end(); ++it)
	{
		//get a hit from the respective ROC:
		Hit h  = it->buf->GetHit(timestamp);
		if((h.is_valid() && h.is_available(timestamp)) || !cell->zerosuppression)
		{
			if(it->GetTriggered())
				h.AddReadoutTime(it->GetAddressName() + "_Trigger", h.GetAvailableTime());
			h.AddReadoutTime(cell->addressname, timestamp);
			if(cell->GetReadoutDelayReference() == "")
				h.SetAvailableTime(timestamp + cell->GetReadoutDelay());
			else
				h.SetAvailableTime(h.GetReadoutTime(cell->GetReadoutDelayReference()) 
										+ cell->GetReadoutDelay());
			bool result = cell->buf->InsertHit(h);

			//return on a writing error in own buffer:
			if(!result)
			{
				//log the losing of the hit:
				if(out != 0)
					*out += h.GenerateString() + "\n";
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

bool NoOverWriteReadout::Read(int timestamp, std::string* out)
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
			if(it->GetTriggered())
				h.AddReadoutTime(it->GetAddressName() + "_Trigger", h.GetAvailableTime());
			h.AddReadoutTime(cell->addressname, timestamp);
			if(cell->GetReadoutDelayReference() == "")
				h.SetAvailableTime(timestamp + cell->GetReadoutDelay());
			else
				h.SetAvailableTime(h.GetReadoutTime(cell->GetReadoutDelayReference()) 
										+ cell->GetReadoutDelay());
			if(!cell->buf->InsertHit(h))
			{
				//log the loss of the hit:
				if(out != 0)
				{
					h.AddReadoutTime("noSpace", timestamp);
					*out += h.GenerateString() + "\n";
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

bool OverWriteReadout::Read(int timestamp, std::string* out)
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
			if(it->GetTriggered())
				h.AddReadoutTime(it->GetAddressName() + "_Trigger", h.GetAvailableTime());
			h.AddReadoutTime(cell->addressname, timestamp);
			if(cell->GetReadoutDelayReference() == "")
				h.SetAvailableTime(timestamp + cell->GetReadoutDelay());
			else
				h.SetAvailableTime(h.GetReadoutTime(cell->GetReadoutDelayReference()) 
										+ cell->GetReadoutDelay());
			if(!cell->buf->InsertHit(h))
			{
				//replace the "oldest" hit:
				Hit oldhit = cell->buf->GetHit(timestamp);
				cell->buf->InsertHit(h);
				//log the loss of the hit:
				if(out != 0)
				{
					oldhit.AddReadoutTime("overwritten", timestamp);
					*out += oldhit.GenerateString() + "\n";
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

bool OneByOneReadout::Read(int timestamp, std::string* out)
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
			cell->hitqueue[i] = child->hitqueue[i];
			//add trigger time information:
			if(child->GetTriggered())
				cell->hitqueue[i].AddReadoutTime(child->GetAddressName() + "_Trigger",
													cell->hitqueue[i].GetAvailableTime());
			cell->hitqueue[i].AddReadoutTime(cell->GetAddressName(), timestamp);
			if(cell->GetReadoutDelayReference() == "")
				cell->hitqueue[i].SetAvailableTime(timestamp + cell->GetReadoutDelay());
			else
				cell->hitqueue[i].SetAvailableTime(
					cell->hitqueue[i].GetReadoutTime(cell->GetReadoutDelayReference()) 
							+ cell->GetReadoutDelay());
			hitfound = true;
		}
		//output from when the hit will be readable:
		//else if(child->hitqueue[i].is_valid() && !child->hitqueue[i].is_available(timestamp))
		//	std::cout << "available from: " << child->hitqueue[i].GetAvailableTime() << std::endl;
	}

	return hitfound;
}

bool OneByOneReadout::ClearChild()
{
	return true;
}

TokenReadout::TokenReadout(ReadoutCell* roc) : ROCReadout(roc), currentindex(0)
{

}

bool TokenReadout::Read(int timestamp, std::string* out)
{
	int children = cell->rocvector.size();
	int startindex = currentindex;	//to save the starting point as a break index
	bool hitfound = false;

	Hit h;

	bool start = true;	//to start checking the other buffers of the ROC used the last time
	while(currentindex != startindex || start)
	{
		if(currentindex == startindex)
			start = false;

		//check for a hit in the ROC:
		h = cell->rocvector[currentindex].buf->GetHit(timestamp, false);

		if((h.is_valid() && h.is_available(timestamp)) || !cell->zerosuppression)
		{
			//add the trigger timestamp when the ROC was triggered:
			if(cell->rocvector[currentindex].GetTriggered())
				h.AddReadoutTime(cell->rocvector[currentindex].GetAddressName() + "_Trigger", 
									h.GetAvailableTime());

			//add the readout timestamp of this ROC:
			h.AddReadoutTime(cell->addressname, timestamp);
			if(cell->GetReadoutDelayReference() == "")
				h.SetAvailableTime(timestamp + cell->GetReadoutDelay());
			else
				h.SetAvailableTime(h.GetReadoutTime(cell->GetReadoutDelayReference()) 
										+ cell->GetReadoutDelay());
			bool result = cell->buf->InsertHit(h);

			//return on a writing error in own buffer:
			if(!result)
			{
				//log the losing of the hit (not necessary for NoFullRead readout): 
				//if(out != 0)
				//	*out += h.GenerateString() + "\n";
				return hitfound;
			}

			//delete the hit from the readout cell
			cell->rocvector[currentindex].buf->GetHit(timestamp, true);

			//update hit-found flag:
			hitfound |= result;

			//do not continue reading, if the buffer is full
			if(cell->buf->is_full())
				return hitfound;			

		}

		//go to the next subordinate ROC:
		if(currentindex < children - 1)
			++currentindex;
		else
			currentindex = 0;
	}

	return hitfound;
}

bool TokenReadout::ClearChild()
{
	return false;
}

SortedROCReadout::SortedROCReadout(ReadoutCell* roc) : ROCReadout(roc), triggertablefront(NULL),
		pattern(0)
{

}

SortedROCReadout::~SortedROCReadout()
{
	triggertablefront = NULL;
}

bool SortedROCReadout::Read(int timestamp, std::string* out)
{
	//get the timestamp which is to be read out:
	static bool notinitialised = true;
	int timestamptoread = -1;
	if(triggertablefront != NULL)
		timestamptoread = (*triggertablefront) | pattern; 
									//pattern application already done in trigger table emplacement
	else if(notinitialised)
	{
		std::cerr << "SortedROCReadout: Detector not initialised!" << std::endl;
		notinitialised = false;
	}

	//do not read at all if the buffer is already full:
	if(cell->buf->is_full())
		return false;

	//to save whether a hit was found:
	bool hitfound = false;

	//check all child ROCs for hits:
	for(auto it = cell->rocvector.begin(); it != cell->rocvector.end(); ++it)
	{
		//get a hit from the respective ROC:
		Hit h  = it->buf->GetHit(timestamp, false);
			//read without deleting as the event timestamp may be wrong

		if((h.is_valid() && h.is_available(timestamp)) || !cell->zerosuppression)
		{
			//check for the correct time stamp parts (exclude bits as stored in this object)
			std::string triggerfieldname = h.FindReadoutTime("_Trigger");
			if(timestamptoread != -1 
				&& timestamptoread != (h.GetReadoutTime(triggerfieldname) | pattern))
				continue;

			it->buf->GetHit(timestamp, true);	//delete the hit from the subordinate ReadoutCell

			//add the trigger timestamp when the ROC was triggered:
			if(it->GetTriggered())
				h.AddReadoutTime(it->GetAddressName() + "_Trigger", h.GetAvailableTime());

			h.AddReadoutTime(cell->addressname, timestamp);
			if(cell->GetReadoutDelayReference() == "")
				h.SetAvailableTime(timestamp + cell->GetReadoutDelay());
			else
				h.SetAvailableTime(h.GetReadoutTime(cell->GetReadoutDelayReference()) 
										+ cell->GetReadoutDelay());
			bool result = cell->buf->InsertHit(h);

			//return on a writing error in own buffer:
			if(!result)
			{
				//log the losing of the hit:
				if(out != 0)
					*out += h.GenerateString() + "\n";
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

bool SortedROCReadout::ClearChild()
{
	return false;
}

void SortedROCReadout::SetTriggerTableFrontPointer(const int* front)
{
	triggertablefront = front;
}

void SortedROCReadout::SetTriggerPattern(int clearpattern)
{
	pattern = clearpattern;
}

MergingReadout::MergingReadout(ReadoutCell* roc) : ROCReadout(roc), mergingaddress("")
{

}

MergingReadout::~MergingReadout()
{

}

bool MergingReadout::Read(int timestamp, std::string* out)
{
	if(mergingaddress.compare("") == 0)
	{
		std::cerr << "Error: Merging Address Missing" << std::endl;
		return false;
	}

	Hit h;
	for(auto it = cell->rocvector.begin(); it != cell->rocvector.end(); ++it)
	{
		Hit bhit = it->buf->GetHit(timestamp);
		if(bhit.is_valid() && bhit.is_available(timestamp))
		{
			if(it->GetTriggered())
				bhit.AddReadoutTime(it->GetAddressName() + "_Trigger", h.GetAvailableTime());
			bhit.AddReadoutTime(cell->addressname, timestamp);
			if(cell->GetReadoutDelayReference() == "")
				bhit.SetAvailableTime(timestamp + cell->GetReadoutDelay());
			else
				bhit.SetAvailableTime(h.GetReadoutTime(cell->GetReadoutDelayReference()) 
										+ cell->GetReadoutDelay());

			if(!h.is_valid())
				h = bhit;
			else
			{
				h.SetAddress(mergingaddress, 
								h.GetAddress(mergingaddress) | bhit.GetAddress(mergingaddress));
				h.SetCharge(h.GetCharge() + bhit.GetCharge());
			}

			bhit.AddReadoutTime("ROCMerge", timestamp);
			*out += bhit.GenerateString() + "\n";
		}
	}

	bool hitfound = false;
	if(h.is_valid())
	{
		if(!cell->buf->InsertHit(h))
		{
			h.AddReadoutTime("noSpace", timestamp);
			*out += h.GenerateString() + "\n";
		}
		else
			hitfound = true;
	}

	return hitfound;
}

bool MergingReadout::ClearChild()
{
	return false;
}

std::string MergingReadout::GetMergingAddressName()
{
	return mergingaddress;
}

void MergingReadout::SetMergingAddressName(std::string addressname)
{
	mergingaddress = addressname;
}


PixelReadout::PixelReadout(ReadoutCell* roc) : cell(roc)
{

}

PixelReadout::~PixelReadout()
{
	cell = 0;
}

bool PixelReadout::Read(int timestamp, std::string* out)
{
	return false;
}

bool PixelReadout::NeedsROCReset()
{
	return false;
}


PPtBReadout::PPtBReadout(ReadoutCell* roc) : PixelReadout(roc)
{

}

bool PPtBReadout::Read(int timestamp, std::string* out)
{
	Hit h = Hit();
	double hitsampletime = -1e10; //time at which the pixel pattern is stored after a hit
	bool result = false;

	// allow several hits per timestamp if the delay between them is larger than the sampledelay:
	do{			//...}while(hitsampletime != -1e10);

		hitsampletime = -1e10;

		std::vector<Hit> remergedhits;

		//find the evaluation time for the group:
		for(auto it = cell->pixelvector.begin(); it != cell->pixelvector.end(); ++it)
		{
			Hit htest = it->GetHit(timestamp, out, cell->sampledelay + 1);
			if(htest.is_valid())
			{
				if(hitsampletime == -1e10)
					hitsampletime = htest.GetTimeStamp() + cell->sampledelay;
				else if(hitsampletime > htest.GetTimeStamp() + cell->sampledelay)
					hitsampletime = htest.GetTimeStamp() + cell->sampledelay;
			}
		}
		if(hitsampletime != -1e10)
			hitsampletime += 1e-5;

		for(auto it = cell->pixelvector.begin(); it != cell->pixelvector.end(); ++it)
		{
			//log the loss of pixel hits due to late sampling:
			if(it->GetDeadTimeEnd() < hitsampletime && it->HitIsValid())
			{
				Hit ph = it->LoadHit(-1, out); //get the hit in any case
				ph.AddReadoutTime("SampleDelayLoss", timestamp);
				if(out != 0)
					*out += ph.GenerateString() + "\n";

				//prepare and place a dummy hit to maintain the readout occupancy for a hit not read
				//  in time:

				ph.SetAddress(it->GetAddressName(), 0);
				ph.SetDeadTimeEnd(hitsampletime+1e-5);
				ph.SetCharge(0);
				ph.ClearReadoutTimes();

				it->CreateHit(ph);
			}
		}

		//enable sampledelay exceeding a timestamp value (e.g. hit TS = 3.9 with sampledelay = 0.3
		//		for TS 4):
		if(hitsampletime > timestamp)
			return result;

		if(hitsampletime != -1e10)
		{
			for(auto it = cell->pixelvector.begin(); it != cell->pixelvector.end(); ++it)
			{
				Hit ph = it->LoadHit(hitsampletime, out);

				if(ph.is_valid())
				{
					//write out the merging of this hit to the lost hit file before decorating it:
					if(cell->GetNumPixels() > 1)
					{
						ph.AddReadoutTime("merged", timestamp);	
						if(out != 0)
							*out += ph.GenerateString() + "\n";
					}

					//use this hit as group hit if it is the first hit pixel in the group:
					if(!h.is_valid())
					{
						ph.AddReadoutTime(cell->addressname, timestamp);
						if(cell->GetReadoutDelayReference() == "")
							ph.SetAvailableTime(timestamp + cell->GetReadoutDelay());
						else
							ph.SetAvailableTime(ph.GetReadoutTime(cell->GetReadoutDelayReference()) 
													+ cell->GetReadoutDelay());
						//add the still dead pixels to the hit read before this pixel:
						if(h.GetAddress(it->GetAddressName()) != -1)
						{
							ph.SetAddress(it->GetAddressName(), it->GetAddress()
															| h.GetAddress(it->GetAddressName()));
							ph.SetCharge(ph.GetCharge() + h.GetCharge());
						}

						h = ph;
					}
					//add the pixel address if it is not the first hit pixel in the group:
					else
					{
						h.SetAddress(it->GetAddressName(),
										h.GetAddress(it->GetAddressName()) | it->GetAddress());
						h.SetCharge(h.GetCharge() + ph.GetCharge());
					}
				}
				else if(!it->IsEmpty(hitsampletime)) // && h.is_valid())
				{
					if(out != NULL)
					{
						Hit sph;
						sph.SetAddress(it->GetAddressName(), it->GetAddress());
						sph.SetCharge(ph.GetCharge());
						sph.AddReadoutTime("remerged", timestamp);
						remergedhits.push_back(sph);
						//*out += sph.GenerateString() + "\n";
					}

					//first dead pixel without a hit in the group:
					if(h.GetAddress(it->GetAddressName()) == -1)
					{
						h.SetAddress(it->GetAddressName(), it->GetAddress());
						h.SetCharge(ph.GetCharge());
					}
					//add address and charge if not the first pixel:
					else
					{
						h.SetAddress(it->GetAddressName(), 
										h.GetAddress(it->GetAddressName()) | it->GetAddress());
						h.SetCharge(h.GetCharge() + ph.GetCharge());
					}

				}
			}
		}

		if(h.is_valid() || !cell->zerosuppression)
		{
			if(cell->buf->InsertHit(h))
				result = true;
			else if(h.is_valid())
			{
				h.AddReadoutTime("BufferFull", timestamp);
				if(out != NULL)
					*out += h.GenerateString() + "\n";
			}
		}

		if(out != NULL && h.is_valid())
		{
			for(auto& it : remergedhits)
			{
				h.SetAddress(cell->pixelvector.front().GetAddressName(), 
					it.GetAddress(cell->pixelvector.front().GetAddressName()));
				h.SetCharge(it.GetCharge());
				h.ClearReadoutTimes();
				h.AddReadoutTime("remerged", it.GetReadoutTime("remerged"));
				*out += h.GenerateString() + "\n";
			}

			remergedhits.clear();
		}

		h = Hit();

	}while(hitsampletime != -1e10);

	return result;
}

PPtBReadoutOrBeforeEdge::PPtBReadoutOrBeforeEdge(ReadoutCell* roc) : PixelReadout(roc)
{

}

bool PPtBReadoutOrBeforeEdge::Read(int timestamp, std::string* out)
{
	Hit h;
	double hitsampletime = -1e10; //time at which the pixel pattern is stored after a hit
	double groupdeadtimeend = 1e10;	//time at which the comparator goes low again

	//find the evaluation time for the group:
	for(auto it = cell->pixelvector.begin(); it != cell->pixelvector.end(); ++it)
	{
		Hit htest = it->GetHit(timestamp, out, cell->sampledelay + 1);
		if(htest.is_valid())
		{
			if(hitsampletime == -1e10)
			{
				hitsampletime = htest.GetTimeStamp() + cell->sampledelay;
				groupdeadtimeend = htest.GetDeadTimeEnd();
			}
			else if(hitsampletime > htest.GetTimeStamp() + cell->sampledelay)
				hitsampletime = htest.GetTimeStamp() + cell->sampledelay;

			if(groupdeadtimeend < htest.GetDeadTimeEnd())
				groupdeadtimeend = htest.GetDeadTimeEnd();
		}
	}
	hitsampletime += 1e-5;

	bool alreadyhigh = false;

	for(auto it = cell->pixelvector.begin(); it != cell->pixelvector.end(); ++it)
	{
		if(!it->IsEmpty(hitsampletime) && !it->HitIsValid())
			alreadyhigh = true;

		// tag hits with longer time walk and earlier dead time ending:
		if(it->HitIsValid() && it->IsEmpty(hitsampletime) 
				&& it->GetDeadTimeEnd() < groupdeadtimeend)
		{
			Hit ph = it->LoadHit(-1, out);
			ph.AddReadoutTime("GroupDeadShort", timestamp);
			if(out != 0)
				*out += ph.GenerateString() + "\n";
		}

		//log the loss of pixel hits due to late sampling:
		if(it->GetDeadTimeEnd() < hitsampletime && it->HitIsValid())
		{
			Hit ph = it->LoadHit(-1, out); //get the hit in any case
			ph.AddReadoutTime("SampleDelayLoss", timestamp);
			if(out != 0)
				*out += ph.GenerateString() + "\n";

			//prepare and place a dummy hit to maintain the readout occupancy for a hit not read
			//  in time:

			ph.SetAddress(it->GetAddressName(), 0);
			ph.SetDeadTimeEnd(hitsampletime+1e-5);
			ph.SetCharge(0);
			ph.ClearReadoutTimes();	//remove the "SampleDelayLoss" Tag from the hit

			it->CreateHit(ph);
		}
	}

	//enable sampledelay exceeding a timestamp value (e.g. hit TS = 3.9 with sampledelay = 0.3
	//		for TS 4):
	if(hitsampletime > timestamp)
		return false;

	for(auto it = cell->pixelvector.begin(); it != cell->pixelvector.end(); ++it)
	{
		Hit ph = it->LoadHit(hitsampletime, out);

		if(alreadyhigh)
		{
			if(ph.is_valid())
			{
				ph.AddReadoutTime("GroupDead", timestamp);
				if(out != 0)
					*out += ph.GenerateString() + "\n";
			}
		}
		else
		{
			//log the merging of the hit if it is valid:
			if(cell->GetNumPixels() > 1 && ph.is_valid())
			{
				ph.AddReadoutTime("merged", timestamp);	
				if(out != 0)
					*out += ph.GenerateString() + "\n";
			}

			//use the first hit pixel in the group for the further way:
			if(!h.is_valid())
			{
				if(cell->GetReadoutDelayReference() == "")
					ph.SetAvailableTime(timestamp + cell->GetReadoutDelay());
				else
					ph.SetAvailableTime(ph.GetReadoutTime(cell->GetReadoutDelayReference())
											+ cell->GetReadoutDelay());
				ph.AddReadoutTime(cell->addressname, timestamp);
				
				h = ph;
			}
			//add the pixel's address if it is not the first hit pixel in the group:
			else if(ph.is_valid())
			{
				h.SetAddress(it->GetAddressName(),
								h.GetAddress(it->GetAddressName()) | it->GetAddress());
				h.SetCharge(h.GetCharge() + ph.GetCharge());
			}
		}
	}

	if(h.is_valid() || !cell->zerosuppression)
	{
		if(cell->buf->InsertHit(h))
			return true;
		else if(h.is_valid())
		{
			h.AddReadoutTime("BufferFull", timestamp);
			if(out != NULL)
				*out += h.GenerateString() + "\n";

			return false;
		}
		else
			return false;
	}
	else
		return false;
}


PixelLogic::PixelLogic(int relation)
{
	this->relation = relation;
}

PixelLogic::PixelLogic(PixelLogic* logic) : relation(logic->relation)
{
	for(auto& it : logic->pixels)
		pixels.push_back(it);

	for(auto& it : logic->ownpixels)
		ownpixels.push_back(it);

	for(auto& it : logic->notownpixels)
		notownpixels.push_back(it);

	for(auto& it : logic->sublogics)
		sublogics.push_back(new PixelLogic(it));
}

PixelLogic::PixelLogic(const PixelLogic& logic) : relation(logic.relation)
{
	for(auto& it : logic.pixels)
		pixels.push_back(it);

	for(auto& it : logic.ownpixels)
		ownpixels.push_back(it);

	for(auto& it : logic.notownpixels)
		notownpixels.push_back(it);

	for(auto& it : logic.sublogics)
		sublogics.push_back(new PixelLogic(it));
}

PixelLogic::~PixelLogic()
{
	for(auto& it : sublogics)
		delete it;

	sublogics.clear();
}

void PixelLogic::AddPixelAddress(int address, bool ownpixel)
{
	pixels.push_back(address);
	if(ownpixel)
		ownpixels.push_back(address);
	else
		notownpixels.push_back(address);
}

void PixelLogic::ClearPixelAddresses()
{
	pixels.clear();
}

int  PixelLogic::GetNumPixelAddresses()
{
	return pixels.size();
}

int  PixelLogic::GetNumOwnPixelAddresses()
{
	return ownpixels.size();
}

void PixelLogic::AddPixelLogic(PixelLogic* sublogic)
{
	sublogics.push_back(sublogic);
	FindNewPixels(&ownpixels, &notownpixels);
}

void PixelLogic::AddPixelLogic(const PixelLogic& sublogic)
{
	sublogics.push_back(new PixelLogic(sublogic));
	FindNewPixels(&ownpixels, &notownpixels);
}

void PixelLogic::ClearPixelLogic()
{
	sublogics.clear();
}

int  PixelLogic::GetNumPixelSubLogics()
{
	return sublogics.size();
}

int  PixelLogic::GetRelation()
{
	return relation;
}

void PixelLogic::SetRelation(int relation)
{
	if(relation < 0 || relation > 6)
		this->relation = -1;
	else
		this->relation = relation;
}

void PixelLogic::FindNewPixels(std::vector<int>* masterown, std::vector<int>* masternotown)
{
	for(auto& it : sublogics)
		it->FindNewPixels(masterown, masternotown);

	for(auto& it : ownpixels)
	{
		auto finder = find(masterown->begin(), masterown->end(), it);
		if(finder == masterown->end())
			masterown->push_back(it);
	}

	for(auto& it : notownpixels)
	{
		auto finder = find(masternotown->begin(), masternotown->end(), it);
		if(finder == masternotown->end())
			masternotown->push_back(it);
	}

	if(masterown == &ownpixels)
		std::sort(masterown->begin(), masterown->end());
	if(masternotown == &notownpixels)
		std::sort(masternotown->begin(), masternotown->end());
}

bool PixelLogic::Evaluate(ReadoutCell* cell, int timestamp)
{
	bool goodresult = true;
	switch(relation)
	{
		case(Nor):
			goodresult = false;
		case(Or):
			for(auto& it : pixels)
				if(!cell->GetPixelAddress(it)->IsEmpty(timestamp))
					return goodresult;
			for(auto& it : sublogics)
				if(it->Evaluate(cell, timestamp))
					return goodresult;
			return !goodresult;
			break;
		case(Nand):
			goodresult = false;
		case(And):
			for(auto& it : pixels)
				if(cell->GetPixelAddress(it)->IsEmpty(timestamp))
					return !goodresult;
			for(auto& it : sublogics)
				if(!it->Evaluate(cell, timestamp))
					return !goodresult;
			return goodresult;
			break;
		case(Xor):
		case(Xnor):
		{
			int resultcounter = 0;
			for(auto& it : pixels)
				if(!cell->GetPixelAddress(it)->IsEmpty(timestamp))
					++resultcounter;
			for(auto& it : sublogics)
				if(it->Evaluate(cell, timestamp))
					++resultcounter;
			if(relation == Xor)
				return (resultcounter == 1);
			else
				return (resultcounter != 1);
			break;
		}
		case(Not):
			for(auto& it : pixels)
				return cell->GetPixelAddress(it)->IsEmpty(timestamp);
			for(auto& it : sublogics)
				return !it->Evaluate(cell, timestamp);
			return true;	//complement of nothing (i.e. false in this case) is true
			break;
		default:
			return false;
	}
}


Hit PixelLogic::ReadHit(ReadoutCell* cell, int timestamp, std::string* out)
{
	Hit h;
	for(auto& it : ownpixels)
	{
		Pixel* pix = cell->GetPixelAddress(it);
		if(!h.is_valid())
		{
			if(pix->HitIsValid())
			{
				h = pix->LoadHit(timestamp, out);

				if(cell->GetNumPixels() > 1)
				{
					Hit ph = h;
					ph.AddReadoutTime("merged", timestamp);
					if(out != 0)
						*out += ph.GenerateString() + "\n";
				}

				h.AddReadoutTime(cell->GetAddressName(), timestamp);
				if(cell->GetReadoutDelayReference() == "")
					h.SetAvailableTime(timestamp + cell->GetReadoutDelay());
				else
					h.SetAvailableTime(h.GetReadoutTime(cell->GetReadoutDelayReference()) 
											+ cell->GetReadoutDelay());

			}
		}
		else if(!pix->IsEmpty(timestamp))
		{
			Hit ph = pix->LoadHit(timestamp, out);
			if(out != 0)
			{
				Hit saving;
				if(!ph.is_valid())
				{
					saving = h;
					saving.SetAddress(pix->GetAddressName(), pix->GetAddress());
					saving.SetCharge(ph.GetCharge());
					saving.AddReadoutTime("remerged", timestamp);
				}
				else
				{
					saving = ph;
					saving.AddReadoutTime("merged", timestamp);
				}
				*out += saving.GenerateString() + "\n";
			}

			h.SetAddress(pix->GetAddressName(), 
							h.GetAddress(pix->GetAddressName()) | pix->GetAddress());
			h.SetCharge(h.GetCharge() + ph.GetCharge());
		}
		 
	}

	//remove new hits in "not own" pixels:
	auto itown = ownpixels.begin();
	for(auto& it : notownpixels)
	{
		if(itown != ownpixels.end() && it == *itown)
		{
			++itown;
			continue;
		}

		Pixel* pix = cell->GetPixelAddress(it);

		Hit ph = pix->LoadHit(timestamp, out);
		if(ph.is_valid() && out != 0)
		{
			ph.AddReadoutTime("ReferencePixelHitDetected", timestamp);
			*out += ph.GenerateString() + "\n";
		}
	}

	return h;
}

void PixelLogic::ClearHit(ReadoutCell* cell, bool resetcharge)
{
	for(auto& it : pixels)
	{
		Pixel* pix = cell->GetPixelAddress(it);
		pix->ClearHit(resetcharge);
	}
}

ComplexReadout::ComplexReadout(ReadoutCell* roc) : PixelReadout(roc), logic(0), edgedetect(0),
		lastevaluation(false), lastevaluationts(-1)
{

}

ComplexReadout::~ComplexReadout()
{
	delete logic;
}

bool ComplexReadout::Read(int timestamp, std::string* out)
{
	if(logic == NULL)
	{
		std::cout << "Error: The Readout Logic is not set up" << std::endl;
		return false;
	}

	//check the time since the last checking of the result for a timestamp with result 0:
	bool waszero = !lastevaluation;	//start from the last evaluation
	if(edgedetect == 2 && !waszero)		//no need for further checking if the last evaluation was 0
	{
		bool test;
		for(lastevaluationts; lastevaluationts < timestamp; ++lastevaluationts)
		{
			test = logic->Evaluate(cell, lastevaluationts);
			if(!test)
			{
				waszero = true;
				break;
			}
		}
	}
	//current evaluation result:
	bool evaluationresult = logic->Evaluate(cell, timestamp);

	if(evaluationresult && (edgedetect != 0 || waszero))
	{
		lastevaluation = true;
		Hit h = logic->ReadHit(cell, timestamp, out);
		if(h.is_valid() || !cell->zerosuppression)
		{
			if(cell->buf->InsertHit(h))
				return true;
			else if(h.is_valid())
			{
				h.AddReadoutTime("BufferFull", timestamp);
				if(out != NULL)
					*out += h.GenerateString() + "\n";

				return false;
			}
			else
				return false;
		}
	}
	else
	{
		lastevaluation = evaluationresult;
		//write rejected hits to the lost hit file:
		Hit h = logic->ReadHit(cell, timestamp, out);
		if(h.is_valid())
		{
			h.AddReadoutTime("LogicReject", timestamp);
			*out += h.GenerateString() + "\n";
		}

		if(!cell->GetZeroSuppression())
			return cell->buf->InsertHit(Hit());
		else
			return false;
	}
}

void ComplexReadout::SetPixelLogic(PixelLogic* logic)
{
	if(this->logic != NULL)
		delete logic;
	this->logic = logic;
}

void ComplexReadout::SetPixelLogic(const PixelLogic& logic)
{
	this->logic = new PixelLogic(logic);
}

PixelLogic* ComplexReadout::GetPixelLogic()
{
	return logic;
}

void ComplexReadout::SetReadoutCell(ReadoutCell* roc)
{
	cell = roc;
}

bool ComplexReadout::NeedsROCReset()
{
	return true;
}

int ComplexReadout::GetEdgeDetect()
{
	return edgedetect;
}

void ComplexReadout::SetEdgeDetect(int edgedet)
{
	edgedetect = edgedet;
}
