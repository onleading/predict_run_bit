#if !defined(AFX_DESCFORMDLG_H__90EE2301_1210_11D5_B99A_00E04C3DCAC5__INCLUDED_)
#define AFX_DESCFORMDLG_H__90EE2301_1210_11D5_B99A_00E04C3DCAC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DescFormDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDescFormDlg form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "interface.h"

class CDescFormDlg : public CFormView
{
public:
	CDescFormDlg();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDescFormDlg)
	virtual ~CDescFormDlg();
	void HideSelf();
	void GetCurrData(float *pCurrData = NULL);

// Form Data
public:
	//{{AFX_DATA(CDescFormDlg)
	enum { IDD = IDD_FORMDLG };
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	void Init(CRect rect, CWnd *pView = NULL);
	void SetStatus(int nSet, int nIndex, int nPeriod, BOOL bIsSpecialLine = FALSE,
		BOOL bInitPara = FALSE,float *pInitPara = NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDescFormDlg)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CDescFormDlg)
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnCancelOp(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	float	m_fCurrValue[MAX_PARAMNUM];
	float	m_fBakValue[MAX_PARAMNUM];
	CWnd*	m_pView;
	CRect	m_rcSavePos;	//用来保存formview出现的位置,主要是保证左上角和宽度位置不变
	CRect	m_rcSaveBak;
	int		m_nPeriod;
	int		m_nEditLength;	//用来保存每一个编辑框的宽度,包括spin的宽度
	CPoint	m_ptEditPos[MAX_PARAMNUM];//用来记录edit在本行所占的矩形中的相对行列位置
	CSize	m_csText;		//保存字体的大小信息
	CFont	m_font;
	TINDEXINFO *m_pTmpIndex;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESCFORMDLG_H__90EE2301_1210_11D5_B99A_00E04C3DCAC5__INCLUDED_)
