// dlgSingleConfig.cpp : implementation file
//

#include "stdafx.h"
#include "Scanner3d.h"
#include "dlgSingleConfig.h"

#include "scanner3dlib.h"
// dlgSingleConfig dialog

IMPLEMENT_DYNAMIC(dlgSingleConfig, CDialog)

dlgSingleConfig::dlgSingleConfig(CWnd* pParent /*=NULL*/)
	: CDialog(dlgSingleConfig::IDD, pParent)
	, m_wallDist(0)
	, m_xlaseroffset(0)
	, m_assumevertical(FALSE)
{

}

dlgSingleConfig::~dlgSingleConfig()
{
}

void dlgSingleConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, m_wallDist);
	DDX_Text(pDX, IDC_EDIT3, m_xlaseroffset);
	DDX_Check(pDX, IDC_CHECK2, m_assumevertical);
}


BEGIN_MESSAGE_MAP(dlgSingleConfig, CDialog)
	ON_BN_CLICKED(IDC_OK, &dlgSingleConfig::OnBnClickedOk)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// dlgSingleConfig message handlers

void dlgSingleConfig::OnBnClickedOk()
{
	GetFromScreen();
	this->OnOK();
}


void dlgSingleConfig::GetFromScreen()
{
	UpdateData();
	ScannerConfigSingle *pCfg = (ScannerConfigSingle *)pScanner->pConfig;

	pCfg->m_reference.d = m_wallDist;

	pCfg->m_laserpos.Wx = m_xlaseroffset;

	pCfg->m_assumelaservertical = (bool)m_assumevertical;

}
void dlgSingleConfig::SetToScreen()
{
	ScannerConfigSingle *pCfg = (ScannerConfigSingle *)pScanner->pConfig;

	m_wallDist = pCfg->m_reference.d;

	m_xlaseroffset = pCfg->m_laserpos.Wx;

	m_assumevertical = pCfg->m_assumelaservertical;

//UpdateData(FALSE);
}


int dlgSingleConfig::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	SetToScreen();
	return 0;
}
