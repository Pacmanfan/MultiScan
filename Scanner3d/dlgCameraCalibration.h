#pragma once


// dlgCameraCalibration dialog

class dlgCameraCalibration : public CDialog
{
	DECLARE_DYNAMIC(dlgCameraCalibration)

public:
	dlgCameraCalibration(CWnd* pParent = NULL);   // standard constructor
	virtual ~dlgCameraCalibration();

// Dialog Data
	enum { IDD = IDD_CAMERACALIBRATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	int m_numcalib;
	int m_chessX;
	int m_chessY;
	CString m_log;
	afx_msg void OnBnClickedStartcalib();
};
