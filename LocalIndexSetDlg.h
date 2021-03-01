#if !defined(AFX_LOCALINDEXSETDLG_H__EB2B6C6A_AED8_4B42_B7C1_1EE84D07B530__INCLUDED_)
#define AFX_LOCALINDEXSETDLG_H__EB2B6C6A_AED8_4B42_B7C1_1EE84D07B530__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LocalIndexSetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLocalIndexSetDlg dialog

class CLocalIndexSetDlg : public CDialog
{
// Construction
public:
	CLocalIndexSetDlg(CWnd* pParent = NULL);   // standard constructor
	~CLocalIndexSetDlg();

// Dialog Data
	//{{AFX_DATA(CLocalIndexSetDlg)
	enum { IDD = IDD_LCINDEXSET_DIALOG };
	CComboBox	m_TQTypeCombo;
	CListCtrl	m_StockList;
	CString	m_Code;
	CString	m_Name;
	CTime	m_BaseDate;
	double	m_BaseVol;
	CString	m_Author;
	CString	m_Description;
	CString	m_Error;
	long	m_lGiveupDays;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLocalIndexSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	LCIDXSET	m_LcIdxSet;			//�Զ������Ϣ
	long		m_LcIdxNo;

	LPLCIDXSTK	m_pLcIdxStk;		//�Զ���ϳɷֹ���Ϣ
	long		m_LcIdxStkNum;		//�Զ���ϳɷֹɸ���
	long		m_LcIdxStkSpc;		//�Զ���ϳɷֹ���Ϣ��С(sizeof(LCIDXSTKINFO)Ϊ��λ)

	CImageList	m_ImageList;
// Implementation
public:
	void SetIdxSet(LPLCIDXSET pLcIdxSet = NULL,long lIndexNo = 0);

protected:
	void FillStockList();

	// Generated message map functions
	//{{AFX_MSG(CLocalIndexSetDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnLcaddstockButton();
	afx_msg void OnLcdelstockButton();
	afx_msg void OnLcclnstockButton();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOCALINDEXSETDLG_H__EB2B6C6A_AED8_4B42_B7C1_1EE84D07B530__INCLUDED_)
