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

#include "evaluation.h"


Evaluation::Evaluation() : input(std::vector<Hit>()), output(std::vector<Hit>()), global_efficiency(1.1), error_layer(std::vector<int>()), num_layer(0), error_area(std::vector<std::vector<int> >()),
error_cluster(std::vector<int>()), delay_total(std::vector<int>()), delay_totalbin(0), delay_total_avg(0), delay_layer(std::vector<std::vector<int> >()), delay_layerbin(0), delay_layer_avg(0)
{
    this->detectorsize[0]=0;
    this->detectorsize[1]=0;
    this->error_areabin[0]=0;
    this->error_areabin[1]=0;
}

bool Evaluation::Evaluate(std::string infilename, std::string failname, std::string passedname)
{
    std::fstream* infile = new std::fstream();
    std::fstream* failfile = new std::fstream();
    std::fstream* passfile = new std::fstream();

    infile->open(infilename.c_str(), std::ios::in);
    failfile->open(failname.c_str(), std::ios::in);
    passfile->open(passedname.c_str(), std::ios::in);

    int eventcounter = 0;
    int lastfailevent = -1;
    int lastpassevent = -1;

    std::vector<Hit> passedHits = std::vector<Hit>();
    std::vector<Hit> failedHits = std::vector<Hit>();
    passedHits.clear();
    failedHits.clear();



    if (failfile->good())// && passfile->good())
    {
        while (!failfile->eof())//||!passfile->eof())
        {
            std::string str = getValidLine(*(&failfile));
            if (str != "")
            {
                failedHits.push_back(Hit(str));
                std::cout << failedHits.back().GenerateString() << std::endl;
            }

            /*c

            }

            if (!failfile->eof())
                do
                {
                    failfile->getline(failline,512,'\n');
                }
                while(!isCommentLine(failline) && !failfile->eof());

            //if (!isCommentLine(line))
            {
             //   std::cout << line << std::endl;
            }*/

        }
    }

    return true;
}


double Evaluation::GetEfficiency()
{
    return 1.0;
}

int Evaluation::findChar(char line[512], char value)
{
    int i = 0;
    while (line[i] == ' ')
        ++i;
    if (line[i] == value)
        return i;
    else
        return -1;
}

bool Evaluation::isCommentLine(char line[512])
{
    if (findChar(line, '#') == -1 && findChar(line, '<') == -1)
        return false;
    else
        return true;
}

std::string Evaluation::getValidLine(std::fstream *file)
{
    char line[512];

    if (!file->eof())
    {
        do
        {
            file->getline(line,512,'\n');
        }
        while(isCommentLine(line) && !file->eof());
    }
    std::string str(line);
    return str;
}










