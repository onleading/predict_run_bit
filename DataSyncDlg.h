#if !defined(AFX_DATASYNCDLG_H__34E317FB_C3BE_4117_B9DE_BACEEF451FFE__INCLUDED_)
#define AFX_DATASYNCDLG_H__34E317FB_C3BE_4117_B9DE_BACEEF451FFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataSyncDlg.h : header file
//

#include "GradientProgressCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CDataSyncDlg dialog

class CDataSyncDlg : public CDialog
{
// Construction
public:
	CDataSyncDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDataSyncDlg)
	enum { IDD = IDD_DATASYNC_DLG };
	CGradientProgressCtrl	m_ctlProgress;
	//}}AFX_DATA

	int			m_nType;			//
	CString		m_strDataSyncFile;
	CString		m_strDesc;	
	short		m_nMainID;	//‘∂≥Ã π”√MainID


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataSyncDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	int		data_filelen ;
	int		data_pos ;
	char *	FileBuffer;
	char	m_strSyncFile[100];
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDataSyncDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnGetDataAck(WPARAM wParam,LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BOOL	TryDataReq(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATASYNCDLG_H__34E317FB_C3BE_4117_B9DE_BACEEF451FFE__INCLUDED_)
