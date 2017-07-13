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

#ifndef _DETECTORBASE
#define _DETECTORBASE

#include <string>
#include <vector>
#include <queue>
#include <fstream>

#include "hit.h"
#include "pixel.h"
#include "readoutcell.h"
#include "TCoord.h"

class DetectorBase
{
public:
	/**
	 * @brief constructor for a new detector object without a working readout system
	 * @details
	 * 
	 * @param addressname    - name of the address part describing the number of the detector
	 * @param address        - address of this detector
	 */
    DetectorBase(std::string addressname, int address);
	DetectorBase();
	DetectorBase(const DetectorBase& templ);
	~DetectorBase();

	virtual void Cleanup();
	
	/**
	 * @brief the identifying string of the address part determining the detector address
	 * @details
	 * @return               - the string identifying the address of the detector
	 */
    std::string GetAddressName();
	void		SetAddressName(std::string addressname);
	
	/**
	 * @brief the actual address of the detector. In hit addresses it occurs together with the
	 *             address name above
	 * @details 
	 * @return               - the address of the detector
	 */
	int 		GetAddress();
	void		SetAddress(int address);
	
	/**
	 * @brief returns a pointer to the readoutcell at index `index`
	 * @details
	 * 
	 * @param index          - the index of the readoutcell of interest
	 * @return               - a pointer to the readoutcell of interest or a NULL pointer if the
	 *                            provided index is out of bounds
	 */
    ReadoutCell*    GetROC(int index);
    /**
     * @brief returns a pointer to the readoutcell with the provided address
     * @details
     * 
     * @param address        - address of the readoutcell of interest
     * @return               - a pointer to the readoutcell of interest or a NULL pointer if the
     *                            provided address is not in use
     */
    ReadoutCell*    GetROCAddress(int address);
    /**
     * @brief adds a copy of a readoutcell to the detector
     * @details
     * 
     * @param readoutcell    - the readoutcell of which a copy will be added to the detector
     */
	void		AddROC(ReadoutCell readoutcell);
	/**
	 * @brief removes all readoutcells from the detector
	 * @details
	 */
	void		ClearROCVector();
	/**
	 * @brief an iterator pointing to the first readoutcell in the detector
	 * @details
	 * @return               - an iterator over all readoutcells in the detector pointing to the
	 *                            first element
	 */
	std::vector<ReadoutCell>::iterator GetROCVectorBegin();
	std::vector<ReadoutCell>::iterator GetROCVectorEnd();
	/**
	 * @brief checks the addresses of all subordinate readout cells for multiple uses of the same
	 *             address and changes the addresses if necessary
	 * @details 
	 * @return               - true if a change was necessary, false if not
	 */
	bool		CheckROCAddresses();
	
	/**
	 * @brief the origin of the detector volume. It is used for the base point of the particle
	 *             tracks going through the detector. The origin is the lower back left corner of
	 *             the detector.
	 * @details
	 * @return               - the origin of the detector
	 */
	TCoord<double>	GetPosition();
	void		SetPosition(TCoord<double> position);
	
	/**
	 * @brief the extension of the detector in 3D space
	 * @details
	 * @return               - the lengths of the detector parallel to all 3 space dimensions
	 */
	TCoord<double>	GetSize();
	void		SetSize(TCoord<double> size);

	/**
	 * @brief checks whether all pixels are inside the detector volume
	 * @details
	 * @return               - true if all pixels are completely inside the detector volume
	 */
    bool        SizeOK();
    /**
     * @brief enlarges the detector volume just as much as needed to include all pixels in the
     *             detector
     * @details
     * @return               - true if the detector was enlarged, false if everything was ok
     */
    bool        EnlargeSize();
	
	/**
	 * @brief place holder for the actions executed by the state machine on the rising edge of the
	 *             clock
	 * @details
	 * 
	 * @param timestamp      - current timestamp that is to be worked on
	 * @param trigger        - trigger signal for triggered readout. The trigger signal is a
	 *                            positive signal.
	 * @param print          - turns on (true) or off printing to terminal
	 * @param updatepitch    - the number of timestamps per which one time output is generated
	 * 
	 * @return               - true if everything was ok. false indicates problems in the state
	 *                            machine execution and should stop the simulation
	 */
    virtual bool        StateMachineCkUp(int timestamp, bool trigger = true,
    										bool print = false, int updatepitch = 1);
    /**
     * @brief the same as StateMachineCkUp() but for the falling edge of the clock
     * @details
     * 
     * @param timestamp      - current timestamp
     * @param trigger        - trigger signal for triggered readout.
     * @param print          - turns on (true) or off printing to terminal
     * @param updatepitch    - the number of timestamps per which one time output is generated
     * 
     * @return               - false indicates problems in the execution and is a signal to stop
     *                            the simulation
     */
    virtual bool 		StateMachineCkDown(int timestamp, bool trigger = true,
    										bool print = false, int updatepitch = 1);
    /**
     * @brief executes StateMachineCkUp() and StateMachineCkDown() at once
     * @details
     * 
     * @param timestamp      - current timestamp
     * @param trigger        - trigger signal for triggered readout
     * 
     * @return               - false on an error, true otherwise
     */
    virtual bool		StateMachine(int timestamp, bool trigger = true);

    /**
     * @brief inserts the hit for the current timestamp in the pixel it belongs into
     * @details
     * 
     * @param hit            - the hit to be placed in the detector
     * @param timestamp      - the current timestamp at which the hit is placed into the detector
     * 
     * @return               - true if the hit was placed, false if not (either no unoccupied
     *                            buffer or wrong (nonexisting) address)
     */
    bool        PlaceHit(Hit hit, int timestamp);
    /**
     * @brief saves the hit to the given file
     * @details
     * 
     * @param hit            - the hit to be saved
     * @param filename       - name of the file to save the hit to
     * @param compact        - determines whether the address names and timestamp names are written
     *                            in the line or not
     */
	void 		SaveHit(Hit hit, std::string filename, bool compact);
	/**
	 * @brief saves the hit to the file set for the detector for good hits
	 * @details 
	 * 
	 * @param hit            - the hit to be saved
	 * @param compact        - write the address names and readout timestamp identifiers in the
	 *                            line if false
	 * 
	 * @return               - true if the hit was saved in the file, false otherwise
	 */
	bool		SaveHit(Hit hit, bool compact = false);
	/**
	 * @brief saves the hit to the file set for the detector for "lost" hits
	 * @details
	 * 
	 * @param hit            - the hit to be saved
	 * @param compact        - write the address names and readout timestamp identifiers in the
	 *                            line if false
	 * 
	 * @return               - true if the hit was saved in the file, false otherwise
	 */
	bool		SaveBadHit(Hit hit, bool compact = false);
	/**
	 * @brief writes the contents of the found hits data stream to the file
	 * @details
	 * @return               - true if the write was successful, false if not
	 */
	bool 		FlushOutput();
	/**
	 * @brief writes the contents of the lost hits data stream to the file
	 * @details
	 * @return               - true if the write was successful, false if not
	 */
	bool 		FlushBadOutput();
	/**
	 * @brief provides all the good output as a string
	 * @details
	 * @return               - a string containing all the good output of the detector
	 */
	std::string GenerateOutput();
	void ClearOutput();
	/**
	 * @brief provides all the bad output as a string
	 * @details
	 * @return               - a string containing all the lost hits of the detector
	 */
	std::string GenerateBadOutput();
	void ClearBadOutput();
	/**
	 * @brief the number of hits available in this detecot in total
	 * @details
	 * @return               - the number of hits available in all buffers summed up
	 */
	int 		HitsEnqueued();
	/**
	 * @brief counts all hits in a specific buffer structure in the detector
	 * @details
	 * 
	 * @param addressname    - address identifier of the buffer structure to investigate or an
	 *                            empty string 
	 * @return               - the number of hits in the selected buffer structure or 0 if the
	 *                            selected buffer structure does not exist
	 */
	int 		HitsAvailable(std::string addressname);

	/**
	 * @brief the name of the file for writing out successfully read out hits
	 * @details
	 * @return               - the currently set filename
	 */
	std::string	GetOutputFile();
	/**
	 * @brief changes the output file for found hits. The previously opened file is closed.
	 *             The contents of the data stream is written to the file beforehand.
	 * @details
	 * 
	 * @param filename       - the file name of the new file to open
	 */
	void 		SetOutputFile(std::string filename);
	/**
	 * @brief the output file for the found hits is closed and the contents of the data stream
	 *             is written to the file beforehand
	 * @details
	 */
	void 		CloseOutputFile();

	/**
	 * @brief the name of the file for writing out "lost" hits
	 * @details
	 * @return               - the currently set filename
	 */
	std::string	GetBadOutputFile();
	/**
	 * @brief changes the output file for lost hits. The previously opened file is closed.
	 *             The contents of the data stream is written to the file beforehand.
	 * @details
	 * 
	 * @param filename       - the file name of the new file to open
	 */
	void 		SetBadOutputFile(std::string filename);
	/**
	 * @brief the output file for the lost hits is closed and the contents of the data stream
	 *             is written to the file beforehand
	 * @details
	 */
	void 		CloseBadOutputFile();

	/**
	 * @brief returns the number of hits read out from the detector since the last reset of the
	 *             counter
	 * @details
	 * @return               - the number of hits since the last counter reset
	 */
	int 		GetHitCounter();
	void		ResetHitCounter();

	/**
	 * @brief generates a string representation of the detector structure in its current state
	 * @details
	 * @return               - the detector structure as a string including newlines
	 */
    std::string PrintDetector();
    
    /**
     * @brief the index of the current state of the state machine. This function is to be
     *             implemented in the inheriting classes.
     * @details
     * @return               - the index of the current state machine state. Here, the state
     *                            machine is not implemented and the return value is an invalid
     *                            state: -1
     */
    virtual int 	GetState();
    /**
     * @brief changes the current state to the index provided. In this base class, this method does
     *             nothing
     * @details
     * 
     * @param index          - index of the new current state
     */
    virtual void 	SetState(int index);
    /**
     * @brief the index of the next state to go to. If not set, the value is invalid (-1).
     * @details
     * @return               - index of the next state, or -1 on an error
     */
    virtual int 	GetNextState();
    /**
     * @brief provides the name of the current state
     * @details
     * @return               - text string containing the name of the current state
     */
    virtual std::string GetCurrentStateName();

    /**
     * @brief creates and returns a copy of itself to avoid double usage of the same pointers
     * @details
     * @return               - a pointer to a copy of the object the method is called upon
     */
    virtual DetectorBase* Clone();

    /**
     * @brief the number of states in the state machine
     * @details
     * @return               - the total number of states in the state machine
     */
    virtual int GetNumStates();

protected:
	std::string 				addressname;
	int 						address;
	std::vector<ReadoutCell> 	rocvector;
	TCoord<double> 				position;
    TCoord<double> 				size;

    /**
     * @brief checks whether the passed readoutcell is completely inside the detector. The 
     *             subordinate readoutcells are called recursively.
     * @details
     * 
     * @param cell           - the readoutcell to check
     * @return               - true if the readoutcell is completely inside the detector, 
     *                            false otherwise
     */
    bool    SizeOKROC(ReadoutCell* cell);
    /**
     * @brief enlarges the detector size to make the passed readoutcell fit inside the detector
     * @details
     * 
     * @param cell           - the readoutcell to check for being inside the detector volume
     * @return               - true if the detector volume was enlarged, false if not
     */
    bool 	EnlargeSizeROC(ReadoutCell* cell);

    int 						hitcounter;
    std::string 				outputfile;
    std::stringstream			sout;			//stream to collect the data from the simulation
    std::fstream 				fout;

    int 						badhitcounter;
    std::string 				badoutputfile;
    std::stringstream			sbadout;		//stream to collect the data from the simulation
    std::fstream 				fbadout;

};


#endif  //_DETECTORBASE
