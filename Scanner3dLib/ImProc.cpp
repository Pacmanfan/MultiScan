#include "ImProc.h"
#include "math.h"

//the compiler wasn't seeing these for some reason
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


//initialize the singleton
ImProc *ImProc::m_instance=0;

ImProc::ImProc(void)
{
	Init();
}

ImProc::~ImProc(void)
{
}

bool ImProc::StartVideo(int camera)
{
	if(m_Video == 0)
	{
		m_Video=cvCaptureFromCAM(camera); // try to connect to any camera
		if(m_Video != 0)
		{
			cvSetCaptureProperty( m_Video, CV_CAP_PROP_FRAME_WIDTH, 640 );
			cvSetCaptureProperty( m_Video, CV_CAP_PROP_FRAME_HEIGHT, 480 );
			return true;
		}
	}
	return false;
}
void ImProc::StopVideo()
{
	if(m_Video != 0)
	{
		cvReleaseCapture(&m_Video);
		m_Video = 0;
	}
}

void ImProc::ReleaseImage(IplImage **image)
{
	
	// release the specified image 
	// and set the pointer to 0
	if(*image)
	{
		cvReleaseImage(image);
		*image =0;
	}
}
void ImProc::Init()
{
	offset = 10;
	m_Video = 0;
	m_CurFrame = 0; // the current frame (color)
	m_CurFrameGrey = 0;
	m_PrevFrame = 0; // the previous frame of video (color)
	m_Reference = 0; // the color reference image with no laser line
	m_ReferenceGrey = 0; // the grey reference image with no laser line
	m_TemporalImage = 0;// the greyscale diff image between 2 successive frames that has been thresholded
}



void ImProc::SetRefImage()
{
	//release the previous images (if any)
	ReleaseImage(&m_ReferenceGrey);
	ReleaseImage(&m_Reference);

	IplImage *Frame=cvQueryFrame(m_Video); // get a frame of video
	//now copy a permanant copy of it into the m_Reference frame
	if(Frame !=0)
	{
		m_Reference = cvCloneImage(Frame);
		//and make a greyscale copy of it
		m_ReferenceGrey = ConvertToGrey(m_Reference);
	}
}

void ImProc::UpdateFrame()
{
	if(!m_Video)
		return;
	//release the previous frame
	ReleaseImage(&m_PrevFrame);

	//make the previous Frame this frame
	if(m_CurFrame != 0) //safety
		m_PrevFrame = cvCloneImage(m_CurFrame);

	IplImage *Frame=cvQueryFrame(m_Video); // get a frame of video
	//release the current frame of video before getting another
	ReleaseImage(&m_CurFrame);
	//now copy a permanant copy of it into the current frame
	m_CurFrame = cvCloneImage(Frame);

	if(m_PrevFrame == 0)
		return; // we need to bail until we have 2 frames

	//now convert this frame and the previous frame to grey
	ReleaseImage(&m_CurFrameGrey);
	m_CurFrameGrey  = ConvertToGrey(m_CurFrame);
	IplImage * curgrey = m_CurFrameGrey;

	if(m_ReferenceGrey == 0) // no reference image set yet
		return;

	IplImage * prevgrey = ConvertToGrey(m_PrevFrame);

	//free and previous temporal image difference threshold picture
	ReleaseImage(&m_TemporalImage);
	//and create a new one
	m_TemporalImage = cvCreateImage(cvSize(curgrey->width,curgrey->height),curgrey->depth,1);


	//Zhang et al. [ZCS03]
	unsigned char *curdat,*prvdat,*tmpdat,*refdat;
	float mindat,maxdat,avdat,diff;
	curdat = (unsigned char *)curgrey->imageData; // the current frame
	prvdat = (unsigned char *)prevgrey->imageData; // the previous frame
	refdat = (unsigned char *)m_ReferenceGrey->imageData; // get a pointer to the greyscale reference image
	tmpdat = (unsigned char *)m_TemporalImage->imageData; // the temporal shadow difference image
	//build a map of the min / max values 
	for(int y =0 ; y < m_CurFrame->height; y ++)
	{
		for(int x = 0; x < m_CurFrame->width; x++)
		{		
			mindat = min(curdat[y * curgrey->widthStep + x],prvdat[y * prevgrey->widthStep + x]);
			maxdat = max(curdat[y * curgrey->widthStep + x],prvdat[y * prevgrey->widthStep + x]);
			avdat = (mindat + maxdat) / 2.0f; // find the dynamic midpoint threshold
			//diff = (float)refdat[y * m_ReferenceGrey->widthStep + x]; // get the pixel from the greyscale reference image
			diff = (float)curdat[y * curgrey->widthStep + x]; // get the pixel from the greyscale reference image
			diff = diff  - avdat; // subtract the midpoint of the dynamic range from the difference in brightness between 2 successive frames of video
			//now, if diff is less than 0, (i.e. 0 crossing), then we have an edge
			// for the purposes of my algorithm ,we're going to make this stand out
			// by saying it's now 255 - (unsigned char)diff to make it really stand out
			// we may need to add in some translation or scaler to tweak this
			tmpdat[y * m_TemporalImage->widthStep + x] = (unsigned char)diff + offset;
		}
	}

	//remember to clean up	
	ReleaseImage(&prevgrey);
}

IplImage *ImProc::ConvertToGrey(IplImage *color)
{
	IplImage *grey;
	grey = cvCreateImage(cvSize(color->width,color->height),color->depth,1);
	cvCvtColor(color, grey , CV_BGR2GRAY);
	return grey;
}