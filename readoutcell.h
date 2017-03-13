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
        ReadoutCell(std::string addressname, int address, int hitqueuelength);
	ReadoutCell();

	
	std::string GetAddressName();
	void		SetAddressName(std::string addressname);
	
	int 		GetAddress();
	void		SetAddress(int address);
	
	bool		GetHitflag();
	void		SetHitflag(bool hitflag);
	
        int		GetHitqueuelength();
	void		SetHitqueuelength(int hitqueuelength);
	
	bool		AddHit(Hit hit);
	Hit 		GetHit();
	bool		PopHit();

	Pixel* 		GetPixel(int index);
	Pixel*		GetPixelAddress(int address);
	void		AddPixel(Pixel pixel);
	void		ClearPixelVector();
	
	ReadoutCell* GetROC(int index);
	ReadoutCell* GetROCAddress(int address);
	void		AddROC(ReadoutCell readoutcell);
	void		ClearROCVector();

        bool		GetNextHitflag();
        void		SetNextHitflag(bool nexthitflag);

        Hit             GetNextHit();
        void            SetNextHit(Hit nexthit);

        void            Apply();

	
private:
	std::string addressname;
	int address;
	bool hitflag;
	int hitqueuelength;
	std::queue<Hit> hitqueue;
	std::vector<Pixel> pixelvector;
	std::vector<ReadoutCell> rocvector;
        Hit nexthit;
        bool nexthitflag;

};


#endif  //_READOUTCELL
