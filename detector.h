#ifndef _DETECTOR
#define _DETECTOR

#include <string>
#include <vector>
#include <queue>
#include <fstream>

#include "hit.h"
#include "pixel.h"
#include "readoutcell.h"
#include "statemachine.h"
#include "TCoord.h"

class Detector
{
public:
	Detector();
	
	Detector(std::string addressname, int address);

	std::string GetAddressName();
	void		SetAddressName(std::string addressname);
	
	int 		GetAddress();
	void		SetAddress(int address);
	
	ReadoutCell* GetROC(int index);
	ReadoutCell* GetROCAddress(int address);
	void		AddROC(ReadoutCell readoutcell);
	void		ClearROCVector();
	std::vector<ReadoutCell>::iterator GetROCVectorBegin();
	std::vector<ReadoutCell>::iterator GetROCVectorEnd();
	
	void 		SaveHit(Hit hit, std::string filename, bool compact);
	
	TCoord<double>	GetPosition();
	void		SetPosition(TCoord<double> position);
	
	TCoord<double>	GetSize();
	void		SetSize(TCoord<double> size);
	
private:
	std::string addressname;
	int address;
	std::vector<ReadoutCell> rocvector;
	TCoord<double> position;
	TCoord<double> size;
	//StateMachine statemachine

};


#endif  //_DETECTOR