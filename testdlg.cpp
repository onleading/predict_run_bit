// testdlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "testdlg.h"
#include "TxtView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestDlg dialog


CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestDlg)
	m_strNewCode = _T("");
	m_strRawCode = _T("");
	//}}AFX_DATA_INIT
}


void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestDlg)
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Text(pDX, IDC_NEWCODE_EDIT, m_strNewCode);
	DDV_MaxChars(pDX, m_strNewCode, 6);
	DDX_Text(pDX, IDC_RAWCODE_EDIT, m_strRawCode);
	DDV_MaxChars(pDX, m_strRawCode, 6);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
	//{{AFX_MSG_MAP(CTestDlg)
	ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestDlg message handlers

BOOL CTestDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CWDATA *pCWData = new CWDATA[1000];
	int nNowCWData = g_pStockIO->GetRecentGbbqInfo(pCWData,1000);
	char AllCode[1000][7];
	int nStkNum=0,i,j;
	for(i=0;i<nNowCWData;i++)
	{
		for(j=0;j<nStkNum;j++)
		{
			if(strcmp(pCWData[i].Code,AllCode[j])==0)
				break;
		}
		if(j>=nStkNum)
			strcpy(AllCode[nStkNum++],pCWData[i].Code);
	}
	delete pCWData;
	//
	int nGPNo,nCnt=0;
	MemStkInfo *pStkInfo;
	CString tmpStr;
	for(j=0;j<nStkNum;j++)
	{
		nGPNo=g_pStockIO->GetIndex(AllCode[j]);
		if(nGPNo>=0)
		{
			pStkInfo=(*g_pStockIO)[nGPNo];
			tmpStr.Format("%s-%s",pStkInfo->Code,pStkInfo->Name);
			m_List.InsertString(nCnt,tmpStr);
			m_List.SetItemData(nCnt,nGPNo);
			nCnt++;
		}
	}
	
	return TRUE;
}

void CTestDlg::OnOK() 
{
	UpdateData(TRUE);
	if(m_strNewCode==m_strRawCode) 
		return;
	MemStkInfo *tmpInfo=g_pStockIO->Get_MemStkInfo(m_strRawCode);
	if(!tmpInfo) return;
	strcpy(tmpInfo->Code,m_strNewCode);
	GetJBZL();
	g_bChangeDebugCode=TRUE;
}


void CTestDlg::OnDblclkList1() 
{
	int nCurSel=m_List.GetCurSel();
	if(nCurSel>=0)
	{
		int GPIndex=m_List.GetItemData(nCurSel);
		MemStkInfo *pStkInfo=(*g_pStockIO)[GPIndex];
		ToZst(pStkInfo->Code,pStkInfo->setcode,FALSE);
		TxtCurGPCode=pStkInfo->Code;
		TxtCurSetCode=pStkInfo->setcode;
		g_CurrentJBM = JBM_TDX;
		CTxtView *tempWnd = (CTxtView *)SwitchToView("Txt");
		if(tempWnd) tempWnd->SendMessage(WM_RICHEDIT_MESSAGE);
	}
}
