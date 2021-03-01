// CaptionButton.cpp: implementation of the CCaptionButton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "CaptionButton.h"
#include "MyInterface.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCaptionButton::CCaptionButton()
{
	m_bShow=FALSE;
	m_pParent=NULL;
	m_rc.SetRect(0,0,0,0);
	m_state=0;
	m_bPressed=FALSE;
	m_type=0;
	m_bChecked=FALSE;
}

CCaptionButton::~CCaptionButton()
{
}

void CCaptionButton::Init(UINT IDB_BM,int nImage,COLORREF mask,int bttype,CWnd *pParent,BOOL bChecked,LPCSTR str)
{
	m_bShow=TRUE;
	m_pParent=pParent;
	m_type=bttype;
	if(bttype==0)
	{
		m_bChecked=FALSE;
		m_state=0;
	}
	else
	{
		m_bChecked=bChecked;
		m_state=m_bChecked?4:0;
	}
	CBitmap	tmpbm;
	HBITMAP hbm=::LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_BM));
	tmpbm.Attach(hbm);
	BITMAP bm;
	tmpbm.GetBitmap(&bm);
	m_IL.Create(bm.bmWidth/nImage,bm.bmHeight,ILC_COLOR32|ILC_MASK,nImage,nImage);
	m_IL.Add(&tmpbm,mask);
	m_IL.SetBkColor(mask);
	tmpbm.Detach();
	tmpbm.DeleteObject();
	DeleteObject(hbm);
	m_str=str;
}

void CCaptionButton::Draw()
{
	CDC *pDC=m_pParent->GetWindowDC();
	m_IL.Draw(pDC,m_state,CPoint(m_rc.left,m_rc.top),ILD_TRANSPARENT);
	CRect rc=m_rc;
	rc.DeflateRect(18,2,0,2);
	CFont btFont;
	btFont.CreateFont(12 ,0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DEFAULT_QUALITY,VARIABLE_PITCH|FF_MODERN,SONG_FONT);
	CFont *poldfont=pDC->SelectObject(&btFont);
	int oldmode=pDC->SetBkMode(TRANSPARENT);
	COLORREF textcor;
	if(g_GUI.IsShellType()==WinXP)
		textcor=RGB(255,255,255);
	else
		textcor=RGB(26,44,210);
	COLORREF oldcor=pDC->SetTextColor(textcor);
	pDC->DrawText(m_str,rc,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	pDC->SetBkMode(oldmode);
	pDC->SetTextColor(oldcor);
	pDC->SelectObject(poldfont);
	btFont.DeleteObject();
	m_pParent->ReleaseDC(pDC);
}

void CCaptionButton::SetRect(int x1,int y1,int x2,int y2)
{
	m_rc.SetRect(x1,y1,x2,y2);
}

void CCaptionButton::SetRect(CRect rc)
{
	m_rc=rc;
}

void CCaptionButton::OffsetRect(int x,int y)
{
	m_rc.OffsetRect(x,y);
}

BOOL CCaptionButton::IsInRect(CPoint pt)
{
	return m_rc.PtInRect(pt);
}

BOOL CCaptionButton::IsChecked()
{
	return m_bChecked;
}

void CCaptionButton::SetState(int n)
{
	switch(m_type)  
	{
	case 0:
	    {
			if(n<0 || n>3) return;
			m_state=n;
	    }
		break;
	case 1:
	    {
			if(n<0 || n>7) return;
			m_state=n+4*(m_bChecked?1:0);
	    }
	    break;
	}
}

int CCaptionButton::GetState()
{
	switch(m_type)  
	{
	case 0:
	    {
			return m_state;
	    }
		break;
	case 1:
	    {
			return m_bChecked?(m_state-4):m_state;	
	    }
	    break;
	}
	return 0;
}

void CCaptionButton::SetCheck(BOOL bCheck)
{
	int s=GetState();
	m_bChecked=bCheck;
	SetState(s);
}

void CCaptionButton::SetPressed(BOOL bPressed)
{
	m_bPressed=bPressed;
}

BOOL CCaptionButton::IsPressed()
{
	return m_bPressed;
}

void CCaptionButton::EnableWindow(BOOL en)
{
	if(!m_bShow || !m_pParent) return;
	if(en)
	{
		if(!IsEnable())
		{
			EraseBkgnd();
			SetState(0);
			((CCaptionButtonDlg*)m_pParent)->DrawCaptionButtons();
		}
	}
	else
	{
		if(IsEnable())
		{
			EraseBkgnd();
			SetState(3);
			((CCaptionButtonDlg*)m_pParent)->DrawCaptionButtons();
		}
	}
}
BOOL CCaptionButton::IsEnable()
{
	return (BOOL)(GetState()!=3);
}

void CCaptionButton::EraseBkgnd()
{
	if(!m_pParent) return;
	((CCaptionButtonDlg*)m_pParent)->m_drawbk=TRUE;
	((CCaptionButtonDlg*)m_pParent)->SendMessage(WM_NCPAINT,1,0);
	((CCaptionButtonDlg*)m_pParent)->m_drawbk=FALSE;
}

void CCaptionButton::ShowWindow(BOOL sw)
{
	if(!m_pParent) return;
	m_bShow=sw;
	EraseBkgnd();
	((CCaptionButtonDlg*)m_pParent)->DrawCaptionButtons();
}

BOOL CCaptionButton::IsShow()
{
	return m_bShow;
}

void CCaptionButton::SetWindowText(LPCTSTR str)
{
	m_str=str;
	if(!m_pParent) return;
	EraseBkgnd();
	((CCaptionButtonDlg*)m_pParent)->DrawCaptionButtons();
}


/////////////////////////////////////////////////////////////////////////////
// CCaptionButtonDlg dialog
IMPLEMENT_DYNAMIC(CCaptionButtonDlg,CDialog)

BEGIN_MESSAGE_MAP(CCaptionButtonDlg, CDialog)
	//{{AFX_MSG_MAP(CCaptionButtonDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CCaptionButtonDlg::CCaptionButtonDlg()
{
}
/*
CCaptionButtonDlg::CCaptionButtonDlg(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
{
	ASSERT(IS_INTRESOURCE(lpszTemplateName) || AfxIsValidString(lpszTemplateName));
	
	m_pParentWnd = pParentWnd;
	m_lpszTemplateName = lpszTemplateName;
	if (IS_INTRESOURCE(m_lpszTemplateName))
		m_nIDHelp = LOWORD((DWORD_PTR)m_lpszTemplateName);
}
*/
CCaptionButtonDlg::CCaptionButtonDlg(UINT nIDTemplate,CWnd* pParentWnd)
{
	m_pParentWnd = pParentWnd;
	m_lpszTemplateName = MAKEINTRESOURCE(nIDTemplate);
	m_nIDHelp = nIDTemplate;
}

/////////////////////////////////////////////////////////////////////////////
// CCaptionButtonDlg message handlers

BOOL CCaptionButtonDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_drawbk=FALSE;
//	InitCaptionButtons();

	return TRUE;
}

void CCaptionButtonDlg::GetCursorPosToWnd(CPoint &pt)
{
	CPoint ptCursor;
	GetCursorPos(&ptCursor);
	CRect rcWnd;
	GetWindowRect(&rcWnd);
	ptCursor.Offset(-rcWnd.left,-rcWnd.top);
	pt=ptCursor;
}

LRESULT CCaptionButtonDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message==WM_NCPAINT)
	{
		LRESULT rt= CDialog::WindowProc(message,wParam,lParam);
		LONG wl= GetWindowLong(m_hWnd,GWL_STYLE);
		if(!(wl & WS_CAPTION)) return rt;

		CRect rcWnd,rcButton;
		GetWindowRect(&rcWnd);
		
		//获取窗体的边框尺寸
		int xFrame,yFrame;
		if(wl & WS_THICKFRAME)
		{
			xFrame = GetSystemMetrics(SM_CXSIZEFRAME);
			yFrame = GetSystemMetrics(SM_CYSIZEFRAME);
		}
		else
		{
			xFrame = GetSystemMetrics(SM_CXFIXEDFRAME);
			yFrame = GetSystemMetrics(SM_CYFIXEDFRAME);
		}
		int yCaption=GetSystemMetrics(SM_CYCAPTION);
		//获取标题按钮的尺寸
		int xSize = GetSystemMetrics(SM_CXSIZE);
		int ySize = GetSystemMetrics(SM_CYSIZE);
	   //定义一个新的标题按钮（非系统按钮）
		int blank=(int)((yCaption-ySize)/2.+0.5);
		rcButton.SetRect(rcWnd.Width() - xFrame - xSize-2 ,
				yFrame+blank , rcWnd.Width()-xFrame-2, yFrame+ySize+blank);
		if(wl & WS_SYSMENU)
		{
			rcButton.OffsetRect(-(xSize),0);
			if(wl & WS_MINIMIZEBOX || wl & WS_MAXIMIZEBOX)
				rcButton.OffsetRect(-(xSize*2),0);
		}
		
		CRect tmprc=rcButton;
		tmprc.left=rcButton.right-xSize-7*m_bt1.m_str.GetLength();
		m_bt1.SetRect(tmprc);
		tmprc.right=tmprc.left-2;
		tmprc.left=tmprc.right-xSize-7*m_bt2.m_str.GetLength();
		m_bt2.SetRect(tmprc);
//		TRACE("--ncpaint %d\n",m_drawbk?1:0);
		if(!m_drawbk)
			DrawCaptionButtons();
		return rt;
	}
	if(message==WM_PAINT)
	{
		LRESULT rt= CDialog::WindowProc(message,wParam,lParam);
//		TRACE("--paint %d\n",m_drawbk?1:0);
		DrawCaptionButtons();
		return rt;
	}
	if(message==WM_NCACTIVATE)
	{
		LRESULT rt= CDialog::WindowProc(message,wParam,lParam);
		LONG wl= GetWindowLong(m_hWnd,GWL_STYLE);
		if(!(wl & WS_CAPTION)) return rt;
//		TRACE("--ncactivate %d\n",m_drawbk?1:0);
		if(!m_drawbk)
			DrawCaptionButtons();
		return rt;
	}
	if(message==WM_NCMOUSEMOVE)
	{
		LONG wl= GetWindowLong(m_hWnd,GWL_STYLE);
		if(!(wl & WS_CAPTION)) return CDialog::WindowProc(message,wParam,lParam);

		CPoint ptCursor;
		GetCursorPosToWnd(ptCursor);
		int nbt=0;
		if(m_bt1.IsShow() && m_bt1.IsInRect(ptCursor) && m_bt1.GetState()!=1 && m_bt1.IsEnable())
		{
			m_bt1.SetState(1);
			nbt=1;
			KillTimer(0xff-nbt);
			SetTimer(0xff-nbt,50,NULL);
		}
		if(m_bt2.IsShow() && m_bt2.IsInRect(ptCursor) && m_bt2.GetState()!=1 && m_bt2.IsEnable())
		{
			m_bt2.SetState(1);
			nbt=2;
			KillTimer(0xff-nbt);
			SetTimer(0xff-nbt,50,NULL);
		}
		if(nbt!=0)
		{
			m_drawbk=TRUE;
			SendMessage(WM_NCPAINT,1,0);
			m_drawbk=FALSE;
//			KillTimer(0xff-nbt);
//			SetTimer(0xff-nbt,50,NULL);
			DrawCaptionButtons();
			return 0;
		}
	}
	if(message==WM_NCLBUTTONDBLCLK)
	{
		LONG wl= GetWindowLong(m_hWnd,GWL_STYLE);
		if(!(wl & WS_CAPTION)) return CDialog::WindowProc(message,wParam,lParam);
		CPoint ptCursor;
		GetCursorPosToWnd(ptCursor);
		if((m_bt1.IsShow() && m_bt1.IsInRect(ptCursor)) || (m_bt2.IsShow() && m_bt2.IsInRect(ptCursor)))
			return 0;
	}
	if(message==WM_NCLBUTTONDOWN)
	{
		LONG wl= GetWindowLong(m_hWnd,GWL_STYLE);
		if(!(wl & WS_CAPTION)) return CDialog::WindowProc(message,wParam,lParam);
		CPoint ptCursor;
		GetCursorPosToWnd(ptCursor);
		int nbt=0;
		if(m_bt1.IsShow() && m_bt1.IsInRect(ptCursor) && m_bt1.GetState()!=2 && m_bt1.IsEnable())
		{
			nbt=1;
			m_bt1.SetState(2);
			m_bt1.SetPressed(TRUE);
		}
		if(m_bt2.IsShow() && m_bt2.IsInRect(ptCursor) && m_bt2.GetState()!=2 && m_bt2.IsEnable())
		{
			nbt=2;
			m_bt2.SetState(2);
			m_bt2.SetPressed(TRUE);
		}
		if(nbt!=0)
		{
			m_drawbk=TRUE;
			SendMessage(WM_NCPAINT,1,0);
			m_drawbk=FALSE;
			DrawCaptionButtons();
			SetCapture();
			return 0;
		}
	}
	if(message==WM_NCRBUTTONDOWN)
	{
		LONG wl= GetWindowLong(m_hWnd,GWL_STYLE);
		if(!(wl & WS_CAPTION)) return CDialog::WindowProc(message,wParam,lParam);
		CPoint ptCursor;
		GetCursorPosToWnd(ptCursor);
		if((m_bt1.IsShow() && m_bt1.IsInRect(ptCursor)) || (m_bt2.IsShow() && m_bt2.IsInRect(ptCursor)))
			return 0;
	}
	if(message==WM_MOUSEMOVE && wParam==MK_LBUTTON)
	{
		LONG wl= GetWindowLong(m_hWnd,GWL_STYLE);
		if(!(wl & WS_CAPTION)) return CDialog::WindowProc(message,wParam,lParam);
		CPoint ptCursor;
		GetCursorPosToWnd(ptCursor);
		int nbt=0;
		if(m_bt1.IsShow() && m_bt1.IsInRect(ptCursor) && m_bt1.IsPressed() && m_bt1.GetState()!=2 && m_bt1.IsEnable())
		{
			nbt=1;
			m_bt1.SetState(2);
			KillTimer(0xff-nbt);
			SetTimer(0xff-nbt,50,NULL);
		}
		if(m_bt2.IsShow() && m_bt2.IsInRect(ptCursor) && m_bt2.IsPressed() && m_bt2.GetState()!=2 && m_bt2.IsEnable())
		{
			nbt=2;
			m_bt2.SetState(2);
			KillTimer(0xff-nbt);
			SetTimer(0xff-nbt,50,NULL);
		}
		if(nbt!=0)
		{
//			KillTimer(0xff-nbt);
//			SetTimer(0xff-nbt,50,NULL);
			m_drawbk=TRUE;
			SendMessage(WM_NCPAINT,1,0);
			m_drawbk=FALSE;
			DrawCaptionButtons();
			return 0;
		}
	}
	if(message==WM_LBUTTONUP)
	{
		LONG wl= GetWindowLong(m_hWnd,GWL_STYLE);
		if(!(wl & WS_CAPTION)) return CDialog::WindowProc(message,wParam,lParam);
		ReleaseCapture();
		CPoint ptCursor;
		GetCursorPosToWnd(ptCursor);
		if(m_bt1.IsShow() && m_bt1.IsInRect(ptCursor) && m_bt1.IsPressed() && m_bt1.IsEnable())
		{
			m_bt1.SetPressed(FALSE);
//			if(m_bt1.m_type==1)
//				m_bt1.ChangeCheck();
			OnCaptionButton1();
			return 0;
		}
		if(m_bt2.IsShow() && m_bt2.IsInRect(ptCursor) && m_bt2.IsPressed() && m_bt2.IsEnable())
		{
			m_bt2.SetPressed(FALSE);
//			if(m_bt2.m_type==1)
//				m_bt2.ChangeCheck();
			OnCaptionButton2();
			return 0;
		}
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CCaptionButtonDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==0xff-1 || nIDEvent==0xff-2)	
	{
		CPoint ptCursor;
		GetCursorPosToWnd(ptCursor);
		int nbt=0;
		if(m_bt1.IsShow() && !m_bt1.IsInRect(ptCursor) && m_bt1.GetState()!=0 && m_bt1.IsEnable())
		{
			nbt=1;
			KillTimer(0xff-nbt);
			m_bt1.SetState(0);
		}
		if(m_bt2.IsShow() && !m_bt2.IsInRect(ptCursor) && m_bt2.GetState()!=0 && m_bt2.IsEnable())
		{
			nbt=2;
			KillTimer(0xff-nbt);
			m_bt2.SetState(0);
		}
		if(nbt!=0)
		{
//			KillTimer(nIDEvent-nbt);
			m_drawbk=TRUE;
			SendMessage(WM_NCPAINT,1,0);
			m_drawbk=FALSE;
			DrawCaptionButtons();
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CCaptionButtonDlg::DrawCaptionButtons()
{
	if(IsIconic()) return;
	if(m_bt1.IsShow())
		m_bt1.Draw();
	if(m_bt2.IsShow())
		m_bt2.Draw();
}
//////////////////////////////////////////////////////////////////////////
//虚函数
void CCaptionButtonDlg::OnCaptionButton1()
{
	TRACE("--CaptionButton1 be clicked!\n");
}

void CCaptionButtonDlg::OnCaptionButton2()
{
	TRACE("--CaptionButton2 be clicked!\n");
}

void CCaptionButtonDlg::InitCaptionButtons()
{
	TRACE("--If you see this message,you should Init CaptionButtons first!\n");
}
