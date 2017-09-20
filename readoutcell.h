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

#ifndef _READOUTCELL
#define _READOUTCELL

#include <string>
#include <vector>
#include <sstream>

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
	friend class OneByOneReadout;
	friend class TokenReadout;
	friend class SortedROCReadout;

	friend class PixelReadout;
	friend class PPtBReadout;
	friend class PPtBReadoutOrBeforeEdge;
	friend class ComplexReadout;

public:
	enum config {PPTB 				=    1,
				 PPTBORBEFOREEDGE   =    2,
				 ZEROSUPPRESSION	=    4,
				 FIFOBUFFER 		=    8,
				 PRIOBUFFER     	=   16,
				 NOREADONFULL 		=   32,
				 NOOVERWRITE 		=   64,
				 OVERWRITEONFULL 	=  128,
				 ONEBYONEREADOUT 	=  256,
				 TOKENREADOUT 		=  512,
				 SORTEDROCREADOUT 	= 1024};

	/**
	 * @brief constructor with basic configuration of the readout cell
	 * @details
	 * 
	 * @param addressname    - text identifier for the address part
	 * @param address        - actual addressing number for the address part given by `addressname`
	 * @param hitqueuelength - number of buffers inside the structure (or number of structures in
	 *                            parallel)
	 * @param configuration  - flags to change the behaviour of the readoutcell. For details see
	 *                            'readoutcell_functions.h'.
	 */
    ReadoutCell(std::string addressname, int address, int hitqueuelength,
  				int configuration = PPTB | ZEROSUPPRESSION | FIFOBUFFER | NOREADONFULL);
	ReadoutCell();
	ReadoutCell(const ReadoutCell& roc);
	~ReadoutCell();

	void Cleanup();

	/**
	 * @brief the flags set for the configuration of the readoutcell
	 * @details
	 * @return               - the flags as defined in the config enum
	 */
	int 		GetConfiguration();
	/**
	 * @brief changes the configuration of the readoutcell. All flags have to be provided at once
	 * @details
	 * 
	 * @param newconfig      - the configuration flags for the readoutcell
	 */
	void 		SetConfiguration(int newconfig);
	/**
	 * @brief enables replacing simple PPtB readout with a complex logic behind the readout instead
	 *             of a simple OR function
	 * @details
	 * 
	 * @param pixelro        - pointer to the object to replace the pixel readout
	 */
	void        SetComplexPPtBReadout(PixelReadout* pixelro);

	/**
	 * @brief the delay after which a hit placed in the readoutcell can be read out. For delays
	 *             shoorter than this number, an invalid hit object will be returned when reading
	 *             a hit from this readoutcell
	 * @details
	 * @return               - the minimum delay necessary to read out a hit. Measured in 
	 * 							  timestamps
	 */	
	int 		GetReadoutDelay();
	/**
	 * @brief changes the minimum delay between placement and readout of hits. To make a hit 
	 *             readable as soon as it is placed in the readoutcell, a delay of -1 has to be 
	 *             applied. shorter delays are possible but harm causality
	 * @details
	 * 
	 * @param delay          - minimum time between placement and successful readout in timestamps
	 */
	void 		SetReadoutDelay(int delay);

	/**
	 * @brief provides the time stamp field name of the reference of the readout delay
	 * @details
	 * @return               - "" if the reference if the input time, or a string containing the
	 *                            name of the time stamp field to use as a reference
	 */
	std::string	GetReadoutDelayReference();
	void 		SetReadoutDelayReference(std::string tsname);

	/**
	 * @brief provides information whether the readout cell is zero suppressed or not
	 * @details 
	 * @return               - true for a zerosuppressed cell, false for a not suppressed cell
	 */
	bool        GetZeroSuppression();

	/**
	 * @brief flag to mark readoutcells in need of a trigger signal without which the hit saved in
	 *             this object will be deleted. The trigger signal has to be present at the exact
	 *             timestamp specified by the hit timestamp and the readoutdelay of this object.
	 *             Before the trigger signal, the hit can not be read out normally.
	 * @details
	 * @return               - true if the readoutcell requires a trigger signal
	 */
	bool 		GetTriggered();
	void 		SetTriggered(bool triggered);
	
	/**
	 * @brief the identifying string for the address part identifying this readoutcell. Not the
	 * 			   address itself. The address name only provides a reference how to interpret 
	 * 			   the address property
	 * @details
	 * @return               - name of the address field of this object
	 */
    std::string GetAddressName();
	void		SetAddressName(std::string addressname);
	
	/**
	 * @brief the address of the object. Only usable with the address name
	 * @details 
	 * @return               - actual address of this object for the address part given by 
	 * 							  `addressname`
	 */
	int 		GetAddress();
	void		SetAddress(int address);
	
	/**
	 * @brief the number of buffers for this structure
	 * @details
	 * @return               - the number of hits that can be stored at once by this structure
	 */
    int			GetHitqueuelength();
	void		SetHitqueuelength(int hitqueuelength);

	/**
	 * @brief provides the position of the readout cell
	 * @details
	 * @return               - a vector containing the position of the corner at smaller 
	 * 							  coordinates
	 */	
	TCoord<double> GetPosition();
	void        SetPosition(TCoord<double> newposition);
	/**
	 * @brief provides the size of the readout cell
	 * @details
	 * @return               - a vector containing the lengths of the readout cell along the 3
	 *                            coordinate axes
	 */
	TCoord<double> GetSize();
	void        SetSize(TCoord<double> newsize);
	
	/**
	 * @brief places a new hit in this structure
	 * @details
	 * 
	 * @param hit            - the hit object to place inside this readoutcell
	 * @param timestamp      - current timestamp at which the placement is to occur
	 * @return               - true if the hit was placed or false if not
	 */
	bool		AddHit(Hit hit, int timestamp = -1);
	/**
	 * @brief reads a hit from the structure according to the behaviour determined by the
	 *             configuration (see 'readoutcell_functions.h')
	 * @details
	 * 
	 * @param timestamp      - current timestamp at which the reading is to occur
	 * @param remove         - determines whether the hit is deleted after reading or not
	 * @return               - a valid hit object if there was one in the readoutcell or an
	 * 							  invalid one if no hit was available or existent
	 */
	Hit 		GetHit(int timestamp = -1, bool remove = true);
	/**
	 * @brief the number of hits in this structure. Not the number of hits ready to be read out,
	 *             but the number of hits occupying buffers
	 * @details
	 * @return               - the number of hits present in the structure
	 */
	int 		GetEnqueuedHits();

	/**
	 * @brief returns the pixel object at the given index
	 * @details
	 * 
	 * @param index          - index of the pixel to return
	 * @return               - a pointer to the pixel of interest, or a NULL pointer on an 
	 * 							  invalid index
	 */	
	Pixel* 		GetPixel(int index);
	/**
	 * @brief returns the pixel object with the given address
	 * @details
	 * 
	 * @param address        - address of the pixel of interest
	 * @return               - a pointer to the pixel of interest, or a NULL pointer on an 
	 * 							  invalid index
	 */
	Pixel*		GetPixelAddress(int address);
	/**
	 * @brief adds a copy of the provided pixel object to the readoutcell
	 * @details
	 * 
	 * @param pixel          - the pixel object to add as it is
	 */
	void		AddPixel(Pixel pixel);
	/**
	 * @brief removes all pixels from the readoutcell
	 * @details
	 */
	void		ClearPixelVector();
	/**
	 * @brief returns the number of pixels connected to this readoutcell
	 * @details
	 * @return               - the number of pixels
	 */
	int         GetNumPixels();
	/**
	 * @brief iterator over the pixels in the readoutcell
	 * @details
	 * @return               - iterator pointing to the first pixel in the vector
	 */
	std::vector<Pixel>::iterator GetPixelsBegin();
	std::vector<Pixel>::iterator GetPixelsEnd();
	/**
	 * @brief recalculates position and size of the readout cell
	 * @details
	 * @return               - true if changes were made, false if not
	 */
	bool UpdateSize();
	
	/**
	 * @brief returns a pointer to a subordinate readoutcell
	 * @details
	 * 
	 * @param index          - index of the readoutcell to return
	 * @return               - a pointer to the readoutcell of interest or a NULL pointer if the
	 *                            index is out of bounds
	 */
	ReadoutCell* GetROC(int index);
	/**
	 * @brief returns a pointer to a subordinate readoutcell given by the address of it
	 * @details
	 * 
	 * @param address        - address of the readoutcell of interest
	 * @return               - a pointer to the readoutcell of interest or a NULL pointer if the
	 *                            passed address is not in use
	 */
	ReadoutCell* GetROCAddress(int address);
	/**
	 * @brief adds a subordinate readoutcell to this one
	 * @details
	 * 
	 * @param readoutcell    - the readoutcell to copy to this object
	 */
	void		AddROC(ReadoutCell readoutcell);
	/**
	 * @brief removes all subordinate readoutcell from this readoutcell
	 * @details
	 */
	void		ClearROCVector();
	/**
	 * @brief the number of directly subrdinate readoutcells to this readoutcell
	 * @details 
	 * @return               - number of direct child readoutcells
	 */
	int         GetNumROCs();
	/**
	 * @brief iterator pointing to the first subordinate readoutcell
	 * @details
	 * @return               - an iterator pointing to the first child readoutcell
	 */
	std::vector<ReadoutCell>::iterator GetROCsBegin();
	std::vector<ReadoutCell>::iterator GetROCsEnd();

	/**
	 * @brief implants the hit object passed in the detector and writes a line to a log file if
	 *             passed on failure
	 * @details
	 * 
	 * @param hit            - the hit to implant in the detector
	 * @param timestamp      - current timestamp at which the hit is implanted
	 * @param out            - pointer to an string for writing for logging failed implants
	 * @return               - true on successful implantation, false on failure
	 */
    bool        PlaceHit(Hit hit, int timestamp, std::string* out = 0);

    /**
     * @brief initiates the transition of hits from pixels to their parent readoutcell. The call is
     *             recursive, so that this method is also called on all subordinate readoutcells
     * @details
     * 
     * @param timestamp      - current timestamp at which this action occurs
     * @param out            - output string for logging lost hits
     * 
     * @return               - true if at least one hit was transferred, false if not
     */
    bool 		LoadPixel(int timestamp, std::string* out = 0);
    /**
     * @brief initiates the transition of hits from one kind of readoutcells to their parent
     *             readoutcell given by the address name. The call is recursive and only changes a
     *             readoutcell if its addressname is correct.
     * @details
     * 
     * @param addressname    - address name of the readoutcell to transfer the hits to: The parent
     *                            ROC address name
     * @param timestamp      - current timestamp at which this action is to occur
     * @param out            - output string for logging lost hits
     * @return               - true if at least one hit was transferred, false if not
     */
    bool 		LoadCell(std::string addressname, int timestamp, std::string* out = 0);
    /**
     * @brief reads one hit from the subordinate readoutcells and deletes this hit in them. The
     * 		       same functionality as GetHit(...)
     * @details
     * 
     * @param timestamp      - current timestamp at which the action is to occur
     * @param remove  		 - determines whether the hit is deleted in the subordinate readoutcell
     *         					  or not
     * @return               - the hit object read, without an available hit present, an invalid
     *                            hit is returned
     */
    Hit 		ReadCell(int timestamp = -1, bool remove = true);
    /**
     * @brief provides the number of hits inside the readoutcell. The call is recursive.
     * @details
     * 
     * @param addressname    - address name to take into account for the hit counting. An empty
     *                            string is used to count all hits in the detector. A combination
     *                            of several address names is not possible
     * @return               - the number of hits in the selected structures
     */
    int 		HitsAvailable(std::string addressname);

    /**
     * @brief generates a string displaying the structure of this readoutcell and its children. The
     *             call is recursive.
     * @details
     * 
     * @param space          - string to be placed at the beginning of each line for indention
     * @return               - a string including newlines showing the detector structure below
     * 							  this readoutcell
     */
    std::string PrintROC(std::string space);

    /**
     * @brief shifts all subordinate objects by the vector provided
     * @details
     * 
     * @param distance       - vector describing the shift of this structure
     */
    void 		ShiftCell(TCoord<double> distance);

    /**
     * @brief removes all hits from the readoutcell and its subordinate readoutcells that expect a
     *             trigger signal at the point in time when this function is called
     * @details
     * 
     * @param timestamp      - current time stamp when this action is to be executed
     * @param sbadout        - output string to log the removed hits
     */
    void 		NoTriggerRemoveHits(int timestamp, std::string* sbadout = 0);

    /**
     * @brief checks the subordinate readout cell addresses for multiple identical addresses and
     *             changes the addresses if necessary. The function is recursive, it is not
     *             necessary to call it on every level.
     * @details
     * @return               - true if an address was changed, false if not
     */
    bool 		CheckROCAddresses();

    /**
     * @brief sets the pointer to the presented element of the trigger table in the 
     * 			   SortedROCReadout strategy. The call is recursive.
     * @details
     * 
     * @param front          - pointer to the trigger queue front element to insert
     * @param clearpattern   - bit pattern which is NOT to be used for the match comparison of the
     *                            trigger timestamp. To use the whole TS set this parameter to 0.
     */
    void        SetTriggerTableFrontPointer(const int* front, const int clearpattern = 0);

    /**
     * @brief saves all hits from the readout cell, its pixels and its substructure into the lost
     *             hit output for finishing the simulation. The hits get an additional readout
     *             time stamp called "SimulationEnd"
     * @details
     * 
     * @param timestamp      - time stamp at the end of the simulation
     * @param sbadout        - output string for the lost hits
     * 
     * @return               - the number of hits removed from the structure
     */
    int         RemoveAndSaveAllHits(int timestamp, std::string* sbadout = 0);
	
private:
	std::string 				addressname;
	int 						address;
	int 						hitqueuelength;
	std::vector<Hit> 			hitqueue;
	std::vector<Pixel> 			pixelvector;
	std::vector<ReadoutCell> 	rocvector;

	TCoord<double> 				position;
	TCoord<double>				size;

	//function objects to change the behaviour of the Readout Cell:
	ROCBuffer*		buf;			//readint/writing to the buffer
	ROCReadout*		rocreadout;		//reading from the child ROCs
	PixelReadout*	pixelreadout;	//reading from the pixels
	bool 			zerosuppression;

	int 			readoutdelay;
	bool 			triggered;
	std::string 	delayreference;

	int 			configuration;	//to save the readout settings according to the config enum

};


#endif  //_READOUTCELL
