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

#ifndef _SIMULATOR
#define _SIMULATOR

#include <vector>
#include <chrono>

#include "detector.h"
#include "xmldetector.h"
#include "EventGenerator.h"
#include "tinyxml2.h"
#include "tinyxml2_addon.h"
#include "TCoord.h"
#include "zip_file.h"

class Simulator
{
public:
	typedef std::chrono::steady_clock::time_point TimePoint;

	enum datatypes {GenerateNewEvents=0, PixelHitFile=1, ITkFile=2};
	struct eventdata{
		eventdata() : datatype(-1), source(""), starttime(0.), numevents(0), firstevent(0), 
				eta(0), sort(false), distance(0) {}
		int datatype;
		std::string source;
		double starttime;
		int numevents;
		int firstevent;
		int eta;
		bool sort;
		double distance;
	};

	Simulator();
	/**
	 * @brief constructor setting the filename for the input file: This is an XML file containing
	 *             information about the detector geometry, optionally the state machine and the
	 *             simulation parameters.
	 * @details
	 * 
	 * @param filename       - filename for the input file to load from
	 */
	Simulator(std::string filename);

	/**
	 * @brief returns the name of the file to load the configuration from
	 * @details
	 * @return               - the file name of the input file
	 */
	std::string GetLoadFileName();
	/**
	 * @brief sets a new input file name and loads the files contents, creating a new detector,
	 *             eventgenerator
	 * @details
	 * 
	 * @param filename       - the new file name to load from, or - if the string is empty - the
	 *                            previously set filename is used to load the data      
	 */
	void LoadInputFile(std::string filename = "");

	/**
	 * @brief provides the name of the file to write logging data to
	 * @details
	 * @return               - the filename of the logging file
	 */
	std::string GetLoggingFile();
	void SetLoggingFile(std::string filename);

	/**
	 * @brief flag whether the detector structure is written to the log file or not
	 * @details
	 * @return               - writes the detector structure to the log file if true
	 */
	bool GetDetectorLogging();
	void SetDetectorLogging(bool printdetector);

	/**
	 * @brief provides the name of the archive to save the data to
	 * @details
	 * @return               - the file name of the archive
	 */
	std::string GetArchiveName();
	void SetArchiveName(std::string archivename);
	/**
	 * @brief provides the information whether the data is saved only in the archive or in normal
	 *             files (additionally to the archive if an archive filename is provided)
	 * @details
	 * @return               - true if only the archive is to be used, false for normal files
	 */
	bool GetArchiveOnly();
	void SetArchiveOnly(bool archiveonly);

	/**
	 * @brief provides the number of events to generate using the event generator
	 * @details
	 * @return               - the number of events (not pixel hits)
	 */
	int GetNumEventsToGenerate();
	void SetNumEventsToGenerate(int events);

	/**
	 * @brief provides the earliest time for the first event to occur
	 * @details
	 * @return               - the earliest point in time at which the first event could occur
	 */
	int GetStartTime();
	void SetStartTime(int starttime);

	eventdata* GetEventOrder(int index = 0);
	void       AddEventOrder(eventdata order);
	void       RemoveEventOrders();

	/**
	 * @brief provides the stop time of the simulation. If no stop time is set the result is -1
	 * @details 
	 * @return               - the stop time of the simulation or -1 if no stop time is set
	 */
	int GetStopTime();
	void SetStopTime(int stoptime);
	/**
	 * @brief removes the stop time by setting the stop time to an invalid value (-1)
	 * @details
	 */
	void RemoveStopTime();

	/**
	 * @brief provides the number of timestamps to delay the end of the simulation if the end
	 *             condition (without a stoptime) is reached
	 * @details
	 * @return               - the delay of the end of the simulation as the number of timestamps
	 */
	int GetStopDelay();
	void SetStopDelay(int stopdelay);

	/**
	 * @brief provides a pointer to a detector in this simulator addressed by its address
	 * @details
	 * 
	 * @param address        - the address of the detector to find
	 * @return               - a pointer to the detector to find or a NULL pointer if no detector
	 *                            with the passed address was found
	 */
	DetectorBase* GetDetector(int address);
	/**
	 * @brief adds the detector passed to the simulator. No copy is created, the object itself is
	 *             used
	 * @details
	 * 
	 * @param detector       - pointer to the detector to add
	 */
	void AddDetector(DetectorBase* detector);
	/**
	 * @brief removes all detectors from the simulator
	 * @details
	 */
	void ClearDetectors();
	/**
	 * @brief returns the number of detectors in this simulator
	 * @details
	 * @return               - the number of detectors in this simulator
	 */
	int GetNumDetectors();

	/**
	 * @brief provides access to the event generator of this simulator
	 * @details
	 * @return               - a pointer to the event generator of this object
	 */
	EventGenerator* GetEventGenerator();
	/**
	 * @brief uses the parameters for number of events,... to generate the events with the event
	 *             generator or generates output to the console if somethind went wrong in the
	 *             configuration of the event generator
	 * @details
	 */
	void InitEventGenerator();
	/**
	 * @brief Initiates the generation of the events in the generation queue
	 * @details
	 * 
	 * @param events		 - the number of additional events to generate with the simple 
	 *                            generator
	 * @param timestamp  	 - earliest possible point in time for the first event. For negative
	 *                            numbers the setting of the simulator object is used. Only applies
	 *                            to the newly generated `events` events
	 */
	void GenerateEvents(int events = 0, double starttime = -1);

	/**
	 * @brief calls the ClockUp() Method for all detectors and provides the trigger signal to them
	 * @details
	 * 
	 * @param timestamp      - the current timestamp
	 * @return               - true on successful execution, false on an error
	 */
    bool ClockUp(int timestamp);
    /**
     * @brief calls the ClockDown() method for all detectors and provides the trigger signal to them
     * @details
     * 
     * @param timestamp      - the current timestamp
     * @return               - true on successful execution, false on an error
     */
    bool ClockDown(int timestamp);

    /**
     * @brief Starts the simulation using stopping conditions as passed
     * @details 
     * 
     * @param stoptime       - the stop time for the simulation, -1 is an invalid stop time
     *                            indicating that the simulation is to end on all detectors being
     *                            empty and no more hits left to insert
     * @param delaystop      - the delay of the stopping after the stopping condition is fulfilled
     *                            in case the stoptime is set to -1
     */
    void SimulateUntil(int stoptime = -1, int delaystop = 0);

    /**
     * @brief generates a string describing all detectors including newlines
     * @details
     * @return               - a string (including newlines) describing the geometric structure of
     *                            all detectors in the simulator
     */
    std::string PrintDetectors();

private:
	//=== Detector Geometry and Event Generator Loading ==
	/**
	 * @brief generates a detector from the XML tree with root node `parent` and adds it to the
	 *             simulator.This is a recursive function calling the respective others for child
	 *             objects by itself.
	 * @details
	 * 
	 * @param parent         - root node of the xml tree describing only the detector of interest
	 * @param pixelsize      - the standard pixel size for the detector to save effort defining
	 *                            the pixels in the XML file
	 */
	void 			LoadDetector(tinyxml2::XMLElement* parent, TCoord<double> pixelsize);
	/**
	 * @brief converts a vector from XML to a usable C++ class object
	 * @details
	 * 
	 * @param coordinate     - the XML node defining the vector
	 * @return               - C++ class representation of the vector
	 */
	TCoord<double> 	LoadTCoord(tinyxml2::XMLElement* coordinate);
	/**
	 * @brief converts the XML tree structure into an event generator and adds it to the simulator
	 * @details
	 * 
	 * @param eventgen       - the root node of the XML tree describing only the event generator
	 */
	void 			LoadEventGenerator(tinyxml2::XMLElement* eventgen);
	/**
	 * @brief Loads a spline function from the XML tree
	 * @details
	 * 
	 * @param eventgen       - the event generator to load the spline to
	 * @param element        - parent element of the spline function
	 */
	void 			LoadSpline(EventGenerator* eventgen, tinyxml2::XMLElement* element);
	/**
	 * @brief converts the XML tree structure into a ReadoutCell. This is a recursive function
	 *             calling the respective others for child objects by itself.
	 * @details
	 * 
	 * @param parent         - the root node of the XML tree describing only the readoutcell
	 * @param pixelsize      - the standard pixel size to use when no pixel size is provided
	 * @param defaultaddressname
	 * 						 - address name to use for child objects if they do not provide a
	 * 						      different one to avoid confusions with duplicate names
	 * 						      
	 * @return               - a readoutcell object to be added to a detector or another 
	 * 							  readoutcell
	 */
	ReadoutCell 	LoadROC(tinyxml2::XMLElement* parent, TCoord<double> pixelsize, 
								std::string defaultaddressname = "ROC");
	/**
	 * @brief converts the XML tree structure representing a pixel into a pixel object
	 * @details 
	 * 
	 * @param parent         - root node of the XML tree representing only the pixel
	 * @param pixelsize      - the standard pixel size to use if no size is provided by the
	 *                            XML tree
	 * 
	 * @return               - the pixel to include in a readoutcell
	 */
	Pixel 			LoadPixel(tinyxml2::XMLElement* parent, TCoord<double> pixelsize);
	/**
	 * @brief loads the configuration of the combinatory PPtB readoutcell
	 * @details
	 * 
	 * @param parent         - root node of the XML tree representing the combinatory logic
	 * @return               - a pointer to the logic description object
	 */
	PixelLogic*     LoadPixelLogic(tinyxml2::XMLElement* parent);
	/**
	 * @brief loads a structure of pixels and readoutcells and copies it shifting the copies by
	 *             a specified vector
	 * @details
	 * 
	 * @param parentcell     - the readoutcell to which the generated structures are to be added
	 *                            to
	 * @param parentnode     - the root node of the structure describing the copy pattern and the
	 *                            objects to copy 
	 * @param pixelsize      - the standard pixel size for pixels not providing their own 
	 *                            pixelsize
	 * @param globalshift    - vector for shifting the whole structure. This is necessary when
	 *                            those copy blocks are direct child objects of each other
	 */
	void 			LoadNPixels(ReadoutCell* parentcell, tinyxml2::XMLElement* parentnode, 
						TCoord<double> pixelsize, 
						TCoord<double> globalshift = TCoord<double>::Null);

	//=== Statemachine Loading ===
	/**
	 * @brief converts an XML tree structure into a state machine for a detector. This method is
	 *             called by the LoadDetector() method if needed. If it is needed, the Detector
	 *             object is replaced by an XMLDetector object capable of handling a dynamically
	 *             loaded state machine
	 * @details
	 * 
	 * @param detector       - a pointer to the detector to add the state machine to. This object
	 *                            has to be replaced by the object referred to by the return value
	 * @param statemachine   - root node of the structure representing the state machine
	 * 
	 * @return               - a pointer to a new detector object which is an XMLDetector object
	 */
	XMLDetector* 	LoadStateMachine(DetectorBase* detector, tinyxml2::XMLElement* statemachine);
	/**
	 * @brief converts a state representation in the XML tree to a C++ class usable by the 
	 *             XMLDetector
	 * @details
	 * 
	 * @param stateelement   - the root node of the tree structure representing the state machine
	 *                            state
	 * @return               - a pointer to the generated state machine state
	 */
	StateMachineState* 	LoadState(tinyxml2::XMLElement* stateelement);
	/**
	 * @brief converts an XML action representation into a C++ class object
	 * @details
	 * 
	 * @param registerchange - the root node of the XML tree representing the action
	 * @return               - the generated RegisterAccess object
	 */
	RegisterAccess 		LoadRegisterChange(tinyxml2::XMLElement* registerchange);
	/**
	 * @brief converts a state transition from XML tree structure to a C++ class
	 * @details
	 * 
	 * @param transition     - the root node of the tree structure representing the state
	 *                            transition
	 * @return               - a pointer to the generated state transition
	 */
	StateTransition* 	LoadStateTransition(tinyxml2::XMLElement* transition);
	/**
	 * @brief converts an XML representation of a comparison into a C++ class representation.
	 *             The function call is recursive to enable nested comparisons.
	 * @details
	 * 
	 * @param comparison     - the root node of the tree structure representing the comparison
	 * @return               - a pointer to the generated comparison object
	 */
	Comparison*			LoadComparison(tinyxml2::XMLElement* comparison);

	/**
	 * @brief converts time differences measured in microseconds to test strings showing the
	 *             time ina humen readable format
	 * @details
	 * 
	 * @param start          - start point in time of the interval to evaluate
	 * @param end            - end point in time of the interval to evaluate
	 * 
	 * @return               - a text representation of the time interval in human readable format
	 */
	std::string 		TimesToInterval(TimePoint start, TimePoint end);

    std::vector<DetectorBase*> detectors;
    EventGenerator eventgenerator;
    //event generation parameters for the event generator:
    int events;			//number of events
    double starttime;	//earliest time possible for the first event
    std::vector<eventdata> eventstoload; //list of events to load/generate

/*
	struct eventdata{
		std::string source;
		double starttime;
		int numevents;
		int firstevent;
		int eta;
	}
*/

    //end conditions for the simulation:
    int stoptime;
    int stopdelay;

    std::string inputfile;
    std::stringstream inputfilecontent;

    std::string logfile;	//name of the file to write logging texts to
    std::stringstream logcontent;
    bool printdetector;

    std::string archivename;	//filename for the archive to save the data to
    bool archiveonly;			//determines whether the data is also saved using normal files or not

};


#endif  //_SIMULATOR
