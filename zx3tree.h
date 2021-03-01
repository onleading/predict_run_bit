#if !defined(AFX_ZX3TREE_H__6699B088_A755_4ADE_A9C9_238FCDD80514__INCLUDED_)
#define AFX_ZX3TREE_H__6699B088_A755_4ADE_A9C9_238FCDD80514__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// zx3tree.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CZx3Tree window
#include "MyScrollbar.h"
#include "mytreectrl.h"
#include "TreeWnd.h"
#include "webbrowser2.h"

#define MAX_META_ITEM	300

typedef struct 
{
	short itemno;			//栏目ID
	char  itemtype;			//栏目类型,0表示父节点,1表示叶节点
	short parentitem;		//父节点是哪一个栏目ID
	char  itemlevel;		//此节点的访问级别
	short productid;		//产品树ID
	char  itemname[50];		//此节点的显示名称
	char  url[255];			//此节点的URL
	char  yyblist[255];		//访问此节点的营业部列表,比如1100;1300;6100...
}META_ITEM_INFO;

class CZx3Tree : public CTreeWnd
{
	void	SetContentAndNavigate(const char *pContent);
	void	FillOneZX(int i);
	void	CarryOut(DWORD ItemData);
	int		TestInfoMngRight(META_ITEM_INFO *pMeta);
public:
	CZx3Tree();
	DECLARE_DYNCREATE(CZx3Tree)

	CMyTreeCtrl*	m_TreeCtrl;
	CFont	*		m_pNewFont;
	CImageList		m_ImageList;

	CMyScrollBar  *	pVScrollBar;
	
	CWebBrowser2  *	m_pExplorer;
	CString			m_strIEBody;
// Attributes
public:
	map<DWORD, string>	m_URLMap;
	map<DWORD, char>	m_LevelFlagMap;
private:
	META_ITEM_INFO *m_pMeta;

	struct ZxCache_Info *m_pZxCache_Info;
	int				m_nZXCookieNum;
	BYTE			m_pZXSecurePackage[1024];
// Operations
public:
	void SetAppearance(COLORREF bColor,COLORREF fColor);
	void FillTree();
	void SetScrollPos();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZx3Tree)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CZx3Tree();

	// Generated message map functions
protected:
	afx_msg void OnVScrollMessage(WPARAM wParam,LPARAM lParam);
	//{{AFX_MSG(CZx3Tree)
	afx_msg void OnFillZxTreeMsg(WPARAM wParam,LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnNcDestroy();
	//}}AFX_MSG
	afx_msg void OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel);
	DECLARE_EVENTSINK_MAP()	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZX3TREE_H__6699B088_A755_4ADE_A9C9_238FCDD80514__INCLUDED_)
