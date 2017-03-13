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
	ReadoutCell(std::string addressname, int address, int hitqueuelength);	
	
	std::string GetAddressName();
	void		SetAddressName(std::string addressname);
	
	int 		GetAddress();
	void		SetAddress(int address);
	
	bool		GetHitflag();
	void		SetHitflag(bool hitflag);
	
	int			GetHitqueuelength();
	void		SetHitqueuelength(int hitqueuelength);
	
	bool		AddHit(Hit hit);
	Hit 		GetHit();
	bool		PopHit();

	Pixel* 		GetPixel(int index);
	Pixel*		GetPixelAddress(int address);
	void		AddPixel(Pixel pixel);
	void		ClearPixelVector();
	int         GetNumPixels();
	std::vector<Pixel>::iterator GetPixelsBegin();
	std::vector<Pixel>::iterator GetPixelsEnd();
	
	ReadoutCell* GetROC(int index);
	ReadoutCell* GetROCAddress(int address);
	void		AddROC(ReadoutCell readoutcell);
	void		ClearROCVector();
	int         GetNumROCs();
	std::vector<ReadoutCell>::iterator GetROCsBegin();
	std::vector<ReadoutCell>::iterator GetROCsEnd();

	
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