// MyPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "MyPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyPropertySheet

IMPLEMENT_DYNAMIC(CMyPropertySheet, CPropertySheet)

CMyPropertySheet::CMyPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CMyPropertySheet::CMyPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CMyPropertySheet::~CMyPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CMyPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CMyPropertySheet)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyPropertySheet message handlers

void CMyPropertySheet::OnPaint() 
{
	CPaintDC dc(this);
	CRect rc;
	GetClientRect(&rc);
	CFont m_fontLogo;
	m_fontLogo.CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE,0,0,0,0,0,0,SONG_FONT);
	CFont* OldFont = dc.SelectObject(&m_fontLogo);
	CString LogStr = g_strSysMark;
	dc.SetTextColor(RGB(80,80,80));
	dc.SetBkMode(TRANSPARENT);
	dc.TextOut(8,rc.bottom-24,LogStr);
	dc.SelectObject(OldFont);
}
