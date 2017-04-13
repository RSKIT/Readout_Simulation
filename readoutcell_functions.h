#ifndef _ROCFUNCTIONS
#define _ROCFUNCTIONS

#include <vector>
#include <fstream>

#include "hit.h"

class ReadoutCell;

//---- Readout Order Classes ----

class ROCBuffer
{
public:
	ROCBuffer(ReadoutCell* roc);

	bool 	InsertHit(const Hit& hit);
	Hit 	GetHit();

	bool 	is_full();
	int 	GetNumHitsEnqueued();
protected:
	ReadoutCell* cell;
};


class FIFOBuffer : public ROCBuffer
{
public:
	FIFOBuffer(ReadoutCell* roc);

	bool 	InsertHit(const Hit& hit);
	Hit 	GetHit();

	int 	GetNumHitsEnqueued();	
private:
};


class PrioBuffer : public ROCBuffer
{
public:
	PrioBuffer(ReadoutCell* roc);

	bool 	InsertHit(const Hit& hit);
	Hit 	GetHit();

	bool 	is_full();
	int 	GetNumHitsEnqueued();
private:
};

//---- End Readout Order Classes ----

//---- ROC Readout Classes ----

class ROCReadout
{
public:
	ROCReadout(ReadoutCell* roc);

	/**
	 * @brief [brief description]
	 * @details [long description]
	 * 
	 * @param timestamp [description]
	 * @param out 			- file for "lost" signals
	 * @return [description]
	 */
	bool Read(int timestamp, std::fstream* out = 0);
protected:
	ReadoutCell* cell;
};

//do not read when the buffer is still full
class NoFullReadReadout : public ROCReadout
{
public:
	NoFullReadReadout(ReadoutCell* roc);

	bool Read(int timestamp, std::fstream* out = 0);
};

//read and delete hits if the own buffer is full, but do not overwrite the own data
class NoOverWriteReadout : public ROCReadout
{
public:
	NoOverWriteReadout(ReadoutCell* roc);

	bool Read(int timestamp, std::fstream* out = 0);
};

//read and delete child data and overwrite own data with new data
class OverWriteReadout : public ROCReadout
{
public:
	OverWriteReadout(ReadoutCell* roc);

	bool Read(int timestamp, std::fstream* out = 0);
};

//---- End ROC Readout Classes ----

//---- Pixel Readout Classes ----

class PixelReadout
{
public:
	PixelReadout(ReadoutCell* roc);

	bool Read(int timestamp, std::fstream* out = 0);
protected:
	ReadoutCell* cell;
};

class PPtBReadout : public PixelReadout
{
public:
	PPtBReadout(ReadoutCell* roc);

	bool Read(int timestamp, std::fstream* out = 0);
};

//---- End Pixel Readout Classes ----

#endif //_ROCFUNCTIONS