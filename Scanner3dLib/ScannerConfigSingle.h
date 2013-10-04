#pragma once
#include "scannerconfig.h"

class ScannerConfigSingle :
	public ScannerConfig
{
public:

	/* the vertical background aligned on the world X/Z plane 
	  this is directly infront of the camera's z vector
	*/

	Plane m_reference;
	// the camera is at 0,0,0 and is looking at world position 0,1,0
	// with an up vector of 0,0,1
	
	/* position of the laser in world coordinates, usually to the right of the camera
	*/
	point_3d m_laserpos; 

	// assume that the laser line is vertical against the back plane
	// otherwise, the tilt is calculated from the top 25 pixels
	bool m_assumelaservertical; 

	ScannerConfigSingle(void);
	~ScannerConfigSingle(void);


	void SetReferencePlaneDist(float mm)
	{
		// look back at the camera by looking down the -Y vector,
		// the distance (d) portion is the distance from the 0,0,0 origin
		m_reference.Set(0,-1,0,mm);
	}

	void SetBrightnessThreshold(unsigned char threshold)
	{
		m_brightnessthreshold = threshold;
	}
	void SetLaserPosition(float x, float y, float z)
	{
		m_laserpos.Set(x,y,z);
		 // the camera position is the same in the world and camera coordinate spaces
		m_laserpos.Cx =x;
		m_laserpos.Cy =y;
		m_laserpos.Cz =z;
	}
	void CreateDefault();
	bool Load(FILE *fp);
	bool Save(FILE *fp);
};
