#include <iostream>
#include <string>

//#include "hit.h"
#include "TCoord.h"
#include "pixel.h"


int main(int argc, char** argv)
{
	//Hit a();
	
	
	TCoord <double> position{10,20,30};
	TCoord <double> size{1,2,3};
	std::string addressname = "row"; 
	int address = 5;
	double threshold = 1093;
	Pixel pix(position, size, addressname, address, threshold);
	std::string abc = pix.GetAddressName();
	std::cout << "Hello World!" << abc << std::endl;
}