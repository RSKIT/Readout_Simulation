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

	Detector* GetDetector(int address);
	void AddDetector(Detector& detector);
	void ClearDetectors();
	int GetNumDetectors();

	EventGenerator* GetEventGenerator();
	void InitEventGenerator();

    void ClockUp();
    void ClockDown();
    void Loader();

    void SimulateUntil(int stoptime = -1);

    std::string PrintDetectors();

private:
	void LoadDetector(tinyxml2::XMLElement* parent, TCoord<double> pixelsize);
	TCoord<double> LoadTCoord(tinyxml2::XMLElement* coordinate);
	void LoadEventGenerator(tinyxml2::XMLElement* eventgen);
	ReadoutCell LoadROC(tinyxml2::XMLElement* parent, TCoord<double> pixelsize, 
							std::string defaultaddressname = "ROC");
	Pixel LoadPixel(tinyxml2::XMLElement* parent, TCoord<double> pixelsize);

    std::vector<Detector> detectors;
    EventGenerator eventgenerator;

    std::string inputfile;
    std::string outputfile;
	
};


#endif  //_SIMULATOR
