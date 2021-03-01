// TdxPassEdit.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "TdxPassEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTdxPassEdit

CTdxPassEdit::CTdxPassEdit()
{
	m_bWantGet=FALSE;
}

CTdxPassEdit::~CTdxPassEdit()
{
}


BEGIN_MESSAGE_MAP(CTdxPassEdit, CEdit)
	//{{AFX_MSG_MAP(CTdxPassEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTdxPassEdit message handlers

LRESULT CTdxPassEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if((message==WM_GETTEXT) ||(message==EM_GETLINE)) 
	{
		if(!m_bWantGet)
			return 0;
		m_bWantGet=FALSE;
	}	
	return CEdit::WindowProc(message, wParam, lParam);
}
