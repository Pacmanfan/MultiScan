// dlgCameraCalibration.cpp : implementation file
//

#include "stdafx.h"
#include "Scanner3d.h"
#include "dlgCameraCalibration.h"
#include "scanner3dlib.h"
#include "dibfromIplImage.h"
#include "cameracalibration.h"
#include "improc.h"

// dlgCameraCalibration dialog

CRect Calibrect;
CDC *CalibpDC;
HDC CalibhDC;
CameraCalibration calib;

extern void DrawDIBSection( HDC hDC, HBITMAP hBitmap, int xDest, int yDest );
bool inited = false;
bool calibrating = false;
int chessX,chessY;
int numsuccesses;

IMPLEMENT_DYNAMIC(dlgCameraCalibration, CDialog)

dlgCameraCalibration::dlgCameraCalibration(CWnd* pParent /*=NULL*/)
	: CDialog(dlgCameraCalibration::IDD, pParent)
	, m_numcalib(20)
	, m_chessX(7)
	, m_chessY(5)
	, m_log(_T(""))
{

}

dlgCameraCalibration::~dlgCameraCalibration()
{
}

void dlgCameraCalibration::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_numcalib);
	DDV_MinMaxInt(pDX, m_numcalib, 1, 100);
	DDX_Text(pDX, IDC_EDIT2, m_chessX);
	DDV_MinMaxInt(pDX, m_chessX, 2, 20);
	DDX_Text(pDX, IDC_EDIT3, m_chessY);
	DDV_MinMaxInt(pDX, m_chessY, 2, 20);
	DDX_Text(pDX, IDC_EDIT4, m_log);
}


BEGIN_MESSAGE_MAP(dlgCameraCalibration, CDialog)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_STARTCALIB, &dlgCameraCalibration::OnBnClickedStartcalib)
END_MESSAGE_MAP()


// dlgCameraCalibration message handlers

void dlgCameraCalibration::OnTimer(UINT_PTR nIDEvent)
{
	if(!inited)
	{
		inited = true;
		CWnd *pwnd = GetDlgItem(IDC_STOPSCANNING2); // get the handle to the image
		CalibpDC =pwnd->GetDC();
		CalibhDC= CalibpDC->GetSafeHdc();	
		GetClientRect(&Calibrect);
	}
	// TODO: Add your message handler code here and/or call default
	if(ImProc::Instance()->VideoConnected())
	{
		//m_CalibFrame=cvQueryFrame(m_Video); // get a frame of video
		ImProc::Instance()->UpdateFrame(); // get new frames
		if(calibrating)
		{
			calib.FeedImage(ImProc::Instance()->GetCurFrameGrey());
		}

		CDibFromIplImage DibFromIplFunctor;
		HBITMAP hBmp = 0;
		
		hBmp = DibFromIplFunctor(ImProc::Instance()->GetCurFrame()); // show in color
		DrawDIBSection(CalibhDC,hBmp,0,0);			
		if(hBmp!=0)
			DeleteObject(hBmp); // release the windows HBITMAP now it's drawn

	}
	
	CDialog::OnTimer(nIDEvent);
}

int dlgCameraCalibration::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;



	SetTimer(1,100,NULL);
	// TODO:  Add your specialized creation code here

	return 0;
}

void dlgCameraCalibration::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
}

void dlgCameraCalibration::OnBnClickedStartcalib()
{
	UpdateData();
	calibrating = true;
	calib.StartCalibration(m_chessX,m_chessY,m_numcalib);
}
