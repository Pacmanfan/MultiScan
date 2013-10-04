#pragma once


// dlgCornerConfig dialog

class dlgCornerConfig : public CDialog
{
	DECLARE_DYNAMIC(dlgCornerConfig)

public:
	dlgCornerConfig(CWnd* pParent = NULL);   // standard constructor
	virtual ~dlgCornerConfig();

// Dialog Data
	enum { IDD = IDD_CORNERCONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
