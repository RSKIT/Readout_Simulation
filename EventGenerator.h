#ifndef _EVENTGENERATOR
#define _EVENTGENERATOR

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <cstdlib>
#include <time.h>
#include <random>
#include <math.h>
#include <deque>
#include <list>

#include "TCoord.h"
#include "hit.h"
#include "pixel.h"
#include "readoutcell.h"
#include "detector.h"



class EventGenerator
{
public:
	EventGenerator();
	EventGenerator(DetectorBase* detector);
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
	void AddDetector(DetectorBase* detector);
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
	DetectorBase* GetDetectorByIndex(int index);
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
	DetectorBase* GetDetectorByAddress(int address);

	/**
	 * @brief the filename for writing out the hits in the generated events including some 
	 *             additional information as the particle track
	 * @details
	 * @return               - the filename used to write out the event information
	 */
	std::string GetOutputFileName();
	void        SetOutputFileName(std::string filename);

	/**
	 * @brief the seed of the random generator
	 * @details
	 * @return               - the seed value of the random generator or zero if it was generated
	 *                            from the PCs features
	 */
	int  GetSeed();
	/**
	 * @brief changes the seed of the random generator. With this function also the event index
	 *             counter is reset.
	 * @details
	 * 
	 * @param seed           - the new seed of the random generator. any value except from zero is
	 *                            used as passed. For zero, the seed is generated using the CPU 
	 *                            time
	 */
	void SetSeed(int seed = 0);

	/**
	 * @brief sigma for the Gaussian distribution of the inclination angle for the particle 
	 *             direction in radians
	 * @details
	 * @return               - the sigma of the gaussian angle distribution in radians
	 */
	double GetInclinationSigma();
	void   SetInclinationSigma(double sigma);

	/**
	 * @brief the sigma of the Gaussian distribution of the radial charge distribution around the
	 *             particle track in microns.
	 * @details
	 * @return               - the radial sigma of the charge distribution in microns
	 */
	double GetClusterSize();
	void   SetClusterSize(double size);

	/**
	 * @brief the number of the event rate in either events per timestamp or events per timestamp
	 *             and area (Events/TS or Events/(µm²TS)). To obtain the unit, call
	 *             GetEventRateGlobal().
	 * @details
	 * @return               - the event rate as the scaling parameter of the exponential decay
	 *                            used for drawing the time differences
	 */
	double GetEventRate();
	/**
	 * @brief the setting whether the event rate is provided as a total number for the detector or
	 *             whether it is passed as rate per area
	 * @details
	 * @return               - true if the rate is provided as total rate (in Events/TS), false for
	 *                            a rate per area (in Events/(µm²TS))
	 */
	bool   GetEventRateGlobal();
	/**
	 * @brief changes the event rate for the event generation
	 * @details
	 * 
	 * @param rate           - the rate number
	 * @param total          - determines whether the unit of `rate` is Events/TS (true) or
	 *                            Events/(µm²TS) (false)
	 */
	void   SetEventRate(double rate, bool total = false);

	/**
	 * @brief provides the conversion factor from the charge number generated in the calculation
	 *             to the unit of choice set by the user
	 * @details
	 * @return               - the scaling factor to make the calculated charge number a number in
	 *                            e.g. Coulomb
	 */
	double GetChargeScaling();
	void   SetChargeScaling(double scalefactor);

	/**
	 * @brief provides the largest space diagonal length (in microns) not to divide further in the
	 *             calculation of the charge in a pixel
	 * @details
	 * @return               - the maximum size of a voxel space diagonal to not divide it further
	 */
	double GetMinSize();
	void   SetMinSize(double diagonallength);

	/**
	 * @brief the number of sigmas of the radial charge distribution after which the distribution
	 *             is set to zero to save computation effort
	 * @details
	 * @return               - the number of sigmas after which the distribution is set to zero
	 */
	int  GetCutOffFactor();
	void SetCutOffFactor(int numsigmas);

	/**
	 * @brief provides the probability of the generation of a trigger signal for the events
	 *             generated.
	 * @details
	 * @return               - trigger generation probability in [0,1]
	 */
	double	GetTriggerProbability();
	/**
	 * @brief changes the probability for the generation of a trigger signal for the signals
	 *             generated. Values > 1 behave like 1 and negative values behave like 0
	 * @details
	 * 
	 * @param probability    - the new trigger generation probability
	 */
	void	SetTriggerProbability(double probability);

	/**
	 * @brief provides the delay of the trigger signal after the event starting point
	 * @details
	 * @return               - the delay of the trigger signal in timestamps
	 */
	int		GetTriggerDelay();
	void	SetTriggerDelay(int delay);

	/**
	 * @brief provides the length of the trigger signal for "finding" triggered signals
	 * @details
	 * @return               - the length of the trigger signal in timestamps
	 */
	int		GetTriggerLength();
	void	SetTriggerLength(int length);

	/**
	 * @brief returns the timestamp when the trigger is turned off again (at this exact timestamp
	 *             the trigger is off)
	 * @details 
	 * @return               - timestamp at which the trigger will be turned off
	 */
	int		GetTriggerOffTime();
	void	SetTriggerOffTime(int timestamp);

	/**
	 * @brief adds a timestamp at which the trigger signal is turned on (before the rising edge)
	 * @details 
	 * 
	 * @param timestamp      - the first timestamp at which the trigger is on
	 */
	void	AddOnTimeStamp(int timestamp);
	/**
	 * @brief the number of timestamps to turn on the trigger signal
	 * @details
	 * @return               - the number of trigger timestamps
	 */
	int		GetNumOnTimeStamps();
	/**
	 * @brief sorts the timestamps chronologically
	 * @details
	 */
	void 	SortOnTimeStamps();
	/**
	 * @brief deletes all stored timestamps
	 * @details
	 */
	void 	ClearOnTimeStamps();

	/**
	 * @brief provides the current trigger state fot the timestamps passed
	 * @details
	 * 
	 * @param timestamp      - the current timestamp to evaluate for
	 * @return               - the trigger signal (active true)
	 */
	bool 	GetTriggerState(int timestamp);

	/**
	 * @brief generates the passed number of events
	 * @details
	 * 
	 * @param firsttime      - earliest possible time for the first event
	 * @param numevents      - the number of events to generate
	 */
	void GenerateEvents(double firsttime = 0, int numevents = 1);
	/**
	 * @brief removes all hits from the event generator's storage
	 * @details
	 */
	void ClearEventQueue();

	/**
	 * @brief returns the number of events generated by now in total since the last setting of
	 *             the random number generator setting of the seed value
	 * @details
	 * @return               - the number of events (not pixel hits) generated by now in total
	 */
	int GetNumEventsGenerated();
	/**
	 * @brief returns the number of events left in the event queue of the event generator object
	 * @details
	 * @return               - the number of events in the event queue (not pixel hits)
	 */
	int GetNumEventsLeft();

	/**
	 * @brief returns a vector containing the pixel hits of the next event and deletes them from
	 *             the event queue
	 * @details
	 * @return  	         - the pixel hits of the next event in time
	 */
	std::vector<Hit> GetNextEvent();
	/**
	 * @brief provides a vector containing the pixel hits of an event specified by the passed 
	 *             index. The pixel hits are not deleted from the event queue
	 * @details
	 * 
	 * @param eventindex     - index of the event to get
	 * @return               - the pixel hits of the event selected or an empty vector if the
	 * 							  event index does not exist in the event queue
	 */	
	std::vector<Hit> GetEvent(int eventindex);
	/**
	 * @brief returns the next pixel hit from the event queue and deletes it there
	 * @details
	 * @return               - the next pixel hit from the event queue or an invalid hit if the
	 *                            event queue is empty
	 */
	Hit GetNextHit();
	/**
	 * @brief provides the next pixel hit from the event queue without removing it from there
	 * @details
	 * @return               - the next hit from the event queue or an invalid hit if the event
	 *                            queue is empty
	 */
	Hit GetHit();

	/**
	 * @brief writes the event queue in human readable form to cout
	 * @details
	 */
	void PrintQueue();	//TODO: change: void -> std::string



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
	 * @param sigma     - sigma of the particle track width (from a Gaussian distribution)
	 * @param setzero   - number of sigmas before the distribution is cut off
	 * @param root      - flag to mark the user call of the function. can be used to turn off
	 *                      some output if set to false. The behaviour is not changed.
	 * @return          - the scaled charge deposited inside the pixel (see ChargeScaling)
	 */
	double GetCharge(TCoord<double> x0, TCoord<double> r, TCoord<double> position, 
						TCoord<double> size, double minsize, double sigma, 
						int setzero = 5, bool root = true);
private:
	/**
	 * @brief scans the detector for a given particle track for the charge generated in the
	 *             pixels and collects the pixel hits in the detector
	 * @details
	 * 
	 * @param hit            - dummy hit object for address generation and time stamping
	 * @param cell           - the readoutcell to investigate
	 * @param direction      - direction of the particle track to investigate
	 * @param setpoint       - set point of the particle track to investigate
	 * @param print          - turns on some output about the charges generates in the pixels
	 *                            (true activates additional output)
	 * @return               - the pixel hits generated by the given particle track and particle
	 *                            parameters
	 */
	std::vector<Hit> ScanReadoutCell(Hit hit, ReadoutCell* cell, TCoord<double> direction, 
										TCoord<double> setpoint, bool print = false);

	std::vector<DetectorBase*> detectors;

	int eventindex;				//index for the next event to generate

	double clustersize;
	double eventrate;
	bool   totalrate;

	std::default_random_engine generator;	//a uniform random generator
	int seed;
	double inclinationsigma;	//sigma for the gaussian distribution of the theta angle 
								// in radians

	double chargescale;			//to make the "charge" calculated a charge in Coulomb or any 
								// other unit

	double minsize;				//maximum space diagonal of a volume treated as a point
	int numsigmas;				//number of sigmas before the gaussian is cut off

	std::string filename;		//filename for the output file


	std::deque<Hit> clusterparts;	//the event queue containing the pixel hits

	//trigger signal generation:
	double triggerprobability;	//generation probability for a trigger to an event
	int triggerdelay;			//delay of the trigger signal after the hit implantation
	int triggerlength;			//length of the trigger signal in timestamps
	bool triggerstate;			//state of the trigger

	std::list<int> triggerturnontimes;
	int triggerturnofftime;
};


#endif //_EVENTGENERATOR
