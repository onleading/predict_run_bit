// UF10Ctrl.cpp: implementation of the UF10Ctrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tdxw.h"
#include "UF10.h"
#include "UF10Ctrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UF10Ctrl::UF10Ctrl(LPCOMINFO pComInfo):UBaseCtrl(pComInfo)
{
	m_pUF10 = new UF10(pComInfo, this);
}

UF10Ctrl::~UF10Ctrl()
{
	TDEL(m_pUF10)
}

BOOL	UF10Ctrl::ShowUnit(HWND hVessel, int UnitType, char *errMsg)
{
	if(m_pUF10)
		return m_pUF10->InitShow(hVessel);
	return FALSE;
}

void	UF10Ctrl::ResizeUnit(int cx, int cy)
{
	if(m_pUF10)
		m_pUF10->ArrangeWnd(CRect(0, 0, cx, cy));
}

void	UF10Ctrl::GetDataAck(WPARAM wParam, LPARAM lParam)
{
	if(m_pUF10)
		m_pUF10->OnGetDataAck(wParam, lParam);
}

void	UF10Ctrl::SetStock(MemStkInfo *pStock)
{
	if(m_pUF10)
		m_pUF10->SetStock(pStock);
}
