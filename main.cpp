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
    if(argc == 2)
    {
        std::cout << "Loading from file \"" << argv[1] << "\" ..." << std::endl;
        Simulator sim(argv[1]);
        sim.LoadInputFile();

        std::cout << "Detectors: " << sim.GetNumDetectors() << std::endl;
        std::cout << sim.PrintDetectors() << std::endl;

        sim.InitEventGenerator();

        sim.SimulateUntil(sim.GetStopTime(), sim.GetStopDelay());
    }
    else
    {

        //generate detector
        Detector Matrix("Test1", 0);

        //generate 4 ROCs with 3 pixels
        for (int x = 0; x<4; x++)
        {
            ReadoutCell roc("roc", x, 1);
            for (int y = 0; y<3;y++)
            {
                TCoord <double> position{(double)x*pixsizex,(double)y*pixsizey,0};
                TCoord <double> size{(double)pixsizex,(double)pixsizey,(double)pixsizez};
                Pixel pix(position, size, "pix", y, 0 );
                roc.AddPixel(pix);
                std::cout << " pixel " << y << " added" << std::endl;
            }
            Matrix.AddROC(roc);
            std::cout << "roc " << x << " added" << std::endl;
        }

        for (int x = 0; x<4; x++)
        {
            //ReadoutCell* roc;
            std::string temp;
            temp = Matrix.GetROC(x)->GetAddressName();
            std::cout << temp << std::endl << std::endl;
            for (int y = 0; y<3;y++)
            {
                temp = Matrix.GetROC(x)->GetPixel(y)->GetAddressName();
                std::cout << temp << std::endl;
            }
        }






        std::cout << Matrix.GetPosition() << std::endl;
        std::cout << Matrix.GetSize() <<std::endl;

        if (Matrix.SizeOK())
            std::cout << "Size OK" << std::endl;
        else
            std::cout << "Size NOT OK" << std::endl;
        if (Matrix.EnlargeSize())
            std::cout << "enlarged" << std::endl;
        else
            std::cout << "not enlarged" << std::endl;
        if (Matrix.SizeOK())
            std::cout << "Size OK" << std::endl;
        else
            std::cout << "Size NOT OK" << std::endl;
        std::cout << Matrix.GetPosition() << std::endl;
        std::cout << Matrix.GetSize() <<std::endl;

        std::cout << Matrix.PrintDetector() << std::endl;

        //Simulator
        EventGenerator evgen(&Matrix);
        evgen.SetOutputFileName("eventgen.dat");
        evgen.SetSeed();
        evgen.SetClusterSize(20);
        evgen.SetEventRate(0.1);
        evgen.SetCutOffFactor(5);
        evgen.SetMinSize(1);	//0.1);
        evgen.SetInclinationSigma(0.15);	//gaussian sigma in radians
        std::cout << "test" << std::endl;
        evgen.GenerateEvents(0, 1); //3);

        std::cout << "Events: " << evgen.GetNumEventsLeft() << std::endl;

        evgen.PrintQueue();

        bool getnewevent = true;
        int currentTS = 0;
        int nextTS = -1;
        std::vector<Hit> nextevent;
        nextevent.clear();
        int eventsleft = evgen.GetNumEventsLeft();
        int k= 0;
        while ((eventsleft != 0 || getnewevent )&& k <50)
        {
            ++k;
            //clock down
            if (getnewevent == true)
            {
                nextevent = evgen.GetNextEvent();
                getnewevent = false;
                nextTS = nextevent.front().GetTimeStamp();
            }
            std::cout << "currentTS: " << currentTS << "; nextEventTS: " << nextTS << std::endl;
            if (currentTS == nextTS)
            {
                std::cout << "I AM HIT!" << std::endl;
                for (auto it = nextevent.begin(); it != nextevent.end(); it++)
                {
                    std::cout << it->GenerateString() << std::endl;
                    Hit hit = *it;
                    std::cout << hit.GenerateString() << std::endl;
                    if (Matrix.PlaceHit(hit))
                        std::cout << "HIT PLACED" << std::endl;
                }


                eventsleft = evgen.GetNumEventsLeft();
                if (eventsleft > 0)
                    getnewevent = true;
            }

            //clock up
            std::cout << "current state: " << Matrix.GetState() << std::endl;
            Matrix.StateMachine();


            //counters up
            currentTS++;
            //Matrix.NextState();

        }



        /*
    	Hit a();
    	ReadoutCell b();
            Detector c("new", 1);
           std::cout << "oldstate: " << c.GetState() << std::endl;
           c.NextState();
           std::cout << "newstate: " << c.GetState() << std::endl;
    	
    	
    	TCoord <double> position{10,20,30};
    	TCoord <double> size{1,2,3};
    	std::string addressname = "row"; 
    	int address = 5;
    	double threshold = 1093;
    	Pixel pix(position, size, addressname, address, threshold);
    	std::string abc = pix.GetAddressName();
            std::cout << "Hello World!" << abc << std::endl;
        */
    }
}

