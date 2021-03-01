#if !defined(AFX_SWITCHERWND_H__91B9D501_D8DB_11D2_9FDF_D79776C64A53__INCLUDED_)
#define AFX_SWITCHERWND_H__91B9D501_D8DB_11D2_9FDF_D79776C64A53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SwitcherWnd.h
//
// Programmed by: JIMMY BRUSH (Kathy007@email.msn.com)
// 
// Legal:
//
// THIS CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.
// You may use and distribute this code provided that you do not
// remove this title header and that you do not charge money for
// it. If you want to update the code, feel free to do so, as long
// as you *mark your changes* in code AND in the revision log below
// (and send it to me ;)

#include "SwitcherButton.h"
#define SWM_SELCHANGE (WM_USER + 137)
#define SWM_UNSELECT (WM_USER + 138)

/////////////////////////////////////////////////////////////////////////////
// CSwitcherWnd window

class CSwitcherWnd : public CWnd
{
// Construction
public:
	CSwitcherWnd();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSwitcherWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL ModifyButton(int index, CString text, HICON icon);
	int GetNumButtons();
	HICON GetButtonIcon(int id);
	CString GetButtonText(int id);
	BOOL SetSel(int num);
	int GetSel();
	int FindButton(CString text, HICON icon = NULL, int StartAt = 0);
	BOOL RemoveButton(int index);
	void RemoveAll();
	BOOL AddButton(CString title, HICON icon);
	BOOL DoCreate(CWnd* parent, int x, int y, int cx, int cy = 25, CString title = "SwitcherWnd");

	virtual ~CSwitcherWnd();

protected:
	int m_iNextButtonStart;
	int m_iButtonWidth;
	int m_iSelectedButton;
	void ResizeButtons(int NewSize);
	CSwitcherButton* GetButtonFromID(int id);
	CPtrArray m_Buttons;
	CWnd* m_wndParent;

	// Generated message map functions
protected:
	//{{AFX_MSG(CSwitcherWnd)
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnUnselect(WPARAM, LPARAM);
	afx_msg LRESULT OnSelChange(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SWITCHERWND_H__91B9D501_D8DB_11D2_9FDF_D79776C64A53__INCLUDED_)
