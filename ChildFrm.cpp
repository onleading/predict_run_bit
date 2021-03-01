// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "TdxW.h"

#include "ChildFrm.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_SYSCOMMAND()
	ON_WM_GETMINMAXINFO()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style&=~FWS_ADDTOTITLE;
	if( !CNewMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CNewMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CNewMDIChildWnd::Dump(dc);
}

#endif //_DEBUG


//让第一个行情窗口不能关闭
void CChildFrame::OnSysCommand(UINT nID, LPARAM lParam) 
{
	if( nID == SC_RESTORE || nID == SC_MINIMIZE )
		return;
	if (nID == SC_CLOSE && this->m_hWnd == FirstWindow )
		return;
	if (nID == SC_CLOSE && this->m_hWnd == BigMenuWindow )
		return;
	if (nID == SC_CLOSE )
	{
		AfxGetMainWnd()->PostMessage(WM_KEYDOWN,VK_ESCAPE);
		return;
	}
	CNewMDIChildWnd::OnSysCommand(nID, lParam);
}

void CChildFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;

	CNewMDIChildWnd::OnGetMinMaxInfo(lpMMI);

	POINT pt;
	if (pFrame->GetFullScreenSize(this, pt))
	{
		if(g_bHasCaption)
			pt.y-=20;
		lpMMI->ptMaxSize = pt;
		lpMMI->ptMaxTrackSize = pt;
	}
}

BOOL CChildFrame::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CNewMDIFrameWnd* pParentWnd, CCreateContext* pContext) 
{
	int temp = CNewMDIChildWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, pContext);
	ShowWindow(SW_SHOWMAXIMIZED);
	return temp;
}

BOOL CChildFrame::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CChildFrame::OnSize(UINT nType, int cx, int cy) 
{
	CNewMDIChildWnd::OnSize(nType, cx, cy);
}

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	LONG style = ::GetWindowLong(m_hWnd,GWL_STYLE);
	::SetWindowLong(m_hWnd,GWL_STYLE,style&(~WS_SYSMENU));

	if (CNewMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

void CChildFrame::CreateDSMenu() 
{
	int nPos=5;
	m_DSMenu.CreatePopupMenu();
	CString tmpMenuStr;
	tmpMenuStr.Format("连接%s",g_DS.DSOEMName);
	m_DSMenu.AppendMenu( MF_STRING,ID_DSINIT,_F(tmpMenuStr));
	tmpMenuStr.Format("断开%s",g_DS.DSOEMName);
	m_DSMenu.AppendMenu( MF_STRING,ID_UNINITDS,_F(tmpMenuStr));
	m_tmpMenu.InsertMenu(nPos,MF_BYPOSITION|MF_POPUP,(UINT)(m_DSMenu.m_hMenu),_F(g_DS.DSOEMName));
}

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
#if defined(OEM_NEWJY)
	return CNewMDIChildWnd::OnCreateClient(lpcs, pContext);
#endif
	
	/*
	if(g_nWebPageNum > 0) //构建Web菜单
	{		
		m_WebMenu.CreatePopupMenu();
		UINT StartID = ID_WEB_MENU_1;
		for(int i = 0;i < g_nWebPageNum;i++)
		{
			if(g_strWebPageURL[i][0]==0)
				m_WebMenu.AppendMenu( MF_SEPARATOR,StartID+i,g_strWebPageStr[i]);
			else
				m_WebMenu.AppendMenu( MF_STRING,StartID+i,g_strWebPageStr[i]);
		}		
	}
	if(g_nWebPageNum2 > 0) //构建Web菜单
	{		
		m_WebMenu2.CreatePopupMenu();
		UINT StartID = ID_WEB2_MENU_1;
		for(int i = 0;i < g_nWebPageNum2;i++)
		{
			if(g_strWebPageURL2[i][0]==0)
				m_WebMenu2.AppendMenu( MF_SEPARATOR,StartID+i,g_strWebPageStr2[i]);
			else
				m_WebMenu2.AppendMenu( MF_STRING,StartID+i,g_strWebPageStr2[i]);
		}		
	}
	*/
	//插入菜单
	if(g_bHasCaption)
	{
		int nWebPos = 7;
		m_tmpMenu.LoadMenu(g_Scheme_King.MainMenuID);
		if(FALSE&&g_bHasDS)
			CreateDSMenu();
		/*
		if(g_nWebPageNum > 0)
			m_tmpMenu.InsertMenu(nWebPos,MF_BYPOSITION|MF_POPUP,(UINT)(m_WebMenu.m_hMenu),g_strMainWebTitle);
		if(g_nWebPageNum2 > 0)
			m_tmpMenu.InsertMenu(nWebPos+1,MF_BYPOSITION|MF_POPUP,(UINT)(m_WebMenu2.m_hMenu),g_strMainWebTitle2);
		*/
	}
	else
	{
		m_tmpMenu.LoadMenu(IDR_MAINFRAME);
		int nPos=5;
		if(FALSE&&g_bHasDS)
		{
			CreateDSMenu();
			nPos++;
		}
	#if defined(OEM_NEWJY) || defined(LEVEL2)
		if(CanAdvLevel2() || g_GC.bHasGPGrade)
		{
			m_AdvFuncMenu.CreatePopupMenu();
			if(g_GC.bHasGPGrade)
			{
				m_AdvFuncMenu.AppendMenu( MF_STRING,ID_GPGRADE_DLG,_F("机构评测和诊断\t18"));
				m_AdvFuncMenu.AppendMenu( MF_STRING,ID_TDXREPORT,_F("研究报告平台\t19"));
				m_AdvFuncMenu.AppendMenu(MF_SEPARATOR);
			}
			if(CanAdvLevel2())
			{
				m_AdvFuncMenu.AppendMenu( MF_STRING,ID_KQS,_F("权证盘口王"));
				m_AdvFuncMenu.AppendMenu( MF_STRING,ID_KQSCFG,_F("权证盘口王设置"));
				if(g_GC.bHasQzSDS)
				{
					m_AdvFuncMenu.AppendMenu(MF_SEPARATOR);
					m_AdvFuncMenu.AppendMenu( MF_STRING,IDC_QZSDS,_F("权证闪电手\t25"));
				}
			}
		#ifndef CITY10
			m_AdvFuncMenu.AppendMenu(MF_SEPARATOR);
			m_AdvFuncMenu.AppendMenu( MF_STRING,IDC_USERCENTER,_F("会员服务中心"));
		#endif
			m_tmpMenu.InsertMenu(nPos,MF_BYPOSITION|MF_POPUP,(UINT)(m_AdvFuncMenu.m_hMenu),_F("深度(&G)"));
			nPos++;
		}
	#endif
		/*
		if(g_nWebPageNum > 0)
			m_tmpMenu.InsertMenu(nPos,MF_BYPOSITION|MF_POPUP,(UINT)(m_WebMenu.m_hMenu),g_strMainWebTitle);
		if(g_nWebPageNum2 > 0)
			m_tmpMenu.InsertMenu(nPos+1,MF_BYPOSITION|MF_POPUP,(UINT)(m_WebMenu2.m_hMenu),g_strMainWebTitle2);
		*/
	}
	m_hMenuShared = m_tmpMenu.m_hMenu;
	return CNewMDIChildWnd::OnCreateClient(lpcs, pContext);
}
