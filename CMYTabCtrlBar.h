// CJTabCtrlBar.h : header file
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(CMYTABCTRLBAR_H_INCLUDED)
#define CMYTABCTRLBAR_H_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>
#include "CMYControlBar.h"

typedef struct
{
	CWnd *pWnd;
	char szLabel[32];
}TCB_ITEM;

/////////////////////////////////////////////////////////////////////////////
// CMyTabCtrlBar tab control bar

#include "MySwitchBar2.h"

#define UM_SWITCHBAR2_CHANGE	WM_USER+0x611
#define UM_SHOWIT_MESSAGE		WM_USER+0x612

class CMyTabCtrlBar : public CMyControlBar
{
// Construction
public:
	CMyTabCtrlBar();

// Attributes
public:
//	CTabCtrl m_tabctrl;
	CMySwitchBar2 m_tabctrl;
protected:
	int m_nActiveTab;
	CList <TCB_ITEM *,TCB_ITEM *> m_wnds;
	
	short m_nAvailIE[30];
// Operations
public:
	void SetActiveWnd(int nNewTab);
	int  GetActiveWnd();
	CImageList* SetTabImageList(CImageList *pImageList);
	BOOL m_bShowTab;
	BOOL m_bAutoSize;
// Overrides
public:
    // ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyTabCtrlBar)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	int	   GetWndNum();
	CWnd* GetWnd(int nWnd);
	BOOL AddWnd(LPCTSTR lpszLabel, CRuntimeClass *pWndClass,BOOL bIE=FALSE,BOOL bMine=FALSE);
	BOOL AddChatWnd(LPCTSTR lpszLabel);
	void RemoveWnd(int nWnd);
	virtual ~CMyTabCtrlBar();
	int		GetCurWnd();
// Generated message map functions
protected:
	//{{AFX_MSG(CMyTabCtrlBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnTreeReflectMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnStartFuncMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDoLevinBuyMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDoLevinSellMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDoWTCancelMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDoQzSDSMessage(WPARAM wParam,LPARAM lParam);

	afx_msg void OnSwitchBar2SelChange(WPARAM wParam,LPARAM lParam);
	afx_msg void OnButtonBack();	
	afx_msg void OnUpdateButtonBack(CCmdUI* pCmdUI);
	afx_msg void OnButtonForward();	
	afx_msg void OnUpdateButtonForward(CCmdUI* pCmdUI);
	afx_msg void OnButtonMainPage();	
	afx_msg void OnUpdateButtonMainPage(CCmdUI* pCmdUI);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // CJTABCTRLBAR_H_INCLUDED
