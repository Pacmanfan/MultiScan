// Scanner3d.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CScanner3dApp:
// See Scanner3d.cpp for the implementation of this class
//

class CScanner3dApp : public CWinApp
{
public:
	CScanner3dApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CScanner3dApp theApp;