// UF10Ctrl.h: interface for the UF10Ctrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UF10CTRL_H__1416B497_74FA_4E16_A8BC_BEBCED6C8315__INCLUDED_)
#define AFX_UF10CTRL_H__1416B497_74FA_4E16_A8BC_BEBCED6C8315__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UBaseCtrl.h"

class UF10;
class UF10Ctrl : public UBaseCtrl
{
public:
	UF10Ctrl(LPCOMINFO pComInfo);
	virtual ~UF10Ctrl();

	BOOL	ShowUnit(HWND hVessel, int UnitType, char *errMsg);
	void	ResizeUnit(int cx, int cy);
	void	GetDataAck(WPARAM wParam, LPARAM lParam);
	void	SetStock(MemStkInfo *pStock);

private:
	UF10	*m_pUF10;
};

#endif // !defined(AFX_UF10CTRL_H__1416B497_74FA_4E16_A8BC_BEBCED6C8315__INCLUDED_)
