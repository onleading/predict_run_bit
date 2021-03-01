#if !defined(AFX_MYDETAIL_H__B7E2A57D_C55B_45D0_B8B2_87D427D652A4__INCLUDED_)
#define AFX_MYDETAIL_H__B7E2A57D_C55B_45D0_B8B2_87D427D652A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyDetailDlg.h : header file
//
struct testDetail
{
	int   iGp;
	NTime nTime;
	short  iToDo;
	float fPrice;			
	long  lCount;			//交易量
	float fValue;			//现金利润
	float fProfit;			//盈利比例
	float fTopProfit;		//最大利润
	float fButtonProfit;	//最小利润
};
/////////////////////////////////////////////////////////////////////////////
// CMyDetailDlg dialog

class CMyDetailDlg : public CDialog
{
// Construction
public:
	CMyDetailDlg(CWnd* pParent = NULL);   // standard constructor

	vector<testDetail> m_testDetails;
	CString m_sName;
	CDialog *m_pParent;
// Dialog Data
	//{{AFX_DATA(CMyDetailDlg)
	enum { IDD = IDD_DETAIL };
	CListCtrl	m_detailList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyDetailDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMyDetailDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()	

private:
	CImageList m_ImageList;
	void FillDetail(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYDETAIL_H__B7E2A57D_C55B_45D0_B8B2_87D427D652A4__INCLUDED_)
