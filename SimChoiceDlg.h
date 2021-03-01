#if !defined(AFX_SIMCHOICEDLG_H__7099A708_C9C3_11D3_8B1F_00E04C66B9E1__INCLUDED_)
#define AFX_SIMCHOICEDLG_H__7099A708_C9C3_11D3_8B1F_00E04C66B9E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SimChoiceDlg.h : header file
//

#include "Label.h"
#include "GradientProgressCtrl.h"

class CSimChoiceDlg : public CDialog
{
	void SetDomainStr();
	BOOL GetIndexValue(MemStkInfo *tmpInfo,int which,float &fValue);
	void FillList();
	void FillConsList(int whichsel);
	void CarryoutToBlock(int WhichBlock);
	void DisableOther();
	void EnableOther();
	BOOL IsMeetCon(MemStkInfo *tmpInfo,USER_CONS tmpCons,CurrStockData *pHQ);
public:
	CSimChoiceDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSimChoiceDlg)
	enum { IDD = IDD_SIMCHOICE_DIALOG };
	CGradientProgressCtrl	m_Carry_Progress;
	CLabel	m_CStatus;
	CStatic	m_SelectedNum;
	CStatic	m_TotalGPNum;
	CTabCtrl	m_ConTab;
	CListCtrl	m_ConList;
	CListBox	m_Condition_List;
	int		m_IsAndRadio;
	BOOL	m_bTQ;
	//}}AFX_DATA
	CImageList	m_ImageList;

	FILEHEAD FileHead;

	int		CurTabSel;
	int		CurListSel;
	
	int		TabListNum[3];

	short	awUsedStkp[MAXGPNUM];	//存放股票序号的数组
	short	SelectedStk[MAXGPNUM];  //成功找到的股票序号数组

	BOOL	IsCarrying;			//是否正在执行选股
	BOOL	IsCarried;

	struct GpDomain_Struct m_GpDomain[MAX_DOMAINNUM];
	int    m_nSelDomain;

	USER_CONS	m_UserCons[50];
	int			ConditionNum;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimChoiceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSimChoiceDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeContab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnModifycondition();
	afx_msg void OnAddcondition();
	afx_msg void OnDeletecondition();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnItemchangedConlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkConlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkConditionList();
	afx_msg void OnCarryoutBlock();
	afx_msg void OnCarryout();
	afx_msg void OnLoadcondition();
	afx_msg void OnSavecondition();
	afx_msg void OnChangedomainButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMCHOICEDLG_H__7099A708_C9C3_11D3_8B1F_00E04C66B9E1__INCLUDED_)
