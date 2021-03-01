#if !defined(AFX_ALLTESTMAPDLG_H__0E1C773A_2923_412A_BFA1_8C67201B304D__INCLUDED_)
#define AFX_ALLTESTMAPDLG_H__0E1C773A_2923_412A_BFA1_8C67201B304D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AllTestMapDlg.h : header file
//
#pragma warning(disable:4786)
#pragma warning(disable:4788)
#include <map>
#include <algorithm>
//#include <functional>
using namespace std;

#include "MyDetailDlg.h" 

class CTestYieldWnd;
/////////////////////////////////////////////////////////////////////////////
// CAllTestMapDlg dialog

class CAllTestMapDlg : public CDialog
{
// Construction
public:
	CAllTestMapDlg(CWnd* pParent = NULL);   // standard constructor
	~CAllTestMapDlg();

// Dialog Data
	//{{AFX_DATA(CAllTestMapDlg)
	enum { IDD = IDD_ALLTESTMAPDLG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAllTestMapDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CTestYieldWnd *m_pDrawWnd;
	// Generated message map functions
	//{{AFX_MSG(CAllTestMapDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	map<long, testDetail, less_equal<long> > m_aTestData;
	vector<testDetail> m_sTestData;
	CArray<testDetail,testDetail&> m_aSDetails;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALLTESTMAPDLG_H__0E1C773A_2923_412A_BFA1_8C67201B304D__INCLUDED_)
