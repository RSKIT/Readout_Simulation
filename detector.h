#ifndef _DETECTOR
#define _DETECTOR

#include <string>
#include <vector>
#include <queue>
#include <fstream>

//#include "hit.h"
//#include "pixel.h"
//#include "readoutcell.h"
#include "detector_base.h"
//#include "TCoord.h"

class Detector : public DetectorBase
{
public:
    Detector(std::string addressname, int address);
	Detector();
	Detector(const Detector& templ);

    enum state {PullDown = 0, LdPix = 1, LdCol = 2, RdCol = 3};
	
    bool	StateMachineCkUp(int timestamp, bool trigger = true);
    bool	StateMachineCkDown(int timestamp, bool trigger = true);

    int 	GetState();
    int 	GetNextState();
    std::string GetCurrentStateName();

    DetectorBase* Clone();
    
    int GetNumStates();
private:
	int currentstate;
	int nextstate;
	int delay;
};


#endif  //_DETECTOR
