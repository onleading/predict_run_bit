#if !defined(AFX_SELECTONEDLG_H__40621FBB_4629_41E7_977B_0D5E663B7BE9__INCLUDED_)
#define AFX_SELECTONEDLG_H__40621FBB_4629_41E7_977B_0D5E663B7BE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectOneDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectOneDlg dialog

struct tag_SelectItem
{
	char ItemStr[255];
};

class CSelectOneDlg : public CDialog
{
// Construction
public:
	CSelectOneDlg(CWnd* pParent = NULL);   // standard constructor
	void AddString(CString tmpStr);
// Dialog Data
	//{{AFX_DATA(CSelectOneDlg)
	enum { IDD = IDD_SELECTONE_DIALOG };
	CListBox	m_List;
	CString	m_strHintStr;
	//}}AFX_DATA

	int  m_nCurSel;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectOneDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	vector<tag_SelectItem> m_Item;
	// Generated message map functions
	//{{AFX_MSG(CSelectOneDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkList1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTONEDLG_H__40621FBB_4629_41E7_977B_0D5E663B7BE9__INCLUDED_)
