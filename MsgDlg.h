#if !defined(AFX_MSGDLG_H__05384283_987D_4E17_95AA_BF968AABB3D7__INCLUDED_)
#define AFX_MSGDLG_H__05384283_987D_4E17_95AA_BF968AABB3D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MsgDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMsgDlg dialog

#include "Label.h"
#include "resource.h"

#define MSG_NORMAL	0
#define MSG_RED		1
#define MSG_GREEN	2

class CMsgDlg : public CDialog
{
// Construction
public:
	CMsgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMsgDlg)
	enum { IDD = IDD_MSG_DIALOG };
	CLabel	m_Hint_Static;
	//}}AFX_DATA

	CString m_strCaption;
	CString	m_strMsg;
	int		m_nMsgFlag;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMsgDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSGDLG_H__05384283_987D_4E17_95AA_BF968AABB3D7__INCLUDED_)
