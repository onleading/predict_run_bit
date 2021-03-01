#if !defined(AFX_EXPORTEXTDLG_H__EAB921F1_1BC5_4EEF_9A7F_48FB39597F87__INCLUDED_)
#define AFX_EXPORTEXTDLG_H__EAB921F1_1BC5_4EEF_9A7F_48FB39597F87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExportExtDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExportExtDlg dialog

#include "GradientProgressCtrl.h"
#include "Label.h"

struct ExportExt_Habit
{
	char strPath[MAX_PATH];
	char strFileNameRule[100];
	int  nWhichSeparator;
	int  nWhichDataRule;
	BOOL bTQ;
	BOOL bMakeTitle;
};

class CExportExtDlg : public CDialog
{
	void FillGPReport();
	void MakeLineStr(CString &tmpLine,struct AnalyData *pTmp,CString &tmpSep,short xsflag);
public:
	CExportExtDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CExportExtDlg)
	enum { IDD = IDD_EXPORTEXT };
	CComboBox	m_Separator_Combo;
	CComboBox	m_DateRule_Combo;
	CString	m_strFileNameRule;
	CString	m_strPath;
	int		m_nWhichDataRule;
	int		m_nWhichSeparator;
	BOOL	m_bTQ;
	BOOL	m_bMakeTitle;
	//}}AFX_DATA
	CListCtrl	m_GpReport_List;
	CLabel		m_Hint_Static,m_Phase_Static;
	CGradientProgressCtrl	m_Progress;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExportExtDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CImageList	m_ImageList;
	int		AddedGPIndex[MAXGPNUM];
	int		AddedGPNum;
	BOOL	m_bTmpHas;
	long	m_lTmpStartDate;
	long	m_lTmpEndDate;
	BOOL	m_bCancel;
	// Generated message map functions
	//{{AFX_MSG(CExportExtDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnBrowse();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPORTEXTDLG_H__EAB921F1_1BC5_4EEF_9A7F_48FB39597F87__INCLUDED_)
