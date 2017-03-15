#ifndef _EVENTGENERATOR
#define _EVENTGENERATOR

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <cstdlib>
#include <time.h>
#include <math.h>
#include <deque>

#include "TCoord.h"
#include "hit.h"
#include "pixel.h"
#include "detector.h"



class EventGenerator
{
public:
	EventGenerator();
	EventGenerator(Detector* detector);
	EventGenerator(int seed, double clustersize = 0, double rate = 0);

	/**
	 * @brief checks whether the output file and the event rate are set
	 * @details
	 * @return               - true if both are set correctly
	 */
	bool IsReady();

	/**
	 * @brief adds a detector object to the eventgenerator
	 * @details the detector passed will be used for the generation of events additionally to
	 *             the detectors already present in the detector. There is no check that a detector
	 *             is already linked in the event generator. A double inclusion will cause "double 
	 *             hits" in the event generator
	 * 
	 * @param detector       - the detector to add
	 */
	void AddDetector(Detector* detector);
	/**
	 * @brief removes all poointers to detectors from the event generator
	 * @details
	 */
	void ClearDetectors();
	/**
	 * @brief returns a pointer to a detector linked in the event generator if present
	 * @details for valid indices, this function returns a pointer to the detector linked at this
	 *             index. For too large or too small indices, a null pointer till be returned.
	 * 
	 * @param index          - the index of the detector to return
	 * @return               - a pointer to a detector or a null pointer in case of an index out 
	 *                            of bounds
	 */
	Detector* GetDetectorByIndex(int index);
	/**
	 * @brief returns a pointer to a detector linked in the event generator, but uses the addess
	 *             of the detector to find it
	 * @details for an address occupied by a detector linked in the event generator, a pointer
	 *             to this detector will be returned. On unoccupied addresses, a null pointer
	 *             will be returned
	 * 
	 * @param address        - the address to look for
	 * @return               - a pointer to a detector or a null pointer if the address is not
	 *                            used by the detectors linked to the event generator
	 */
	Detector* GetDetectorByAddress(int address);

	std::string GetOutputFileName();
	void        SetOutputFileName(std::string filename);

	/* seed for the random generator */
	int  GetSeed();
	void SetSeed(int seed = 0);

	double GetClusterSize();
	void   SetClusterSize(double size);

	double GetEventRate();
	void   SetEventRate(double rate);

	double GetChargeScaling();
	void   SetChargeScaling(double scalefactor);

	double GetMinSize();
	void   SetMinSize(double diagonallength);

	int  GetCutOffFactor();
	void SetCutOffFactor(int numsigmas);

	void GenerateEvents(double firsttime = 0, int numevents = 1);

	int GetNumEventsGenerated();
	int GetNumEventsLeft();

	std::vector<Hit> GetNextEvent();
	std::vector<Hit> GetEvent(int eventindex);
	Hit GetNextHit();
	Hit GetHit();

	void PrintQueue();



	/**
	 * @brief evaluates the charge generated in a pixel for a given particle track with a
	 *          "charge generation length"
	 * @details 
	 * 
	 * @param x0        - set point of the line equation
	 * @param r         - direction vector of the line equation
	 * @param position  - origin (not middle) of the volume to investigate
	 * @param size      - size of the volume to investigate
	 * @param minsize   - minimum space diagonal of an element to evaluate
	 * @param sigma     - sigma of the particle track width (from a gaussian)
	 * @param setzero   - number of sigmas before the distribution is cut off
	 * @param root      - flag to mark the user call of the function. can be used to turn off
	 *                      some output if set to false. The behaviour is not changed
	 * @return          - the charge deposited inside the pixel
	 */
	double GetCharge(TCoord<double> x0, TCoord<double> r, TCoord<double> position, 
						TCoord<double> size, double minsize, double sigma, 
						int setzero = 5, bool root = true);
private:
	std::vector<Hit> ScanReadoutCell(Hit hit, ReadoutCell* cell, TCoord<double> direction, 
										TCoord<double> setpoint);

	std::vector<Detector*> detectors;

	int eventindex;	//index for the next event to generate

	double clustersize;
	double eventrate;
	int seed;

	double chargescale;	//to make the "charge" calculated a charge in Coulomb

	double minsize;		//maximum space diagonal of a volume treated as a point
	int numsigmas;		//number of sigmas before the gaussian is cut off

	std::string filename;		//filename for the output file


	std::deque<Hit> clusterparts;
};


#endif //_EVENTGENERATOR