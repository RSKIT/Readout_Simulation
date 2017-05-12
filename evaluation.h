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
