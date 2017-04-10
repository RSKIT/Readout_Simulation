#include "readoutcell_functions.h"

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

Hit ROCBuffer::GetHit()
{
	if(cell->hitqueue.size() == 0)
		return Hit();
	else
	{
		Hit h = cell->hitqueue.front();
		cell->hitqueue.pop_front();
		return h;
	}
}

bool ROCBuffer::is_full()
{
	return (cell->hitqueue.size() >= cell->hitqueuelength);
}

FIFOBuffer::FIFOBuffer(ReadoutCell* roc) : ROCBuffer(roc)
{

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

Hit FIFOBuffer::GetHit()
{
	if(cell->hitqueue.size() == 0)
		return Hit();
	else
	{
		Hit h = cell->hitqueue.front();
		cell->hitqueue.pop_front();
		return h;
	}
}

PrioBuffer::PrioBuffer(ReadoutCell* roc) : ROCBuffer(roc)
{
	if(cell == 0)
		return;
	else
	{
		cell->hitqueue.clear();
		for(int i = 0; i < *bufferlength; ++i)
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

Hit PrioBuffer::GetHit()
{
	for(int i = 0; i < cell->hitqueuelength; ++i)
	{
		if(cell->hitqueue[i].is_valid())
		{
			Hit h = cell->hitqueue[i];
			cell->hitqueue[i] = Hit();
			return h;
		}
	}
	return Hit();
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
	if(cell->buf.is_full())
		return false;

	//to save whether a hit was found
	bool hitfound = false;

	//check all child ROCs for hits:
	for(auto it = cell->rocvector.begin(); it != cell->rocvector.end(); ++i)
	{
		//get a hit from the respective ROC:
		Hit h  = it->buf.GetHit();
		if(h.is_valid())
		{
			h.AddReadoutTime(cell->addressname, timestamp);
			bool result = cell->buf.InsertHit(h);

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
			if(cell->buf.is_full())
				return hitfound;
		}
	}

	return hitfound;
}