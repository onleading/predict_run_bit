// UShowMulInfoCtrl.h: interface for the UShowMulInfoCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UShowMulInfoCTRL_H__1416B497_74FA_4E16_A8BC_BEBCED6C8315__INCLUDED_)
#define AFX_UShowMulInfoCTRL_H__1416B497_74FA_4E16_A8BC_BEBCED6C8315__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UBaseCtrl.h"

class UShowMulInfo;
class UShowMulInfoCtrl : public UBaseCtrl
{
public:
	UShowMulInfoCtrl(LPCOMINFO pComInfo);
	virtual ~UShowMulInfoCtrl();

	BOOL	ShowUnit(HWND hVessel, int UnitType, char *errMsg);
	void	ResizeUnit(int cx, int cy);
	void	GetDataAck(WPARAM wParam, LPARAM lParam);

private:
	UShowMulInfo	*m_pUShowMulInfo;
};

#endif // !defined(AFX_UShowMulInfoCTRL_H__1416B497_74FA_4E16_A8BC_BEBCED6C8315__INCLUDED_)
