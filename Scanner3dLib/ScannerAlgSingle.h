#pragma once
/*
This class encapsulates the 3d laser line scanner algorithm that
utilizes a single backplane. the Top 25 pixels along with the known position of the laser 
are used to determine the plane of the laser line across the object.
*/
#include "ScannerAlg.h"
#include "scannerconfigsingle.h"

class ScannerAlgSingle :public ScannerAlg
{
public:

	ScannerAlgSingle(void);
	~ScannerAlgSingle(void);
	void ProcessFrame(float zrot);
	int FindLaser(IplImage *diffFrame, int pos);
	bool FindLaserPlane(IplImage *diffFrame, Plane *pl);
	void CreateDefaultConfiguration();
	bool SaveConfiguration();
	bool LoadConfiguration();
};
