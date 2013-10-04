//// The .h code...................................................
// DibFromIplImage.h
#pragma once

#include <windef.h>				// Definition of HBITMAP
#include <cv.h>
#include <highgui.h>

// Functor which creates a Device Indepentant Bitbap (DIB) from a IplImage
class CDibFromIplImage
{
public:
    HBITMAP operator()(const IplImage* Image) const;
};