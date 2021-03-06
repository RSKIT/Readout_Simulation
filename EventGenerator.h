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

#ifndef _EVENTGENERATOR
#define _EVENTGENERATOR

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
//#include <cstdlib>	//supplied RAND_MAX
#include <time.h>
#include <random>
#include <math.h>
#include <deque>
#include <list>
#include <thread>
#include <algorithm>

#include "spline.h"

#include "TCoord.h"
#include "hit.h"
#include "pixel.h"
#include "readoutcell.h"
#include "detector.h"

#include <TTree.h>
#include <TFile.h>
#include <TVectorF.h>


class EventGenerator
{
public:
	//data structure to pass events for generic 3D line generator to threads:
	class particletrack {public:
						  int index;
						  TCoord<double> setpoint;
						  TCoord<double> direction;
						  double time;
						  bool trigger;};

	//data structre to store single bin charges for ITk events:
  	struct ChargeDistr {
		unsigned char etamodule;
		unsigned short etaindex;
		unsigned short phiindex;
		float charge;
	};

	//structures for processed ITk events (including pile-up):
	struct Event {unsigned int bcid;
				  int etamodule;
				  bool trigger;
				  bool operator==(const Event& second) const
				  {
				  	  return (etamodule == second.etamodule) && (bcid == second.bcid);
				  }
				  bool operator<(const Event& second) const
				  {
				  	  return (bcid < second.bcid);
				  }
				};
	struct ChargeDistrModule {
		unsigned int etaindex;
		unsigned int phiindex;
		float charge_track;
		float charge_noise;
		float charge_xtalk;
	};

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
	 * @brief provides the number of threads to be used for the charge calculation inside the
	 *             pixels during event generation
	 * @details
	 * @return               - the number of threads during event generation
	 */
	int    GetThreads();
	void   SetThreads(unsigned int numthreads);

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
	 * @brief generates a string containing a list of all time stamps at which the trigger is
	 *             turned on
	 * @details
	 * @return               - string containing the trigger turn on time stamps
	 */
	std::string PrintOnTimeStamps();

	/**
	 * @brief provides the current trigger state fot the timestamps passed
	 * @details
	 * 
	 * @param timestamp      - the current timestamp to evaluate for
	 * @param print          - write the trigger state on change to the terminal if set to true
	 * @return               - the trigger signal (active true)
	 */
	bool 	GetTriggerState(int timestamp, bool print = false);
	/**
	 * @brief provides the setting about trigger generation: Either the trigger signals are
	 *             generated per cluster or per time stamp
	 * @details
	 * @return               - true if triggers are generated per cluster, false if they are
	 *                            generated per time stamp
	 */
	bool 	GetTriggerOnClusters();
	void  	SetTriggerOnClusters(bool triggerpercluster);

	/**
	 * @brief generates the passed number of events
	 * @details
	 * 
	 * @param firsttime      - earliest possible time for the first event
	 * @param numevents      - the number of events to generate
	 * @param threads		 - the number of threads to use for this task, use 0 to use all cores,
	 *                            -1 indicates the use of the internally saved setting
	 * @param writeout       - determines whether the data is written directly to a file or not
	 * @param printtoterminal - determines whether the printing to terminal is done (true) or not
	 *                            (false)
	 */
	void GenerateEvents(double firsttime = 0, int numevents = 1, int threads = -1, 
							bool writeout = true, bool printtoterminal = true, 
							int updatepitch = 10);
	/**
	 * @brief loads pixel hits from a file
	 * @details
	 * 
	 * @param filename       - filename to load the hits from
	 * @param sort           - the contents of the hit queue will be sorted after the loading if
	 *                            this parameter is set to true
	 * @param timeshift		 - time to be added to the pixel hits
	 * 
	 * @return               - the number of pixel hits loaded
	 */
	int  LoadEventsFromFile(std::string filename, bool sort = true, double timeshift = 0.);
	/**
	 * @brief loads the pixel hits from the passed filestream
	 * @details
	 * 
	 * @param file           - file stream to load the hits from
	 * @param sort           - the contents of the hit queue will be sorted after the loading if
	 *                            this parameter is set to true
	 * @param timeshift      - time to be added to the pixel hits
	 * 
	 * @return               - the number of pixel hits loaded
	 */
	int  LoadEventsFromStream(std::fstream* file, bool sort = true, double timeshift = 0.);
	/**
	 * @brief removes all hits from the event generator's storage
	 * @details
	 */
	void ClearEventQueue();
	/**
	 * @brief sorts the hits in the event queue chronologically as they are expected to be sorted
	 * @details
	 */
	void SortEventQueue();

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
	 * @brief provides the time stamp of the last event stored in this object
	 * @details
	 * @return               - the last time stamp at which hits are inserted into the detector
	 */
	int GetLastEventTimestamp();

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

	/**
	 * @brief calculates the charge inside a (pixel) volume from a charge distribution
	 * @details 
	 * 
	 * @param charge         - charge distribution as charges in (smaller) volumes
	 * @param position       - origin of the (pixel) volume to evaluate
	 * @param size           - size of the (pixel) volume to evaluate
	 * @param granularity    - the size of a single volume in the charge distribution
	 * @param detectorsize   - total size of the detector
	 * @param print          - turns on some debug output
	 * 
	 * @return               - the charge inside the (pixel) volume as charge in the union volume
	 *                            of the pixel and the charge distribution volumes
	 */
	double GetCharge(std::vector<ChargeDistr>& charge, TCoord<double> position, 
						TCoord<double> size, TCoord<double> granularity, 
						TCoord<double> detectorsize, bool print = false);

	/**
	 * @brief calculates the charge inside a (pixel) volume from a charge distribution without
	 *             folding back of the quad/double module to the detector size
	 * @details
	 * 
	 * @param charge         - the charge distribution as charges in (smaller) volumes
	 * @param position       - origin of the (pixel) volume to evaluate
	 * @param size           - size of the (pixel) volume to evaluate
	 * @param granularity    - the size of a single volume in the charge distribution
	 * @param noisescaling   - scaling factor for the noise contribution in the charge data
	 * @param xtalkscaing    - scaling factor for the crosstalk contribution in the charge data
	 * @param print          - turns on some debug output
	 * 
	 * @return               - the charge inside the (pixel) volume as charge in the union of
	 *                            pixel and charge distribution volumes
	 */
	double GetCharge(std::vector<ChargeDistrModule>& charge, TCoord<double> position, 
						TCoord<double> size, TCoord<double> granularity, double noisescaling = 1,
						double xtalkscaling = 1, bool print = false);

	// ===  Spline functions for deadtime and timewalk calculations ===

	/**
	 * @brief adds a point for the interpolation of the dead time. The points do not have to be
	 *             ordered. They are sorted on first use of the spline function
	 * @details
	 * 
	 * @param charge         - the x value of the data point
	 * @param deadtime       - the y value of the data point
	 */
	void AddDeadTimePoint(double charge, double deadtime);
	/**
	 * @brief removes all data points from the spline
	 * @details
	 */
	void ClearDeadTimePoints();
	/**
	 * @brief update the Dead Time spline with the points in added by now
	 * @details
	 */
	void SetupDeadTimeSpline();
	/**
	 * @brief returns the value of the spline at the given position and recalculates the set points
	 *             of the spline if necessary (number of set points changed) or it is triggered by
	 *             the programmer/user
	 * @details
	 * 
	 * @param charge         - the x value for the evaluation of the spline
	 * @param forceupdate    - if true, the spline set points are regenerated
	 * 
	 * @return               - the value of the spline function at the given position
	 */
	double GetDeadTime(double charge, bool forceupdate = false);
	bool SaveDeadTimeSpline(std::string filename, double resolution);

	/**
	 * @brief adds a point for the time walk characteristics for the sensor. The points added do
	 *             not have to be sorted, as they will be sorted before first usage.
	 * @details 
	 * 
	 * @param charge         - the x value of the point
	 * @param timewalk       - the y value of the point
	 */
	void AddTimeWalkPoint(double charge, double timewalk);
	/**
	 * @brief removes all set points from the spline
	 * @details
	 */
	void ClearTimeWalkPoints();
	/**
	 * @brief update the time walk spline with the points added by now
	 * @details
	 */
	void SetupTimeWalkSpline();
	/**
	 * @brief evaluated the spline function at the passed position and recalculates the spline
	 *             function beforehand if necessary (changed number of set points)
	 * @details
	 * 
	 * @param charge         - the x value to evaluate the spline at
	 * 
	 * @return               - the time walk for the given charge
	 */
	double GetTimeWalk(double charge);
	bool SaveTimeWalkSpline(std::string filename, double resolution);

	/**
	 * @brief generates the log file contents as a string for writing it to an archive
	 * @details
	 * @return               - the content string of the event generation file
	 */
	std::string GenerateLog();
	void ClearLog();

	/**
	 *  @brief loads hit events from a ROOT file and generates Hit objects out of the data
	 *  @details
	 *  
	 *  @param filename      - file name of the ROOT file
	 *  @param firstline     - the first entry to consider when loading the ROOT file
	 *  @param numlines      - the number of entries to read from the ROOT file. To read all data
	 *                            this parameter is set to "-1"
	 *  @param firsttime     - earliest time possible for the first event
	 *  @param eta           - the eta index of the modules to use, 1 - 21 for one "ring", 0 for
	 *                            all modules
	 *  @param granularity   - the lengths of the voxels in the data (active volume)
	 *  @param numthreads    - the number of threads to use for the computation, use 0 to use 
	 *                            all cores
	 *  @param writeout      - if true, the log of the generated events is written to a file,
	 *                            or not if false
	 *  @param regroup       - divides the events not only by eventID but also spacially if larger
	 *                            than 0. It specifies the maximum distance between neighbouring
	 *                            pixels in micrometers
	 *  @param sort          - sort the resulting hits if true
	 *  @param print         - turn on or off output to terminal
	 *  @param updatepitch   - the number of events to process before printing updated progress
	 *                            information
	 *  
	 *  @return              - the number of events loaded from the ROOT file
	 */
	int LoadITkEvents(std::string filename, int firstline = 0, int numlines = -1, 
						double firsttime = 0, int eta = 0, 
						TCoord<double> granularity = TCoord<double>::Null,
						int numthreads = -1, bool writeout = true, double regroup = 0,
						bool sort = true, bool print = false, int updatepitch = 10);

	/**
	 * @brief loads hit events from a ROOT file and generates Hit objects out of the data
	 * @details
	 *  
	 * @param filename      - file name of the ROOT file
	 * @param firstline     - the first entry to consider when loading the ROOT file
	 * @param numlines      - the number of entries to read from the ROOT file. To read all data,
	 *                            this parameter is set to "-1". If this number exceeds the number
	 *                            of data sets in the file, the readout is stopped at the end of
	 *                            the file.
	 * @param firsttime     - time of the first event in units of simulation time stamps
	 * @param numeventstogenerate 
	 *  					 - the number of event to draw from the data loaded
	 * @param freqscaling   - the factor to get from simulation time stamps to bc time stamps.
	 *  						  For a simulation timestamp equalling to 6.25 ns and a BC time of
	 *  						  25 ns, this parameter is 4.
	 * @param eta           - the module ring in the barrel to use. Set to 0 to use all rings.
	 *                            Possible values run from 1 to 8 for quad modules and from 9 to 21
	 *                            for double modules.
	 * @param noisescaling  - the scaling of the noise in the data file for the simulation
	 * @param xtalkscaling  - the scaling of the crosstalk contribution to the signal charge 
	 * @param granularity   - the lengths of the voxels in the data (active volume)
	 * @param numthreads    - the number of threads to use for the computation, use 0 to use 
	 *                            all cores
	 * @param writeout      - if true, the log of the generated events is written to a file,
	 *                            or not if false
	 * @param print         - turn on or off output to terminal
	 * @param sort          - sort the resulting hits if true
	 * @param updatepitch   - the number of events to process before printing updated progress
	 *                            information
	 *  
	 * @return              - the number of events extracted from the data
	 */
	int LoadProcessedITkEvents(std::string filename, int firstline = 0, int numlines = -1,
						double firsttime = 0, int numeventstogenerate = -1, double freqscaling = 1,
						int eta = 0, double noisescaling = 1, double xtalkscaling = 1,
						TCoord<double> granularity = TCoord<double>::Null, 
						int numthreads = -1, bool writeout = true, bool print = false, 
						bool sort = false, int updatepitch = 10);
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

	/**
	 * @brief scans the detector for a given charge distribution and evaluates the pixel hits for
	 *             the charge distribution.
	 * @details
	 * 
	 * @param hit            - dummy hit object for address generation and time stamping
	 * @param cell           - the readout cell to investigate
	 * @param charge         - vector describing the charge distribution
	 * @param chargestart    - start of the encapsulating box volume of the charge distribution
	 * @param chargeend	     - end of the encapsulating box volume of the charge distribution
	 * @param granularity    - size of the volume covered by a single data point in the charge
	 *                            distribution
	 * @param detectorsize   - total size of the detector for folding back data
	 * @param print          - turns on some debug output
	 * @return               - the pixel hits generated by the charge distribution
	 */
	std::vector<Hit> ScanReadoutCell(Hit hit, ReadoutCell* cell, std::vector<ChargeDistr>& charge,
		                                TCoord<double> chargestart, TCoord<double> chargeend,
		                                TCoord<double> granularity, TCoord<double> detectorsize,
		                                bool print = false);

	/**
	 * @brief method to be called by threads for the evaluation of particle tracks. It provides
	 *             hit objects and logging output via parameters
	 *             
	 * @param itself         - to be filled with this. The EventGenerator object on which the
	 *                            thread is to work
	 * @param begin			 - iterator pointing to the first particletrack to evaluate
	 * @param end			 - iterator pointing to the first particletrack not to evaluate anymore
	 *                            (as the end() iterator)
	 * @param pixelhits		 - reference to a vector in which the hit objects are stored. Use a
	 *                            separate vector for each thread
	 * @param output		 - reference to a stringstream in which the logging information is
	 *                            collected to write it to a file at the end. Use a separate vector
	 *                            for each thread
	 * @param id			 - number for the identification of the thread. Only for printing to
	 *                            std::cout
	 * @param printtoterminal - determines whether output is written to terminal (true) or not
	 * @param updatepitch    - the pitch between two update outputs of the generation. Only values
	 *                            >0 are allowed. For invalid numbers the parameter is set to 10.
	 */
	static void GenerateHitsFromTracks(EventGenerator* itself, 
										std::vector<particletrack>::iterator begin,
										std::vector<particletrack>::iterator end,
										std::vector<Hit>* pixelhits,
										std::string* output,
										int id = -1,
										bool printtoterminal = true,
										int updatepitch = 10);

	/**
	 * @brief method to be called by threads for the evaluation of charge distributions. It
	 *             provides hit objects and logging output via parameters
	 * @details
	 * 
	 * @param itself         - to be filled with `this`. The EventGenerator object on which the
	 *                            thread is supposed to work
	 * @param begin          - the first charge distribution to evaluate
	 * @param end            - the first charge distribution not to evaluate any more
	 * @param times          - the event time stamps for the charge distributions as map for
	 *                            event ID and time stamp
	 * @param triginfos      - trigger information for the events as map with the event ID as key
	 * @param granularity    - size of the volume covered by a data point in the charge 
	 *                            distribution
	 * @param detectorsize   - size of the complete detector for folding back the data
	 * @param pixelhits      - output vector for the pixel hits
	 * @param output         - output stream for logging
	 * @param firsteventid   - event ID to use for the pixel hits of the event in `begin`. The
	 *                            next events will get an incremented event ID
	 * @param id             - ID for the thread for identification in the output
	 * @param print          - turns on (true) or off the printing to the terminal
	 * @param updatepitch    - the number of events to analyse of which once a progress update line
	 *                            is written to the terminal
	 */
	static void GenerateHitsFromChargeDistributions(EventGenerator* itself,
				//std::map<unsigned int, std::vector<ChargeDistr> >::iterator begin,
				//std::map<unsigned int, std::vector<ChargeDistr> >::iterator end,
				std::deque<std::pair<unsigned int, std::vector<ChargeDistr> > >::iterator begin,
				std::deque<std::pair<unsigned int, std::vector<ChargeDistr> > >::iterator end,
				std::map<unsigned int, double>* times,
				std::map<unsigned int, bool>* triginfos,
				TCoord<double> granularity,
				TCoord<double> detectorsize,
				std::vector<Hit>* pixelhits,
				std::string* output,
				int firsteventid, int id = -1,
				bool print = false,
				int updatepitch = 10);

	/**
	 * @brief takes a cluster a tries to separate it into several clusters which are spatially
	 *             separated
	 * @details
	 * 
	 * @param resultclusters - map for storing the resulting clusters
	 * @param begin          - the first cluster to evaluate
	 * @param end            - the first cluster to not evaluate any more (like map::end())
	 * @param granularity    - size of the voxels of the data
	 * @param maxdistance    - maximum distance of neighbouring pixels in one cluster
	 * @param id             - identifying number for this call
	 * @param numclusters    - total number of clusters to evaluate (for output reasons)
	 * @param print          - turns on (true) or off printing to the terminal
	 * @param updatepitch    - the number of events after which an progress update is written to
	 *                            the terminal. If <=0 it will be set to 10.
	 */
	static void SeparateClusters(std::map<unsigned int, std::vector<ChargeDistr> >* resultclusters,
						std::map<unsigned int, std::vector<ChargeDistr> >::iterator begin,
						std::map<unsigned int, std::vector<ChargeDistr> >::iterator end,
						TCoord<double> granularity, double maxdistance,
						int id, int numclusters, bool print = false, int updatepitch = 10);

	/**
	 * @brief method to be called by threads for the evaluation of charge distributions. It
	 *             provides hit objects and logging output via parameters for processed event
	 *             data (including pile-up)
	 * @details
	 * 
	 * @param itself         - the event generator to work on (necessary as the thread can not
	 *                            operate on an object)
	 * @param events		 - the events (BCID, eta and phi indices of the modules) to use for
	 *                            the hit generation
	 * @param data           - all charge distributions with corresponding event identifyer
	 * @param granularity    - size of a single volume in the charge distributions
	 * @param noisescaling   - scaling factor for the noise contribution in the charge data
	 * @param xtalkscaling   - scaling factor for the crosstalk contribution in the charge data
	 * @param pixelhits      - output vector of the hits generated by the charge distributions
	 *                            referred to by the entries in `events`
	 * @param output         - logging output of the generated hit data
	 * @param firsteventid   - the event id of the first generated event
	 * @param firsttime      - time in simulation time steps for the first event
	 * @param freqscaling    - scaling from the BC time steps to the simulation time steps. For
	 *                            example for a 6.25 ns simulation time step and a 25 ns BC time
	 *                            step, this parameter is 4
	 * @param id             - id for output to the terminal to identify the thread
	 * @param print          - turns on or off output
	 * @param updatepitch    - sets the pitch of output to reduce the amount of data written to
	 *                            the terminal
	 */
	static void GenerateHitsFromProcessedChargeDistributions(EventGenerator* itself, 
						std::vector<Event>* events, 
						std::map<Event, std::vector<ChargeDistrModule> >* data, 
						TCoord<double> granularity, double noisescaling, double xtalkscaling, 
						std::vector<Hit>* pixelhits, std::string* output, int firsteventid, 
						double firsttime = 0, double freqscaling = 1, int id = -1, 
						bool print = false, int updatepitch = 10);

	/**
	 * @brief scans the passed readout cell for hits of the charge distribution with the readout
	 *             cell's contents
	 * @details
	 * 
	 * @param hit            - template hit for filling with the address of the pixels
	 * @param cell           - the readout cell to scan
	 * @param granularity    - size of a single volume in the charge distribution
	 * @param noisescaling   - scaling factor for the noise contribution in the charge data
	 * @param xtalkscaling   - scaling factor for the crosstalk contribution in the charge data
	 * @param print          - turns on (or off) debug output
	 * @return               - a vector of hit objects representing the hit pixels in the readout
	 *                            cell for the passed charge distribution
	 */
	std::vector<Hit> ScanReadoutCell(Hit hit, ReadoutCell* cell, 
						std::vector<ChargeDistrModule>& charge, TCoord<double> chargestart, 
						TCoord<double> chargeend, TCoord<double> granularity, 
						double noisescaling = 1, double xtalkscaling = 1, bool print = false);


	std::vector<DetectorBase*> detectors;

	int eventindex;				//index for the next event to generate

	double clustersize;
	double eventrate;
	bool   totalrate;

	//std::default_random_engine generator;	//a uniform random generator
	std::mt19937_64 generator;	//instantiation of the Mersenne Twister random generator
	int seed;

	int threads;				//the number of threads to use for the calculation of the event
								// pixel charges (0 = use all available cores)

	double inclinationsigma;	//sigma for the gaussian distribution of the theta angle 
								// in radians

	double chargescale;			//to make the "charge" calculated a charge in Coulomb or any 
								// other unit

	double minsize;				//maximum space diagonal of a volume treated as a point
	int numsigmas;				//number of sigmas before the gaussian is cut off

	std::string filename;		//filename for the output file
	std::string genoutput;//storage for the data of the eventgen output file when writing to
	                            // an archive


	std::deque<Hit> clusterparts;	//the event queue containing the pixel hits
	int lasteventtimestamp;

	//trigger signal generation:
	bool triggeronclusters;		//determines whether trigger signals are generated on clusters
								//  (true) or on time stamps (false)
	double triggerprobability;	//generation probability for a trigger to an event
	int triggerdelay;			//delay of the trigger signal after the hit implantation
	int triggerlength;			//length of the trigger signal in timestamps
	bool triggerstate;			//state of the trigger, set to true if no triggers are added

	std::list<int> triggerturnontimes;
	int triggerturnofftime;

	tk::spline deadtime;
	std::vector<double> deadtimeX, deadtimeY;
	int pointsindtspline;
	tk::spline timewalk;
	std::vector<double> timewalkX, timewalkY;
	int pointsintwspline;
};


#endif //_EVENTGENERATOR
