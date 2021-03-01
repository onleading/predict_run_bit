// TableTitleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "TableTitleDlg.h"
#include "CWTableUnit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTableTitleDlg dialog


CTableTitleDlg::CTableTitleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTableTitleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTableTitleDlg)
	m_CWTime1 = 0;
	m_CWTime2 = 0;
	m_strGpEdit = _T("");
	//}}AFX_DATA_INIT
}


void CTableTitleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTableTitleDlg)
	DDX_Control(pDX, IDC_STATIC_TITLE, m_TitleLabel);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_CWTime1);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_CWTime2);
	DDX_Text(pDX, IDC_GPEDIT, m_strGpEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTableTitleDlg, CDialog)
	//{{AFX_MSG_MAP(CTableTitleDlg)
	ON_WM_ERASEBKGND()
	ON_EN_CHANGE(IDC_GPEDIT, OnChangeGpedit)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnButtonRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTableTitleDlg message handlers

BOOL CTableTitleDlg::OnEraseBkgnd(CDC* pDC) 
{
	
	return CDialog::OnEraseBkgnd(pDC);
}

void CTableTitleDlg::OnChangeGpedit() 
{
	 m_strGpEdit = _T("");
	 UpdateData(FALSE);
}

#include "KeyGuy.h"
BOOL CTableTitleDlg::PreTranslateMessage(MSG* pMsg) 
{
	BOOL KeyGuyFlag = FALSE;
	BOOL bEditFocus  = (GetFocus()==GetDlgItem(IDC_GPEDIT));
	if (pMsg->message == WM_CHAR && KeyGuyFlag == FALSE && bEditFocus)
	{
		CWnd *pWnd = this;
		if(pMsg->wParam>=0x81&&pMsg->wParam<=0xFE&&g_KeyGuyCnInfo.nKeyGuyCNing==0)	//中文开始输入
		{	
			memset(&g_KeyGuyCnInfo, 0, sizeof(g_KeyGuyCnInfo));
			g_KeyGuyCnInfo.nKeyGuyCNing = 1;
			g_KeyGuyCnInfo.pOwner = pWnd;
			g_KeyGuyCnInfo.strKeyGuyCN[0]=(char)pMsg->wParam;
			g_KeyGuyCnInfo.strKeyGuyCN[1]=0;
		}
		else if(g_KeyGuyCnInfo.nKeyGuyCNing==1&&g_KeyGuyCnInfo.pOwner==pWnd)		//输入中
		{
			::KillTimer(AfxGetMainWnd()->GetSafeHwnd(), 100);
			int nCNLen = strlen(g_KeyGuyCnInfo.strKeyGuyCN);
			g_KeyGuyCnInfo.strKeyGuyCN[nCNLen]=(char)pMsg->wParam;
			g_KeyGuyCnInfo.strKeyGuyCN[nCNLen+1]=0;
			//用于汉字输入
			::SetTimer(AfxGetMainWnd()->GetSafeHwnd(), 100, 10, NULL);
		}
		else if((pMsg->wParam>=0x30&&pMsg->wParam<=0x39)||(pMsg->wParam>='a'&&pMsg->wParam<='z')
			||(pMsg->wParam>='A'&&pMsg->wParam<='Z')||pMsg->wParam== '.'||pMsg->wParam== '?'||pMsg->wParam== '*')
		{
			CKeyGuy dlg;
			CRect rect;
			pWnd->GetWindowRect(&rect);
			dlg.RightBottomPoint = CPoint(rect.right,rect.bottom)+CSize(0, 160);
			
			dlg.bHasCmd = TRUE;
			KeyGuyFlag = TRUE;
			
			char tempchar;
			char tempstr[2]={0};
			
			tempchar   = (char)pMsg->wParam;
			tempstr[0] = tempchar;
			tempstr[1] = 0;
			dlg.m_edit = (CString)tempstr;
			
			if( dlg.DoModal() == IDOK && dlg.ReturnType != -1)
			{
				char GPCode[SH_CODE_LEN+2]={0};
				strcpy(GPCode, (char*)(LPCSTR)dlg.ReturnValue);
				MemStkInfo *pInfo = g_pStockIO->Get_MemStkInfo(&GPCode[1], GPCode[0]-'0');
				m_strGpEdit = pInfo->Name;
				UpdateData(FALSE);
			}
			
			KeyGuyFlag = FALSE;
			
			return TRUE;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CTableTitleDlg::ShowMore(short nTimeFlag, short nGpInputFlag)
{
	m_nTimeFlag = nTimeFlag;
	m_nGpInputFlag = nGpInputFlag;

	if(nTimeFlag<1)
	{
		GetDlgItem(IDC_STATICBGQ)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DATETIMEPICKER1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DATETIMEPICKER2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATICT2T)->ShowWindow(SW_HIDE);
	}
	else if(nTimeFlag==1)
	{
		GetDlgItem(IDC_DATETIMEPICKER2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATICT2T)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATICBGQ)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DATETIMEPICKER1)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_STATICBGQ)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DATETIMEPICKER1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DATETIMEPICKER2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATICT2T)->ShowWindow(SW_SHOW);
	}
	if(nGpInputFlag<1)
	{
		GetDlgItem(IDC_STATICGPI)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_GPEDIT)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_STATICGPI)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_GPEDIT)->ShowWindow(SW_SHOW);
	}
	if(nTimeFlag<1&&nGpInputFlag<1)
		GetDlgItem(IDC_BUTTON_REFRESH)->ShowWindow(SW_HIDE);
	else GetDlgItem(IDC_BUTTON_REFRESH)->ShowWindow(SW_SHOW);
}

void CTableTitleDlg::GetTimeParams(TableBGQ *pBgq)
{
	UpdateData(TRUE);
	pBgq->nCwTimeS = 10000*m_CWTime1.GetYear()+100*m_CWTime1.GetMonth()+m_CWTime1.GetDay();
	pBgq->nCwTimeE = 10000*m_CWTime2.GetYear()+100*m_CWTime2.GetMonth()+m_CWTime2.GetDay();
}

void CTableTitleDlg::OnOK()
{
	
}

void CTableTitleDlg::OnCancel()
{
	
}

BOOL CTableTitleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_TitleLabel.SetTextColor(RGB(255,0,0));
	m_CWTime1 = m_CWTime2 = CTime::GetCurrentTime();
	m_CWTime1 -= CTimeSpan(365*2,0,0,0);
	ShowMore(0, 0);
	UpdateData(FALSE);
	
	return TRUE;
}

void CTableTitleDlg::OnButtonRefresh() 
{
	if(!m_pTableUnit) return;

	UpdateData(TRUE);

	TableBGQ tbgq={0};
	if(m_nTimeFlag==1)
	{
		tbgq.nCwTimeS = tbgq.nCwTimeE = 10000*m_CWTime1.GetYear()+100*m_CWTime1.GetMonth()+m_CWTime1.GetDay();
		m_pTableUnit->SetParamAndGetData(&tbgq, TRUE);
	}
	else if(m_nTimeFlag==2)
	{
		tbgq.nCwTimeS =10000*m_CWTime1.GetYear()+100*m_CWTime1.GetMonth()+m_CWTime1.GetDay();
		tbgq.nCwTimeE =10000*m_CWTime2.GetYear()+100*m_CWTime2.GetMonth()+m_CWTime2.GetDay();
		m_pTableUnit->SetParamAndGetData(&tbgq, TRUE);
	}
	else 	m_pTableUnit->GetData(TRUE);
}
