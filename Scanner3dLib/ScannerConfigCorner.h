#pragma once
#include "scannerconfig.h"
// how many pix to look into on each side of the image
// to determine the plane of the laser
#define SCANNERINSET 50 

class ScannerConfigCorner :
	public ScannerConfig
{
public:

	//static const int m_inset = 50; // the pixel distance on either side to examine to determine the slope
	Plane m_leftcorner;
	Plane m_rightcorner;

	ScannerConfigCorner(void);
	~ScannerConfigCorner(void);
	void CreateDefault();
	bool Load(FILE *fp);
	bool Save(FILE *fp);
};
