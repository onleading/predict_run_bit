#if !defined(AFX_REQTHREAD_H__15D97F8C_4AC0_4011_8DBB_C867B1D86255__INCLUDED_)
#define AFX_REQTHREAD_H__15D97F8C_4AC0_4011_8DBB_C867B1D86255__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReqThread.h : header file
//

class CHQComm;
/////////////////////////////////////////////////////////////////////////////
// CReqThread thread

class CReqThread : public CWinThread
{
	DECLARE_DYNCREATE(CReqThread)
public:
	CReqThread();           // protected constructor used by dynamic creation
	virtual ~CReqThread();
// Attributes
public:
	HANDLE			m_hStartEvent,m_hEndEvent;
	CHQComm	  *		m_pHQComm;

	void	InitIt(CHQComm *pComm);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReqThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CReqThread)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REQTHREAD_H__15D97F8C_4AC0_4011_8DBB_C867B1D86255__INCLUDED_)
