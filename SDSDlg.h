#if !defined(AFX_SDSDlg_H__36949044_C475_11D3_8B1F_00E04C66B9E1__INCLUDED_)
#define AFX_SDSDlg_H__36949044_C475_11D3_8B1F_00E04C66B9E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SDSDlg.h : header file

/////////////////////////////////////////////////////////////////////////////
// CSDSDlg dialog

#define MAX_USE_SHQZ_NUM	100

class CSDSDlg : public CDialog
{
public:
	CSDSDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL Create(CWnd *pParent);
// Dialog Data
	//{{AFX_DATA(CSDSDlg)
	enum { IDD = IDD_SDS_DIALOG };
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDSDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON		m_hIcon;
	BOOL		m_bStartFClient_OK;
	FASTHQ		m_HQ;
	int			m_XsFlag;

	char		m_strCode[MAX_USE_SHQZ_NUM][SH_CODE_LEN+1];
	int			m_nCodeNum;
	int			m_nSel;
	int			m_NowFlag;
	BOOL		m_bLevinModal;
	BOOL		m_bDrawToken;

	CMenu		m_MainMenu,m_CodeMenu;

	CRect		m_BuyRect_Sell[5],m_BuyRect_Buy[5];
	CRect		m_SellRect_Sell[5],m_SellRect_Buy[5];

	BOOL	StartFClient();
	void	EndFClient();

	void	DrawIt(CRect rect,CDC *pDC);

	void	DoLevin(int which,BOOL bIsBuy);
	// Generated message map functions
	//{{AFX_MSG(CSDSDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSDSACK(WPARAM wParam,LPARAM lParam);
	afx_msg void OnSHQZ(UINT nID);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDSDlg_H__36949044_C475_11D3_8B1F_00E04C66B9E1__INCLUDED_)
