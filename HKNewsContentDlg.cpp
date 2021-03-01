// EmbedNewsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "UHKNews.h"
#include "MyRichEdit.h"
#include "HKNewsContentDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEmbedNewsDlg dialog


CHKNewsContentDlg::CHKNewsContentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHKNewsContentDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEmbedNewsDlg)
	//}}AFX_DATA_INIT
	m_NewsContent = _T("");
	m_pNewsUnit = NULL;
	m_pRichEdit = NULL;

	m_hIcon = AfxGetApp()->LoadIcon(IDI_ZX2);
}

CHKNewsContentDlg::~CHKNewsContentDlg()
{
	TDEL(m_pRichEdit);
}

void CHKNewsContentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEmbedNewsDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHKNewsContentDlg, CDialog)
	//{{AFX_MSG_MAP(CEmbedNewsDlg)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEmbedNewsDlg message handlers

BOOL CHKNewsContentDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon,TRUE);
	SetIcon(m_hIcon,FALSE);
	
	if(!m_pRichEdit)
	{
		m_pRichEdit = new CMyRichEdit;
		CRect rc; GetClientRect(&rc);
		m_pRichEdit->Create(WS_VISIBLE|WS_CHILD|ES_AUTOVSCROLL|ES_READONLY|WS_VSCROLL|WS_TABSTOP|ES_MULTILINE,rc,this,IDC_GGINFO);
		m_pRichEdit->SetMenuIndex(-1);
		m_pRichEdit->ApplyTxtSize();
		//°×É«±³¾°
		m_pRichEdit->SetBackgroundColor(FALSE,TxtColor.TxtBackColor);
		CHARFORMAT cf;
		m_pRichEdit->GetDefaultCharFormat(cf);
		cf.crTextColor = TxtColor.TxtForeColor;
		m_pRichEdit->SetDefaultCharFormat(cf);
	}
	
	return TRUE;
}

void CHKNewsContentDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	CRect rc; GetClientRect(&rc);
	if(m_pRichEdit)
		m_pRichEdit->MoveWindow(rc);
}

void CHKNewsContentDlg::OnClose() 
{
	CWnd::OnClose();

	if(m_pNewsUnit)
		m_pNewsUnit->KillDlg();
}
//////////////////////////////////////////////////////////////////////////
void	CHKNewsContentDlg::UpdateContent(CString &strContent)
{
	m_pRichEdit->SetRedraw(FALSE);
	m_pRichEdit->SetWindowText(strContent);
	m_pRichEdit->SetRedraw(TRUE);
	m_pRichEdit->Invalidate(FALSE);
}
