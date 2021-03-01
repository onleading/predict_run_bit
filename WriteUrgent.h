#if !defined(AFX_WRITEURGENT_H__5B96F85A_187C_4D71_89BE_AD06739D7C07__INCLUDED_)
#define AFX_WRITEURGENT_H__5B96F85A_187C_4D71_89BE_AD06739D7C07__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WriteUrgent.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWriteUrgent dialog

class CWriteUrgent : public CDialog
{
// Construction
public:
	CWriteUrgent(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWriteUrgent)
	enum { IDD = IDD_WRITEURGENT };
	CComboBox	m_GroupCombo;
	CRichEditCtrl	m_Content_Edit;
	CTime	m_time;
	CString	m_strSource;
	CString	m_Content;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWriteUrgent)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWriteUrgent)
	virtual BOOL OnInitDialog();
	afx_msg void OnImportfile();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WRITEURGENT_H__5B96F85A_187C_4D71_89BE_AD06739D7C07__INCLUDED_)
