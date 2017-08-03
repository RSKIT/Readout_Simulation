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

#ifndef _XMLDETECTOR
#define _XMLDETECTOR

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <utility>


#include "detector_base.h"

/**
 * @brief data structure to load dynamically actions on e.g. registers and counters. It is only a
 *                 grouping class to clean the code
 * @details
 */
class RegisterAccess
{
public:
	std::string what;		//action to perform
	std::string parameter;	//text parameter for the action
	double value;			//double parameter for the action (chosen as small integers can also
	                        // be represented with it)

	RegisterAccess() {
		what = "";
		parameter = "";
		value = 0;
	}
};

/**
 * @brief data structure to dynamically load comparisons
 * @details
 */
class Comparison
{
public:
	//options for the comparison:
	enum Relations {
		Smaller 		=  1,
		SmallerEqual 	=  2,
		Larger  		=  3,
		LargerEqual		=  4,
		Equal 			=  5,
		NotEqual 		=  6,
		Or 				=  7,
		And 			=  8,
		Xor 			=  9,
		empty           = 10	//to enable skipping of the writing of the comparison in the XML
	};

	//property to use for the comparison:
	enum Choice {
		Comp        = 1,
		Value 		= 2,
		Register 	= 3
	};

	Comparison(int relation = Smaller);
	Comparison(const Comparison& comp);

	~Comparison();

	Comparison& operator=(const Comparison& comp);

	/**
	 * @brief the relation operator of the comparison
	 * @details
	 * @return               - the code for the relation operator (see enum Relations)
	 */
	int  GetRelation();
	void SetRelation(int operand);

	/**
	 * @brief the property choice for the L-value (first) or the R-value (second) respectively
	 * @details
	 * @return               - the enum Choice code for the property to use on the respevtive
	 *                            side of the comparison
	 */
	int  GetFirstChoice();
	bool SetFirstChoice(int choice);
	int  GetSecondChoice();
	bool SetSecondChoice(int choice);

	/**
	 * @brief returns a pointer to the L-value (first) or the R-value (second) comparison.
	 *             Comparison objects can recursively contain other Comparison objects
	 * @details
	 * @return               - a pointer to the respective child comparison object
	 */
	Comparison* GetFirstComparison();
	/**
	 * @brief sets a copy of the passed comparison object as L-value (first) or R-value (second)
	 *             child
	 * @details
	 * 
	 * @param comp           - the comparison object to copy for the use as subordinate comparison
	 */
	void SetFirstComparison(const Comparison& comp);
	/**
	 * @brief sets the passed comparison as new subordinate comparison object. No copy is generated
	 *             and the previously used comparison object is deleted
	 * @details
	 * 
	 * @param comp           - comparison object to use as subordinate comparison
	 */
	void SetFirstComparison(Comparison* comp);
	Comparison* GetSecondComparison();
	void SetSecondComparison(const Comparison& comp);
	void SetSecondComparison(Comparison* comp);

	/**
	 * @brief a fixed value for the L-value (first) or R-value (second) of the comparison. This 
	 *                       value will be overwritten when the respective choice is not
	 *                       Comparison::Value and the Comparison is evaluated.
	 * @details
	 * @return               - the currently set value for the fixed value of the comparison side
	 */
	double GetFirstValue();
	void   SetFirstValue(double value);
	double GetSecondValue();
	void   SetSecondValue(double value);

	/**
	 * @brief the reading action from a register or counter for the evaluation of the comparison
	 * @details
	 * @return               - a copy of the RegisterAccess object stored in the comparison object
	 */
	RegisterAccess GetFirstRegisterAccess();
	void   SetFirstRegisterAccess(const RegisterAccess& regacc);
	RegisterAccess GetSecondRegisterAccess();
	void   SetSecondRegisterAccess(const RegisterAccess& regacc);

	//set the data for the RegisterAccess expressions:
	/**
	 * @brief sets the result for the evaluation of the Comparison object.
	 * @details
	 * 
	 * @param value          - the result of the evaluation of the L-value (first) or the R-value
	 *                            (second) of the comparison
	 */
	void EvalFirstRegister(double value);
	void EvalSecondRegister(double value);
	/**
	 * @brief checks the values for being set to prevent wrong results because of unset values
	 * @details
	 * @return               - true if both sides have been set
	 */
	bool ReadyForEvaluation();

	/**
	 * @brief executes the comparison of the whole comparison structure below the object is is
	 *             called upon
	 * @details
	 * @return               - the result of the comparison or false if the comparison was not set
	 *                            up properly
	 */
	bool Evaluate();

	/**
	 * @brief generates a string with newlines representing the data inside of the comparison
	 *             object. Also unused but set parameters are printed
	 * @details
	 * 
	 * @param spaces         - white space to put at the beginning of each line to properly indent
	 *                            the structure. For each child layer, some spacesare added to the
	 *                            number of spaces provided
	 */
	std::string PrintComparison(std::string spaces="") const;
private:
	int relation;

	int firstchoice;	//determines which of the next three objects is to be used
	Comparison* firstcomp;
	double firstval;
	RegisterAccess firstreg;
	bool firstregset;	//stores whether firstreg has been evaluated since the last comparison
	                    //  evaluation
	
	int secondchoice;	//determines which of the next three objects is to be used
	Comparison* secondcomp;
	double secondval;
	RegisterAccess secondreg;
	bool secondregset;	//stores whether secondreg has been evaluated since the last comparison
	                    //  evaluation
};

/**
 * @brief class describing the transition between states of the state machine class below
 * @details
 */
class StateTransition
{
public:
	StateTransition();
	StateTransition(const StateTransition& trans);
	~StateTransition();

	void Cleanup();

	/**
	 * @brief the name of the next state
	 * @details
	 * @return               - a string containing the name of the next state after this state
	 *                            transition
	 */
	std::string GetNextState();
	void SetNextState(std::string nextstate);

	/**
	 * @brief delay on the transition to the next state. Useful for transition states in which
	 *             nothing happens
	 * @details
	 * @return               - the delay in timestamps for the transition as additional cycles
	 */
	int  GetDelay();
	void SetDelay(int delay);

	/**
	 * @brief a pointer to the comparison object for this state transition
	 * @details
	 * @return               - a pointer to the comparison object of this state transition
	 */
	Comparison* GetComparison();
	/**
	 * @brief sets a copy of the passed comparison as the comparison of this state transition
	 * @details
	 * 
	 * @param comp           - the comparison object to copy to the state transition
	 */
	void SetComparison(const Comparison& comp);
	/**
	 * @brief sets the passed comparison object (as a pointer) for the state transition object. No
	 *             copy is created
	 * @details
	 * 
	 * @param comp           - pointer to the comparison object to use for the state transition
	 */
	void SetComparison(Comparison* comp);

	/**
	 * @brief removes all actions on the state transition from this object
	 * @details
	 */
	void ClearRegisterChanges();
	/**
	 * @brief adds an action to execute in case of satisfied comparison
	 * @details
	 * 
	 * @param change         - the action to execute on execution of this state transition
	 */
	void AddRegisterChange(RegisterAccess change);
	/**
	 * @brief the number of actions to execute on execution of this state transition
	 * @details 
	 * @return               - integer number of actions to execute in this state transition
	 */
	int  GetNumRegisterChanges();
	/**
	 * @brief an iterator to the first action to execute
	 * @details
	 * @return               - iterator at begin() of the vector containing the actions for the
	 *                            state transition
	 */
	std::vector<RegisterAccess>::iterator GetRegisterChangesBegin();
	std::vector<RegisterAccess>::iterator GetRegisterChangesEnd();

	/**
	 * @brief evaluates the comparison of the state transition
	 * @details
	 * @return               - the result of the comparison evaluation or false if the comparison
	 *                            was not set up properly beforehand (use GetComparison() to set it
	 *                            up beforehand)
	 */
	bool Evaluate();

private:
	std::string nextstate;
	int delay;

	Comparison* condition;

	std::vector<RegisterAccess> counterchanges;

};

/**
 * @brief a state of the state machine to load dynamically
 * @details
 */
class StateMachineState
{
public:
	StateMachineState();
	~StateMachineState();

	void Cleanup();

	/**
	 * @brief the name of the state
	 * @details
	 * @return               - the name of the state as a string
	 */
	std::string GetStateName();
	void  SetStateName(std::string name);

	/**
	 * @brief removes all actions to execute in this state
	 * @details
	 */
	void ClearRegisterChanges();
	/**
	 * @brief adds one action to execute in this state as the last action to execute
	 * @details
	 * 
	 * @param change         - the action to add
	 */
	void AddRegisterChange(RegisterAccess change);
	/**
	 * @brief the number of actions to perform in this state
	 * @details
	 * @return               - the integer number of actions to perform
	 */
	int  GetNumRegisterChanges();
	/**
	 * @brief provides an iterator over the actions to perform pointing to the first element
	 * @details
	 * @return               - iterator pointing to the first element of the action vector
	 */
	std::vector<RegisterAccess>::iterator GetRegisterChangesBegin();
	std::vector<RegisterAccess>::iterator GetRegisterChangesEnd();

	/**
	 * @brief removes all state transitions from the state
	 * @details
	 */
	void ClearStateTransitions();
	/**
	 * @brief adds a new state transition to the state. A copy of the passed state transition is
	 *             added to the state. The new state transition is added at the end of the state
	 *             transition vector.
	 * @details
	 * 
	 * @param transition     - the state transition to add
	 */
	void AddStateTransition(StateTransition transition);
	/**
	 * @brief adds a new state transition to the state. The object is used directly and no copy
	 *             is created. The new state transition is added at the end of the state
	 *             transition vector.
	 * @details
	 * 
	 * @param transition     - pointer to the state transition object to use in this state
	 */
	void AddStateTransition(StateTransition* transition);
	/**
	 * @brief the number of state transitions for this state
	 * @details
	 * @return               - integer number of state transitions
	 */
	int  GetNumStateTransitions();
	/**
	 * @brief provides an iterator pointing to the first state transition of this state
	 * @details
	 * @return               - interator pointing to the first state transition
	 */
	std::vector<StateTransition*>::iterator GetStateTransitionsBegin();
	std::vector<StateTransition*>::iterator GetStateTransitionsEnd();

private:
	std::string name;

	std::vector<RegisterAccess>  registerchanges;
	std::vector<StateTransition*> transitions;
};



/**
 * @brief implementation of the detector to be loaded dynamically
 * @details
 */
class XMLDetector: public DetectorBase
{
public:
	/**
	 * @brief constructor with parameters for the addressing of the detector
	 * @details
	 * 
	 * @param addressname    - address part identifier for the detector address
	 * @param address        - the actual address for the detector object
	 */
	XMLDetector(std::string addressname, int address);
	XMLDetector();
	XMLDetector(const XMLDetector& templ);
	XMLDetector(const DetectorBase* templ);
	~XMLDetector();

	void Cleanup();

	/**
	 * @brief performs the actions on the "rising edge" of the clock. All synchronous actions
	 *             are executed here.
	 * @details
	 * 
	 * @param timestamp      - the current timestamp of this function call
	 * @param trigger        - trigger signal for triggered readoutcells. Not used here.
     * @param print          - turns on (true) or off printing to terminal
     * @param updatepitch    - the number of timestamps per which one time output is generated
	 * 
	 * @return               - true on successful execution, false on an error
	 */
    bool	StateMachineCkUp(int timestamp, bool trigger = true, 
    							bool print = false, int updatepitch = 1);
    /**
     * @brief performs the actions on the "falling edge" of the clock. All synchronisation actions
     *             are executed here (e.g. synchronising hits in the pixels). Also the checking of
     *             triggered signals is done here.
     * @details
     * 
     * @param timestamp      - the current timestamp of this function call
     * @param trigger        - trigger signal for triggered readoutcells. Actually used here.
     * @param print          - turns on (true) or off printing to terminal
     * @param updatepitch    - the number of timestamps per which one time output is generated
     * 
     * @return               - true on successful execution, false on an error
     */
    bool	StateMachineCkDown(int timestamp, bool trigger = true,
    							bool print = false, int updatepitch = 1);

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
    void 	SetState(int index);
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
     * @brief adds a variable to the state machine accessible from all states
     * @details
     * 
     * @param name           - identifying name of the counter
     * @param value          - initial value
     */
    void AddCounter(std::string name, double value);

    /**
     * @brief adds a copy of the passed state to the state machine
     * @details
     * 
     * @param state          - the state machine state to copy and include in the state machine
     */
    void AddState(const StateMachineState& state);
    /**
     * @brief adds a state machine state to the state machine. The object the pointer is pointing
     *             to is used itself. No copy is created.
     * @details
     * 
     * @param state          - pointer to the state machine state to include
     */
    void AddState(StateMachineState* state);
    /**
     * @brief a pointer to the state at the passed index for modification
     * @details
     * 
     * @param index          - index of the state machine state of interest
     * @return               - a pointer to the state at `index`, or a NULL pointer on an invalid
     *                            index
     */
    StateMachineState* GetState(int index);
    /**
     * @brief a pointer to the first state with the passed name
     * @details
     * 
     * @param statename      - name of the state of interest
     * @return               - a pointer to the first state with a matching state name or a NULL
     *                            pointer if the name is not used
     */
    StateMachineState* GetState(std::string statename);
    /**
     * @brief the number of states in the state machine
     * @details
     * @return               - the total number of states in the state machine
     */
    int GetNumStates();
    /**
     * @brief removes all states from the state machine
     * @details
     */
    void ClearStates();
private:
	int currentstate;
	int nextstate;
	std::vector<StateMachineState*> states;
	std::map<std::string, double>  counters;

	/**
	 * @brief sets the value of the specified counter or creates it if it does not exist yet
	 * @details
	 * 
	 * @param name           - identifier of the counter/variable to set
	 * @param value          - the new value for the counter/variable
	 */
	void SetCounter(std::string name, double value);
	/**
	 * @brief increases the value of a counter or creates a new counter and sets its value to the
	 *             value passed
	 * @details
	 * 
	 * @param name           - name of the counter/variable to increment
	 * @param increment      - the value by which the counter is to be incremented
	 */
	void IncrementCounter(std::string name, double increment = 1);
	/**
	 * @brief decreases the value of a counter or creates a new counter and sets its value to
	 *             -`decrement`
	 * @details
	 * 
	 * @param name           - name of the counter/variable to decrement
	 * @param decrement      - absolute value of the decrement
	 */
	void DecrementCounter(std::string name, double decrement = 1);
	/**
	 * @brief provides the value of a counter
	 * @details
	 * 
	 * @param name           - the name of the counter/variable to retrieve
	 * @return               - the value of the counter/variable indexed by `name` or zero if the
	 *                            passed name is not in use
	 */
	double GetCounter(std::string name);

	/**
	 * @brief executes the action passed for the given time
	 * @details
	 * 
	 * @param regacc         - action descriptor to execute
	 * @param timestamp      - timestamp at which the execution is to take place
	 * @param print          - allows printing to terminal if set to true
	 */
	void ExecuteRegisterChanges(RegisterAccess regacc, int timestamp, bool print = false);

	/**
	 * @brief evaluates the register accesses in the passed comparison to enable its evaluation
	 * @details
	 * 
	 * @param comp           - the comparison to prepare for evaluation
	 */
	void FillComparison(Comparison* comp);
	/**
	 * @brief executes a query on the detector/statemachine/counters and returns the resulting 
	 *             value
	 * @details
	 * 
	 * @param regacc         - the query to execute
	 * @return               - the resulting double value of the query
	 */
	double GetValue(RegisterAccess regacc);
};

#endif //_XMLDETECTOR