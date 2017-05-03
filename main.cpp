#include <iostream>
#include <string>

#include "hit.h"
#include "TCoord.h"
#include "pixel.h"
#include "readoutcell.h"
#include "detector.h"
#include "EventGenerator.h"
#include "simulator.h"

const int pixsizex = 50;
const int pixsizey = 150;
const int pixsizez = 30;

int main(int argc, char** argv)
{
    if(argc == 2)
    {
        std::cout << "Loading from file \"" << argv[1] << "\" ..." << std::endl;
        Simulator sim(argv[1]);
        sim.LoadInputFile();

        //std::cout << "Detectors: " << sim.GetNumDetectors() << std::endl;
        //std::cout << sim.PrintDetectors() << std::endl;

        //sim.InitEventGenerator();

        sim.SimulateUntil(sim.GetStopTime(), sim.GetStopDelay());
    }
    else
    {
        std::cout << "Wrong number of Parameters!" << std::endl;
    }
}
