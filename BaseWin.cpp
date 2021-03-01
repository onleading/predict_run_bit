#include "stdafx.h"
#include "BaseWin.h"

BaseWin::BaseWin()
{
	SetRect(0,0,0,0);
}

BaseWin::~BaseWin()
{
}

void BaseWin::SetTitle(CString str)
{
}

int  BaseWin::Handle(UINT message, WPARAM wParam, LPARAM lParam)
{
	return -1;
}
