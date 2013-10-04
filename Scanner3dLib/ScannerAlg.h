#pragma once

#include <cv.h>
#include <highgui.h>
#include "scannerconfig.h"
#include "point3d.hpp"
#include "ScannerFrame.h"
#include "plane.h"

/*
A little about this algorithm:
for starters, it's using open cv
The config holds an image of the object to be scanned with no laser as a reference
coordinates are in mm

order of ops:

StartScan is called (opens point file) SetConfig () // with the camera and initial image, and output filename
ProcessFrame is called for X frames, each with a current Z rotation on the platform
ProcessFrame (if successful) will produce a Frame object
the Frame object holds:
	z rotation
	list of 3d unprojected points
	index #
	original image

EndScan is called

*/
class ScannerAlg
{
private:
	bool m_scanning;
public:
	List *m_pFrames; // list of frames generated

	ScannerConfig *pConfig;
	ScannerAlg();
	~ScannerAlg();

	Color GetColor(int xpos,int ypos);
	virtual void StartScan();
	bool IsScanning(){return m_scanning;}
	virtual void ProcessFrame(float zrot){}	
	virtual int FindLaser(IplImage *diffFrame, int pos){return 0;}
	virtual bool FindLaserPlane(IplImage *diffFrame, Plane *pl){return false;}
	virtual void EndScan();
	virtual void CreateDefaultConfiguration(){}
	virtual bool SaveConfiguration(){return false;}
	virtual bool LoadConfiguration(){return false;}

	bool PlaneIntersect(Plane *plane,Point2D pos,point_3d *pnt_intersect);
	void ClearData();
};
