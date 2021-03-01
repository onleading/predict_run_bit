// CUShowMulInfoCtrlWnd.h: interface for the CUShowMulInfoCtrlWnd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUShowMulInfoCTRLWND_H__2B37D716_B577_4F4E_A84F_17B157541128__INCLUDED_)
#define AFX_CUShowMulInfoCTRLWND_H__2B37D716_B577_4F4E_A84F_17B157541128__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class UShowMulInfo;

class CUShowMulInfoCtrlWnd : public CWnd  
{
// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTxtView)
	//}}AFX_VIRTUAL

public:
	CUShowMulInfoCtrlWnd(UShowMulInfo *pUShowMulInfo);
	virtual ~CUShowMulInfoCtrlWnd();

	void	Resize();

private:
	UShowMulInfo	*m_pUShowMulInfo;
	BOOL	m_bLayoutFlag, m_bQuitting;

protected:
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnGetDataAck(WPARAM wParam,LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//{{AFX_MSG(CUShowMulInfoCtrlWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_CUShowMulInfoCTRLWND_H__2B37D716_B577_4F4E_A84F_17B157541128__INCLUDED_)
