#include "stdafx.h"
#include "SafeComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSafeComboBox::CSafeComboBox(BOOL bSafeMode)
	: m_wndEdit(bSafeMode)
{
}

CSafeComboBox::~CSafeComboBox()
{
}

BOOL CSafeComboBox::Create(DWORD dwStyle,const RECT& rect,CWnd* pParentWnd,UINT nID)
{
	if(!CComboBox::Create(dwStyle,rect,pParentWnd,nID)) return FALSE;
	SubclassEditWindow();
	return TRUE;
}

BEGIN_MESSAGE_MAP(CSafeComboBox, CComboBox)
	//{{AFX_MSG_MAP(CSafeComboBox)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSafeComboBox::PreSubclassWindow() 
{
	CComboBox::PreSubclassWindow();

	SubclassEditWindow();
}

void CSafeComboBox::OnSelchange() 
{	
	CString strText;
	LONG nCurSel=GetCurSel();
	if(nCurSel<0) return;
	GetLBText(nCurSel,strText);
	m_wndEdit.SetTextSafe(strText);
	m_wndEdit.CleanString(&strText);
}

LRESULT CSafeComboBox::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch(message)
	{
	case CB_GETEDITSEL:
		if(::IsWindow(m_wndEdit.m_hWnd))
			m_wndEdit.SendMessage(EM_GETSEL,wParam,lParam);
		break;
	case CB_LIMITTEXT:
		if(::IsWindow(m_wndEdit.m_hWnd))
			m_wndEdit.SendMessage(EM_LIMITTEXT,wParam,lParam);
		break;
	case CB_SETEDITSEL:
		if(::IsWindow(m_wndEdit.m_hWnd))
			m_wndEdit.SendMessage(EM_SETSEL,wParam,lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		return 0;
		break;
	case WM_PAINT:
		{
			// 调用默认方法进行绘制
			CComboBox::DefWindowProc(message, wParam, lParam);

			// 在客户区覆盖背景色(矫正CComboBox不支持只读模式导致的界面绘制不规整)
			
			// 得到编辑框和当前空间位置
			if(m_wndEdit.IsReadOnly() && m_wndEdit.IsWindowEnabled())
			{
				CRect rcEdit;
				m_wndEdit.GetWindowRect(&rcEdit);
				ScreenToClient(&rcEdit);
				CRect rcClient;
				GetClientRect(&rcClient);
				rcClient.DeflateRect(GetSystemMetrics(SM_CXBORDER)+1,GetSystemMetrics(SM_CYBORDER)+1);
				rcClient.DeflateRect(0,0,GetSystemMetrics(SM_CXVSCROLL),0);

				// 重新绘制编辑区域内容
				InvalidateRect(NULL,FALSE);
				CRgn rgnClient,rgnEdit;
				rgnClient.CreateRectRgn(rcClient.left,rcClient.top,rcClient.right,rcClient.bottom);
				rgnEdit.CreateRectRgn(rcEdit.left,rcEdit.top,rcEdit.right,rcEdit.bottom);
				CPaintDC PaintDC(this);
				PaintDC.SelectClipRgn(&rgnClient,RGN_COPY);
				PaintDC.ExcludeClipRect(&rcEdit);
				PaintDC.FillSolidRect(&rcClient,GetSysColor(COLOR_3DFACE));
				m_wndEdit.RedrawWindow();
				ValidateRect(NULL);
			}
		}
		return 0;
		break;
	}
	return CComboBox::DefWindowProc(message, wParam, lParam);
}

VOID CSafeComboBox::SetExtendedStyle(DWORD dwExtendedStyle)
{	m_wndEdit.SetExtendedStyle(dwExtendedStyle);
	RedrawWindow();
}

BOOL CSafeComboBox::ModifyExtendedStyle(DWORD dwRemove,DWORD dwAdd,BOOL bRedraw)
{	BOOL bSuccess=m_wndEdit.ModifyExtendedStyle(dwRemove,dwAdd,bRedraw);
	RedrawWindow();
	return bSuccess;
}


VOID CSafeComboBox::SubclassEditWindow()
{
	// 如果没有子类化,得到子窗口进行子类化
	if(m_wndEdit.GetSafeHwnd()==NULL)
	{	HWND hEditCtrl=::GetWindow(m_hWnd,GW_CHILD);
		if(hEditCtrl!=NULL)
		{	m_wndEdit.SubclassWindow(hEditCtrl);
			::ShowWindow(hEditCtrl,SW_HIDE);
			::ShowWindow(hEditCtrl,SW_SHOW);
			DWORD dwExtendedStyle=m_wndEdit.GetExtendedStyle();
			m_wndEdit.SetExtendedStyle(dwExtendedStyle&(~ES_EX_HASBORDER));
			m_wndEdit.SetFeedBack(TRUE,NULL,0);
		}
	}
}
