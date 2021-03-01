#if !defined(AFX_MYSTATUSBAR_H__C9F1B097_FD4B_11D2_98A3_0088CC032229__INCLUDED_)
#define AFX_MYSTATUSBAR_H__C9F1B097_FD4B_11D2_98A3_0088CC032229__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StatusWnd.h"
#include "MovingInfoDlg.h"

class CMyStatusBar : public CStatusBar
{
	CStatusWnd	*	m_pStatusWnd;
	CMovingInfoDlg	*m_pMovingInfoDlg;
public:
	CMyStatusBar();
	virtual ~CMyStatusBar();
	void	RedrawStatusWnd();

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyStatusBar)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CMyStatusBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
