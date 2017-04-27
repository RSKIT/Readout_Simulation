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
		if(h.is_available(timestamp))
		{
			if(remove)
				cell->hitqueue.erase(cell->hitqueue.begin());
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
		if(h.is_available(timestamp))
		{
			if(remove)
				cell->hitqueue.erase(cell->hitqueue.begin());
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
				cell->hitqueue[i] = Hit();
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
		if(h.is_valid() || !cell->zerosuppression)
		{
			h.AddReadoutTime(cell->addressname, timestamp);
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
		if(h.is_valid() || !cell->zerosuppression)
		{
			h.AddReadoutTime(cell->addressname, timestamp);
			if(!cell->buf->InsertHit(h))
			{
				//log the losing of the hit:
				if(out != 0 && out->is_open())
					*out << h.GenerateString() << std::endl;

			}
			else
				hitfound = true;
		}
	}

	return hitfound;
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
		if(h.is_valid() || !cell->zerosuppression)
		{
			h.AddReadoutTime(cell->addressname, timestamp);
			if(!cell->buf->InsertHit(h))
			{
				//replace the "oldest" hit:
				Hit oldhit = cell->buf->GetHit(timestamp);
				cell->buf->InsertHit(h);
				//log the losing of the hit:
				oldhit.AddReadoutTime("overwritten", timestamp);
				if(out != 0 && out->is_open())
					*out << oldhit.GenerateString() << std::endl;
			}
			else
				hitfound = true;
		}
	}

	return hitfound;
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

		if(ph.is_valid())
		{
			ph.AddReadoutTime(cell->addressname, timestamp);

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
