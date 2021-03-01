#if !defined(AFX_WEBTREE_H__6699B088_A755_4ADE_A9C9_238FCDD80514__INCLUDED_)
#define AFX_WEBTREE_H__6699B088_A755_4ADE_A9C9_238FCDD80514__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// webtree.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CWebTree window

#include "IniFile.h"
#include "MyScrollbar.h"
#include "mytreectrl.h"
#include "TreeWnd.h"

struct tag_URL
{
	char addr[255];
};

class CWebTree : public CTreeWnd
{
	void AddNodeFromIni(CTreeCtrl * pTC,HTREEITEM hParent,LPCSTR AppName,CIniFile *pinif,int index,vector<tag_URL> *purl);
	int  GetChildItemCount(HTREEITEM h,CTreeCtrl *pTC);
	void SetNodeIcon(CTreeCtrl *pTC,HTREEITEM hParent,CImageList *pimage,vector<tag_URL> *purl);
	void CreateTreeFromIni(LPCSTR inifile);
	BOOL GetFirstURLAboutStock(const char *GpCode,CString &strUrl);
public:
	CWebTree();
	DECLARE_DYNCREATE(CWebTree)

	CMyTreeCtrl*	m_TreeCtrl;
	CFont	*		m_pNewFont;
	CImageList		m_ImageList;

	CMyScrollBar	*pVScrollBar;

	vector<tag_URL> m_url;
	HICON m_hRoot,m_hLink,m_hLinkToStock;
	char m_strCurStock[SH_CODE_LEN+1];
	char m_strLastStockUrl[255];
// Attributes
public:

// Operations
public:
	void SetAppearance(COLORREF bColor,COLORREF fColor);
	void FillTree();
	void SetScrollPos();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWebTree)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWebTree();

	// Generated message map functions
protected:
	afx_msg void OnVScrollMessage(WPARAM wParam,LPARAM lParam);
	//{{AFX_MSG(CWebTree)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeReflectMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnNcDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEBTREE_H__6699B088_A755_4ADE_A9C9_238FCDD80514__INCLUDED_)
