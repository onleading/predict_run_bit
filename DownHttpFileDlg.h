#if !defined(AFX_DOWNHTTPFILEDLG_H__5381D950_565C_4433_8AFB_CBB9A862B583__INCLUDED_)
#define AFX_DOWNHTTPFILEDLG_H__5381D950_565C_4433_8AFB_CBB9A862B583__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Label.h"

class CDownHttpFileDlg : public CDialog
{
// Construction
public:
	CDownHttpFileDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDownHttpFileDlg)
	enum { IDD = IDD_DOWNHTTPFILE_DIALOG };
	CLabel	m_Info_Label;
	CLabel	m_Hint_Static;
	//}}AFX_DATA

	CString		m_strURL;
	CString		m_strDestPathFile;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDownHttpFileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON		m_hIcon;
	// Generated message map functions
	//{{AFX_MSG(CDownHttpFileDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif

