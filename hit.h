#ifndef _HIT
#define _HIT

#include <string>
#include <sstream>
#include <map>
#include <utility>

class Hit
{
public:
	Hit();
	Hit(const Hit& hit);

	/**
	 * @brief checks whether the hit belongs to an existing event
	 * @details 
	 * @return     - true if the event is valid, false if a parameter is not set
	 */
	bool 	is_valid();

	/**
	 * @brief provides the time stamp of the hit
	 * @details 
	 * @return     - the time stamp of the hit
	 */
	double 	GetTimeStamp();
	void 	SetTimeStamp(double timestamp);

	/**
	 * @brief provides the identifying index linking the hit to an event
	 * @details
	 * @return     - the event index
	 */
	int  	GetEventIndex();
	void 	SetEventIndex(int index);

	/**
	 * @brief provides the time when the pixel can again detect a hit after this one
	 * @details
	 * @return     - the point in time when the pixel is "armed" again
	 */
	double 	GetDeadTimeEnd();
	void 	SetDeadTimeEnd(double time);

	/**
	 * @brief provides the charge that generated this hit
	 * @details 
	 * @return     - the charge generating this hit
	 */
	double 	GetCharge();
	void	SetCharge(double charge);

	/**
	 * @brief adds an address-name - address pair to the hit
	 * @details
	 * 
	 * @param name - the name of this address (e.g. "Column")
	 * @param addr - the address going with this identifier
	 */
	void 	AddAddress(std::string name, int addr);
	/**
	 * @brief returns the address to the given identifier or an invalid address
	 * @details
	 * 
	 * @param name - the identifier to the address to return
	 * @return     - the address to the given identifier or "-1" on an invalid identifier
	 */
	int  	GetAddress(std::string name);
	/**
	 * @brief change an existing address part; to add a new address field, use AddAddress()
	 * @details
	 * 
	 * @param name - the identifier to the address part to change
	 * @param addr - the new value for the address part given by `name`
	 * 
	 * @return     - true on success, false if the identifier `name` was not found
	 */
	bool 	SetAddress(std::string name, int addr);
	/**
	 * @brief provides the number of parts of the address
	 * @details
	 * @return     - the number of parts in the address (e.g. pixel address, column address => 2)
	 */
    int  	AddressSize();
    /**
     * @brief delete all address parts
     * @details
     */
	void 	ClearAddress();

	/**
	 * @brief adds a timestamp for the readout into a higher level buffer
	 * @details
	 * 
	 * @param name - address part identifier where the hit is read out to (e.g. "Column" for 
	 *                 shifting to the column buffer)
	 * @param timestamp - the time stamp when the hit is transferred to the structure
	 */
	void 	AddReadoutTime(std::string name, int timestamp);
	/**
	 * @brief returns the readout time for the structure with the passed address identifier
	 * @details
	 * 
	 * @param name - address part identifier
	 * @return     - the readout time stamp or "-1" on an invalid address part identifier
	 */
	int 	GetReadoutTime(std::string name);
	/**
	 * @brief changes an existing readout time stamp
	 * @details
	 * 
	 * @param name - the address part identifier of the entry to change
	 * @param timestamp - the new time stamp for this readout address part
	 * 
	 * @return     - true on successful change, false if the address part identifier was not found
	 */
	bool 	SetReadoutTime(std::string name, int timestamp);
	/**
	 * @brief returns the number of readout time stamps
	 * @details 
	 * @return     - the number of readout time stamps
	 */
	int 	ReadoutTimeSize();
	/**
	 * @brief deletes all readout time stamps from this hit
	 * @details 
	 */
	void 	ClearReadoutTimes();

	/**
	 * @brief generates a string containing all information of the hit
	 * @details
	 * 
	 * @param compact   - if false, the address part identifier names are included, if false
	 *                       they are skipped
	 * @return     - a string containing the hit information
	 */
	std::string GenerateString(bool compact = false);
private:
	int 	eventindex;
	double 	timestamp;
	double 	deadtimeend;
	double 	charge;

	std::map<std::string, int> address;
	std::map<std::string, int> readouttimestamps;
};

#endif //_HIT
