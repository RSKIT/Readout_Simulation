#ifndef _PIXEL
#define _PIXEL

#include <string>
#include <fstream>

#include "hit.h"
#include "TCoord.h"

class Pixel
{
public:

	typedef TCoord<double> double3d;

    Pixel(double3d position, double3d size,
            std::string addressname, int address, double threshold);
	Pixel();

	/**
	 * @brief the origin of the active pixel volume (lower left back corner) in micrometers
	 * @details
	 * @return               - the position of the origin of the pixel
	 */
	double3d	GetPosition();
	void		SetPosition(TCoord<double> position);
	
	/**
	 * @brief the size of the active pixel volume in micrometers
	 * @details 
	 * @return               - the size of the active pixel volume
	 */
	double3d	GetSize();
	void		SetSize(TCoord<double> size);
	
	/**
	 * @brief the minimum charge to be deposited in the active pixel volume to create a hit,
	 *          provided in the unit as given by the charge scaling of the event generator
	 * @details
	 * @return               - the charge threshold for generating a hit in the pixel
	 */
	double 		GetThreshold();
	void		SetThreshold(double threshold);
	
	/**
	 * @brief the detection efficiency of the pixel. This introduces fluctuations into the system
	 * @details
	 * @return               - the detection efficiency of the pixel
	 */
	double 		GetEfficiency();
	void		SetEfficiency(double efficiency);

	/**
	 * @brief a factor to scale the dead time of the pixel compared to the standard value
	 *             calculated in the event generator
	 * @details
	 * @return               - the factor for the dead time scaling of this pixel
	 */
	double 		GetDeadTimeScaling();
	bool 		SetDeadTimeScaling(double factor);

	/**
	 * @brief the delay from the signal start time until the pixel is marked as hit. This parameter
	 *             is used by the event generator shifting the implant time for the hits generated
	 *             for this pixel
	 * @details
	 * @return               - the detection delay in timestamps
	 */
	double 		GetDetectionDelay();
	bool 		SetDetectionDelay(double delay);

	/**
	 * @brief the earliest time at which the pixel can again detect a new hit
	 * @details this parameter is to prevent implanting a new hit into the pixel when the previous
	 *             one is read out before the end of the dead time
	 * @return               - earliest time at which the next hit can be accepted
	 */
	double		GetDeadTimeEnd();
	void		SetDeadTimeEnd(double deadtimeend);

	/**
	 * @brief the identifying string for the address part identifying this pixel. Not the address
	 *             itself. The address name only provides a reference how to interpret the address
	 * @details
	 * @return               - name of the address field of this object
	 */
    std::string GetAddressName();
	void		SetAddressName(std::string addressname);
	
	/**
	 * @brief the address of the object. Only usable with the address name
	 * @details 
	 * @return               - actual address of this object for the address part given by 
	 * 							  `addressname`
	 */
	int 		GetAddress();
	void		SetAddress(int address);

	
	/**
	 * @brief determines whether the hit saved in this pixel is empty (/invalid) or not
	 * @details
	 * @return              - true for a valid hit, false on empty hit
	 */
	bool 		HitIsValid();
	/**
	 * @brief reads the hit from the pixel and deletes it from the pixel it the dead time of the
	 *             hit is over
	 * @details
	 * 
	 * @param timestamp      - the current timestamp at which the readout happens
	 * @return               - the hit object if it was valid and available or an empty hit object
	 */
	Hit 		GetHit(int timestamp = -1);
	/**
	 * @brief places a hit into the pixel after checking that it is not occupied
	 * @details
	 * 
	 * @param hit            - the hit object to place in the pixel
	 * @return               - true if the hit was placed inside the pixel, false if not
	 */
	bool		CreateHit(Hit hit);
	/**
	 * @brief removes a hit from the pixel. The dead time is not removed!
	 * @details
	 */
	void		ClearHit();
	/**
	 * @brief reads the hit from the pixel and deletes it in the pixel. The dead time is not 
	 * 			   removed!
	 * @details 
	 * 
	 * @param timestamp      - current timestamp at which the readout is to occur
	 * @return               - the hit object stored inside the pixel, or an empty/invalid hit
	 *                            object if it was empty
	 */	
	Hit 		LoadHit(int timestamp);

private:
	double3d 	position;
	double3d 	size;
	double 		threshold;
	double 		efficiency;
	double 		deadtimescaling;
	double 		detectiondelay;
	double 		deadtimeend;	//to store the dead time when the hit is read out before the 
								//  "signal" ends

	Hit 		hit;
	std::string addressname;
	int 		address;


};


#endif  //_PIXEL
