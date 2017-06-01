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

    /**
     * @brief loads data from the EventGenerator's output
     * @details
     * 
     * @param filename       - the file to load from
     * @return               - the number of Hit objects loaded
     */
    int     LoadInputHits(std::string filename);
    /**
     * @brief loads data for successfully read out hits from a file
     * @details
     * 
     * @param filename       - the file to load from
     * @return               - the number of Hit objects loaded
     */
    int     LoadPassedOutputHits(std::string filename);
    /**
     * @brief loads data for Hits lost during the simulation
     * @details
     * 
     * @param filename       - the file to load from
     * @return               - the number of Hit object loaded
     */
    int     LoadFailedOutputHits(std::string filename);

    /**
     * @brief provides a Hit object from one of the three sources (EventGenerator, Read Hits, 
     *             Lost Hits).
     * @details
     * 
     * @param index          - the index of the Hit object in the vector
     * @param input          - the source of the Hit:
     *                            Input = Hits generated by the EventGenerator
     *                            Pass  = successfully read out hits
     *                            Fail  = lost hits
     * 
     * @return               - the hit object requested or an invalid hit for an index out of range
     */
    Hit     GetHit(unsigned int index, int input = Pass);
    /**
     * @brief adds a Hit object to the evaluation structure
     * @details
     * 
     * @param hit            - the Hit object to add
     * @param input          - sets whether it is a Pass, Fail or Input hit
     * 
     * @return               - the number of hits added (in this case 1)
     */
    int     InsertHits(Hit hit, int input = Pass);
    /**
     * @brief adds several hits to the evaluation structure, but all to the same category
     * @details
     * 
     * @param hits           - the Hit objects to add
     * @param input          - the category the Hits belong to (Pass, Fail, Input)
     * 
     * @return               - the number of Hit objects added (size of the vector)
     */
    int     InsertHits(std::vector<Hit>& hits, int input = Pass);
    /**
     * @brief removes all hits from the specified category
     * @details
     * 
     * @param input          - the category of hits to remove (Pass, Fail, Input)
     */
    void    ClearHits(int input = Pass);

    /**
     * @brief generates a scatter plot from the data provided (for options for xaxis and yaxis,
     *             see description of GetDoubleValue())
     * @details
     * 
     * @param xaxis          - the field of the Hit objects to use as x-coordinate
     * @param yaxis          - the field of the Hit objects to use as y-coordinate
     * @param input          - the category of data to use for the plot (Pass, Fail, Input)
     * @return               - the resulting TGraph (unformated)
     */
    TGraph* GenerateScatterplot(std::string xaxis, std::string yaxis, int input = Pass);

    /**
     * @brief generates a histogram from a property of the hit objects (for options
     *             see description of GetDoubleValue())
     * @details
     * 
     * @param value          - the property of the Hit objects to use
     * @param start          - the lower edge of the first bin
     * @param end            - the upper edge of the last bin
     * @param binwidth       - width of a single bin
     * @param input          - the category of data to use (Pass, Fail, Input)
     * @return               - the resulting histogram
     */
    TH1*    GenerateHistogram(std::string value,
                                    double start, double end, double binwidth, int input = Pass);

    /**
     * @brief a special histogram allowing to histogram differences of times (even though all 
     *             fields of the Hit objects are accessible)
     * @details
     * 
     * @param firsttime      - the earlier time to use for the time difference (see description
     *                            of GetDoubleValue()) for options)
     * @param secondtime     - the later time to use for the time difference (same as above)
     * @param start          - lower edge of the first bin
     * @param end            - upper edge of the last bin
     * @param binwidth       - width of a single bin
     * @param input          - category of data to use (Pass, Fail, Input)
     * @return               - the unformatted delay histogram
     */
    TH1*    GenerateDelayHistogram(std::string firsttime, std::string secondtime,
                                    double start, double end, double binwidth, int input = Pass);

    /**
     * @brief calculates the integral of the histogram and normalises the result to be 1 for the
     *             integral over the whole histogram
     * @details
     * 
     * @param histogram      - the histogram to generate the integration curve from
     * @return               - a TGraph containing the integration curve
     */
    TGraph* GenerateIntegrationCurve(TH1* histogram);

    /**
     * @brief takes an object, formats it and generates the plot in a new window
     * @details
     * 
     * @param graph          - the graph to plot
     * @param xtitle         - title for the x axis
     * @param ytitle         - title for the y axis
     * @param options        - drawing options for the ROOT Draw[Clone]() method
     * @return               - a pointer to the canvas used to plot the graph
     */
    TCanvas* Plot(TGraph* graph, std::string xtitle, std::string ytitle, 
                                    std::string options = "AP*");
    TCanvas* Plot(TH1* histogram, std::string xtitle, std::string ytitle, 
                                    std::string options = "");
    TCanvas* Plot(TH2* histogram, std::string xtitle, std::string ytitle, std::string ztitle, 
                    std::string options = "colz");

    /**
     * @brief extracts all hits from the group hit provided using a dictionary provided
     * @details
     * 
     * @param encoding       - the map connecting the codes with the actual addresses
     * @param hit            - the group hit object to analyse for all contained hits
     * 
     * @return               - a vector containing all hits as decoded with the provided encoding
     */
    std::vector<Hit> SeparateHit(std::map<int, int>& encoding, Hit& hit, 
                                    std::string addrname = "PixelDiode");
    /**
     * @brief replaces the contents of a hit category with its decoded data representation
     * @details
     * 
     * @param encoding       - the map connecting the codes with the actual addresses
     * @param addrname       - the address field to use for the decoding
     * @param input          - the category to use for the evaluation
     * 
     * @return               - the resulting number of hits
     */
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
