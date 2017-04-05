#include "detector.h"

Detector::Detector() : DetectorBase()
{

}


Detector::Detector(std::string addressname, int address) : DetectorBase(addressname, address)
{

}

Detector::Detector(const Detector& templ) : DetectorBase(templ)
{

}

void Detector::StateMachineCkUp(int timestamp)
{
    switch(currentstate)
    {
        case PullDown:

            /* generate hits & set flag1 in pixels <-- moved to main/simulator
             * */

            break;

        case LdPix:
            std::cout << "-->LdPix<--" << std::endl;
            /* iterate all pixels in all rocs
             * flag1-->flag2*/
            for (auto &it : rocvector)
                if (it.LdPix())
                    std::cout << "Pixel flag loaded" << std::endl;
                else
                    std::cout << "No pixel flag loaded" << std::endl;;

            break;

        case LdCol:
            std::cout << "-->LdCol<--" << std::endl;
            /* get hit from first pix (prio logic) of each roc to rocqueue, done
             * reset this pix' flags, done
             * delete in pix
             * set roc flag */
            for (auto &it : rocvector)
                it.LdCol();

            break;

        case RdCol:
            std::cout << "-->RdCol<--" << std::endl;
            /* if flag in roc,
             * return first data (prio logic)
             * pop data in roc,
             * check if hitvector size !=0 --> hitflag
             * */
            for (auto &it: rocvector)
            {
                Hit hit = it.RdCol();
                //std::cout << hit.GenerateString()<< std::endl;

                if (hit.GetEventIndex() == -1)
                    std::cout << "NO HIT FOUND!!" << std::endl;
                else
                {
                    std::cout << "HIT FOUND: " << hit.GenerateString() << std::endl;
                    hit.AddReadoutTime(addressname, timestamp);
                    SaveHit(hit, false);
                }

            }

            break;
    }
    NextState();
}

void Detector::StateMachineCkDown(int timestamp)
{
    
}

int Detector::NextState()
{
    switch(currentstate)
    {
        case PullDown   : return currentstate = LdPix;
        case LdPix      : return currentstate = LdCol;
        case LdCol      : return currentstate = RdCol;
        case RdCol      : return currentstate = PullDown;
    }
}
