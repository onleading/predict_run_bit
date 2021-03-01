// CMyTabCtrlBar.cpp : implementation file
//
// DevStudio Style Resizable Docking Tab Control Bar.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CMYTabCtrlBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyTabCtrlBar

CMyTabCtrlBar::CMyTabCtrlBar()
{
	m_nActiveTab = 0;
	memset(&m_nAvailIE,0,sizeof(short)*30);
	m_bShowTab=TRUE;
	m_bAutoSize=TRUE;
}

CMyTabCtrlBar::~CMyTabCtrlBar()
{
	while(!m_wnds.IsEmpty())
	{
		TCB_ITEM *pMember=m_wnds.RemoveHead();
		delete pMember;
	}
}

#define IDC_TABCTRLBAR 616

BEGIN_MESSAGE_MAP(CMyTabCtrlBar, CMyControlBar)
	ON_MESSAGE(UM_SWITCHBAR2_CHANGE,OnSwitchBar2SelChange)
	//{{AFX_MSG_MAP(CMyTabCtrlBar)
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGED()
	ON_MESSAGE(UM_TREEREFLECT_MESSAGE,OnTreeReflectMsg)
	ON_MESSAGE(UM_STARTFUNC_MESSAGE,OnStartFuncMessage)
	ON_MESSAGE(UM_DOLEVINBUY_MESSAGE,OnDoLevinBuyMessage)
	ON_MESSAGE(UM_DOLEVINSELL_MESSAGE,OnDoLevinSellMessage)
	ON_MESSAGE(UM_DOWTCANCEL_MESSAGE,OnDoWTCancelMessage)
	ON_MESSAGE(UM_DOQZSDS_MESSAGE,OnDoQzSDSMessage)

	ON_COMMAND(IDC_BUTTON_BACK, OnButtonBack)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_BACK, OnUpdateButtonBack)	

	ON_COMMAND(IDC_BUTTON_FORWARD, OnButtonForward)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_FORWARD, OnUpdateButtonForward)	

	ON_COMMAND(IDC_BUTTON_MAINPAGE, OnButtonMainPage)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_MAINPAGE, OnUpdateButtonMainPage)	
	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyTabCtrlBar message handlers

void CMyTabCtrlBar::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CWnd *pWnd;
	if(!m_bShowTab)
		m_tabctrl.MoveWindow(0,0,0,0);

	if (IsFloating()) 
	{
		if(m_bShowTab)
			m_tabctrl.MoveWindow( 1, lpwndpos->cy-21, lpwndpos->cx-1, 23 );
		for (POSITION pos=m_wnds.GetHeadPosition(); pos; m_wnds.GetNext(pos)) 
		{
			pWnd=m_wnds.GetAt(pos)->pWnd;
			if(m_bShowTab)
				pWnd->MoveWindow(1, 1, lpwndpos->cx-1, lpwndpos->cy-22);
			else
				pWnd->MoveWindow(1, 1, lpwndpos->cx-1, lpwndpos->cy-1);
		}
	}
	else if (IsHorzDocked()) 
	{
		if(!m_bRightBtn)
		{
			if(m_bShowTab)
			{
				if(m_bGripper)
					m_tabctrl.MoveWindow( 11, lpwndpos->cy-25, lpwndpos->cx-10, 23 );
				else
					m_tabctrl.MoveWindow( 1, lpwndpos->cy-25, lpwndpos->cx-1, 23 );
			}
			for (POSITION pos=m_wnds.GetHeadPosition(); pos; m_wnds.GetNext(pos)) 
			{
				pWnd=m_wnds.GetAt(pos)->pWnd;
				if(m_bShowTab)
				{
					if(m_bGripper)
						pWnd->MoveWindow(12, -1, lpwndpos->cx-14, lpwndpos->cy-24);
					else
						pWnd->MoveWindow(0, 0, lpwndpos->cx, lpwndpos->cy-24);
				}
				else
				{
					if(m_bGripper)
						pWnd->MoveWindow(12, -1, lpwndpos->cx-14, lpwndpos->cy);
					else
						pWnd->MoveWindow(0, 0, lpwndpos->cx, lpwndpos->cy-6);
				}
			}
		}
		else
		{
			if(m_bShowTab)
				m_tabctrl.MoveWindow( 1, lpwndpos->cy-25, lpwndpos->cx-15, 23 );
			for (POSITION pos=m_wnds.GetHeadPosition(); pos; m_wnds.GetNext(pos)) 
			{
				pWnd=m_wnds.GetAt(pos)->pWnd;
				if(m_bShowTab)
				{
					if(m_bGripper)
						pWnd->MoveWindow(1, -1, lpwndpos->cx-16, lpwndpos->cy-24);
					else
						pWnd->MoveWindow(2, -1, lpwndpos->cx-4, lpwndpos->cy-24);
				}
				else
				{
					if(m_bGripper)
						pWnd->MoveWindow(1, -1, lpwndpos->cx-16, lpwndpos->cy);
					else
						pWnd->MoveWindow(2, -1, lpwndpos->cx-4, lpwndpos->cy);
				}
			}
		}
	}
	else 
	{
		if(g_bTopTabWorkSpace)
		{
			CRect tmprc=GetButtonRect();
				m_tabctrl.MoveWindow( 2, 2, lpwndpos->cx-7-tmprc.Width()-2, 20 );
			for (POSITION pos=m_wnds.GetHeadPosition(); pos; m_wnds.GetNext(pos)) 
			{
				pWnd=m_wnds.GetAt(pos)->pWnd;
				pWnd->MoveWindow(2, 24, lpwndpos->cx-7, lpwndpos->cy-31);
			}
		}
		else
		{
			if(m_bShowTab)
				m_tabctrl.MoveWindow( 2, lpwndpos->cy-28, lpwndpos->cx-7, 24 );
			for (POSITION pos=m_wnds.GetHeadPosition(); pos; m_wnds.GetNext(pos)) 
			{
				pWnd=m_wnds.GetAt(pos)->pWnd;
				if(m_bShowTab)
				{
					if(m_bGripper)
						pWnd->MoveWindow(2, 14, lpwndpos->cx-7, lpwndpos->cy-42);
					else
						pWnd->MoveWindow(2, 0, lpwndpos->cx-7, lpwndpos->cy-28);
				}
				else
				{
					if(m_bGripper)
						pWnd->MoveWindow(2, 14, lpwndpos->cx-7, lpwndpos->cy-14);
					else
						pWnd->MoveWindow(2, 0, lpwndpos->cx-7, lpwndpos->cy);
				}
			}
		}
	}
	CMyControlBar::OnWindowPosChanged(lpwndpos);
}

// *** K.Stowell
CImageList* CMyTabCtrlBar::SetTabImageList(CImageList *pImageList)
{
	return m_tabctrl.SetImageList (pImageList);
}

int CMyTabCtrlBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMyControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_tabctrl.Create(_T("MyControlBar"),"",WS_CHILD|WS_VISIBLE,
		CRect(0,0,0,0), this, IDC_TABCTRLBAR))
	{
		TRACE0("Unable to create tab control bar\n");
		return -1;
	}
	
	m_tabctrl.Init(m_hWnd,UM_SWITCHBAR2_CHANGE);

	m_tabctrl.SetMinTabWidth(18);	//设置最小TAB宽度

	return 0;
}

int  CMyTabCtrlBar::GetCurWnd()
{
	return m_tabctrl.GetCurSel();
}

void CMyTabCtrlBar::OnSwitchBar2SelChange(WPARAM wParam,LPARAM lParam)
{
	SetActiveWnd(m_tabctrl.GetCurSel());
	Invalidate();
}

//////////////////////////////////////////////////
// The remainder of this class was written by Dirk Clemens...

BOOL CMyTabCtrlBar::AddWnd(LPCTSTR lpszLabel, CRuntimeClass *pWndClass,BOOL bIE,BOOL bMine)
{
	char tabtitle[100];
	strcpy(tabtitle,lpszLabel);

	CWnd* pWnd;
	TRY
	{
		pWnd = (CWnd*)pWndClass->CreateObject();
		if (pWnd == NULL)
			AfxThrowMemoryException();
	}
	CATCH_ALL(e)
	{
		TRACE0("Out of memory creating a Wnd.\n");
		return FALSE;
	}
	END_CATCH_ALL
		
	// Create with the right size and position
	if (!pWnd->Create(NULL,NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this, 0))
	{
		TRACE0("Warning: couldn't create client pane for Wnd.\n");
		return FALSE;
	}

	TCB_ITEM *pMember=new TCB_ITEM;
	pMember->pWnd=pWnd;
	strcpy(pMember->szLabel, tabtitle);
	m_wnds.AddTail(pMember);
	
	int nWnds = m_wnds.GetCount();
	if (nWnds!=1)
	{
		pWnd->EnableWindow(FALSE);
		pWnd->ShowWindow(SW_HIDE);
	}

	m_tabctrl.AddItem((LPTSTR)tabtitle);

	if(bIE && nWnds < 30)
		m_nAvailIE[nWnds-1]=1;

	return TRUE;
}

void CMyTabCtrlBar::RemoveWnd(int nWnd)
{
	if(nWnd < 0) return;
	m_wnds.RemoveAt(m_wnds.FindIndex(nWnd));
}

void CMyTabCtrlBar::SetActiveWnd(int nNewTab)
{
	if(m_bAutoSize)
	{
		m_sizeVert.cx = g_nDefaultZxWidth; 
		m_pDockSite->DelayRecalcLayout();
	}
		
	if (nNewTab < 0 || nNewTab>=GetWndNum() ) return; 
	if (nNewTab!=-1 && nNewTab!=m_nActiveTab)
	{
        TCB_ITEM *newMember=m_wnds.GetAt(m_wnds.FindIndex(nNewTab));
        TCB_ITEM *oldMember=NULL;
		if(!newMember) return;

        if (m_nActiveTab!=-1)
        {
            oldMember=m_wnds.GetAt(m_wnds.FindIndex(m_nActiveTab));
            oldMember->pWnd->EnableWindow(FALSE);
            oldMember->pWnd->ShowWindow(SW_HIDE);
        }
        newMember->pWnd->EnableWindow(TRUE);
        newMember->pWnd->ShowWindow(SW_SHOW);
        newMember->pWnd->SetFocus();
        m_nActiveTab = nNewTab;
		m_tabctrl.SetCurSel(m_nActiveTab);

		newMember->pWnd->SendMessage(UM_SHOWIT_MESSAGE,0,0);
    }
}

int CMyTabCtrlBar::GetActiveWnd()
{
	return m_nActiveTab;
}

CWnd* CMyTabCtrlBar::GetWnd(int nWnd)
{
	if (nWnd!=-1)
	{
        TCB_ITEM *pMember=m_wnds.GetAt(m_wnds.FindIndex(nWnd));
		return (CWnd*)pMember->pWnd;
	}
	else
		return NULL;
}

int  CMyTabCtrlBar::GetWndNum()
{
	int nRet = 0;
	for (POSITION pos=m_wnds.GetHeadPosition(); pos; m_wnds.GetNext(pos))
		nRet++;
	return nRet;
}

void CMyTabCtrlBar::OnTreeReflectMsg(WPARAM wParam,LPARAM lParam)
{
	if(wParam < 0 || wParam >= GetWndNum()) return;
	GetWnd(wParam)->SendMessage(UM_TREEREFLECT_MESSAGE,0,0);
}

void CMyTabCtrlBar::OnStartFuncMessage(WPARAM wParam,LPARAM lParam)
{
	for(int i=0;i < GetWndNum();i++)
		GetWnd(i)->SendMessage(UM_STARTFUNC_MESSAGE,0,0);
}

void CMyTabCtrlBar::OnDoLevinBuyMessage(WPARAM wParam,LPARAM lParam)
{
	for(int i=0;i < GetWndNum();i++)
		GetWnd(i)->SendMessage(UM_DOLEVINBUY_MESSAGE,wParam,lParam);
}

void CMyTabCtrlBar::OnDoLevinSellMessage(WPARAM wParam,LPARAM lParam)
{
	for(int i=0;i < GetWndNum();i++)
		GetWnd(i)->SendMessage(UM_DOLEVINSELL_MESSAGE,wParam,lParam);
}

void CMyTabCtrlBar::OnDoWTCancelMessage(WPARAM wParam,LPARAM lParam)
{
	for(int i=0;i < GetWndNum();i++)
		GetWnd(i)->SendMessage(UM_DOWTCANCEL_MESSAGE,wParam,lParam);
}

void CMyTabCtrlBar::OnDoQzSDSMessage(WPARAM wParam,LPARAM lParam)
{
	for(int i=0;i < GetWndNum();i++)
		GetWnd(i)->SendMessage(UM_DOQZSDS_MESSAGE,wParam,lParam);
}

void CMyTabCtrlBar::OnButtonBack()
{
	int nCur = GetCurWnd();
	if(nCur < 0) return;
	CWnd *pWnd = GetWnd(nCur);
	if(pWnd)
		pWnd->SendMessage(UM_IE_BACK_MESSAGE,0,0);
}

void CMyTabCtrlBar::OnUpdateButtonBack(CCmdUI* pCmdUI)
{
	int nCur = GetCurWnd();
	if(nCur >= 0)
		pCmdUI->Enable(m_nAvailIE[nCur]);
	else
		pCmdUI->Enable(FALSE);
}

void CMyTabCtrlBar::OnButtonForward()
{
	int nCur = GetCurWnd();
	if(nCur < 0) return;
	CWnd *pWnd = GetWnd(nCur);
	if(pWnd)
		pWnd->SendMessage(UM_IE_FORWARD_MESSAGE,0,0);	
}

void CMyTabCtrlBar::OnUpdateButtonForward(CCmdUI* pCmdUI)
{
	int nCur = GetCurWnd();
	if(nCur >= 0)
		pCmdUI->Enable(m_nAvailIE[nCur]);
	else
		pCmdUI->Enable(FALSE);
}

void CMyTabCtrlBar::OnButtonMainPage()
{
	int nCur = GetCurWnd();
	if(nCur < 0) return;
	CWnd *pWnd = GetWnd(nCur);
	if(pWnd)
		pWnd->SendMessage(UM_IE_MAINPAGE_MESSAGE,0,0);
}

void CMyTabCtrlBar::OnUpdateButtonMainPage(CCmdUI* pCmdUI)
{
	int nCur = GetCurWnd();
	if(nCur >= 0)
		pCmdUI->Enable(m_nAvailIE[nCur]);
	else
		pCmdUI->Enable(FALSE);
}
