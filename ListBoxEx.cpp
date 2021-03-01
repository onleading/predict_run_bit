// ListBoxEx.cpp : implementation file
//

#include "stdafx.h"
#include "UBase.h"
#include "ListBoxEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListBoxEx

CListBoxEx::CListBoxEx()
{
	m_iMaxWidth=0;
	m_bIconListBox=false;
	m_iItemHeight=16;
	m_clrBackground=RGB(255,255,255);
	m_clrText=RGB(0,0,0);
 	m_clrFocused=RGB(0,0,180);
	m_clrSpecial=RGB(0,255,255);

	m_pUnit = NULL;
	m_aRegedMsg.clear();
}

CListBoxEx::~CListBoxEx()
{
	m_aRegedMsg.clear();
}


BEGIN_MESSAGE_MAP(CListBoxEx, CListBox)
	//{{AFX_MSG_MAP(CListBoxEx)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


void CListBoxEx::RegisterMsg(UINT message)
{
	for(int i=0;i<m_aRegedMsg.size()&&m_aRegedMsg[i]!=message;i++);
	if(i==m_aRegedMsg.size())
		m_aRegedMsg.push_back(message);
}

/////////////////////////////////////////////////////////////////////////////
// CListBoxEx message handlers

BOOL CListBoxEx::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{    
   if(zDelta<0)
   {
	   SendMessage(WM_KEYDOWN,(WPARAM)VK_DOWN);
	   SendMessage(WM_KEYUP,(WPARAM)VK_DOWN);
   }
   else
   {
		SendMessage(WM_KEYDOWN,(WPARAM)VK_UP,0);
		SendMessage(WM_KEYUP,(WPARAM)VK_UP,0);
   }
    return 0;
}

void CListBoxEx::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC dc;
	dc.Attach(lpDIS->hDC);
	
	CRect rectFull = lpDIS->rcItem;
	CRect rect = rectFull;
	UINT nIndex = lpDIS->itemID;
	char buf[256];
	GetText(lpDIS->itemID,buf);
	buf[255] = '\0';
	int nLen = strlen(buf);
	
   	int data=(int)lpDIS->itemData;
	if(lpDIS->itemState&ODS_SELECTED)
		dc.FillSolidRect(rect,m_clrFocused);
	else
		dc.FillSolidRect(rect,m_clrBackground);
	
	if(m_bIconListBox&&GetCount())
	{
		HICON h=m_ImageList.ExtractIcon(LOWORD(data));
		::DrawIconEx(dc,rect.left,rect.top+(rect.Height()-16)/2,h,16,16,0,0,DI_NORMAL);
		::DestroyIcon(h);
		rect.left+=18;
	}
	if(HIWORD(data)&0x1000) //特殊选项
		dc.SetTextColor(m_clrSpecial);
	else
		dc.SetTextColor(m_clrText);

 	dc.DrawText(buf,nLen,rect,DT_SINGLELINE|DT_VCENTER);
	
	if(lpDIS->itemState&ODS_FOCUS)
		dc.DrawFocusRect(rectFull);
	dc.Detach();	
}

void CListBoxEx::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
  	lpMeasureItemStruct->itemHeight=m_iItemHeight;
	 
}

void CListBoxEx::OnSize(UINT nType, int cx, int cy) 
{
	CListBox::OnSize(nType, cx, cy);
	if(::IsWindow(m_hWnd))
	{
		CRect rc; 
		GetClientRect(rc);
		if(rc.Width()>m_iMaxWidth)
		{
			m_iMaxWidth=rc.Width();
			SetHorizontalExtent(m_iMaxWidth);	
		}
	}     	
}

int CListBoxEx::AddString(LPCTSTR lpszItem)
{
   int count=GetCount();
   return InsertString(count,lpszItem);
}

int CListBoxEx::InsertString(int nIndex, LPCTSTR lpszItem)
{
   CFont *pOldFont,*pFont=GetFont();
   CDC *pDC=GetDC();
   pOldFont=pDC->SelectObject(pFont);
   CSize sz=pDC->GetTextExtent(lpszItem);
   sz.cx+=48;
   if(sz.cx>m_iMaxWidth)
   {
	   m_iMaxWidth=sz.cx;
	   SetHorizontalExtent(m_iMaxWidth);
   }
   pDC->SelectObject(pOldFont);
   ReleaseDC(pDC);
   return   CListBox::InsertString(  nIndex,lpszItem);
}

//附注，每一项相关的data的低16位表示其图标索引，高的16位则含义为，最低位各示为
//是否缩进，最高位各示是否总是以一种特别的颜色显示
void CListBoxEx::IndentItem(int index)
{
    DWORD data;
	data=GetItemData(index);
	if(data==LB_ERR)
	{
		data=0;
	}
 	data|=0x00010000;
	SetItemData(index,data);
}

void CListBoxEx::SetItemIcon(int itemindex, int iconindex)
{
     DWORD data;
	 data=GetItemData(itemindex);
	 if(data==LB_ERR)
	 {
		 data=0;
	 }
	 data+=iconindex;
	 SetItemData(itemindex,data);
}

void CListBoxEx::SetImageList(CImageList *pImage)
{
    m_bIconListBox=true;
	if(m_ImageList.GetSafeHandle())
		m_ImageList.DeleteImageList();
	m_ImageList.Create(pImage);
}

void CListBoxEx::SetItemHeight(int height)
{
   m_iItemHeight=height;
   if(::IsWindow(m_hWnd))
   {
	   int cnt=GetCount();
	   for(int i=0;i<cnt;i++)
	   {
		   CListBox::SetItemHeight(i,height);
	   }	    
   }
}

BOOL CListBoxEx::OnEraseBkgnd(CDC* pDC) 
{
    CRect rc;
	GetClientRect(rc);

	CBitmap bmp,*pbmpOld;
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC,rc.Width(),rc.Height());
	pbmpOld=memDC.SelectObject(&bmp);
 	memDC.FillSolidRect(rc,m_clrBackground);
	pDC->BitBlt(0,0,rc.Width(),rc.Height(),&memDC,0,0,SRCCOPY);
	memDC.SelectObject(pbmpOld);
	memDC.DeleteDC();
	bmp.DeleteObject();
	
	return TRUE;
}

void CListBoxEx::SetTextColor(COLORREF clr)
{
	m_clrText=clr;
}

void CListBoxEx::SetBKColor(COLORREF clr)
{ 
	m_clrBackground=clr;
}

void CListBoxEx::ResetContent()
{
	CRect rc;
	GetClientRect(rc);
	m_iMaxWidth=rc.Width();
	CListBox::ResetContent();
}


void CListBoxEx::SetFocusColor(COLORREF clr)
{
	m_clrFocused=clr;
}

void CListBoxEx::SetSpecialItem(int index)
{
    DWORD data;
	data=GetItemData(index);
	if(data==LB_ERR)
		data=0;
 	data|=0x10000000;
	SetItemData(index,data);
}

int CListBoxEx::GetItemHeight()
{
	return m_iItemHeight;
}

void CListBoxEx::SetSpecialItemColor(COLORREF clr)
{
	m_clrSpecial=clr;
}

void CListBoxEx::OnRButtonDown(UINT nFlags, CPoint point) 
{
	
	CListBox::OnRButtonDown(nFlags, point);
}

LRESULT CListBoxEx::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(m_pUnit)
	{
		for(int i=0;i<m_aRegedMsg.size();i++)
		{
			if(m_aRegedMsg[i]==message)
				return m_pUnit->ProcessMsg(message, wParam, lParam);
		}
	}
	if(message==WM_MOUSEMOVE) 
		return 0;
	
	return CListBox::WindowProc(message, wParam, lParam);
}
