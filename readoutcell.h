#ifndef _READOUTCELL
#define _READOUTCELL

#include <string>
#include <vector>
#include <queue>

#include "hit.h"
#include "pixel.h"

class ReadoutCell
{
public:
	ReadoutCell();
	ReadoutCell(std::string addressname, int address, std::vector<Pixel> pixelvector, std::vector<ReadoutCell> rocvector);	
	
	std::string GetAddressName();
	void		SetAddressName(std::string addressname);
	
	int 		GetAddress();
	void		SetAddress(int address);
	
	bool		GetHitflag();
	void		SetHitflag(bool hitflag);
	
	int			GetHitqueuelength();
	void		SetHitqueuelength(int hitqueuelength);
	
	bool		AddHit(Hit hit);

	Pixel& 		GetPixel(int index);
	Pixel&		GetPixelAddress(int address);
	void		AddPixel(Pixel pixel);
	void		ClearPixelVector();
	
	ReadoutCell& GetROC(int index);
	ReadoutCell& GetROCAddress(int address);
	void		AddROC(ReadoutCell readoutcell);
	void		ClearROCVector();

	
private:
	std::string addressname;
	int address;
	bool hitflag;
	int hitqueuelength;
	std::queue<Hit> hitqueue;
	std::vector<Pixel> pixelvector;
	std::vector<ReadoutCell> rocvector;

};


#endif  //_READOUTCELL