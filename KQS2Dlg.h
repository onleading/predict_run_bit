#if !defined(AFX_KQS2Dlg_H__36949044_C475_11D3_8B1F_00E04C66B9E1__INCLUDED_)
#define AFX_KQS2Dlg_H__36949044_C475_11D3_8B1F_00E04C66B9E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KQS2Dlg.h : header file

/////////////////////////////////////////////////////////////////////////////
// CKQS2Dlg dialog

#define KQS_TICK_NUM	5

class CKQS2Dlg : public CDialog
{
public:
	CKQS2Dlg(CWnd* pParent = NULL);   // standard constructor
	BOOL Create(CWnd *pParent);
// Dialog Data
	//{{AFX_DATA(CKQS2Dlg)
	enum { IDD = IDD_KQS2_DIALOG };
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKQS2Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON		m_hIcon;
	BOOL		m_bStartFClient_OK;
	
	FASTHQ_10	m_HQ;
	FASTHQ_10	m_Last_HQ;
	TICKDATA	m_pTick[KQS_TICK_NUM];
	int			m_nTickNum;

	int			m_XsFlag;

	MemStkInfo *m_pStkInfo;
	BOOL		m_bLevinModal;
	BOOL		m_bDrawToken;

	CRect		m_BuyRect_Sell[10],m_BuyRect_Buy[10];
	CRect		m_SellRect_Sell[10],m_SellRect_Buy[10];

	BOOL		StartFClient();
	void		EndFClient();

	void		DrawIt(CRect rect,CDC *pDC);
	void		DoLevin(int which,BOOL bIsBuy);
	void		OnChangeCode(MemStkInfo *pInfo);
	// Generated message map functions
	//{{AFX_MSG(CKQS2Dlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKQSACK(WPARAM wParam,LPARAM lParam);
	afx_msg void OnSHQZ(UINT nID);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KQS2Dlg_H__36949044_C475_11D3_8B1F_00E04C66B9E1__INCLUDED_)
