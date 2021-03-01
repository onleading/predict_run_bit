#if !defined(AFX_INDEXSORTDLG_H__93B9EFE1_9DC9_11D3_B921_0000800104D5__INCLUDED_)
#define AFX_INDEXSORTDLG_H__93B9EFE1_9DC9_11D3_B921_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// indexsortdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIndexSortDlg dialog

#include "Label.h"
#include "DescFormDlg.h"
#include "interface.h"

class CIndexSortDlg : public CDialog
{
public:
	CIndexSortDlg(CWnd* pParent = NULL);   // standard constructor
	~CIndexSortDlg();

// Dialog Data
	//{{AFX_DATA(CIndexSortDlg)
	enum { IDD = IDD_INDEXSORT_DIALOG };
	CStatic	m_OutStatic;
	CListBox	m_OutLineListBox;
	CListCtrl	m_IndexList;
	CLabel	m_CStatus;
	CProgressCtrl	m_Carry_Progress;
	CComboBox	m_CalcCycle_Combo;
	//}}AFX_DATA
	CImageList m_ImageList;
	int m_lCurrentIndex;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIndexSortDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	// Generated message map functions
	//{{AFX_MSG(CIndexSortDlg)
	afx_msg void OnItemchangedIndexList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeCalccycleCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CDescFormDlg	m_Form;
	TINDEXINFO		*m_pIndex;
	void			FillOutLine(long index);
	int				*m_parPos;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INDEXSORTDLG_H__93B9EFE1_9DC9_11D3_B921_0000800104D5__INCLUDED_)
