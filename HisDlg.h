#if !defined(AFX_HISDLG_H__FC5CA677_CA0C_4ACB_BC86_59669B363BC3__INCLUDED_)
#define AFX_HISDLG_H__FC5CA677_CA0C_4ACB_BC86_59669B363BC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HisDlg.h : header file
/////////////////////////////////////////////////////////////////////////////

#include "UZst.h"
#include "UFjb.h"
#include "UFsb.h"
#include "UTick.h"
#include "UMmp.h"
#include "UHisk.h"
#include "SwitchEx.h"
#include "UnitView.h"

class CHisDlg : public CDialog
{
// Construction
public:
	CHisDlg(CWnd* pParent = NULL);   // standard constructor
	~CHisDlg();

	short		m_nCurGP;
	long		m_nCurDate;
	CWnd   *	m_pView;
	CNewZfjsTab *	m_pSwitchBar;
// Dialog Data
	//{{AFX_DATA(CHisDlg)
	enum { IDD = IDD_HIS_DIALOG };
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHisDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
private:
	CRect	m_Rect;			//画区域
	UNITS	m_Units;		//存放各Units信息的结构
	MemStkInfo *m_pStkInfo;

	short	m_nCurSwitch;
	short	m_nSwitchWid;

	BOOL	m_bSplitting;
	int		m_nOldSplitX;

	//远程使用
	short	m_nMainID;
	void	SetMainID();
protected:
	HICON	m_hIcon;
	void	DrawSwitchCtrl();
	int		EscHandle();

	void SwitchPad(int whichSwitch);
	void ResizePad(int flag=COM_OnlyDraw);
	void CreateUnit(int which,int initGPNo,int type,int style=0);
	BOOL ChangeUnitGP(int which,int iNo);
	BOOL ChangeUnitDate(int which,long date);

	int	HitTest(CPoint point);

	void OnCornerClick(UINT nID);
	void OnUpdateCornerClick(CCmdUI *pCmdUI);
	void OnGetDataAck(WPARAM wParam,LPARAM lParam);
	void GetMainID(WPARAM wParam,LPARAM lParam);	//采用消息的方式，直接填写g_nMainID
	// Generated message map functions
	//{{AFX_MSG(CHisDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
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
	afx_msg void OnQjStat();
	afx_msg void OnAllQjStat();
	afx_msg void OnManyDayZst(UINT nID);
	afx_msg void OnUpdateManyDayZst(CCmdUI *pCmdUI);
	afx_msg void OnWritetotxtfile();
	afx_msg void OnOverlapZS();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HISDLG_H__FC5CA677_CA0C_4ACB_BC86_59669B363BC3__INCLUDED_)
