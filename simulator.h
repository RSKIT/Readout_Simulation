#ifndef _SIMULATOR
#define _SIMULATOR

#include <vector>

#include "detector.h"
#include "EventGenerator.h"
#include "tinyxml2.h"
#include "tinyxml2_addon.h"

class Simulator
{
public:
	Simulator();
	Simulator(std::string filename);

	std::string GetFileName();
	void SetFileName(std::string filename);

	Detector* GetDetector(int address);
	void AddDetector(Detector& detector);
	void ClearDetectors();

	EventGenerator* GetEventGenerator();
	void InitEventGenerator();

    void ClockUp();
    void ClockDown();
    void Loader();

    void SimulateUntil(int stoptime = -1);

private:
    std::vector<Detector> detectors;
    EventGenerator eventgenerator;

    std::string inputfile;
	
};


#endif  //_SIMULATOR
