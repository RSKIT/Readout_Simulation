/*
    ROME (ReadOut Modelling Environment)
    Copyright © 2017  Rudolf Schimassek (rudolf.schimassek@kit.edu),
                      Felix Ehrler (felix.ehrler@kit.edu),
                      Karlsruhe Institute of Technology (KIT)
                                - ASIC and Detector Laboratory (ADL)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3 as 
    published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    This file is part of the ROME simulation framework.
*/

#ifndef _ROCFUNCTIONS
#define _ROCFUNCTIONS

#include <vector>
#include <fstream>
#include <iostream>

#include "hit.h"

class ReadoutCell;

//---- Readout Order Classes ----

/**
 * generic class for readout from the buffer of a readoutcell
 */
class ROCBuffer
{
public:
	ROCBuffer(ReadoutCell* roc);

	/**
	 * @brief put a hit object inside the buffer structure of the readoutcell `cell`
	 * @details
	 * 
	 * @param hit            - the hit to place in the readoutcell
	 * @return               - true if the hit was placed, false if not
	 */
	virtual bool 	InsertHit(const Hit& hit);
	/**
	 * @brief reads one hit from the readoutcell to which this object belongs
	 * @details
	 * 
	 * @param timestamp      - the timestamp at which the readout is to occur
	 * @param remove         - removes the hit from the readoutcell if true, or not on false
	 * 
	 * @return               - the first hit to be read out in the readoutcell or an invalid hit
	 *                            if no hit is present or no hit is available for readout (readout
	 *                            delay)
	 */
	virtual Hit 	GetHit(int timestamp, bool remove = true);

	/**
	 * @brief to remove hits that require a trigger signal. Only call when trigger signal is low
	 * @details
	 * 
	 * @param timestamp      - current timestamp to check whether a hit is expecting a trigger 
	 *                            signal "now"
	 * @param fbadout        - output file opened to log deleted hits due to missing trigger
	 * 
	 * @return               - true if a hit was removed, false if not
	 */
	virtual bool  	NoTriggerRemoveHits(int timestamp, std::fstream* fbadout);

	/**
	 * @brief checks whether all buffers are occupied
	 * @details
	 * @return               - true if all buffers in the readoutcell are occupied, false otherwise
	 */
	virtual bool 	is_full();
	/**
	 * @brief counts the number of hits enqueued in the readoutcell
	 * @details
	 * @return               -the number of hits in the readoutcell
	 */
	virtual int 	GetNumHitsEnqueued();
protected:
	ReadoutCell* cell;
};


/**
 * @brief Actual implementation of the Reading and Writing class. The hits are treated like in a 
 *                 FIFO. The first hit put inside is read out first.
 */
class FIFOBuffer : public ROCBuffer
{
public:
	FIFOBuffer(ReadoutCell* roc);

	bool 	InsertHit(const Hit& hit);
	Hit 	GetHit(int timestamp, bool remove = true);

	bool  	NoTriggerRemoveHits(int timestamp, std::fstream* fbadout);

	bool 	is_full();
	int 	GetNumHitsEnqueued();	
private:
};


/**
 * @brief Second implementation of the reading and writing class. This class implements a prio
 *                 chain readout. The hits are placed in a buffer and stay there until they are
 *                 deleted. When reading, the first valid hit is returned. When writing, the first
 *                 unoccupied buffer is filled.
 */
class PrioBuffer : public ROCBuffer
{
public:
	PrioBuffer(ReadoutCell* roc);

	bool 	InsertHit(const Hit& hit);
	Hit 	GetHit(int timestamp, bool remove = true);

	bool  	NoTriggerRemoveHits(int timestamp, std::fstream* fbadout);

	bool 	is_full();
	int 	GetNumHitsEnqueued();
private:
};

//---- End Readout Order Classes ----

//---- ROC Readout Classes ----

/**
 * @brief This class describes how hit data is read from subordinate readout cells.
 */
class ROCReadout
{
public:
	ROCReadout(ReadoutCell* roc);

	/**
	 * @brief transfers hit objects from subordinate readoutcells to the readoutcell this object
	 *             belongs to.
	 * @details
	 * 
	 * @param timestamp      - current timestamp at which this action is to occur
	 * @param out 			 - output file opened for logging the hits that get lost in this very
	 *                            process
	 * @return 				 - true if at least one hit was transferred, false if not
	 */
	virtual bool Read(int timestamp, std::fstream* out = 0);
	/**
	 * @brief flag that indicates whether the hit exists as a copy in a different object and has to
	 *             be deleted at several positions. This is necessary for the ROCBuffer classes
	 * @details
	 * @return               - true if the hit is also to be deleted in a subordinate readoutcell,
	 * 							  false if not
	 */
	virtual bool ClearChild();
protected:
	ReadoutCell* cell;
};

/**
 * @brief implementation of the ROCReadout class. When all buffers of the readoutcell are full,
 *                 no read operation is executed
 */
class NoFullReadReadout : public ROCReadout
{
public:
	NoFullReadReadout(ReadoutCell* roc);

	bool Read(int timestamp, std::fstream* out = 0);
	bool ClearChild();
};

/**
 * @brief implementation of the ROCReadout class. Hits are always read from subordinate 
 *                 readoutcells, but when no buffer is free, the read hit is lost.
 */
class NoOverWriteReadout : public ROCReadout
{
public:
	NoOverWriteReadout(ReadoutCell* roc);

	bool Read(int timestamp, std::fstream* out = 0);
	bool ClearChild();
};

/**
 * @brief implementation of the ROCReadout class. Hit data is always read from subordinate
 *                 readoutcells and if no buffer is free in the readoutcell this object belongs to,
 *                 the oldest hit is deleted to free space for the new hit.
 */
class OverWriteReadout : public ROCReadout
{
public:
	OverWriteReadout(ReadoutCell* roc);

	bool Read(int timestamp, std::fstream* out = 0);
	bool ClearChild();
};

/**
 * @brief implementation of the ROCReadout class. This special readout scheme implements a 1-by-1
 *                 connection between 2 readoutcells of the same bufferlength (it will throw an
 *                 error if they do not match). A hit in the i-th buffer of the subordinate 
 *                 readoutcell will be transferred to the i-th buffer of the superordinate
 *                 readoutcell. The hits are cleared in both readoutcells when they are read out
 *                 from the superordinate one. Apart from that, the readout is the same as for the
 *                 NoFullReadReadout.
 */
class OneByOneReadout : public ROCReadout
{
public:
	OneByOneReadout(ReadoutCell* roc);

	bool Read(int timestamp, std::fstream* out = 0);
	bool ClearChild();
};

//---- End ROC Readout Classes ----

//---- Pixel Readout Classes ----

/**
 * @brief This class describes the readout from pixels connected to the readoutcell this object
 *                 belongs to.
 */
class PixelReadout
{
public:
	PixelReadout(ReadoutCell* roc);

	/**
	 * @brief read the hit information from the pixels to the readoutcell
	 * @details
	 * 
	 * @param timestamp      - current timestamp at which the action is to occur
	 * @param out            - output file opened for logging the lost hits
	 * 
	 * @return               - true if a hit was loaded, false if not
	 */
	virtual bool Read(int timestamp, std::fstream* out = 0);
protected:
	ReadoutCell* cell;
};

/**
 * @brief Implementation of the PixelReadout class. The addresses of the pixels in the readoutcell
 *                 are or-chained and read as one hit to the readoutcell.
 */
class PPtBReadout : public PixelReadout
{
public:
	PPtBReadout(ReadoutCell* roc);

	bool Read(int timestamp, std::fstream* out = 0);
};

//---- End Pixel Readout Classes ----

#endif //_ROCFUNCTIONS