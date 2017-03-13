#include <iostream>
#include <string>

#include "hit.h"
#include "TCoord.h"
#include "pixel.h"
#include "readoutcell.h"
#include "detector.h"

int pixsizex = 10;
int pixsizey = 20;
int pixsizez = 1;

int main(int argc, char** argv)
{
    //generate detector
    Detector Matrix("Test1", 0);

    //generate ROC with 4 pixels
    for (int x = 0; x<4; x++)
    {
        ReadoutCell roc("roc" + std::to_string(x), x, 1);
        for (int y = 0; y<3;y++)
        {
            TCoord <double> position{(double)x*pixsizex,(double)y*pixsizey,0};
            TCoord <double> size{(double)pixsizex,(double)pixsizey,(double)pixsizez};
            Pixel pix(position, size, "pix" + std::to_string(y), y, 0 );
            roc.AddPixel(pix);
            std::cout << "pix" << y << "added" << std::endl;
        }
        Matrix.AddROC(roc);
        std::cout << "roc" << x << "added" << std::endl;
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
        std::cout << "Hello World!" << abc << std::endl;*/
}
