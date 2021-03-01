// HqSrcMgn.cpp: implementation of the CHqSrcMgn class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IDefine.h"
#include "HideNetWnd.h"
#include "HqSrcMgn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHqSrcMgn::CHqSrcMgn()
{
	m_nHqNetThreadNum = 0;
	memset(m_ppNetThread, 0, MAX_HQ_COM*sizeof(CHideNetWnd*));
}

CHqSrcMgn::~CHqSrcMgn()
{
	memset(m_ppNetThread, 0, MAX_HQ_COM*sizeof(CHideNetWnd*));
	m_nHqNetThreadNum = 0;
}
//////////////////////////////////////////////////////////////////////////
BOOL	CHqSrcMgn::RegisterOneHqSource(LPSTR strError, UINT DataType,  char *strIPAddr, char *strPort, char *strMarket)
{
	if(m_nHqNetThreadNum>=MAX_HQ_COM-1)
	{
		lstrcpy(strError,"建立的行情源数据线程太多,无法继续!");
		return FALSE;
	}
	m_ppNetThread[m_nHqNetThreadNum] = 
		(CHideNetWnd*)AfxBeginThread(RUNTIME_CLASS(CHideNetWnd),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
	if(!m_ppNetThread[m_nHqNetThreadNum])
	{
		lstrcpy(strError,"由于系统问题线程启动失败!");
		return FALSE;
	}
	m_ppNetThread[m_nHqNetThreadNum]->RegisterDataEngin(m_nHqNetThreadNum,DataType,strIPAddr,strPort,strMarket);
	++m_nHqNetThreadNum;
	return TRUE;
}

extern void		LoadTestQueue();
//顺生
void	CHqSrcMgn::StartHqThread()
{
	LoadTestQueue();
	for(int i=0;i<m_nHqNetThreadNum;i++)
		m_ppNetThread[i]->StartHqThread(3);
}

BOOL	CHqSrcMgn::KillHqThread(int nThread, int nMaxKillSec)
{
	CHideNetWnd* pThread = m_ppNetThread[nThread];
	if(!pThread) return TRUE;
	MSG msg={0}; DWORD nStartTime = GetTickCount();
	pThread->PostThreadMessage(UM_KILLTHREAD,0,0);

	DWORD dwExitCode = 0;
	while ( GetExitCodeThread(pThread->m_hThread,&dwExitCode) || TRUE )
	{
		if(dwExitCode!=STILL_ACTIVE) 
		{
			pThread = NULL;
			return TRUE;
		}
		Sleep(10);
		if(GetTickCount()-nStartTime>nMaxKillSec*1000)
		{
			TerminateThread(pThread->m_hThread, 1);
			pThread = NULL;
			return FALSE;
		}
		PeekMsg();
		dwExitCode = 0;
	}
	pThread = NULL;
	return TRUE;
}

//逆亡,保证资源清理顺序
void	CHqSrcMgn::KillAllHqThread()
{	
	for(int i=m_nHqNetThreadNum-1;i>=0;i--)
	{
		KillHqThread(i, 2);
		--m_nHqNetThreadNum;
	}
}

void	CHqSrcMgn::SuspendAll()
{
	for(int i=0;i<m_nHqNetThreadNum;i++)	m_ppNetThread[i]->SuspendThread();
}

void	CHqSrcMgn::ResumeAll()
{
	for(int i=0;i<m_nHqNetThreadNum;i++)	m_ppNetThread[i]->ResumeThread();
}
