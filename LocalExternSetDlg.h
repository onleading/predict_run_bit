#if !defined(AFX_LOCALEXTERNSETDLG_H__6AC181DE_5335_4BCD_8C73_6CB106F1F774__INCLUDED_)
#define AFX_LOCALEXTERNSETDLG_H__6AC181DE_5335_4BCD_8C73_6CB106F1F774__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LocalExternSetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLocalExternSetDlg dialog

class CLocalExternSetDlg : public CDialog
{
// Construction
public:
	CLocalExternSetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLocalExternSetDlg)
	enum { IDD = IDD_LCEXTERNSET_DIALOG };
	CComboBox	m_VolRatio;
	CComboBox	m_LcExtFormat9;
	CComboBox	m_LcExtFormat8;
	CComboBox	m_LcExtFormat7;
	CComboBox	m_LcExtFormat6;
	CComboBox	m_LcExtFormat5;
	CComboBox	m_LcExtFormat4;
	CComboBox	m_LcExtFormat3;
	CComboBox	m_LcExtFormat2;
	CComboBox	m_LcExtFormat10;
	CComboBox	m_LcExtFormat1;
	CComboBox	m_SpaceMark;
	CComboBox	m_DateFormat;
	CString	m_LcExtPath;
	CString	m_LcExtCode;
	CString	m_LcExtName;
	long	m_IgnoreLine;
	CString	m_Error;
	//}}AFX_DATA

public:
	BOOL		m_bInited;
	LCEXTSET	m_LcExtSet;			//自定组合信息
	long		m_LcExtNo;

public:
	void SetExtSet(LPLCEXTSET pLcExtSet = NULL,long lExtNo = 0);

protected:
	void SetFormat(CComboBox &LcExtFmtBox,char (*BoxStr)[12],long lStrNum,long lDefault = 0);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLocalExternSetDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLocalExternSetDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonPath();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOCALEXTERNSETDLG_H__6AC181DE_5335_4BCD_8C73_6CB106F1F774__INCLUDED_)
