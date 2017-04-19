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
	
    void	StateMachineCkUp(int timestamp);
    void	StateMachineCkDown(int timestamp);

    int 	GetState();
    int 	GetNextState();
    std::string GetCurrentStateName();

    DetectorBase* Clone();
    
private:
	int currentstate;
	int nextstate;
	int delay;
};


#endif  //_DETECTOR
