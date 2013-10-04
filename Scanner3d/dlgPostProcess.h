#pragma once
#include "scanner3dlib.h"

// dlgPostProcess dialog

class dlgPostProcess : public CDialog
{
	DECLARE_DYNAMIC(dlgPostProcess)
	
public:
	List m_points; // the current set of points we're working with
	dlgPostProcess(CWnd* pParent = NULL);   // standard constructor
	virtual ~dlgPostProcess();

// Dialog Data
	enum { IDD = IDD_POSTPROCESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMerge();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedClear();
};
