// UWeb2.h: interface for the UWeb2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UWEB2_H__01AFD6C2_B2AC_4B7A_BB53_B89E5FE190A9__INCLUDED_)
#define AFX_UWEB2_H__01AFD6C2_B2AC_4B7A_BB53_B89E5FE190A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UBase.h"
#include "BtnST.h"

class CHtmlCtrl;
class UWeb2 : public UBase  
{
	BOOL			m_bCreated;
	CButtonST		m_btnOpen;

	CString			m_URL;
	BOOL			m_bGPRelation;
	int				m_nLastCurGP;
	
public:
	UWeb2(LPCOMINFO pComInfo);
	virtual ~UWeb2();

	void SetURL(CString tmpURL,BOOL bAtOnceRun,BOOL bMustGPRelation=FALSE);
	CString GetURL();

	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

private:
	void	Navigate();

public:
	int				m_nRefreshEscape;
private:
	HWND		m_hStatic;
	CHtmlCtrl	*m_pExplorer;

	int			m_nRefreshFlag;

};

#endif // !defined(AFX_UWEB2_H__01AFD6C2_B2AC_4B7A_BB53_B89E5FE190A9__INCLUDED_)
