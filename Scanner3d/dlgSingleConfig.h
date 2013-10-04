#pragma once


// dlgSingleConfig dialog

class dlgSingleConfig : public CDialog
{
	DECLARE_DYNAMIC(dlgSingleConfig)

public:
	dlgSingleConfig(CWnd* pParent = NULL);   // standard constructor
	virtual ~dlgSingleConfig();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void GetFromScreen();
	void SetToScreen();
	float m_wallDist;
	float m_xlaseroffset;
	BOOL m_assumevertical;
	afx_msg void OnBnClickedOk();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
