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
    if(!fout.is_open() && outputfile != "")
    {
        fout.open(outputfile.c_str(), std::ios::out | std::ios::app);
        if(!fout.is_open())
        {
            std::cout << "Could not open outputfile \"" << outputfile << "\"." << std::endl;
            return false;
        }
    }
    if(!fbadout.is_open() && badoutputfile != "")
    {
        fbadout.open(badoutputfile.c_str(), std::ios::out | std::ios::app);
        if(!fbadout.is_open())
        {
            std::cout << "Could not open outputfile \"" << badoutputfile << "\" for lost hits."
                      << std::endl;
            return false;
        }
    }

    std::cout << "State: " << GetCurrentStateName() << std::endl;

    //to pause the state machine in any state:
    if(delay > 0)
    {
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
                    result |= it.LoadCell("Column", timestamp, &fbadout);

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
                    if (it.LoadPixel(timestamp, &fbadout))
                    {
                        std::cout << it.GetAddressName() << " " << it.GetAddress() 
                                  << ": Pixel flag loaded" << std::endl;
                        result = true;
                    }
                    else
                        std::cout << it.GetAddressName() << " " << it.GetAddress() 
                                  << ": No pixel flag loaded" << std::endl;
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
                    if(it.LoadCell("CU", timestamp, &fbadout))
                    {
                        result = true;

                        Hit hit = it.GetHit();  //equivalent to ReadCell()
                        hit.AddReadoutTime(addressname, timestamp);
                        SaveHit(hit, false);
                    }
                }

                static int counter = 0;

                if(result)
                {
                    int morehits = 0;
                    for(auto it = rocvector.begin(); it != rocvector.end(); ++it)
                        morehits += it->HitsAvailable("Column");

                    if(morehits > 0 && counter >= 63)   //changable 6bit value
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
    currentstate = nextstate;
    std::cout << "-- State Transition --" << std::endl;
    return true;
}

int Detector::GetState()
{
    return currentstate;
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