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


Evaluation::Evaluation() : input(std::vector<Hit>()), outputpass(std::vector<Hit>()),
        outputfail(std::vector<Hit>())
{

}

int Evaluation::LoadInputHits(std::string filename)
{
    return LoadHits(&input, filename);
}

#ifdef mycpp11support
int Evaluation::LoadInputHits(std::string archivename, std::string filename)
{
    zip_file archive;

    std::fstream f;
    f.open(archivename.c_str(), std::ios::in | std::ios::binary);
    if(!f.is_open())
        return 0;

    f.close();

    archive.load(archivename);

    if(!archive.has_file(filename))
        return 0;

    std::stringstream data("");

    data << archive.read(filename) << std::endl;

    return LoadHits(&input, data);
}
#endif //C++11 support

int Evaluation::LoadPassedOutputHits(std::string filename)
{
    return LoadHits(&outputpass, filename);
}

#ifdef mycpp11support
int Evaluation::LoadPassedOutputHits(std::string archivename, std::string filename)
{
    zip_file archive;

    std::fstream f;
    f.open(archivename.c_str(), std::ios::in | std::ios::binary);
    if(!f.is_open())
        return 0;

    f.close();

    archive.load(archivename);

    if(!archive.has_file(filename))
        return 0;

    std::stringstream data("");

    data << archive.read(filename) << std::endl;

    return LoadHits(&outputpass, data);
}
#endif //C++11 support

int Evaluation::LoadFailedOutputHits(std::string filename)
{
    return LoadHits(&outputfail, filename);
}

#ifdef mycpp11support
int Evaluation::LoadFailedOutputHits(std::string archivename, std::string filename)
{
    zip_file archive;

    std::fstream f;
    f.open(archivename.c_str(), std::ios::in | std::ios::binary);
    if(!f.is_open())
        return 0;

    f.close();

    archive.load(archivename);

    if(!archive.has_file(filename))
        return 0;

    std::stringstream data("");

    data << archive.read(filename) << std::endl;

    return LoadHits(&outputfail, data);
}
#endif //C++11 support

Hit Evaluation::GetHit(unsigned int index, int input)
{
    std::vector<Hit>* vec = GetVectorPointer(input);

    if(vec == 0)
        return Hit();

    if(index >= vec->size())
        return Hit();
    else
        return (*vec)[index];
}

int Evaluation::InsertHits(Hit hit, int input)
{
    std::vector<Hit>* vec = GetVectorPointer(input);

    vec->push_back(hit);

    return 1;
}

int Evaluation::InsertHits(std::vector<Hit>& hits, int input)
{
    std::vector<Hit>* vec = GetVectorPointer(input);

    vec->insert(vec->end(), hits.begin(), hits.end());

    return hits.size();
}

void Evaluation::ClearHits(int input)
{
    std::vector<Hit>* vec = GetVectorPointer(input);

    if(vec != 0)
        vec->clear();
}

int Evaluation::GetNumHits(int input)
{
    std::vector<Hit>* vec = GetVectorPointer(input);
    if(vec != 0)
        return GetVectorPointer(input)->size();
    else
        return 0;
}

TGraph* Evaluation::GenerateScatterplot(std::string xaxis, std::string yaxis, int input)
{
    std::vector<Hit>* vec = GetVectorPointer(input);

    TGraph* graph = new TGraph(0);
    static int idcounter = 0;
    std::stringstream s("");
    s << "Scatterplot_" << ++idcounter;

    graph->SetName(s.str().c_str());

    double x;
    double y;

    bool xtimestamp = (xaxis.compare("timestamp") == 0);
    bool ytimestamp = (yaxis.compare("timestamp") == 0);

    for(std::vector<Hit>::iterator it = vec->begin(); it != vec->end(); ++it)
    {
        x = GetDoubleValue(*it, xaxis);
        y = GetDoubleValue(*it, yaxis);

        graph->SetPoint(graph->GetN(), x, y);
    }

    return graph;
}

TH1* Evaluation::GenerateHistogram(std::string value,  
                                    double start, double end, double binwidth, int input)
{
    std::vector<Hit>* vec = GetVectorPointer(input);

    std::stringstream s("");
    static int index = 0;
    s << "Histogram_" << ++index;
    TH1* hist = new TH1I(s.str().c_str(), "Histogram", (end-start)/binwidth, start, end);

    for(std::vector<Hit>::iterator it = vec->begin(); it != vec->end(); ++it)
        hist->Fill(GetDoubleValue(*it, value));

    return hist;
}

TH1* Evaluation::GenerateDelayHistogram(std::string firsttime, std::string secondtime,
                                            double start, double end, double binwidth, int input)
{
    std::vector<Hit>* vec = GetVectorPointer(input);

    std::stringstream s("");
    static int index = 0;
    s << "DelayHistogram_" << ++index;
    TH1* hist = new TH1I(s.str().c_str(), "Histogram", (end-start)/binwidth, start, end);

    double t1;
    double t2;
    for(std::vector<Hit>::iterator it = vec->begin(); it != vec->end(); ++it)
    {
        t1 = GetDoubleValue(*it, firsttime);
        t2 = GetDoubleValue(*it, secondtime);

        hist->Fill(t2-t1);
    }

    return hist;
}

TGraph* Evaluation::GenerateIntegrationCurve(TH1* histogram)
{
    TGraph* graph = new TGraph(0);

    static int idcounter = 0;
    std::stringstream s("");
    s << "IntegrationCurve_" << ++idcounter;

    graph->SetName(s.str().c_str());

    double start = histogram->GetBinLowEdge(0);
    double binwidth = histogram->GetBinWidth(0);

    for(int i = 0; i <= histogram->GetNbinsX()+1; ++i)    //all bins, including the overflow bin
        graph->SetPoint(i, start + i * binwidth, histogram->Integral(0,i));

    double maximum = graph->GetY()[graph->GetN()-1];
    for(int i = 0; i < graph->GetN(); ++i)
        graph->SetPoint(i, graph->GetX()[i], graph->GetY()[i]/maximum);

    return graph;
}

TCanvas* Evaluation::Plot(TGraph* graph, std::string xtitle, std::string ytitle, 
                            std::string options, TLegend* leg, std::string legtitle)
{
    TCanvas* c = new TCanvas();
    c->SetWindowSize(1000,700);
    c->SetLeftMargin(0.13);
    c->SetRightMargin(0.08);
    c->Update();

    graph->GetXaxis()->SetTitleSize(0.05);
    graph->GetXaxis()->SetLabelSize(0.05);
    graph->GetYaxis()->SetTitleSize(0.05);
    graph->GetYaxis()->SetLabelSize(0.05);
    graph->GetXaxis()->SetTitle(xtitle.c_str());
    graph->GetYaxis()->SetTitle(ytitle.c_str());

    //graph->SetStats(false);

    graph->GetYaxis()->SetTitleOffset(1.1);

    graph->Draw(options.c_str());

    if(leg != 0)
        leg->AddEntry(graph, legtitle.c_str(), "p");

    c->Update();

    return c;
}

TCanvas* Evaluation::Plot(TH1* histogram, std::string xtitle, std::string ytitle, 
                            std::string options, TLegend* leg, std::string legtitle)
{
    TCanvas* c = new TCanvas();
    c->SetWindowSize(1000,700);
    c->SetLeftMargin(0.13);
    c->SetRightMargin(0.08);
    c->Update();

    histogram->GetXaxis()->SetTitleSize(0.05);
    histogram->GetXaxis()->SetLabelSize(0.05);
    histogram->GetYaxis()->SetTitleSize(0.05);
    histogram->GetYaxis()->SetLabelSize(0.05);
    histogram->GetXaxis()->SetTitle(xtitle.c_str());
    histogram->GetYaxis()->SetTitle(ytitle.c_str());

    histogram->SetStats(false);

    histogram->GetYaxis()->SetTitleOffset(1.1);

    histogram->Draw(options.c_str());

    if(leg != 0)
        leg->AddEntry(histogram, legtitle.c_str(), "l");

    c->Update();

    return c;
}

TCanvas* Evaluation::Plot(TH2* histogram, std::string xtitle, std::string ytitle, 
                            std::string ztitle, std::string options, TLegend* leg,
                            std::string legtitle)
{
    TCanvas* c = new TCanvas();
    c->SetWindowSize(1080, 700);
    c->SetLeftMargin(0.13/1.08);
    c->SetRightMargin((0.08+/*c->GetRightMargin()*/0.08)/1.08);
    c->Update();

    histogram->GetXaxis()->SetTitleSize(0.05);
    histogram->GetXaxis()->SetLabelSize(0.05);
    histogram->GetYaxis()->SetTitleSize(0.05);
    histogram->GetYaxis()->SetLabelSize(0.05);
    histogram->GetZaxis()->SetTitleSize(0.05);
    histogram->GetZaxis()->SetLabelSize(0.05);
    
    histogram->GetYaxis()->SetTitleOffset(1.1);

    histogram->SetStats(false);

    if(options.compare("colz") == 0)
    {
        histogram->Draw("COLZ");

        gPad->Update();
        TPaletteAxis *palette = (TPaletteAxis*)histogram->GetListOfFunctions()->FindObject("palette");
        palette->SetX2NDC(palette->GetX1NDC()+0.03);
        if(histogram->GetXaxis()->GetBinLowEdge(histogram->GetNbinsX()) < 1e-3)
            palette->SetY1NDC(0.17);    //avoid overlap with the power label of the x axis
        palette->Draw();
    }
    else
        histogram->Draw(options.c_str());

    if(leg != 0)
        leg->AddEntry(histogram, legtitle.c_str(), "f");

    c->Update();

    return c;

}

TCanvas* Evaluation::AddPlot(TCanvas* canvas, TGraph* graph, std::string options, 
                                TLegend* leg, std::string legtitle)
{
    if(canvas == 0 || graph == 0)
        return canvas;

    if(options.find("same") == std::string::npos && options.find("SAME") == std::string::npos)
        options = options + "same";

    if(leg != 0)
        graph->SetMarkerColor(leg->GetNRows()+1);

    graph->Draw(options.c_str());

    if(leg != 0)
        leg->AddEntry(graph, legtitle.c_str(), "p");

    canvas->Update();

    return canvas;
}

TCanvas* Evaluation::AddPlot(TCanvas* canvas, TH1* histogram, std::string options, 
                                TLegend* leg, std::string legtitle)
{
    if(canvas == 0 || histogram == 0)
        return canvas;

    if(options.find("same") == std::string::npos && options.find("SAME") == std::string::npos)
        options = options + "same";

    if(leg != 0)
        histogram->SetLineColor(leg->GetNRows()+1);

    histogram->Draw(options.c_str());

    if(leg != 0)
        leg->AddEntry(histogram, legtitle.c_str(), "l");

    canvas->Update();

    return canvas;
}

TCanvas* Evaluation::AddPlot(TCanvas* canvas, TH2* histogram, std::string options, 
                                TLegend* leg, std::string legtitle)
{
    if(canvas == 0 || histogram == 0)
        return canvas;

    if(options.find("same") == std::string::npos && options.find("SAME") == std::string::npos)
        options = options + "same";

    histogram->Draw(options.c_str());

    if(leg != 0)
        leg->AddEntry(histogram, legtitle.c_str(), "f");

    canvas->Update();

    return canvas;
}

TCanvas* Evaluation::AddLegend(TCanvas* canvas, TLegend* legend)
{
    if(canvas == 0 || legend == 0)
        return canvas;

    legend->Draw();

    canvas->Update();

    return canvas;
}

std::vector<Hit> Evaluation::SeparateHit(std::map<int, int>& encoding, Hit& hit, 
                                            std::string addrname)
{
    std::vector<Hit> hits;

    for(std::map<int, int>::iterator it = encoding.begin(); it != encoding.end(); ++it)
    {
        if((hit.GetAddress(addrname) & it->second) == it->second)
        {
            Hit h = hit;
            h.SetAddress(addrname, it->first);
            hits.push_back(h);
        }
    }

    return hits;
}

int Evaluation::SeparateHits(std::map<int, int>& encoding, std::string addrname, int input)
{
    std::vector<Hit> hits;

    std::vector<Hit>* vec = GetVectorPointer(input);

    for(std::vector<Hit>::iterator it = vec->begin(); it != vec->end(); ++it)
    {
        for(std::map<int, int>::iterator eit = encoding.begin(); eit != encoding.end(); ++eit)
        {
            if((it->GetAddress(addrname) & eit->second) == eit->second)
            {
                Hit h = *it;
                h.SetAddress(addrname, eit->first);
                hits.push_back(h);
            }
        }
    }

    vec->clear();
    vec->insert(vec->end(), hits.begin(), hits.end());

    return vec->size();
}

int Evaluation::LoadHits(std::vector<Hit>* vec, std::string filename)
{
    std::fstream f;
    f.open(filename.c_str(), std::ios::in);
    if(!f.is_open())
    {
        std::cout << "Could not open \"" << filename << "\"." << std::endl;
        return 0;
    }

    const int linelength = 1024;
    char line[linelength];
    int eventindex = -1;
    bool trigger = false;

    int hitcounter = 0;

    while(!f.eof())
    {
        f.getline(line, 1024, '\n');
        if(line[0] != '#')
        {
            Hit h = Hit(std::string(line));
            if(h.is_valid())
            {
                if(trigger && h.GetEventIndex() == eventindex)
                    h.AddReadoutTime("Trigger", 1);
                else
                    h.AddReadoutTime("Trigger", 0);

                vec->push_back(h);
                ++hitcounter;
            }
        }
        else
        {
            std::string text;
            std::stringstream s("");
            s << line;
            s >> text >> text;
            if(text.compare("Trigger") == 0)
                trigger = true;
            else if(text.compare("Event") == 0)
            {
                s >> eventindex;
                trigger = false;
            }
        }
    }

    f.close();

    return hitcounter;
}

int Evaluation::LoadHits(std::vector<Hit>* vec, std::stringstream& filecontents)
{
    const int linelength = 1024;
    char line[linelength];
    int eventindex = -1;
    bool trigger = false;

    int hitcounter = 0;

    while(filecontents.getline(line, 1024, '\n'))
    {
        if(line[0] != '#')
        {
            Hit h = Hit(std::string(line));
            if(h.is_valid())
            {
                if(trigger && h.GetEventIndex() == eventindex)
                    h.AddReadoutTime("Trigger", 1);
                else
                    h.AddReadoutTime("Trigger", 0);

                vec->push_back(h);
                ++hitcounter;
            }
        }
        else
        {
            std::string text;
            std::stringstream s("");
            s << line;
            s >> text >> text;
            if(text.compare("Trigger") == 0)
                trigger = true;
            else if(text.compare("Event") == 0)
            {
                s >> eventindex;
                trigger = false;
            }
        }
    }

    return hitcounter;
}

std::vector<Hit>* Evaluation::GetVectorPointer(int input)
{
    switch(input)
    {
        case(Input):
            return &(this->input);
        case(Pass):
            return &outputpass;
        case(Fail):
            return &outputfail;
        default:
            return 0;
    }
}

double Evaluation::GetDoubleValue(Hit& hit, std::string value)
{
    double x = -1;

    if(value.compare("eventid") == 0)
        x = hit.GetEventIndex();
    else if(value.compare("timestamp") == 0)
        x = hit.GetTimeStamp();
    else if(value.compare("deadtimeend") == 0)
        x = hit.GetDeadTimeEnd();
    else if(value.compare("deadtime") == 0)
        x = hit.GetDeadTimeEnd() - hit.GetTimeStamp();
    else if(value.compare("charge") == 0)
        x = hit.GetCharge();
    else if(value.substr(0,4).compare("addr") == 0)
        x = hit.GetAddress(value.substr(4));
    else if(value.substr(0,2).compare("TS") == 0)
        x = hit.GetReadoutTime(value.substr(2));

    return x;
}

int Evaluation::GetIntValue(Hit& hit, std::string value)
{
    int x = -1;

    if(value.compare("eventid") == 0)
        x = hit.GetEventIndex();
    else if(value.compare("timestamp") == 0)
        x = hit.GetTimeStamp();
    else if(value.compare("deadtimeend") == 0)
        x = hit.GetDeadTimeEnd();
    else if(value.compare("deadtime") == 0)
        x = hit.GetDeadTimeEnd() - hit.GetTimeStamp();
    else if(value.compare("charge") == 0)
        x = hit.GetCharge();
    else if(value.substr(0,4).compare("addr") == 0)
        x = hit.GetAddress(value.substr(4));
    else if(value.substr(0,2).compare("TS") == 0)
        x = hit.GetReadoutTime(value.substr(2));

    return x;

}
