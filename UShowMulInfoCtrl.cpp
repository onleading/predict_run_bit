// UShowMulInfoCtrl.cpp: implementation of the UShowMulInfoCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tdxw.h"
#include "UShowMulInfo.h"
#include "UShowMulInfoCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UShowMulInfoCtrl::UShowMulInfoCtrl(LPCOMINFO pComInfo):UBaseCtrl(pComInfo)
{
	m_pUShowMulInfo = new UShowMulInfo(pComInfo, this);
}

UShowMulInfoCtrl::~UShowMulInfoCtrl()
{
	TDEL(m_pUShowMulInfo)
}

BOOL	UShowMulInfoCtrl::ShowUnit(HWND hVessel, int UnitType, char *errMsg)
{
	if(m_pUShowMulInfo)
		return m_pUShowMulInfo->InitShow(hVessel);
	return FALSE;
}

void	UShowMulInfoCtrl::ResizeUnit(int cx, int cy)
{
	if(m_pUShowMulInfo)
		m_pUShowMulInfo->ArrangeWnd(CRect(0, 0, cx, cy));
}

void	UShowMulInfoCtrl::GetDataAck(WPARAM wParam, LPARAM lParam)
{
	if(m_pUShowMulInfo)
		m_pUShowMulInfo->OnGetDataAck(wParam, lParam);
}
