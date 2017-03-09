#include <iostream>
#include <string>

#include "TCoord.h"
#include "hit.h"

double GetCharge(TCoord<double> x0, TCoord<double> r, TCoord<double> position, TCoord<double> size,
					double minsize, double sigma, int setzero = 5, bool root = true)
{
	static int counter = 0;

	if(root)
	{
		counter = 0;
	}

	++counter;

	double distance = 0;
	double charge = 0;

	TCoord<double> pixelmiddle = position + 0.5 * size;


	distance = (x0-pixelmiddle-((pixelmiddle-x0)*r)/r.abs()/r.abs()*r).abs();
	distance -= setzero * sigma;


	if(distance >= 0.5 * size.abs())
	{
		if(root)
			std::cout << "Iterations: " << counter << std::endl;
		return 0;
	}
	else if(size.abs() >= 2 * minsize)
	{
		TCoord<double> newsize = 0.5 * size;
		TCoord<double> newposition = position;
		TCoord<double> shift = TCoord<double>::Null;
		for (int x = 0; x < 8; ++x)
		{
			if((x&1) != 0)
				shift[0] = newsize[0];
			else
				shift[0] = 0;
			if((x&2) != 0)
				shift[1] = newsize[1];
			else
				shift[1] = 0;
			if((x&4) != 0)
				shift[2] = newsize[2];
			else
				shift[2] = 0;

			charge += GetCharge(x0,r,position + shift, newsize, minsize, sigma, setzero, false);
		}

		if(root)
			std::cout << "Iterations: " << counter << std::endl;
		return charge;
	}
	else
	{

		distance = (x0-pixelmiddle-((pixelmiddle-x0)*r)/r.abs()/r.abs()*r).abs();
		charge = 0.1269873/sigma*exp(-distance*distance/2/sigma/sigma) * size[0]*size[1]*size[2];

		if(root)
			std::cout << "Iterations: " << counter << std::endl;
		return charge;
	}
}

int main(int argc, char** argv)
{
	Hit a();

	std::cout << "Hello World!" << std::endl;

	std::cout << "This is a test for the numerical integration of volumes" << std::endl;

	TCoord<double> x0{0,1,1}; //{1,0,0};	//origin of the cylinder
	TCoord<double> r{1,0,0}; //{1,2.4,7};	//direction of the cylinder
	TCoord<double> position{-2,-2,-2};
	TCoord<double> size{4,4,4};

	double dx = 2./64;

	double sigma = 0.1; //0.3;

	double charge = 0;
	double distance = 0;
	TCoord<double> x{0,0,0};
	int counter = 0;

	//for(int i=0;i<1000;++i)
	{
		for(x[0] = position[0]; x[0] <= position[0] + size[0]; x[0] += dx)
		{
			for(x[1] = position[1]; x[1] <= position[1] + size[1]; x[1] += dx)
			{
				for(x[2] = position[2]; x[2] <= position[2] + size[2]; x[2] +=dx)
				{
					distance = (x0-x-((x-x0)*r)/r.abs()/r.abs()*r).abs();
					charge += 0.1269873/sigma*exp(-distance*distance/2/sigma/sigma) * pow(dx,3);
	
					++counter;
				}
			}
		}
	}

	std::cout << "Iterations: " << counter << std::endl
			  << "Charge:     " << charge << std::endl;


	//now the same calculation with octtree division of the pixel:

	double octcharge = 0;
	int octcounter = 0;

	std::cout << "The same calculation with octtree division of the pixel:" << std::endl;

	octcharge = GetCharge(x0,r, position, size, dx * std::sqrt(3), sigma, 5);

	std::cout << "Charge: " << octcharge << std::endl;
}