// Scanner3dDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Scanner3d.h"
#include "Scanner3dDlg.h"
#include "gl_window.h"
#include <cv.h>
#include <highgui.h>
#include "dibfromIplImage.h"
#include <scanner3dlib.h>
#include "dlgCameraCalibration.h"
#include "dlgCornerConfig.h"
#include "dlgSingleConfig.h"
#include "improc.h"
#include "leastsquares.h"
#include <cvStructuredLight.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//extern GL_Window *glwnd;
//extern MFC_Window *wnd;

CRect rect;
CDC *pDC;
HDC hDC;
HPEN greenPen;

ScannerAlgCorner *pCorner = 0;
ScannerAlgSingle *pSingle = 0;
ScannerAlg *pScanner;

void DrawDIBSection( HDC hDC, HBITMAP hBitmap, int xDest, int yDest ); // forward ref

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CScanner3dDlg dialog




CScanner3dDlg::CScanner3dDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScanner3dDlg::IDD, pParent)
	//, m_brightnessthreshold(0)
	, m_walldist(0)
	, m_laserXoffset(0)
	, m_camviewdist(0)
	, m_log(_T(""))
	, m_sldBrightness(255)
	, m_brightoffset(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CScanner3dDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//	DDX_Text(pDX, IDC_EDIT1, m_brightnessthreshold);
	//	DDV_MinMaxInt(pDX, m_brightnessthreshold, 0, 255);
	DDX_Text(pDX, IDC_EDIT4, m_camviewdist);
	DDX_Text(pDX, IDC_LOG, m_log);
	DDX_Control(pDX, IDC_DISPLAY, m_displaytype);
	DDX_Control(pDX, IDC_CMBALG, m_cmbAlg);
	DDX_Control(pDX, IDC_SHOWLASER, m_showlaser);
	DDX_Control(pDX, IDC_CONNECT, m_cmdConnect);
	DDX_Control(pDX, IDC_STARTSCANNING, m_startstopscan);
	DDX_Slider(pDX, IDC_BRIGHTNESS, m_sldBrightness);
	DDV_MinMaxInt(pDX, m_sldBrightness, 0, 255);
	DDX_Control(pDX, IDC_BRIGHTNESS, m_sldbright);
	DDX_Slider(pDX, IDC_BRIGHTOFFSET, m_brightoffset);
	DDV_MinMaxInt(pDX, m_brightoffset, -20, 20);
	DDX_Control(pDX, IDC_BRIGHTOFFSET, m_sldbroffset);
}

BEGIN_MESSAGE_MAP(CScanner3dDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CALIBERATE, &CScanner3dDlg::OnBnClickedCaliberate)
	ON_BN_CLICKED(IDC_STARTSCANNING, &CScanner3dDlg::OnBnClickedStartscanning)
	ON_BN_CLICKED(IDC_SAVEDATA, &CScanner3dDlg::OnBnClickedSavedata)
	ON_BN_CLICKED(IDC_CONNECT, &CScanner3dDlg::OnBnClickedConnect)
	ON_CBN_SELCHANGE(IDC_DISPLAY, &CScanner3dDlg::OnCbnSelchangeDisplay)
	ON_BN_CLICKED(IDC_CMDPOSTPROCESS, &CScanner3dDlg::OnBnClickedCmdpostprocess)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CAMERACALIB, &CScanner3dDlg::OnBnClickedCameracalib)
	ON_CBN_SELCHANGE(IDC_CMBALG, &CScanner3dDlg::OnCbnSelchangeCmbalg)
	ON_BN_CLICKED(IDC_ALGORITHMOPTIONS, &CScanner3dDlg::OnBnClickedAlgorithmoptions)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CScanner3dDlg message handlers

BOOL CScanner3dDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
//	glwnd->EnableOpenGL();

	if(LoadConfig() == 0)
	{
		SaveConfig();// create a default config and save it		
	}
	//create the scanner library interface

	pCorner = new ScannerAlgCorner();
	pSingle = new ScannerAlgSingle();
	pScanner = pSingle;
	SetToScreen(); // update the screen variables

	CWnd *pwnd = GetDlgItem(IDC_STATIC); // get the handle to the image
	pDC =pwnd->GetDC();
    hDC= pDC->GetSafeHdc();	
	greenPen=CreatePen(PS_SOLID, 1, RGB(0,255,0));
	GetClientRect(&rect);

	m_sldbright.SetRange(0,255);
	m_sldbright.SetPos(128);

	m_sldbroffset.SetRange(-20,20);
	m_sldbroffset.SetPos(10);

/*
	set up combo box
*/
	m_displaytype.AddString("Color");
	m_displaytype.AddString("Temporal Difference");
	m_displaytype.SetCurSel(0);
	displaytype = 0; // color

	m_cmbAlg.AddString("Vertical Line Single Backplane");
	m_cmbAlg.AddString("Horizontal Line Corner");
	m_cmbAlg.SetCurSel(0);

	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CScanner3dDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CScanner3dDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CScanner3dDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CScanner3dDlg::OnBnClickedCaliberate()
{
	UpdateData(TRUE); // save the parameters, because the log function will overwrite them
	if(ImProc::Instance()->VideoConnected())
	{
		// this is the frame with no laser
		ImProc::Instance()->SetRefImage();
		AddMessage("Reference Image Set");
	}else
	{
		AddMessage("Video Device not connected");
	}

}

void CScanner3dDlg::OnBnClickedStartscanning()
{
	if(pScanner->IsScanning())
	{
		AddMessage("Stopping Scan");
		pScanner->EndScan();
		this->m_startstopscan.SetWindowTextA("Start Scanning");
	}else{
	///m_startstopscan
		if(ImProc::Instance()->VideoConnected())
		{
			GetFromScreen();		
			AddMessage("Starting Scan");
			
			pScanner->StartScan();
			this->m_startstopscan.SetWindowTextA("Stop Scanning");
		}
		else
		{
			AddMessage("Video Not Connected - cannot start scan");
		}
	}
	
}

void CScanner3dDlg::OnTimer(UINT nIDEvent) 
{
	//CDialog::OnTimer(nIDEvent);	
	

	IplImage* m_Frame = 0;
	if(ImProc::Instance()->VideoConnected())
	{
		ImProc::Instance()->UpdateFrame(); // update all the frames of video

		CDibFromIplImage DibFromIplFunctor;
		HBITMAP hBmp = 0;

		switch(displaytype)
		{
		case 0:	// color			
			hBmp = DibFromIplFunctor(ImProc::Instance()->GetCurFrame());
			DrawDIBSection(hDC,hBmp,0,0);			
		break;
		case 1: //difference 
			if(ImProc::Instance()->GetTemporalDiff() !=0)
			{
				//display it
				hBmp = DibFromIplFunctor(ImProc::Instance()->GetTemporalDiff());
				DrawDIBSection(hDC,hBmp,0,0);	
			}
			break;

		}
		if(hBmp!=0)
			DeleteObject(hBmp); // release the windows HBITMAP now it's drawn

		if(pScanner->IsScanning())
		{
			// the scanner converts it to greyscale and manages the resources internally
			pScanner->ProcessFrame(0.0f); // assume 0 rotation for now.
		}

		switch(pScanner->pConfig->m_scantype)
		{
			case eLeftRightCorner:
				DrawLine(100,0,100,ImProc::Instance()->GetCurFrame()->height);
				DrawLine(ImProc::Instance()->GetCurFrame()->width - 100,0,ImProc::Instance()->GetCurFrame()->width - 100,ImProc::Instance()->GetCurFrame()->height);
				if(m_showlaser.GetCheck() == 1)
				{
					if(ImProc::Instance()->GetTemporalDiff()!=0)
					{
						DoLaserLine(ImProc::Instance()->GetTemporalDiff());
						DoLaserLinePlane(ImProc::Instance()->GetTemporalDiff());
					}
				}
				break;
			case eTop25:
				break;
		}


					
		CDialog::OnTimer(nIDEvent);
	}
}

void CScanner3dDlg::DoLaserLinePlane(IplImage * image)
{
	List pnts;
	float m,b;
	int width = 640; // this needs to be real image width
	Point2D tmppnts[50];
	//Point2D tmppnts[25];
	for(int xpos = 0; xpos <50 ; xpos ++)
	{
		int ypos = pScanner->FindLaser(image,xpos);
		if(ypos != -1)
		{
			//Point2D *pnt = new Point2D();
			tmppnts[xpos].Set(xpos,ypos);
			//pnt->Set(xpos,ypos);
			pnts.Add(&tmppnts[xpos]); // add only valid points
		}
	}
	FindLeastSquare(&pnts,&m,&b); // find the slope
	Point2D p1,p2;
	p1.X = 0;
	p1.Y = (long)((m * p1.X) + b);

	p2.X = 100;
	p2.Y = (long)((m * p2.X) + b);

	DrawLine(p1.X,p1.Y,p2.X,p2.Y);
	//release the points

	pnts.Destroy();
	for(int xpos = 0; xpos < 50 ; xpos ++)
	{
		int ypos = pScanner->FindLaser(image,xpos + (width-50));
		if(ypos != -1)
		{
			//Point2D *pnt = new Point2D();
			tmppnts[ xpos ].Set(xpos + (width-50),ypos);
			//pnt->Set(xpos,ypos);
			pnts.Add(&tmppnts[xpos]); // add only valid points
		}
	}
	FindLeastSquare(&pnts,&m,&b); // find the slope
	//Point2D p1,p2;
	p1.X = width - 100;
	p1.Y = (long)((m * p1.X) + b);

	p2.X = width;
	p2.Y = (long)((m * p2.X) + b);

	DrawLine(p1.X,p1.Y,p2.X,p2.Y);

}


void CScanner3dDlg::DoLaserLine(IplImage * image)
{
	for(int xpos = 0; xpos <639 ; xpos ++)
	{
		int ypos = pScanner->FindLaser(image,xpos);
		if(ypos != -1)
		{
			DrawLine(xpos,ypos,xpos+1,ypos);
		}
	}
}

void CScanner3dDlg::DrawLine(int xs, int ys, int xe, int ye)
{
	HGDIOBJ oldPen=SelectObject(hDC,greenPen);
	POINT pntArray[2];
	pntArray[0].x=xs;
	pntArray[0].y=ys;
	pntArray[1].x=xe;
	pntArray[1].y=ye;

	Polyline(hDC, pntArray, 2);
	SelectObject(hDC,oldPen);
}

// DrawDIBSection	- Draws a DIB section onto a device
// hDC		- Handle to a device context
// hBitmap	- Handle of the DIB Section
// xDest	- x-coordinate of the upper-left corner of the destination rect
// yDest	- y-coordinate of the upper-left corner of the destination rect
void DrawDIBSection( HDC hDC, HBITMAP hBitmap, int xDest, int yDest )
{
	HPALETTE hPal;

	HDC hDCMem = ::CreateCompatibleDC( hDC );

	// Create a logical palette for the bitmap
	DIBSECTION ds;
	BITMAPINFOHEADER &bmInfo = ds.dsBmih;
	if( ::GetObject(hBitmap, sizeof(ds), &ds ) == 0 )
		return;			// Not a DIB Section
	
	HGDIOBJ hBmpOld =  ::SelectObject(hDCMem, hBitmap);

	int nColors = bmInfo.biClrUsed ? bmInfo.biClrUsed : 1 << ds.dsBm.bmBitsPixel;
	
	if( ::GetDeviceCaps(hDC, RASTERCAPS) & RC_PALETTE )
	{
		// Create a halftone palette if colors > 256. 
		if( nColors > 256 )
			hPal = ::CreateHalftonePalette(hDC);
		else
		{
			// Create the palette
			RGBQUAD *pRGB = new RGBQUAD[nColors];
			
			::GetDIBColorTable( hDCMem, 0, nColors, pRGB );
			
			UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
			LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
			
			pLP->palVersion = 0x300;
			pLP->palNumEntries = nColors;
			
			for( int i=0; i < nColors; i++)
			{
				pLP->palPalEntry[i].peRed = pRGB[i].rgbRed;
				pLP->palPalEntry[i].peGreen = pRGB[i].rgbGreen;
				pLP->palPalEntry[i].peBlue = pRGB[i].rgbBlue;
				pLP->palPalEntry[i].peFlags = 0;
			}
			
			hPal = ::CreatePalette( pLP );
			
			delete[] pLP;
			delete[] pRGB;
		}

		HPALETTE hPalOld = ::SelectPalette(hDC,hPal,FALSE);
		::RealizePalette(hDC);
		BitBlt(hDC,xDest,yDest,bmInfo.biWidth,bmInfo.biHeight,hDCMem,0,0,SRCCOPY);

		::SelectPalette(hDC,hPalOld,FALSE);
		// delete GDI objects
		::DeleteObject(hPal);
	}
	else
		BitBlt(hDC,xDest,yDest,bmInfo.biWidth,bmInfo.biHeight,hDCMem,0,0,SRCCOPY);

	::SelectObject(hDCMem, hBmpOld);
	::DeleteDC(hDCMem);
}
/*
void CScanner3dDlg::OnBnClickedStopscanning2()
{
	m_scanning  = false;
	AddMessage("Stopping Scan");
	pScanner->EndScan();
}
*/
void CScanner3dDlg::OnBnClickedSavedata()
{
		// TODO: Add your control notification handler code here
	this->UpdateData();

	char strFilter[] = { "PLY Files (*.ply)|*.ply|All Files (*.*)|*.*||" };

	CFileDialog FileDlg(FALSE, NULL, NULL, 0, (LPCTSTR)strFilter);

	if( FileDlg.DoModal() == IDOK )
	{
		//pScanner->SaveData((char *)(const char *)FileDlg.GetFileName());
		PostProcessor pp;		
		List lst;
		pp.Composite(&lst); // simple raw export
		pp.SaveData((char *)(const char *)FileDlg.GetFileName(),&lst);

	}
	else
		return;

}

void CScanner3dDlg::OnBnClickedConnect()
{
	if(ImProc::Instance()->VideoConnected() == true)
	{
		m_cmdConnect.SetWindowTextA("Connect Camera");
		ImProc::Instance()->StopVideo();
		AddMessage("Disconnected from Video Device");
		KillTimer(1);
	}else{
		if(ImProc::Instance()->StartVideo(0))
		{
			m_cmdConnect.SetWindowTextA("Disconnect Camera");
			AddMessage("Connected to Video Device");
			SetTimer(1,100,NULL);
		}else
		{
			AddMessage("Cannot Connect to Video Device");
		}
	}
}


void CScanner3dDlg::AddMessage(CString message)
{
	m_log = message + "\r\n" + m_log;
	UpdateData(FALSE);
}

void CScanner3dDlg::OnCbnSelchangeDisplay()
{
	displaytype = m_displaytype.GetCurSel();
}

void CScanner3dDlg::OnBnClickedCmdpostprocess()
{
	dlgPostProcess dlgpp;
	dlgpp.DoModal();
}

void CScanner3dDlg::OnDestroy()
{

	CDialog::OnDestroy();
}

void CScanner3dDlg::OnBnClickedCameracalib()
{
	dlgCameraCalibration dlg;
	dlg.DoModal();
}

void CScanner3dDlg::OnCbnSelchangeCmbalg()
{
	// choose the algorithm here and set it in the config
	int idx = m_cmbAlg.GetCurSel();
	switch(idx)
	{
		case 0:
			pScanner=pSingle;
			break;
		case 1:
			pScanner=pCorner;
			break;
	}
	//need a display from config func.
	UpdateData(FALSE); // tell it to display now.
}

// this function updates the screen with data from the 
// pScannerAlg;

void CScanner3dDlg::SetToScreen()
{
	if(!pScanner->LoadConfiguration())
	{
		pScanner->CreateDefaultConfiguration();
		pScanner->SaveConfiguration();
	}
	m_sldBrightness = pScanner->pConfig->m_brightnessthreshold;
	m_camviewdist = pScanner->pConfig->m_camera.viewing_distance;
	UpdateData(FALSE);
}

void CScanner3dDlg::GetFromScreen()
{
	UpdateData(TRUE);
	
	pScanner->pConfig->m_brightnessthreshold = m_sldBrightness;
	pScanner->pConfig->m_camera.viewing_distance = m_camviewdist;

	AddMessage("Saving parameters");
	pScanner->SaveConfiguration();

}

void CScanner3dDlg::OnBnClickedAlgorithmoptions()
{
	CDialog *pDlg = 0;
	switch(pScanner->pConfig->m_scantype)
	{
		case eLeftRightCorner:
			pDlg = new dlgCornerConfig();
			break;
		case eTop25:
			pDlg = new dlgSingleConfig();
			break;
	}
	if(pDlg != 0)
	{
		pDlg->DoModal();
		delete pDlg;
	}
}

/*
void CScanner3dDlg::OnTRBNThumbPosChangingBrightness(NMHDR *pNMHDR, LRESULT *pResult)
{
	// This feature requires Windows Vista or greater.
	// The symbol _WIN32_WINNT must be >= 0x0600.
	NMTRBTHUMBPOSCHANGING *pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	UpdateData();
	pScanner->pConfig->m_brightnessthreshold = this->m_sldBrightness;
}
*/


void CScanner3dDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	UpdateData();
	pScanner->pConfig->m_brightnessthreshold = this->m_sldBrightness;
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	
	ImProc::Instance()->offset = m_brightoffset;
}
