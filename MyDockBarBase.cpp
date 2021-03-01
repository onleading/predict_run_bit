// MyDockBarBase.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "MyDockBarBase.h"
#include "Func.h"
#include "MyInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyDockBarBase

CMyDockBarBase::CMyDockBarBase()
{
	memset(m_bt,0,sizeof(MyBitmapBt*)*MAX_BT);
	for(int i=0;i<MAX_BT;i++)
		m_bt_str[i]="";
	m_leftbuttondown=FALSE;
	m_dockpos=0;
	memset(m_TitleBTBK,0,sizeof(LPCTSTR)*9);
}

CMyDockBarBase::~CMyDockBarBase()
{
	for(int i=0;i<MAX_BT;i++)
	{
		TDEL(m_bt[i]);
	}
}

BEGIN_MESSAGE_MAP(CMyDockBarBase, CDialogBar)
	//{{AFX_MSG_MAP(CMyDockBarBase)
//	ON_WM_MOUSEMOVE()
//	ON_WM_LBUTTONDOWN()
//	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMyDockBarBase::NextBtRect(CRect &rc,int offset)
{
	switch(m_dockpos)  
	{
	case 0:
	case 2:
	    {
			rc.OffsetRect(offset,0);
	    }
		break;
	case 1:
	case 3:
	    {
			rc.OffsetRect(0,offset);
	    }
	    break;
	}
}

int CMyDockBarBase::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialogBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	CRect rc,rc2;
	GetWindowRect(&rc);
	GetParent()->ScreenToClient(&rc);

	switch(m_dockpos)  
	{
	case 0:
		{
			rc.bottom=rc.top+35;
		}
		break;
	case 2:
	    {
			rc.top=rc.bottom-35;
	    }
		break;
	case 1:
		{
			rc.left=rc.right-35;
		}
		break;
	case 3:
	    {
			rc.right=rc.left+35;
	    }
	    break;
	}
	MoveWindow(rc);
	HINSTANCE tmphi=AfxGetResourceHandle();
	AfxSetResourceHandle(g_resModule);
	switch(m_dockpos)  
	{
	case 0:
	    {
			m_TitleBTBK[2]=m_TitleBTBK[5]=m_TitleBTBK[8]="IDB_tBt_N_Background";
	    }
		break;
	case 1:
	    {
			m_TitleBTBK[2]=m_TitleBTBK[5]=m_TitleBTBK[8]="IDB_Bt_N_Background";
	    }
	    break;
	case 2:
		{

		}
		break;
	case 3:
		{

		}
		break;
	}

	GetClientRect(&rc2);
	switch(m_dockpos)  
	{
	case 0:
	case 2:
	    {
			rc2.top+=1;
			rc2.bottom-=1;
			rc2.left+=10;
			rc2.right=rc2.left+55;
	    }
		break;
	case 1:
	case 3:
	    {
			rc2.top+=10;
			rc2.left+=1;
			rc2.right-=1;
			rc2.bottom=rc2.top+33;
	    }
	    break;
	}
	CString tmpstr;
	for(int n=0;n<(int)m_nButton;n++)
	{
		m_bt[n]=new MyBitmapBt(ID_DOCKBAR_BT0+m_dockpos*MAX_BT+n,_F(m_bt_str[n]),TitleBt,DT_RIGHT|DT_VCENTER);
		tmpstr.Format("%s%d",m_nIDB_Bt_Center,n);
		m_bt[n]->SetCenterImage((LPCTSTR)tmpstr);
		m_bt[n]->SetBK(m_TitleBTBK);
		m_bt[n]->Create(NULL,"",WS_VISIBLE|WS_CHILD,rc2,this,ID_DOCKBAR_BT0+m_dockpos*MAX_BT+n);
		m_bt_rect[n]=rc2;
		switch(m_dockpos)  
		{
		case 0:
		case 2:
		    {
				m_bt[n]->SetTextCor(RGB(255,255,255));
				NextBtRect(rc2,70);
		    }
			break;
		case 1:
		case 3:
		    {
				NextBtRect(rc2,50);
		    }
		    break;
		}
	}
	AfxSetResourceHandle(tmphi);
	return 0;
}

BOOL CMyDockBarBase::OnEraseBkgnd(CDC* pDC) 
{
	pDC=GetWindowDC();
	for(int i=0;i<(int)m_nButton;i++)
	{
		if(m_bt[i]->IsWindowVisible())
			pDC->ExcludeClipRect(m_bt_rect[i]);
	}

	ModifyDC(pDC);

	CRect rc;
	CDC memdc;
	memdc.CreateCompatibleDC(pDC);
	GetClientRect(&rc);
	CBitmap membm;
	membm.CreateCompatibleBitmap(pDC,rc.Width(),rc.Height());
	CBitmap *poldbm=memdc.SelectObject(&membm);
	memdc.FillSolidRect(0,0,rc.Width(),rc.Height(),RGB(255,255,255));
	CRect rc2=rc;
	switch(m_dockpos)  
	{
	case 0:
		{
			CRect rc3=rc2;
			rc3.bottom=rc3.top+15;
			rc2.top=rc3.bottom;
			g_GUI.DrawGradient(&memdc,rc3,RGB(181,188,217),RGB(127,147,197),FALSE);
			g_GUI.DrawGradient(&memdc,rc2,RGB(108,134,185),RGB(158,187,217),FALSE);
		}
	case 2:
	    {
	    }
		break;
	case 1:
	    {
			rc2.DeflateRect(0,1,1,1);
			g_GUI.DrawGradient(&memdc,rc2,RGB(189,222,237),RGB(120,172,196),TRUE);
	    }
	    break;
	case 3:
		{
			rc2.DeflateRect(1,1,0,1);
			g_GUI.DrawGradient(&memdc,rc2,RGB(189,222,237),RGB(120,172,196),TRUE);
		}
		break;
	}
	pDC->BitBlt(0,0,rc.Width(),rc.Height(),&memdc,0,0,SRCCOPY);
	memdc.SelectObject(poldbm);
	memdc.DeleteDC();
	membm.DeleteObject();

	return TRUE;
}

LRESULT CMyDockBarBase::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message==UM_MYBITMAPBUTTON)
	{
		UINT n=(UINT)lParam-ID_DOCKBAR_BT0-m_dockpos*MAX_BT;
		if(n>=0 && n<m_nButton)
		{
			OnButton(n);
		}	
	}
	return CDialogBar::DefWindowProc(message, wParam, lParam);
}

BOOL CMyDockBarBase::Create(CWnd* pParentWnd, UINT nIDTemplate,UINT nStyle, UINT nID,UINT nButton,LPCTSTR nIDB_Bt0_Center)
{
	if(nStyle & CBRS_ALIGN_TOP)
		m_dockpos=0;
	else
	if(nStyle & CBRS_ALIGN_RIGHT)
		m_dockpos=1;
	else
	if(nStyle & CBRS_ALIGN_BOTTOM)
		m_dockpos=2;
	else
	if(nStyle & CBRS_ALIGN_LEFT)
		m_dockpos=3;
	m_nButton=nButton;
	m_nIDB_Bt_Center=nIDB_Bt0_Center;
	return CDialogBar::Create(pParentWnd,nIDTemplate,nStyle,nID);
}

void CMyDockBarBase::SetBtText(CString str[MAX_BT])
{
	for(int i=0;i<MAX_BT;i++)
	{
		m_bt_str[i]=str[i];
	}
}

void CMyDockBarBase::OnButton(UINT nIndex)
{
}

void CMyDockBarBase::ModifyDC(CDC *pDC)
{
}

