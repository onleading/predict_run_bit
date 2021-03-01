#if !defined(AFX_HIDENETWND_H__F6C00956_5F8A_43DB_BF02_7C0073FAC804__INCLUDED_)
#define AFX_HIDENETWND_H__F6C00956_5F8A_43DB_BF02_7C0073FAC804__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HideNetWnd.h : header file
//


class CHideNetWndClass;
//支持的行情源
class CHqSource;
/////////////////////////////////////////////////////////////////////////////
// CHideNetWnd thread
class CHideNetWnd : public CWinThread
{
	DECLARE_DYNCREATE(CHideNetWnd)
protected:
	CHideNetWnd();           // protected constructor used by dynamic creation

// Attributes
public:
	CHideNetWndClass	*m_pHideNetWnd;
	CHqSource			*m_pHq;
	HANDLE				m_StartEvent;

private:
	UINT				m_dataSource;
	CString				m_strIPAddr;
	CString				m_strPort;
	CString				m_strMarket;
	int					m_iNo;				

// Operations
public:
	void	RegisterDataEngin(int iNo,UINT DataType,		//行情源类型,同类主站行情源
		char *strIPAddr="", char *strPort="",
							char *strMarket="");
	BOOL	StartHqThread(int nMaxInitSec);

	void	FinalKill();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHideNetWnd)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL
	

// Implementation
protected:
	virtual ~CHideNetWnd();
	BOOL	StartDataEngine(void);

	// Generated message map functions
	//{{AFX_MSG(CHideNetWnd)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg	LRESULT OnKillThread(WPARAM wParam,LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HIDENETWND_H__F6C00956_5F8A_43DB_BF02_7C0073FAC804__INCLUDED_)
