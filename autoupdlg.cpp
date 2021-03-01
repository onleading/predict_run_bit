// autoupdlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "autoupdlg.h"
#include "AutoupInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoupDlg dialog


CAutoupDlg::CAutoupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAutoupDlg)
	m_bAWeekNoHint = FALSE;
	//}}AFX_DATA_INIT
	m_bMustAutoup = FALSE;
}


void CAutoupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoupDlg)
	DDX_Control(pDX, IDC_CONTENT_EDIT, m_RichEditEx);
	DDX_Control(pDX, IDC_CLIENTVER_STATIC, m_ClientVer_Static);
	DDX_Check(pDX, IDC_AWEEKNOHINT_CHECK, m_bAWeekNoHint);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAutoupDlg, CDialog)
	//{{AFX_MSG_MAP(CAutoupDlg)
	ON_BN_CLICKED(IDC_AUTOUP_BUTTON, OnAutoupButton)
	ON_NOTIFY( EN_LINK, IDC_CONTENT_EDIT, OnRichEditExLink )
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoupDlg message handlers

BOOL CAutoupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_ClientVer_Static.SetTextColor(RGB(255,0,0));
	CString tmpStr;
	tmpStr.Format("客户端新版本V%.4f",g_NeedAutoupInfo.ThisVer);
	if(g_NeedAutoupInfo.bNeedAutoup==2)
	{
		tmpStr+=", 你的版本太低,必须升级!";
		m_bMustAutoup = TRUE;
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_AWEEKNOHINT_CHECK)->EnableWindow(FALSE);
	}
	m_ClientVer_Static.SetWindowText(_F(tmpStr));
	m_bAWeekNoHint=g_GH.AWeekNoHint;

	unsigned mask = ::SendMessage(m_RichEditEx.m_hWnd, EM_GETEVENTMASK, 0, 0);
    ::SendMessage(m_RichEditEx.m_hWnd, EM_SETEVENTMASK, 0, mask | ENM_LINK);
    ::SendMessage(m_RichEditEx.m_hWnd, EM_AUTOURLDETECT, true, 0);  //自动检测URL
	m_RichEditEx.SetWindowText(_F(g_NeedAutoupInfo.Descripton));
	UpdateData(FALSE);
	return TRUE;
}

void CAutoupDlg::SureNoHint()
{
	UpdateData(TRUE);
	if(m_bAWeekNoHint)
	{
		CTime NowDate = CTime::GetCurrentTime();		
		g_GH.NoHintStartDate=NowDate.GetYear()*10000+NowDate.GetMonth()*100+NowDate.GetDay();
	}
	g_GH.AWeekNoHint = m_bAWeekNoHint;
}

void CAutoupDlg::OnAutoupButton() 
{
	AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_AUTOUP);
	if(m_bMustAutoup)
	{
		g_bDirectQuit = TRUE;
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
	}
	else
		SureNoHint();
	
	CDialog::OnOK();
}

void CAutoupDlg::OnCancel() 
{
	if(m_bMustAutoup) return;
	SureNoHint();
	CDialog::OnCancel();
}

extern void ProcessRichEditExLink(CRichEditCtrl *pCtrl,NMHDR*  in_pNotifyHeader,LRESULT* out_pResult);
void CAutoupDlg::OnRichEditExLink(NMHDR*  in_pNotifyHeader,LRESULT* out_pResult)
{
	ProcessRichEditExLink(&m_RichEditEx,in_pNotifyHeader,out_pResult);
}

int CAutoupDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;	
	PeekMsg();	//这里不加PeekMsg,IE控件会吸收按钮,造成死锁 ???	
	return 0;
}
