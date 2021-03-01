// TreeWnd.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "TreeWnd.h"
#include "KeyGuy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeWnd

CTreeWnd::CTreeWnd()
{
	KeyGuyFlag = FALSE;
}

CTreeWnd::~CTreeWnd()
{
}


BEGIN_MESSAGE_MAP(CTreeWnd, CWnd)
	//{{AFX_MSG_MAP(CTreeWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTreeWnd message handlers

BOOL CTreeWnd::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_CHAR && KeyGuyFlag == FALSE)
	{
		if(ProcessKeyGuy(pMsg,this,KeyGuyFlag))
			return TRUE;
	}
	
	return CWnd::PreTranslateMessage(pMsg);
}
