#ifndef _SIMULATOR
#define _SIMULATOR

#include <vector>
#include <chrono>

#include "detector.h"
#include "xmldetector.h"
#include "EventGenerator.h"
#include "tinyxml2.h"
#include "tinyxml2_addon.h"
#include "TCoord.h"

class Simulator
{
public:
	typedef std::chrono::steady_clock::time_point TimePoint;

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

	DetectorBase* GetDetector(int address);
	void AddDetector(DetectorBase* detector);
	void ClearDetectors();
	int GetNumDetectors();

	EventGenerator* GetEventGenerator();
	void InitEventGenerator();
	void GenerateEvents(int events, double starttime = -1);

    bool ClockUp(int timestamp);
    bool ClockDown(int timestamp);

    void SimulateUntil(int stoptime = -1, int delaystop = 0);

    std::string PrintDetectors();

private:
	void 			LoadDetector(tinyxml2::XMLElement* parent, TCoord<double> pixelsize);
	TCoord<double> 	LoadTCoord(tinyxml2::XMLElement* coordinate);
	void 			LoadEventGenerator(tinyxml2::XMLElement* eventgen);
	ReadoutCell 	LoadROC(tinyxml2::XMLElement* parent, TCoord<double> pixelsize, 
								std::string defaultaddressname = "ROC");
	Pixel 			LoadPixel(tinyxml2::XMLElement* parent, TCoord<double> pixelsize);
	void 			LoadNPixels(ReadoutCell* parentcell, tinyxml2::XMLElement* parentnode, 
						TCoord<double> pixelsize);

	//Statemachine-Loading:
	XMLDetector* 	LoadStateMachine(DetectorBase* detector, tinyxml2::XMLElement* statemachine);
	StateMachineState 	LoadState(tinyxml2::XMLElement* stateelement);
	RegisterAccess 		LoadRegisterChange(tinyxml2::XMLElement* registerchange);
	StateTransition 	LoadStateTransition(tinyxml2::XMLElement* transition);
	Comparison 			LoadComparison(tinyxml2::XMLElement* comparison);

	std::string 		TimesToInterval(TimePoint start, TimePoint end);

    std::vector<DetectorBase*> detectors;
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
