// ColorButton.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "ColorButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorButton

CColorButton::CColorButton()
{
	m_color = 0x00ffffff;	//Ä¬ÈÏÎª°×É«
}

CColorButton::~CColorButton()
{
}


BEGIN_MESSAGE_MAP(CColorButton, CButton)
	//{{AFX_MSG_MAP(CColorButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorButton message handlers

void CColorButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC DC;
	DC.Attach(lpDrawItemStruct->hDC);

	CBrush *pOldBrush;
	CBrush brush(m_color);

	pOldBrush=DC.SelectObject(&brush);
	DC.Rectangle(&(lpDrawItemStruct->rcItem));
	DC.SelectObject(pOldBrush);
	brush.DeleteObject();
	DC.Detach();
}

COLORREF CColorButton::GetColor()
{
	return m_color;
}

COLORREF CColorButton::SetColor(COLORREF crf)
{
	COLORREF tmpcrf = m_color;
	m_color = crf;
	return tmpcrf;
}

void CColorButton::Refresh()
{
	Invalidate();
}