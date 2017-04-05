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
    ReadoutCell(std::string addressname, int address, int hitqueuelength, bool pptb=false);
	ReadoutCell();

	
    std::string GetAddressName();
	void		SetAddressName(std::string addressname);
	
	int 		GetAddress();
	void		SetAddress(int address);
	
	bool		GetHitflag();
	void		SetHitflag(bool hitflag);
	
    int			GetHitqueuelength();
	void		SetHitqueuelength(int hitqueuelength);

	bool 		GetPPtBState();
	void 		SetPPtBState(bool pptb);
	
	bool		AddHit(Hit hit, int timestamp = -1);
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

    bool		GetNextHitflag();
    void		SetNextHitflag(bool nexthitflag);

    Hit         GetNextHit();
    void        SetNextHit(Hit nexthit, int timestamp = -1);

    void        Apply();

    bool        PlaceHit(Hit hit);
    bool        LdPix();
    bool        LdCol();
    Hit         RdCol();

    bool 		LoadPixelFlag(int timestamp, std::fstream* out = 0);
    bool 		LoadPixel(int timestamp, std::fstream* out = 0);
    bool 		LoadCell(std::string addressname, int timestamp, std::fstream* out = 0);
    Hit 		ReadCell();

    std::string PrintROC(std::string space);

    void ShiftCell(TCoord<double> distance);
	
private:
	std::string addressname;
	int address;
	bool hitflag;
	int hitqueuelength;
	std::queue<Hit> hitqueue;
	std::vector<Pixel> pixelvector;
	std::vector<ReadoutCell> rocvector;
    //structure to apply new hits synchronuous in all ROCs:
    Hit nexthit;
    bool nexthitflag;
    int nexthitTS;

    bool ispptb;

};


#endif  //_READOUTCELL
