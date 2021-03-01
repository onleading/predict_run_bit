// CUShowMulInfoCtrlWnd.cpp: implementation of the CUShowMulInfoCtrlWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tdxw.h"
#include "UShowMulInfoCtrlWnd.h"
#include "UShowMulInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUShowMulInfoCtrlWnd::CUShowMulInfoCtrlWnd(UShowMulInfo *pUShowMulInfo)
{
	m_pUShowMulInfo = pUShowMulInfo;
	m_bLayoutFlag = FALSE;
	m_bQuitting = FALSE;
}

CUShowMulInfoCtrlWnd::~CUShowMulInfoCtrlWnd()
{

}

BEGIN_MESSAGE_MAP(CUShowMulInfoCtrlWnd, CWnd)
	ON_MESSAGE(UM_GETDATA_ACK,OnGetDataAck)
	//{{AFX_MSG_MAP(CTxtView)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////
int CUShowMulInfoCtrlWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pUShowMulInfo->OnCreate();
	m_bLayoutFlag = TRUE;
	return 0;
}

void CUShowMulInfoCtrlWnd::OnDestroy() 
{
	m_bQuitting = TRUE;
	CWnd::OnDestroy();	
}

void CUShowMulInfoCtrlWnd::Resize()
{
	if(!m_bLayoutFlag) return;
	m_pUShowMulInfo->Resize();
	Invalidate(TRUE);
}

void CUShowMulInfoCtrlWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	Resize();
}

BOOL CUShowMulInfoCtrlWnd::OnEraseBkgnd(CDC* pDC) 
{
	return m_pUShowMulInfo->OnEraseBkgnd(pDC);
}

void CUShowMulInfoCtrlWnd::OnGetDataAck(WPARAM wParam,LPARAM lParam)
{
	m_pUShowMulInfo->OnGetDataAck(wParam, lParam);
}
