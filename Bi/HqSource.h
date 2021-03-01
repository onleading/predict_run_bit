// HqSource.h: interface for the CHqSource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HQSOURCE_H__CFAABA7A_2B90_4221_8341_E451B8C6DEE3__INCLUDED_)
#define AFX_HQSOURCE_H__CFAABA7A_2B90_4221_8341_E451B8C6DEE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHqSource  
{
public:
	CHqSource();
	virtual ~CHqSource();

	virtual short	Connect2Host(LPSTR strError,const char *hostname,UINT hostport);
	virtual	void	NetRegister(HWND hMsgWnd, UINT MsgID, long lEvent, int iNo=0);			//接收变异步
	virtual BOOL	InitHq(LPSTR strError)=0;									//此时为同步								//此时要保证是异步,如返回FALSE则重新Init
	virtual void	UnInitHq(void)=0;

public:
	DWORD	m_nSourceType;
	SOCKET	m_hSocket;
	BOOL	m_bConnected;

	long	m_lTestAlive;
	int		m_iNo;
};

#endif // !defined(AFX_HQSOURCE_H__CFAABA7A_2B90_4221_8341_E451B8C6DEE3__INCLUDED_)
