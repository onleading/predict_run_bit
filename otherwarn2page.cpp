// OtherWarnPage.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "OtherWarn2Page.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COtherWarn2Page property page

IMPLEMENT_DYNCREATE(COtherWarn2Page, CPropertyPage)

COtherWarn2Page::COtherWarn2Page() : CPropertyPage(COtherWarn2Page::IDD)
{
	//{{AFX_DATA_INIT(COtherWarn2Page)
	m_bPopup = FALSE;
	m_nBeepFlag = -1;
	m_strWavFile = _T("");
	//}}AFX_DATA_INIT

	m_Type = 0;
}

COtherWarn2Page::~COtherWarn2Page()
{
}

void COtherWarn2Page::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COtherWarn2Page)
	DDX_Check(pDX, IDC_POPUPCHECK, m_bPopup);
	DDX_Radio(pDX, IDC_RADIO1, m_nBeepFlag);
	DDX_Text(pDX, IDC_WAVFILE_EDIT, m_strWavFile);
	DDV_MaxChars(pDX, m_strWavFile, 78);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COtherWarn2Page, CPropertyPage)
	//{{AFX_MSG_MAP(COtherWarn2Page)
	ON_BN_CLICKED(IDC_BROWSE,OnBrowse)
	ON_BN_CLICKED(IDC_RADIO1, OnBeepRadio)
	ON_BN_CLICKED(IDC_RADIO2, OnBeepRadio)
	ON_BN_CLICKED(IDC_RADIO3, OnBeepRadio)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COtherWarn2Page message handlers

BOOL COtherWarn2Page::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if(m_Type==1)
	{
		CString strIni = g_WSXHStr+"srvwarn.dat";
		m_bPopup = GetPrivateProfileInt("WARNOPT", "Popup", 1, strIni);
		m_nBeepFlag = GetPrivateProfileInt("WARNOPT", "BeepFlag", 0, strIni);
		char strWave[MAX_PATH]={0};
		GetPrivateProfileString("WARNOPT", "WavFile", "", strWave, MAX_PATH, strIni);
		m_strWavFile = strWave;
	}
	else
	{
		m_bPopup = g_WarnParam2.bTJPopup ;
		m_nBeepFlag = g_WarnParam2.nTJBeepFlag;
		m_strWavFile = g_WarnParam2.TJWavFile;
	}
	
	UpdateData(FALSE);
	return TRUE;
}

void COtherWarn2Page::OnOK() 
{
	UpdateData(TRUE);

	if(m_Type==1)
	{
		CString strIni = g_WSXHStr+"srvwarn.dat";
		WritePrivateProfileString("WARNOPT","Popup",IntToStr(m_bPopup),strIni);
		WritePrivateProfileString("WARNOPT","BeepFlag",IntToStr(m_nBeepFlag),strIni);
		WritePrivateProfileString("WARNOPT","WavFile",m_strWavFile,strIni);
	}
	else
	{
		g_WarnParam2.bTJPopup = m_bPopup;
		g_WarnParam2.nTJBeepFlag = m_nBeepFlag;
		strcpy(g_WarnParam2.TJWavFile,m_strWavFile);
	}

	CPropertyPage::OnOK();
}

void COtherWarn2Page::OnBrowse()
{
	UpdateData(TRUE);
	CFileDialog FileDialog(TRUE,NULL,NULL,
							OFN_NOCHANGEDIR,
							_F("声音文件(*.wav)|*.wav|全部类型|*.*||"));
	if (FileDialog.DoModal() == IDOK)
	{
		m_strWavFile = FileDialog.GetPathName();
		UpdateData(FALSE);
	}	
}

void COtherWarn2Page::OnBeepRadio()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_BROWSE)->EnableWindow(m_nBeepFlag==2);
	GetDlgItem(IDC_TEST)->EnableWindow(m_nBeepFlag==2);
	GetDlgItem(IDC_WAVFILE_EDIT)->EnableWindow(m_nBeepFlag==2);	
}

void COtherWarn2Page::OnTest() 
{
	UpdateData(TRUE);
	if(m_strWavFile.IsEmpty())
	{
		TDX_MessageBox(NULL,"请选择声音文件!",MB_OK|MB_ICONERROR);
		return;
	}
	::PlaySound(m_strWavFile,NULL,SND_ASYNC);	
}
