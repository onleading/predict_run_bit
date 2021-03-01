#ifndef __SAFE_COMBOBOX__H__
#define __SAFE_COMBOBOX__H__
#if _MSC_VER > 1000
#pragma once
#endif
#include "SafeEdit.h"

class CSafeComboBox : public CComboBox
{
public:
	CSafeComboBox(BOOL bSafeMode=TRUE);
	virtual ~CSafeComboBox();
	BOOL Create(DWORD dwStyle,const RECT& rect,CWnd* pParentWnd,UINT nID);
	CSafeEdit& GetSafeEdit() { return m_wndEdit; }
	VOID SetExtendedStyle(DWORD dwExtendedStyle);
	BOOL ModifyExtendedStyle(DWORD dwRemove,DWORD dwAdd,BOOL bRedraw);
public:
	//{{AFX_VIRTUAL(CSafeComboBox)
	protected:
	virtual void PreSubclassWindow();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(CSafeComboBox)
	afx_msg void OnSelchange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	VOID SubclassEditWindow();
protected:
	CSafeEdit m_wndEdit;
};

//{{AFX_INSERT_LOCATION}}
#endif
