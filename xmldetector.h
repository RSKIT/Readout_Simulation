#ifndef _XMLDETECTOR
#define _XMLDETECTOR

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <utility>


#include "detector_base.h"

class RegisterAccess
{
public:
	std::string what;
	std::string parameter;
	double value;

	RegisterAccess() {
		what = "";
		parameter = "";
		value = 0;
	}
};

class Comparison
{
public:

	enum Relations {
		Smaller 		=  1,
		SmallerEqual 	=  2,
		Larger  		=  3,
		LargerEqual		=  4,
		Equal 			=  5,
		NotEqual 		=  6,
		Or 				=  7,
		And 			=  8,
		Xor 			=  9
	};

	enum Choice {
		Comp        = 1,
		Value 		= 2,
		Register 	= 3
	};

	Comparison(int relation = Smaller);
	Comparison(const Comparison& comp);

	~Comparison();

	int  GetRelation();
	void SetRelation(int operand);

	int  GetFirstChoice();
	bool SetFirstChoice(int choice);
	int  GetSecondChoice();
	bool SetSecondChoice(int choice);

	Comparison* GetFirstComparison();
	void SetFirstComparison(const Comparison& comp);
	Comparison* GetSecondComparison();
	void SetSecondComparison(const Comparison& comp);

	double GetFirstValue();
	void   SetFirstValue(double value);
	double GetSecondValue();
	void   SetSecondValue(double value);

	RegisterAccess GetFirstRegisterAccess();
	void   SetFirstRegisterAccess(const RegisterAccess& regacc);
	RegisterAccess GetSecondRegisterAccess();
	void   SetSecondRegisterAccess(const RegisterAccess& regacc);

	//set the data for the RegisterAccess expressions:
	void EvalFirstRegister(double value);
	void EvalSecondRegister(double value);
	bool ReadyForEvaluation();

	bool Evaluate();

	std::string PrintComparison(std::string spaces="") const;
private:
	int relation;

	int firstchoice;	//determines which of the next three objects is to be used
	Comparison* firstcomp;
	double firstval;
	RegisterAccess firstreg;
	bool firstregset;
	
	int secondchoice;	//determines which of the next three objects is to be used
	Comparison* secondcomp;
	double secondval;
	RegisterAccess secondreg;
	bool secondregset;
};

class StateTransition
{
public:
	StateTransition();
	StateTransition(const StateTransition& trans);

	std::string GetNextState();
	void SetNextState(std::string nextstate);

	int  GetDelay();
	void SetDelay(int delay);

	Comparison* GetComparison();
	void SetComparison(const Comparison& comp);

	void ClearRegisterChanges();
	void AddRegisterChange(RegisterAccess change);
	int  GetNumRegisterChanges();
	/*const*/ 
	std::vector<RegisterAccess>::iterator GetRegisterChangesBegin();
	std::vector<RegisterAccess>::iterator GetRegisterChangesEnd();

	bool Evaluate();

private:
	std::string nextstate;
	int delay;

	Comparison condition;

	std::vector<RegisterAccess> counterchanges;

};


class StateMachineState
{
public:
	StateMachineState();

	std::string GetStateName();
	void  SetStateName(std::string name);

	void ClearRegisterChanges();
	void AddRegisterChange(RegisterAccess change);
	int  GetNumRegisterChanges();
	std::vector<RegisterAccess>::iterator GetRegisterChangesBegin();
	std::vector<RegisterAccess>::iterator GetRegisterChangesEnd();

	void ClearStateTransitions();
	void AddStateTransition(StateTransition transition);
	int  GetNumStateTransitions();
	std::vector<StateTransition>::iterator GetStateTransitionsBegin();
	std::vector<StateTransition>::iterator GetStateTransitionsEnd();

private:
	std::string name;

	std::vector<RegisterAccess>  registerchanges;
	std::vector<StateTransition> transitions;
};



//the actual detector class:
class XMLDetector: public DetectorBase
{
public:
	XMLDetector(std::string addressname, int address);
	XMLDetector();
	XMLDetector(const XMLDetector& templ);

    bool	StateMachineCkUp(int timestamp);
    bool	StateMachineCkDown(int timestamp);

    int 	GetState();
    int 	GetNextState();
    std::string GetCurrentStateName();

    DetectorBase* Clone();

    void AddCounter(std::string name, double value);

    void AddState(const StateMachineState& state);
    StateMachineState* GetState(int index);
    StateMachineState* GetState(std::string statename);
    int GetNumStates();
    void ClearStates();
private:
	int currentstate;
	int nextstate;
	std::vector<StateMachineState> states;
	std::map<std::string, double>  counters;

	void SetCounter(std::string name, double value);
	void IncrementCounter(std::string name, double increment = 1);
	void DecrementCounter(std::string name, double decrement = 1);
	double GetCounter(std::string name);

	void ExecuteRegisterChanges(RegisterAccess regacc, int timestamp);

	void FillComparison(Comparison* comp);
	double GetValue(RegisterAccess regacc);
};

#endif //_XMLDETECTOR