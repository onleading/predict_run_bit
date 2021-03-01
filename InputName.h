#if !defined(AFX_INPUTNAME_H__A277BCE2_5186_11D3_8C7B_0000800104D5__INCLUDED_)
#define AFX_INPUTNAME_H__A277BCE2_5186_11D3_8C7B_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputName.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputName dialog

class CInputName : public CDialog
{
// Construction
public:
	CInputName(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputName)
	enum { IDD = IDD_INPUTNAME };
	CEdit	m_NewNameEdit;
	CString	m_NewName;
	//}}AFX_DATA
	CString HintInfo;
	CString Caption;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputName)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputName)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTNAME_H__A277BCE2_5186_11D3_8C7B_0000800104D5__INCLUDED_)
