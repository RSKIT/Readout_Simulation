#include "simulator.h"
Simulator::Simulator()
{

}

Simulator::Simulator(std::string filename)
{

}

std::string Simulator::GetLoadFileName()
{
	return inputfile;
}

void Simulator::LoadInputFile(std::string filename)
{
	/*
	inputfile = filename;

	tinyxml2::XMLDocument doc;
	doc.LoadFile(inputfile);

	tinyxml2::XMLElement* elem = doc.GetFirstChild();
	while(elem != 0)
	{
		if(elem)//TODO: not even really started...

		if(elem != doc.GetLastChild())
			elem = elem.NextSibling();
		else
			elem = 0;
	}*/
}

std::string Simulator::GetSaveFileName()
{

}

void Simulator::SetLoadFileName(std::string filename)
{

}

Detector* Simulator::GetDetector(int address)
{

}
	
void Simulator::AddDetector(Detector& detector)
{

}

void Simulator::ClearDetectors()
{

}

EventGenerator* Simulator::GetEventGenerator()
{

}

void Simulator::InitEventGenerator()
{

}

void Simulator::ClockUp()
{

}

void Simulator::ClockDown()
{

}

void Simulator::Loader()
{

}

void Simulator::SimulateUntil(int stoptime)
{

}
