// ExportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "ExportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int s_LastExportTab=0;

/////////////////////////////////////////////////////////////////////////////
// CExportDlg dialog

CExportDlg::CExportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExportDlg)
	m_strFileName = _T("");
	m_bAllHQ = 0;
	//}}AFX_DATA_INIT
	m_nTabSel = s_LastExportTab;
	m_bGrid = FALSE;
	m_strInitFileName = "T1";
}


void CExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExportDlg)
	DDX_Control(pDX, IDC_INFO_STATIC, m_Info_Static);
	DDX_Control(pDX, IDC_TAB, m_TabCtrl);
	DDX_Text(pDX, IDC_FILENAME, m_strFileName);
	DDV_MaxChars(pDX, m_strFileName, 255);
	DDX_Radio(pDX, IDC_ALLHQ_RADIO, m_bAllHQ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExportDlg, CDialog)
	//{{AFX_MSG_MAP(CExportDlg)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_ADVANCE, OnAdvance)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExportDlg message handlers

BOOL CExportDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Info_Static.SetTextColor(RGB(0,64,64));

	m_TabCtrl.InsertItem(0,_F("格式文本文件"),0);
	m_TabCtrl.InsertItem(1,_F("Excel文件"),0);
	m_TabCtrl.InsertItem(2,_F("图形文件"),0);
	m_TabCtrl.SetCurSel(m_nTabSel);

	ShowOrHide();

	return TRUE;
}

void CExportDlg::ShowOrHide()
{
	GetDlgItem(IDC_INFO_STATIC)->ShowWindow(m_nTabSel!=2?SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_ALLHQ_RADIO)->ShowWindow(m_nTabSel!=2?SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_ALLHQ_RADIO2)->ShowWindow(m_nTabSel!=2?SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_HINT_STATIC)->ShowWindow(m_nTabSel!=2?SW_SHOW:SW_HIDE);

	GetDlgItem(IDC_ALLHQ_RADIO)->ShowWindow(m_bGrid?SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_ALLHQ_RADIO2)->ShowWindow(m_bGrid?SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_HINT_STATIC)->ShowWindow(m_bGrid?SW_SHOW:SW_HIDE);

	if(m_nTabSel==0)
		m_strFileName = g_PersonalPath+m_strInitFileName+".TXT";
	else if(m_nTabSel==1)
		m_strFileName = g_PersonalPath+m_strInitFileName+".XLS";
	else if(m_nTabSel==2)
		m_strFileName = g_PersonalPath+m_strInitFileName+".BMP";
	UpdateData(FALSE);
}

void CExportDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_nTabSel = m_TabCtrl.GetCurSel();
	ShowOrHide();
	*pResult = 0;
}

void CExportDlg::OnOK() 
{
	UpdateData(TRUE);
	TrimString(m_strFileName);
	if(m_strFileName.IsEmpty())
	{
		GetDlgItem(IDC_FILENAME)->SetFocus();
		return;
	}
	int nPos = m_strFileName.ReverseFind('\\');
	if(nPos>0)
		g_PersonalPath = m_strFileName.Left(nPos+1);

	s_LastExportTab=m_nTabSel;
	CDialog::OnOK();
}

void CExportDlg::OnBrowse() 
{
	UpdateData(TRUE);
	CFileDialog FileDialog(FALSE/*Save AS*/,NULL,(m_nTabSel==0)?(g_PersonalPath+m_strInitFileName+".TXT"):((m_nTabSel==1)?(g_PersonalPath+m_strInitFileName+".XLS"):(g_PersonalPath+m_strInitFileName+".BMP")),
							OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXTENSIONDIFFERENT,
							(m_nTabSel==0)?_F("文本文件(*.TXT)|*.TXT||"):((m_nTabSel==1)?_F("Excel文件(*.XLS)|*.XLS||"):_F("BMP文件(*.BMP)|*.BMP||")));
	if (FileDialog.DoModal() == IDOK)
	{
		CString FilePath = FileDialog.GetPathName();
		CString FileName = FileDialog.GetFileName();
		int nPos = FilePath.Find(FileName);
		if(nPos>0) g_PersonalPath = FilePath.Left(nPos);
		GetDlgItem(IDC_FILENAME)->SetWindowText(FileDialog.GetPathName());
	}
}

#include "ExportExtDlg.h"
void CExportDlg::OnAdvance() 
{
	CDialog::OnCancel();

	CExportExtDlg dlg;
	dlg.DoModal();
}
