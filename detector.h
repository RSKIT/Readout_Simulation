#ifndef _DETECTOR
#define _DETECTOR

#include <string>
#include <vector>
#include <queue>

#include "hit.h"
#include "pixel.h"
#include "readoutcell.h"
#include "statemachine.h"

class Detector
{
public:
	Detector();
	
	Detector(std::string addressname, int address)

	std::string GetAddressName();
	void		SetAddressName(std::string addressname);
	
	int 		GetAddress();
	void		SetAddress(int address);
	
	ReadoutCell* GetROC(int index);
	ReadoutCell* GetROCAddress(int address);
	void		AddROC(ReadoutCell readoutcell);
	void		ClearROCVector();
	
	bool 		SaveHit(Hit hit);
	
private:
	std::string addressname;
	int address;
	std::vector<ReadoutCell> rocvector;
	//StateMachine statemachine

};


#endif  //_DETECTOR