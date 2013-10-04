
#include "ScannerAlg.h"
#include "rtutil.hpp"
#include "improc.h"
// local function for unprojecting a 2d point back to 3d
void UnProject(Point2D &p,point_3d *out, camera *cam, int Wid,int Hei);

ScannerAlg::ScannerAlg()
{
	Build_Look_Up_Tables();
	m_pFrames = new List();
	m_scanning = false;
}

ScannerAlg::~ScannerAlg()
{
	ClearData();
	delete m_pFrames;
}

void ScannerAlg::StartScan()
{
	//clear any old data
	ClearData();
	m_scanning = true;
}



/*
This is a generalized plane intersection routine
It can be used for the back reference plane, or
for the laserplane intersections.

*/
bool ScannerAlg::PlaneIntersect(Plane *plane,Point2D pos,point_3d *pnt_intersect)
{
	bool retval = false;
	point_3d raypoint; // a point we use to create the ray
	Vector3d direction; //the ray 
	point_3d cam_pos; //position of the camera
	pConfig->m_camera.GetPosition(&cam_pos); // I think this should work now...
	// use the Camera world Z cordinate to unproject
	raypoint.Cz = 1; // look into the sceen
	IplImage *pRefImage = ImProc::Instance()->GetCurFrame();
	UnProject(pos,&raypoint,&pConfig->m_camera,pRefImage->width,pRefImage->height);
	raypoint = pConfig->m_camera.global_view.Untransform(raypoint); // camera to world
	direction = raypoint - cam_pos; // create a vector
	direction.Normalize(); //and normalize it to a length of 1 
	// now intersect this with the plane and take that point
	if(IntersectPlane(plane,&cam_pos,&direction,pnt_intersect))
	{
		retval = true; // mark it as a valid intersection
	}
	return retval;
}




void ScannerAlg::EndScan()
{
	m_scanning = false;
}

/*
This function clears all the data in the frames
*/
void ScannerAlg::ClearData()
{
	for(int c = 0; c< m_pFrames->Count(); c ++)
	{
		ScannerFrame *sf = (ScannerFrame *)m_pFrames->GetItem(c);
		delete sf; // free up the memory
	}
	m_pFrames->Destroy(); //remove all entries in the list
}

/*
Assumes BGR image
*/
Color ScannerAlg::GetColor(int xpos,int ypos)
{
	Color tmp;
	//look at the color reference image and get the specified color
	IplImage *pRefImage = ImProc::Instance()->GetCurFrame();
	int widthstep = pRefImage->widthStep;
	unsigned char * dat = (unsigned char *)pRefImage->imageData;
	tmp.B = dat[(ypos * widthstep) + (xpos*3)];
	tmp.G = dat[(ypos * widthstep) + (xpos*3)+1];
	tmp.R = dat[(ypos * widthstep) + (xpos*3)+2];
	return tmp;
}

