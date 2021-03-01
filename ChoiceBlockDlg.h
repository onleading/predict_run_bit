#if !defined(AFX_CHOICEBLOCKDLG_H__FD97F4D2_D5DF_44B7_8F20_F654070A796E__INCLUDED_)
#define AFX_CHOICEBLOCKDLG_H__FD97F4D2_D5DF_44B7_8F20_F654070A796E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChoiceBlockDlg.h : header file
//
#include "DescFormDlg.h"
#include "DragListCtrl.h"
#include "GradientProgressCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CChoiceBlockDlg dialog
class CChoiceBlockDlg : public CDialog
{
// Construction
public:
	CChoiceBlockDlg(CWnd* pParent = NULL);   // standard constructor
	
	void InsertType(HTREEITEM hFatherItem, long nTypeNo);
	void FillTree();
	long	m_lPos;				//当前指标在相应集合中的位置
	float   m_curParams[MAX_PARAMNUM];
// Dialog Data
	//{{AFX_DATA(CChoiceBlockDlg)
	enum { IDD = IDD_CHOICEBLOCKDLG };
	CTreeCtrl	m_tree;
	CStatic	m_OutStatic;
	CListBox	m_Condition_List;
	CComboBox	m_CalcCycle_Combo;
	BOOL	m_bQJ;
	CTime	m_BeginTime;
	CTime	m_EndTime;
	CGradientProgressCtrl	m_Carry_Progress;
	BOOL	m_bTimerRefresh;
	//}}AFX_DATA

	int CurrentIndex;
	CONDITIONINFO ConditionInfo[MAX_TJCOMB_NUM];	//允许最多MAX_TJCOMB_NUM个条件的组合

	TINDEXINFO	*pIndex[MAX_TJCOMB_NUM];
	int ConditionNum;

	BOOL IsCarrying;
	BOOL IsCarried;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChoiceBlockDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChoiceBlockDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddcondition();
	afx_msg void OnDeletecondition();
	afx_msg void OnSelchangeCalccycleCombo();
	afx_msg void OnQjCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void OnLoadcondition();

private:
	int		m_nSet;
	int		m_nPeriod;
	CDescFormDlg m_Form;
	CImageList m_ImageList;
	LPCTSTR m_lpszCurrActive;

	FILEHEAD FileHead;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHOICEBLOCKDLG_H__FD97F4D2_D5DF_44B7_8F20_F654070A796E__INCLUDED_)
