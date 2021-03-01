#if !defined(AFX_HKNEWSDLG_H__6473F2F3_E0F9_471E_A66F_F7CF88D79227__INCLUDED_)
#define AFX_HKNEWSDLG_H__6473F2F3_E0F9_471E_A66F_F7CF88D79227__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HKNewsDlg.h : header file
//
#include "UHKNews.h"
#include "UnitView.h"
/////////////////////////////////////////////////////////////////////////////
// CHKNewsDlg dialog

class CHKNewsDlg : public CDialog
{
// Construction
public:
	CHKNewsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHKNewsDlg)
	enum { IDD = IDD_HKNEWS_DLG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHKNewsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

private:
	CRect	m_Rect;			//画区域
	UNITS	m_Units;		//存放各Units信息的结构

// Implementation
protected:
	HICON	m_hIcon;

	void ResizePad(int flag=COM_OnlyDraw);
	void CreateUnit(int which,int initGPNo,int type,int style=0);

	// Generated message map functions
	//{{AFX_MSG(CHKNewsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HKNEWSDLG_H__6473F2F3_E0F9_471E_A66F_F7CF88D79227__INCLUDED_)
