#include "ScannerConfigCorner.h"

ScannerConfigCorner::ScannerConfigCorner(void)
{
	m_scantype = eLeftRightCorner;
	CreateDefault();
}

ScannerConfigCorner::~ScannerConfigCorner(void)
{
}
void ScannerConfigCorner::CreateDefault()
{
	point_3d lookat;
	//m_inset = 40;//
	m_camera.global_view.Initialize();
	// the Right is pointing in the +X direction
	m_rightcorner.Set(1,0,0,0);
	// the Left points to the +Y direction
	m_leftcorner.Set(0,1,0,0);
	// the camera is no longer the origin in this alg
	// it should be on the +Y,+X axis, looking at the origin
	m_camera.SetPosition(200,200,0);
	lookat.Set(0,0,0);
	Vector3d up;
	up.Set(0,0,1);
	m_camera.LookAt(&lookat,&up);
	/*
	(right)
		+Y
		|			(camera)
		|      top down view of axises in world space
		|->    Z goes up
		|
		|		^
		|		|
		Z--------------- +X (left)
	*/
}

bool ScannerConfigCorner::Save(FILE *fp)
{
	m_leftcorner.Save(fp);
	m_rightcorner.Save(fp);	
	return true;
}

bool ScannerConfigCorner::Load(FILE * fp)
{
	m_leftcorner.Load(fp);
	m_rightcorner.Load(fp);
	return true;
}