//// The .cpp code..................................................

#include "stdafx.h"
#include "DibFromIplImage.h"
#include "cxcore.h"
#include "cv.h"
#include "highgui.h"				// For utility function: convert one image to another with optional vertical flip.

// Functor which creates a Device Indepentant Bitbap (DIB) from an IplImage
HBITMAP CDibFromIplImage::operator()(const IplImage* Image) const
{
	assert(Image);

	IplImage * imgTemp = 0;

	// Creates IPL image (header and data)
	imgTemp = cvCreateImage(cvGetSize(Image), 8, 3 );

	// If greyscale convert to colour else just make a copy.
	if(1 == Image->nChannels)
	{	
		cvCvtColor(Image, imgTemp, CV_GRAY2RGB);
	}
	else
	{
		cvCopyImage(Image, imgTemp);
	}

	int bpp = imgTemp->nChannels * 8;
	assert(imgTemp->width >= 0 && imgTemp->height >= 0 && (bpp == 8 || bpp == 24 || bpp == 32));
	
	CvMat dst;						
		// Matrix
	void* dst_ptr = 0;
	

	// Create on the stack
	unsigned char buffer[sizeof(BITMAPINFO) + 255*sizeof(RGBQUAD)];

	// The BITMAPINFO structure defines the dimensions and color information for a DIB. 
	BITMAPINFO* bmi = (BITMAPINFO*)buffer;

	ZeroMemory(&(bmi->bmiHeader), sizeof(BITMAPINFOHEADER));
	bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi->bmiHeader.biWidth = imgTemp->width;
	bmi->bmiHeader.biHeight = imgTemp->origin ? abs(imgTemp->height) : -abs(imgTemp->height);
	bmi->bmiHeader.biPlanes = 1;
	bmi->bmiHeader.biBitCount = bpp;
	bmi->bmiHeader.biCompression = BI_RGB;

	if (bpp == 8) 
	{
		RGBQUAD* palette = bmi->bmiColors;
		int i;
		for (i = 0; i < 256; i++) 
		{
			palette[i].rgbRed = palette[i].rgbGreen = palette[i].rgbBlue = (BYTE)i;
			palette[i].rgbReserved = 0;
		}
	}

	HBITMAP hBmp = CreateDIBSection(NULL, bmi, DIB_RGB_COLORS, &dst_ptr, 0, 0);
	if(NULL == hBmp)
	{
		DWORD dwError = ::GetLastError();
//			DisplaySystemErrorMsg(_T("CreateDIBSection"));
		return FALSE;
	}
	cvInitMatHeader(&dst, imgTemp->height, imgTemp->width, CV_8UC3, dst_ptr, (imgTemp->width * imgTemp->nChannels + 3) & -4);
	cvConvertImage(imgTemp, &dst, imgTemp->origin ? CV_CVTIMG_FLIP : 0);
	cvReleaseImage(&imgTemp);
	return hBmp;
}