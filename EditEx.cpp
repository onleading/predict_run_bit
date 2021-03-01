#include "stdafx.h"
#include "EditEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEditEx::CEditEx()
{
	m_nPermitStyle=PMTSTY_ANY;
}

CEditEx::~CEditEx()
{
}

void CEditEx::setPermitStyle(int style)
{
	m_nPermitStyle=style;
}

BEGIN_MESSAGE_MAP(CEditEx, CEdit)
	//{{AFX_MSG_MAP(CEditEx)
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(EN_UPDATE, OnUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CEditEx::OnChar(UINT nChar,UINT nRepCnt, UINT nFlags) 
{
	if(isprint(nChar))
	{	switch(m_nPermitStyle)
		{
		case PMTSTY_ANY:
			break;
		case PMTSTY_NUM:
			{	CString strPermitString="0123456789";
				if(strPermitString.Find((char)nChar)==-1) return;
			}
			break;
		case PMTSTY_NAME:
			{	CString strForbidString="~!@#$%^&*()_+|-=\\{}[]:\";'<>?,./";
				if(strForbidString.Find((char)nChar)!=-1) return;
			}
			break;
		case PMTSTY_FILENAME:
			{	CString strForbidString="~!@#$%^&*()_+|-=\\{}[]:\";'<>?,/";
				if(strForbidString.Find((char)nChar)!=-1) return;
			}
			break;
		case PMTSTY_FILEPATH:
			{	CString strForbidString="~!@#$%^&*()_+|-=\\{}[]:\";'<>?,";
				if(strForbidString.Find((char)nChar)!=-1) return;
			}
			break;
		case PMTSTY_HEX:
			{	CString strPermitString="0123456789ABCDEFabcdef";
				if(strPermitString.Find((char)nChar)==-1) return;
			}
			break;
		}
	}
	CEdit::OnChar(nChar,nRepCnt,nFlags);
}

void CEditEx::OnUpdate() 
{
	CString strWndTextBefore,strWndTextAfter;
	GetWindowText(strWndTextBefore);
	switch(m_nPermitStyle)
	{
	case PMTSTY_ANY:
		strWndTextAfter=strWndTextBefore;
		break;
	case PMTSTY_NUM:
		{	CString strPermitString="0123456789";
			strWndTextAfter=strWndTextBefore.SpanIncluding(strPermitString);
		}
		break;
	case PMTSTY_NAME:
		{	CString strForbidString="~!@#$%^&*()_+|-=\\{}[]:\";'<>?,./";
			strWndTextAfter=strWndTextBefore.SpanExcluding(strForbidString);
		}
		break;
	case PMTSTY_FILENAME:
		{	CString strForbidString="~!@#$%^&*()_+|-=\\{}[]:\";'<>?,/";
			strWndTextAfter=strWndTextBefore.SpanExcluding(strForbidString);
		}
		break;
	case PMTSTY_FILEPATH:
		{	CString strForbidString="~!@#$%^&*()_+|-=\\{}[]:\";'<>?,";
			strWndTextAfter=strWndTextBefore.SpanExcluding(strForbidString);
		}
		break;
	case PMTSTY_HEX:
		{	CString strPermitString="0123456789ABCDEFabcdef";
			strWndTextAfter=strWndTextBefore.SpanIncluding(strPermitString);
		}
		break;
	}
	if(strWndTextAfter!=strWndTextBefore)
	{	int nStartChar,nEndChar;
		GetSel(nStartChar,nEndChar);
		SetWindowText(strWndTextAfter);
		SetSel(nStartChar,nEndChar);
	}
}

