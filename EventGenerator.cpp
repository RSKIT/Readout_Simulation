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

#include "EventGenerator.h"

EventGenerator::EventGenerator() : filename(""), eventindex(0), clustersize(0), eventrate(0), 
		seed(0), threads(0), inclinationsigma(0.3), chargescale(1), numsigmas(3), 
		detectors(std::vector<DetectorBase*>()), triggerprobability(0), triggerdelay(0),
		triggerlength(0), triggerstate(true), triggerturnofftime(-1), 
		triggerturnontimes(std::list<int>()), totalrate(true), deadtime(tk::spline()),
		deadtimeX(std::vector<double>()), deadtimeY(std::vector<double>()), pointsindtspline(-1),
		timewalk(tk::spline()), timewalkX(std::vector<double>()), timewalkY(std::vector<double>()),
		pointsintwspline(-1), genoutput(std::stringstream(""))
{
	SetSeed(0);
}

EventGenerator::EventGenerator(DetectorBase* detector) : filename(""), eventindex(0), 
		clustersize(0), eventrate(0), seed(0), threads(0), inclinationsigma(0.3), chargescale(1), 
		numsigmas(3), triggerprobability(0), triggerdelay(0), triggerlength(0), 
		triggerstate(true), triggerturnofftime(-1), triggerturnontimes(std::list<int>()),
		totalrate(true), deadtime(tk::spline()), deadtimeX(std::vector<double>()), 
		deadtimeY(std::vector<double>()), pointsindtspline(-1), timewalk(tk::spline()), 
		timewalkX(std::vector<double>()), timewalkY(std::vector<double>()), pointsintwspline(-1),
		genoutput(std::stringstream(""))
{
	detectors.push_back(detector);

	SetSeed(0);
}

EventGenerator::EventGenerator(int seed, double clustersize, double rate) : filename(""), 
		eventindex(0), chargescale(1), threads(0), inclinationsigma(0.3), 
		detectors(std::vector<DetectorBase*>()), triggerprobability(0), triggerdelay(0),
		triggerlength(0), triggerstate(true), triggerturnofftime(-1), 
		triggerturnontimes(std::list<int>()), totalrate(true), deadtime(tk::spline()),
		deadtimeX(std::vector<double>()), deadtimeY(std::vector<double>()), pointsindtspline(-1),
		timewalk(tk::spline()), timewalkX(std::vector<double>()), timewalkY(std::vector<double>()),
		pointsintwspline(-1), genoutput(std::stringstream(""))
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

int EventGenerator::GetThreads()
{
	return threads;
}

void EventGenerator::SetThreads(unsigned int numthreads)
{
	std::cout << "Set Threads to " << numthreads << " (was " << threads << ")" << std::endl;
	if(numthreads > std::thread::hardware_concurrency())
		threads = 0;
	else
		threads = numthreads;
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

bool EventGenerator::GetEventRateGlobal()
{
	return totalrate;
}

void EventGenerator::SetEventRate(double rate, bool total)
{
	eventrate = rate;
	totalrate = total;
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
		//remove all timestamps at the front for this current timestamp:
		while(triggerturnontimes.front() == timestamp)
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

void EventGenerator::GenerateEvents(double firsttime, int numevents, int numthreads, bool writeout)
{
	if(!IsReady() || numevents == 0)
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

	double detectorarea = (detectorend[0] - detectorstart[0]) 
							* (detectorend[1] - detectorstart[1]);

	double time = firsttime;

	//normal distribution for the generation of the theta angles (result in radians):
	std::normal_distribution<double> distribution(0.0,inclinationsigma);

	//generate the particle tracks:
	std::vector<particletrack> particles;
	particles.reserve(numevents);
	std::cout << "Thread numbers: " << threads << " / " << numthreads << std::endl;
	if(numthreads < 0)
		numthreads = threads;
	if(numthreads == 0)
		numthreads = std::thread::hardware_concurrency();
	std::vector<std::vector<particletrack>::iterator> startpoints;
	for (int i = 0; i < numevents; ++i)
	{
		//std::cout << "   Generating Particle Track " << i << " of " << numevents 
		//		  << " ..." << std::endl;
		//get a new random particle track:
		particletrack track; // = particletrack();

		track.index = i;

		double theta = distribution(generator);
		if(theta < 0)
			theta = -theta;
		double phi = 2* 3.14159265 * (generator() / double(generator.max()));
		track.direction[0] = cos(phi)*sin(theta);
		track.direction[1] = sin(phi)*sin(theta);
		track.direction[2] = cos(theta);

		for(int i = 0; i < 3; ++i)
			track.setpoint[i] = generator() / double(generator.max()) 
									* (detectorend[i] - detectorstart[i]) + detectorstart[i];

		//generate the next time stamp:
		if(totalrate)
			time += -log(generator()/double(generator.max())) / eventrate;
		else
			time += -log(generator()/double(generator.max())) / eventrate / detectorarea;

		track.time = time;

		//generate the trigger for this event:
		if(generator()/double(generator.max()) < triggerprobability)
		{
			track.trigger = true;
			//if the trigger arrives slightly after the clock transition it will be recognised
			//  one timestamp later -> +0.9 timestamps
			AddOnTimeStamp(int(time + triggerdelay + 0.9));
		}
		else
			track.trigger = false;


		particles.push_back(track);

		if((i % (numevents/numthreads+1)) == 0)
		{
			auto partend = --(particles.end());
			startpoints.push_back(partend);
		}
	}
	startpoints.push_back(particles.end());

	std::cout << "Generating " << numevents << " particle tracks done." << std::endl;

	std::cout << "Starting parallel evaluation of the tracks on " << numthreads 
			  << " threads." << std::endl;

	//variables for the worker threads:
	std::vector<Hit> threadhits[numthreads];
	std::stringstream outputs[numthreads];
	std::thread* workers[numthreads];

	//start the worker threads:
	for(int i = 0; i < numthreads; ++i)
	{
		std::thread* worker = new std::thread(GenerateHitsFromTracks, this, startpoints[i], 
												startpoints[i+1], &(threadhits[i]), &(outputs[i]),
												i);

		workers[i] = worker;
	}

	
	std::fstream fout;
	if(writeout)
	{
		fout.open(filename.c_str(), std::ios::out | std::ios::app);

		if(!fout.is_open())
			std::cout << "Could not open output file \"" << filename 
					  << "\" to write the generated events." << std::endl;
	}

	//join the threads again and store the results:
	for(int i = 0; i < numthreads; ++i)
	{
		if(workers[i]->joinable())
		{
			workers[i]->join();

			std::cout << "Thread #" << i << " joined." << std::endl;

			for(auto& it : threadhits[i])
				clusterparts.push_back(it);

			if(writeout)
				fout << outputs[i].str();
			genoutput << outputs[i].str();
		}
	}

	fout.close();

	std::sort(clusterparts.begin(), clusterparts.end());

	return;
}

int EventGenerator::LoadEventsFromFile(std::string filename, bool sort, double timeshift)
{
	std::fstream f;
	f.open(filename.c_str(), std::ios::in);
	if(!f.is_open())
	{
		std::cout << "Could not open input file \"" << filename << "\"." << std::endl;
		return 0;
	}
	else
	{
		int result = LoadEventsFromStream(&f, sort, timeshift);
		f.close();
		return result;
	}

}

int EventGenerator::LoadEventsFromStream(std::fstream* file, bool sort, double timeshift)
{
	int pixelhitcount = 0;
	int maxindex = 0;
	char line[1000];

	while(!file->eof())
	{
		file->getline(line, 1000,'\n');
		if(line[0] != '#')
		{
			Hit h = Hit(std::string(line));
			if(h.is_valid())
			{
				h.SetEventIndex(h.GetEventIndex() + eventindex);
				h.SetTimeStamp(h.GetTimeStamp() + timeshift);
				clusterparts.push_back(h);
				++pixelhitcount;

				if(h.GetEventIndex() > maxindex);
					maxindex = h.GetEventIndex();
			}
		}
		else
		{
			std::stringstream s("");
			std::string text;
			s << line;
			s >> text >> text;
			if(text.compare("Trigger:") == 0)
			{
				int start;
				int ende;
				s >> start >> text >> ende;
				SetTriggerLength(ende-start);
				AddOnTimeStamp(start);
			}
		}
		
	}

	if(sort)
		std::sort(clusterparts.begin(), clusterparts.end());

	eventindex = maxindex + 1;

	return pixelhitcount;
}

void EventGenerator::ClearEventQueue()
{
	clusterparts.clear();
}

void EventGenerator::SortEventQueue()
{
	std::sort(clusterparts.begin(), clusterparts.end());
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

double EventGenerator::GetCharge(std::vector<ChargeDistr>& charge, TCoord<double> position, 
						TCoord<double> size, TCoord<double> granularity, 
						TCoord<double> detectorsize, bool print)
{
	TCoord<double> start = position;
	TCoord<double> end   = position + size;

	double pixelcharge = 0;

	//get the maximum index fitting into the defined detector for folding back the data:
	int xfold = detectorsize[0] / granularity[0];
	int yfold = detectorsize[1] / granularity[1];

	//maximum index for fitting complete detectors into the data. E.g. for a detector fitting
	//  3.7 times into the width of the data, the data after 3 times the test detector width is
	//  rejected. But 3 times the width without folding back is used:
	int xmax = 8000 * granularity[0] / detectorsize[0];
	int ymax = 8000 * granularity[1] / detectorsize[1];

	xmax = (xmax * detectorsize[0]) / granularity[0];
	ymax = (ymax * detectorsize[1]) / granularity[1];

	for(auto& it : charge)
	{
		if(it.etaindex > xmax || it.phiindex > ymax)
			continue;

		TCoord<double> cstart = {granularity[0]*(it.etaindex % xfold),
								 granularity[1]*(it.phiindex % yfold),
								 0};
		TCoord<double> cend   = cstart + granularity;

		pixelcharge += it.charge * chargescale * 
						(OverlapVolume(start, end, cstart, cend).volume() / granularity.volume());
	}

    if(print)
    	std::cout << "Charge: " << pixelcharge << std::endl;

	return pixelcharge;
}


void EventGenerator::AddDeadTimePoint(double charge, double deadtime)
{
	deadtimeX.push_back(charge);
	deadtimeY.push_back(deadtime);
}

void EventGenerator::ClearDeadTimePoints()
{
	deadtimeX.clear();
	deadtimeY.clear();
}

double EventGenerator::GetDeadTime(double charge, bool forceupdate)
{
	if(pointsindtspline != deadtimeX.size() || forceupdate)
	{
		std::vector<double> xvals, yvals;
		if(deadtimeX.size() < 3)
		{
			for(int i=0;i<3;++i)
			{
				xvals.push_back(i);
				yvals.push_back(20);
			}

			pointsindtspline = 0;
		}
		else
		{
			auto ity = deadtimeY.begin();
			for(auto& it : deadtimeX)
			{
				auto searchit = xvals.begin();
				auto sortyit = yvals.begin();
				while(searchit != xvals.end() && *searchit <= it)
				{
					++searchit;
					++sortyit;
				}
				xvals.insert(searchit, it);
				yvals.insert(sortyit, *ity);

				++ity;
			}

			pointsindtspline = xvals.size();
		}

		deadtime.set_points(xvals, yvals);
	}

	return deadtime(charge);
}

bool EventGenerator::SaveDeadTimeSpline(std::string filename, double resolution)
{
	double min = 1e10;
	double max = -1e10;

	for(auto& it : deadtimeX)
	{
		if(it < min)
			min = it;
		if(it > max)
			max = it;
	}

	double range = max - min;

	min -= range / 5;
	max += range / 5;

	std::fstream f;
	f.open(filename.c_str(), std::ios::out | std::ios::app);
	if(!f.is_open())
		return false;

	for(double x = min; x < max; x += resolution)
		f << x << " " << GetDeadTime(x) << std::endl;

	f.close();

	return true;
}

void EventGenerator::AddTimeWalkPoint(double charge, double timewalk)
{
	timewalkX.push_back(charge);
	timewalkY.push_back(timewalk);
}

void EventGenerator::ClearTimeWalkPoints()
{
	timewalkX.clear();
	timewalkY.clear();
}

double EventGenerator::GetTimeWalk(double charge, bool forceupdate)
{
	if(pointsintwspline != timewalkX.size() || forceupdate)
	{
		std::vector<double> xvals,yvals;

		if(timewalkX.size() < 3)
		{
			for(int i = 0; i < 3; ++i)
			{
				xvals.push_back(i);
				yvals.push_back(0);
			}

			pointsintwspline = 0;
		}
		else
		{
			auto ity = timewalkY.begin();
			for(auto& it : timewalkX)
			{
				auto searchit = xvals.begin();
				auto sortyit = yvals.begin();
				while(searchit != xvals.end() && *searchit <= it)
				{
					++searchit;
					++sortyit;
				}
				xvals.insert(searchit, it);
				yvals.insert(sortyit, *ity);

				++ity;
			}

			pointsintwspline = xvals.size();
		}

		timewalk.set_points(xvals, yvals);
	}

	return timewalk(charge);	
}

bool EventGenerator::SaveTimeWalkSpline(std::string filename, double resolution)
{
	double min = 1e10;
	double max = -1e10;

	for(auto& it : timewalkX)
	{
		if(it < min)
			min = it;
		if(it > max)
			max = it;
	}

	double range = max - min;

	min -= range / 5;
	max += range / 5;

	std::fstream f;
	f.open(filename.c_str(), std::ios::out | std::ios::app);
	if(!f.is_open())
		return false;

	for(double x = min; x < max; x += resolution)
		f << x << " " << GetTimeWalk(x) << std::endl;

	f.close();

	return true;
}

std::string EventGenerator::GenerateLog()
{
	return genoutput.str();
}

void EventGenerator::ClearLog()
{
	genoutput.str("");
}

std::vector<Hit> EventGenerator::ScanReadoutCell(Hit hit, ReadoutCell* cell, 
									TCoord<double> direction, TCoord<double> setpoint, 
									bool print)
{
	std::vector<Hit> globalhits;

	if(cell == 0)
		return globalhits;
	else
	{
		hit.AddAddress(cell->GetAddressName(), cell->GetAddress());

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

			if(charge > it->GetThreshold() 
					&& generator()/double(generator.max()) <= it->GetEfficiency())
			{
				if(print)
					std::cout << "Threshold: " << it->GetThreshold() << " < Charge: " 
							  << charge << std::endl;

				Hit phit = hit;
				phit.AddAddress(it->GetAddressName(),it->GetAddress());
				phit.SetCharge(charge);
				phit.SetTimeStamp(phit.GetTimeStamp() + GetTimeWalk(charge));
				if(phit.GetTimeStamp() == -1)
					phit.SetTimeStamp(0);
				phit.SetDeadTimeEnd(phit.GetTimeStamp() + GetDeadTime(charge));	

				if(print)
					std::cout << "Times from Splines (charge: " << charge 
							  << "): TW: " << GetTimeWalk(charge)
							  << "; DT: " << GetDeadTime(charge) << std::endl;

				globalhits.push_back(phit);
			}
		}
	}

	return globalhits;
}

int countDigits(int value)
{
	int digits = 0;
	while(value > 0)
	{
		++digits;
		value /= 10;
	}

	if(digits > 0)
		return digits;
	else
		return 1;
}

void EventGenerator::GenerateHitsFromTracks(EventGenerator* itself,
									std::vector<particletrack>::iterator begin, 
									std::vector<particletrack>::iterator end,
									std::vector<Hit>* pixelhits,
									std::stringstream* output, int id)
{
	std::cout << "Started thread with id " << std::this_thread::get_id() << " ..." << std::endl;

	//generate the template hit object for this event:
	Hit hittemplate;
	int counter = 0;

	for(auto it = begin; it != end; ++it)
	{
        hittemplate.SetTimeStamp(it->time);
        hittemplate.SetEventIndex(it->index);

        //add the event to the output stream:
		*output << "# Event " << it->index << std::endl
			    << "# Trajectory: g: x(t) = " << it->setpoint << " + t * " 
			    << it->direction << std::endl
	 		    << "# Time: " << it->time << std::endl;
	 	if(it->trigger)
			*output << "# Trigger: " << int(it->time + itself->triggerdelay + 0.9) << " - " 
		 		    << int(it->time + itself->triggerdelay + 0.9 + itself->triggerlength) 
		 		    << std::endl;

		//get the hits from the readout cells:
		std::vector<Hit> hits;
		for(auto& dit : itself->detectors)
		{
			for(auto rit = dit->GetROCVectorBegin(); rit != dit->GetROCVectorEnd(); ++rit)
			{
                hits = itself->ScanReadoutCell(hittemplate, &(*rit), it->direction, 
                								it->setpoint, false);

				//copy the hits to the event queue:
				for(auto& it2 : hits)
				{
					pixelhits->push_back(it2);

					//also save the hit in the output file:
					*output << "  " << it2.GenerateString() << std::endl;
				}
			}
		}

		if((counter++ % 10) == 0)
		{
			if(id < 0)
				std::cout << "Process " << std::this_thread::get_id() << ": Generated " 
						  << std::setw(4) << counter << " Events" << std::endl;
			else
			{
				static int numthreads = countDigits(std::thread::hardware_concurrency());
				std::cout << "Process " << std::setw(numthreads) << id << ": Generated " 
						  << std::setw(4) << counter << " Events" << std::endl;
			}
		}
	}

	std::sort(pixelhits->begin(), pixelhits->end());
}

int EventGenerator::LoadITkEvents(std::string filename, int firstline, int numlines, 
									double firsttime, int eta,
									TCoord<double> granularity, int numthreads,	bool writeout,
									double regroup, bool sort)
{
	//data structure for the clustered hit information:
	std::map<unsigned int, std::vector<ChargeDistr> > clusters;	//eventID and Charge Distribution
		//each event has its own ID (unsigned int) and a vector containing the data on the 

	//Load the ROOT file:
	TFile* f = new TFile(filename.c_str());

	//extract the TTree object containing the data:
	TTree* hits = 0;
	f->GetObject("hitstree", hits);

	//abort if it is not found
	if(hits == 0)
	{
		delete f;
		return 0;
	}

	//loading of the granularity from the root file. Use {5,5,50} if not provided in ROOT file:
	if(granularity.isZero())
	{
		TVectorF * v = new TVectorF(1);

		//get eta dimension:
		f->GetObject("eta_dim", v);
		if(v != 0)
		{
			granularity[0] = (*v)[0];
			//delete v;
		}
		else
			granularity[0] = 5;

		//get phi dimension:
		f->GetObject("phi_dim", v);
		if(v != 0)
		{
			granularity[1] = (*v)(0);
			//delete v;
		}
		else
			granularity[1] = 5;

		//get depletion thickness:
		f->GetObject("depletion", v);
		if(v != 0)
		{
			granularity[2] = (*v)(0);
			//delete v;
		}
		else
			granularity[2] = 50;

		delete v;
	}

	//=== clustering of the charge distribution information ===
	unsigned int eventid;
	unsigned char etamodule;
	unsigned short etaindex;
	unsigned short phiindex;
	float charge;

	hits->SetBranchAddress("charge", &charge);
	hits->SetBranchAddress("eventId", &eventid);
	hits->SetBranchAddress("eta_module", &etamodule);
	hits->SetBranchAddress("phi_index", &phiindex);
	hits->SetBranchAddress("eta_index", &etaindex);

	if(numlines == -1)
		numlines = hits->GetEntries();
	else
		numlines += firstline;

	if(numlines > hits->GetEntries())
		numlines = hits->GetEntries();

	for(int i = firstline; i < numlines; ++i)
	{
		hits->GetEntry(i);

		if(eta != 0 && etamodule != eta)
			continue;

		ChargeDistr singlecluster;
		singlecluster.charge = charge;
		singlecluster.etamodule = etamodule;
		singlecluster.etaindex = etaindex;
		singlecluster.phiindex = phiindex;

		auto it = clusters.find(eventid);
		if(it != clusters.end())
			it->second.push_back(singlecluster);
		else
		{
			std::vector<ChargeDistr> tempvec;
			tempvec.push_back(singlecluster);
			clusters.insert(std::make_pair(eventid, tempvec));
		}
	}
	delete f;
	//=== end clustering ===

	std::cout << "Clusters: " << clusters.size() << std::endl;

	//=== regrouping ===
	if(regroup > 0)
	{
		std::cout << "Start regrouping events ... (" << clusters.size() << ")" << std::endl;

		//find number of threads to use:
		int rgthreads = numthreads;
		if(rgthreads < 0)
			rgthreads = threads;
		if(rgthreads == 0)
			rgthreads = std::thread::hardware_concurrency();

		//prepare start end end points for the threads:
		std::vector<std::map<unsigned int, std::vector<ChargeDistr> >::iterator> startpoints;
		int totalclusters = clusters.size();
		int index = 0;
		for(auto it = clusters.begin(); it != clusters.end(); ++it)
		{
			if((index % (totalclusters/rgthreads+1)) == 0)
				startpoints.push_back(it);
			++index;
		}
		startpoints.push_back(clusters.end());

		//resulting new clusters of the threads:
		std::map<unsigned int, std::vector<ChargeDistr> > threadclusters[rgthreads];
		std::thread* workers[rgthreads];

		//start the worker threads:
		for(int i = 0; i < rgthreads; ++i)
		{
			std::thread* worker = new std::thread(SeparateClusters, &(threadclusters[i]),
											startpoints[i], startpoints[i+1], granularity,
											regroup, i, totalclusters/rgthreads+1);

			workers[i] = worker;
		}

		//wait for all threads to finish:
		for(int i = 0; i < rgthreads; ++i)
		{
			if(workers[i]->joinable())
			{
				workers[i]->join();
				std::cout << "Thread #" << i << " joined." << std::endl;

				delete workers[i];
			}
		}

		std::cout << "Clusters (old): " << clusters.size() << std::endl;
		for(int i = 0; i < rgthreads; ++i)
			std::cout << " Clusters (new): " << threadclusters[i].size() << std::endl;

		//replace the old data with the newly generated data:
		clusters.clear();
		for(int i = 0; i < rgthreads; ++i)
		{
			for(auto& it : (threadclusters[i]))
			{
				int newid = it.first;
				//find an unused event ID:
				while(clusters.find(newid) != clusters.end())
					newid += 128;

				clusters.insert(std::make_pair(newid, it.second));
			}
		}

		std::cout << "Regrouping done. (" << clusters.size() << ")" << std::endl;
	}
	//=== end regrouping ===

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

	double detectorarea = (detectorend[0] - detectorstart[0]) 
							* (detectorend[1] - detectorstart[1]);

	std::map<unsigned int, double> clustertimes;	//start times for the events
	std::map<unsigned int, bool> triggeredevents;	//connects the eventIDs with trigger information

	double time = firsttime;
	for(auto it : clusters)
	{
		//generate the next time stamp:
		if(totalrate)
			time += -log(generator()/double(generator.max())) / eventrate;
		else
			time += -log(generator()/double(generator.max())) / eventrate / detectorarea;

		clustertimes.insert(std::make_pair(it.first, time));

		if(generator()/double(generator.max()) < triggerprobability)
		{
			//if the trigger arrives slightly after the clock transition it will be recognised
			//  one timestamp later -> +0.9 timestamps
			AddOnTimeStamp(int(time + triggerdelay + 0.9));
			triggeredevents.insert(std::make_pair(it.first, true));
		}
		else
			triggeredevents.insert(std::make_pair(it.first, false));
	}

	//=== start parallel evaluation of the clusters ===
	//find the number of threads to use:
	std::cout << "Thread numbers: " << threads << " / " << numthreads << std::endl;
	if(numthreads < 0)
		numthreads = threads;
	if(numthreads == 0)
		numthreads = std::thread::hardware_concurrency();
	std::vector<std::map<unsigned int, std::vector<ChargeDistr> >::iterator> startpoints;

	//prepare the distribution of the data over the threads:
	auto it     = clusters.begin();
	int numevents = clusters.size();
	for(int i = 0; i < numevents; ++i)
	{
		if((i % (numevents / numthreads + 1)) == 0)
			startpoints.push_back(it);

		++it;
	}
	startpoints.push_back(clusters.end());


	//Test output of the read charge distributions:
	//for(auto& it : clusters)
	//{
	//	std::cout << "Event: " << it.first << std::endl;
	//	for(auto& it2 : it.second)
	//	{
	//		std::cout << "  " << it2.etaindex << " ; " << it2.phiindex << " : " << it2.charge
	//				<< std::endl;
	//	}
	//}

	//variables for the worker threads:
	std::vector<Hit> threadhits[numthreads];
	std::stringstream outputs[numthreads];
	std::thread* workers[numthreads];

	//start the worker threads:
	for(int i = 0; i < numthreads; ++i)
	{
		std::thread* worker = new std::thread(GenerateHitsFromChargeDistributions, this, 
												startpoints[i], startpoints[i+1], 
												&clustertimes, &triggeredevents,
												granularity, detectorend - detectorstart,
												&(threadhits[i]), &(outputs[i]),
												eventindex + (numevents/numthreads+1)*i ,i);

		workers[i] = worker;
	}

	eventindex += numevents;

	std::fstream fout;
	if(writeout)
	{
		fout.open(this->filename.c_str(), std::ios::out | std::ios::app);

		if(!fout.is_open())
			std::cout << "Could not open output file \"" << filename 
					  << "\" to write the generated events." << std::endl;
	}

	//join the threads again and store the results:
	for(int i = 0; i < numthreads; ++i)
	{
		if(workers[i]->joinable())
		{
			workers[i]->join();

			std::cout << "Thread #" << i << " joined." << std::endl;

			
			clusterparts.insert(clusterparts.end(), threadhits[i].begin(), threadhits[i].end());

			if(fout.is_open())
			{
				fout << (outputs[i]).str();
				fout.flush();
			}
			genoutput << outputs[i].str();
			genoutput.flush();
		}
	}

	fout.close();

	if(sort)
		std::sort(clusterparts.begin(), clusterparts.end());


	return numevents;
}

std::vector<Hit> EventGenerator::ScanReadoutCell(Hit hit, ReadoutCell* cell, 
													std::vector<ChargeDistr>& charge,
													TCoord<double> chargestart,
													TCoord<double> chargeend,
													TCoord<double> granularity, 
													TCoord<double> detectorsize,
													bool print)
{
	std::vector<Hit> globalhits;

	if(cell == 0)
		return globalhits;
	else
	{
		hit.AddAddress(cell->GetAddressName(), cell->GetAddress());

		//scan sub-cells:
		for(auto it = cell->GetROCsBegin(); it != cell->GetROCsEnd(); ++it)
		{
			//do not evaluate readout cells which do not contain charge:
			if(OverlapVolume(chargestart, chargeend, it->GetPosition(), 
								it->GetPosition() + it->GetSize()).volume() == 0)
				continue;

			std::vector<Hit> localhits = ScanReadoutCell(hit, &(*it), charge, chargestart,
															chargeend, granularity, detectorsize);

			globalhits.insert(globalhits.end(), localhits.begin(), localhits.end());
		}

		//scan pixels:
		for(auto it = cell->GetPixelsBegin(); it != cell->GetPixelsEnd(); ++it)
		{
			double pixelcharge = GetCharge(charge, it->GetPosition(), it->GetSize(), granularity,
											detectorsize, print);

			if(pixelcharge > it->GetThreshold() && 
				(it->GetEfficiency() == 1 || generator()/double(generator.max()) 
													<= it->GetEfficiency()))
			{
				if(print)
					std::cout << "Threshold: " << it->GetThreshold() << " < Charge: " 
							  << pixelcharge << std::endl;

				//prepare the hit object:
				Hit phit = hit;
				phit.AddAddress(it->GetAddressName(),it->GetAddress());
				phit.SetCharge(pixelcharge);
				//set TimeStamp according to characteristics:
				phit.SetTimeStamp(phit.GetTimeStamp() + GetTimeWalk(pixelcharge));
				if(phit.GetTimeStamp() == -1)	//for negative times, the timestamp is set invalid
					phit.SetTimeStamp(0);
				phit.SetDeadTimeEnd(phit.GetTimeStamp() + GetDeadTime(pixelcharge));	

				if(print)
					std::cout << "Times from Splines (charge: " << pixelcharge 
							  << "): TW: " << GetTimeWalk(pixelcharge)
							  << "; DT: " << GetDeadTime(pixelcharge) << std::endl;

				globalhits.push_back(phit);
			}
		}
	}
}

void EventGenerator::GenerateHitsFromChargeDistributions(EventGenerator* itself,
							std::map<unsigned int, std::vector<ChargeDistr> >::iterator begin,
							std::map<unsigned int, std::vector<ChargeDistr> >::iterator end,
							std::map<unsigned int, double>* times,
							std::map<unsigned int, bool>* triginfos,
							TCoord<double> granularity,
							TCoord<double> detectorsize,
							std::vector<Hit>* pixelhits,
							std::stringstream* output, int firsteventid, int id)
{
	std::cout << "Started thread with id " << std::this_thread::get_id() << " ..." << std::endl;

	if(output == 0)
		return;

	//generate the template hit object for this event:
	Hit hittemplate;
	int counter = 0;	//for writing current status to the command line
	int eventid = firsteventid;

	for(auto it = begin; it != end; ++it)
	{
		//== calculate the extent of the charge distribution ==
		TCoord<double> start = TCoord<double>{1e10,1e10,1e10};
		TCoord<double> end   = TCoord<double>{-1e10,-1e10,-1e10};

		//get the maximum index fitting into the defined detector for folding back the data:
		int xfold = detectorsize[0] / granularity[0];
		int yfold = detectorsize[1] / granularity[1];

		int xmax = 8000 * granularity[0] / detectorsize[0];
		int ymax = 8000 * granularity[1] / detectorsize[1];

		xmax = (xmax * detectorsize[0]) / granularity[0];
		ymax = (ymax * detectorsize[1]) / granularity[1];

		//loop over the charge contributions:
		for(auto& itc : it->second)
		{
			//skip data which will not be used (see GetCharge()):
			if(itc.etaindex > xmax || itc.phiindex > ymax)
				continue;

			TCoord<double> cstart = {granularity[0]*(itc.etaindex % xfold),
									 granularity[1]*(itc.phiindex % yfold),
									 0};
			TCoord<double> cend   = cstart + granularity;

			for(int i = 0; i < 3; ++i)
			{
				if(start[i] > cstart[i])
					start[i] = cstart[i];
				if(end[i] < cend[i])
					end[i] = cend[i];
			}
		}
		//== end charge distribution size calculation ==

		hittemplate.SetTimeStamp((*times)[it->first]);
		hittemplate.SetEventIndex(eventid);
		++eventid;

		//log the event header:
		*output << "# Event " << it->first << std::endl
		        << "# Time " << (*times)[it->first] << std::endl;
		if((*triginfos)[it->first])
		{
			int triggerstart = int((*times)[it->first] + itself->triggerdelay + 0.9);
			*output << "# Trigger " << triggerstart << " - " 
		 		    << int(triggerstart + itself->triggerlength) << std::endl;
		}

		std::vector<Hit> localhits;
		//loop all detectors
		for(auto& dit : itself->detectors)
		{
			//do not scan detectors with no overlap with the charge distribution:
			if(OverlapVolume(start, end, dit->GetPosition(), 
					dit->GetPosition() + dit->GetSize()).volume() == 0)
				continue;

			//loop the ROCs in the current detector:
			for(auto rit = dit->GetROCVectorBegin(); rit != dit->GetROCVectorEnd(); ++rit)
			{
				//skip readout cells which do not contain charge from this event:
				if(OverlapVolume(start, end, rit->GetPosition(), 
						rit->GetPosition() + rit->GetSize()).volume() == 0)
					continue;

				localhits = itself->ScanReadoutCell(hittemplate, &(*rit), it->second, 
													start, end, granularity, detectorsize, false);

				//copy the pixel hits to the output vector:
				pixelhits->insert(pixelhits->end(), localhits.begin(), localhits.end());

				//save the hits in the output file:
				for(auto& pit : localhits)
					*output << "  " << pit.GenerateString() << std::endl;
			}
		}

		if((counter++ % 10) == 0)
		{
			if(id < 0)
				std::cout << "Process " << std::this_thread::get_id() << ": Generated " 
						  << std::setw(4) << counter << " Events" << std::endl;
			else
			{
				static int numthreads = countDigits(std::thread::hardware_concurrency());
				std::cout << "Process " << std::setw(numthreads) << id << ": Generated " 
						  << std::setw(4) << counter << " Events" << std::endl;
			}
		}
	}

	std::sort(pixelhits->begin(), pixelhits->end());
}

void EventGenerator::SeparateClusters(
						std::map<unsigned int, std::vector<ChargeDistr> >* resultclusters,
						std::map<unsigned int, std::vector<ChargeDistr> >::iterator begin,
						std::map<unsigned int, std::vector<ChargeDistr> >::iterator end,
						TCoord<double> granularity, double maxdistance, int id, int numclusters)
{
	std::stringstream s("");
	if(id < 0)
		s << "Started thread with ID " << std::this_thread::get_id() << std::endl;
	else
		s << "Started thread with ID " << id << std::endl;
	std::cout << s.str();
	std::cout.flush();

	maxdistance = pow(maxdistance, 2);
	
	for(auto itc = begin; itc != end; ++itc)
	{
		std::list<ChargeDistr> rawdata;
		//rawdata.clear();
		std::list<std::list<ChargeDistr>> newclusters;
		//newclusters.clear();

		rawdata.insert(rawdata.end(), itc->second.begin(), itc->second.end());

		//add the first hit:
		if(rawdata.size() > 0)
		{
			newclusters.push_back(std::list<ChargeDistr>());
			newclusters.back().push_back(rawdata.front());
			rawdata.pop_front();
		}

		auto it = rawdata.begin();

		bool addedonepixel = false;	//flag for adding a hit from the rawdata list
		bool newhit = false;		//flag for the newclusters list to break
		while(rawdata.size() > 0)
		{
			//std::cout << "Remaining hits: " << rawdata.size() << std::endl;
			//check whether the new pixel belongs to an existing cluster:
			for(auto& it2 : newclusters)
			{
				//check for the same eta (i.e. ring of modules; 
				//	+/- eta is covered by different eventID)
				if(it->etamodule == it2.front().etamodule)
				{
					//check distance to existing pixels in already processes pixels:
					for(auto& it3 : it2)
					{
						if((pow(granularity[0] * (it->etaindex - it3.etaindex), 2) 
								+ pow(granularity[1] * (it->phiindex - it3.phiindex), 2))
										<= maxdistance)
						{
							newhit = true;
							addedonepixel = true;

							it2.push_back(*it);
							it = rawdata.erase(it);
							break;
						}
					}

					if(newhit == true)
					{
						newhit = false;
						break;
					}
				}
			}
			
			if(rawdata.size() == 0)
				break;

			++it;
			if(it == rawdata.end())
			{
				//add a new cluster if the remaining ones do not belong to any existing cluster:
				if(addedonepixel == false)
				{
					//std::list<ChargeDistr>* nl = new std::list<ChargeDistr>();
					//nl->push_back(rawdata.front());
					//newclusters.push_back(nl);
					//rawdata.pop_front();
					newclusters.push_back(std::list<ChargeDistr>());
					newclusters.back().push_back(rawdata.front());
					rawdata.pop_front();
				}
				else
					addedonepixel = false;

				it = rawdata.begin();
			}
		}

		unsigned int oldeventid = itc->first;
		for(auto& it : newclusters)
		{
			std::vector<ChargeDistr> onecluster;

			onecluster.insert(onecluster.end(), it.begin(), it.end());

			while(resultclusters->find(oldeventid) != resultclusters->end())
				oldeventid += 128; // += 1 << 7;	//BC part of the event ID


			resultclusters->insert(std::make_pair(oldeventid, onecluster));
		}

		//for(auto& it : newclusters)
		//	delete it;
		newclusters.clear();

		if((numclusters-- % 10) == 0)
		{
			std::stringstream s("");
			if(id < 0)
				s << "Process " << std::this_thread::get_id();
			else
			{
				static int numthreads = countDigits(std::thread::hardware_concurrency());
				s << "Process " << std::setw(numthreads) << id;
			}
			s << ": Remaining " << std::setw(4) << numclusters << " Events" << std::endl;
			std::cout << s.str();
			std::cout.flush();
		}
	}

	std::cout << "Ende Thread #" << id << "!" << std::endl;
}
