#ifndef _EVENTGENERATOR
#define _EVENTGENERATOR

#include "TCoord.h"

class EventGenerator
{
public:
	EventGenerator();

	/**
	 * @brief evaluates the charge generated in a pixel for a given particle track with a
	 *          "charge generation length"
	 * @details 
	 * 
	 * @param x0        - set point of the line equation
	 * @param r         - direction vector of the line equation
	 * @param position  - origin of the pixel to evaluate (not the middle)
	 * @param size      - the size of the pixel to evaluate
	 * @param minsize   - minimum space diagonal of an element to evaluate
	 * @param sigma     - sigma of the particle track width (from a gaussian)
	 * @param setzero   - number of sigmas before the distribution is cut off
	 * @param root      - flag to mark the user call of the function. can be used to turn off
	 *                      some output if set to false. The behaviour is not changed
	 * @return          - the charge deposited inside the pixel
	 */
	double GetCharge(TCoord<double> x0, TCoord<double> r, TCoord<double> position, 
						TCoord<double> size, double minsize, double sigma, 
						int setzero = 5, bool root = true)
private:
};


#endif //_EVENTGENERATOR