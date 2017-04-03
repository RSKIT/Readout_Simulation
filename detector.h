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

    enum state {PullDown = 0, LdPix = 1, LdCol = 2, RdCol = 3};
	
    void	StateMachine();

    int 	NextState();
    
private:

};


#endif  //_DETECTOR
