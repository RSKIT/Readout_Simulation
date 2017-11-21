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
#include <fstream>

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

bool WriteToLog(std::string text, bool close = true)
{
    std::cout << text;
    
    std::fstream fgenerallog;

    fgenerallog.open("ROMEprogress.log", std::ios::out | std::ios::app);

    if(fgenerallog.is_open())
    {
        fgenerallog << text;
        if(close)
            fgenerallog.close();
        return true;
    }
    else
        return false;
}


int main(int argc, char** argv)
{
    std::cout << std::endl
              << "    *****************************************\n"
              << "    *                                       *\n"
              << "    *      ROME Simulation Framework        *\n"
              << "    *        developed at KIT-ADL           *\n"
              << "    * by Rudolf Schimassek and Felix Ehrler *\n"
              << "    *                                       *\n"
              << "    *   Version: 0.9.8-beta (21.11.2017)    *\n"
              << "    *                                       *\n"
              << "    *****************************************\n" 
              << std::endl;

    //Data Container for the file names:
    std::vector<std::string> files;

    //starting output preparation:
    std::stringstream s("");
    s << "[" << GetDateTime() << "] Called with ";

    std::string file = "";

    //Load Data from command line arguments:
    for(int i = 1; i < argc; ++i)
    {
        file = argv[i];
        std::fstream f;
        f.open(file.c_str(), std::ios::in);
        if(f.is_open())
        {
            files.push_back(file);
            f.close();
        }
        else
            std::cout << "Could not open file \"" << file << "\"." << std::endl;
    }

    //try loading filenames from pipelined data:
    
    if(argc == 1)    //only accept piped arguments if no parameters are passed
    {
        while(std::cin >> file)
        {
            if(file != "")
            {
                std::fstream f;
                f.open(file.c_str(), std::ios::in);
                if(f.is_open())
                {
                    files.push_back(file);
                    f.close();
                }
                else
                    std::cout << "Could not open file \"" << file << "\"." << std::endl;

            }
            else
                break;
        }
    }

    //write out starting output:
    s << files.size() << " arguments" << std::endl;
    WriteToLog(s.str());


    //=== Run the simulations ===

    int i = 1;  //index for the simulation files
    for(auto& it : files)
    {
        std::string now = GetDateTime();
        std::stringstream s("");
        s << "[" << now << "] Starting Simulation " << i << "/" << files.size() 
                << ": \"" << it << "\"" << std::endl;
        WriteToLog(s.str());

        Simulator sim(it);
        
        int subsimulation = 0;
        do{
            sim.LoadInputFile();
            while(subsimulation < sim.GetFirstSubSimIndex())
            {
                ++subsimulation;
                sim.GoToNextParameterSetting();
            }

            now = GetDateTime();
            std::stringstream s("");
            s << "[" << now << "]   Starting sub-simulation " << ++subsimulation
                        << "/" << sim.GetNumParameterSettings() << std::endl;
            WriteToLog(s.str());

            sim.SimulateUntil(sim.GetStopTime(), sim.GetStopDelay());

            sim.Cleanup();
        }while(sim.GoToNextParameterSetting() && subsimulation <= sim.GetLastSubSimIndex());

        sim.ClearScanParameters();

        ++i;
    }

    //state that no file was provided - if this statement is true:
    if(files.size() == 0)
        WriteToLog(std::string("[") + GetDateTime() 
                        + "] No Parameters passed! Nothing to do here ...\n");



    std::string now = GetDateTime();
    std::stringstream s2("");
    s2 << "[" << now << "] Simulation(s) finished" << std::endl;
    WriteToLog(s2.str());

    return 0;
}
