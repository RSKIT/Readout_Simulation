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

void Detector::StateMachineCkUp(int timestamp)
{
    if(!fout.is_open())
    {
        fout.open(outputfile.c_str(), std::ios::out | std::ios::app);
        if(!fout.is_open())
        {
            std::cout << "Could not open outputfile \"" << outputfile << "\"." << std::endl;
            return;
        }
    }

    //to pause the state machine in any state:
    if(delay > 0)
    {
        --delay;
        return;
    }

    switch(currentstate)
    {
        case PullDown:
            nextstate = LdPix;
            break;

        case LdPix:
            {
            std::cout << "--> Load Pixels <--" << std::endl;

            bool result = false;
            for (auto &it : rocvector)
            {
                if (it.LoadPixel(timestamp, &fbadout))
                {
                    std::cout << it.GetAddressName() << " " << it.GetAddress() 
                              << "Pixel flag loaded" << std::endl;
                    result = true;
                }
                else
                    std::cout << it.GetAddressName() << " " << it.GetAddress() 
                              << "No pixel flag loaded" << std::endl;;
            }

                if(result)
                    nextstate = LdCol;
                else
                    nextstate = PullDown;
            }
            break;

        case LdCol:
            {
            std::cout << "--> Load Column <--" << std::endl;

            for (auto &it : rocvector)
                it.LoadCell("Column", timestamp, &fbadout);
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

                    Hit hit = it.GetHit();
                    hit.AddReadoutTime(addressname, timestamp);
                    SaveHit(hit, false);
                }
            }

            if(result)
            {
                int morehits = 0;
                for(auto it = rocvector.begin(); it != rocvector.end(); ++it)
                    morehits += it->HitsAvailable("Column");

                if(morehits > 0)
                {
                    nextstate = RdCol;
                    delay = 7;
                }
                else
                    nextstate = PullDown;
            }
            else
                nextstate = PullDown;

            }
            break;
    }
}

void Detector::StateMachineCkDown(int timestamp)
{
    currentstate = nextstate;
}

int Detector::GetState()
{
    return currentstate;
}

int Detector::GetNextState()
{
    return nextstate;
}
