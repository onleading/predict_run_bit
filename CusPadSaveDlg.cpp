// CusPadSaveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "CusPadSaveDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCusPadSaveDlg dialog


CCusPadSaveDlg::CCusPadSaveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCusPadSaveDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCusPadSaveDlg)
	m_sChName = _T("");
	m_sEnName = _T("");
	m_sCmdNum = _T("");
	//}}AFX_DATA_INIT
	m_iDynaTab = -1;
	m_nCmdNum = -1;
	m_editEnName.setPermitStyle(CEditEx::PMTSTY_FILENAME);
}


void CCusPadSaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCusPadSaveDlg)
	DDX_Control(pDX, IDC_ENNAME, m_editEnName);
	DDX_Control(pDX, IDC_ERRORINFO, m_errInfo);
	DDX_Text(pDX, IDC_CHNAME, m_sChName);
	DDV_MaxChars(pDX, m_sChName, 38);
	DDX_Text(pDX, IDC_ENNAME, m_sEnName);
	DDV_MaxChars(pDX, m_sEnName, 14);
	DDX_Text(pDX, IDC_NUMNAME, m_sCmdNum);
	DDV_MaxChars(pDX, m_sCmdNum, 4);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCusPadSaveDlg, CDialog)
	//{{AFX_MSG_MAP(CCusPadSaveDlg)
	ON_EN_CHANGE(IDC_CHNAME, OnChangeChname)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCusPadSaveDlg message handlers

void CCusPadSaveDlg::OnChangeChname()
{	
	UpdateData(TRUE);
	m_sEnName=GetPYStr(m_sChName,5);
	UpdateData(FALSE);
}


BOOL CCusPadSaveDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int nCmdNum;
	if(g_vDynaCmdTab.empty())	nCmdNum = 1;
	else nCmdNum = GetNextDef();
	char str[4]; Num2KeyString(str, nCmdNum);
	m_sCmdNum = "." + (CString)str;
	for(int i=0;i<g_vDynaCmdTab.size();i++)
	{
		if(strcmp(m_sChName, g_vDynaCmdTab[i].ChName)==0 &&
			strcmp(m_sEnName, g_vDynaCmdTab[i].EnName)==0)
		{
			m_sCmdNum = (CString)g_vDynaCmdTab[i].CmdInfo.Code;
			m_iDynaTab = i;
			break;
		}
	}
	UpdateData(FALSE);
	
	return TRUE;
}

void CCusPadSaveDlg::OnOK()
{
	UpdateData(TRUE);
	CDC *pDC = m_errInfo.GetDC();
	pDC->SetBkMode(TRANSPARENT);
	if(strcmp(m_sCmdNum, "")==0)
	{
		CFont *oldFont = g_d.SelectFont(pDC, GRAPH_FONT);
		m_errInfo.SetTextColor(RGB(255,0,0));
		m_errInfo.SetText(_F("快捷命令为空，请重新输入！"));
		g_d.RestoreFont(pDC, oldFont);
		return;
	}
	for(int i=0;m_iDynaTab>-1&&i<g_vDynaCmdTab.size();i++)
	{
		if(i==m_iDynaTab) continue;
		if(strcmp(m_sEnName, g_vDynaCmdTab[i].EnName)==0 &&
			strcmp(m_sChName, g_vDynaCmdTab[i].ChName)==0)
		{
			CFont *oldFont = g_d.SelectFont(pDC, GRAPH_FONT);
			m_errInfo.SetTextColor(RGB(255,0,0));
			m_errInfo.SetText(_F("名字有重复，请重新输入！"));
			g_d.RestoreFont(pDC, oldFont);
			return;
		}
	}
	if(strcmp(m_sEnName, "")==0 || strcmp(m_sChName, "")==0)
	{
		CFont *oldFont = g_d.SelectFont(pDC, GRAPH_FONT);
		m_errInfo.SetTextColor(RGB(255,0,0));
		m_errInfo.SetText(_F("名字为空，请重新输入！"));
		g_d.RestoreFont(pDC, oldFont);
		return;
	}
	if(!CheckInputCmdNum(pDC)) return;
	
	CDialog::OnOK();
}

BOOL CCusPadSaveDlg::CheckInputCmdNum(CDC *pDC)
{
	CFont *oldFont = g_d.SelectFont(pDC, GRAPH_FONT);
	m_errInfo.SetTextColor(RGB(255,0,0));

	LPTSTR curCmd = m_sCmdNum.GetBuffer(MAX_CMDNUM_STRING);
	if(m_sCmdNum.GetLength()!=MAX_CMDNUM_STRING)
	{
		m_errInfo.SetText(_F("快捷命令长度不匹配!格式为.0??"));
		m_sCmdNum = "";
		return FALSE;
	}
	if(m_sCmdNum[0]!='.')
	{
		m_errInfo.SetText(_F("快捷命令格式错误!格式为.0??"));
		m_sCmdNum = "";
		return FALSE;
	}
	char num[MAX_CMDNUM_STRING]; memset(num, 0, MAX_CMDNUM_STRING*sizeof(char));
	strncpy(num, curCmd+1, 3*sizeof(char));
	if(atoi(num)<MIN_CMD_NUM||atoi(num)>MAX_CMD_NUM)
	{
		m_errInfo.SetText(_F("快捷命令数字范围不对!格式为.0??"));
		m_sCmdNum = "";
		return FALSE;
	}
	for(int i=0;i<g_vDynaCmdTab.size();i++)
	{
		if(i==m_iDynaTab) continue;
		if(atoi(num)==g_vDynaCmdTab[i].nCmdNum)
		{
			m_errInfo.SetText(_F("已有版面用此快捷命令!"));
			m_sCmdNum = "";
			return FALSE;
		}
	}
	m_nCmdNum = atoi(num);
	m_sCmdNum.ReleaseBuffer();
	g_d.RestoreFont(pDC, oldFont);
	return TRUE;
}
