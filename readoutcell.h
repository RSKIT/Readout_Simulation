#ifndef _READOUTCELL
#define _READOUTCELL

#include <string>
#include <vector>
//#include <queue>

#include "hit.h"
#include "pixel.h"
#include "readoutcell_functions.h"

class ReadoutCell
{
	//classes for strategy design pattern:
	friend class ROCBuffer;
	friend class FIFOBuffer;
	friend class PrioBuffer;

	friend class ROCReadout;
	friend class NoFullReadReadout;
	friend class NoOverWriteReadout;
	friend class OverWriteReadout;

	friend class PixelReadout;
	friend class PPtBReadout;

public:
	enum config {PPTB 				=  1,
				 ZEROSUPPRESSION	=  2,
				 FIFOBUFFER 		=  4,
				 PRIOBUFFER     	=  8,
				 NOREADONFULL 		= 16,
				 NOOVERWRITE 		= 32,
				 OVERWRITEONFULL 	= 64};

	//TODO: change the constructors to include the different behaviours
	//			also include Getter/Setter functions
    ReadoutCell(std::string addressname, int address, int hitqueuelength,
  				int configuration = PPTB | ZEROSUPPRESSION | FIFOBUFFER | NOREADONFULL);
	ReadoutCell();

	int 		GetConfiguration();
	void 		SetConfiguration(int newconfig);
	
    std::string GetAddressName();
	void		SetAddressName(std::string addressname);
	
	int 		GetAddress();
	void		SetAddress(int address);
	
    int			GetHitqueuelength();
	void		SetHitqueuelength(int hitqueuelength);
	
	bool		AddHit(Hit hit, int timestamp = -1);
	Hit 		GetHit();
	int 		GetEnqueuedHits();

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

    bool        PlaceHit(Hit hit);

    bool 		LoadPixel(int timestamp, std::fstream* out = 0);
    bool 		LoadCell(std::string addressname, int timestamp, std::fstream* out = 0);
    Hit 		ReadCell();
    int 		HitsAvailable(std::string addressname);

    std::string PrintROC(std::string space);

    void ShiftCell(TCoord<double> distance);
	
private:
	std::string 				addressname;
	int 						address;
	bool 						hitflag;
	int 						hitqueuelength;
	std::vector<Hit> 			hitqueue;
	std::vector<Pixel> 			pixelvector;
	std::vector<ReadoutCell> 	rocvector;

	//function objects to change the behaviour of the Readout Cell:
	ROCBuffer 		buf;			//readint/writing to the buffer
	ROCReadout 		rocreadout;		//reading from the child ROCs
	PixelReadout 	pixelreadout;	//reading from the pixels
	bool 			zerosuppression;

	int 			configuration;	//to save the readout settings according to the config enum

};


#endif  //_READOUTCELL
