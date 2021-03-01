// ClientSocket.cpp : implementation file

#include "stdafx.h"
#include "ClientSocket.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientSocket

CClientSocket::CClientSocket()
{
	m_bStartAsyn = FALSE;
	m_pThread = NULL;

	m_pHQComm = NULL;
	m_nCommType = 0;
}

CClientSocket::~CClientSocket()
{
}

void CClientSocket::InitIt(CHQComm *pComm,int commtype)
{
	m_pHQComm = pComm;
	m_nCommType = commtype;
}

BOOL CClientSocket::StartThread()
{
	TDEL(m_pThread);
	m_pThread = new CReceiveThread();		//开始创建一个Proc线程
	if (!m_pThread)
		return FALSE;
	BOOL bRet = FALSE;
	if(m_nCommType==0)
		bRet = m_pThread->InitIt(this,m_pHQComm,m_nCommType,g_pReceive,ANSBUFFER_LEN,&g_AnsHeader);
	else
		bRet = m_pThread->InitIt(this,m_pHQComm,m_nCommType,g_pReceive3,ANSBUFFER_LEN,&g_AnsHeader3);
	if(!bRet)	//设置线程的参数
	{
		TDEL(m_pThread);
		return FALSE;
	}
	if (!m_pThread->CreateThread())		//执行该线程
	{
		TDEL(m_pThread);
		return FALSE;
	}
	return TRUE;
}

// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CClientSocket, CAsyncSocket)
	//{{AFX_MSG_MAP(CClientSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////

void CClientSocket::OnReceive(int nErrorCode) 
{
 	if(m_bStartAsyn && m_pThread)
		SetEvent(m_pThread->m_hStartEvent);	
	CAsyncSocket::OnReceive(nErrorCode);
}

void CClientSocket::OnClose(int nErrorCode) 
{
	CAsyncSocket::OnClose(nErrorCode);
	BOOL bCurDSOnLine = DSOnLine;
	m_pHQComm->Disconnect();
	AfterSocketClose(m_nCommType);
	if(m_nCommType==0 && bCurDSOnLine)
		g_nDSAutoConnectFlag = AUTOCONNECT_RECONNECT;
}

void CClientSocket::OnConnect(int nErrorCode) 
{
	CWnd *pTmpWnd = g_pConnectWnd;
	//如果是行情的自动重连状态,消息发向主窗口
	if(m_nCommType==0 && g_nAutoConnectFlag==AUTOCONNECT_PENDING)		
		pTmpWnd = g_pMainWin;
	else if(m_nCommType==1 && g_nDSAutoConnectFlag==AUTOCONNECT_PENDING)
		pTmpWnd = g_pMainWin;

	if(pTmpWnd && m_bStartAsyn)
	{
		if(nErrorCode==0)	pTmpWnd->PostMessage(UM_CONNECT_SUCCESS,1,m_nCommType);
		else				pTmpWnd->PostMessage(UM_CONNECT_SUCCESS,0,m_nCommType);
	}
	else if(pTmpWnd && g_ProxyInfo.m_bProxy)
	{
		if(nErrorCode==0)
		{
			if(g_ProxyInfo.m_nSockType==0)		//SOCK4代理
				pTmpWnd->PostMessage(UM_CONNECT_SUCCESS,11,m_nCommType);
			else if(g_ProxyInfo.m_nSockType==1)	//SOCK5代理
				pTmpWnd->PostMessage(UM_CONNECT_SUCCESS,12,m_nCommType);
			else								//HTTP代理
				pTmpWnd->PostMessage(UM_CONNECT_SUCCESS,13,m_nCommType);
		}
		else	//连接失败
			pTmpWnd->PostMessage(UM_CONNECT_SUCCESS,10,m_nCommType);
	}
	CAsyncSocket::OnConnect(nErrorCode);
}
