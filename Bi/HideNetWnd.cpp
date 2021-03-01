// HideNetWnd.cpp : implementation file
//

#include "stdafx.h"
#include "IDefine.h"
#include "HostAHq.h"
#include "HideNetWndClass.h"
#include "HideNetWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHideNetWnd

IMPLEMENT_DYNCREATE(CHideNetWnd, CWinThread)

CHideNetWnd::CHideNetWnd()
{
	m_dataSource = -1;
	m_pHq = NULL;
	m_StartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_iNo = 0;
}

CHideNetWnd::~CHideNetWnd()
{
	CloseHandle(m_StartEvent);
}

BOOL CHideNetWnd::InitInstance()
{
	m_pHideNetWnd = new CHideNetWndClass(this);
	m_pHideNetWnd->CreateEx(0,AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW),"HIDENET",WS_OVERLAPPEDWINDOW|WS_VISIBLE,CRect(0,0,0,0),NULL,0);
	m_pHideNetWnd->ShowWindow(SW_HIDE);

	return StartDataEngine();
}

BOOL CHideNetWnd::StartDataEngine()
{
	BOOL bDSReady = FALSE;
	char strError[256]={0};
	switch(m_dataSource)
	{
	case DATA_HOSTA:
		if(!m_pHq) m_pHq = new CHostAHq;
		if(!m_pHq) break;
		m_pHq->NetRegister(m_pHideNetWnd->GetSafeHwnd(), 0, 0, m_iNo);
		m_pHq->InitHq(strError);
		bDSReady = TRUE;
		break;
	default:
		break;
	}
	SetEvent(m_StartEvent);
	
	return TRUE;
}

BOOL	CHideNetWnd::StartHqThread(int nMaxInitSec)
{
	MSG msg={0}; DWORD nStartTime = GetTickCount();
	ResumeThread();
	while( WaitForSingleObject(m_StartEvent,0)!= WAIT_OBJECT_0 )
	{
		if(GetTickCount()-nStartTime>nMaxInitSec*1000)
			return FALSE;
		while( ::PeekMessage(&msg,NULL,0,0,PM_REMOVE) )
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	return TRUE;
}

//×¢²áÐÐÇéÔ´
void CHideNetWnd::RegisterDataEngin(int iNo,UINT DataSource, char *strIPAddr, char *strPort, char *strMarket)
{
	m_iNo = iNo;
	m_dataSource = DataSource;
	m_strIPAddr = strIPAddr;
	m_strPort = strPort;
	m_strMarket = strMarket;
}

int CHideNetWnd::ExitInstance()
{
	return CWinThread::ExitInstance();
}

void CHideNetWnd::FinalKill()
{
	if(m_pHq) m_pHq->UnInitHq();
	DELETE_PTR(m_pHq);
	if(m_pHideNetWnd)
	{
		m_pHideNetWnd->KillTimer(m_dataSource);
		m_pHideNetWnd->DestroyWindow();
		delete m_pHideNetWnd; m_pHideNetWnd = NULL;
	}
}

BEGIN_MESSAGE_MAP(CHideNetWnd, CWinThread)
	//{{AFX_MSG_MAP(CHideNetWnd)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_THREAD_MESSAGE(UM_KILLTHREAD,OnKillThread)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHideNetWnd message handlers
LRESULT CHideNetWnd::OnKillThread(WPARAM wParam,LPARAM lParam)
{
	FinalKill();
	::PostQuitMessage(0);
	return 0;
}