// MyStatusBar.cpp : implementation file

#include "stdafx.h"
#include "TdxW.h"
#include "MyStatusBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CMyStatusBar::CMyStatusBar()
{
	m_pStatusWnd = NULL;
	m_pMovingInfoDlg = NULL;
}

CMyStatusBar::~CMyStatusBar()
{
}

BEGIN_MESSAGE_MAP(CMyStatusBar, CStatusBar)
	//{{AFX_MSG_MAP(CMyStatusBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int nGetRollBarHeight()
{
	int nHeight = 0;
	if(g_GH.Roll.bRollOpen[0])
		nHeight+=g_nRollInfoHeight;
	if(g_GH.Roll.bRollOpen[1] && g_GC.bSupportSlowStatusRoll)
		nHeight+=g_nRollInfoHeight;
	return nHeight;
}

void GetOpenStatusFromRollShow()
{
	if(g_GH.Roll.bFloat) return;
	if(g_GH.Roll.bShow)
	{
		g_GH.Roll.bRollOpen[0]=TRUE;
		if(g_GC.bSupportSlowStatusRoll)
			g_GH.Roll.bRollOpen[1]=TRUE;
	}
	else
	{
		g_GH.Roll.bRollOpen[0]=FALSE;
		if(g_GC.bSupportSlowStatusRoll)
			g_GH.Roll.bRollOpen[1]=FALSE;
	}
}

void GetRollShowFromOpenStatus()
{
	if(g_GH.Roll.bFloat) return;
	g_GH.Roll.bShow = g_GH.Roll.bRollOpen[0] || g_GH.Roll.bRollOpen[1];
}

int CMyStatusBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatusBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pStatusWnd = new CStatusWnd;
	if(m_pStatusWnd)
		m_pStatusWnd->Create(_T("RaderKing"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,ID_STATUS_WND);

	if(g_GC.bSupportStatusRoll)
	{
		m_pMovingInfoDlg = new CMovingInfoDlg(this);
		m_pMovingInfoDlg->Create(IDD_MOVINGINFO,this);
		m_pMovingInfoDlg->DeleteAllSysInfo();

		SYS_INFO si={0};
		strcpy(si.content,g_strPreName);
		si.url[0] = 0;
	//	m_pMovingInfoDlg->AddSysInfo(si);

		strncpy(si.content,(char*)(LPCSTR)g_strMoving, 255);
	//	strncpy(si.url, "www.tdx.com.cn", 255);
		m_pMovingInfoDlg->AddSysInfo(si);	

		m_pMovingInfoDlg->ShowWindow(SW_SHOW);
	}

	return 0;
}

void CMyStatusBar::RedrawStatusWnd()
{
	if(m_pStatusWnd && ::IsWindow(m_pStatusWnd->m_hWnd))
		m_pStatusWnd->Invalidate(FALSE);
}

void CMyStatusBar::OnSize(UINT nType, int cx, int cy) 
{
	CStatusBar::OnSize(nType, cx, cy);

	if(g_GH.Roll.bShow && !g_GH.Roll.bFloat)
	{
		CRect rc;
		GetClientRect(&rc);
		rc.left+=(0.6f*rc.Width());

		rc.top=rc.top+2;
		if(g_GH.Roll.bRollOpen[1])
			rc.top+=g_nRollInfoHeight-2;
		rc.bottom=rc.top+g_nRollInfoHeight-2;
		m_pMovingInfoDlg->SetWindowPos(NULL,rc.left,rc.top,rc.Width(),rc.Height(),SWP_SHOWWINDOW);
	}
	if(m_pStatusWnd && ::IsWindow(m_pStatusWnd->m_hWnd))
	{
		CRect rc;
		GetClientRect(&rc);
		rc.right =(0.6f*rc.Width());
		rc.top+=2;
		if(g_GH.Roll.bRollOpen[0] && g_GH.Roll.bRollOpen[1])
			rc.top-=1;
		m_pStatusWnd->SetWindowPos(NULL,rc.left,rc.top,rc.Width(),rc.Height(),SWP_SHOWWINDOW);
	}
}

void CMyStatusBar::OnDestroy() 
{
	TDEL(m_pMovingInfoDlg);
	TDEL(m_pStatusWnd);

	CStatusBar::OnDestroy();	
}

BOOL CMyStatusBar::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==UM_ROLLINFOPOP_MSG || pMsg->message==UM_SLOWROLLINFOPOP_MSG || pMsg->message==UM_CLOSEROLL_MSG || pMsg->message==UM_CLOSESLOWROLL_MSG)
	{
		if(g_pMainWin)
		{
			g_pMainWin->PostMessage(pMsg->message,pMsg->wParam,pMsg->lParam);
			return TRUE;
		}
	}
	return CStatusBar::PreTranslateMessage(pMsg);	
}
