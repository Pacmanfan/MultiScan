#include "ScannerAlgCorner.h"
#include "ScannerConfigCorner.h"
#include "improc.h"
#include "LeastSquares.h"

ScannerAlgCorner::ScannerAlgCorner(void)
{
	pConfig = new ScannerConfigCorner();	
}

ScannerAlgCorner::~ScannerAlgCorner(void)
{
}


// the corner algorithm that works horizontally
// look at the left and right side of the image
// determine the laser plane
//iterate through all the columns (xpos)
//and find the laser intersection point
//intersect the screen coordinate with the laser plane
// get the points and store.

void ScannerAlgCorner::ProcessFrame(float zrot)
{
	// get the current processed frame
	IplImage * diffImage = ImProc::Instance()->GetTemporalDiff();

	//frame has already been converted to greyscale or canny here
	Plane laserplane;
	if(FindLaserPlane(diffImage,&laserplane))
	{
		//create a new scanner frame to hold some data
		ScannerFrame *sf = new ScannerFrame();
		sf->m_zrot = zrot;
		Point2D p2d; // a temporariy 2d point
		point_3d intersectcurrent; // the solved point of intersection

		//alright, we've found the plane of the laser
		//now iterate through and determine the 3d points
		for(p2d.X = SCANNERINSET; p2d.X < diffImage->width - SCANNERINSET ; p2d.X++)
		{
			p2d.Y = FindLaser(diffImage,p2d.X);
			if(p2d.Y == -1)
				continue; // skip, no laser found

			if(PlaneIntersect(&laserplane,p2d,&intersectcurrent))
			{
				//we should probably check to see that the point isn't waaaaay off in the distance
				//create a new point
				point_3d *saved = new point_3d(intersectcurrent.Wx,intersectcurrent.Wy,intersectcurrent.Wz);
				saved->m_p2d = p2d; // save the original 2d position for later optimization
				saved->m_color = GetColor(p2d.X,p2d.Y); // get the color value
				sf->m_pPoints->Add(saved);
			}
		}
		if(sf->m_pPoints->Count() > 0)
		{
			m_pFrames->Add(sf);
		}
		else
		{
			//no data in this frame
			delete sf;
		}
	}
}



/*
	Find the y position of the laser on the specified x line
	in the diffFrame image
	return -1 if not found
	now here, we can look for the topmost,bottomost,center, or brightest pixel
	to make it easy starting off, let's look for the topmost that exceeds
	the threshold.
	if we're using the canny algorithm, just look for the top
*/

int ScannerAlgCorner::FindLaser(IplImage *diffFrame, int xpos)
{
	//get a pointer to the data
	unsigned char *data = (unsigned char *)diffFrame->imageData;
	unsigned char dat;

	unsigned char brightest = 0 ;
	int brightestYpos  = -1;

	for(int ypos = 0 ; ypos < diffFrame->height; ypos++)
	{
		dat = data[(ypos * diffFrame->widthStep) + xpos]; // current pixel data
		if(dat >= pConfig->m_brightnessthreshold)
		{
			//if we're using the canny algorithm, just look for the leftmost, because it's a 2 color image
			if(pConfig->m_usecanny)
			{				
				return ypos;//find the leftmost
			}else{ // find the brightest
				//found a bright spot
				if(dat > brightest)
				{
					brightest = dat;
					brightestYpos = ypos;
				}
			}
		}
	}
	return brightestYpos;
}
/*
 look at the left SCANNERINSET pixels
 find the positions
 throw out any outliers
 generate a least squares line that describes the line
 pick 2 points from that line 
 do the same for the right side
 pick 2 points
 convert to screen->camera->world
 use the world points to generate the plane
*/
bool ScannerAlgCorner::FindLaserPlane(IplImage *diffFrame, Plane *pl)
{

	List leftpnts,rightpnts;
	float left_m,left_b,right_m,right_b;
	Point2D left_tmppnts[SCANNERINSET];
	Point2D right_tmppnts[SCANNERINSET];
	Point2D L1,L2,R1,R2; // the line segment that describes the slope on the left
	for(int xpos = 0; xpos <SCANNERINSET ; xpos ++)
	{
		int l_ypos = FindLaser(diffFrame,xpos); // the left side
		int r_ypos = FindLaser(diffFrame,xpos + (diffFrame->width - SCANNERINSET )); // the right side
		if(l_ypos != -1)
		{
			left_tmppnts[xpos].Set(xpos,l_ypos);
			leftpnts.Add(&left_tmppnts[xpos]); // add only valid points
		}
		if(r_ypos != -1)
		{
			right_tmppnts[xpos].Set(xpos + (diffFrame->width - SCANNERINSET ),r_ypos);
			rightpnts.Add(&right_tmppnts[xpos]); // add only valid points
		}
	}
	//we should check the list here and see if there are enough points to determine a plane
	if(leftpnts.Count() < (SCANNERINSET/2)) // not enough points to get a good slope
		return false;
	// we should probably check for outliers as well to make sure it's a good slope
	FindLeastSquare(&leftpnts,&left_m,&left_b); // find the slope
	L1.Set(0,(left_m * 0) + left_b);
	L2.Set(100,(left_m * 100) + left_b);
	//and for the right side

	if(rightpnts.Count() < (SCANNERINSET/2)) // not enough points to get a good slope
		return false;
	// we should probably check for outliers as well to make sure it's a good slope
	FindLeastSquare(&rightpnts,&right_m,&right_b); // find the slope
	R1.Set((diffFrame->width - 100),(right_m * (diffFrame->width - 100)) + right_b);
	R2.Set(diffFrame->width,(right_m * diffFrame->width) + right_b);
	//clear the lists
	leftpnts.Destroy();
	rightpnts.Destroy();
	// now we've got 4 points, 2 from the left plane, 2 from the right
	// unproject them to go from screen-camera
	point_3d vertices[4];
	//order L1 - L2 - R1 - R2
	ScannerConfigCorner *cfg = (ScannerConfigCorner *)pConfig;
	// then untransform them to go from camera to world
	if(!PlaneIntersect(&cfg->m_leftcorner,L1,&vertices[0]))
			return false;
	if(!PlaneIntersect(&cfg->m_leftcorner,L2,&vertices[1]))
			return false;
	if(!PlaneIntersect(&cfg->m_rightcorner,R1,&vertices[2]))
			return false;
	if(!PlaneIntersect(&cfg->m_rightcorner,R2,&vertices[3]))
			return false;
	//now we can calculate the Plane
	pl->CalculatePlaneEquation(vertices,4);
	return true;
}

void ScannerAlgCorner::CreateDefaultConfiguration()
{
	pConfig->CreateDefault();
}
bool ScannerAlgCorner::SaveConfiguration()
{
	FILE *fp = fopen("Corner.cfg","wb");
	if(fp)
	{
		pConfig->Save(fp);
		fclose(fp);
		return true;
	}
	return false;
}
bool ScannerAlgCorner::LoadConfiguration()
{
	FILE *fp = fopen("Corner.cfg","rb");
	if(fp)
	{
		pConfig->Load(fp);
		fclose(fp);
		return true;
	}
	return false;
}