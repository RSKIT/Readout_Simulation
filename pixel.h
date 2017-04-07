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

	
	double3d	GetPosition();
	void		SetPosition(TCoord<double> position);
	
	double3d	GetSize();
	void		SetSize(TCoord<double> size);
	
	double 		GetThreshold();
	void		SetThreshold(double threshold);
	
	double 		GetEfficiency();
	void		SetEfficiency(double efficiency);

	double 		GetDeadTimeScaling();
	bool 		SetDeadTimeScaling(double factor);

	double 		GetDetectionDelay();
	bool 		SetDetectionDelay(double delay);

	double		GetDeadTimeEnd();
	void		SetDeadTimeEnd(double deadtimeend);

    std::string GetAddressName();
	void		SetAddressName(std::string addressname);
	
	int 		GetAddress();
	void		SetAddress(int address);

	
	bool 		HitIsValid();
	Hit 		GetHit(int timestamp = -1);
	bool		CreateHit(Hit hit);
	void		ClearHit();
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
