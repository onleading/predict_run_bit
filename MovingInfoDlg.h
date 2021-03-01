#if !defined(AFX_MOVINGINFODLG_H__02AFE9F1_8D19_4A77_A844_8D6FA5088AB0__INCLUDED_)
#define AFX_MOVINGINFODLG_H__02AFE9F1_8D19_4A77_A844_8D6FA5088AB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MovingInfoDlg.h : header file
/////////////////////////////////////////////////////////////////////////////

#include "iconbutton.h"
#include "struct.h"

struct POS  //某一段文字的起始位置和结束位置
{
	int head;
	int tail;
};

class CMainFrame;

//滚动文字的显示原理
/*
  假设文字从左到右显示，则我们就把所有的文字假想排成一排，这样，每一段要显示的文字都有一个起始坐标，我们定义一个矩形，
  这个矩形从这排文字的左向右移，如果文字的字，或尾在这个矩形内，则应该显示出来。
  从上到下也一样
 */

class CMovingInfoDlg : public CDialog
{
// Construction
public:
	void AddSysInfo(SYS_INFO si);
	void DeleteAllSysInfo();
	void InitNext();
private:
	void SetScrollStyle(bool bEnableStop);
	void SetScrollInfo(bool bShowSysInfo);
	void SetScrollDirection(bool bLeftToRight);
	void ArrangeCtrl();
	int	  m_iHeight;//对话框高度
	bool m_bMaximized;//是否在最大化状态
	bool m_bLeftToRight;//是否从左到右显示,不是从左到右就是从下到上,但显示多行信息时只能从下到上
    bool m_bShowSysInfo;//是否显示系统信息,不是显示系统信息就是显示股票信息,
	bool m_bEnableStop;//显示风格，是否某行到行首后，暂停一下 
    bool m_bTextStopped;//是否滚动的文字正处于停止状态
	int  m_iStopCount;//停止计数，指明应该停止多长个定时器周期

	int	 m_nMovingSpaceTime,m_nMovingStopTime;
	vector<SYS_INFO>  m_vtSysInfo;
	vector<SYS_INFO>  m_vtNowSysInfo;
	vector<POS>       m_vtPos;

	POS  m_posNow;//我们能够看得见的文字的起始位置和结束位置
	bool m_bRestart;//是否本次显示已经结束，应该开始下次显示了	
	
    int m_iTextBoxHeight;
    int m_iDlgWidth;

	CRect	m_rcSingleLine,m_rcMultiLine;
	CMainFrame * m_pMF;

	BOOL	m_bLayoutFlag;
public:	
	CMovingInfoDlg(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CMovingInfoDlg)
	enum { IDD = IDD_MOVINGINFO };
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMovingInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMovingInfoDlg)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg BOOL OnShowToolTip(UINT id,NMHDR *pTTTStruct ,LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOVINGINFODLG_H__02AFE9F1_8D19_4A77_A844_8D6FA5088AB0__INCLUDED_)
