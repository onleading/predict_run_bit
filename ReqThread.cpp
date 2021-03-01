// ReqThread.cpp : implementation file
//

#include "stdafx.h"
#include "ReqThread.h"
#include "HQComm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReqThread

IMPLEMENT_DYNCREATE(CReqThread, CWinThread)

CReqThread::CReqThread()
{
	m_bAutoDelete = FALSE;
	m_hEndEvent = m_hStartEvent = NULL;
	m_pHQComm = NULL;
}

CReqThread::~CReqThread()
{
	
}

void CReqThread::InitIt(CHQComm *pComm)
{
	m_pHQComm = pComm;
}


BOOL CReqThread::InitInstance()
{
	m_hStartEvent  = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEndEvent    = CreateEvent(NULL, TRUE, FALSE, NULL);
	if ( !m_hEndEvent || !m_hStartEvent )
		return FALSE;
	return TRUE;
}

int CReqThread::ExitInstance()
{
	if ( m_hEndEvent )	CloseHandle( m_hEndEvent );
	if ( m_hStartEvent) CloseHandle( m_hStartEvent);
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CReqThread, CWinThread)
	//{{AFX_MSG_MAP(CReqThread)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReqThread message handlers

int CReqThread::Run() 
{
	while ( ::WaitForSingleObject(m_hEndEvent,0)!= WAIT_OBJECT_0 )
	{
		::WaitForSingleObject(m_hStartEvent,INFINITE);
		ResetEvent ( m_hStartEvent );
		if ( ::WaitForSingleObject(m_hEndEvent,0) == WAIT_OBJECT_0 )
			break;
		if(m_pHQComm)
			m_pHQComm->SendBuf();
	}
	return 0;
}
