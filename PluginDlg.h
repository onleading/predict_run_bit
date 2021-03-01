#if !defined(AFX_PLUGINDLG_H__5676A682_EB04_11D4_BB9D_00E04C66B1EB__INCLUDED_)
#define AFX_PLUGINDLG_H__5676A682_EB04_11D4_BB9D_00E04C66B1EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PluginDlg.h : header file
//

#include "plugin.h"
#include "Label.h"
#include "GradientProgressCtrl.h"

#define MAX_PLUGIN_NUMM 100

/////////////////////////////////////////////////////////////////////////////
// CPluginDlg dialog

class CPluginDlg : public CDialog
{
	void FillList();
	void SetDomainStr();
	void ShowHideParam(int ParamNum,int WhichIndex);
	void DisableOther();
	void EnableOther();
	void CarryoutToBlock(int WhichBlock);
public:
	CPluginDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPluginDlg)
	enum { IDD = IDD_PLUGIN_DIALOG };
	CGradientProgressCtrl m_Carry_Progress;
	CLabel	m_CStatus;
	CStatic	m_TotalGPNum;
	CStatic	m_SelectedNum;
	CEdit	m_Param1;
	CEdit	m_Param2;
	CEdit	m_Param3;
	CEdit	m_Param4;
	CSpinButtonCtrl	m_Param1_Spin;
	CSpinButtonCtrl	m_Param2_Spin;
	CSpinButtonCtrl	m_Param3_Spin;
	CSpinButtonCtrl	m_Param4_Spin;
	CStatic	m_Param1_Static;
	CStatic	m_Param2_Static;
	CStatic	m_Param3_Static;
	CStatic	m_Param4_Static;
	CListCtrl	m_Plugin_List;
	CComboBox	m_CalcCycle_Combo;
	BOOL	m_bTQ;
	CTime	m_BeginTime;
	CTime	m_EndTime;
	int		m_nDontUseRecentData;
	//}}AFX_DATA

private:
	PLUGIN	m_PluginInfo[MAX_PLUGIN_NUMM];
	int		m_nPluginNum;
	char	m_strPlugPath[MAX_PLUGIN_NUMM][_MAX_PATH];
	int		m_nCurSel;

	CImageList m_ImageList;

	short  awUsedStkp[MAXGPNUM];	//存放股票序号的数组
	short  SelectedStk[MAXGPNUM];   //成功找到的股票序号数组

	BOOL IsCarrying;
	BOOL IsCarried;

	struct GpDomain_Struct m_GpDomain[MAX_DOMAINNUM];
	int    m_nSelDomain;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPluginDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPluginDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDeltaposParam1Spin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposParam2Spin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposParam3Spin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposParam4Spin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusParam1();
	afx_msg void OnKillfocusParam2();
	afx_msg void OnKillfocusParam3();
	afx_msg void OnKillfocusParam4();
	afx_msg void OnCarryout();
	afx_msg void OnCarryoutBlock();
	afx_msg void OnItemchangedPluginList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangedomainButton();
	afx_msg void OnRefreshlistButton();
	afx_msg void OnPlughelpButton();
	afx_msg void OnSelchangeCalccycleCombo();
	afx_msg void OnRadio3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLUGINDLG_H__5676A682_EB04_11D4_BB9D_00E04C66B1EB__INCLUDED_)
