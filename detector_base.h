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
	DetectorBase(const DetectorBase& templ);
	

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
	
	
	TCoord<double>	GetPosition();
	void		SetPosition(TCoord<double> position);
	
	TCoord<double>	GetSize();
	void		SetSize(TCoord<double> size);

    bool        SizeOK();
    bool        EnlargeSize();
	
    void        StateMachineCkUp();
    void 		StateMachineCkDown();
    void		StateMachine();

    void        SetState(int nextstate);
    int 		GetState();
    int 		NextState();

    bool        PlaceHit(Hit hit);
	void 		SaveHit(Hit hit, std::string filename, bool compact);
	bool		SaveHit(Hit hit, bool compact = false);
	std::string	GetOutputFile();
	void 		SetOutputFile(std::string filename);
	void 		CloseOutputFile();

	int 		GetHitCounter();
	void		ResetHitCounter();

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

    int hitcounter;
    std::string outputfile;
    std::fstream fout;

};


#endif  //_DETECTORBASE
