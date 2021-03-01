// GSDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "GSDlg.h"
#include "KeyGuy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGSDlg dialog


CGSDlg::CGSDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGSDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGSDlg)
	//}}AFX_DATA_INIT
	KeyGuyFlag = FALSE;
	m_strGPCode = "";
}


void CGSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGSDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGSDlg, CDialog)
	//{{AFX_MSG_MAP(CGSDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGSDlg message handlers

BOOL CGSDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	
	return TRUE;
}

void CGSDlg::OnOK() 
{

	CDialog::OnOK();
}

void CGSDlg::OnCancel() 
{
	
	CDialog::OnCancel();
}

void CGSDlg::StartKeyBuy(BOOL bAuto,char tempchar)
{
	CKeyGuy dlg;

	CRect rect;
	GetWindowRect(&rect);
	dlg.RightBottomPoint = CPoint(rect.right,rect.bottom);

	dlg.bHasCmd = FALSE;
	dlg.bAlwaysOn = TRUE;
	KeyGuyFlag = TRUE;

	char tempstr[2];
	if(bAuto)
	{
		tempstr[0]=tempchar;
		tempstr[1]='\0';
		dlg.m_edit=(CString)tempstr;
	}
	if( dlg.DoModal() == IDOK )
	{
		int index=-1;
		if(dlg.ReturnType >= 20) //如果是扩展行情
			index = g_pStockIO->GetIndex(dlg.ReturnValue.Mid(2),dlg.ReturnType-20);
		else if(dlg.ReturnType == 0)
		{
			CString tmpCode = dlg.ReturnValue.Mid(1);
			short	setcode = dlg.ReturnValue[0]-'0';
			index = g_pStockIO->GetIndex(tmpCode,setcode);
		}
		if(index == -1)
		{	
			KeyGuyFlag = FALSE;
			return;
		}
		MemStkInfo *pInfo = (*g_pStockIO)[index];
		if(pInfo)
		{
			m_strGPCode = pInfo->Code;
			GetDlgItem(IDC_GPCODE)->SetWindowText(m_strGPCode);
		}
	}
	KeyGuyFlag=FALSE;
}

BOOL CGSDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_CHAR && KeyGuyFlag == FALSE)
	{
		if((pMsg->wParam>=0x30&&pMsg->wParam<=0x39)||(pMsg->wParam>='a'&&pMsg->wParam<='z')
		||(pMsg->wParam>='A'&&pMsg->wParam<='Z')||pMsg->wParam== '.'||pMsg->wParam== '?')		
		{
			StartKeyBuy(TRUE,(char)pMsg->wParam);
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
