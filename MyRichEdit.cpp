// MyRichEdit.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "MyRichEdit.h"
#include "TxtView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyRichEdit

CMyRichEdit::CMyRichEdit()
{
	m_menuIndex = 0;
	m_pFont = NULL;
	m_bUseTxtColor = TRUE;
}

CMyRichEdit::~CMyRichEdit()
{
	TDEL(m_pFont);
}

BEGIN_MESSAGE_MAP(CMyRichEdit, CRichEditCtrl)
	//{{AFX_MSG_MAP(CMyRichEdit)
	ON_WM_RBUTTONUP()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyRichEdit message handlers

void CMyRichEdit::SetMenuIndex(int index)
{
	m_menuIndex = index;
}

void CMyRichEdit::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if(m_menuIndex<0) 
	{
		CRichEditCtrl::OnRButtonUp(nFlags, point);
		return;
	}
	return;
	
	CNewMenu menu;
	menu.LoadMenu (IDR_RIGHTMENU);
//	menu.LoadToolBar(IDR_MAINFRAME);
	CNewMenu *pPopup=(CNewMenu *)menu.GetSubMenu(m_menuIndex);
	if(m_menuIndex == 3)	//如果是基本面的菜单，则动态添加菜单
	{
		for(int i = 0;i < g_TdxCfg.JbmNum;i++)
			pPopup->AppendMenu (MF_STRING,ID_JBM_MENU1+i,_F(g_TdxCfg.Jbmname[i]));
	}

	ClientToScreen(&point);
	pPopup->TrackPopupMenu (TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,AfxGetMainWnd());
	
	CRichEditCtrl::OnRButtonUp(nFlags, point);
}

void CMyRichEdit::ApplyTxtSize()
{
	SetRedraw(FALSE);

	TDEL(m_pFont);
	m_pFont = new CFont;
	m_pFont->CreateFont(g_nFontSize[2],0,0,0,g_nFontWeigth[2],0,0,0,(g_b2000XPSys&&stricmp(g_strFontName[2],"System")==0)?ANSI_CHARSET:DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,g_b2000XPSys?ANTIALIASED_QUALITY:DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,g_strFontName[2]);
	SetFont(m_pFont);

	SetReadOnly(TRUE);

	CHARFORMAT cf;
	GetDefaultCharFormat(cf);
	cf.dwMask = CFM_COLOR|CFM_CHARSET;
	cf.dwEffects = CFE_BOLD;
	cf.bCharSet = GB2312_CHARSET;

	if(m_bUseTxtColor)
	{
		cf.crTextColor = TxtColor.TxtForeColor ;
		SetBackgroundColor(FALSE,TxtColor.TxtBackColor);
	}
	else
	{
		cf.crTextColor = VipColor.VOLCOLOR;
		SetBackgroundColor(FALSE,VipColor.BACKCOLOR);
	}
	SetDefaultCharFormat(cf);
	SetRedraw(TRUE);
	Invalidate(FALSE);
}

BOOL CMyRichEdit::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);
	if(m_bUseTxtColor)
		pDC->FillSolidRect (&rect,TxtColor.TxtBackColor);
	else
		pDC->FillSolidRect (&rect,VipColor.BACKCOLOR);
	return TRUE;
}
