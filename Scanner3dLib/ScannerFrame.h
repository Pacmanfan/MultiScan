#pragma once
#include "listitem.h"
class ScannerFrame
{
public:
	List *m_pPoints;
	float m_zrot; // zrotation
	ScannerFrame(void);
	~ScannerFrame(void);
};
