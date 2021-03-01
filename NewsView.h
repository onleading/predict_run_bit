// NewsView.h : interface of the CNewsView class
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEWSVIEW_H__C642EAA4_597B_11D3_8C7B_0000800104D5__INCLUDED_)
#define AFX_NEWSVIEW_H__C642EAA4_597B_11D3_8C7B_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyRichEdit.h"

struct StringComp
{
	BOOL operator()(const CString c1, const CString c2) const
	{
		return c1.Compare(c2)<0;
	}
};

class CNewsView : public CView
{
	void		ShowOneFile(int whichtype,const char *filename,int which=0);
	void		SetFindFlag(int pos,int len,COLORREF clr);
	void		ProcessFind();
	void		JustifyPos();
	
	void		GetRemoteTitle(int Which);
	void		GetRemoteFile(int CurTitleNo);
	void		GetXXGTitle(void);
	void		GetXXGTxt(char *title, char *fullTitle);

	short		m_nMainID;
	CFile		theRemoteFile;
	BOOL		bRemoteFileOpen;
	DWORD		nCurRemotePos;
	int			m_nCurTitleNo;
	char		m_TitleStr[256*75];	//放标题的缓冲区
	int			m_nTitleCount;		//读标题的次数
protected:
	CNewsView();
	DECLARE_DYNCREATE(CNewsView)

// Attributes
public:
// Operations
public:
	CListCtrl		m_TypeList;
	CListCtrl		m_TitleList;

	CImageList		m_ImageList;

	CMyRichEdit		m_RichEdit;
	CHARFORMAT		cfm;

	BOOL			m_bFullWindow;
	int				TitleNum;
	BOOL			KeyGuyFlag;
	BOOL			m_bQuitting;
	int				CurrentType;
	int				CurrentTitle;
	CFont *			m_pFont;

	struct tag_FindResult2	m_FindResult2[MAX_FINDRESULT];
	int						m_nFindNum;
	
	std::map<CString, CString, StringComp> m_mXxgTxt;
	int						m_nXXGType;
	short					m_nXXGItemNum;
	char					m_XXGTitleStr[256*100];		//放整理后的标题，最多100条
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewsView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNewsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	void OnTypeChange();
	void OnTitleChange();

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAddNote(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAllWindow(CCmdUI* pCmdUI);
	afx_msg void OnKeyGuyMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnGetDataAck(WPARAM wParam,LPARAM lParam);
	afx_msg void OnRightClick(UINT nID);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//{{AFX_MSG(CNewsView)
	afx_msg void OnItemchangedTitleList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedTypeList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnTxtfind();
	afx_msg void OnFindresult();
	afx_msg void OnRichEditExLink( NMHDR* in_pNotifyHeader, LRESULT* out_pResult );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWSVIEW_H__C642EAA4_597B_11D3_8C7B_0000800104D5__INCLUDED_)
