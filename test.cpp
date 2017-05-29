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
#include <map>
#include <utility>

#include "evaluation.cpp"

int test()
{
    std::cout << "test" << std::endl;
    Evaluation asd = Evaluation();
    
    std::map<int, int> encoding;

    for(int i=0;i<12;++i)
        encoding.insert(std::make_pair(i,1<<i));

    for(int i = 20; i < 55; i += 5)
    {
        asd.ClearHits();
        std::stringstream s("");
        s << i << "_12_1_readhits.dat";
        asd.LoadPassedOutputHits(s.str());
        asd.SeparateHits(encoding);

        TH1* hist = asd.GenerateDelayHistogram("timestamp","Detector", -100,15000,10);
        TGraph* graph = asd.GenerateIntegrationCurve(hist);

        TCanvas* c1 = asd.Plot(hist, "Readout Delay (in Timestamps)", "Counts", "");
        TCanvas* c2 = asd.Plot(graph, "Readout Delay (in Timestamps)", 
                                "Integrated Fraction of Hits", "AP*");

        s.str("");
        s << i << "_12_1_ReadoutDelay.svg";
        c1->SaveAs(s.str().c_str());
        s.str("");
        s << i << "_12_1_IntegratedFraction.svg";
        c2->SaveAs(s.str().c_str());
    }

    /*
    asd.LoadInputHits("25_4_3_eventgen.dat");
    asd.LoadPassedOutputHits("25_4_3_readhits.dat");
    asd.LoadFailedOutputHits("25_4_3_losthits.dat");

    TH1* hist = asd.GenerateDelayHistogram("timestamp", "Detector", 0, 10000, 10);
    TGraph* graph = asd.GenerateIntegrationCurve(hist);

    asd.Plot(hist, "Readout Delay (in Timestamps)", "Counts", "");
    asd.Plot(graph, "Readout Delay (in Timestamps)", "Integrated Fraction of Hits", "AP*");
    */

    std::cout << "test" << std::endl;
    return 0;
}
