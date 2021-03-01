#if !defined(AFX_SENDRECFILEDLG_H__304CB353_7601_4D3D_BB6D_C20ED2AE7A23__INCLUDED_)
#define AFX_SENDRECFILEDLG_H__304CB353_7601_4D3D_BB6D_C20ED2AE7A23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendRecFileDlg.h : header file
//
#include "GradientProgressCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CSendRecFileDlg dialog

struct SendRec_Item
{
	char	ItemName[50];
	char	ItemPath[100];
	long	ItemFileSize;
};

#define MAX_SENDRECV_ITEM	20

class CSendRecFileDlg : public CDialog
{
// Construction
public:
	CSendRecFileDlg(CWnd* pParent = NULL);   // standard constructor
	short m_nFlag;	//标志，0：上传，1：下载
// Dialog Data
	//{{AFX_DATA(CSendRecFileDlg)
	enum { IDD = IDD_SENDREC_FILE };
	CStatic	m_static;
	CGradientProgressCtrl	m_progress;
	CListCtrl	m_listfile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendRecFileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void				GetDownList();
	
	struct SendRec_Item m_Item[MAX_SENDRECV_ITEM];
	int					m_ItemNum;
	
	char				m_strLoginName[50];
	char				m_strPassword[50];

	// Generated message map functions
	//{{AFX_MSG(CSendRecFileDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg void OnItemchangedFileList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDRECFILEDLG_H__304CB353_7601_4D3D_BB6D_C20ED2AE7A23__INCLUDED_)
