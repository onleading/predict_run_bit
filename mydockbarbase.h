#if !defined(AFX_MYDOCKBAR_H__9C64BE29_0473_49A6_9315_9A437B155011__INCLUDED_)
#define AFX_MYDOCKBAR_H__9C64BE29_0473_49A6_9315_9A437B155011__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyDockBarBase.h : header file
//
#include "MyBitmapBt.h"

#define MAX_BT							10
#define ID_DOCKBAR_BT0					1000

/////////////////////////////////////////////////////////////////////////////
// CMyDockBarBase window

class CMyDockBarBase : public CDialogBar
{
// Construction
public:
	CMyDockBarBase();
	
// Attributes
public:
	CPoint m_pt;
	BOOL m_leftbuttondown;
	MyBitmapBt *m_bt[MAX_BT];
	CRect m_bt_rect[MAX_BT];
	CString m_bt_str[MAX_BT];
	LPCTSTR m_TitleBTBK[9];
	int m_dockpos;//停靠位置,0,1,2,3对应:上,右,下,左
	UINT m_nButton;
	CString m_nIDB_Bt_Center;
// Operations
public:
	BOOL Create(CWnd* pParentWnd, UINT nIDTemplate,UINT nStyle, UINT nID,UINT nButton,LPCTSTR nIDB_Bt0_Center);
	void NextBtRect(CRect &rc,int offset);
	void SetBtText(CString str[MAX_BT]);
	virtual void OnButton(UINT nIndex);
	virtual void ModifyDC(CDC *pDC);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyDockBarBase)
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyDockBarBase();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyDockBarBase)
//	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
//	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYDOCKBAR_H__9C64BE29_0473_49A6_9315_9A437B155011__INCLUDED_)
