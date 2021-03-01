#ifndef CONTROL_H_
#define CONTROL_H_

#include "MySwitchBar.h"
#include "MyTreeCtrl.h"
#include "MyScrollBar.h"

extern BOOL InitColorfulSwitchBar(CWnd *pParentWnd,CMySwitchBar *pBar,UINT nCtrolID);
extern void SetTreeCtrlAppearance(CMyTreeCtrl*	pTreeCtrl,CMyScrollBar *pVScrollBar,COLORREF bColor,COLORREF fColor);

#endif