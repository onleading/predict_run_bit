#if !defined(AFX_SORTLISTCTRL_H__10ACF2EC_2BEE_4B90_9307_9C7549FD337F__INCLUDED_)
#define AFX_SORTLISTCTRL_H__10ACF2EC_2BEE_4B90_9307_9C7549FD337F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SortListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSortListCtrl window

class CSortListCtrl : public CListCtrl
{
// Construction
public:
	CSortListCtrl();

// Attributes
public:
	BOOL m_fAsc;//是否顺序排序
	int m_nSortedCol;//当前排序的列

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSortListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSortListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSortListCtrl)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SORTLISTCTRL_H__10ACF2EC_2BEE_4B90_9307_9C7549FD337F__INCLUDED_)
