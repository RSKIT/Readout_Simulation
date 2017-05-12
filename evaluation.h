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

#ifndef _EVALUATION
#define _EVALUATION

#include <vector>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <vector>

#include "hit.cpp"

class Evaluation
{
public:
    Evaluation();

    bool                Evaluate(std::string infilename, std::string failname, std::string passedname);

    double              GetEfficiency();

    /*std::vector<int>    GetErrorPerLayer();
    std::vector<int>    GetErrorPerLayer(int layer);

    std::vector<std::vector<int> > GetErrorPerArea();
    std::vector<std::vector<int> > GetErrorPerArea(double xmin, double xmax, double ymin, double ymax);

    std::vector<int>    GetErrorCluster();

    std::vector<int>    GetDelay();

    std::vector<std::vector<int> > GetDelayLayer();*/


private:
    std::vector<Hit>    input;
    std::vector<Hit>    output;

    double              global_efficiency;

    std::vector<int>    error_layer;
    int                 num_layer;


    std::vector<std::vector<int> > error_area;
    double              detectorsize[2];
    double              error_areabin[2];

    std::vector<int>    error_cluster;

    std::vector<int>    delay_total;
    double              delay_totalbin;
    double              delay_total_avg;

    std::vector<std::vector<int> > delay_layer;
    double              delay_layerbin;
    double              delay_layer_avg;

    int                 findChar(char line[512], char value);
    bool                isCommentLine(char line[512]);
    std::string         getValidLine(std::fstream* file);


};


#endif  //_EVALUATION
