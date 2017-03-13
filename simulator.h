#ifndef _SIMULATOR
#define _SIMULATOR

#include <vector>

#include "detector.h"
//#include "eventgenerator.h"

class Simulator
{
public:
    void ClockUp();
    void ClockDown();
    void Loader();
private:
    std::vector<Detector> detectors;
    //Eventgenerator eventgenerator;
	
};


#endif  //_SIMULATOR
