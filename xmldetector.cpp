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

#include "xmldetector.h"

Comparison::Comparison(int relation) : firstchoice(Value), secondchoice(Value), firstval(0),
		secondval(0), firstreg(RegisterAccess()), secondreg(RegisterAccess()), firstcomp(NULL),
		secondcomp(NULL), firstregset(false), secondregset(false), relation(0)
{
	this->relation = relation;
}

Comparison::Comparison(const Comparison& comp) : firstchoice(comp.firstchoice), 
		secondchoice(comp.secondchoice), firstval(comp.firstval), secondval(comp.secondval),
		firstreg(comp.firstreg), secondreg(comp.secondreg),	firstregset(false), 
		secondregset(false), relation(comp.relation)
{
	if(comp.firstcomp != NULL)
		firstcomp = new Comparison(*(comp.firstcomp));
	else
		firstcomp = NULL;

	if(comp.secondcomp != NULL)
		secondcomp = new Comparison(*(comp.secondcomp));
	else
		secondcomp = NULL;
}

Comparison::~Comparison()
{
	if(firstcomp != 0)
	{
		delete firstcomp;
		firstcomp = NULL;
	}
	if(secondcomp != 0)
	{
		delete secondcomp;
		secondcomp = NULL;
	}
}

Comparison& Comparison::operator=(const Comparison& comp)
{
	relation = comp.relation;

	firstchoice = comp.firstchoice;
	secondchoice = comp.secondchoice;

	firstval = comp.firstval;
	secondval = comp.secondval;

	firstreg = comp.firstreg;
	secondreg = comp.secondreg;

	firstregset = false;
	secondregset = false;

	if(comp.firstcomp != NULL)
		firstcomp = new Comparison(*comp.firstcomp);
	else
		firstcomp = NULL;
	if(comp.secondcomp != NULL)
		secondcomp = new Comparison(*comp.secondcomp);

	return *this;
}

int  Comparison::GetRelation()
{
	return relation;
}

void Comparison::SetRelation(int operand)
{
	relation = operand;
}

int  Comparison::GetFirstChoice()
{
	return firstchoice;
}

bool Comparison::SetFirstChoice(int choice)
{
	if(choice >= 0 && choice < 4)
	{
		firstchoice = choice;
		firstregset = false;
		return true;
	}
	else
		return false;
}

int  Comparison::GetSecondChoice()
{
	return secondchoice;
}

bool Comparison::SetSecondChoice(int choice)
{
	if(choice >= 0 && choice < 4)
	{
		secondchoice = choice;
		secondregset = false;
		return true;
	}
	else
		return false;
}

Comparison* Comparison::GetFirstComparison()
{
	return firstcomp;
}

void Comparison::SetFirstComparison(const Comparison& comp)
{
	if(firstcomp != NULL)
		delete firstcomp;
	firstcomp = new Comparison(comp);
	firstchoice = Comp;
	firstregset = false;
}

void Comparison::SetFirstComparison(Comparison* comp)
{
	//prevent eternal recursion until crash:
	if(this == comp)
		return;
	if(firstcomp != NULL)
		delete firstcomp;
	firstcomp = comp;
	firstchoice = Comp;
	firstregset = false;
}

Comparison* Comparison::GetSecondComparison()
{
	return secondcomp;
}

void Comparison::SetSecondComparison(const Comparison& comp)
{
	//prevent eternal recursion until crash:
	if(this == &comp)
		return;
	if(secondcomp != NULL)
		delete secondcomp;
	secondcomp = new Comparison(comp);
	secondchoice = Comp;
	secondregset = false;
}

void Comparison::SetSecondComparison(Comparison* comp)
{
	if(secondcomp != NULL)
		delete secondcomp;
	secondcomp = comp;
	secondchoice = Comp;
	secondregset = false;
}

double Comparison::GetFirstValue()
{
	return firstval;
}

void   Comparison::SetFirstValue(double value)
{
	firstval = value;
	firstchoice = Value;
}

double Comparison::GetSecondValue()
{
	return secondval;
}

void   Comparison::SetSecondValue(double value)
{
	secondval = value;
	secondchoice = Value;
}

RegisterAccess Comparison::GetFirstRegisterAccess()
{
	return firstreg;
}

void   Comparison::SetFirstRegisterAccess(const RegisterAccess& regacc)
{
	firstreg = regacc;
	firstchoice = Register;
	firstregset = false;
}

RegisterAccess Comparison::GetSecondRegisterAccess()
{
	return secondreg;
}

void   Comparison::SetSecondRegisterAccess(const RegisterAccess& regacc)
{
	secondreg = regacc;
	secondchoice = Register;
	secondregset = false;
}

void Comparison::EvalFirstRegister(double value)
{
	firstregset = true;
	if(firstchoice != Value)
		firstval = value;
}

void Comparison::EvalSecondRegister(double value)
{
	secondregset = true;
	if(secondchoice != Value)
		secondval = value;
}

bool Comparison::ReadyForEvaluation()
{
	switch(firstchoice)
	{
		case(Comp):
			if(firstcomp == NULL)
				return false;
			else if(!firstcomp->ReadyForEvaluation())
				return false;
			break;
		case(Value):
			break;
		case(Register):
			if(!firstregset)
				return false;
			break;
		default:
			return false;
	}

	switch(secondchoice)
	{
		case(Comp):
			if(secondcomp == NULL)
				return false;
			else if(!secondcomp->ReadyForEvaluation())
				return false;
			break;
		case(Value):
			break;
		case(Register):
			if(!secondregset)
				return false;
			break;
		default:
			return false;
	}

	return true;
}

bool Comparison::Evaluate()
{
	if(!ReadyForEvaluation())
		return false;

	if(firstchoice == Comp)
		firstval = (firstcomp->Evaluate())?1:0;

	if(secondchoice == Comp)
		secondval = (secondcomp->Evaluate())?1:0;

	firstregset  = false;
	secondregset = false;

	switch(relation)
	{
		case(Smaller):
			return firstval  < secondval;
		case(SmallerEqual):
			return firstval <= secondval;
		case(Larger):
			return firstval  > secondval;
		case(LargerEqual):
			return firstval >= secondval;
		case(Equal):
			return firstval == secondval;
		case(NotEqual):
			return firstval != secondval;
		case(Or):
			return (firstval != 0) || (secondval != 0);
		case(And):
			return (firstval != 0) && (secondval != 0);
		case(Xor):
			return (firstval != 0) ^ (secondval != 0);
		default:
			return false;
	}
}

std::string Comparison::PrintComparison(std::string spaces) const
{
	std::stringstream s("");

	s << spaces << "Comparison: (relation: " << relation << "):\n"
	  << spaces << " Lvalue: " << firstchoice << "\n" << spaces;

	s << "  value:  " << firstval << std::endl;

	s << spaces  << "  RegAcc: " << firstreg.what << std::endl;

	if(firstcomp != NULL)
		s << firstcomp->PrintComparison(spaces + "  ");

	s << spaces << " Rvalue: " << secondchoice << "\n" << spaces;

	s << "  value:  " << secondval << std::endl << spaces
	  << "  RegAcc: " << secondreg.what << std::endl;
	if(secondcomp != NULL)
		s << secondcomp->PrintComparison(spaces + "  ");

	return s.str();
}

/*****************************
 *  StateTransition Methods  *
 *****************************/

StateTransition::StateTransition() : nextstate(""), delay(0), 
		counterchanges(std::vector<RegisterAccess>()), condition(new Comparison())
{

}

StateTransition::StateTransition(const StateTransition& trans) : nextstate(trans.nextstate),
		delay(trans.delay)
{
	for(auto& it : trans.counterchanges)
		counterchanges.push_back(it);

	condition = new Comparison(*(trans.condition));
}

std::string StateTransition::GetNextState()
{
	return nextstate;
}

void StateTransition::SetNextState(std::string nextstate)
{
	this->nextstate = nextstate;
}

int StateTransition::GetDelay()
{
	return delay;
}

void StateTransition::SetDelay(int delay)
{
	this->delay = delay;
}

Comparison* StateTransition::GetComparison()
{
	return condition;
}

void StateTransition::SetComparison(const Comparison& comp)
{
	condition = new Comparison(comp);
}

void StateTransition::SetComparison(Comparison* comp)
{
	condition = comp;
}

void StateTransition::ClearRegisterChanges()
{
	counterchanges.clear();
}

void StateTransition::AddRegisterChange(RegisterAccess change)
{
	counterchanges.push_back(change);
}

int  StateTransition::GetNumRegisterChanges()
{
	return counterchanges.size();
}

/*const*/ std::vector<RegisterAccess>::iterator StateTransition::GetRegisterChangesBegin()
{
	return counterchanges.begin();
}

/*const*/ std::vector<RegisterAccess>::iterator StateTransition::GetRegisterChangesEnd()
{
	return counterchanges.end();
}

bool StateTransition::Evaluate()
{
	return condition->Evaluate();
}


/*******************************
 *  StateMachineState Methods  *
 *******************************/

StateMachineState::StateMachineState() : name(""), registerchanges(std::vector<RegisterAccess>()),
		transitions(std::vector<StateTransition*>())
{

}

std::string StateMachineState::GetStateName()
{
	return name;
}

void  StateMachineState::SetStateName(std::string name)
{
	this->name = name;
}

void StateMachineState::ClearRegisterChanges()
{
	registerchanges.clear();
}

void StateMachineState::AddRegisterChange(RegisterAccess change)
{
	registerchanges.push_back(change);
}

int  StateMachineState::GetNumRegisterChanges()
{
	return registerchanges.size();
}

/*const*/ std::vector<RegisterAccess>::iterator StateMachineState::GetRegisterChangesBegin()
{
	return registerchanges.begin();
}

/*const*/ std::vector<RegisterAccess>::iterator StateMachineState::GetRegisterChangesEnd()
{
	return registerchanges.end();
}

void StateMachineState::ClearStateTransitions()
{
	transitions.clear();
}

void StateMachineState::AddStateTransition(StateTransition transition)
{
	transitions.push_back(new StateTransition(transition));
}

void StateMachineState::AddStateTransition(StateTransition* transition)
{
	transitions.push_back(transition);
}

int  StateMachineState::GetNumStateTransitions()
{
	return transitions.size();
}

/*const*/ std::vector<StateTransition*>::iterator StateMachineState::GetStateTransitionsBegin()
{
	return transitions.begin();
}

/*const*/ std::vector<StateTransition*>::iterator StateMachineState::GetStateTransitionsEnd()
{
	return transitions.end();
}

/*************************
 *  XMLDetector Methods  *
 *************************/

XMLDetector::XMLDetector(std::string addressname, int address)
		: DetectorBase(addressname, address), currentstate(0), nextstate(-1),
		states(std::vector<StateMachineState*>()), counters(std::map<std::string, double>())
{
	counters.insert(std::make_pair("delay", 0));
}

XMLDetector::XMLDetector() : DetectorBase(), currentstate(0), nextstate(-1),
		states(std::vector<StateMachineState*>()), counters(std::map<std::string, double>())
{
	counters.insert(std::make_pair("delay", 0));
}

XMLDetector::XMLDetector(const XMLDetector& templ) : DetectorBase(templ), 
		currentstate(templ.currentstate), nextstate(templ.nextstate), counters(templ.counters)
{
	for(auto& it : templ.states)
		states.push_back(new StateMachineState(*it));
}

bool XMLDetector::StateMachineCkUp(int timestamp, bool trigger)
{
	//check for valid state:
	if(currentstate >= states.size() || currentstate < 0)
	{
		std::cout << "State Machine Error" << std::endl;
		return false;
	}

    //do not execute anything when a delay is active:
    if(GetCounter("delay") > 0)
    {
    	std::cout << "Delay: " << GetCounter("delay") << std::endl;
    	DecrementCounter("delay");
    	return true;
    }

	StateMachineState* state = states[currentstate];


	std::cout << "State: " << state->GetStateName() << std::endl;

	//change Registers, LoadPixels, LoadROCs,...:
	auto itend = state->GetRegisterChangesEnd();
	for(auto it = state->GetRegisterChangesBegin(); it != itend; ++it)
		ExecuteRegisterChanges(*it, timestamp);

	//state transitions:
	//nextstate = -1;

	auto endtransitions = state->GetStateTransitionsEnd();
	for(auto it = state->GetStateTransitionsBegin(); it != endtransitions; ++it)
	{
		FillComparison((*it)->GetComparison());

		if((*it)->Evaluate())
		{
			SetCounter("delay", (*it)->GetDelay());

			auto itend = (*it)->GetRegisterChangesEnd();
			for(auto regit = (*it)->GetRegisterChangesBegin(); regit != itend; ++regit)
				ExecuteRegisterChanges(*regit, timestamp);

			//find the next state:
			int index = 0;
			for(auto stateit = states.begin(); stateit != states.end(); ++stateit)
			{
				if((*stateit)->GetStateName().compare((*it)->GetNextState()) == 0)
				{
					nextstate = index;
					break;
				}
				++index;
			}

			break;
		}
	}

	if(nextstate == -1)
	{
		std::cout << "Debug Output for all Transitions:" << std::endl;
		for(auto it = state->GetStateTransitionsBegin(); it != endtransitions; ++it)
		{
			std::cout << "Transition to: " << (*it)->GetNextState() << std::endl;
			std::cout << (*it)->GetComparison()->PrintComparison("  ");
		}

	}

	return true;
}

bool XMLDetector::StateMachineCkDown(int timestamp, bool trigger)
{
	if(!trigger)
	{
		//std::cout << "remove hits (missing trigger)" << std::endl;
		for(auto it = rocvector.begin(); it != rocvector.end(); ++it)
			it->NoTriggerRemoveHits(timestamp, &sbadout);
	}

	//execute special actions for making signals synchronous if they are defined:
	StateMachineState* state = GetState("synchronisation");
	if(state != 0)
	{
		//change Registers, LoadPixels, LoadROCs,...:
		auto itend = state->GetRegisterChangesEnd();
		for(auto it = state->GetRegisterChangesBegin(); it != itend; ++it)
			ExecuteRegisterChanges(*it, timestamp);
	}

	//execute a state transition for the "normal" state machine part:
	//do not execute a state change on a delay:
    if(GetCounter("delay") > 0)
    	return true;

    currentstate = nextstate;
    nextstate = -1;
    std::cout << "-- State Transition --" << std::endl;

    return true;
}

int XMLDetector::GetState()
{
	return currentstate;
}

void XMLDetector::SetState(int index)
{
	if(index >= 0 && index < states.size())
		currentstate = index;
}

int XMLDetector::GetNextState()
{
	return nextstate;
}

std::string XMLDetector::GetCurrentStateName()
{
	if(currentstate < 0 || currentstate >= states.size())
		return "";
	else
		return states[currentstate]->GetStateName();
}

DetectorBase* XMLDetector::Clone()
{
	return new XMLDetector(*this);
}

void XMLDetector::AddCounter(std::string name, double value)
{
	SetCounter(name, value);
}

void XMLDetector::AddState(const StateMachineState& state)
{
	states.push_back(new StateMachineState(state));
}

void XMLDetector::AddState(StateMachineState* state)
{
	states.push_back(state);
}

StateMachineState* XMLDetector::GetState(int index)
{
	if(index < 0 || index >= states.size())
		return 0;
	else
		return states[index];
}

StateMachineState* XMLDetector::GetState(std::string statename)
{
	for(auto& it : states)
	{
		if(it->GetStateName().compare(statename) == 0)
			return it;
	}
	return 0;
}

int XMLDetector::GetNumStates()
{
	return states.size();
}

void XMLDetector::ClearStates()
{
	states.clear();
}


void XMLDetector::SetCounter(std::string name, double value)
{
	auto it = counters.find(name);
	if(it == counters.end())
		counters.insert(std::make_pair(name, value));
	else
		it->second = value;
}

void XMLDetector::IncrementCounter(std::string name, double increment)
{
	auto it = counters.find(name);
	if(it == counters.end())
		counters.insert(std::make_pair(name, increment));
	else
		it->second += increment;
}

void XMLDetector::DecrementCounter(std::string name, double decrement)
{
	auto it = counters.find(name);
	if(it == counters.end())
		counters.insert(std::make_pair(name, -decrement));
	else
		it->second -= decrement;
}

double XMLDetector::GetCounter(std::string name)
{
	auto it = counters.find(name);
	if(it == counters.end())
		return 0;
	else
		return it->second;
}

void XMLDetector::ExecuteRegisterChanges(RegisterAccess regacc, int timestamp)
{
	if(regacc.what.compare("cout") == 0)
	{
		if(regacc.value != 0)
			std::cout << regacc.parameter << std::endl;
		else
			std::cout << regacc.parameter;
	}
	else if(regacc.what.compare("printhitsavailable") == 0)
	{
		if(regacc.value != 0)
			std::cout << HitsAvailable(regacc.parameter) << std::endl;
		else
			std::cout << HitsAvailable(regacc.parameter);
	}
	else if(regacc.what.compare("printcounter") == 0)
	{
		if(regacc.value != 0)
			std::cout << GetCounter(regacc.parameter) << std::endl;
		else
			std::cout << GetCounter(regacc.parameter);
	}
	else if(regacc.what.compare("setcounter") == 0)
		SetCounter(regacc.parameter, regacc.value);
	else if(regacc.what.compare("incrementcounter") == 0)
		IncrementCounter(regacc.parameter, regacc.value);
	else if(regacc.what.compare("decrementcounter") == 0)
		DecrementCounter(regacc.parameter, regacc.value);

	else if(regacc.what.compare("loadpixel") == 0)
	{
		bool result = false;
		for(auto& it : rocvector)
			result |= it.LoadPixel(timestamp, &sbadout);

		SetCounter("loadpixel", (result)?1:0);
	}
	else if(regacc.what.compare("loadcell") == 0)
	{
		bool result = false;
		for(auto& it : rocvector)
			result |= it.LoadCell(regacc.parameter, timestamp, &sbadout);

		SetCounter("loadcell_" + regacc.parameter, (result)?1:0);
	}
	else if(regacc.what.compare("readcell") == 0)
	{
	    bool result = false;
        for (auto &it: rocvector)
        {
            Hit hit = it.GetHit(timestamp);  //equivalent to ReadCell()
            if(hit.is_valid())
            {
            	hit.AddReadoutTime(addressname, timestamp);
            	SaveHit(hit, false);
            	IncrementCounter("readhits");
            	result = true;
            }
        }

        SetCounter("readcell", (result)?1:0);
	}

}

void XMLDetector::FillComparison(Comparison* comp)
{
	if(comp == 0)
		return;

	double value;
	switch(comp->GetFirstChoice())
	{
		case(Comparison::Comp):
			FillComparison(comp->GetFirstComparison());
			break;
		case(Comparison::Register):
			value = GetValue(comp->GetFirstRegisterAccess());
			comp->EvalFirstRegister(value);
			break;
		case(Comparison::Value):
			comp->EvalFirstRegister(-1);
			break;
		default:
			break;
	}
	
	switch(comp->GetSecondChoice())
	{
		case(Comparison::Comp):
			FillComparison(comp->GetSecondComparison());
			break;
		case(Comparison::Register):
			value = GetValue(comp->GetSecondRegisterAccess());
			comp->EvalSecondRegister(value);
			break;
		case(Comparison::Value):
			comp->EvalSecondRegister(-1);
			break;
		default:
			break;
	}



	return;
}

double XMLDetector::GetValue(RegisterAccess regacc)
{
	if(regacc.what.compare("getcountervalue") == 0)
		return GetCounter(regacc.parameter);
	else if(regacc.what.compare("hitsavailable") == 0)
	{
		int hits = 0;
		for(auto& it : rocvector)
			hits += it.HitsAvailable(regacc.parameter);
		return hits;
	}
}