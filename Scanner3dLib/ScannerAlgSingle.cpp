#include "ScannerAlgSingle.h"
#include "improc.h"

ScannerAlgSingle::ScannerAlgSingle(void)
{
	pConfig = new ScannerConfigSingle();
}

ScannerAlgSingle::~ScannerAlgSingle(void)
{
}

// the simpler alg that works vertically
void ScannerAlgSingle::ProcessFrame(float zrot)
{
	//get the current diff image
	IplImage * diffImage = ImProc::Instance()->GetTemporalDiff();
	if(diffImage == 0) // must be first frame, bail
		return;
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
		for(p2d.Y = 25; p2d.Y < diffImage->height;p2d.Y++)
		{
			p2d.X = FindLaser(diffImage,p2d.Y);
			if(p2d.X == -1)
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
	Find the x position of the laser on the specified y line
	in the diffFrame image
	return -1 if not found
	now here, we can look for the rightmost,leftmost,center, or brightest pixel
	to make it easy starting off, let's look for the leftmost that exceeds
	the threshold.
	//if we're using the canny algorithm, just look for the left
*/

int ScannerAlgSingle::FindLaser(IplImage *diffFrame, int ypos)
{
	//get a pointer to the data
	unsigned char *data = (unsigned char *)diffFrame->imageData;
	unsigned char dat;

	unsigned char brightest = 0 ;
	int brightestXpos  = -1;

	for(int xpos = 0 ; xpos < diffFrame->width; xpos++)
	{
		dat = data[(ypos * diffFrame->widthStep) + xpos]; // current pixel data
		if(dat >= pConfig->m_brightnessthreshold)
		{
			//if we're using the canny algorithm, just look for the leftmost, because it's a 2 color image
			if(pConfig->m_usecanny)
			{				
				return xpos;//find the leftmost
			}else{ // find the brightest
				//found a bright spot
				if(dat > brightest)
				{
					brightest = dat;
					brightestXpos = xpos;
				}
			}
		}
	}
	return brightestXpos;
}


/*
	This function will find the plane of the laser that 
	intersects the back wall
	find the laser in the top 25 pix of the diff image,
	this will be used to create a plane
	from the laser to the vertical background reference plane
	an int array so we can determine the plane of the laser
	we're not going to assume it's vertical <- maybe we should!
*/
bool ScannerAlgSingle::FindLaserPlane(IplImage *diffFrame, Plane *pl)
{
	bool retval = false;
	ScannerConfigSingle *cfg = (ScannerConfigSingle *)pConfig;
	int top25Xpos[25];
	Point2D top; // the topmost 2d point  where the laser is visible in the top 25 lines
	Point2D bottom; // the bottommost 2d point  where the laser is visible in the top 25 lines
	top.Set(-1,-1); // initialize to bad val
	bottom.Set(-1,-1);

	for (int y =0 ;y < 25 ; y++)
	{
		top25Xpos[y] = FindLaser(diffFrame,y);
		if(top.X == -1)
		{
			if(top25Xpos[y] != -1)
			{
				top.X = top25Xpos[y]; // got the min
				top.Y = y; //save the ypos
			}
		}
	}
	for (int y =0 ;y < 25 ; y++) // iterate through again
	{
		if(bottom.X == -1)
		{
			if(top25Xpos[(25-1) - y] != -1)
			{
				bottom.X = top25Xpos[(25-1) - y]; // got the min
				bottom.Y = (25-1) - y; //save the ypos
			}
		}
	}
	if((bottom.X != -1) && (top.X != -1))
	{
		//calculate the plane
		/* 
			create storage for 3 points
			two will be the positions on the reference back wall
			the 3rd is the position of the laser
			from this, we can determine the plane of the laser
			intersecting the object
			(Hope I got the vertex ordering correct)
		*/
		point_3d vertices[3]; 

		if(cfg->m_assumelaservertical)
		{
			//instead of using the bottom, use the top again offset
			Point2D tmp = top;
			tmp.Y += 10;
			if(!PlaneIntersect(&cfg->m_reference,tmp,&vertices[0]))
				return false;
		}
		else
		{
			if(!PlaneIntersect(&cfg->m_reference,bottom,&vertices[0]))
				return false;
		}
		if(!PlaneIntersect(&cfg->m_reference,top,&vertices[1]))
			return false;

		// we already know the world/camera position of the laser
		vertices[2] = cfg->m_laserpos;
		
		//now we can calculate the Plane
		pl->CalculatePlaneEquation(vertices,3);
		retval = true;
	}
	return retval;
}

void ScannerAlgSingle::CreateDefaultConfiguration()
{

}
bool ScannerAlgSingle::SaveConfiguration()
{
	FILE *fp = fopen("Single.cfg","wb");
	if(fp)
	{
		pConfig->Save(fp);
		fclose(fp);
		return true;
	}
	return false;

}
bool ScannerAlgSingle::LoadConfiguration()
{
	FILE *fp = fopen("Single.cfg","rb");
	if(fp)
	{
		pConfig->Load(fp);
		fclose(fp);
		return true;
	}
	return false;
}