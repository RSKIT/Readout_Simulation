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

#include "detector.h"

Detector::Detector() : DetectorBase(), currentstate(PullDown), nextstate(PullDown), delay(0)
{

}


Detector::Detector(std::string addressname, int address) : DetectorBase(addressname, address),
        currentstate(PullDown), nextstate(PullDown), delay(0)
{

}

Detector::Detector(const Detector& templ) : DetectorBase(templ), currentstate(templ.currentstate),
        nextstate(templ.nextstate), delay(templ.delay)
{

}

bool Detector::StateMachineCkUp(int timestamp, bool trigger)
{
    std::cout << "State: " << GetCurrentStateName() << std::endl;

    //to pause the state machine in any state:
    if(delay > 0)
    {
        std::cout << "Delay: " << delay << std::endl;
        --delay;
        return true;
    }

    switch(currentstate)
    {
        case PullDown:
            nextstate = LdCol;
            delay = 1;
            break;

        case LdCol:
            {
                std::cout << "--> Load Column <--" << std::endl;

                bool result = false;
                for (auto &it : rocvector)
                    result |= it.LoadCell("Column", timestamp, &sbadout);

                if(result)
                    std::cout << "Hit(s) Loaded" << std::endl;

                static int counter = 0;
                if(++counter < 4)
                    nextstate = LdCol;
                else
                {
                    counter = 0;
                    nextstate = LdPix;
                    delay = 1;
                }
            }
            break;

        case LdPix:
            {
                std::cout << "--> Load Pixels <--" << std::endl;

                int hitsavailable = 0;
                bool result = false;
                for (auto &it : rocvector)
                {
                    hitsavailable += it.HitsAvailable("Column");
                    result |= it.LoadCell("Pixel", timestamp, &sbadout);
                }

                if(result)
                    std::cout << "Hit(s) found" << std::endl;

                static int counter = 0;
                if(hitsavailable > 0)
                {
                    counter = 0;
                    nextstate = RdCol;
                    delay = 1;
                }
                /*
                //the MuPix8 only checks the End-of-Column registers:
                else if(result)
                {
                    nextstate = PullDown;
                    delay = 1;
                    counter = 0;
                }
                */
                else if(++counter < 1) //changeable 6bit value
                {
                    nextstate = LdPix;
                }
                else
                {
                    counter = 0;
                    nextstate = PullDown;
                    delay = 1;
                }
            }
            break;


        case RdCol:
            {
                std::cout << "--> Read Column <--" << std::endl;

                bool result = false;
                for (auto &it: rocvector)
                {
                    if(it.LoadCell("CU", timestamp, &sbadout))
                    {
                        result = true;

                        Hit hit = it.GetHit(timestamp);  //equivalent to ReadCell()
                        if(hit.is_valid())
                        {
                            hit.AddReadoutTime(addressname, timestamp);
                            SaveHit(hit, false);
                        }
                    }
                }

                if(result)
                    std::cout << "Hit(s) read out" << std::endl;

                static int counter = 0;

                if(result)
                {
                    int morehits = 0;
                    for(auto it = rocvector.begin(); it != rocvector.end(); ++it)
                        morehits += it->HitsAvailable("Column");

                    if(morehits > 0 && counter <= 63)   //changable 6bit value
                    {
                        nextstate = RdCol;
                        ++counter;
                    }
                    else
                    {
                        nextstate = PullDown;
                        counter = 0;
                    }
                }
                else
                {
                    nextstate = PullDown;
                    counter = 0;
                }

                delay = 1;

            }
            break;
    }

    return true;
}

bool Detector::StateMachineCkDown(int timestamp, bool trigger)
{
    //write out, where hits are in the detector:
    //std::cout << "hits in the detector: " << HitsAvailable("") << std::endl
    //          << "  Pixel:  " << HitsAvailable("Pixel") << std::endl
    //          << "  Column: " << HitsAvailable("Column") << std::endl
    //          << "  CU:     " << HitsAvailable("CU") << std::endl;

    //Hit synchronisaation:
    bool result = false;
    for (auto &it : rocvector)
        result |= it.LoadPixel(timestamp, &sbadout);

    if(result)
        std::cout << "Hit(s) loaded to Pixel" << std::endl;


    currentstate = nextstate;
    std::cout << "-- State Transition --" << std::endl;
    return true;
}

int Detector::GetState()
{
    return currentstate;
}

void Detector::SetState(int index)
{
    if(index >= 0 && index < 4)
        currentstate = index;
}

int Detector::GetNextState()
{
    return nextstate;
}

std::string Detector::GetCurrentStateName()
{
    switch(currentstate)
    {
        case(PullDown):
            return "PullDown";
        case(LdPix):
            return "LdPix";
        case(LdCol):
            return "LdCol";
        case(RdCol):
            return "RdCol";
        default:
            std::stringstream s("");
            s << currentstate;
            return s.str();
    }
}

DetectorBase* Detector::Clone()
{
    return new Detector(*this);
}

int Detector::GetNumStates()
{
    return 4;
}