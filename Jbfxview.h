#if !defined(AFX_JBFXVIEW_H__09C32BE2_A0CA_11D3_B921_0000800104D5__INCLUDED_)
#define AFX_JBFXVIEW_H__09C32BE2_A0CA_11D3_B921_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ComView2.h"

/////////////////////////////////////////////////////////////////////////
//
class CJbfxView : public CComView2
{
protected:
	CJbfxView();
	DECLARE_DYNCREATE(CJbfxView)
private:
	BOOL		KeyGuyFlag;
	BOOL		m_bQuitting;
	BOOL		m_bLayoutFlag;
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJbfxView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CJbfxView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CJbfxView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_JBFXVIEW_H__09C32BE2_A0CA_11D3_B921_0000800104D5__INCLUDED_)
