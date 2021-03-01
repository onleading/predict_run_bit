#ifndef _HQCOMM_H
#define _HQCOMM_H

#include "ClientSocket.h"
#include "ReqThread.h"

#define MAX_HQ_PILE_NUM			8
#define MAX_DIRECT_PILE_NUM		5

#define MAX_TIMECNT				50

class CHQComm
{
public:
	CHQComm(int commtype=0);
	virtual ~CHQComm();
	void	SetServerProperties( LPCTSTR sServerHostName, UINT nPort);
	CString GetServerHostName();
	UINT	GetPort();
	CString GetErrorStr();

	BOOL	Disconnect();
	BOOL	Connect();
	BOOL	StartThread();
	BOOL	AfterConnectProxy();
	BOOL	ConnectTdxProxy();

	int		SendIt(void *lpBuf, int nBufLen);
	int		SendOneBuf(char *buf,int len);
	int		SendBuf();	

	void	SetSendTop(int value);
	int		GetSendTop();

	void	SetEncryptKey(char *KeyStr);
	void	DecryptBuf(char *buf,int nBufLen);

	void	SetSendTimeCnt(int nTimeCnt);
	DWORD	GetSendTimeCnt();
public:
	CReqThread	*		m_pThread;
	int					m_nCommType;		//0:�������� 1:����������

	BOOL				m_bBuffEven;		//�Ƿ���һ���շ����
	BOOL				m_bDirectEven;		//ֱ�ӷ����Ƿ���һ���շ����
	int					m_bDirectSendNum;	//�Ѿ�ֱ�ӷ��͵ĸ���
	CCriticalSection	m_EvenSection;		//

	REQBUFFER	m_HQSendBuf;
	short		m_HQBufFlag;				//0:�գ�1:�ѷ��ͣ�2:��
	CString		m_sError;
private:
	void		SafeCloseSocket();
private:
	UINT		m_nPort;
	CString		m_sHostName;
	BOOL		m_bConnected;
	CBlowfish*	m_pBlowfish;

	DWORD		m_nTimeCnt;

	CClientSocket		ClientSock;
	REQBUFFER			m_SendBuf[MAX_SENDBUF_NUM];
	short				m_SendTop;
	CCriticalSection	m_TopSection;
};

#define HQ_NLB			0
#define L2HQ_NLB		1
#define DSHQ_NLB		2
#define VIPHQ_NLB		3
#define SSDSHQ_NLB		4

class CHQComm_NLB
{
public:
	//�븺�ؾ����йصľ�̬��Ա
	static BOOL	 NLB_ReqInfo(const char *hostname,UINT hostport,struct NLBInfo *pNLBInfo,int nIndex,BOOL bDS=FALSE);
	static int	 NLB_GetMinweight(int flag);
	static int   ReceiveNLBData(SOCKET m_sock,char *pReceive);
	static void	 CloseThreadSocket(int nIndex);
	
	static int   ReceiveWTNLBData(SOCKET m_sock,char *pReceive);
	static BOOL	 NLB_WTReqInfo(const char *hostname,UINT hostport,struct NLBInfo *pNLBInfo,int nIndex);
	
	static char* NLB_GetLastErr();
	static char  NLB_ErrStr[100];
	static SOCKET NLB_allsock[150];


};

#endif
