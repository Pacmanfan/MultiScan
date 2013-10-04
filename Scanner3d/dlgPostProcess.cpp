// dlgPostProcess.cpp : implementation file
//

#include "stdafx.h"
#include "Scanner3d.h"
#include "dlgPostProcess.h"
#include "scanner3dlib.h"

// dlgPostProcess dialog

IMPLEMENT_DYNAMIC(dlgPostProcess, CDialog)

dlgPostProcess::dlgPostProcess(CWnd* pParent /*=NULL*/)
	: CDialog(dlgPostProcess::IDD, pParent)
{

}

dlgPostProcess::~dlgPostProcess()
{
}

void dlgPostProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(dlgPostProcess, CDialog)
	ON_BN_CLICKED(IDC_MERGE, &dlgPostProcess::OnBnClickedMerge)
	ON_BN_CLICKED(IDC_SAVE, &dlgPostProcess::OnBnClickedSave)
	ON_BN_CLICKED(IDC_CLEAR, &dlgPostProcess::OnBnClickedClear)
END_MESSAGE_MAP()


// dlgPostProcess message handlers

void dlgPostProcess::OnBnClickedMerge()
{
	PostProcessor pp;
	pp.Merge(&m_points);
}

void dlgPostProcess::OnBnClickedSave()
{
	this->UpdateData();

	char strFilter[] = { "PLY Files (*.ply)|*.ply|All Files (*.*)|*.*||" };

	CFileDialog FileDlg(FALSE, NULL, NULL, 0, (LPCTSTR)strFilter);

	if( FileDlg.DoModal() == IDOK )
	{
		PostProcessor pp;		
		pp.SaveData((char *)(const char *)FileDlg.GetFileName(),&m_points);
	}
	else
		return;

}

void dlgPostProcess::OnBnClickedClear()
{
	m_points.Destroy();// this will leak memory
}
