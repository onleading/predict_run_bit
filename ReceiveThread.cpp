// ReceiveThread.cpp : implementation file

#include "stdafx.h"
#include "ReceiveThread.h"

#include "ClientSocket.h"

#include "HQComm.h"
#include "zlib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReceiveThread

IMPLEMENT_DYNCREATE(CReceiveThread, CWinThread)

CReceiveThread::CReceiveThread()
{
	m_hStartEvent = m_hEndEvent = NULL;
	m_bAutoDelete = FALSE;
	m_pNow  = 0;

	m_pHQComm = NULL;
	m_pReceive = NULL;
	m_nMaxRecLen = 0;
}

CReceiveThread::~CReceiveThread()
{

}

BOOL CReceiveThread::InitIt(CClientSocket *pSocket,CHQComm *pComm,int nCommType,char *pReceive,int nMaxRecLen,ANSHEADER *pAnsHeader)
{
	m_pSock			= pSocket;
	m_pHQComm		= pComm;
	m_nCommType		= nCommType;

	m_pReceive		= pReceive;
	m_nMaxRecLen	= nMaxRecLen;
	m_pAnsHeader	= pAnsHeader;
	//创建两个事件
	m_hStartEvent  = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEndEvent    = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!m_hStartEvent||!m_hEndEvent)
		return FALSE;
	return TRUE;
}

BOOL CReceiveThread::InitInstance()
{
	return TRUE;
}

int CReceiveThread::ExitInstance()
{
	if (m_hStartEvent) CloseHandle(m_hStartEvent);
	if (m_hEndEvent)   CloseHandle(m_hEndEvent);

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CReceiveThread, CWinThread)
	//{{AFX_MSG_MAP(CReceiveThread)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

int CReceiveThread::Run()
{
	long		Sepec,Sepec1;
	ANSHEADER	Header,*pHeader;
	int			AnsHeaderLen = sizeof(ANSHEADER);
	int			PacketLen;
	short		nBytesRead;
	char		Buffer[ANSBUFFER_LEN+20];
	char		TestBuff[8]={0};
	int			TestBuffPos = 0;
	while( ::WaitForSingleObject(m_hEndEvent,0) != WAIT_OBJECT_0 )
	{
		::WaitForSingleObject(m_hStartEvent, INFINITE);
		ResetEvent(m_hStartEvent);
		if( ::WaitForSingleObject(m_hEndEvent,0) == WAIT_OBJECT_0 )
			break;
		memset(Buffer,0,ANSBUFFER_LEN+20);
		nBytesRead = m_pSock->Receive(Buffer,ANSBUFFER_LEN+20);
		InterlockedExchangeAdd(&g_nNetInCount, nBytesRead);
		
		for(int j = 0;j < nBytesRead;j++)
		{
			if(TestBuffPos < 7)
			{
				TestBuff[TestBuffPos++] = Buffer[j];
				m_pBuff[m_pNow++]=Buffer[j];
				continue;
			}
			if(TestBuffPos != 7)
				memcpy(TestBuff,TestBuff+1,7);
			else
				TestBuffPos++;
			TestBuff[7] = Buffer[j];
			memcpy(&Sepec,TestBuff,4);
			memcpy(&Sepec1,TestBuff+4,4);
			if(Sepec == SEPARATOR_NUM)
			{
				if(Sepec1!=SEPARATOR_NUM)
				{
					if(m_pNow > AnsHeaderLen+7)
					{
						memcpy(&Header,m_pBuff,AnsHeaderLen);
						PacketLen = Header.PacketLen;
						if(PacketLen == m_pNow-7-AnsHeaderLen)
						{
							*m_pAnsHeader = Header;
							memset(m_pReceive,0,m_nMaxRecLen*sizeof(char));
							memcpy(m_pReceive,m_pBuff+AnsHeaderLen,PacketLen);

							//解压
							if (Header.Info.Compressed)
							{
								DWORD nRawLen = Header.RawLen;
								char  * pRawBuf = new char[nRawLen];
								if (!pRawBuf)	return -1;
								if(!uncompress((BYTE *)pRawBuf,&nRawLen,(BYTE *)m_pReceive,PacketLen))
								{
									memcpy(m_pReceive,pRawBuf,nRawLen);
									Header.RawLen = nRawLen;
								}
								delete pRawBuf;
							}
								
							if(g_pMainWin)
							{
								short reqtype = GetReqType(m_pAnsHeader->AssisID);
								if(reqtype == BUFFER_REQTYPE)
								{
									m_pHQComm->m_EvenSection.Lock();
									m_pHQComm->m_bBuffEven = TRUE;
									m_pHQComm->m_EvenSection.Unlock();
									if(m_pHQComm && m_pHQComm->m_pThread)
										SetEvent(m_pHQComm->m_pThread->m_hStartEvent);
								}
								else
								{
									m_pHQComm->m_bDirectEven = TRUE;
									m_pHQComm->m_bDirectSendNum -= 1;
								}
								m_pHQComm->SetSendTimeCnt(0);
								if(g_pDSConnectWnd && m_nCommType==1)	 
									g_pDSConnectWnd->SendMessage(UM_GETDATA_ACK,m_nCommType);
								else
									g_pMainWin->SendMessage(UM_GETDATA_ACK,m_nCommType);
							}
						}
					}
					memcpy(m_pBuff,TestBuff,8);
					m_pNow = 8;
				}
				else
				{
					pHeader = (ANSHEADER *)m_pBuff;
					pHeader->PacketLen -= 4;
					TestBuffPos = 0;
					m_pNow-=3;
				}
			}
			else
				m_pBuff[m_pNow++]=Buffer[j];
		}
		if(m_pNow > AnsHeaderLen)
		{
			memcpy(&Header,m_pBuff,AnsHeaderLen);
			PacketLen = Header.PacketLen;
			if(PacketLen == m_pNow-AnsHeaderLen)
			{
				*m_pAnsHeader = Header;
				memset(m_pReceive,0,m_nMaxRecLen*sizeof(char));
				memcpy(m_pReceive,m_pBuff+AnsHeaderLen,PacketLen);

				//解压
				if (Header.Info.Compressed)
				{
					DWORD nRawLen = Header.RawLen;
					char  * pRawBuf = new char[nRawLen];
					if (!pRawBuf)	return -1;
					if(!uncompress((BYTE *)pRawBuf,&nRawLen,(BYTE *)m_pReceive,PacketLen))
					{
						memcpy(m_pReceive,pRawBuf,nRawLen);
						Header.RawLen = nRawLen;
					}
					delete pRawBuf;
				}
				if(g_pMainWin)
				{
					short reqtype = GetReqType(m_pAnsHeader->AssisID);
					if(reqtype == BUFFER_REQTYPE)
					{
						m_pHQComm->m_EvenSection.Lock();
						m_pHQComm->m_bBuffEven = TRUE;
						m_pHQComm->m_EvenSection.Unlock();
						if(m_pHQComm && m_pHQComm->m_pThread)
							SetEvent(m_pHQComm->m_pThread->m_hStartEvent);
					}
					else
					{
						m_pHQComm->m_bDirectEven = TRUE;
						m_pHQComm->m_bDirectSendNum -= 1;
					}
					m_pHQComm->SetSendTimeCnt(0);

					DWORD msghead = 0xF8F8F8F8;
					if(memcmp(m_pReceive,&msghead,sizeof(DWORD))==0)
						g_pMainWin->SendMessage(UM_POPMSG,0,(LPARAM)(m_pReceive+sizeof(DWORD)));
					else 
					{
						if(g_pDSConnectWnd && m_nCommType==1)
							g_pDSConnectWnd->SendMessage(UM_GETDATA_ACK,m_nCommType);
						else
							g_pMainWin->SendMessage(UM_GETDATA_ACK,m_nCommType);
					}
				}
				m_pNow		= 0;
				TestBuffPos = 0;
			}
		}
	}
	return 0;
}
