#if !defined(AFX_ZX2TREE_H__6699B088_A755_4ADE_A9C9_238FCDD80514__INCLUDED_)
#define AFX_ZX2TREE_H__6699B088_A755_4ADE_A9C9_238FCDD80514__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// zxtree.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CZx2Tree window
#include "MyScrollbar.h"
#include "mytreectrl.h"
#include "TreeWnd.h"

struct AutoDownFileStruct
{
	char	MainURL[200];
	short	URLNum;
	char    Title[50][50];
	char    URL[50][200];
	short	OpenIE[50];
	short	IconIndex[50];
};

class CZx2Tree : public CTreeWnd
{
	void FillOneZX(int i);
	void GetAutoDownFile(char *pFileName,struct AutoDownFileStruct &tmpStruct);
	void CarryOut_WebInfoTreeID(const char *pInfo);
	void CarryOut(DWORD ItemData);
public:
	CZx2Tree();
	DECLARE_DYNCREATE(CZx2Tree)

	CMyTreeCtrl*	m_TreeCtrl;
	CFont	*		m_pNewFont;
	CImageList		m_ImageList;

	CMyScrollBar	*pVScrollBar;

// Attributes
public:
	map<DWORD, string> m_URLMap;
	map<DWORD, short>  m_URLIEModeMap;
	map<DWORD, struct AutoDownFileStruct> m_AutoDownMap;
// Operations
public:
	void SetAppearance(COLORREF bColor,COLORREF fColor);
	void FillTree();
	void SetScrollPos();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZx2Tree)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CZx2Tree();

	long		treeid_num;
	long		cur_refresh_treeid;
	long		infotreeid[50];
	HTREEITEM	infotreeitem[50];
	// Generated message map functions
protected:
	afx_msg void OnVScrollMessage(WPARAM wParam,LPARAM lParam);
	//{{AFX_MSG(CZx2Tree)
	afx_msg void OnFillZxTreeMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnCjzx2TitleMsg(WPARAM wParam,LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnNcDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZX2TREE_H__6699B088_A755_4ADE_A9C9_238FCDD80514__INCLUDED_)
