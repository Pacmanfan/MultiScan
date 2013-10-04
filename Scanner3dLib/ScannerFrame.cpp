
#include "ScannerFrame.h"
#include "point3d.hpp"

ScannerFrame::ScannerFrame(void)
{
	m_pPoints = new List();
	m_zrot = 0; // assume no rotation for now.
}

ScannerFrame::~ScannerFrame(void)
{
	// iterate through and delete all point
	for(ListItem *li = m_pPoints->list; li !=0; li=li->next)
	{
		point_3d *p = (point_3d *)li->data;
		delete p;
	}
	delete m_pPoints;
}
