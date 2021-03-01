#if !defined(AFX_TXTVIEW_H__5A8AD482_2306_11D3_98A5_0088CC032229__INCLUDED_)
#define AFX_TXTVIEW_H__5A8AD482_2306_11D3_98A5_0088CC032229__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TxtView.h : header file

#include "MyRichEdit.h"
#include "FlatButton.h"
#include "webbrowser2.h"

class CTxtView : public CView
{
	void SetSelColor();
	void UpdateButton();
	void UncheckAllButton();
	int  GetCurSelBtn();
	void JustifyButton(CRect rcdlg);
	void Resize();

	void ProcessF10Content(int which);
	void ProcessFind();
	void SetFindFlag(int pos,int len,COLORREF clr);
	void GetTdxJBZL(const char *code,short setcode);
	void GetRemoteFile(const char *Code,short setcode);

	void Get3Tomo(short *lp2Index);
protected:
	CTxtView();				// protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTxtView)
public:
	CMyRichEdit m_RichEdit;
	CHARFORMAT	cfm;

	BOOL	m_bSegmentTxt;
	CFont  *m_pFont;
	BOOL	KeyGuyFlag;
	BOOL	m_bQuitting;
	CString m_TxtCurGPCode;
	short	m_TxtCurSetCode;
	int		CurrentJBM;		//CurrentJBM为-1表示通达信资讯
	short	GetCurrentGPIndex();

	//用于远程
	short	m_nMainID;
	CFile	theRemoteFile;
	BOOL	bRemoteFileOpen;
	DWORD	nCurRemotePos;

	CFlatButton m_btnF10[MAX_F10CAPTION];	//所有的f10每次最大能同时提供16条信息
	BOOL m_bLayoutFlag;
	BOOL m_bCanClickFL;
	char	*m_pTmpBuf;
	CWebBrowser2   *m_pExplorer;
	BOOL			m_bGPRelation;

	short	m_whichfl;
	char	FileBuffer[32*1024+200];
	
	//
	short	m_nPageGPIndex[MAXGPNUM];
	int		m_nPageGPNum;
	short	m_nCurPageGP;
	short	m_PreNowNext_Index[3];
	int		m_nMostLeft;
	CRect	m_PreNext_Rect[5];
	
	LinkedSetInfo m_LSetInfoP, m_LSetInfoN;
	COLORREF m_SelColor,m_ForeColor,m_BackColor;
	int		 m_nRichEditTop;

	BOOL	m_bDS;
// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTxtView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTxtView();

	void	SetPageGP(short *lpnIndex,short nTotalGP,short nCurGP,LinkedSetInfo *pLSetInfo=NULL);
	int		GetNextPageGP(short *lpnIndex,short num,BOOL bNext);
	// Generated message map functions
protected:
	afx_msg void OnClickFLButton(UINT nID);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRichEditMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnGetDataAck(WPARAM wParam,LPARAM lParam);
	afx_msg void OnUpdateCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAddNote(CCmdUI* pCmdUI);
	afx_msg void OnRightClick(UINT nID);
	afx_msg void OnSelectJBM(UINT nID);
	afx_msg void OnUpdateSelectJBM(CCmdUI *pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	afx_msg void OnCancelMark(void);
	afx_msg void OnMarkStockFlag(UINT nID);
	afx_msg void OnUpdateMarkStockFlag(CCmdUI* pCmdUI);
	//{{AFX_MSG(CTxtView)
	afx_msg void OnInBlock();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRightAddblock();
	afx_msg void OnRightAddZxg();
	afx_msg void OnDelFromBlock();
	afx_msg void OnOtherjbm();
	afx_msg void OnUpdateOtherjbm(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnTxtfind();
	afx_msg void OnFindresult();
	afx_msg void DocumentComplete(LPDISPATCH /* pDisp */, VARIANT* URL);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRichEditExLink( NMHDR* in_pNotifyHeader, LRESULT* out_pResult );
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TXTVIEW_H__5A8AD482_2306_11D3_98A5_0088CC032229__INCLUDED_)
