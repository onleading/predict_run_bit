// MyTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "MyTreeCtrl.h"
#include "MyInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyTreeCtrl
CMyTreeCtrl::CMyTreeCtrl(BOOL bOwnerDraw)
{
	m_bOwnerDraw=bOwnerDraw;
}

CMyTreeCtrl::~CMyTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CMyTreeCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemexpanding)
	ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMyTreeCtrl::SetBkMode(BOOL bOwnerDraw)
{
	m_bOwnerDraw=bOwnerDraw;
}

int CMyTreeCtrl::GetExpandTreeNodeNum(CMyTreeCtrl *pTreeCtrl) //最多支持三层树
{
	int totalnum = 0;
	HTREEITEM hRoot=TVI_ROOT,hChild=TVI_ROOT,hChildChild=TVI_ROOT;
	hRoot = pTreeCtrl->GetRootItem();
	while(hRoot)
	{
		totalnum++;
		if(pTreeCtrl->GetItemState(hRoot,TVIS_EXPANDED )&TVIS_EXPANDED )
		{
			hChild = pTreeCtrl->GetChildItem(hRoot);
			while (hChild)
			{
				totalnum++;
				if(pTreeCtrl->GetItemState(hChild,TVIS_EXPANDED )&TVIS_EXPANDED )
				{
					hChildChild = pTreeCtrl->GetChildItem(hChild);
					while (hChildChild)
					{
						totalnum++;
						hChildChild = pTreeCtrl->GetNextItem(hChildChild, TVGN_NEXT);
					}
				}
				hChild = pTreeCtrl->GetNextItem(hChild, TVGN_NEXT);
			}
		}
		hRoot = pTreeCtrl->GetNextItem(hRoot, TVGN_NEXT);
	}
	return totalnum;
}

int CMyTreeCtrl::GetTreeItemPos(CMyTreeCtrl *pTreeCtrl,HTREEITEM hItem) //最多支持三层树
{
	int pos = 0;
	HTREEITEM hRoot=TVI_ROOT,hChild=TVI_ROOT,hChildChild=TVI_ROOT;
	hRoot = pTreeCtrl->GetRootItem();
	while(hRoot)
	{
		if(hRoot==hItem)
			return pos;
		pos++;
		if(pTreeCtrl->GetItemState(hRoot,TVIS_EXPANDED )&TVIS_EXPANDED  )
		{
			hChild = pTreeCtrl->GetChildItem(hRoot);
			while (hChild)
			{
				if(hChild==hItem)
					return pos;
				pos++;
				if(pTreeCtrl->GetItemState(hChild,TVIS_EXPANDED )&TVIS_EXPANDED )
				{
					hChildChild = pTreeCtrl->GetChildItem(hChild);
					while (hChildChild)
					{
						if(hChildChild==hItem)
							return pos;
						pos++;
						hChildChild = pTreeCtrl->GetNextItem(hChildChild, TVGN_NEXT);
					}
				}
				hChild = pTreeCtrl->GetNextItem(hChild, TVGN_NEXT);
			}
		}
		hRoot = pTreeCtrl->GetNextItem(hRoot, TVGN_NEXT);
	}
	return 0;
}

HTREEITEM CMyTreeCtrl::GetTreeItem(CMyTreeCtrl *pTreeCtrl,int nPos) //最多支持三层树
{
	int totalnum = 0;
	HTREEITEM hRoot=TVI_ROOT,hChild=TVI_ROOT,hChildChild=TVI_ROOT;
	hRoot = pTreeCtrl->GetRootItem();
	while(hRoot)
	{
		if(totalnum==nPos)
			return hRoot;
		totalnum++;
		if(pTreeCtrl->GetItemState(hRoot,TVIS_EXPANDED )&TVIS_EXPANDED  )
		{
			hChild = pTreeCtrl->GetChildItem(hRoot);
			while (hChild)
			{
				if(totalnum==nPos)
					return hChild;
				totalnum++;
				if(pTreeCtrl->GetItemState(hChild,TVIS_EXPANDED )&TVIS_EXPANDED )
				{
					hChildChild = pTreeCtrl->GetChildItem(hChild);
					while (hChildChild)
					{
						if(totalnum==nPos)
							return hChildChild;
						totalnum++;
						hChildChild = pTreeCtrl->GetNextItem(hChildChild, TVGN_NEXT);
					}
				}
				hChild = pTreeCtrl->GetNextItem(hChild, TVGN_NEXT);
			}
		}
		hRoot = pTreeCtrl->GetNextItem(hRoot, TVGN_NEXT);
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CMyTreeCtrl message handlers
void CMyTreeCtrl::OnNcPaint() 
{
}

void CMyTreeCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
}

BOOL CMyTreeCtrl::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

//参数是:目标CDC,顶点坐标,要画成的尺寸,透明色,原CDC
//extern void DrawTransparencyDC(CDC *pDC,CPoint pt,CSize dSize,CDC *pSrc,CSize sSize,COLORREF rgbMask);

void CMyTreeCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	if(!m_bOwnerDraw)
	{
		DefWindowProc(WM_PAINT, (WPARAM)dc.m_hDC , 0);
		return;
	}
	CRect rcclient;
	GetClientRect(&rcclient);
	
	// create a compatible memory dc 
	CDC memdc;
	memdc.CreateCompatibleDC(&dc); 
	CBitmap bitmap,*pOldbm;
	bitmap.CreateCompatibleBitmap(&dc, rcclient.Width(), rcclient.Height());
	pOldbm=memdc.SelectObject( &bitmap );
	DefWindowProc(WM_PAINT, (WPARAM)memdc.m_hDC , 0);

	CDC maskdc;
	maskdc.CreateCompatibleDC(&dc);
	CBitmap maskbitmap,*pOldbm2;
	maskbitmap.CreateCompatibleBitmap(&dc, rcclient.Width(), rcclient.Height());
	pOldbm2=maskdc.SelectObject( &maskbitmap );

	DrawBKImage(&maskdc,rcclient);
	g_GUI.DrawTransparencyDC(&maskdc,CPoint(0,0),CSize(rcclient.Width(),rcclient.Height()),&memdc,CSize(rcclient.Width(),rcclient.Height()),RGB(255,255,255));

	dc.BitBlt(rcclient.left, rcclient.top, rcclient.Width(), rcclient.Height(), 
		&maskdc,rcclient.left, rcclient.top,SRCCOPY);
	memdc.SelectObject(pOldbm);
	bitmap.DeleteObject();
	maskdc.SelectObject(pOldbm2);
	maskbitmap.DeleteObject();
	maskdc.DeleteDC();
	memdc.DeleteDC();
}

void CMyTreeCtrl::DrawBKImage(CDC *pDC,CRect rc)
{
}

void CMyTreeCtrl::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	Invalidate();
	SetRedraw(TRUE);
	*pResult = 0;
}

void CMyTreeCtrl::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	SetRedraw(FALSE);
	*pResult = 0;
}

BOOL CMyTreeCtrl::SetBKImage(LPCTSTR LpszResource)
{
	if( m_bitmap.m_hObject != NULL )
		m_bitmap.DeleteObject();
	
	HBITMAP hbmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), 
		LpszResource, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION|LR_LOADFROMFILE);
	
	if( hbmp == NULL ) 
		return FALSE;
	
	m_bitmap.Attach( hbmp );
	
	return TRUE;
}

LRESULT CMyTreeCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch(message)
	{
	case WM_MOUSEWHEEL:
	case WM_VSCROLL:
	case WM_HSCROLL:
	    {
			LRESULT r=CTreeCtrl::WindowProc(message, wParam, lParam);
			SetRedraw(TRUE);
			return r;
	    }
	    break;
	case WM_PAINT:
		{
			::PostMessage(GetParent()->m_hWnd,UM_RESETSCROLLBAR,0,0);
		}
		break;
	}
	return CTreeCtrl::WindowProc(message, wParam, lParam);
}
