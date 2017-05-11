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
    //Load Data from command line arguments:
    if(argc > 1)
    {
        for(int i = 1; i < argc; ++i)
        {
            std::cout << "Loading from file \"" << argv[i] << "\" ..." << std::endl;
            Simulator sim(argv[i]);
            sim.LoadInputFile();

            sim.SimulateUntil(sim.GetStopTime(), sim.GetStopDelay());
        }
    }
    else
    {
        //try loading filenames from pipelined data:
        std::string file = "";
        std::vector<std::string> files;

        while(std::cin >> file)
            files.push_back(file);

        for(auto& it : files)
        {
            std::cout << "Loading from file \"" << it << "\" ..." << std::endl;
            Simulator sim(it);
            sim.LoadInputFile();

            sim.SimulateUntil(sim.GetStopTime(), sim.GetStopDelay());
        }

        //state that no file was provided - if this statement is true:
        if(files.size() == 0)
            std::cout << "No Parameters passed! Nothing to do here ..." << std::endl;
    }
}
