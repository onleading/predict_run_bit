#if !defined(AFX_TVVIEW_H__09C32BE2_A0CA_11D3_B921_0000800104D5__INCLUDED_)
#define AFX_TVVIEW_H__09C32BE2_A0CA_11D3_B921_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTvView : public CView
{
protected:
	CTvView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTvView)
// Attributes
public:
	BOOL		KeyGuyFlag;
	BOOL		m_bQuitting;
public:
	short		GetCurrentGPIndex();
	BOOL		ProcessGPCode(const char *code);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTvView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnDraw(CDC* pDC);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTvView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTvView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKeyGuyMessage(WPARAM wParam,LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_TVVIEW_H__09C32BE2_A0CA_11D3_B921_0000800104D5__INCLUDED_)
