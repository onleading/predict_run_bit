// TxtFindDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "TxtFindDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTxtFindDlg dialog


CTxtFindDlg::CTxtFindDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTxtFindDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTxtFindDlg)
	m_nRadio = 0;
	//}}AFX_DATA_INIT
	m_nFindNum = 0;
}


void CTxtFindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTxtFindDlg)
	DDX_Control(pDX, IDC_FINDTXT_COMBO, m_FindTxt_Combo);
	DDX_Radio(pDX, IDC_RADIO1, m_nRadio);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTxtFindDlg, CDialog)
	//{{AFX_MSG_MAP(CTxtFindDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTxtFindDlg message handlers

BOOL CTxtFindDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_FindTxt_Combo.SetItemHeight(0,16);
	m_FindTxt_Combo.SetItemHeight(-1,16);

	GetDlgItem(IDC_RADIO1)->SetWindowText(m_Str1);
	GetDlgItem(IDC_RADIO1)->EnableWindow(m_bEnable1);
	GetDlgItem(IDC_RADIO2)->SetWindowText(m_Str2);
	GetDlgItem(IDC_RADIO2)->EnableWindow(m_bEnable2);
	if(!m_bEnable1) m_nRadio = 1;
	if(!m_bEnable2) m_nRadio = 0;

	//ÃÓ≥‰Combo
	char str[TXTCACHE_NUM][30];
	int nCacheNum = LoadSomeString(str,"TXTFIND","CACHETXT",TXTCACHE_NUM,"CACHENUM",g_WSXHStr+g_strUserCfg);
	for(int i = 0;i < nCacheNum;i++)
		m_FindTxt_Combo.AddString(str[i]);
	if(nCacheNum > 0) 
		m_FindTxt_Combo.SetCurSel(0);

	UpdateData(FALSE);
	return TRUE;
}

void CTxtFindDlg::OnOK() 
{
	UpdateData(TRUE);
	m_FindTxt_Combo.GetWindowText(m_strFindTxt);
	TrimString(m_strFindTxt);
	if(m_strFindTxt.IsEmpty()) return;
	m_nFindNum = 0;
	int len = m_strFindTxt.GetLength(),pos,lastpos=0;
	while((pos=m_strFindTxt.Find("+",lastpos))!=-1)
	{
		m_FindStr[m_nFindNum++] = m_strFindTxt.Mid(lastpos,pos-lastpos);
		lastpos = pos+1;
		if(m_nFindNum >= MAX_ANDSTR) break;
	}	
	if(m_nFindNum < MAX_ANDSTR)
		m_FindStr[m_nFindNum++] = m_strFindTxt.Mid(lastpos);
	for(int i=0;i < m_nFindNum;i++)
		TrimString(m_FindStr[i]);
	g_nFindRadio = m_nRadio;

	char str[TXTCACHE_NUM][30];
	int nCacheNum = GetComboTextList(str,&m_FindTxt_Combo,TXTCACHE_NUM);
	SaveSomeString(str,"TXTFIND","CACHETXT",nCacheNum,"CACHENUM",g_WSXHStr+g_strUserCfg);
	CDialog::OnOK();
}
