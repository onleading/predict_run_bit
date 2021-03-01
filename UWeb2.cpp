// UWeb2.cpp: implementation of the UWeb2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tdxw.h"
#include "HtmlCtrl.h"
#include "UWeb2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UWeb2::UWeb2(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_bCreated = FALSE;
	m_hStatic = 0;
	m_pExplorer = NULL;
	m_URL = "";
	m_nLastCurGP = -1;
	m_bGPRelation = FALSE;

	m_nRefreshFlag = 0;
	m_nRefreshEscape = 0;
}

UWeb2::~UWeb2()
{
	m_pExplorer->DestroyWindow();
	TDEL(m_pExplorer);
}

void UWeb2::Navigate()
{
	if(!m_pExplorer) return;
	try
	{
		CString tmpURL=m_URL;
		if(m_bGPRelation)
			TransferToRealURL(tmpURL,m_pStkInfo);
		else
			TransferToRealURL(tmpURL,NULL);
		m_pExplorer->Navigate2(tmpURL);
	}
	catch(...)
	{
	}
}

void UWeb2::SetURL(CString tmpURL,BOOL bAtOnceRun,BOOL bMustGPRelation)
{
	m_URL = tmpURL;
	m_nLastCurGP = -1;
	m_bGPRelation = FALSE;
	if(m_URL.Find("xxxxxx")>=0)
		m_bGPRelation=TRUE;
	if(bMustGPRelation)
		m_bGPRelation=TRUE;
	if(bAtOnceRun)
		Navigate();
}

CString UWeb2::GetURL()
{
	return m_URL;
}

#define WEB_HEAD_HEIGHT 0 
int  UWeb2::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_nCurGP==-1) //如果Unit为空，则另行处理
	{
		if(message==WM_PAINT)
		{
			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
			DESTORY_TMPDC
			return 1;
		}
		return UBase::ProcessMsg(message,wParam,lParam);
	}
	switch(message)
	{
	case WM_SIZE:
		UBase::ProcessMsg(WM_SIZE,wParam,lParam);
		if(!m_bCreated)
		{
			try
			{
				m_hStatic = ::CreateWindow("STATIC","",WS_VISIBLE|WS_CHILD|WS_BORDER,0,0,0,0,
				m_pView->GetSafeHwnd(),NULL,AfxGetInstanceHandle(),NULL);
				m_pExplorer = new CHtmlCtrl;
				m_pExplorer->CreateFromWnd(m_hStatic, ID_HTMLSTATIC, m_pView);

				if(m_URL.GetLength()>0)
					Navigate();
			}
			catch (...) 
			{
				m_pExplorer=NULL;
			}
			m_btnOpen.Create(NULL,WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),m_pView,ID_OPENURL);
			m_btnOpen.SetTransparent(FALSE);
			if(m_nInCom2)
				m_btnOpen.SetIcon(IDI_FIND,IDI_FIND);
			else
				m_btnOpen.SetIcon(IDI_QUIT,IDI_QUIT);				
			m_btnOpen.DrawBorder(FALSE);
			m_bCreated = TRUE;
		}
		if(m_pExplorer && m_pExplorer->m_hWnd && ::IsWindow(m_pExplorer->m_hWnd))
		{
			if(m_nInCom2)
				m_btnOpen.MoveWindow(left,top,WEB_HEAD_HEIGHT,WEB_HEAD_HEIGHT);
			else
				m_btnOpen.MoveWindow(right-WEB_HEAD_HEIGHT,top,WEB_HEAD_HEIGHT,WEB_HEAD_HEIGHT);
			m_pExplorer->SetWindowPos(NULL,left,top+WEB_HEAD_HEIGHT,Width(),Height()-WEB_HEAD_HEIGHT,SWP_SHOWWINDOW);
		}
		return 1;		
	case WM_PAINT:
		{
			if(wParam == 1)
			{				
				if(m_bGPRelation && m_nLastCurGP!=m_nCurGP)
				{
					m_nLastCurGP = m_nCurGP;
					Navigate();
				}
			}
			CDC *pDC = m_pView->GetDC();
			CRect rc;
			rc.CopyRect(this);
			rc.bottom=rc.top+WEB_HEAD_HEIGHT;
			if(m_nInCom2)
				rc.left+=WEB_HEAD_HEIGHT;
			else
				rc.right-=WEB_HEAD_HEIGHT;
			g_d.Bar(pDC,&rc,OVERLAP_BRUSH);
			m_pView->ReleaseDC(pDC);
		}
		return 1;
	case WM_TIMER:
		{
			m_nRefreshFlag++;
			if(m_nRefreshEscape>0&&m_nRefreshFlag>=m_nRefreshEscape)
			{
				m_pExplorer->Refresh();
				m_nRefreshFlag = 0;
			}
		}
		break;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}