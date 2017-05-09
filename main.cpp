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
