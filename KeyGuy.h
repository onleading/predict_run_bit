#if !defined(AFX_KEYGUY_H__00A2C7A1_FD73_11D2_98A3_0088CC032229__INCLUDED_)
#define AFX_KEYGUY_H__00A2C7A1_FD73_11D2_98A3_0088CC032229__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/////////////////////////////////////////////////////////////////////////////

#include "ColorListBox.h"
#include "MyBitmapBt.h"
class CKeyGuy : public CDialog
{
// Construction
public:
 	CKeyGuy(CWnd* pParent = NULL);   // standard constructor
	~CKeyGuy();
    int lpnIndex[MAXKGITEM];

	static BOOL OnCommonProcess(int Type,const char *Value);
	
//Attributes
public:
	CRect m_title_rect;
	CRect m_edit_rect;
	CRect m_list_rect;
	CRect m_sysbt_close_rect;
	CRect m_wnd_rect;
	CRgn m_wnd_rgn;
//Operation
public:
	void GetCursorPosToWnd(CPoint &pt);

// Dialog Data
	//{{AFX_DATA(CKeyGuy)
	enum { IDD = IDD_KEYGUY_DIALOG };
	CColorListBox	m_ListControl;
	CEdit	m_EditControl;
	CString	m_edit;
	//}}AFX_DATA
	BOOL	bHasCmd;
	BOOL	bAlwaysOn;
	int		ReturnType;
	CString	ReturnValue;
	
	CPoint RightBottomPoint;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeyGuy)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	struct KeyGuyContent RowList[MAXKGITEM];
	int	nRowNum;
	
	CFont	*m_pNewFont;
protected:

	// Generated message map functions
	//{{AFX_MSG(CKeyGuy)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnChangeEdit();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkList();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYGUY_H__00A2C7A1_FD73_11D2_98A3_0088CC032229__INCLUDED_)
