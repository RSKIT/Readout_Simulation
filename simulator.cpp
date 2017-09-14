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

#include "simulator.h"

Simulator::Simulator() : detectors(std::vector<DetectorBase*>()), eventgenerator(EventGenerator()),
		events(0), starttime(0), stoptime(-1), stopdelay(0), inputfile(""), logfile(""),
		logcontent(std::string("")), archivename(""), archiveonly(false), 
		inputfilecontent(std::string("")), outputlevel(23), tsprintpitch(10), 
		triggersorting(false)
{

}

Simulator::Simulator(std::string filename) : detectors(std::vector<DetectorBase*>()), 
		eventgenerator(EventGenerator()), events(0), starttime(0), stoptime(-1), stopdelay(0),
		inputfile(filename), logfile(""), logcontent(std::string("")), archivename(""), 
		archiveonly(false), inputfilecontent(std::string("")), 
		outputlevel(23), tsprintpitch(10), triggersorting(false)
{

}

void Simulator::Cleanup()
{
	for(auto& it : detectors)
	{
		it->Cleanup();
		delete it;
	}

	detectors.clear();

	eventgenerator.ClearDetectors();
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
	bool checkaddresses = false;

	tinyxml2::XMLElement* simulation = doc.FirstChildElement();
	tinyxml2::XMLElement* elem = simulation->FirstChildElement();
	while(elem != 0)
	{
		std::string elementname = std::string(elem->Name());

		tinyxml2::XMLElement* newelem = elem;
		if(elementname.compare("Scan") == 0)
		{
			newelem = ScanNode(elem);
			elementname = std::string(newelem->Name());
		}

		if(outputlevel & loadsimulation)
			std::cout << "Element: " << elementname << std::endl;

		if(elementname.compare("Detector") == 0)
			LoadDetector(newelem, standardpixel);
		else if(elementname.compare("Standardpixel") == 0)
			standardpixel = LoadTCoord(newelem);
		else if(elementname.compare("EventGenerator") == 0)
			LoadEventGenerator(newelem);
		else if(elementname.compare("SortTriggerTimeStamps") == 0)
		{
			if(newelem->QueryBoolAttribute("sort", &triggersorting) != tinyxml2::XML_NO_ERROR)
				triggersorting = false;
		}
		else if(elementname.compare("SimulationEnd") == 0)
		{
			//load end time:
			tinyxml2::XMLError error = newelem->QueryIntAttribute("t", &stoptime);
			if(error != tinyxml2::XML_NO_ERROR)
				stoptime = -1;
			//load delay for stopping:
			error = newelem->QueryIntAttribute("stopdelay", &stopdelay);
			if(error != tinyxml2::XML_NO_ERROR)
				stopdelay = 0;
		}
		else if(elementname.compare("Logging") == 0)
		{
			const char* nam = newelem->Attribute("filename");
			logfile = (nam != 0)?std::string(nam):"";
			printdetector = false;
			if(newelem->QueryBoolAttribute("printdetectors", &printdetector) 
					!= tinyxml2::XML_NO_ERROR)
				printdetector = false;
		}
		else if(elementname.compare("Archive") == 0)
		{
			const char* nam = newelem->Attribute("filename");
			archivename = (nam != 0)?std::string(nam):"";
			if(newelem->QueryBoolAttribute("archiveonly", &archiveonly) != tinyxml2::XML_NO_ERROR
				|| archivename == "")
				archiveonly = false;
		}
		else if(elementname.compare("CheckAddresses") == 0)
		{
			if(newelem->QueryBoolAttribute("check", &checkaddresses) != tinyxml2::XML_NO_ERROR)
				checkaddresses = false;
		}
		else if(elementname.compare("Output") == 0)
		{
			outputlevel = 0;
			tsprintpitch = 10;

			bool test = false;
			if(newelem->QueryBoolAttribute("loadsimulation", &test) == tinyxml2::XML_NO_ERROR)
				outputlevel |= ((test)?loadsimulation:0);
			if(newelem->QueryBoolAttribute("eventgeneration", &test) == tinyxml2::XML_NO_ERROR)
				outputlevel |= ((test)?eventgeneration:0);
			if(newelem->QueryBoolAttribute("statemachineoutput", &test) == tinyxml2::XML_NO_ERROR)
				outputlevel |= ((test)?statemachineoutput:0);
			if(newelem->QueryBoolAttribute("timestampoutput", &test) == tinyxml2::XML_NO_ERROR)
				outputlevel |= ((test)?timestampoutput:0);
			if(newelem->QueryBoolAttribute("eventinsertion", &test) == tinyxml2::XML_NO_ERROR)
				outputlevel |= ((test)?eventinsertion:0);

			if(newelem->QueryIntAttribute("outputpitch", &tsprintpitch) != tinyxml2::XML_NO_ERROR)
				tsprintpitch = 10;
		}

		if(elem != simulation->LastChildElement())
			elem = elem->NextSiblingElement();
		else
			elem = 0;
	}

	//check for multiple uses of addresses
	if(checkaddresses)
	{
		bool changewasnecessary = false;
		std::vector<int> usedaddresses;
		for(auto it = detectors.begin(); it != detectors.end(); ++it)
		{
	        auto addrit = find(usedaddresses.begin(), usedaddresses.end(), (*it)->GetAddress());
	        while(addrit != usedaddresses.end())
	        {
	            (*it)->SetAddress((*it)->GetAddress() + 1);
	            addrit = find(usedaddresses.begin(), usedaddresses.end(), (*it)->GetAddress());

	            changewasnecessary = true;
	        }

	        usedaddresses.push_back((*it)->GetAddress());

	        changewasnecessary |= (*it)->CheckROCAddresses();
		}

		if(changewasnecessary)
		{
			logcontent += "Address Changes were applied ...\n";
			std::cout << "Address Changes were applied ..." << std::endl;
		}
	}

	for(auto it = detectors.begin(); it != detectors.end(); ++it)
	{
		(*it)->EnlargeSize();
		eventgenerator.AddDetector(*it);
	}

	if(logfile != "")
		logcontent += "Loading data from \"" + filename + "\" ...\n";

	if(archivename != "")
	{
		tinyxml2::XMLPrinter printer;
    	doc.Print( &printer );
		inputfilecontent = std::string(printer.CStr());
	}
}

std::string Simulator::GetLoggingFile()
{
	return logfile;
}

void Simulator::SetLoggingFile(std::string filename)
{
	logfile = filename;
}

bool Simulator::GetDetectorLogging()
{
	return printdetector;
}

void Simulator::SetDetectorLogging(bool printdetector)
{
	this->printdetector = printdetector;
}

std::string Simulator::GetArchiveName()
{
	return archivename;
}

void Simulator::SetArchiveName(std::string archivename)
{
	this->archivename = archivename;
}

bool Simulator::GetArchiveOnly()
{
	return archiveonly;
}

void Simulator::SetArchiveOnly(bool archiveonly)
{
	this->archiveonly = archiveonly;
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

Simulator::eventdata* Simulator::GetEventOrder(int index)
{
	if(index < 0 || index > eventstoload.size())
		return 0;
	else
		return &eventstoload[index];
}

void Simulator::AddEventOrder(eventdata order)
{
	eventstoload.push_back(order);
}

void Simulator::RemoveEventOrders()
{
	eventstoload.clear();
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

int Simulator::GetOutputFlags()
{
	return outputlevel;
}

void Simulator::SetOutputFlags(int flags)
{
	outputlevel = flags;
}

int Simulator::GetTSPrintPitch()
{
	return tsprintpitch;
}

void Simulator::SetTSPrintPitch(int pitch)
{
	if(pitch > 0)
		tsprintpitch = pitch;
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
	detectors.push_back(detector->Clone());
	auto it = detectors.end();
	--it;
	eventgenerator.AddDetector(*it);
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

	eventgenerator.GenerateEvents(starttime, events, -1, !archiveonly, 
									(outputlevel & loadsimulation), tsprintpitch);
	events = 0;
}

void Simulator::GenerateEvents(int events, double starttime)
{
	//legacy call support: before the pregenerated data was available, the generator was just
	//  called with a number of events
	if(events > 0)
	{
		eventdata newevents;
		newevents.datatype = Simulator::GenerateNewEvents;
		newevents.numevents = events;
		newevents.starttime = starttime;

		eventstoload.push_back(newevents);
	}

	if(outputlevel & eventgeneration)
		std::cout << "Entries in loadqueue: " << eventstoload.size() << std::endl;

	//set up detector characteristics:
	eventgenerator.SetupTimeWalkSpline();
	eventgenerator.SetupDeadTimeSpline();

	for(auto& it : eventstoload)
	{
		switch(it.datatype)
		{
			case(GenerateNewEvents):
			    eventgenerator.GenerateEvents(it.starttime, it.numevents, -1, !archiveonly,
			    						(outputlevel & eventgeneration), tsprintpitch);
			    break;
			case(PixelHitFile):
			    eventgenerator.LoadEventsFromFile(it.source, true, it.starttime);
			    break;
			case(ITkFile):
			    eventgenerator.LoadITkEvents(it.source, it.firstevent, it.numevents, it.starttime,
			    								it.eta, TCoord<double>::Null, -1, !archiveonly,
			    								it.distance, it.sort, 
			    								(outputlevel & eventgeneration), tsprintpitch);
			    break;
			default:
				std::cout << "Unknown Data Type for source \"" << it.source << "\"" << std::endl;
				break;
		}
	}
	if(eventstoload.size() > 1)
		eventgenerator.SortEventQueue();

	eventstoload.clear();
}

bool Simulator::ClockUp(int timestamp)
{
	for(auto it = detectors.begin(); it != detectors.end(); ++it)
	{
		if(!(*it)->StateMachineCkUp(timestamp, eventgenerator.GetTriggerState(timestamp),
				(outputlevel & statemachineoutput), tsprintpitch))
			return false;
	}
	return true;
}

bool Simulator::ClockDown(int timestamp)
{
	for(auto it = detectors.begin(); it != detectors.end(); ++it)
	{
		if(!(*it)->StateMachineCkDown(timestamp, eventgenerator.GetTriggerState(timestamp),
					(outputlevel & statemachineoutput), tsprintpitch))
			return false;
	}

	return true;
}

void Simulator::SimulateUntil(int stoptime, int delaystop)
{
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	//if(events > 0)
	//{
	//	eventgenerator.GenerateEvents(starttime, events, -1, !archiveonly);
	//	events = 0;
	//}
	if(eventstoload.size() > 0)
		GenerateEvents();

	//only show the event queue if it is reasonably short:
	if((outputlevel & eventgeneration) != 0 && eventgenerator.GetNumEventsLeft() <= 100)
		eventgenerator.PrintQueue();

	std::chrono::steady_clock::time_point endEventGen = std::chrono::steady_clock::now();

	if(triggersorting)
		eventgenerator.SortOnTimeStamps();	//sort the trigger turn on timestamps

	int timestamp = 0;
	double nextevent = eventgenerator.GetHit().GetTimeStamp();

	int hitcounter = 0;

	//turn off the trigger if no trigger signals are prepared but they are foreseen 
	// (probability < 1):
	if(eventgenerator.GetTriggerProbability() > 0 && eventgenerator.GetTriggerProbability() < 1
			&& eventgenerator.GetNumOnTimeStamps() == 0)
		eventgenerator.SetTriggerOffTime(timestamp);

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
					if((*it)->PlaceHit(hit, timestamp))
						break;
				}

				++hitcounter;
			}

			if(outputlevel & eventinsertion)
				std::cout << "Inserted " << hitcounter << " signals by now..." << std::endl;
		}

		if(!ClockUp(timestamp))
			break;

		if(!ClockDown(timestamp))
			break;

		++timestamp;

		//delay the stopping of the simulation for "stop-on-done" (see while()):
		if(nextevent == -1 && eventgenerator.GetNumOnTimeStamps() == 0)
		{
			//count the remaining events:
			int hitcount = 0;
			for(auto& it : detectors)
			{
				//check for remaining triggers to read out
				if(it->GetTriggerTableEntries() > 0)
				{
					hitcount += 1;
					break;
				}
				//check for triggered hit gap fill readout or for unsorted detectors:
				else if(it->GetGapFill() || it->GetTriggerTableDepth() == 0)
				{
					hitcount += it->HitsEnqueued();
					break;
				}
			}
			if(hitcount == 0)
				--stopdelay;
		}

		if(outputlevel & statemachineoutput)
			std::cout << "        TS: " << timestamp << "; nextEvent: " << nextevent 
						<< "; stopdelay: " << stopdelay << std::endl;
		//short output of timestamp:
		else if((outputlevel & timestampoutput) != 0 && (timestamp % tsprintpitch) == 0)
		{
			static int lasteventtimestamp = -1;
			if(eventgenerator.GetLastEventTimestamp() != -1)
				lasteventtimestamp = eventgenerator.GetLastEventTimestamp();
			std::cout << "Timestamp (current/last event's): " << timestamp << "/" 
						<< lasteventtimestamp << " (Stop delay: " << stopdelay << ")" 
						<< std::endl;
		}
	}

	//dump the remaining hits from the detectors into the corresponding lost hit files:
	int remaininghits = 0;
	for(auto& it : detectors)
		remaininghits += it->WriteRemainingHitsToBadOut(timestamp);
	//write out later...

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	//Write out end output:
	zip_file archive;	//zip archive to write compressed data to
	zip_file oldarchive;

	//try to load an existing archive to add the data to:
	if(archivename != "")
	{
		std::fstream f;
		f.open(archivename.c_str(), std::ios::in | std::ios::binary);
		if(f.is_open())
		{
			f.close();
			oldarchive.load(archivename);
		}
	}

	//save event generator output to the archive (writing out to file was already done 
	//  if it was requested)
	if(archivename != "")
	{
		std::string filename = eventgenerator.GetOutputFileName();
		//check whether the file exists already in the archive
		if(oldarchive.has_file(filename))
		{
			std::stringstream s("");
			s << oldarchive.read(filename) << std::endl
			  << eventgenerator.GenerateLog();

			archive.writestr(filename, s.str());
		}
		else
			archive.writestr(eventgenerator.GetOutputFileName(), eventgenerator.GenerateLog());

		eventgenerator.ClearLog();
	}

	//count the signals read out from the detectors:
	int dethitcounter = 0;
	for(auto it = detectors.begin(); it != detectors.end(); ++it)
	{
		if(archivename != "")
		{
			if(oldarchive.has_file((*it)->GetOutputFile()))
			{
				std::stringstream s("");
				s << oldarchive.read((*it)->GetOutputFile()) << std::endl
				  << (*it)->GenerateOutput();
				archive.writestr((*it)->GetOutputFile(), s.str());
			}
			else
				archive.writestr((*it)->GetOutputFile(), (*it)->GenerateOutput());

			if(oldarchive.has_file((*it)->GetBadOutputFile()))
			{
				std::stringstream s("");
				s << oldarchive.read((*it)->GetBadOutputFile()) << std::endl
				  << (*it)->GenerateBadOutput();
				archive.writestr((*it)->GetBadOutputFile(), s.str());
			}
			else
				archive.writestr((*it)->GetBadOutputFile(), (*it)->GenerateBadOutput());
		}
		if(!archiveonly)
		{
			(*it)->FlushOutput();
			(*it)->ClearOutput();
			(*it)->FlushBadOutput();
			(*it)->ClearBadOutput();
		}
		else
		{
			(*it)->ClearOutput();
			(*it)->ClearBadOutput();
		}
		dethitcounter += (*it)->GetHitCounter();
	}

	if(outputlevel & eventinsertion)
		std::cout << "Simulation done." << std::endl << "  injected signals: " << hitcounter
				  << std::endl << "  read out signals: " << dethitcounter << std::endl
				  << "  Efficiency:       " << dethitcounter/double(hitcounter) << std::endl;

	std::cout  << "Remaining Hits in the detector(s): " << remaininghits << std::endl
			   << "Event Generation Time: " << TimesToInterval(begin, endEventGen) << std::endl
			   << "Simulation Time:       " << TimesToInterval(endEventGen, end) << std::endl;

	//save the log file:
	if(logfile != "")
	{
		//generate log content:
		if(printdetector)
			logcontent += PrintDetectors() + "\n";
		std::stringstream s("");
		s << "Simulated " << timestamp << " timestamps" << std::endl;

		s << "Remaining Hits in the detector(s): " << remaininghits << std::endl;

		s << "Simulation done." << std::endl 
		  << "  injected signals: " << hitcounter << std::endl 
		  << "  read out signals: " << dethitcounter << std::endl
		  << "  Efficiency:       " << dethitcounter/double(hitcounter) << std::endl;

		s << "Event Generation Time: " << TimesToInterval(begin, endEventGen) << std::endl
		  << "Simulation Time:       " << TimesToInterval(endEventGen, end) << std::endl;

		logcontent += s.str();

		//save to archive:
		if(archivename != "")
		{
			if(oldarchive.has_file(logfile.c_str()))
			{
				//std::stringstream s("");
				//s << oldarchive.read(logfile) << std::endl << logcontent;
				archive.writestr(logfile, oldarchive.read(logfile) + "\n" + logcontent);
			}
			else
				archive.writestr(logfile, logcontent);
		}

		//save to normal file:
		if(!archiveonly)
		{
			std::fstream logf;
			logf.open(logfile.c_str(), std::ios::out | std::ios::app);
			if(logf.is_open())
			{
				logf << logcontent;
				logf.close();
			}			
		}

		logcontent = "";
	}

	//add the XML configuration file to the archive:
	if(archivename != "")
	{
		//write the input XML file into the archive:
		if(oldarchive.has_file(inputfile))
		{
			std::stringstream s("");
			s << inputfile; // the initial filename
			int counter = 0;

			//extract filename and ending from the initial filename:
			int pointposition = inputfile.find(".");
			std::string filenamestart = inputfile.substr(0,pointposition);
			std::string filenameend = inputfile.substr(pointposition, std::string::npos);

			//copy all existing files from the old archive to the new one:
			do{
				archive.writestr(s.str(), oldarchive.read(s.str()));
				s.str("");
				s << filenamestart << "_" << ++counter << filenameend;
			}while(oldarchive.has_file(s.str()));

			//write the new configuration into the new archive:
			archive.writestr(s.str(), inputfilecontent);
		}
		else
			archive.writestr(inputfile, inputfilecontent);

		//copy the other files from the old archive:
		std::vector<std::string> oldfiles = oldarchive.namelist();
		for(auto& it : oldfiles)
		{
			if(!archive.has_file(it))
				archive.writestr(it, oldarchive.read(it));
		}

	}

	oldarchive.reset();

	//save the archive:
	if(archivename != "")
		archive.save(archivename);
}

void Simulator::LoadDetector(tinyxml2::XMLElement* parent, TCoord<double> pixelsize)
{
	if(outputlevel & loadsimulation)
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

	//optional check for double addresses:
	bool checkafterbuild = false;
	if(parent->QueryBoolAttribute("CheckAfterBuild", &checkafterbuild) != tinyxml2::XML_NO_ERROR)
		checkafterbuild = false;

	//length of a hit sorting FIFO (only used with SortedROCReadout):
	int trigtablength = 0;
	if(parent->QueryIntAttribute("TriggerTableLength", &trigtablength) != tinyxml2::XML_NO_ERROR)
		trigtablength = 0;

	int trigtimemask = 0;
	if(parent->QueryIntAttribute("TriggerTimeStampMask", &trigtimemask) != tinyxml2::XML_NO_ERROR)
		trigtimemask = 0;

	bool gapfill = false;
	if(parent->QueryBoolAttribute("TriggerGapFill", &gapfill) != tinyxml2::XML_NO_ERROR)
		gapfill = false;

	DetectorBase* det = new Detector(addressname, address);
	det->SetOutputFile(outputfile);
	det->SetBadOutputFile(badoutputfile);

	det->SetTriggerTableDepth(trigtablength);
	det->SetTriggerTimeMask(trigtimemask);
	det->SetGapFill(gapfill);

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
		else if(childname.compare("StateMachine") == 0)	
		{
			DetectorBase* detst = LoadStateMachine(det, child);
			det->Cleanup();
			delete det;
			det = detst;
			detst = 0;
		}

		if(child != parent->LastChildElement())
			child = child->NextSiblingElement();
		else
			child = 0;
	}

	det->EnlargeSize();

	//optional check for double addesses:
	if(checkafterbuild)
	{
		if(det->CheckROCAddresses())
		{
			logcontent += "Address Changes were applied ...\n";
			if(outputlevel & loadsimulation)
				std::cout << "Address Changes were applied ..." << std::endl;
		}
	}

	//Set the detector pointer of SortedROCReadout:
	for(auto it = det->GetROCVectorBegin(); it != det->GetROCVectorEnd(); ++it)
		it->SetTriggerTableFrontPointer(det->GetTriggerTableFrontPointer(), 
											det->GetTriggerTimeMask());

	AddDetector(det);

	delete det;
}

TCoord<double> Simulator::LoadTCoord(tinyxml2::XMLElement* coordinate)
{
	if(outputlevel & loadsimulation)
		std::cout << "  LoadTCoord" << std::endl;

	TCoord<double> coord;
	const char* axis[] = {"x","y","z"};

	for(int i = 0; i < 3; ++i)
	{
		if(coordinate->QueryDoubleAttribute(axis[i], &coord[i]) != tinyxml2::XML_NO_ERROR)
			coord[i] = 0;
	}

	return coord;
}

void Simulator::LoadEventGenerator(tinyxml2::XMLElement* eventgen)
{
	if(outputlevel & loadsimulation)
		std::cout << "  LoadEventGenerator" << std::endl;

	eventgenerator = EventGenerator();

	tinyxml2::XMLElement* element = eventgen->FirstChildElement();
	while(element != 0)
	{
		std::string name = std::string(element->Value());

		tinyxml2::XMLElement* newelement = element;
		if(name.compare("Scan") == 0)
		{
			newelement = ScanNode(element);
			name = std::string(newelement->Value());
		}

		if(name.compare("Seed") == 0)
		{
			int seed;
			eventgenerator.SetSeed((newelement->QueryIntAttribute("x0",&seed)
										== tinyxml2::XML_NO_ERROR)?seed:0);
		}
		else if(name.compare("Output") == 0)
		{
			const char* nam = newelement->Attribute("filename");
			if(nam != 0)
				eventgenerator.SetOutputFileName(std::string(nam));
		}
		else if(name.compare("EventRate") == 0)
		{
			double rate;
			if(newelement->QueryDoubleAttribute("f",&rate) != tinyxml2::XML_NO_ERROR)
				rate = 0;
			bool totalrate;
			if(newelement->QueryBoolAttribute("absolute", &totalrate) != tinyxml2::XML_NO_ERROR)
				totalrate = true;
			eventgenerator.SetEventRate(rate, totalrate);

		}
		else if(name.compare("ClusterSize") == 0)
		{
			double size;
			eventgenerator.SetClusterSize((newelement->QueryDoubleAttribute("sigma",&size)
											== tinyxml2::XML_NO_ERROR)?size:0);
		}
		else if(name.compare("CutOffFactor") == 0)
		{
			int cutoff;
			eventgenerator.SetCutOffFactor((newelement->QueryIntAttribute("numsigmas",&cutoff)
											== tinyxml2::XML_NO_ERROR)?cutoff:0);			
		}
		else if(name.compare("InclinationSigma") == 0)
		{
			double inclsigma;
			eventgenerator.SetInclinationSigma(
				(newelement->QueryDoubleAttribute("sigma", &inclsigma)
												== tinyxml2::XML_NO_ERROR)?inclsigma:3);
		}
		else if(name.compare("ChargeScale") == 0)
		{
			double scale;
			eventgenerator.SetChargeScaling((newelement->QueryDoubleAttribute("scale", &scale)
											== tinyxml2::XML_NO_ERROR)?scale:1);
		}
		else if(name.compare("MinSize") == 0)
		{
			double minsize;
			eventgenerator.SetMinSize((newelement->QueryDoubleAttribute("diagonal", &minsize)
										== tinyxml2::XML_NO_ERROR)?minsize:1);
		}
		else if(name.compare("NumEvents") == 0)
		{
			eventdata newevents;
			newevents.datatype = Simulator::GenerateNewEvents;

			if(newelement->QueryIntAttribute("n", &newevents.numevents) != tinyxml2::XML_NO_ERROR)
				newevents.numevents = 0;
			if(newelement->QueryDoubleAttribute("start", &newevents.starttime) 
					!= tinyxml2::XML_NO_ERROR)
				newevents.starttime = 0;

			eventstoload.push_back(newevents);
		}
		else if(name.compare("InputFile") == 0)
		{
			eventdata newevents;
			newevents.datatype = Simulator::PixelHitFile;

			if(newelement->QueryBoolAttribute("sort", &newevents.sort) != tinyxml2::XML_NO_ERROR)
				newevents.sort = true;
			if(newelement->QueryDoubleAttribute("timeshift", &newevents.starttime) 
					!= tinyxml2::XML_NO_ERROR)
				newevents.starttime = 0.;
			const char* nam = newelement->Attribute("filename");
			if(nam != 0)
				newevents.source = std::string(nam);

			eventstoload.push_back(newevents);
		}
		else if(name.compare("ITkEvents") == 0)
		{
			eventdata neweventgroup;
			neweventgroup.datatype = datatypes::ITkFile;

			const char* nam = newelement->Attribute("filename");
			if(nam != 0)
				neweventgroup.source = std::string(nam);
			if(newelement->QueryIntAttribute("firstelement", &neweventgroup.firstevent) 
						!= tinyxml2::XML_NO_ERROR)
				neweventgroup.firstevent = 0;
			if(newelement->QueryIntAttribute("numelements", &neweventgroup.numevents)
						!= tinyxml2::XML_NO_ERROR)
				neweventgroup.numevents = -1;
			if(newelement->QueryIntAttribute("eta", &neweventgroup.eta) != tinyxml2::XML_NO_ERROR)
				neweventgroup.eta = 0;
			if(newelement->QueryDoubleAttribute("starttime", &neweventgroup.starttime)
						!= tinyxml2::XML_NO_ERROR)
				neweventgroup.starttime = 0;
			if(newelement->QueryBoolAttribute("sort", &neweventgroup.sort) 
						!= tinyxml2::XML_NO_ERROR)
				neweventgroup.sort = false;
			if(newelement->QueryDoubleAttribute("regroup", &neweventgroup.distance) 
						!= tinyxml2::XML_NO_ERROR)
				neweventgroup.distance = 0;

			eventstoload.push_back(neweventgroup);
		}
		//else if(name.compare("EventStart") == 0)
		//{
		//	if(newelement->QueryDoubleAttribute("t", &starttime) != tinyxml2::XML_NO_ERROR)
		//		starttime = 0;	
		//}
		else if(name.compare("TriggerProbability") == 0)
		{
			double probability = 0;
			if(newelement->QueryDoubleAttribute("p", &probability) != tinyxml2::XML_NO_ERROR)
				eventgenerator.SetTriggerProbability(0);
			else
				eventgenerator.SetTriggerProbability(probability);

			bool triggeronclusters = true;
			if(newelement->QueryBoolAttribute("triggeronclusters", &triggeronclusters) 
					!= tinyxml2::XML_NO_ERROR)
				eventgenerator.SetTriggerOnClusters(true);
			else
				eventgenerator.SetTriggerOnClusters(triggeronclusters);
		}
		else if(name.compare("TriggerDelay") == 0)
		{
			int delay = 0;
			if(newelement->QueryIntAttribute("delay", &delay) != tinyxml2::XML_NO_ERROR)
				eventgenerator.SetTriggerDelay(0);
			else
				eventgenerator.SetTriggerDelay(delay);
		}
		else if(name.compare("TriggerLength") == 0)
		{
			int length = 0;
			if(newelement->QueryIntAttribute("length", &length) != tinyxml2::XML_NO_ERROR)
				eventgenerator.SetTriggerLength(0);
			else
				eventgenerator.SetTriggerLength(length);
		}
		else if(name.compare("Threads") == 0)
		{
			int maxthreads = 0;
			if(newelement->QueryIntAttribute("n",&maxthreads) != tinyxml2::XML_NO_ERROR)
				eventgenerator.SetThreads(0);
			else
				eventgenerator.SetThreads(maxthreads);
		}
		else if(name.compare("DeadTimeCurve") == 0 || name.compare("TimeWalkCurve") == 0)
		{
			LoadSpline(&eventgenerator, newelement);
		}

		if(element != eventgen->LastChildElement())
			element = element->NextSiblingElement();
		else
			element = 0;
	}
}

void Simulator::LoadSpline(EventGenerator* eventgen, tinyxml2::XMLElement* element)
{
	if(outputlevel & loadsimulation)
		std::cout << "  LoadSpline" << std::endl;
	bool deadtime = (std::string(element->Value()).compare("DeadTimeCurve") == 0);
	tinyxml2::XMLElement* child = element->FirstChildElement();

	while(child != 0)
	{
		if(std::string(child->Value()).compare("Point") == 0)
		{
			double x, y;
			if(child->QueryDoubleAttribute("charge", &x) == tinyxml2::XML_NO_ERROR
				&& child->QueryDoubleAttribute("time", &y) == tinyxml2::XML_NO_ERROR)
			{
				if(deadtime)
					eventgen->AddDeadTimePoint(x, y);
				else
					eventgen->AddTimeWalkPoint(x, y);
			}
		}

		if(child != element->LastChildElement())
			child = child->NextSiblingElement();
		else
			child = 0;
	}

	//save the shape of the spline to a log file if a filename is provided:
	const char* nam = element->Attribute("filename");
	std::string filename = (nam != 0)?std::string(nam):"";
	double resolution = 0.1;
	if(element->QueryDoubleAttribute("resolution", &resolution) != tinyxml2::XML_NO_ERROR)
		resolution = 0.1;

	if(filename != "")
	{
		if(deadtime)
			eventgen->SaveDeadTimeSpline(filename, resolution);
		else
			eventgen->SaveTimeWalkSpline(filename, resolution);
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

bool Simulator::GoToNextParameterSetting(int scanid)
{
	// "user" call without scan id. Get the first one:
	if(scanid == -1)
	{
		if(scanindices.size() > 0)
			scanid = scanindices.begin()->first;
		else
			return false;
	}
	else //get the next scan id:
	{
		auto it = ++(scanindices.find(scanid));
		if(it != scanindices.end())
			scanid = it->first;
		else //return on non existing next scan id
			return false;
	}
	//scanid is a valid key now

	auto it = scanindices.find(scanid);
	if(it->second < scanindexmaxima.find(scanid)->second)
	{
		++(it->second);
		return true;
	}
	else
	{
		it->second = 0;
		return GoToNextParameterSetting(scanid);
	}
}

int Simulator::GetNumParameterSettings()
{
	int settings = 1;
	for(auto& it : scanindexmaxima)
		settings *= it.second + 1;

	return settings;
}

void Simulator::ClearScanParameters()
{
	scanindices.clear();
	scanindexmaxima.clear();
}

ReadoutCell Simulator::LoadROC(tinyxml2::XMLElement* parent, TCoord<double> pixelsize,
								std::string defaultaddressname)
{
	if(outputlevel & loadsimulation)
		std::cout << "    LoadROC" << std::endl;

	static std::map<std::string, int> latestindex;

	const char* nam = parent->Attribute("addrname");
	std::string addressname = (nam != 0)?std::string(nam):defaultaddressname;

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
	else if(readouttype.compare("OneByOne") == 0)
		configuration |= ReadoutCell::ONEBYONEREADOUT;
	else if(readouttype.compare("Token") == 0)
		configuration |= ReadoutCell::TOKENREADOUT;
	else if(readouttype.compare("Sorted") == 0)
		configuration |= ReadoutCell::SORTEDROCREADOUT;
	else //if(readouttype.compare("NoReadOnFull") == 0)
		configuration |= ReadoutCell::NOREADONFULL;

	//Readout Delay:
	int readoutdelay = 0;
	if(parent->QueryIntAttribute("ReadoutDelay", &readoutdelay) != tinyxml2::XML_NO_ERROR)
		readoutdelay = 0;

	//Triggered ROC:
	bool triggeredroc = false;
	if(parent->QueryBoolAttribute("Triggered", & triggeredroc) != tinyxml2::XML_NO_ERROR)
		triggeredroc = false;

	//Readout Delay Reference (only makes sense for triggered signals):
	std::string delayreference = "";
	nam = parent->Attribute("DelayReference");
	if(nam != 0)
		delayreference = std::string(nam);

	//Address checking:
	bool checkaddresses = false;
	if(parent->QueryBoolAttribute("CheckAfterBuild", &checkaddresses) != tinyxml2::XML_NO_ERROR)
		checkaddresses = false;

	ReadoutCell roc(addressname, address, queuelength, configuration);

	roc.SetReadoutDelay(readoutdelay);
	roc.SetTriggered(triggeredroc);
	roc.SetReadoutDelayReference(delayreference);

	tinyxml2::XMLElement* child = parent->FirstChildElement();
	while(child != 0)
	{
		std::string childname = std::string(child->Value());

		tinyxml2::XMLElement* newchild = child;
		if(childname.compare("Scan") == 0)
		{
			newchild = ScanNode(child);
			childname = std::string(newchild->Value());
		}

		if(childname.compare("ROC") == 0)
			roc.AddROC(LoadROC(newchild, pixelsize, childaddressname));
		else if(childname.compare("Pixel") == 0)
			roc.AddPixel(LoadPixel(newchild, pixelsize));
		else if(childname.compare("NTimes") == 0)
			LoadNPixels(&roc, newchild, pixelsize);
		else if(childname.compare("PixelLogic") == 0)
		{
			PixelLogic* logic = LoadPixelLogic(newchild);
			ComplexReadout* cro = new ComplexReadout(&roc);
			cro->SetPixelLogic(logic);
			roc.SetComplexPPtBReadout(cro);
		}

		if(child != parent->LastChildElement())
			child = child->NextSiblingElement();
		else
			child = 0;
	}

	if(checkaddresses)
		roc.CheckROCAddresses();

	return roc;
}

Pixel Simulator::LoadPixel(tinyxml2::XMLElement* parent, TCoord<double> pixelsize)
{
	if(outputlevel & loadsimulation)
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

		tinyxml2::XMLElement* newproperties = properties;
		if(name.compare("Scan") == 0)
		{
			newproperties = ScanNode(properties);
			name = std::string(newproperties->Value());
		}

		if(name.compare("Position") == 0)
			position = LoadTCoord(newproperties);
		else if(name.compare("Size") == 0)
			size = LoadTCoord(newproperties);
		else if(name.compare("Threshold") == 0)
		{
			tinyxml2::XMLError error = newproperties->QueryDoubleAttribute("thr", &threshold);
			if(error != tinyxml2::XML_NO_ERROR)
				threshold = 0.;
		}
		else if(name.compare("Efficiency") == 0)
		{
			tinyxml2::XMLError error = newproperties->QueryDoubleAttribute("n", &efficiency);
			if(error != tinyxml2::XML_NO_ERROR)
				efficiency = 1.;
		}
		else if(name.compare("DeadTimeScaling") == 0)
		{
			tinyxml2::XMLError error = newproperties->QueryDoubleAttribute("factor", 
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

PixelLogic* Simulator::LoadPixelLogic(tinyxml2::XMLElement* parent)
{
	if(parent == 0)
		return 0;

	if(outputlevel & loadsimulation)
		std::cout << "   LoadPixelLogic" << std::endl;

	PixelLogic* logic = new PixelLogic();

	const char* nam = parent->Attribute("operator");
	std::string operation = (nam != 0)?std::string(nam):"";

	//load the relation between the pixels:
	if(operation.compare("Or") == 0)
		logic->SetRelation(PixelLogic::Or);
	else if(operation.compare("Nor") == 0)
		logic->SetRelation(PixelLogic::Nor);
	else if(operation.compare("And") == 0)
		logic->SetRelation(PixelLogic::And);
	else if(operation.compare("Nand") == 0)
		logic->SetRelation(PixelLogic::Nand);
	else if(operation.compare("Xor") == 0)
		logic->SetRelation(PixelLogic::Xor);
	else if(operation.compare("Xnor") == 0)
		logic->SetRelation(PixelLogic::Xnor);
	else if(operation.compare("Not") == 0)
		logic->SetRelation(PixelLogic::Not);
	//else	//with this line a missing operation results in an error, without it an OR is assumed
	//	return 0;

	tinyxml2::XMLElement* child = parent->FirstChildElement();
	while(child != 0)
	{
		std::string name = std::string(child->Value());

		tinyxml2::XMLElement* newchild = child;
		if(name.compare("Scan") == 0)
		{
			newchild = ScanNode(child);
			name = std::string(newchild->Value());
		}

		if(name.compare("Pixel") == 0)
		{
			int address;
			if(newchild->QueryIntAttribute("addr", &address) != tinyxml2::XML_NO_ERROR)
				address = -1;
			bool ownaddress;
			if(newchild->QueryBoolAttribute("own", &ownaddress) != tinyxml2::XML_NO_ERROR)
				ownaddress = false;

			logic->AddPixelAddress(address, ownaddress);
		}
		else if(name.compare("PixelLogic") == 0)
			logic->AddPixelLogic(LoadPixelLogic(newchild));

		if(child != parent->LastChildElement())
			child = child->NextSiblingElement();
		else
			child = 0;
	}

	if(outputlevel & loadsimulation)
		std::cout << "Logic Sizes: all " << logic->GetNumPixelAddresses() << "; own "
					<< logic->GetNumOwnPixelAddresses() << std::endl;

	return logic;
}

void Simulator::LoadNPixels(ReadoutCell* parentcell, tinyxml2::XMLElement* parentnode, 
							TCoord<double> pixelsize, TCoord<double> globalshift)
{
	if(outputlevel & loadsimulation)
		std::cout << "    LoadNPixels" << std::endl;

	//for a PPtB ROC, the pixel addresses have to be provided:
	if(parentcell == 0)
		return;

	int numelements;
	TCoord<double> shift;
	tinyxml2::XMLError error;

	error = parentnode->QueryIntAttribute("n", &numelements);
	if(error != tinyxml2::XML_NO_ERROR)
		numelements = -1;	//set to -1 if missing and reserve 0 for shift of the contents
	shift = LoadTCoord(parentnode);

	//Pixel pix;
	//ReadoutCell roc;
	tinyxml2::XMLElement* elem = parentnode->FirstChildElement();
	while(elem != 0)
	{
		std::string name = std::string(elem->Value());

		tinyxml2::XMLElement* newelem = elem;
		if(name.compare("Scan") == 0)
		{
			newelem = ScanNode(elem);
			name = std::string(newelem->Value());
		}

		if(name.compare("Pixel") == 0)
		{
			Pixel pix = LoadPixel(newelem, pixelsize);

			if(pix.GetAddress() < parentcell->GetNumPixels())
				pix.SetAddress(parentcell->GetNumPixels());
			if(globalshift != TCoord<double>::Null)
				pix.SetPosition(pix.GetPosition() + globalshift);
			//shifting for single instance:
			if(numelements == 0)
			{
				pix.SetPosition(pix.GetPosition() + shift);
				parentcell->AddPixel(pix);
			}
			for(int i = 0; i < numelements; ++i)
			{
				parentcell->AddPixel(pix);
				pix.SetPosition(pix.GetPosition() + shift);
				pix.SetAddress(pix.GetAddress() + 1);
			}
		}
		else if(name.compare("ROC") == 0)
		{
			ReadoutCell roc = LoadROC(newelem, pixelsize, "c" + parentcell->GetAddressName());

			if(roc.GetAddress() < parentcell->GetNumROCs())
				roc.SetAddress(parentcell->GetNumROCs());
			if(globalshift != TCoord<double>::Null)
				roc.ShiftCell(globalshift);
			//shifting for single instance:
			if(numelements == 0)
			{
				roc.ShiftCell(shift);
				parentcell->AddROC(roc);
			}
			for(int i = 0; i < numelements; ++i)
			{
				parentcell->AddROC(roc);
				roc.ShiftCell(shift);
				roc.SetAddress(roc.GetAddress() + 1);
			}

			//roc.Cleanup();
		}
		else if(name.compare("NTimes") == 0)
		{
			TCoord<double> thisshift = globalshift;

			//shifting for single instance:
			if(numelements == 0)
				LoadNPixels(parentcell, newelem, pixelsize, thisshift + shift);

			for(int i = 0; i < numelements; ++i)
			{
				LoadNPixels(parentcell, newelem, pixelsize, thisshift);
				thisshift += shift;
			}
		}

		if(elem != parentnode->LastChildElement())
			elem = elem->NextSiblingElement();
		else
			elem = 0;
	}

	return;
}

XMLDetector* Simulator::LoadStateMachine(DetectorBase* detector, 
				tinyxml2::XMLElement* statemachine)
{
	if(outputlevel & loadsimulation)
		std::cout << "  LoadStateMachine" << std::endl;

	if(detector == 0 || statemachine == 0)
		return 0;

	tinyxml2::XMLError error;

	//transform the detector to an XMLDetector:
	XMLDetector* det = new XMLDetector(detector);
	

	tinyxml2::XMLElement* child = statemachine->FirstChildElement();

	while(child != 0)
	{
		std::string value = std::string(child->Value());

		tinyxml2::XMLElement* newchild = child;
		if(value.compare("Scan") == 0)
		{
			newchild = ScanNode(child);
			value = std::string(newchild->Value());
		}

		if(value.compare("Counter") == 0)
		{
			const char* nam = newchild->Attribute("name");
			std::string countername = (nam != 0)?std::string(nam):"";
			if(countername != "")
			{
				double cvalue;
				error = newchild->QueryDoubleAttribute("value", &cvalue);
				det->AddCounter(countername, cvalue);
			}
		}
		else if(value.compare("State") == 0)
					det->AddState(LoadState(newchild));

		if(child != statemachine->LastChildElement())
			child = child->NextSiblingElement();
		else
			child = 0;
	}

	while(det->GetState(det->GetState())->GetStateName().compare("synchronisation") == 0 
		&& det->GetState() < det->GetNumStates())
		det->SetState(det->GetState()+1);

	return det;
}

StateMachineState* Simulator::LoadState(tinyxml2::XMLElement* stateelement)
{
	if(outputlevel & loadsimulation)
		std::cout << "    LoadState: ";

	StateMachineState* state = new StateMachineState();

	if(stateelement == 0)
		return state;

	const char* nam;
	nam = stateelement->Attribute("name");
	std::string statename = (nam != 0)?std::string(nam):"";
	static int statecounter = 0;
	if(statename == "")
	{
		std::stringstream s("");
		s << "State_" << statecounter++;
		statename = s.str();
	}

	state->SetStateName(statename);

	if(outputlevel & loadsimulation)
		std::cout << statename << std::endl;

	tinyxml2::XMLElement* child = stateelement->FirstChildElement();
	while(child != 0)
	{
		std::string value = std::string(child->Value());

		tinyxml2::XMLElement* newchild = child;
		if(value.compare("Scan") == 0)
		{
			newchild = ScanNode(child);
			value = std::string(newchild->Value());
		}

		if(value.compare("Action") == 0)
			state->AddRegisterChange(LoadRegisterChange(newchild));
		else if(value.compare("StateTransition") == 0)
		{
			state->AddStateTransition(LoadStateTransition(newchild));
		}

		if(child != stateelement->LastChildElement())
			child = child->NextSiblingElement();
		else
			child = 0;
	}

	return state;
}

RegisterAccess Simulator::LoadRegisterChange(tinyxml2::XMLElement* registerchange)
{
	if(outputlevel & loadsimulation)
		std::cout << "      LoadRegisterChange: ";

	RegisterAccess regacc;

	if(registerchange == 0)
		return regacc;

	const char* nam = registerchange->Attribute("what");
	regacc.what = (nam != 0)?std::string(nam):"";

	nam = registerchange->Attribute("parameter");
	regacc.parameter = (nam != 0)?std::string(nam):"";

	tinyxml2::XMLError error = registerchange->QueryDoubleAttribute("value", &regacc.value);
	if(error != tinyxml2::XML_NO_ERROR)
		regacc.value = 0;

	if(outputlevel & loadsimulation)
		std::cout << "\"" << regacc.what << "\"" << std::endl;

	return regacc;
}

StateTransition* Simulator::LoadStateTransition(tinyxml2::XMLElement* transition)
{
	if(outputlevel & loadsimulation)
		std::cout << "      LoadStateTransition to ";

	StateTransition* trans = new StateTransition();

	if(transition == 0)
		return trans;

	//name of the next state:
	const char* nam = transition->Attribute("nextstate");
	trans->SetNextState((nam != 0)?std::string(nam):"");

	if(outputlevel & loadsimulation)
		std::cout << trans->GetNextState() << std::endl;

	//transition delay:
	int delay;
	tinyxml2::XMLError error = transition->QueryIntAttribute("delay", &delay);
	if(error != tinyxml2::XML_NO_ERROR)
		delay = 0;
	trans->SetDelay(delay);

	tinyxml2::XMLElement* child = transition->FirstChildElement();
	while(child != 0)
	{
		std::string value = std::string(child->Value());

		tinyxml2::XMLElement* newchild = child;
		if(value.compare("Scan") == 0)
		{
			newchild = ScanNode(child);
			value = std::string(newchild->Value());
		}

		if(value.compare("Action") == 0)
			trans->AddRegisterChange(LoadRegisterChange(newchild));
		else if(value.compare("Condition") == 0)
			trans->SetComparison(LoadComparison(newchild));

		if(child != transition->LastChildElement())
			child = child->NextSiblingElement();
		else
			child = 0;
	}

	return trans;
}

Comparison* Simulator::LoadComparison(tinyxml2::XMLElement* comparison)
{
	if(outputlevel & loadsimulation)
		std::cout << "      LoadComparison" << std::endl;

	Comparison* comp = new Comparison();
	if(comparison == 0)
		return comp;

	const char* nam = comparison->Attribute("relation");
	std::string rel = (nam != 0)?std::string(nam):"";
	if(rel.compare("Smaller") == 0)
		comp->SetRelation(Comparison::Smaller);
	else if(rel.compare("SmallerEqual") == 0)
		comp->SetRelation(Comparison::SmallerEqual);
	else if(rel.compare("Larger") == 0)
		comp->SetRelation(Comparison::Larger);
	else if(rel.compare("LargerEqual") == 0)
		comp->SetRelation(Comparison::LargerEqual);
	else if(rel.compare("Equal") == 0)
		comp->SetRelation(Comparison::Equal);
	else if(rel.compare("NotEqual") == 0)
		comp->SetRelation(Comparison::NotEqual);
	else if(rel.compare("Or") == 0)
		comp->SetRelation(Comparison::Or);
	else if(rel.compare("And") == 0)
		comp->SetRelation(Comparison::And);
	else if(rel.compare("Xor") == 0)
		comp->SetRelation(Comparison::Xor);
	else
	{
		std::cout << "Error: Missing relation" << std::endl;
		comp->SetRelation(-1);
	}

	tinyxml2::XMLElement* child = comparison->FirstChildElement();
	while(child != 0)
	{
		std::string value = std::string(child->Value());

		tinyxml2::XMLElement* newchild = child;
		if(value.compare("Scan") == 0)
		{
			newchild = ScanNode(child);
			value = std::string(newchild->Value());
		}

		if(value.compare("Lvalue") == 0)
		{
			double lval;
			if(newchild->QueryDoubleAttribute("value", &lval) == tinyxml2::XML_NO_ERROR)
				comp->SetFirstValue(lval);
			else
			{
				tinyxml2::XMLElement* grandchild = newchild->FirstChildElement();
				if(std::string(grandchild->Value()).compare("Scan") == 0)
					grandchild = ScanNode(grandchild);
				if(grandchild != 0)
				{
					std::string val = std::string(grandchild->Value());
					if(val.compare("Action") == 0)
						comp->SetFirstRegisterAccess(LoadRegisterChange(grandchild));
					else if(val.compare("Condition") == 0)
						comp->SetFirstComparison(LoadComparison(grandchild));
					else
						std::cout << "Error: Missing LValue" << std::endl;
				}
			}
		}
		else if(value.compare("Rvalue") == 0)
		{
			double rval;
			if(child->QueryDoubleAttribute("value", &rval) == tinyxml2::XML_NO_ERROR)
				comp->SetSecondValue(rval);
			else
			{
				tinyxml2::XMLElement* grandchild = child->FirstChildElement();
				if(std::string(grandchild->Value()).compare("Scan") == 0)
					grandchild = ScanNode(grandchild);
				if(grandchild != 0)
				{
					std::string val = std::string(grandchild->Value());
					if(val.compare("Action") == 0)
						comp->SetSecondRegisterAccess(LoadRegisterChange(grandchild));
					else if(val.compare("Condition") == 0)
						comp->SetSecondComparison(LoadComparison(grandchild));
					else
						std::cout << "Error: Missing RValue" << std::endl;
				}
			}
		}


		if(child != comparison->LastChildElement())
			child = child->NextSiblingElement();
		else
			child = 0;
	}

	if(outputlevel & loadsimulation)
		std::cout << "comparisonLoader:" << std::endl << comp->PrintComparison(" ");

	return comp;
}

tinyxml2::XMLElement* Simulator::ScanNode(tinyxml2::XMLElement* element)
{
	if(element == 0)
		return 0;

	int scanid = 0;
	if(element->QueryIntAttribute("scanid", &scanid) != tinyxml2::XML_NO_ERROR)
	{
		std::cout << "Error: missing Scan parameter ID! Aborting." << std::endl;
		return 0;
	}

	//count the values for this parameter (`for(i=0;i<=maxindex;++i)`):
	tinyxml2::XMLElement* child = element->FirstChildElement();
	tinyxml2::XMLElement* object = 0;
	int maxindex = 0;
	while(child != 0)
	{
		std::string name = std::string(child->Value());

		tinyxml2::XMLElement* newchild = child;
		if(name.compare("Scan") == 0)
		{
			newchild = ScanNode(child);
			name = std::string(newchild->Value());
		}

		if(name.compare("Value") == 0)
			++maxindex;
		else if(name.compare("Object") == 0)
			object = newchild->FirstChildElement();
		
		if(child != element->LastChildElement())
			child = child->NextSiblingElement();
		else
			child = 0;
	}
	--maxindex;	//comparison is smaller-equal

	if(object == 0)
	{
		std::cout << "Error: missing object to modify for scan ID " << scanid << std::endl;
		return 0;
	}

	//check whether the scan ID is already in use:
	auto found = scanindexmaxima.find(scanid);
	if(found == scanindexmaxima.end())	//not found
	{
		//add a new scan index:
		scanindices.insert(std::make_pair(scanid, 0));
		scanindexmaxima.insert(std::make_pair(scanid,maxindex));
	}
	else if(found->second != maxindex)	//found, but wrong number of parameters:
	{
		std::cout << "Error: wrong number of parameters for scan ID " << scanid << std::endl;
		return 0;
	}

	//find the parameter and its value to replace:
	int index = 1;
	std::stringstream s("");
	s << "parameter" << index;
	char parnam[13];
	std::strcpy(parnam, s.str().c_str());

	while(element->Attribute(parnam) != 0)
	{
		std::string parameter = "";
		std::string parvalue = "";
		const char* nam = element->Attribute(parnam);
		parameter = ((nam != 0)?std::string(nam):"");
		
		int numvalue = 0;	//the number of the next value node
		child = element->FirstChildElement();
		while(child != 0)
		{
			tinyxml2::XMLElement* newchild = child;
			if(std::string(child->Value()).compare("Scan") == 0)
				newchild = ScanNode(child);

			if(std::string(newchild->Value()).compare("Value") == 0)
			{
				if(numvalue == scanindices.find(scanid)->second)
				{
					std::stringstream s2("");
					s2 << "val" << index;
					char valnam[7];
					std::strcpy(valnam, s2.str().c_str());
					nam = newchild->Attribute(valnam);
					parvalue = ((nam != 0)?std::string(nam):"");
					if(parvalue == "")
					{
						std::cout << "Missing parameter value for scan ID " << scanid << std::endl;
						return 0;
					}
					else
						break;
				}
				else
					++numvalue;
			}

			if(child != element->LastChildElement())
				child = child->NextSiblingElement();
			else
				child = 0;
		}

		//change the desired parameter to the new value:
		object->SetAttribute(parameter.c_str(), parvalue.c_str());

		++index;
		s.str("");
		s << "parameter" << index;
		std::strcpy(parnam, s.str().c_str());
	}

	if(std::string(object->Value()).compare("Scan") == 0)
		return ScanNode(object);
	else
		return object;
}

std::string Simulator::TimesToInterval(TimePoint start, TimePoint end)
{
	long duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

	std::stringstream timetext("");
	bool started = false;

	if(duration > 86400000)	//1 day
	{
		timetext << duration/86400000 << " days";
		duration %= 86400000;
		started = true;
	}
	if(duration > 3600000)	//1 hour
	{
		if(started)
			timetext << " ";
		else
			started = true;
		timetext << duration/3600000 << " hours";
		duration %= 3600000;
	}
	if(duration > 60000)	//1 minute
	{
		if(started)
			timetext << " ";
		else
			started = true;
		timetext << duration/60000 << " minutes";
		duration %= 60000;
	}
	if(duration > 1000)		// 1 second
	{
		if(started)
			timetext << " ";
		else
			started = true;
		timetext << duration/1000 << " seconds";
		duration %= 1000;
	}

	if(started)
		timetext << " ";
	if(duration != 0)
		timetext << duration << " milliseconds";

	if(!started && duration == 0)
		return "0 milliseconds";

	return timetext.str();
}
