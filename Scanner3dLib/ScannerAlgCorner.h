#pragma once

#include "ScannerAlg.h"
#include "ScannerConfigCorner.h"

class ScannerAlgCorner : public ScannerAlg
{
public:

	ScannerAlgCorner(void);
	~ScannerAlgCorner(void);
	void ProcessFrame(float zrot);
	int FindLaser(IplImage *diffFrame, int pos);
	bool FindLaserPlane(IplImage *diffFrame, Plane *pl);
	void CreateDefaultConfiguration();
	bool SaveConfiguration();
	bool LoadConfiguration();
};