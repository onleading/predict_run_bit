// ColorListBox.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "ColorListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorListBox

CColorListBox::CColorListBox()
{
	m_ItemColorValue[0] = RGB(0,0,0);
	m_ItemColorValue[1] = RGB(64,0,128);
	m_ItemColorValue[2] = RGB(255,0,0);
	m_ItemColorValue[3] = RGB(0,0,255);
	m_ItemColorValue[4] = RGB(0,0,128);
	m_ItemColorValue[5] = RGB(128,0,0);
	m_nHeight = 19;
}

CColorListBox::~CColorListBox()
{
}


BEGIN_MESSAGE_MAP(CColorListBox, CListBox)
	//{{AFX_MSG_MAP(CColorListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorListBox message handlers

void CColorListBox::SetCustomHeight(int nHeight)
{
	m_nHeight = nHeight;
}

void CColorListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	int kind = lpDrawItemStruct->itemData;
	if(kind < 0 || kind > 5) kind = 0;
	int CurrItem = lpDrawItemStruct->itemID;
	if (CurrItem < 0) return;
	char buf[80];
	GetText(CurrItem, buf);
	int nLen = strlen(buf);

	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	dc.SetTextColor(m_ItemColorValue[kind]);
	if((lpDrawItemStruct->itemAction | ODA_SELECT) &&
	   (lpDrawItemStruct->itemState & ODS_SELECTED))
	{
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, RGB(0,0,128));
		dc.SetTextColor(RGB(255,255,0));//选中时文字用高亮色
	}
	else
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, ::GetSysColor(COLOR_WINDOW));
	lpDrawItemStruct->rcItem.top+=2;
	lpDrawItemStruct->rcItem.bottom+=2;
	dc.DrawText(buf, nLen, &lpDrawItemStruct->rcItem, DT_VCENTER|DT_SINGLELINE);
	dc.Detach();
}

void CColorListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemHeight = m_nHeight;
}
