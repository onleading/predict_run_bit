#if !defined(AFX_ZHSortDlg_H__36949044_C475_11D3_8B1F_00E04C66B9E1__INCLUDED_)
#define AFX_ZHSortDlg_H__36949044_C475_11D3_8B1F_00E04C66B9E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ZHSortDlg.h : header file
//
#include "CaptionButton.h"
/////////////////////////////////////////////////////////////////////////////
// CZHSortDlg dialog

class CZHSortDlg : public CCaptionButtonDlg
{
	void	ProcessSort();
	float	fGetValue(int Type,short GpIndex);
	void	Sort(int Type,BOOL bAscendSort);
	void	DrawOneCell(CDC *pDC,int type);
	void	OnGpVip(int iIndex);
	int		HitTest(CPoint point);

	void	CalcSomeInfo(int nFlag);
	int		GetValidIndex(int nIndex,BOOL bNext);
	int		ToNextUnit(int nCurrUnit, int nSkipVal, BOOL bNext);
public:
	CZHSortDlg(CWnd* pParent = NULL);   // standard constructor
	short	GetCurrentGPIndex();
	void	SetSCType(int nWhichType);
	void	SetStyle(int nStyle);

	virtual void OnCaptionButton1();
	virtual void OnCaptionButton2();
	virtual void InitCaptionButtons();

// Dialog Data
	//{{AFX_DATA(CZHSortDlg)
	enum { IDD = IDD_ZHSORT_DIALOG };
	//}}AFX_DATA

	int		WhichType;
	char	TypeStr[9][25];
	CRect	CellRect[9];
	short   TypeFlag[9];
	int		m_nUnitRow, m_nUnitCol;
	int		m_nRowNum;
	int		m_nItemHeight;

	short	lpnUsedStkIndex[MAXGPNUM];	//指向本窗口实际能显示的股票序号数组
	int		nGpNum;

	short	SortResult[9][20];			//存放排序结果的股票索引
	ZHRESULT m_ZHBuf[9];			//存放远程排序的内容

	int		m_nCurSel;	//0<m_nCurSel<54

	BOOL	m_bHasData;
	short	m_nMainID;
	BOOL	m_bRefreshDraw;

	CToolTipCtrl	m_MarkToolTip;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZHSortDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CZHSortDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnChangeSCType(WPARAM wParam,LPARAM lParam);
	afx_msg void OnChangeZFPeriod(WPARAM wParam,LPARAM lParam);
	afx_msg void OnGetDataAck(WPARAM wParam,LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZHSortDlg_H__36949044_C475_11D3_8B1F_00E04C66B9E1__INCLUDED_)
