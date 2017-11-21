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
#include <sstream>
#include <iostream>
#include <algorithm>
#include <deque>

#include "hit.h"

class ReadoutCell;	//solve circular dependency with ReadoutCell class


//---- Readout Order Classes ----

/**
 * generic class for readout from the buffer of a readoutcell
 */
class ROCBuffer
{
public:
	ROCBuffer(ReadoutCell* roc);
	~ROCBuffer();

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
	 * @param sbadout        - string collecting the data to write to the output file containing
	 *                            the lost hits due to missing trigger
	 * 
	 * @return               - true if a hit was removed, false if not
	 */
	virtual bool  	NoTriggerRemoveHits(int timestamp, std::string* sbadout = 0);

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

	bool  	NoTriggerRemoveHits(int timestamp, std::string* sbadout = 0);

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

	bool  	NoTriggerRemoveHits(int timestamp, std::string* sbadout = 0);

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
	~ROCReadout();

	/**
	 * @brief transfers hit objects from subordinate readoutcells to the readoutcell this object
	 *             belongs to.
	 * @details
	 * 
	 * @param timestamp      - current timestamp at which this action is to occur
	 * @param out 			 - output string for logging the hits that get lost in this very
	 *                            process to collect for writing to a file
	 * @return 				 - true if at least one hit was transferred, false if not
	 */
	virtual bool Read(int timestamp, std::string* out = 0);
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

	bool Read(int timestamp, std::string* out = 0);
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

	bool Read(int timestamp, std::string* out = 0);
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

	bool Read(int timestamp, std::string* out = 0);
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

	bool Read(int timestamp, std::string* out = 0);
	bool ClearChild();
};

/**
 * @brief implementation of the ROCReadout class. This readout scheme does not use a priority chain
 *                 as the ither readout schemes. Instead, a token is passed around to find the next
 *                 hit. The major difference is, that now for example EoCs can be loaded again
 *                 before all have been read out. This implies that the readout scheme is like for
 *                 the NoFullReadReadout.
 */
class TokenReadout :public ROCReadout
{
public:
	TokenReadout(ReadoutCell* roc);

	bool Read(int timestamp, std::string* out = 0);
	bool ClearChild();
private:
	int currentindex;
};


class SortedROCReadout : public ROCReadout
{
public:
	SortedROCReadout(ReadoutCell* roc);
	~SortedROCReadout();

	bool Read(int timestamp, std::string* out = 0);
	bool ClearChild();

	/**
	 * @brief sets the pointer to the time stamp which is to be read out at the moment
	 * @details
	 * 
	 * @param front          - pointer to the element presented by the trigger table
	 */
	void SetTriggerTableFrontPointer(const int* front);
	/**
	 * @brief sets the bits of the timestamp which should be not considered for the comparison
	 * @details
	 * 
	 * @param clearpattern   - the bit pattern which are to be cleared before comparison with
	 *                            the trigger timestamp
	 */
	void SetTriggerPattern(int clearpattern);
private:
	const int* triggertablefront;
	int pattern;

};

class MergingReadout : public ROCReadout
{
public:
	MergingReadout(ReadoutCell* roc);
	~MergingReadout();

	bool Read(int timestamp, std::string* out = 0);
	bool ClearChild();

	std::string GetMergingAddressName();
	void SetMergingAddressName(std::string addressname);
private:
	std::string mergingaddress;
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
	~PixelReadout();

	/**
	 * @brief read the hit information from the pixels to the readoutcell
	 * @details
	 * 
	 * @param timestamp      - current timestamp at which the action is to occur
	 * @param out            - output string for logging the lost hits into a file
	 * 
	 * @return               - true if a hit was loaded, false if not
	 */
	virtual bool Read(int timestamp, std::string* out = 0);
	/**
	 * @brief determines whether the function has to be set manually
	 * @details
	 * @return               - if this function returns true, the PPtB readout object has to be
	 *                            copied separately
	 */
	virtual bool NeedsROCReset();
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

	bool Read(int timestamp, std::string* out = 0);
};

/**
 * @brief Implementation of the PixelReadout class. The addresses or the pixels are or-chained and
 *                 are read when the result of the OR shows a rising edge. If the OR-result already
 *                 was high, the new hits are "lost"
 */
class PPtBReadoutOrBeforeEdge : public PixelReadout
{
public:
	PPtBReadoutOrBeforeEdge(ReadoutCell* roc);

	bool Read(int timestamp, std::string* out = 0);
};

/**
 * @brief implementation of a runtime depiction of combinatory logic between pixel hits. It
 *                 provides information whether a hit occured and the address resulting from the
 *                 hite
 * @details
 */
class PixelLogic
{
public:
	/**
	 * @brief constructor for a logic element
	 * @details
	 * 
	 * @param relation       - the operator between the pixels as specified in the enum operators
	 */
	PixelLogic(int relation = 0);
	PixelLogic(PixelLogic* logic);
	PixelLogic(const PixelLogic& logic);
	~PixelLogic();

	//the operators possible for the combinatory logic
	enum operators {Or   = 0,
	                And  = 1,
	            	Xor  = 2,
	                Nor  = 3,
	            	Nand = 4,
	            	Xnor = 5,
	            	Not  = 6
	            };

	/**
	 * @brief adds a pixel reference to the logic element and specified whether it will contribute
	 *             to the resulting hit address. This is necessary as surrounding pixels may be
	 *             included in a group preventing the generation of a hit
	 * @details
	 * 
	 * @param address        - the address of the pixel in the group
	 * @param ownpixel       - this address will contribute to the resulting group address if this
	 *                            parameter is true, or not if it is false
	 */
	void AddPixelAddress(int address, bool ownpixel = true);
	/**
	 * @brief removes all pixels from the logic element
	 * @details
	 */
	void ClearPixelAddresses();
	/**
	 * @brief provides the total number of pixels associated with this logic element
	 * @details
	 * @return               - the number of pixels in this logic element
	 */
	int  GetNumPixelAddresses();
	/**
	 * @brief provides the number of pixels contributing to the group address
	 * @details
	 * @return               - the number of pixels contributing to the group address
	 */
	int  GetNumOwnPixelAddresses();

	/**
	 * @brief the logic implements recursion to allow implementation of more complex logic. This
	 *             method adds a subordinate logic element to this logic element. Subordinate logic
	 *             elements are treated like pixels and can arbitrarily combined.
	 *             The PixelLogic object is used directly, no copy is generated.
	 * @details
	 * 
	 * @param sublogic       - pointer to the subordinate logic element
	 */
	void AddPixelLogic(PixelLogic* sublogic);
	/**
	 * @brief the same as above, but a copy of the subordinate logic is created
	 * @details
	 * 
	 * @param sublogic       - the subordinate logic to add a copy of to the current logic element
	 */
	void AddPixelLogic(const PixelLogic& sublogic);
	/**
	 * @brief removes all subordinate logic elements
	 * @details
	 */
	void ClearPixelLogic();
	/**
	 * @brief provides the number of subordinate logic elements inside this logic element
	 * @details
	 * @return               - the number of subordinate elements
	 */
	int  GetNumPixelSubLogics();

	/**
	 * @brief provides the code of the logic operator of this logic element. The code used is
	 *             defined in the enum operators
	 * @details
	 * @return               - the code of the operator used in this logic element
	 */
	int  GetRelation();
	void SetRelation(int relation);

	/**
	 * @brief searches the subordinate logic elements for pixels contributing (or not contributing)
	 *             to the group address to save them in the vectors passed. This function is called
	 *             recursively on all subordinate logic elements with the parameters passed here
	 * @details
	 * 
	 * @param masterown      - pointer to the vector to collect all hits that are marked as owned
	 *                            at least once
	 * @param masternotown   - pointer to the vector containing hits that were at least once not
	 *                            declared as own pixels
	 */
    void FindNewPixels(std::vector<int>* masterown, std::vector<int>* masternotown);
    /**
     * @brief evaluates whether there was a hit in the passed readout cell
     * @details 
     * 
     * @param cell           - the readout cell to evaluate
     * @param timestamp      - the current time stamp at which the readout cell is to be evaluated
     * 
     * @return               - true, if the logic result is positive, false if not
     */
	bool Evaluate(ReadoutCell* cell, int timestamp);
	/**
	 * @brief generates the group hit according to the logic for whether a hit is to be generated
	 *             and which pixels are to be considered for the address
	 * @details
	 * 
	 * @param cell           - the readout cell to evaluate
	 * @param timestamp      - the current time stamp of the evaluation
	 * @param out            - output string for the logging of failed readout
	 * @return               - the resulting hit object or an invalid hit object when there was no
	 *                            hit to be read out
	 */
	Hit  ReadHit(ReadoutCell* cell, int timestamp, std::string* out = 0);
	/**
	 * @brief removes hits from the pixels used in the PixelLogic object
	 * @details
	 * 
	 * @param cell          - the readout cell to with the pixels belong
	 * @param resetcharge   - determines whether the charge is kept (false), or not
	 */
	void ClearHit(ReadoutCell* cell, bool resetcharge);
private:
	std::vector<PixelLogic*> sublogics;
	std::vector<int> pixels;
	std::vector<int> ownpixels;
	std::vector<int> notownpixels;
	int relation;
};

/**
 * @brief implementation of the pixel readout with complex logic between the pixels
 * @details
 */
class ComplexReadout : public PixelReadout
{
public:
	ComplexReadout(ReadoutCell* roc);
	~ComplexReadout();

	/**
	 * @brief evaluates the pixels in the readout cell for hits and reads them according to the
	 *             combinatory logic supplied
	 * @details
	 * 
	 * @param timestamp      - current time stamp of the evaluation
	 * @param out            - output string for the failed reads (lost/overwritten hits,...)
	 * 
	 * @return               - true if a hit was generated, false if not
	 */
	bool Read(int timestamp, std::string* out = 0);

	/**
	 * @brief sets a new combinatory logic structure for the evaluation of the pixels. The object
	 *             the passed pointer is pointing at is directly used, no copy is created
	 * @details
	 * 
	 * @param logic          - pointer to the data structure representing the combinatory logic
	 */
	void SetPixelLogic(PixelLogic* logic);
	/**
	 * @brief same as above, but a copy of the logic structure is generated
	 * @details
	 * 
	 * @param logic          - the logic to add as a copy
	 */
	void SetPixelLogic(const PixelLogic& logic);
	/**
	 * @brief provides a pointer to the logic inside the object
	 * @details
	 * @return               - a pointer to the combinatory logic data structure
	 */
	PixelLogic* GetPixelLogic();


	/**
	 * @brief changes the readout cell the ComplexReadout object is referenced to as this structure
	 *             is more complex than the other options and copying is also more complex
	 * @details
	 * 
	 * @param roc            - the new readout cell the object is referenced to
	 */
	void SetReadoutCell(ReadoutCell* roc);
	/**
	 * @brief method to determine whether the PixelReadout pointer in a readout cell needs special
	 *             treatment
	 * @details
	 * @return               - true, as this ComplexReadout class needs special treatment
	 */
	bool NeedsROCReset();

	/**
	 * @brief provides the edge detection setting for this complex readout. Possible options are:
	 *             0 - no edge detection. If a new hit occurs, a hit is generated.
	 *             1 - use the result of the last evaluation as a reference for the edge detection
	 *                   i.e. only if the last evaluation resulted in zero, a new hit can be detected
	 *             2 - check all time stamps since the last evaluation for a zero result and if it
	 *                   occured in any of those time stamps, a new hit can be generated.
	 * @details
	 * @return               - the setting for the edge detection
	 */
	int GetEdgeDetect();
	void SetEdgeDetect(int edgedet);
private:
	PixelLogic* logic;

	int edgedetect;
	bool lastevaluation;
	int lastevaluationts;
};
//---- End Pixel Readout Classes ----

#endif //_ROCFUNCTIONS