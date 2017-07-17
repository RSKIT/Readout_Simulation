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
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>

#include "hit.h"
#include "TCoord.h"
#include "pixel.h"
#include "readoutcell.h"
#include "detector.h"
#include "EventGenerator.h"
#include "simulator.h"

std::string GetDateTime()
{
    std::time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char buffer[80];
    strftime(buffer, 80,"%e.%m.%y %X", localtime(&tt));

    return std::string(buffer);
}


int main(int argc, char** argv)
{
    std::fstream fgenerallog;
    fgenerallog.open("ROMEprogress.log", std::ios::out | std::ios::app);

    bool writelog = fgenerallog.is_open();

    if(writelog)
    {
        //std::time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        fgenerallog << "Call at: " << GetDateTime() << " with ";
    }

    //Load Data from command line arguments:
    if(argc > 1)
    {
        fgenerallog << (argc-1) << " arguments" << std::endl;
        for(int i = 1; i < argc; ++i)
        {
            //std::time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            //char buffer[80];
            //strftime(buffer, 80,"%e.%m.%y %X", localtime(&tt));
            std::string now = GetDateTime();
            if(writelog)
            {
                fgenerallog << "[" << now << "] Starting Simulation " 
                            << i << "/" << (argc-1) << ": \"" << argv[i] << "\"" << std::endl;
            }
            std::cout << "[" << now << "] Loading from file \"" << argv[i] 
                        << "\" ..." << std::endl;

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

        fgenerallog << files.size() << " arguments" << std::endl;

        int i = 1;  //index for the simulation files
        for(auto& it : files)
        {
            //std::time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::string now = GetDateTime();
            if(writelog)
            {
                fgenerallog << "[" << now << "] Starting Simulation " 
                            << i << "/" << files.size() << ": \"" << it << "\"" << std::endl;
            }
            std::cout << "[" << now << "] Loading from file \"" << it 
                        << "\" ..." << std::endl;

            Simulator sim(it);
            sim.LoadInputFile();

            sim.SimulateUntil(sim.GetStopTime(), sim.GetStopDelay());

            ++i;
        }

        //state that no file was provided - if this statement is true:
        if(files.size() == 0)
        {
            if(writelog)
                fgenerallog << "No Parameters passed! Nothing to do here ..." << std::endl;
            std::cout << "No Parameters passed! Nothing to do here ..." << std::endl;
        }
    }

    //std::time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string now = GetDateTime();
    if(writelog)
        fgenerallog << "[" << now << "] Simulation(s) finished" << std::endl;
    std::cout << "[" << now << "] Simulation(s) finished" << std::endl;

    fgenerallog.close();

    return 0;
}
