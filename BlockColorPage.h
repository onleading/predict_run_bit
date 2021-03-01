#if !defined(AFX_BLOCKCOLORPAGE_H__D6B4FE30_2B56_43B3_9AA3_78C391CEF8FD__INCLUDED_)
#define AFX_BLOCKCOLORPAGE_H__D6B4FE30_2B56_43B3_9AA3_78C391CEF8FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BlockColorPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBlockColorPage dialog

class CBlockColorPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CBlockColorPage)

// Construction
public:
	CBlockColorPage();
	~CBlockColorPage();

// Dialog Data
	//{{AFX_DATA(CBlockColorPage)
	enum { IDD = IDD_BLOCKCOLOR_PAGE };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBlockColorPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
// Implementation
protected:
	void ChgBlock(int nBlockID,BOOL bDelete);
	void ChgColor(int nColorID);

	void FillName(int nBlockID = 0);
	void FillColor(int nColorID = 0);

protected:
	// Generated message map functions
	//{{AFX_MSG(CBlockColorPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLOCKCOLORPAGE_H__D6B4FE30_2B56_43B3_9AA3_78C391CEF8FD__INCLUDED_)
