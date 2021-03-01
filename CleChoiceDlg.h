#if !defined(AFX_CLECHOICEDLG_H__8C228581_74F1_11D4_BB99_00E04C66B1EB__INCLUDED_)
#define AFX_CLECHOICEDLG_H__8C228581_74F1_11D4_BB99_00E04C66B1EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CleChoiceDlg.h : header file
/////////////////////////////////////////////////////////////////////////////

#include "DragListCtrl.h"
#include "Label.h"

class CCleChoiceDlg : public CDialog
{
	void FillList();
	CString GetColStr(int colflag,MemStkInfo *tmpInfo);
	void FillHQ(BOOL bFirst = TRUE);
	void GetConGP(int TabSel,int ListSel,BOOL bInfo);
public:
	CCleChoiceDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCleChoiceDlg)
	enum { IDD = IDD_CLECHOICE_DIALOG };
	CLabel	m_GPNum_Static;
	CEdit	m_ConInfoEdit;
	CDragListCtrl	m_GPList;
	CTabCtrl	m_ConTab;
	CListCtrl	m_ConList;
	//}}AFX_DATA
	CImageList	m_ImageList;

	int		CurTabSel;
	int		CurListSel;
	int		CurGPSel;
	
	short	m_nSelGPIndex[MAXGPNUM];
	short	m_nSelNum;

	BOOL	bConInfo;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCleChoiceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCleChoiceDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSavetoblock();
	afx_msg void OnSelchangeContab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnConinfo();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnItemchangedConlist(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnTovip();
	afx_msg void OnItemchangedGplist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkGplist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLECHOICEDLG_H__8C228581_74F1_11D4_BB99_00E04C66B1EB__INCLUDED_)
