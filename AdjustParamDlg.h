#if !defined(AFX_ADJUSTPARAMDLG_H__90EE2320_1210_11D5_B99A_00E04C3DCAC5__INCLUDED_)
#define AFX_ADJUSTPARAMDLG_H__90EE2320_1210_11D5_B99A_00E04C3DCAC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdjustParamDlg.h : header file
//

#include "DescFormDlg.h"
#include "interface.h"
/////////////////////////////////////////////////////////////////////////////
// CAdjustParamDlg dialog

class CAdjustParamDlg : public CDialog
{
// Construction
public:
	CAdjustParamDlg(CWnd* pParent = NULL);   // standard constructor
	void Init(int nSet, long lIndex, int nPeriod, CWnd *pCurrView);

// Dialog Data
	//{{AFX_DATA(CAdjustParamDlg)
	enum { IDD = IDD_ADJUSTPARAM };
	CStatic	m_OutStatic;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdjustParamDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAdjustParamDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDelbtn();
	afx_msg void OnResumebtn();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int		m_nSet;
	long	m_lIndex;
	int		m_nPeriod;
	TINDEXINFO *m_pIndex;
	CDescFormDlg m_Form;
	CWnd	*m_pView;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADJUSTPARAMDLG_H__90EE2320_1210_11D5_B99A_00E04C3DCAC5__INCLUDED_)
