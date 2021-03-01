#if !defined(AFX_CLIENTSOCKET_H__5C36FE61_0C72_11D4_A4CE_00E04C66B9E1__INCLUDED_)
#define AFX_CLIENTSOCKET_H__5C36FE61_0C72_11D4_A4CE_00E04C66B9E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClientSocket.h : header file

#include "ReceiveThread.h"

class CHQComm;
class CClientSocket : public CAsyncSocket
{
public:
	CClientSocket();
	virtual ~CClientSocket();

	CReceiveThread *m_pThread;
	CHQComm		   *m_pHQComm;
	int				m_nCommType;

	void	InitIt(CHQComm *pComm,int commtype);
	BOOL	m_bStartAsyn;
	BOOL	StartThread();
// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientSocket)
	public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CClientSocket)
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTSOCKET_H__5C36FE61_0C72_11D4_A4CE_00E04C66B9E1__INCLUDED_)
