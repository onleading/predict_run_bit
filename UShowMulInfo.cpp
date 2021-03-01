// UShowMulInfo.cpp: implementation of the UShowMulInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tdxw.h"
#include "UShowMulInfo.h"
#include "UShowMulInfoCtrl.h"
#include "UShowMulInfoCtrlWnd.h"
#include "HtmlCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UShowMulInfo::UShowMulInfo(LPCOMINFO pComInfo, UShowMulInfoCtrl *pCtrl)
{
	m_pCtrl = pCtrl;
	m_pCtrlWnd = NULL;
	memcpy(&m_ComInfo, pComInfo, sizeof(m_ComInfo));

	m_bQuitting = FALSE;
	m_bLayoutFlag = FALSE;
}

UShowMulInfo::~UShowMulInfo()
{
	if(m_pExplorer)
	{
		m_pExplorer->DestroyWindow();
		TDEL(m_pExplorer);
	}

	if(m_pCtrlWnd)
	{
		m_pCtrlWnd->DestroyWindow();
		delete m_pCtrlWnd;
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL	UShowMulInfo::InitShow(HWND hVessel)
{
	m_hVessel = hVessel;
	if(!m_pCtrlWnd||!m_pCtrlWnd->m_hWnd)
	{
		delete m_pCtrlWnd;
		m_pCtrlWnd = new CUShowMulInfoCtrlWnd(this);
		m_pCtrlWnd->Create(NULL,"hideshowmulinfownd",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),CWnd::FromHandle(m_hVessel),IDC_HIDENETWND);
		if(!m_pCtrlWnd||!m_pCtrlWnd->m_hWnd) return FALSE;
	}

	return TRUE;
}

void	UShowMulInfo::ArrangeWnd(CRect &rc)
{
	if(!m_pCtrlWnd) return;

	if(m_pCtrlWnd&&m_pCtrlWnd->m_hWnd)
		m_pCtrlWnd->MoveWindow(&rc, FALSE);
}

void UShowMulInfo::OnGetDataAck(WPARAM wParam,LPARAM lParam)
{

}
//////////////////////////////////////////////////////////////////////////
void UShowMulInfo::Resize()
{
	CRect rc;
	m_pCtrlWnd->GetClientRect(&rc);

	m_pExplorer->SetWindowPos(NULL,0,0,rc.Width(),rc.Height()-20,SWP_SHOWWINDOW);
}

BOOL UShowMulInfo::OnEraseBkgnd(CDC* pDC) 
{
	/*
	CRect rect;
	m_pCtrlWnd->GetClientRect(&rect);
	pDC->FillSolidRect (&rect,TxtColor.TxtBackColor);
	*/
	return TRUE;
}

void UShowMulInfo::OnCreate()
{
	try
	{
		m_hStatic = ::CreateWindow("STATIC","",WS_VISIBLE|WS_CHILD|WS_BORDER,0,0,0,0,
			m_pCtrlWnd->GetSafeHwnd(),NULL,AfxGetInstanceHandle(),NULL);
		m_pExplorer = new CHtmlCtrl;
		m_pExplorer->CreateFromWnd(m_hStatic, ID_HTMLSTATIC, m_pCtrlWnd);
	//	m_pExplorer->SetOuterUnit(this);
		m_pExplorer->Navigate("http://www.sina.com.cn");
		
	}
	catch (...) 
	{
		m_pExplorer=NULL;
	}
}
