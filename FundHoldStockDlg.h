#if !defined(AFX_FUNDHOLDSTOCKDLG_H__A781BEA0_F6C3_4DCC_8591_9813F811FBC6__INCLUDED_)
#define AFX_FUNDHOLDSTOCKDLG_H__A781BEA0_F6C3_4DCC_8591_9813F811FBC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FundHoldStockDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFundHoldStockDlg dialog
#include "SortListCtrl.h"
#include "PieChartCtrl.h"
#include "JJGrid.h"

typedef struct HOLD_FUNDINFO
{
	char szName[20];
	float fAmount;
	float fZGBRate;
	float fNetRate;
	float fLastAmount;
}HOLD_FUNDINFO;

class CFundHoldStockDlg : public CDialog
{
// Construction
private:
	vector<HOLD_FUNDINFO> vttmp;
	vector<CString> vtPoiName;
public:
	CFundHoldStockDlg(CWnd* pParent = NULL);   // standard constructor
	int m_nType;//OPENFUND,CLOSEDFUND,HOLDSTOCK
	int m_nFlag;

	pFUNDBASE m_pFundBase;	//���������Ϣ�ṹ
	pFUNDHOLDSTOCK m_pFundHoldStock; //����ֹɽṹ
	
	char m_lpszSymbol[7];//֤ȯ����
	float m_fTotalNet;//��ֵ�ܶ�
	int m_nFHSNum;	//����ֹɽṹ����
	int m_nFund;	//���������Ϣ�ṹ����
	CString m_strTitle;
// Dialog Data
	//{{AFX_DATA(CFundHoldStockDlg)
	enum { IDD = IDD_FUNDHOLDSTOCK };
	CSortListCtrl	m_list1;
	//}}AFX_DATA
	CImageList		m_ImageList;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFundHoldStockDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPieChartCtrl m_Pie;

	// Generated message map functions
	//{{AFX_MSG(CFundHoldStockDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void FillCtl(const char* lpszSymbol);
	void Initlist();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FUNDHOLDSTOCKDLG_H__A781BEA0_F6C3_4DCC_8591_9813F811FBC6__INCLUDED_)
