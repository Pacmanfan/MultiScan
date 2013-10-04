#pragma once
#include "scanner3dlib.h"

// a set of tools used for image processing 
// frames of video to be sent into the scanner library
// Assume it's a singleton, so the scanner algorithms 
// can access it globally
class ImProc
{
private:

	CvCapture *m_Video;
	IplImage* m_CurFrame; // the current frame (color)
	IplImage* m_CurFrameGrey; // the current frame (grey)
	IplImage* m_PrevFrame; // the previous frame of video (color)
	IplImage* m_Reference; // the color reference image with no laser line
	IplImage* m_ReferenceGrey; // the grey reference image with no laser line
	IplImage* m_TemporalImage;// the greyscale diff image between 2 successive frames

	void ReleaseImage(IplImage **image);// called privately in this class
	// the init is called by the constructor
	void Init();
	// the convert to greyscale image is private 
	IplImage *ConvertToGrey(IplImage *color); 

	ImProc(void); // constructor is private
public:
	int offset;
	static ImProc *m_instance;
	/*
		This class is a singleton, so we need to initialize it 
		through the call to the instance
	*/
	static ImProc *ImProc::Instance()
	{
		if(m_instance == 0 )
		{
			m_instance = new ImProc();
		}
		return m_instance;
	}

	~ImProc(void);

	// call this to grab a new image, and set up images to be processed
	void UpdateFrame();
	// the reference image needs to be set for the algorithms to work
	void SetRefImage();
	//starting the video input
	bool StartVideo(int camera = -1);
	//stopping the video input
	void StopVideo();
	bool VideoConnected()
	{
		if (m_Video !=0 )
			return true;
		return false;
	}
	//retrieve the current frame
	
	IplImage *GetReference(){return m_Reference;}
	IplImage *GetReferenceGrey(){return m_ReferenceGrey;}

	IplImage *GetCurFrame(){return m_CurFrame;}
	IplImage *GetCurFrameGrey(){return m_CurFrameGrey;}
	IplImage *GetPrevFrame(){return m_PrevFrame;}
	IplImage *GetTemporalDiff(){return m_TemporalImage;}
};
