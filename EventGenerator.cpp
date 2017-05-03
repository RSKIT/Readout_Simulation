#include "EventGenerator.h"

EventGenerator::EventGenerator() : filename(""), eventindex(0), clustersize(0), eventrate(0), 
		seed(0), inclinationsigma(0.3), chargescale(1), numsigmas(3), 
		detectors(std::vector<DetectorBase*>()), triggerprobability(0), triggerdelay(0),
		triggerlength(0), triggerstate(true), triggerturnofftime(-1), 
		triggerturnontimes(std::list<int>())
{
	SetSeed(0);
}

EventGenerator::EventGenerator(DetectorBase* detector) : filename(""), eventindex(0), 
		clustersize(0), eventrate(0), seed(0), inclinationsigma(0.3), chargescale(1), 
		numsigmas(3), triggerprobability(0), triggerdelay(0), triggerlength(0), 
		triggerstate(true), triggerturnofftime(-1), triggerturnontimes(std::list<int>())
{
	detectors.push_back(detector);

	SetSeed(0);
}

EventGenerator::EventGenerator(int seed, double clustersize, double rate) : filename(""), 
		eventindex(0), chargescale(1), inclinationsigma(0.3), 
		detectors(std::vector<DetectorBase*>()), triggerprobability(0), triggerdelay(0),
		triggerlength(0), triggerstate(true), triggerturnofftime(-1), 
		triggerturnontimes(std::list<int>())
{
	this->seed 		  = seed;
	SetSeed(seed);
	this->clustersize = clustersize;
	this->eventrate   = rate;
}

bool EventGenerator::IsReady()
{
	if(filename != "" && eventrate != 0)
		return true;
	else
		return false;
}

void EventGenerator::AddDetector(DetectorBase* detector)
{
	if(detector != 0)
		detectors.push_back(detector);
}
	
void EventGenerator::ClearDetectors()
{
	detectors.clear();
}

DetectorBase* EventGenerator::GetDetectorByIndex(int index)
{
	if(index >= detectors.size() || index < 0)
		return 0;
	else
		return detectors[index];
}
	
DetectorBase* EventGenerator::GetDetectorByAddress(int address)
{
	for(auto it : detectors)
	{
		if(it->GetAddress() == address)
			return it;
	}

	//if the detector is not found, return a null pointer:
	return 0;
}

std::string EventGenerator::GetOutputFileName()
{
	return filename;
}

void EventGenerator::SetOutputFileName(std::string filename)
{
	this->filename = filename;
}

int EventGenerator::GetSeed()
{
	return seed;
}

void EventGenerator::SetSeed(int seed)
{
	this->seed = seed;
	eventindex = 0;		//reset the event counter on a change of the seed

	//generate a seed from the cpu time on '0':
	if(seed == 0)
		generator.seed(time(0));
	else
		generator.seed(seed);
}

double EventGenerator::GetInclinationSigma()
{
	return inclinationsigma;
}

void EventGenerator::SetInclinationSigma(double sigma)
{
	if(sigma < 0)
		inclinationsigma = -sigma;
	else
		inclinationsigma = sigma;
}

double EventGenerator::GetClusterSize()
{
	return clustersize;
}

void EventGenerator::SetClusterSize(double size)
{
	//only save positive sizes to avoid constant error checking:
	if(size < 0)
		clustersize = -size;
	else
		clustersize = size;
}

double EventGenerator::GetEventRate()
{
	return eventrate;
}

void EventGenerator::SetEventRate(double rate)
{
	eventrate = rate;
}

double EventGenerator::GetChargeScaling()
{
	return chargescale;
}

void EventGenerator::SetChargeScaling(double scalefactor)
{
	if(scalefactor != 0)
		chargescale = scalefactor;
}


double EventGenerator::GetMinSize()
{
	return minsize;
}

void EventGenerator::SetMinSize(double diagonallength)
{
	if(diagonallength < 0)
		diagonallength = -diagonallength;

	minsize = diagonallength;
}

int EventGenerator::GetCutOffFactor()
{
	return numsigmas;
}

void EventGenerator::SetCutOffFactor(int numsigmas)
{
	if(numsigmas < 0)
		this->numsigmas = -numsigmas;
	else
		this->numsigmas = numsigmas;
}

double EventGenerator::GetTriggerProbability()
{
	return triggerprobability;
}

void EventGenerator::SetTriggerProbability(double probability)
{
	triggerprobability = probability;
}

int EventGenerator::GetTriggerDelay()
{
	return triggerdelay;
}

void EventGenerator::SetTriggerDelay(int delay)
{
	if(delay <= 0)
		triggerdelay = 0;
	else
		triggerdelay = delay;
}

int EventGenerator::GetTriggerLength()
{
	return triggerlength;
}

void EventGenerator::SetTriggerLength(int length)
{
	if(length <= 0)
		triggerlength = 0;
	else
		triggerlength = length;
}

int EventGenerator::GetTriggerOffTime()
{
	return triggerturnofftime;
}

void EventGenerator::SetTriggerOffTime(int timestamp)
{
	triggerturnofftime = timestamp;
}

void EventGenerator::AddOnTimeStamp(int timestamp)
{
	triggerturnontimes.push_back(timestamp);
	triggerstate = false;
}

int EventGenerator::GetNumOnTimeStamps()
{
	return triggerturnontimes.size();
}

void EventGenerator::SortOnTimeStamps()
{
	triggerturnontimes.sort();
}

void EventGenerator::ClearOnTimeStamps()
{
	triggerturnontimes.clear();
}

bool EventGenerator::GetTriggerState(int timestamp)
{
	if(triggerturnontimes.size() == 0)
		return true;

	if(timestamp == triggerturnontimes.front())
	{
		triggerstate = true;
		triggerturnofftime = timestamp + triggerlength;
		triggerturnontimes.pop_front();

		std::cout << "Trigger on" << std::endl;
	}
	else if(timestamp == triggerturnofftime)
	{
		triggerstate = false;
		std::cout << "Trigger off" << std::endl;
	}

	return triggerstate;
}

void EventGenerator::GenerateEvents(double firsttime, int numevents)
{
	if(!IsReady())
		return;

	//calculate the total extent of the detector arrangement:
	TCoord<double> detectorstart = detectors.front()->GetPosition();
	TCoord<double> detectorend   = detectorstart;
	for(auto& it : detectors)
	{
		for(int i=0;i<3;++i)
		{
			//one coordinate smaller than the start:
			if(it->GetPosition()[i] < detectorstart[i])
				detectorstart[i] = it->GetPosition()[i];
			if(it->GetPosition()[i] + it->GetSize()[i] < detectorstart[i])
				detectorstart[i] = it->GetPosition()[i] + it->GetSize()[i];

			//one coordinate larger than the end:
			if(it->GetPosition()[i] > detectorend[i])
				detectorend[i] = it->GetPosition()[i];
			if(it->GetPosition()[i] + it->GetSize()[i] > detectorend[i])
				detectorend[i] = it->GetPosition()[i] + it->GetSize()[i];
		}
	}

	std::fstream fout;
	fout.open(filename.c_str(), std::ios::out | std::ios::app);

	if(!fout.is_open())
		std::cout << "Could not open output file \"" << filename 
				  << "\" to write the generated events." << std::endl;

	double time = firsttime;

	//normal distribution for the generation of the theta angles (result in radians):
	std::normal_distribution<double> distribution(0.0,inclinationsigma);

	//generate the events:
	for (int i = 0; i < numevents; ++i)
	{
		std::cout << "   Generating Event " << i << " of " << numevents << " ..." << std::endl;
		//get a new random particle track:
		TCoord<double> direction;

		double theta = distribution(generator);
		if(theta < 0)
			theta = -theta;
		double phi   = 2* 3.14159265 * (generator() / double(RAND_MAX));
		direction[0] = cos(phi)*sin(theta);
		direction[1] = sin(phi)*sin(theta);
		direction[2] = cos(theta);

		//for(int i = 0; i < 3; ++i)
		//	direction[i] = generator() / double(RAND_MAX);
		TCoord<double> setpoint;	//inside the detector volume
		for(int i = 0; i < 3; ++i)
			setpoint[i] = generator() / double(RAND_MAX) * (detectorend[i]-detectorstart[i]) 
							+ detectorstart[i];

		//generate the next time stamp:
		time += -log(generator()/double(RAND_MAX)) / eventrate;

		//save the parameters of the hit in the file for the events:
		if(fout.is_open())
			fout << "# Event " << eventindex << std::endl
				 << "# Trajectory: g: x(t) = " << setpoint << " + t * " << direction << std::endl
				 << "# Time: " << time << std::endl;

		//generate the trigger for this event:
		if(generator()/double(RAND_MAX) < triggerprobability)
		{
			//if the trigger arrives slightly after the clock transition it will be recognised
			//  one timestamp later -> +0.9 timestamps
			AddOnTimeStamp(int(time + triggerdelay + 0.9));
			if(fout.is_open())
				fout << "# Trigger: " << int(time + triggerdelay + 0.9) << " - " 
					 << int(time + triggerdelay + 0.9 + triggerlength) << std::endl;
		}

		//generate the template hit object for this event:
		Hit hittemplate;
		hittemplate.SetTimeStamp(time);
		hittemplate.SetEventIndex(eventindex);
		++eventindex;

		//get the hits from the readout cells:
		std::vector<Hit> hits;
		for(auto dit : detectors)
		{
			for(auto it = dit->GetROCVectorBegin(); it != dit->GetROCVectorEnd(); ++it)
			{
                hits = ScanReadoutCell(hittemplate, &(*it), direction, setpoint, false);

				//copy the hits to the event queue:
				for(auto it2 : hits)
				{
					clusterparts.push_back(it2);

					//also save the hit in the output file:
					if(fout.is_open())
						fout << "  " << it2.GenerateString() << std::endl;
				}
			}
		}
	}

	fout.close();
}

void EventGenerator::ClearEventQueue()
{
	clusterparts.clear();
}

int EventGenerator::GetNumEventsGenerated()
{
	return eventindex;
}

std::vector<Hit> EventGenerator::GetNextEvent()
{
	std::vector<Hit> event;

	if(clusterparts.size() == 0)
		return event;

	//get the eventindex of the first hit in the queue:
	int thiseventindex = clusterparts.front().GetEventIndex();

	//move all hits from the event to a vector:
	while(clusterparts.size() > 0 && thiseventindex == clusterparts.front().GetEventIndex())
	{
			event.push_back(clusterparts.front());

			clusterparts.pop_front();
	}

	return event;
}

std::vector<Hit> EventGenerator::GetEvent(int eventindex)
{
	std::vector<Hit> event;

	if(clusterparts.size() == 0)
		return event;

	//check whether the desired eventindex is present in the queue:
	if(eventindex >= this->eventindex || eventindex < clusterparts.front().GetEventIndex())
		return event;

	//find the first hit of the desired event:
	auto it = clusterparts.begin();
	while(eventindex > it->GetEventIndex())
		++it;
	//copy the event to a vector:
	while(eventindex == it->GetEventIndex())
	{
		event.push_back(*it);
		++it;
	}

	return event;
}

Hit EventGenerator::GetNextHit()
{
	if(clusterparts.size() == 0)
		return Hit();
	else
	{
		Hit h = clusterparts.front();
		clusterparts.pop_front();

		return h;
	}
}

Hit EventGenerator::GetHit()
{
	if(clusterparts.size() == 0)
		return Hit();
	else
		return clusterparts.front();
}

int EventGenerator::GetNumEventsLeft()
{
	if(clusterparts.size() == 0)
		return 0;
	else
		return eventindex - clusterparts.front().GetEventIndex();
}

void EventGenerator::PrintQueue()
{
	std::cout << "Events enqueued:" << std::endl;
	for(auto it : clusterparts)
		std::cout << "  " << it.GenerateString() << std::endl;
}

double EventGenerator::GetCharge(TCoord<double> x0, TCoord<double> r, TCoord<double> position,
					TCoord<double> size, double minsize, double sigma, int setzero, bool root)
{
	//to count the iterations used for the calculation:
	static int counter = 0;

	//reset the counter if the method is called by the user:
	if(root)
		counter = 0;

	++counter;

	double distance = 0;
	double charge = 0;

	TCoord<double> pixelmiddle = position + 0.5 * size;

	//distance between pixelmiddle and the "particle track":
	distance = (x0-pixelmiddle-((pixelmiddle-x0)*r)/r.abs()/r.abs()*r).abs();
	distance -= setzero * sigma;	//distance from the "border" of the particle track


	//no part of the track is inside the volume:
	if(distance >= 0.5 * size.abs())
	{
		if(root)
			std::cout << "Iterations: " << counter << std::endl;
		return 0;
	}
	//some part of the track can be inside the volume and the volume is larger than the minimun:
	else if(size.abs() >= 2 * minsize)
	{
		TCoord<double> newsize = 0.5 * size;
		//TCoord<double> newposition = pix.GetPosition();
		TCoord<double> shift = TCoord<double>::Null;
		for (int x = 0; x < 8; ++x)
		{
			if((x&1) != 0)
				shift[0] = newsize[0];
			else
				shift[0] = 0;
			if((x&2) != 0)
				shift[1] = newsize[1];
			else
				shift[1] = 0;
			if((x&4) != 0)
				shift[2] = newsize[2];
			else
				shift[2] = 0;

			charge += GetCharge(x0, r, position + shift, newsize, minsize, sigma, 
								setzero, false);
		}

		if(root)
			std::cout << "Iterations: " << counter << std::endl;
		return charge;
	}
	//calculate the charge for the "minimum" volume:
	else
	{

		distance = (x0-pixelmiddle-((pixelmiddle-x0)*r)/r.abs()/r.abs()*r).abs();
		charge = chargescale * 0.1269873 / sigma * exp(-distance*distance/2/sigma/sigma) 
					* size[0]*size[1]*size[2];

		if(root)
			std::cout << "Iterations: " << counter << std::endl;
		return charge;
	}
}


std::vector<Hit> EventGenerator::ScanReadoutCell(Hit hit, ReadoutCell* cell, 
									TCoord<double> direction, TCoord<double> setpoint, 
									bool print)
{
	std::vector<Hit> globalhits;

	hit.AddAddress(cell->GetAddressName(), cell->GetAddress());
	if(cell == 0)
		return globalhits;
	else
	{
		//scan Sub-Cells:
		for(auto it = cell->GetROCsBegin(); it != cell->GetROCsEnd(); ++it)
		{

			//test one subdetector for hits:
			std::vector<Hit> localhits = ScanReadoutCell(hit, &(*it), direction, setpoint);

			//add the hits in the subparts of the detector to the hit vector:
			for(auto it2 : localhits)
				globalhits.push_back(it2);
		}

		//scan pixels:
		for(auto it = cell->GetPixelsBegin(); it != cell->GetPixelsEnd(); ++it)
		{
			double charge = GetCharge(setpoint, direction, it->GetPosition(), it->GetSize(), 
                                        minsize, clustersize, numsigmas, print);

			if(charge > it->GetThreshold() && generator()/double(RAND_MAX) <= it->GetEfficiency())
			{
				if(print)
					std::cout << "Threshold: " << it->GetThreshold() << " < Charge: " 
							  << charge << std::endl;

				Hit phit = hit;
				phit.AddAddress(it->GetAddressName(),it->GetAddress());
				phit.SetCharge(charge);
				phit.SetDeadTimeEnd(phit.GetTimeStamp() + 20);	
								//TODO: change to an adequate function
				globalhits.push_back(phit);
			}
		}
	}

	return globalhits;
}
