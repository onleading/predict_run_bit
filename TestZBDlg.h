#if !defined(AFX_TESTZBDLG_H__C04AB602_071F_11D5_B99A_00E04C3DCAC5__INCLUDED_)
#define AFX_TESTZBDLG_H__C04AB602_071F_11D5_B99A_00E04C3DCAC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestZBDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTestZBDlg dialog
const int MAXCOUNT = 50;
#include "Label.h"
#include "interface.h"
#include "DragListCtrl.h"
#include "GradientProgressCtrl.h"

class CTestZBDlg : public CDialog
{
// Construction
public:
	CTestZBDlg(CWnd* pParent = NULL);   // standard constructor
	int m_nAddedZBIndex[MAXCOUNT];
	int m_nAddedZBNum;

// Dialog Data
	//{{AFX_DATA(CTestZBDlg)
	enum { IDD = IDD_TESTZBDLG };
	CEdit	m_TestTotal;
	CEdit	m_GPTotal;
	CEdit	m_HaveDoneTotal;
	CLabel	m_OutInfo;
	CComboBox	m_AimCombo;
	CComboBox	m_PeriodCombo;
	CDragListCtrl	m_List;
	CGradientProgressCtrl	m_Progress;
	CTime	m_BeginTime;
	CTime	m_EndTime;
	int		m_nDontUseRecentData;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestZBDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTestZBDlg)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelectall();
	virtual void OnOK();
	afx_msg void OnSelchangeCombo2();
	afx_msg void OnRadio14();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int				m_nPeriod;		//保存测试周期(日线,周线...)
	LPHISDAT		m_pAnalyData;	//分析数据
	float			m_fMaxBonus;	//用来记录在不同的计算条件下的最大bonus(不同的bonus意思各不相同)
	int				m_nYLTimes;		//盈利次数
	int				m_nKSTimes;		//亏损次数
	short			m_fParaValue[MAX_PARAMNUM];//最大收益时的各个参数取值
	int				m_nCurrentGPNo;	//当前股票序号
	int				m_nCurrTestZB;	//当前正在测试的指标
	BOOL			m_bSelectAll;	//当前是否处于全选状态
	BOOL			m_IsCarrying;	//正在执行的过程中(主要用于在强制关闭主程序时通知父关闭它
	BOOL			m_IsCarried;	//已执行完毕
	int				m_bSelected[MAXCOUNT];	//保存当前哪些指标被选中
	CImageList		m_ImageList;
	void			FillGPReport();	//设置各item的图标
	BOOL			PeekMyMsg(int TimeOut);	//消息循环,防止界面冻住
	void			DisableOther();
	void			EnableOther();
	void			CalcBonus(TINDEXINFO *pIndex,long ReadNum, int nType);
	void			CalcFormula0(float **OutData, long lReadNum, TINDEXINFO *pIndex);
	void			CalcFormula1(float **OutData, long lReadNum, TINDEXINFO *pIndex);
	void			CalcFormula2(float **OutData, long lReadNum, TINDEXINFO *pIndex);
	void			CalcFormula3(float **OutData, long lReadNum, TINDEXINFO *pIndex);
	void			CalcFormula4(float **OutData, long lReadNum, TINDEXINFO *pIndex);
	void			CalcFormula5(float **OutData, long lReadNum, TINDEXINFO *pIndex);
	void			CalcFormula6(float **OutData, long lReadNum, TINDEXINFO *pIndex);
	void			CalcFormula7(float **OutData, long lReadNum, TINDEXINFO *pIndex);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTZBDLG_H__C04AB602_071F_11D5_B99A_00E04C3DCAC5__INCLUDED_)
