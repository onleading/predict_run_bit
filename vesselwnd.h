#if !defined(AFX_VESSELWND_H__20CCA960_DBF9_433B_AD25_ABD81C4A62C6__INCLUDED_)
#define AFX_VESSELWND_H__20CCA960_DBF9_433B_AD25_ABD81C4A62C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// vesselwnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVesselWnd window
class UBaseInner;
class CVesselWnd : public CWnd
{
	void PaintBK();
public:
	CVesselWnd(UBaseInner *pUnit,int nUnitType=0);

private:
	UBaseInner *m_pUnit;
	int m_nUnitType;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVesselWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVesselWnd();
	
	void PaintVessel();
	// Generated message map functions
protected:
	//{{AFX_MSG(CVesselWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VESSELWND_H__20CCA960_DBF9_433B_AD25_ABD81C4A62C6__INCLUDED_)
