#if !defined(AFX_RECEIVETHREAD_H__5C36FE62_0C72_11D4_A4CE_00E04C66B9E1__INCLUDED_)
#define AFX_RECEIVETHREAD_H__5C36FE62_0C72_11D4_A4CE_00E04C66B9E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CClientSocket;
class CHQComm;
class CReceiveThread : public CWinThread
{
	DECLARE_DYNCREATE(CReceiveThread)
public:
	CReceiveThread();
	virtual ~CReceiveThread();

	BOOL		InitIt(CClientSocket *pSocket,CHQComm *pComm,int nCommType,char *pReceive,int nMaxRecLen,ANSHEADER *pAnsHeader);

	CClientSocket  *m_pSock;
	CHQComm		   *m_pHQComm;
	int				m_nCommType;
	char		   *m_pReceive;
	int				m_nMaxRecLen;
	ANSHEADER	   *m_pAnsHeader;

	HANDLE			m_hStartEvent,m_hEndEvent;
	char			m_pBuff[ANSBUFFER_LEN+20];
	int				m_pNow;
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReceiveThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CReceiveThread)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECEIVETHREAD_H__5C36FE62_0C72_11D4_A4CE_00E04C66B9E1__INCLUDED_)
