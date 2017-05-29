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

#include "TGraph.h"
#include "TH1.h"
#include "TH1I.h"
#include "TH2.h"
#include "TH2I.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TPaletteAxis.h"
#include "TList.h"

#include "hit.cpp"

class Evaluation
{
public:
    enum source {Input = 0, Pass = 1, Fail = 2};

    Evaluation();

    int     LoadInputHits(std::string filename);
    int     LoadPassedOutputHits(std::string filename);
    int     LoadFailedOutputHits(std::string filename);

    Hit     GetHit(unsigned int index, int input = Pass);
    int     InsertHits(Hit hit, int input = Pass);
    int     InsertHits(std::vector<Hit>& hits, int input = Pass);
    void    ClearHits(int input = Pass);

    TGraph* GenerateScatterplot(std::string xaxis, std::string yaxis, int input = Pass);

    TH1*    GenerateHistogram(std::string value,
                                    double start, double end, double binwidth, int input = Pass);

    TH1*    GenerateDelayHistogram(std::string firsttime, std::string secondtime,
                                    double start, double end, double binwidth, int input = Pass);

    TGraph* GenerateIntegrationCurve(TH1* histogram);

    TCanvas* Plot(TGraph* graph, std::string xtitle, std::string ytitle, 
                                    std::string options = "AP*");
    TCanvas* Plot(TH1* histogram, std::string xtitle, std::string ytitle, 
                                    std::string options = "");
    TCanvas* Plot(TH2* histogram, std::string xtitle, std::string ytitle, std::string ztitle, 
                    std::string options = "colz");

    std::vector<Hit> SeparateHit(std::map<int, int>& encoding, Hit& hit, 
                                    std::string addrname = "PixelDiode");
    int              SeparateHits(std::map<int, int>& encoding, std::string addrname = "PixelDiode",
                                    int input = Pass);
private:
    int LoadHits(std::vector<Hit>* vec, std::string filename);
    std::vector<Hit>*   GetVectorPointer(int input);

    double GetDoubleValue(Hit& hit, std::string value);
    int    GetIntValue(Hit& hit, std::string value);

    std::vector<Hit>    input;
    std::vector<Hit>    outputpass;
    std::vector<Hit>    outputfail;

};


#endif  //_EVALUATION
