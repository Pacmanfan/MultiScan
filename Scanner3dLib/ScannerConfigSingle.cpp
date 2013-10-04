#include "ScannerConfigSingle.h"

ScannerConfigSingle::ScannerConfigSingle(void)
{
	m_scantype = eTop25;
	// create defaults here
	CreateDefault();
}

ScannerConfigSingle::~ScannerConfigSingle(void)
{

}

void ScannerConfigSingle::CreateDefault()
{
	m_brightnessthreshold = 128;
	SetLaserPosition(200,0,0); // 200 mm to the left of the camera
	m_assumelaservertical = false;
	
	m_usecanny = false;
	m_canny_threshold1 = 10;
	m_canny_threshold2 = 100;
	m_canny_apertureSize = 3;

	point_3d lookat;
	Vector3d up;

	m_reference.Set(0,-1,0,300);
	m_camera.global_view.Initialize();		
	m_camera.viewing_distance = 500;
	lookat.Set(0,100,0); // look down the world y axis
	up.Set(0,0,1);
	m_camera.LookAt(&lookat,&up);
	m_camera.global_view.Rotate(.5,.5,0); // need to make this askew because of a matix pivot / inverse issue
	/*
		In this alg, the wall is at the walldist away, and the camera
		is considered to be 0,0,0 origin.
		------------------------------ +X (reference plane)
		|				|
		|				V
		|
		|
		|
		|				^
		|				|
		-Y			(camera)
					 origin (0,0,0)
	*/

}

bool ScannerConfigSingle::Save( FILE *fp)
{
	ScannerConfig::Save(fp);
	m_reference.Save(fp);
	m_laserpos.Save(fp);
	fwrite(&m_assumelaservertical,sizeof(m_assumelaservertical),1,fp);
	return true;
}

bool ScannerConfigSingle::Load(FILE *fp)
{
	ScannerConfig::Load(fp);
	m_reference.Load(fp);
	m_laserpos.Load(fp);
	fread(&m_assumelaservertical,sizeof(m_assumelaservertical),1,fp);
	return true;
}