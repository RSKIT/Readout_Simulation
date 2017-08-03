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

#ifndef _DETECTOR
#define _DETECTOR

#include <string>
#include <vector>
#include <queue>
#include <fstream>

//#include "hit.h"
//#include "pixel.h"
//#include "readoutcell.h"
#include "detector_base.h"
//#include "TCoord.h"

/**
 * @brief implementation of the MuPix8 State Machine for the MuPix8 sensor (the same state machine
 *                 is also in use in other sensors, but initiates slightly different actions)
 * @details
 */
class Detector : public DetectorBase
{
public:
    Detector(std::string addressname, int address);
	Detector();
	Detector(const Detector& templ);
    ~Detector();

    //Names for the states of the state machine:
    enum state {PullDown = 0, LdPix = 1, LdCol = 2, RdCol = 3};
	
    /**
     * @brief executes the actions of the states and prepares the state transitions
     * @details
     * 
     * @param timestamp      - the current timestamp for the function call
     * @param trigger        - the trigger signal for the detector readout. In this implementation
     *                            the parameter is not used
     * @param print          - turns on (true) or off printing to terminal
     * @param updatepitch    - the number of timestamps per which one time output is generated
     * 
     * @return               - false on an error, true otherwise
     */
    bool	StateMachineCkUp(int timestamp, bool trigger = true,
                                bool print = false, int updatepitch = 10);
    /**
     * @brief performs the synchronisation of hits placed in the detector and removes hits from
     *             triggered buffers on missing trigger signals
     * @details
     * 
     * @param timestamp      - the current timestamp for the function call
     * @param trigger        - the trigger signal for the detector. Hits from triggered
     *                            readoutcells are removed if this parameter is false at the
     *                            triggering timestamp
     * @param print          - turns on (true) or off printing to terminal
     * @param updatepitch    - the number of timestamps per which one time output is generated
     * 
     * @return               - false on an error, true otherwise
     */
    bool	StateMachineCkDown(int timestamp, bool trigger = true,
                                bool print = false, int updatepitch = 10);

    /**
     * @brief the index of the current state of the state machine.
     * @details
     * @return               - the index of the current state machine state
     */
    int 	GetState();
    /**
     * @brief changes the current state to the index provided after checking the validity of
     *             the index
     * @details
     * 
     * @param index          - new index of the current state
     */
    void    SetState(int index);
    /**
     * @brief the index of the next state.
     * @details
     * @return               - the index of the next state if it is set, or -1 in case of an error
     *                            or an unset index
     */
    int 	GetNextState();
    /**
     * @brief the text name of the current state
     * @details
     * @return               - the name of the current state as a string
     */
    std::string GetCurrentStateName();

    /**
     * @brief creates and returns a copy of itself to avoid double usage of the same pointers
     * @details
     * @return               - a pointer to a copy of the object the method is called upon
     */
    DetectorBase* Clone();
    
    /**
     * @brief the number of states in the state machine
     * @details
     * @return               - the total number of states in the state machine
     */
    int GetNumStates();
private:
	int currentstate;
	int nextstate;
	int delay;
};


#endif  //_DETECTOR
