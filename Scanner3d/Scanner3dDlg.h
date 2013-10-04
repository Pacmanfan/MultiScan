// Scanner3dDlg.h : header file
//

#pragma once


#include "afxwin.h"
#include "dlgPostProcess.h"
#include "afxcmn.h"

class MFC_Window :public CFrameWnd
{
    public:
    MFC_Window()
    {
        Create(NULL,0);
    }
};

/*
BEGIN_MESSAGE_MAP( MFC_Window, CFrameWnd)

END_MESSAGE_MAP()
*/

// CScanner3dDlg dialog
class CScanner3dDlg : public CDialog
{
// Construction
public:
	CScanner3dDlg(CWnd* pParent = NULL);	// standard constructor
	void DrawLine(int xs, int ys, int xe, int ye);
	void DoLaserLine(IplImage * image);
	void DoLaserLinePlane(IplImage * image); // show the calculate laser plane
// Dialog Data
	enum { IDD = IDD_SCANNER3D_DIALOG };
	
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//CFrameGrabber m_FrameGrabber;
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedCaliberate();
	afx_msg void OnBnClickedStartscanning();
	//int m_brightnessthreshold;
	float m_walldist;

	float m_laserXoffset;
	float m_camviewdist;

	afx_msg void OnBnClickedStopscanning2();
	afx_msg void OnBnClickedSavedata();
	CString m_log;
	afx_msg void OnBnClickedConnect();
	void AddMessage(CString message);
	afx_msg void OnCbnSelchangeDisplay();
	CComboBox m_displaytype;
	int displaytype;
	afx_msg void OnBnClickedCmdpostprocess();
//	double m_cannythreshlow;
//	double m_cannythreshhigh;
//	int m_cannyaperature;
//	BOOL m_usecanny;
	afx_msg void OnDestroy();
	BOOL m_assumevertical;
	afx_msg void OnBnClickedCameracalib();
	CComboBox m_cmbAlg;
	afx_msg void OnCbnSelchangeCmbalg();
	void SetToScreen();
	void GetFromScreen();
	afx_msg void OnBnClickedAlgorithmoptions();
	CButton m_showlaser;
	CButton m_cmdConnect;
	CButton m_startstopscan;
	int m_sldBrightness;
	afx_msg void OnTRBNThumbPosChangingBrightness(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawBrightness(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CSliderCtrl m_sldbright;
	int m_brightoffset;
	CSliderCtrl m_sldbroffset;
};
