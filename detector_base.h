#ifndef _DETECTORBASE
#define _DETECTORBASE

#include <string>
#include <vector>
#include <queue>
#include <fstream>

#include "hit.h"
#include "pixel.h"
#include "readoutcell.h"
#include "TCoord.h"

class DetectorBase
{
public:
    DetectorBase(std::string addressname, int address);
	DetectorBase();
	

    enum state {PullDown = 0, LdPix = 1, LdCol = 2, RdCol = 3};

    std::string GetAddressName();
	void		SetAddressName(std::string addressname);
	
	int 		GetAddress();
	void		SetAddress(int address);
	
    ReadoutCell*    GetROC(int index);
    ReadoutCell*    GetROCAddress(int address);
	void		AddROC(ReadoutCell readoutcell);
	void		ClearROCVector();
	std::vector<ReadoutCell>::iterator GetROCVectorBegin();
	std::vector<ReadoutCell>::iterator GetROCVectorEnd();
	
	void 		SaveHit(Hit hit, std::string filename, bool compact);
	
	TCoord<double>	GetPosition();
	void		SetPosition(TCoord<double> position);
	
	TCoord<double>	GetSize();
	void		SetSize(TCoord<double> size);

    bool        SizeOK();
    bool        EnlargeSize();
	
    void        StateMachine();

    void        SetState(int nextstate);
    int 		GetState();
    int 		NextState();

    bool            PlaceHit(Hit hit, double deadtimeend = 0);

    std::string PrintDetector();
    

protected:
	std::string addressname;
	int address;
	std::vector<ReadoutCell> rocvector;
	TCoord<double> position;
    TCoord<double> size;

    int currentstate;
    bool    SizeOKROC(ReadoutCell* cell);
    bool EnlargeSizeROC(ReadoutCell* cell);



};


#endif  //_DETECTORBASE
