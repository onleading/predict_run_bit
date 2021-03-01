// UShowMulInfo.h: interface for the UShowMulInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USHOWMULINFO_H__861F4654_ADDD_4EEE_9AAE_2AC2E2F9CD62__INCLUDED_)
#define AFX_USHOWMULINFO_H__861F4654_ADDD_4EEE_9AAE_2AC2E2F9CD62__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UBase.h"

class UShowMulInfoCtrl;
class CUShowMulInfoCtrlWnd;
class CHtmlCtrl;

class UShowMulInfo
{
public:
	UShowMulInfo(LPCOMINFO pComInfo, UShowMulInfoCtrl *pCtrl);
	virtual ~UShowMulInfo();

	void	OnCreate();
	void	Resize();					
	BOOL	OnEraseBkgnd(CDC* pDC);	
	
	BOOL	InitShow(HWND hVessel);				
	void	ArrangeWnd(CRect &rc);	
	void	OnGetDataAck(WPARAM wParam,LPARAM lParam);

public:
	BOOL	m_bQuitting, m_bLayoutFlag;

private:
	UShowMulInfoCtrl		*m_pCtrl;
	CUShowMulInfoCtrlWnd	*m_pCtrlWnd;
	COMINFO			m_ComInfo;
	HWND			m_hVessel;

	HWND		m_hStatic;
	CHtmlCtrl	*m_pExplorer;
};

#endif // !defined(AFX_USHOWMULINFO_H__861F4654_ADDD_4EEE_9AAE_2AC2E2F9CD62__INCLUDED_)
