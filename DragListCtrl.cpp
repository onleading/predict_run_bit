// DragListCtrl.cpp : implementation file

#include "stdafx.h"
#include "tdxw.h"
#include "DragListCtrl.h"
#include "ComView.h"
#include "TxtView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDragListCtrl

CDragListCtrl::CDragListCtrl()
{
	m_pDragImage = NULL;
	m_bDragging = FALSE;
	m_pDropWnd = NULL;

	m_bGP = TRUE;
}

void CDragListCtrl::SetDragType(BOOL bGP)
{
	m_bGP = bGP;
}

CDragListCtrl::~CDragListCtrl()
{
	if(m_pDragImage)
		delete m_pDragImage;
}


BEGIN_MESSAGE_MAP(CDragListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CDragListCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDragListCtrl message handlers

int CDragListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CDragListCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	m_nDragIndex = pNMListView->iItem;

	DraggingGPNo = GetItemData(m_nDragIndex);
	if(m_pDragImage)	  
		delete m_pDragImage;
	CPoint pt(8,8);	
	m_pDragImage = CreateDragImage (m_nDragIndex, &pt);
	SetCursor(m_hCannotDrop);
	m_pDragImage->BeginDrag (0, CPoint (8, 8));
	m_pDragImage->DragEnter (GetDesktopWindow (), pNMListView->ptAction);

	m_bDragging = TRUE;
	SetCapture ();
	
	*pResult = 0;
}

void CDragListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		CPoint pt(point);
		ClientToScreen (&pt);
		m_pDragImage->DragMove (pt);

		CWnd * pWnd,*pChild = NULL;
		pWnd = WindowFromPoint(pt);//注意如果是当前对话框

		while(pWnd)
		{
			pWnd->ScreenToClient(&pt);
			pChild = pWnd->ChildWindowFromPoint(pt);
			if(pChild && pChild != pWnd && pChild != this) // 如果有子窗口，则继续下一层
			{
				pWnd->ClientToScreen(&pt);
				pWnd = pChild;
			}
			else break;	//如果没有符合条件的子窗口，则是所找窗口
		}
		
		BOOL bCanDrop = FALSE;	//设置可Drop布尔量
		if(m_bGP)	//如果拖的是股票
		{
			if(pWnd && (pWnd->IsKindOf (RUNTIME_CLASS (CRichEditCtrl)) || pWnd->IsKindOf (RUNTIME_CLASS (CComView))) )
				bCanDrop = TRUE;
		}
		else
		{
			if(pWnd && (pWnd->IsKindOf (RUNTIME_CLASS (CComView))) )
				bCanDrop = TRUE;
		}
		if(bCanDrop) //给DropWnd窗口赋值
		{	
			m_pDropWnd = pWnd;
			SetCursor(m_hCanDrop);
		}
		else
		{
			m_pDropWnd = NULL;
			SetCursor(m_hCannotDrop);
		}
	}
	CListCtrl::OnMouseMove(nFlags, point);
}

void CDragListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		if(m_bGP)	//如果拖的是股票
		{
			if ( m_pDropWnd && m_pDropWnd->IsKindOf (RUNTIME_CLASS (CRichEditCtrl)) )
			{
				if(DraggingGPNo!=-1)
				{
					TxtCurGPCode = (*g_pStockIO)[DraggingGPNo]->Code;
					TxtCurSetCode = (*g_pStockIO)[DraggingGPNo]->setcode;
					(m_pDropWnd->GetParent())->SendMessage(WM_RICHEDIT_MESSAGE);
				}
			}
			else if(m_pDropWnd && m_pDropWnd->IsKindOf (RUNTIME_CLASS (CComView)) )
			{
				if(DraggingGPNo!=-1)
				{
					ComCurGPCode = (*g_pStockIO)[DraggingGPNo]->Code;
					ComCurSetCode = (*g_pStockIO)[DraggingGPNo]->setcode;
					m_pDragImage->DragLeave(GetDesktopWindow ());
					m_pDragImage->EndDrag();
					ReleaseCapture();
					SetCursor(LoadCursor(NULL,IDC_ARROW));
					m_bDragging = FALSE;
					m_pDropWnd->SendMessage(WM_CHANGEGP_MESSAGE);
					if(!((CComView *)m_pDropWnd)->IsInPageGP(ComCurSetCode,ComCurGPCode))
						((CComView *)m_pDropWnd)->SetPageGP(NULL,-1,-1);
					return;
				}
			}
		}
		else //如果是指标
		{
			if(m_pDropWnd && m_pDropWnd->IsKindOf (RUNTIME_CLASS (CComView)) )
			{
				m_pDragImage->DragLeave(GetDesktopWindow ());
				m_pDragImage->EndDrag();
				ReleaseCapture();
				SetCursor(LoadCursor(NULL,IDC_ARROW));
				m_bDragging = FALSE;
				g_ZbDragPoint = point;
				ClientToScreen(&g_ZbDragPoint);
				m_pDropWnd->ScreenToClient(&g_ZbDragPoint);
				m_pDropWnd->SendMessage(UM_DRAGZBTREE,HIWORD(DraggingGPNo),LOWORD(DraggingGPNo));
				return;
			}
		}
		m_pDragImage->DragLeave(GetDesktopWindow ());
		m_pDragImage->EndDrag();
		ReleaseCapture();
		SetCursor(LoadCursor(NULL,IDC_ARROW));
		m_bDragging = FALSE;
	}
	
	CListCtrl::OnLButtonUp(nFlags, point);
}
