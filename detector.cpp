#include "detector.h"

Detector::Detector() : 
	addressname(""), address(0), rocvector(std::vector<ReadoutCell>())
{
	
}

De

std::string GetAddressName()
void		SetAddressName(std::string addressname)
	
int 		GetAddress()
void		SetAddress(int address)
	
ReadoutCell* GetROC(int index)
ReadoutCell* GetROCAddress(int address)
void		AddROC(ReadoutCell readoutcell)
void		ClearROCVector()
	
bool 		SaveHit(Hit hit)