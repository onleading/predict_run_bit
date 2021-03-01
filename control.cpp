#include "StdAfx.h"
#include "control.h"

BOOL InitColorfulSwitchBar(CWnd *pParentWnd,CMySwitchBar *pBar,UINT nCtrolID)
{
	long bk_white[12]={ IDB_MYSWITCHBAR_L_N,IDB_MYSWITCHBAR_R_N,IDB_MYSWITCHBAR_BK_N,
						IDB_MYSWITCHBAR_L_D,IDB_MYSWITCHBAR_R_D,IDB_MYSWITCHBAR_BK_D,
						IDB_MYSWITCHBAR_L_O,IDB_MYSWITCHBAR_R_O,IDB_MYSWITCHBAR_BK_O,
						0,0,IDB_MYSWITCHBAR_BK };
	long bk[12] =	  {	IDB_MYSWITCHBAR_L_D,IDB_MYSWITCHBAR_R_D,IDB_MYSWITCHBAR_BK_D,
						IDB_MYSWITCHBAR_L_N,IDB_MYSWITCHBAR_R_N,IDB_MYSWITCHBAR_BK_N,
						IDB_MYSWITCHBAR_L_O,IDB_MYSWITCHBAR_R_O,IDB_MYSWITCHBAR_BK_O,
						0,0,IDB_MYSWITCHBAR_BK };
	long bk_bt[6]=	  {	IDB_MYSWITCHBAR_BT_H_LEFT,IDB_MYSWITCHBAR_BT_H_LEFT,IDB_MYSWITCHBAR_BT_H_LEFT,
						IDB_MYSWITCHBAR_BT_H_RIGHT,IDB_MYSWITCHBAR_BT_H_RIGHT,IDB_MYSWITCHBAR_BT_H_RIGHT };

	pBar->Init(nCtrolID,MSBS_H_T,0,0);
	pBar->SetItemOffset(10);
	if(g_d.IsBlackColorScheme(VipColor.BACKCOLOR))
		pBar->SetBK(bk);
	else
		pBar->SetBK(bk_white);
	pBar->SetButtonBK(bk_bt);

	COLORREF SwitchBakColor=VipColor.BACKCOLOR;
	COLORREF TextColor_N=RGB(0,0,0);
	COLORREF TextColor_D=RGB(255,255,255);
	if(g_d.IsBlackColorScheme(VipColor.BACKCOLOR))
	{
		TextColor_N=RGB(255,255,255);
		TextColor_D=RGB(0,0,0);
	}
	pBar->SetBkColor(SwitchBakColor,g_GUI.DarkenScaleColor(50,SwitchBakColor),g_GUI.LightenScaleColor(50,SwitchBakColor));
	pBar->SetTextColor(TextColor_N,TextColor_D,0);
	
	return pBar->Create(NULL,"",WS_VISIBLE|WS_CHILD|WS_TABSTOP,CRect(0,0,0,0),pParentWnd,nCtrolID);
}

void SetTreeCtrlAppearance(CMyTreeCtrl*	pTreeCtrl,CMyScrollBar *pVScrollBar,COLORREF bColor,COLORREF fColor) 
{
	pTreeCtrl->SetBkColor (bColor);
	if(pVScrollBar)
	{
#ifdef OEM_NEWJY
		COLORREF tmpColor[10];
		for(int i=0;i<10;i++)
			tmpColor[i]=g_GUI.GetColorCfgFromResource(g_resModule,"IDR_COLOR","Tree_MyScrollBar",i);
		pVScrollBar->SetScrollColor(tmpColor);
#else
		pVScrollBar->SetScrollColor(RGB(128,128,128),1,bColor,RGB(128,128,128));
#endif
	}
	pTreeCtrl->SetTextColor (fColor);
}

