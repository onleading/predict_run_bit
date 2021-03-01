#if !defined(AFX_DSZHSortDlg_H__36949044_C475_11D3_8B1F_00E04C66B9E1__INCLUDED_)
#define AFX_DSZHSortDlg_H__36949044_C475_11D3_8B1F_00E04C66B9E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DSZHSortDlg.h : header file
//
#include "CaptionButton.h"
#include "UZhSort.h"
#include "USortGrid.h"
#include "UnitView.h"
/////////////////////////////////////////////////////////////////////////////
// CDSZHSortDlg dialog

class CDSZHSortDlg : public CCaptionButtonDlg
{
public:
	CDSZHSortDlg(CWnd* pParent = NULL);   // standard constructor
	~CDSZHSortDlg();

	int	GetWhichType()
	{
		UZhSort * pUzhSort = (UZhSort*)m_Units.pUnit[0];
		if(pUzhSort) 
			return pUzhSort->WhichType;
		return -1;
	}

	virtual void OnCaptionButton1();
	virtual void OnCaptionButton2();
	virtual void InitCaptionButtons();

// Dialog Data
	//{{AFX_DATA(CDSZHSortDlg)
	enum { IDD = IDD_ZHSORT_DIALOG };
	//}}AFX_DATA
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDSZHSortDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	CRect	m_Rect;			//画区域
	UNITS	m_Units;		//存放各Units信息的结构

	//远程使用
	short	m_nMainID;
	void	SetMainID();
// Implementation
protected:
	HICON	m_hIcon;

	void ResizePad(int flag=COM_OnlyDraw);
	void CreateUnit(int which,int initGPNo,int type,int style=0);
	void OnGpVip(int iIndex);

	void OnGetDataAck(WPARAM wParam,LPARAM lParam);
	void GetMainID(WPARAM wParam,LPARAM lParam);	//采用消息的方式，直接填写g_nMainID
	// Generated message map functions
	//{{AFX_MSG(CHisDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetunitoption();
	afx_msg void OnChangeZhSortSC(UINT nID);
	afx_msg void OnUpdateChangeZhSortSC(CCmdUI *pCmdUI);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DSZHSortDlg_H__36949044_C475_11D3_8B1F_00E04C66B9E1__INCLUDED_)
