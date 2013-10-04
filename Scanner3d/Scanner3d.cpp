// Scanner3d.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Scanner3d.h"
#include "Scanner3dDlg.h"
#include "gl_window.h"
#include <cv.h>
#include <highgui.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CScanner3dApp

BEGIN_MESSAGE_MAP(CScanner3dApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CScanner3dApp construction

CScanner3dApp::CScanner3dApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CScanner3dApp object

CScanner3dApp theApp;

// CScanner3dApp initialization
//MFC_Window *wnd=0;
GL_Window *glwnd=0;

BOOL CScanner3dApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

/*
	wnd = new MFC_Window();
    m_pMainWnd = wnd;
    m_pMainWnd->ShowWindow(1);

	glwnd = new GL_Window();
	//glwnd->ShowWindow(1);
*/
	CScanner3dDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
