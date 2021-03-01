#if !defined(AFX_SELECTGPDLG_H__E0BB1104_7198_11D3_B921_0000800104D5__INCLUDED_)
#define AFX_SELECTGPDLG_H__E0BB1104_7198_11D3_B921_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectGPDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectGPDlg dialog

#include "DragListCtrl.h"

class CSelectGPDlg : public CDialog
{
	void FillGPTypeList();
	void FillGPList();
// Construction
public:
	CSelectGPDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectGPDlg)
	enum { IDD = IDD_SELECTGP_DIALOG };
	CTabCtrl	m_SelectGP_Tab;
	CListCtrl	m_GPType_List;
	CDragListCtrl	m_GP_List;
	//}}AFX_DATA
	int SelectedGPIndex[MAXGPNUM];
	int SelectedGPNum;

	BOOL bSingleSelect;
	int  bOverlapGpType; //0:一般 1:叠加 2:选择基准 3:选择组合 4:期货标的
	int  bCurDomain;

	MemStkInfo *m_pStkInfo;
	BOOL bHasDS;
private:
	CImageList m_ImageList1;
	CImageList m_ImageList2;
	int CurrentTabSel;
	int CurrentGPTypeSel;

	CString	BlockGPName[USERBLOCK_NUM+2]; //支持USERBLOCK_NUM个板块股和自选股、股票池

	BOOL KeyGuyFlag;

	STKRELATION  m_LinkData[100];
	int			 m_nLinkNum;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectGPDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectGPDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeSelectgpTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkGpList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedGptypeList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelectall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTGPDLG_H__E0BB1104_7198_11D3_B921_0000800104D5__INCLUDED_)
