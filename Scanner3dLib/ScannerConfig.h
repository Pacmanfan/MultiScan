#pragma once
#include "cv.h"
#include "highgui.h"// For utility function: convert one image to another with optional vertical flip.
#include "stdio.h"
#include "plane.h"
#include "camera.h"
#include "point3d.hpp"

enum eScantype
{
	eTop25 = 0, //generate the laser plane by looking at the top 25 pixels
	eLeftRightCorner = 1, // look on the left, right side of the object for laser line
};

class ScannerConfig
{
public:

	// all algorithms need a camera
	camera m_camera;
	/*
		The brightness threshhold is used to determine
		if pixels on the subtracted image are bright enough to process
	*/
	unsigned char m_brightnessthreshold;

	//we'll see how canny pans out, i might need a different edge-finding function
	bool m_usecanny;
	double m_canny_threshold1;
	double m_canny_threshold2;
	int m_canny_apertureSize;

	eScantype m_scantype;

	ScannerConfig(void);
	~ScannerConfig(void);

	virtual void CreateDefault()= 0 ; // set up for a specific algorithm
	//void SetAlgType(eScantype st);

	virtual bool Save(FILE *fp);
	virtual bool Load(FILE *fp);
};
