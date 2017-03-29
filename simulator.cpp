#include "simulator.h"
Simulator::Simulator()
{

}

Simulator::Simulator(std::string filename)
{

}

std::string Simulator::GetFileName()
{

}

void Simulator::SetFileName(std::string filename)
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
