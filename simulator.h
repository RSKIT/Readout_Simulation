#ifndef _SIMULATOR
#define _SIMULATOR

#include <vector>

#include "detector.h"
#include "EventGenerator.h"
#include "tinyxml2.h"
#include "tinyxml2_addon.h"
#include "TCoord.h"

class Simulator
{
public:
	Simulator();
	Simulator(std::string filename);

	std::string GetLoadFileName();
	void LoadInputFile(std::string filename = "");

	std::string GetSaveFileName();
	void SetLoadFileName(std::string filename);

	int GetNumEventsToGenerate();
	void SetNumEventsToGenerate(int events);

	int GetStartTime();
	void SetStartTime(int starttime);

	int GetStopTime();
	void SetStopTime(int stoptime);
	void RemoveStopTime();

	int GetStopDelay();
	void SetStopDelay(int stopdelay);

	Detector* GetDetector(int address);
	void AddDetector(Detector& detector);
	void ClearDetectors();
	int GetNumDetectors();

	EventGenerator* GetEventGenerator();
	void InitEventGenerator();
	void GenerateEvents(int events, double starttime = -1);

    void ClockUp();
    void ClockDown();

    void SimulateUntil(int stoptime = -1, int delaystop = 0);

    std::string PrintDetectors();

private:
	void LoadDetector(tinyxml2::XMLElement* parent, TCoord<double> pixelsize);
	TCoord<double> LoadTCoord(tinyxml2::XMLElement* coordinate);
	void LoadEventGenerator(tinyxml2::XMLElement* eventgen);
	ReadoutCell LoadROC(tinyxml2::XMLElement* parent, TCoord<double> pixelsize, 
							std::string defaultaddressname = "ROC");
	Pixel LoadPixel(tinyxml2::XMLElement* parent, TCoord<double> pixelsize);
	void LoadNPixels(ReadoutCell* parentcell, tinyxml2::XMLElement* parentnode, 
						TCoord<double> pixelsize);

    std::vector<Detector> detectors;
    EventGenerator eventgenerator;
    //event generation parameters for the event generator:
    int events;			//number of events
    double starttime;	//earliest time possible for the first event

    //end conditions for the simulation:
    int stoptime;
    int stopdelay;

    std::string inputfile;
    std::string outputfile;
	
};


#endif  //_SIMULATOR
