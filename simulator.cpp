#include "simulator.h"
Simulator::Simulator() : events(0), starttime(0), stoptime(-1), stopdelay(0)
{

}

Simulator::Simulator(std::string filename) : inputfile(filename), events(0), starttime(0),
		stoptime(-1), stopdelay(0)
{

}

std::string Simulator::GetLoadFileName()
{
	return inputfile;
}

void Simulator::LoadInputFile(std::string filename)
{
	if(filename == "")
		filename = inputfile;
	else
		inputfile = filename;

	std::cout << "Loading data from \"" << filename << "\" ..." << std::endl;

	detectors.clear();
	eventgenerator.ClearEventQueue();

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError error = doc.LoadFile(filename.c_str());

	doc.PrintError();

	TCoord<double> standardpixel{0,0,0};

	tinyxml2::XMLElement* simulation = doc.FirstChildElement();
	tinyxml2::XMLElement* elem = simulation->FirstChildElement();
	while(elem != 0)
	{
		std::string elementname = std::string(elem->Name());

		std::cout << "Element: " << elementname << std::endl;

		if(elementname.compare("Detector") == 0)
			LoadDetector(elem, standardpixel);
		else if(elementname.compare("Standardpixel") == 0)
			standardpixel = LoadTCoord(elem);
		else if(elementname.compare("EventGenerator") == 0)
			LoadEventGenerator(elem);
		else if(elementname.compare("SimulationEnd") == 0)
		{
			//load end time:
			tinyxml2::XMLError error = elem->QueryIntAttribute("t", &stoptime);
			if(error != tinyxml2::XML_NO_ERROR)
				stoptime = -1;
			//load delay for stopping:
			error = elem->QueryIntAttribute("stopdelay", &stopdelay);
			if(error != tinyxml2::XML_NO_ERROR)
				stopdelay = 0;
		}

		if(elem != simulation->LastChildElement())
			elem = elem->NextSiblingElement();
		else
			elem = 0;
	}

	for(auto it = detectors.begin(); it != detectors.end(); ++it)
	{
		(*it)->EnlargeSize();
		eventgenerator.AddDetector(*it);
	}

}

std::string Simulator::GetSaveFileName()
{
	return outputfile;
}

void Simulator::SetLoadFileName(std::string filename)
{
	outputfile = filename;
}

int Simulator::GetNumEventsToGenerate()
{
	return events;
}

void Simulator::SetNumEventsToGenerate(int events)
{
	this->events = events;
}

int Simulator::GetStartTime()
{
	return starttime;
}

void Simulator::SetStartTime(int starttime)
{
	this->starttime = starttime;
}

int Simulator::GetStopTime()
{
	return stoptime;
}

void Simulator::SetStopTime(int stoptime)
{
	this->stoptime = stoptime;
}

void Simulator::RemoveStopTime()
{
	stoptime = -1;
}

int Simulator::GetStopDelay()
{
	return stopdelay;
}

void Simulator::SetStopDelay(int stopdelay)
{
	this->stopdelay = stopdelay;
}


DetectorBase* Simulator::GetDetector(int address)
{
	if(detectors.size() == 0)
		return 0;

	for(auto it = detectors.begin(); it != detectors.end(); ++it)
	{
		if((*it)->GetAddress() == address)
			return *it;
	}

	return 0;
}
	
void Simulator::AddDetector(DetectorBase* detector)
{
	std::cout << "Input:  "
			  << detector->GetPosition() << " size: " << detector->GetSize() << std::endl;

	detectors.push_back(detector->Clone());
	auto it = detectors.end();
	--it;
	eventgenerator.AddDetector(*it);

	//auto it = --detectors.end();
	std::cout << "Output: "
			  << (*it)->GetPosition() << " size: " << (*it)->GetSize() << std::endl;
}

void Simulator::ClearDetectors()
{
	eventgenerator.ClearDetectors();
	for(auto& it : detectors)
		delete it;
	detectors.clear();
}

int Simulator::GetNumDetectors()
{
	return detectors.size();
}

EventGenerator* Simulator::GetEventGenerator()
{
	return &eventgenerator;
}

void Simulator::InitEventGenerator()
{
	if(!eventgenerator.IsReady())
	{
		std::cout << "EventGenerator is not set up properly because of missing parameters" 
				  << std::endl;

		std::cout << "output file: \"" << eventgenerator.GetOutputFileName() << "\"" << std::endl;
		std::cout << "rate: " << eventgenerator.GetEventRate() << std::endl;
		return;
	}

	eventgenerator.GenerateEvents(starttime, events);
	events = 0;
}

void Simulator::GenerateEvents(int events, double starttime)
{
	if(starttime < 0)
		eventgenerator.GenerateEvents(events,this->starttime);
	else
		eventgenerator.GenerateEvents(events, starttime);
}

void Simulator::ClockUp(int timestamp)
{
	for(auto it = detectors.begin(); it != detectors.end(); ++it)
		(*it)->StateMachineCkUp(timestamp);
}

void Simulator::ClockDown(int timestamp)
{
	for(auto it = detectors.begin(); it != detectors.end(); ++it)
		(*it)->StateMachineCkDown(timestamp);
}

void Simulator::SimulateUntil(int stoptime, int delaystop)
{
	if(events > 0)
	{
		eventgenerator.GenerateEvents(starttime, events);
		events = 0;
	}

	eventgenerator.PrintQueue();

	int timestamp = 0;
	double nextevent = eventgenerator.GetHit().GetTimeStamp();

	int hitcounter = 0;

	while(timestamp <= stoptime || (stoptime == -1 && stopdelay >= 0))
	{
		while(timestamp >= nextevent && nextevent != -1)
		{
			//load the next event:
			std::vector<Hit> event = eventgenerator.GetNextEvent();
			//update the time stamp for the next event:
			nextevent = eventgenerator.GetHit().GetTimeStamp();

			//insert the hits of the current event into the detector(s):
			for(auto hit : event)
			{
				for(auto it = detectors.begin(); it != detectors.end(); ++it)
				{
					if((*it)->PlaceHit(hit))
						break;
				}

				++hitcounter;
			}

			std::cout << "Inserted " << hitcounter << " signals by now..." << std::endl;
		}

		ClockUp(timestamp);
		ClockDown(timestamp);

		++timestamp;

		//delay the stopping of the simulation for "stop-on-done" (see while()):
		if(nextevent == -1)
		{
			//count the remaining events:
			int hitcount = 0;
			for(auto& it : detectors)
				hitcount += it->HitsEnqueued();
			if(hitcount == 0)
				--stopdelay;
		}

		std::cout << "        TS: " << timestamp << "; nextEvent: " << nextevent << "; stopdelay: "
				  << stopdelay << std::endl;
	}

	//count the signals read out from the detectors:
	int dethitcounter = 0;
	for(auto it = detectors.begin(); it != detectors.end(); ++it)
		dethitcounter += (*it)->GetHitCounter();

	std::cout << "Simulation done." << std::endl << "  injected signals: " << hitcounter
			  << std::endl << "  read out signals: " << dethitcounter << std::endl
			  << "  Efficiency:       " << dethitcounter/double(hitcounter) << std::endl;
}

void Simulator::LoadDetector(tinyxml2::XMLElement* parent, TCoord<double> pixelsize)
{
	std::cout << "  LoadDetector" << std::endl;

	static std::map<std::string, int> latestindex;

	//load address and address identifier from the file:
	std::string addressname;
	int address;
	
	const char* nam = parent->Attribute("addrname");
	addressname = (nam != 0)?std::string(nam):"det";

	tinyxml2::XMLError error = parent->QueryIntAttribute("addr", &address);
	if(error != tinyxml2::XML_NO_ERROR)
		address = 0;

	//check address for consistency and add not provided information:
	if(latestindex.find(addressname) != latestindex.end())
	{
		if(error != tinyxml2::XML_NO_ERROR)
			address = ++latestindex[addressname];
		else if(address > latestindex[addressname])
			latestindex[addressname] = address;
		//else
			//nothing to do
	}
	else
	{
		if(error != tinyxml2::XML_NO_ERROR)
			address = 0;

		latestindex.insert(std::make_pair(addressname,address));
	}

	//check for an output file name for this detector:
	nam = parent->Attribute("outputfile");
	std::string outputfile = (nam != 0)?std::string(nam):"";
	nam = parent->Attribute("losthitfile");
	std::string badoutputfile = (nam != 0)?std::string(nam):"";

	DetectorBase* det = new Detector(addressname, address);	//TODO: change for different detectors
	det->SetOutputFile(outputfile);
	det->SetBadOutputFile(badoutputfile);

	tinyxml2::XMLElement* child = parent->FirstChildElement();
	while(child != 0)
	{
		std::string childname = std::string(child->Value());
		if(childname.compare("ROC") == 0)
			det->AddROC(LoadROC(child, pixelsize));
		else if(childname.compare("Position")== 0)
			det->SetPosition(LoadTCoord(child));
		else if(childname.compare("Size") == 0)
			det->SetSize(LoadTCoord(child));
		//else if(childname.compare("StateMachine"))	
					//to include writing the StateMachine in the XML file
			//LoadStateMachine(child, xmldet);

		if(child != parent->LastChildElement())
			child = child->NextSiblingElement();
		else
			child = 0;
	}

	det->EnlargeSize();

	//xmldet.Setup(det);

	//if(xmldat.is_setup())
	//	AddDetector(xmldet);
	//else
		AddDetector(det);
}

TCoord<double> Simulator::LoadTCoord(tinyxml2::XMLElement* coordinate)
{
	std::cout << "  LoadTCoord" << std::endl;	//TODO: remove this line

	TCoord<double> coord;
	tinyxml2::XMLError error;
	const char* axis[] = {"x","y","z"};

	for(int i = 0; i < 3; ++i)
	{
		error = coordinate->QueryDoubleAttribute(axis[i], &coord[i]);
		if(error != tinyxml2::XML_NO_ERROR)
			std::cout << "Error reading " << axis[i] << " axis" << std::endl;
	}

	return coord;
}

void Simulator::LoadEventGenerator(tinyxml2::XMLElement* eventgen)
{
	std::cout << "  LoadEventGenerator" << std::endl;

	eventgenerator = EventGenerator();

	tinyxml2::XMLElement* element = eventgen->FirstChildElement();
	while(element != 0)
	{
		std::string name = std::string(element->Value());

		if(name.compare("Seed") == 0)
		{
			int seed;
			eventgenerator.SetSeed((element->QueryIntAttribute("x0",&seed)
										== tinyxml2::XML_NO_ERROR)?seed:0);
		}
		else if(name.compare("Output") == 0)
		{
			const char* nam = element->Attribute("filename");
			if(nam != 0)
				eventgenerator.SetOutputFileName(std::string(nam));
		}
		else if(name.compare("EventRate") == 0)
		{
			double rate;
			eventgenerator.SetEventRate((element->QueryDoubleAttribute("f",&rate) 
											== tinyxml2::XML_NO_ERROR)?rate:0);
		}
		else if(name.compare("ClusterSize") == 0)
		{
			double size;
			eventgenerator.SetClusterSize((element->QueryDoubleAttribute("sigma",&size)
											== tinyxml2::XML_NO_ERROR)?size:0);
		}
		else if(name.compare("CutOffFactor") == 0)
		{
			int cutoff;
			eventgenerator.SetCutOffFactor((element->QueryIntAttribute("numsigmas",&cutoff)
											== tinyxml2::XML_NO_ERROR)?cutoff:0);			
		}
		else if(name.compare("InclinationSigma") == 0)
		{
			double inclsigma;
			eventgenerator.SetInclinationSigma((element->QueryDoubleAttribute("sigma",&inclsigma)
												== tinyxml2::XML_NO_ERROR)?inclsigma:3);
		}
		else if(name.compare("ChargeScale") == 0)
		{
			double scale;
			eventgenerator.SetChargeScaling((element->QueryDoubleAttribute("scale", &scale)
											== tinyxml2::XML_NO_ERROR)?scale:1);
		}
		else if(name.compare("MinSize") == 0)
		{
			double minsize;
			eventgenerator.SetMinSize((element->QueryDoubleAttribute("diagonal", &minsize)
										== tinyxml2::XML_NO_ERROR)?minsize:1);
		}
		else if(name.compare("NumEvents") == 0)
		{
			if(element->QueryIntAttribute("n", &events) != tinyxml2::XML_NO_ERROR)
				events = 0;
		}
		else if(name.compare("EventStart") == 0)
		{
			if(element->QueryDoubleAttribute("t", &starttime) != tinyxml2::XML_NO_ERROR)
				starttime = 0;	
		}


		if(element != eventgen->LastChildElement())
			element = element->NextSiblingElement();
		else
			element = 0;
	}
}

std::string Simulator::PrintDetectors()
{
	std::stringstream s("");
	for(auto it = detectors.begin(); it != detectors.end(); ++it)
	{
		if(it != detectors.begin())
			s << std::endl;
		s << (*it)->PrintDetector();
	}

	return s.str();
}

ReadoutCell Simulator::LoadROC(tinyxml2::XMLElement* parent, TCoord<double> pixelsize,
								std::string defaultaddressname)
{
	std::cout << "    LoadROC" << std::endl;

	static std::map<std::string, int> latestindex;

	const char* nam = parent->Attribute("addrname");
	std::string addressname = (nam != 0)?std::string(nam):defaultaddressname;
	//if(addressname == "")
	//	addressname = defaultaddressname;

	int address;
	tinyxml2::XMLError error = parent->QueryIntAttribute("addr", &address);
	if(latestindex.find(addressname) != latestindex.end())
	{
		if(error != tinyxml2::XML_NO_ERROR)
			address = ++latestindex[addressname];
		else if(address > latestindex[addressname])
			latestindex[addressname] = address;
		//else
			//nothing to do
	}
	else
	{
		if(error != tinyxml2::XML_NO_ERROR)
			address = 0;

		latestindex.insert(std::make_pair(addressname,address));
	}

	//check whether a name for child ROCs is given:
	nam = parent->Attribute("childaddrname");
	std::string childaddressname = (nam != 0)?std::string(nam):("c" + defaultaddressname);
	//if(childaddressname == "")
	//	childaddressname = "c" + defaultaddressname;

	int queuelength;
	error = parent->QueryIntAttribute("queuelength", & queuelength);
	if(error != tinyxml2::XML_NO_ERROR)
		queuelength = 1;

	//Configuration of the ROC:
	int configuration = 0;
	
	//PPtB Readout (without alternative at the moment):
	bool pptb = true;
	error = parent->QueryBoolAttribute("pptb", &pptb);
	if(error != tinyxml2::XML_NO_ERROR)
		pptb = true;
	if(pptb)
		configuration |= ReadoutCell::PPTB;
	
	//zero suppression:
	bool zerosuppr = true;
	error = parent->QueryBoolAttribute("zerosuppression", &zerosuppr);
	if(error != tinyxml2::XML_NO_ERROR)
		zerosuppr = true;
	if(zerosuppr)
		configuration |= ReadoutCell::ZEROSUPPRESSION;

	//buffer type:
	nam = parent->Attribute("buffertype");
	std::string buffertype = (nam != 0)?std::string(nam):"FIFOBuffer";
	if(buffertype.compare("PrioBuffer") == 0)
		configuration |= ReadoutCell::PRIOBUFFER;
	else //if(buffertype.compare("FIFOBuffer") == 0)
		configuration |= ReadoutCell::FIFOBUFFER;

	//readout mechanism:
	nam = parent->Attribute("readoutmechanism");
	std::string readouttype = (nam != 0)?std::string(nam):"NoReadOnFull";
	if(readouttype.compare("OverWrite") == 0)
		configuration |= ReadoutCell::OVERWRITEONFULL;
	else if(readouttype.compare("NoOverWrite") == 0)
		configuration |= ReadoutCell::NOOVERWRITE;
	else //if(readouttype.compare("NoReadOnFull") == 0)
		configuration |= ReadoutCell::NOREADONFULL;

	ReadoutCell roc(addressname, address, queuelength, configuration);

	tinyxml2::XMLElement* child = parent->FirstChildElement();
	while(child != 0)
	{
		std::string childname = std::string(child->Value());
		if(childname.compare("ROC") == 0)
			roc.AddROC(LoadROC(child, pixelsize, childaddressname));
		else if(childname.compare("Pixel") == 0)
			roc.AddPixel(LoadPixel(child, pixelsize));
		else if(childname.compare("NTimes") == 0)
			LoadNPixels(&roc, child, pixelsize);

		if(child != parent->LastChildElement())
			child = child->NextSiblingElement();
		else
			child = 0;
	}

	return roc;
}

Pixel Simulator::LoadPixel(tinyxml2::XMLElement* parent, TCoord<double> pixelsize)
{
	std::cout << "    LoadPixel" << std::endl;

	TCoord<double> position;
	TCoord<double> size = pixelsize;
	double threshold = 0.;
	double efficiency = 1.;
	double deadtimescaling = 1.;

	static int lastaddress = -1;
	int address;

	tinyxml2::XMLError error = parent->QueryIntAttribute("addr", &address);
	if(error != tinyxml2::XML_NO_ERROR)
		address = ++lastaddress;
	else
		lastaddress = address;

	const char* nam = parent->Attribute("addrname");
	std::string addrname = (nam != 0)?std::string(nam):"pix";


	tinyxml2::XMLElement* properties = parent->FirstChildElement();
	while(properties != 0)
	{
		std::string name = std::string(properties->Value());
		if(name.compare("Position") == 0)
			position = LoadTCoord(properties);
		else if(name.compare("Size") == 0)
			size = LoadTCoord(properties);
		else if(name.compare("Threshold") == 0)
		{
			tinyxml2::XMLError error = properties->QueryDoubleAttribute("thr", &threshold);
			if(error != tinyxml2::XML_NO_ERROR)
				threshold = 0.;
		}
		else if(name.compare("Efficiency") == 0)
		{
			tinyxml2::XMLError error = properties->QueryDoubleAttribute("n", &efficiency);
			if(error != tinyxml2::XML_NO_ERROR)
				efficiency = 1.;
		}
		else if(name.compare("DeadTimeScaling") == 0)
		{
			tinyxml2::XMLError error = properties->QueryDoubleAttribute("factor", 
														&deadtimescaling);
			if(error != tinyxml2::XML_NO_ERROR)
				deadtimescaling = 1.;
		}

		if(properties != parent->LastChildElement())
			properties = properties->NextSiblingElement();
		else
			properties = 0;
	}

	Pixel pix(position, size, addrname, address, threshold);
	pix.SetEfficiency(efficiency);
	pix.SetDeadTimeScaling(deadtimescaling);

	return pix;
}

void Simulator::LoadNPixels(ReadoutCell* parentcell, tinyxml2::XMLElement* parentnode, 
							TCoord<double> pixelsize)
{
	//for a PPtB ROC, the pixel addresses have to be provided:
	if(parentcell == 0)
		return;

	int numelements;
	TCoord<double> shift;
	tinyxml2::XMLError error;

	error = parentnode->QueryIntAttribute("n", &numelements);
	if(error != tinyxml2::XML_NO_ERROR)
		numelements = 0;
	shift = LoadTCoord(parentnode);

	Pixel pix;
	ReadoutCell roc;
	int pixelset = 0;	//to check whether a pixel was defined in this block
	tinyxml2::XMLElement* elem = parentnode->FirstChildElement();
	while(elem != 0)
	{
		std::string name = std::string(elem->Value());
		if(name.compare("Pixel") == 0)
		{
			pix = LoadPixel(elem, pixelsize);
			pixelset = 1;
		}
		else if(name.compare("ROC") == 0)
		{
			roc = LoadROC(elem, pixelsize, "c" + parentcell->GetAddressName());
			pixelset = 2;
		}

		if(elem != parentnode->LastChildElement())
			elem = elem->NextSiblingElement();
		else
			elem = 0;
	}

	if(pixelset == 1)
	{
		for(int i = 0; i < numelements; ++i)
		{
			parentcell->AddPixel(pix);
			pix.SetPosition(pix.GetPosition() + shift);
			pix.SetAddress(pix.GetAddress() + 1);
		}
	}
	else if(pixelset == 2)
	{
		for(int i = 0; i< numelements; ++i)
		{
			parentcell->AddROC(roc);
			roc.ShiftCell(shift);
			roc.SetAddress(roc.GetAddress() + 1);
		}
	}

	return;
}
