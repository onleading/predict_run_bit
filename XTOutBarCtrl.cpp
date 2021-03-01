// XTOutBarCtrl.cpp : implementation of the CXTOutBarCtrl class.
//
// This file is a part of the Xtreme Toolkit for MFC.
// ©1998-2002 Codejock Software, All Rights Reserved.
//
// This source code can only be used under the terms and conditions 
// outlined in the accompanying license agreement.
//
// support@codejock.com
// http://www.codejock.com
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "XTOutBarCtrl.h"
#include "XTMemDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const clr3DShadow= ::GetSysColor(COLOR_3DSHADOW);
const clrWindow= ::GetSysColor(COLOR_WINDOW);
const bXPMode= FALSE;
const clrXP3DFace= ::GetSysColor(COLOR_3DFACE);
const clrBtnText=  ::GetSysColor(COLOR_BTNTEXT);
const clrWindowFrame = ::GetSysColor(COLOR_WINDOWFRAME);
const clr3DHilight=  ::GetSysColor(COLOR_3DHILIGHT);
const clr3DFace =::GetSysColor(COLOR_3DFACE);
const clrHighlight = ::GetSysColor(COLOR_HIGHLIGHT);
const clrXPHighlightLight= ::GetSysColor(COLOR_HIGHLIGHT);
const clrXPHighlight=::GetSysColor(COLOR_HIGHLIGHT);


typedef enum XT_RCOB
{
    RC_OB_ICON = 1,
    RC_OB_LABEL,
    RC_OB_BOTH
};

const int NONE      =-1;
const int FOLDER    = 0;
const int ITEM      = 1;
const int UPSCROLL  = 2;
const int DNSCROLL  = 3;

/////////////////////////////////////////////////////////////////////////////
// CXTEditItem helper class

class  CXTEditItem : public CEdit
{
	DECLARE_DYNAMIC(CXTEditItem)

public:

    // Constructs a CXTEditItem object.
	CXTEditItem();

	// Destroys a CXTEditItem object, handles cleanup and de-allocation.
    virtual ~CXTEditItem();

	int		m_nIndex;
	BOOL	m_bEscapeKey;
	CString m_strText;
	WPARAM	m_wParam;
	BOOL	m_bNoDown;

	//:Ignore
    //{{AFX_VIRTUAL(CXTEditItem)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
    //:End Ignore

protected:

	//:Ignore
    //{{AFX_MSG(CXTEditItem)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
    //:End Ignore

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CXTEditItem

CXTEditItem::CXTEditItem()
{
    m_bEscapeKey = FALSE;
    m_nIndex     = -1;
    m_wParam     = OBN_XT_ONGROUPENDEDIT;
    m_bNoDown    = FALSE;
}

CXTEditItem::~CXTEditItem()
{

}

IMPLEMENT_DYNAMIC(CXTEditItem, CEdit)

BEGIN_MESSAGE_MAP(CXTEditItem, CEdit)
    //{{AFX_MSG_MAP(CXTEditItem)
    ON_WM_KILLFOCUS()
    ON_WM_CREATE()
    ON_WM_CHAR()
    ON_WM_SETCURSOR()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CXTEditItem::OnKillFocus(CWnd* /*pNewWnd*/) 
{
    PostMessage(WM_CLOSE, 0, 0);
    if (!m_bEscapeKey)
    {
        GetWindowText(m_strText);
        if (m_strText != "")
        {
            CWnd* pOwner = GetOwner();
            ASSERT_VALID(pOwner);
            
			pOwner->SendMessage(XTWM_OUTBAR_NOTIFY,m_wParam, (LPARAM)this);
        }
    }
}

BOOL CXTEditItem::PreTranslateMessage(MSG* pMsg) 
{
    if (pMsg->wParam == VK_RETURN)
    {
        PostMessage(WM_CLOSE, 0, 0);
        return TRUE;
    }
    else if (pMsg->wParam == VK_ESCAPE)
    {
        PostMessage(WM_CLOSE, 0, 0);
        return m_bEscapeKey = TRUE;
    }
    
    return CEdit::PreTranslateMessage(pMsg);
}

void CXTEditItem::PostNcDestroy() 
{
    CEdit::PostNcDestroy();
    delete this;
}

int CXTEditItem::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CEdit::OnCreate(lpCreateStruct) == -1)
        return -1;

    CXTOutBarCtrl* pParent = (CXTOutBarCtrl*)GetParent();
    ASSERT_VALID(pParent);

    SetFont(pParent->GetFontX());
	
    return 0;
}

void CXTEditItem::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    CXTOutBarCtrl* pParent = (CXTOutBarCtrl*)GetParent();
    ASSERT_VALID(pParent);

    if (m_wParam == OBN_XT_ONGROUPENDEDIT)
    {
        CEdit::OnChar(nChar, nRepCnt, nFlags);
        return;
    }
    if (nChar == VK_ESCAPE || nChar == VK_RETURN)
    {
        if (nChar == VK_ESCAPE) m_bEscapeKey = TRUE;
        pParent->SetFocus();
        return;
    }
    CEdit::OnChar(nChar, nRepCnt, nFlags);
    CString str;
    CRect rect, parentrect;
    GetClientRect(&rect);
    pParent->GetClientRect(&parentrect);
    ClientToScreen(&rect);
    pParent->ScreenToClient(&rect);
    GetWindowText(str);
    CWindowDC dc(this);
    CFont*pFont = pParent->GetFontX();
    CFont*pFontDC = dc.SelectObject(pFont);
    CRect szrc(rect);
    szrc.bottom = szrc.top;

    if (m_bNoDown == TRUE)
    {
        dc.DrawText(str, szrc, DT_CALCRECT);
        if (szrc.right >= parentrect.right - 1) rect.right = parentrect.right - 1;
        else rect.right = szrc.right;
        MoveWindow(&rect);
        return;
    }

    dc.DrawText(str, szrc, DT_WORDBREAK|DT_CENTER|DT_CALCRECT);
    dc.SelectObject(pFontDC);
    CSize size = szrc.Size();

    if (size.cx > rect.Width())
    {
        if (size.cx + rect.left < parentrect.right) rect.right = rect.left + size.cx;
        else rect.right = parentrect.right;
        MoveWindow(&rect);
    }
    else if (size.cy > rect.Height())
    {
        if (size.cy + rect.bottom < parentrect.bottom) rect.bottom = rect.top + size.cy;
        else rect.bottom = parentrect.bottom;
        MoveWindow(&rect);
    }
}

BOOL CXTEditItem::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	UNREFERENCED_PARAMETER(pWnd);
	UNREFERENCED_PARAMETER(nHitTest);
	UNREFERENCED_PARAMETER(message);

	HCURSOR hCur = ::LoadCursor(NULL, IDC_IBEAM);
	::SetCursor(hCur);
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CXTOutBarCtrl

CXTOutBarCtrl::CXTOutBarCtrl() : m_penBlack(PS_SOLID, 1, RGB(0,0,0))
{
    m_rcUpArrow.SetRectEmpty();
    m_rcDownArrow.SetRectEmpty();

    m_dwFlags               = OBS_XT_DEFAULT;
    m_pLargeImageList       = NULL;
    m_pSmallImageList       = NULL;
    m_sizeOffset.cx         = 4;
    m_sizeOffset.cy         = 3;
    m_sizeMargin.cx         = 5;
    m_sizeMargin.cy         = 5;
    m_nFolderHeight         = 22;
    m_nSelFolder            = 0;
    m_nLastFolderSelected   = -1;
    m_nFolderHilighted      = -1;
    m_nLastItemSelected     = -1;
    m_nItemHilighted        = -1;
    m_nLastDragItemDraw     = -1;
    m_nLastDragItemDrawType = -1;
    m_nFirstItem            = 0;
    m_nIconSpacingSmall     = 10;
    m_nIconSpacingLarge     = 8;
    m_nAnimationTickCount   = 3;
    m_nSelAnimCount         = 0;
    m_nSelAnimTiming        = 0;
    m_nHitInternal1         = 0;
    m_nHitInternal2         = 0;
    m_bUpArrow              = FALSE;
    m_bDownArrow            = FALSE;
    m_bUpPressed            = FALSE;
    m_bDownPressed          = FALSE;
    m_bLooping              = FALSE;
    m_bPressedHighlight     = FALSE;
    m_bIconPressed          = FALSE;
    m_bUserClrBack          = FALSE;
    m_bUserClrText          = FALSE;
    m_clrBack               = clr3DShadow;//
    m_clrText               = clrWindow;

    SetFontX(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));
}

CXTOutBarCtrl::~CXTOutBarCtrl()
{
    for (int iFolder = 0; iFolder < m_arFolder.GetSize(); iFolder++)
    {
		m_nSelFolder = iFolder;
        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(iFolder);
        delete( pBarFolder );
    }

    m_arFolder.RemoveAll();
    m_penBlack.DeleteObject();
    m_font.DeleteObject();
}

IMPLEMENT_DYNCREATE(CXTOutBarCtrl, CWnd)

BEGIN_MESSAGE_MAP(CXTOutBarCtrl, CWnd)
    //{{AFX_MSG_MAP(CXTOutBarCtrl)
    ON_WM_TIMER()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
    ON_WM_SETCURSOR()
    ON_WM_SIZE()
    ON_COMMAND(XT_IDC_LARGEICON, OnLargeIcon)
    ON_UPDATE_COMMAND_UI(XT_IDC_LARGEICON, OnUpdateLargeIcon)
    ON_COMMAND(XT_IDC_SMALLICON, OnSmallIcon)
    ON_UPDATE_COMMAND_UI(XT_IDC_SMALLICON, OnUpdateSmallIcon)
    ON_COMMAND(XT_IDC_REMOVEITEM, OnRemoveItem)
    ON_UPDATE_COMMAND_UI(XT_IDC_REMOVEITEM, OnUpdateRemoveItem)
    ON_COMMAND(XT_IDC_RENAMEITEM, OnRenameItem)
    ON_UPDATE_COMMAND_UI(XT_IDC_RENAMEITEM, OnUpdateRenameItem)
	ON_WM_RBUTTONUP()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
    ON_MESSAGE(XTWM_OUTBAR_NOTIFY, OnEndLabelEdit)
END_MESSAGE_MAP()

long CXTOutBarCtrl::OnEndLabelEdit(WPARAM wParam, LPARAM lParam)
{
    CXTEditItem* pEdit = (CXTEditItem*)lParam;
    ASSERT_VALID(pEdit);

    XT_OUTBAR_INFO obi;
    obi.nIndex    = pEdit->m_nIndex;
    obi.nDragFrom = -1;
    obi.nDragTo   = -1;
    obi.lpszText  = (LPCTSTR)pEdit->m_strText;

    CWnd* pOwner = (CWnd*)GetOwner();
    ASSERT_VALID(pOwner);

    if (wParam == OBN_XT_ONGROUPENDEDIT)
    {
		obi.bFolder = true;

		LRESULT lResult = pOwner->SendMessage(XTWM_OUTBAR_NOTIFY,
            OBN_XT_ONGROUPENDEDIT, (LPARAM)&obi);

        if (lResult == TRUE)
        {
            CRect rc;
            GetFolderRect(obi.nIndex, rc);
            SetFolderText(obi.nIndex, obi.lpszText);
            InvalidateRect(rc, FALSE);
        }
    }

    if (wParam == OBN_XT_ONLABELENDEDIT)
    {
		obi.bFolder = false;

		LRESULT lResult = pOwner->SendMessage(XTWM_OUTBAR_NOTIFY,
            OBN_XT_ONLABELENDEDIT, (LPARAM)&obi);

        if (lResult == TRUE)
        {
            CRect rc;
            GetInsideRect(rc);
            SetItemText(obi.nIndex, obi.lpszText);
            InvalidateRect(rc, TRUE);
        }
    }

    return 0;
}

void CXTOutBarCtrl::SetFolderText( const int iIndex, LPCTSTR lpszFolderName )
{
    if (m_arFolder.GetSize() != 0)
    {
        ASSERT((iIndex >= 0) && (iIndex < GetFolderCount()));
        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(iIndex);
        pBarFolder->SetName( lpszFolderName );
    }
}

void CXTOutBarCtrl::SetItemText( const int iIndex, LPCTSTR lpszItemName )
{
    if (m_arFolder.GetSize() != 0)
    {
        ASSERT(m_nSelFolder >= 0 && m_nSelFolder < GetFolderCount());

        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(m_nSelFolder);

        CXTBarItem* pBarItem = pBarFolder->GetItemAt(iIndex);
        pBarItem->SetName( lpszItemName );
    }
}

BOOL CXTOutBarCtrl::Create(DWORD dwStyle, const RECT&  rect, CWnd* pParentWnd, UINT nID, const DWORD dwFlags)
{
    if(!CreateEx(NULL, dwStyle, rect, pParentWnd, nID, dwFlags)) {
        return FALSE;
    }

    return TRUE;
}

BOOL CXTOutBarCtrl::CreateEx(DWORD dwExStyle, DWORD dwStyle, const RECT&  rect, CWnd* pParentWnd, UINT nID, const DWORD dwFlags)
{
    ASSERT(dwStyle & WS_CHILD);
    ASSERT_VALID(pParentWnd);

    if(!CWnd::CreateEx(dwExStyle, NULL, NULL, dwStyle, rect, pParentWnd, nID)) {
        return FALSE;
    }

    m_dwFlags = dwFlags;

    return TRUE;
}

BOOL CXTOutBarCtrl::IsSmallIconView(const int iFolder/*=-1*/) const
{
	if (GetFolderCount() <= 0 || iFolder == -1)
		return ((m_dwFlags & OBS_XT_SMALLICON)==OBS_XT_SMALLICON);

	CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(iFolder);
	return pBarFolder->m_bSmallIcons;
}

void CXTOutBarCtrl::SetSmallIconView(const BOOL bSet, const int iFolder/*=-1*/)
{
    m_nFirstItem = 0;

	if (iFolder != -1)
	{
		if (iFolder >= 0 && iFolder < GetFolderCount())
		{
			CXTBarFolder* pbf = (CXTBarFolder*)m_arFolder.GetAt(iFolder);
			pbf->m_bSmallIcons = bSet;
		}
	}
	else
	{
		// do all current folders, and set flag so new folders
		// have the chosen style
		int nFolders = GetFolderCount();
		for (int i = 0; i < nFolders; i++)
		{
			CXTBarFolder* pbf = (CXTBarFolder*)m_arFolder.GetAt(i);
			pbf->m_bSmallIcons = bSet;
		}

		if (bSet && ! IsSmallIconView()) 
			m_dwFlags |= OBS_XT_SMALLICON;
		
		else if (! bSet && IsSmallIconView()) 
			m_dwFlags &= ~OBS_XT_SMALLICON;
	}

	CRect rc;
	GetInsideRect(rc);
	InvalidateRect(rc, false);
}

DWORD CXTOutBarCtrl::GetFlag() const
{
    return m_dwFlags;
}

void CXTOutBarCtrl::ModifyFlag(const DWORD&  dwRemove, const DWORD&  dwAdd, const UINT uRedraw)
{
    if (dwRemove) {
        m_dwFlags &= ~dwRemove;
    }
    if (dwAdd) {
        m_dwFlags |= dwAdd;
    }
    if (GetSafeHwnd())
    {
        if (uRedraw != 0) {
            SetWindowPos(0,0,0,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOMOVE|uRedraw);
        }
        else {
            Invalidate();
        }
    }
}

void CXTOutBarCtrl::OnTimer(UINT nIDEvent) 
{
    if (nIDEvent == 1)
    {
        CPoint pt(GetMessagePos());

        ScreenToClient(&pt);

        CRect rc;
        GetClientRect(&rc);
        if (!(rc.PtInRect(pt)))
        {
            HighlightFolder(-1);
            HighlightItem(-1);
            KillTimer(1);
        }
    }
    else if (nIDEvent == 3 && m_nLastItemSelected >= 0)
    {
        m_nSelAnimCount ++;
        if (m_nSelAnimCount > 10) m_nSelAnimCount = -1;
        if (m_nSelAnimCount == 0)
            DrawAnimItem(-1, 1, m_nLastItemSelected);
        if (m_nSelAnimCount == 1)
            DrawAnimItem(0, 0, m_nLastItemSelected);
        if (m_nSelAnimCount == 2)
            DrawAnimItem(1, 1, m_nLastItemSelected);
        if (m_nSelAnimCount == 3)
            DrawAnimItem(0, 0, m_nLastItemSelected);
    }
    CWnd::OnTimer(nIDEvent);
}

void CXTOutBarCtrl::DrawAnimItem(const int iOffsetX, const int iOffsetY, const int iIndex)
{
    CImageList* pImageList = GetFolderImageList(m_nSelFolder, IsSmallIconView(m_nSelFolder));

    CRect rc, irc;
    GetInsideRect(irc);
    GetItemRect(m_nSelFolder, m_nLastItemSelected, rc);

    if (m_arFolder.GetSize() != 0)
    {
        if (m_nLastItemSelected >= m_nFirstItem && irc.bottom > rc.bottom && irc.top < rc.top)
        {
            ASSERT(m_nSelFolder >= 0 && m_nSelFolder < GetFolderCount());
            CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(m_nSelFolder);

            ASSERT(iIndex >= 0 && iIndex < pBarFolder->GetItemCount());
            CXTBarItem* pBarItem = (CXTBarItem*)pBarFolder->GetItemAt(iIndex);

            ASSERT(pBarItem && pImageList);

            CClientDC dc(this);

            if (IsSmallIconView(m_nSelFolder))
            {
                if (pImageList)
                {
                    IMAGEINFO ii;
                    pImageList->GetImageInfo(pBarItem->GetIndex(), &ii);
                    CSize szImage = CRect(ii.rcImage).Size();
                    CPoint pt;
                    pt.x = rc.left + 2;
                    pt.y = rc.top + (rc.Height() - szImage.cy) / 2;

                    CRect rcBck(pt.x-1, pt.y-1, pt.x + szImage.cx+2, pt.y + szImage.cy+2);
                    dc.FillSolidRect(rcBck, clr3DShadow);

                    pt.x += iOffsetX;
                    pt.y += iOffsetY;

                    pImageList->Draw(&dc, pBarItem->GetIndex(), pt, ILD_NORMAL);
                }
            }
            else
            {
                if (pImageList)
                {
                    IMAGEINFO ii;
                    pImageList->GetImageInfo(pBarItem->GetIndex(), &ii);
                    CSize szImage = CRect(ii.rcImage).Size();
                    CPoint pt;
                    pt.x = rc.left + (rc.Width() - szImage.cx) / 2;
                    pt.y = rc.top;

                    CRect rcBck(pt.x-1, pt.y-1, pt.x + szImage.cx+2, pt.y + szImage.cy+2);
                    dc.FillSolidRect(rcBck, clr3DShadow);

                    pt.x += iOffsetX;
                    pt.y += iOffsetY;

                    pImageList->Draw(&dc, pBarItem->GetIndex(), pt, ILD_NORMAL);
                }
            }
        }
    }
}

void CXTOutBarCtrl::OnPaint() 
{
    // background is already filled in gray
    CPaintDC dc(this);

    if (!m_bUserClrBack) 
    {
        m_clrBack = bXPMode? 
            clrXP3DFace : 
            clr3DShadow;
    }
    if (!m_bUserClrText)
    {
        m_clrText = bXPMode? 
            clrBtnText : 
            clrWindow;
    }

    // Get the client rect.
    CRect rectClient;
    GetClientRect(&rectClient);

    // Paint to a memory device context to help
    // eliminate screen flicker.
    CXTMemDC memDC(&dc, rectClient,::GetSysColor(COLOR_3DFACE));//ÐÞ¸Ä
    int nSavedDC = memDC.SaveDC();

    CRect rc;
    GetInsideRect(rc);
    if (!GetFolderChild())
    {
        memDC.FillSolidRect(rc, m_clrBack);

        if (!bXPMode) {
            memDC.Draw3dRect(rc, clrWindowFrame, clr3DShadow);
        }
    }

    int max = m_arFolder.GetSize();
    CRect frc;
    for (int t = 0; t < max; t++)
    {
        GetFolderRect(t, frc);
        DrawFolder(&memDC, t, frc, F_NORMAL);
    }
    if (!GetFolderChild())
    {
        int f,l;
        GetVisibleRange(m_nSelFolder, f,l);
        m_rcUpArrow.SetRect(0,0,GetSystemMetrics(SM_CXVSCROLL), GetSystemMetrics(SM_CXVSCROLL));
        m_rcDownArrow = m_rcUpArrow;
        m_rcUpArrow.OffsetRect(rc.right - 5 - GetSystemMetrics(SM_CXVSCROLL), rc.top +5);
        m_rcDownArrow.OffsetRect(rc.right - 5 - GetSystemMetrics(SM_CXVSCROLL), rc.bottom - 5 - GetSystemMetrics(SM_CXVSCROLL));

        if (f > 0)
        {
            if (m_bUpPressed) memDC.DrawFrameControl(m_rcUpArrow, DFC_SCROLL, DFCS_SCROLLUP|DFCS_PUSHED);
            else memDC.DrawFrameControl(m_rcUpArrow, DFC_SCROLL, DFCS_SCROLLUP);
            m_bUpArrow = TRUE;
            memDC.ExcludeClipRect(m_rcUpArrow);
        }
        else m_bUpArrow = FALSE;

        if (l < GetItemCount() - 1)
        {
            if (m_bDownPressed) memDC.DrawFrameControl(m_rcDownArrow, DFC_SCROLL, DFCS_SCROLLDOWN|DFCS_PUSHED);
            else memDC.DrawFrameControl(m_rcDownArrow, DFC_SCROLL, DFCS_SCROLLDOWN);
            m_bDownArrow = TRUE;
            memDC.ExcludeClipRect(m_rcDownArrow);
        }
        else m_bDownArrow = FALSE;

        PaintItems(&memDC, m_nSelFolder, rc);
    }

    dc.BitBlt(rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), &memDC, 0,0, SRCCOPY);

    memDC.RestoreDC(nSavedDC);

    if (m_nFolderHilighted >= 0)
    {
        int i = m_nFolderHilighted;
        m_nFolderHilighted = -1;
        HighlightFolder(i);
    }
    if (m_nItemHilighted >= 0)
    {
        int i = m_nItemHilighted;
        m_nItemHilighted = -1;
        HighlightItem(i, m_bPressedHighlight);
    }
}

BOOL CXTOutBarCtrl::OnEraseBkgnd(CDC* pDC) 
{
    UNUSED_ALWAYS(pDC);
    return TRUE;
}

BOOL CXTOutBarCtrl::GetFolderRect(const int iIndex, CRect&  rect) const
{
    if (m_arFolder.GetSize() != 0)
    {
        int max = m_arFolder.GetSize();
        ASSERT(iIndex >= 0 && iIndex < max);

        if (iIndex >= 0 && iIndex < max)
        {
            CRect rc;
            GetClientRect(rc);
            if (iIndex > m_nSelFolder)
                rect.SetRect(rc.left, rc.bottom - ((max - iIndex))* m_nFolderHeight, rc.right, 
                rc.bottom - (max - iIndex - 1)* m_nFolderHeight);
            else 
                rect.SetRect(rc.left, rc.top + iIndex* m_nFolderHeight - 1, rc.right, 
                rc.top + (1 + iIndex)* m_nFolderHeight - 1);
            return TRUE;
        }
    }
    return FALSE;
}

void CXTOutBarCtrl::GetItemRect(const int iFolder, const int iIndex, CRect&  rect)
{
    CRect rc;
    GetInsideRect(rc);
    int top = rc.top;
    CSize sz(0,0);
    int y = 0;
    for (int t = 0; t < iIndex; t++)
    {
        sz = GetItemSize(iFolder, t, RC_OB_BOTH);
        top += sz.cy;

        if (IsSmallIconView(iFolder)) 
        {
            top += m_nIconSpacingSmall; 
        }
        else 
        {
            top += m_nIconSpacingLarge;
        }
        if (t == m_nFirstItem - 1) y = top - rc.top;
    }
    sz = GetItemSize(iFolder, iIndex, RC_OB_BOTH);
    rect.SetRect(rc.left, top, rc.left + sz.cx, top + sz.cy);

    rect.top -= y;
    rect.bottom -= y;

    rect.left += m_sizeMargin.cx;
    rect.top  += m_sizeMargin.cy;
    rect.bottom += m_sizeMargin.cy;

    if (!IsSmallIconView(iFolder)) 
    {
        rect.left = rc.left;
        rect.right = rc.right;
    }
}

void CXTOutBarCtrl::DrawFolder(CDC* pDC, const int iIndex, CRect rect, const FOLDER_HILIGHT eHilight)
{
    if (m_arFolder.GetSize() != 0)
    {
        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(iIndex);

        int nSavedDC = pDC->SaveDC();

        CPen pen(PS_SOLID, 1, clr3DShadow);
        
        CRect rcFolder(rect);

        switch (eHilight)
        {
        case F_NORMAL:
            {
                pDC->SelectObject(&pen);
                pDC->MoveTo(rect.left, rect.top);
                pDC->LineTo(rect.right, rect.top);
            
                rect.top ++;

                pDC->Draw3dRect(rect, clr3DHilight, clr3DShadow);
                rect.InflateRect(-1,-1);
                pDC->FillSolidRect(rect, clr3DFace);
                pDC->SetBkMode(TRANSPARENT);
                pDC->SelectObject(GetFontX());

                rcFolder.left  += 4;
                rcFolder.right -= 4;
            
                pDC->DrawText(pBarFolder->GetName(), rcFolder,
                    DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            }
            break;
        case F_SELECT:
            {
                pDC->SelectObject(&pen);
                pDC->MoveTo(rect.left+1, rect.top);
                pDC->LineTo(rect.right, rect.top);

                rect.top ++;
                pDC->Draw3dRect(rect, clrWindowFrame, clr3DHilight);
                rect.InflateRect(-1,-1);
                if (!bXPMode)
                {
                    pDC->Draw3dRect(rect, clr3DShadow, clr3DFace);
                    rect.InflateRect(-1,-1);
                }

                pDC->FillSolidRect(rect, clr3DFace);

                pDC->SetBkMode(TRANSPARENT);
                pDC->SelectObject(GetFontX());

                rcFolder.left  += 4;
                rcFolder.right -= 4;
            
                pDC->DrawText(pBarFolder->GetName(), rcFolder,
                    DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            }
            break;
        case F_HILIGHT:
            {
                pDC->SelectObject(&pen);
                pDC->MoveTo(rect.left, rect.top);
                pDC->LineTo(rect.right, rect.top);
            
                rect.top ++;

                pDC->Draw3dRect(rect, clr3DHilight, clrWindowFrame);
                rect.InflateRect(-1,-1);

                pDC->FillSolidRect(rect, clr3DFace);
                pDC->SetBkMode(TRANSPARENT);
                pDC->SelectObject(GetFontX());

                rcFolder.left  += 4;
                rcFolder.right -= 4;
            
                pDC->DrawText(pBarFolder->GetName(), rcFolder,
                    DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            }
            break;
        }

        pDC->RestoreDC(nSavedDC);
        pen.DeleteObject();
    }
}

int CXTOutBarCtrl::AddFolder( LPCTSTR lpszFolderName, const DWORD dwData )
{
    CXTBarFolder* pBarFolder = new CXTBarFolder(lpszFolderName, dwData);
    ASSERT(pBarFolder);

	pBarFolder->m_bSmallIcons = (m_dwFlags & OBS_XT_SMALLICON);

    return m_arFolder.Add((void*)pBarFolder);
}

void CXTOutBarCtrl::GetInsideRect(CRect&  rect) const
{
    GetClientRect(rect);
    if (m_arFolder.GetSize() > 0)
    {
        int max = m_arFolder.GetSize();
        rect.top += m_nFolderHeight* (m_nSelFolder + 1) - 1;//+ 2;
        rect.bottom -= (max - m_nSelFolder - 1)*m_nFolderHeight;
        return;
    }
}

void CXTOutBarCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
    int iIndex, ht = HitTestEx(point, iIndex);

    if (ht != FOLDER && m_nFolderHilighted >= 0) HighlightFolder(-1);
    if (ht != ITEM   && m_nItemHilighted >= 0) HighlightItem(-1);

    if (ht == FOLDER)
    {
        HighlightFolder(iIndex);
        SetTimer(1,100,NULL);
    }
    else if (ht == ITEM)
    {
        HighlightItem(iIndex);
        SetTimer(1,100,NULL);
    }

    CWnd::OnMouseMove(nFlags, point);
}

int CXTOutBarCtrl::HitTestEx(const CPoint&  point, int& iIndex)
{
    if (m_arFolder.GetSize() != 0)
    {
        if (m_bUpArrow && m_rcUpArrow.PtInRect(point)) return UPSCROLL;
        if (m_bDownArrow && m_rcDownArrow.PtInRect(point)) return DNSCROLL;

        int max = m_arFolder.GetSize(), t;

        CRect rc;
        for (t = 0; t < max; t++)
        {
            GetFolderRect(t, rc);
            if (rc.PtInRect(point)) 
            {
                iIndex = t;
                return FOLDER;
            }
        }
        GetInsideRect(rc);
        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(m_nSelFolder);
        max = pBarFolder->GetItemCount();
        for (t = m_nFirstItem; t < max; t++)
        {
            CRect rcItem;
            if (!IsSmallIconView(m_nSelFolder)) 
            {
                GetIconRect(m_nSelFolder, t, rcItem);
                if (rcItem.PtInRect(point))
                {
                    iIndex = t;
                    return ITEM;
                }
                else if (rcItem.top > rc.bottom) break;
                GetLabelRect(m_nSelFolder, t, rcItem);
                rcItem.top -= m_sizeOffset.cy;

                if (rcItem.PtInRect(point))
                {
                    iIndex = t;
                    return ITEM;
                }
                else if (rcItem.top > rc.bottom) break;
            }
            else
            {
                GetItemRect(m_nSelFolder, t, rcItem);
                if (rcItem.PtInRect(point))
                {
                    iIndex = t;
                    return ITEM;
                }
                else if (rcItem.top > rc.bottom) break;
            }
        }
    }
    return NONE;
}

void CXTOutBarCtrl::HighlightFolder(const int iIndex)
{
    CWnd* pFocus = GetFocus();
    if (pFocus != NULL && pFocus != this && IsChild(pFocus)) {
        return;
    }

    if (m_nFolderHilighted != iIndex)
    {
        CClientDC dc(this);

        if (m_nFolderHilighted >= 0)
        {
            CRect rc;
            if (GetFolderRect(m_nFolderHilighted, rc))
            {
                DrawFolder(&dc, m_nFolderHilighted, rc, F_NORMAL);
            }
        }
        if (iIndex >= 0)
        {
            CRect rc;
            if (GetFolderRect(iIndex, rc))
            {
                DrawFolder(&dc, iIndex, rc, F_HILIGHT);
            }
        }
        else
        {
            CRect rc;
            if (GetFolderRect(m_nFolderHilighted, rc))
            {
                DrawFolder(&dc, m_nFolderHilighted, rc, F_NORMAL);
            }
        }
        m_nFolderHilighted = iIndex;
    }
}

void CXTOutBarCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
    if (GetFocus() != this) SetFocus();

    int iIndex, ht = HitTestEx(point, iIndex);
    m_nLastDragItemDrawType = -1;

    CRect inRc;
    GetInsideRect(inRc);

    if (ht == FOLDER)
    {
        BOOL bHigh = TRUE;
        CRect rc;
        GetFolderRect(iIndex, rc);

        if (::GetCapture() == NULL)
        {
            SetCapture();
            ASSERT(this == GetCapture());
            CClientDC dc(this);
            DrawFolder(&dc, iIndex, rc, F_SELECT);
            AfxLockTempMaps();
            for (;;)
            {
                MSG msg;
                VERIFY(::GetMessage(&msg, NULL, 0, 0));
                if (CWnd::GetCapture() != this) break;
                switch (msg.message)
                {
                case WM_MOUSEMOVE:
                    {
                        CPoint pt(msg.lParam);
                        int idx, ht1 = HitTestEx(pt, idx);
                        if (ht1 == FOLDER && idx == iIndex)
                        {
                            if (!bHigh)
                            {
                                DrawFolder(&dc, iIndex, rc, F_SELECT);
                                bHigh = TRUE;
                            }
                        }
                        else
                        {
                            if (bHigh)
                            {
                                DrawFolder(&dc, iIndex, rc, F_NORMAL);
                                bHigh = FALSE;
                            }
                        }
                    }
                    break;
                case WM_LBUTTONUP:
                    {
                        ReleaseCapture();

                        if (bHigh)
                        {
                            DrawFolder(&dc, iIndex, rc, F_NORMAL);
                            bHigh = FALSE;
                        }
                        CPoint pt(msg.lParam);
                        int idx, ht1 = HitTestEx(pt, idx);
                        if (ht1 == FOLDER && idx != m_nSelFolder)
                            SetSelFolder(idx);
                    }
                    goto ExitLoop2;
                case WM_KEYDOWN:    
                    if (msg.wParam != VK_ESCAPE) 
                        break;

                default:
                    DispatchMessage(&msg);
                    break;
                }
            }
        ExitLoop2:
            ReleaseCapture();
            AfxUnlockTempMaps(FALSE);
        }
        if (bHigh) InvalidateRect(rc, FALSE);
    }
    else 
		m_nLastFolderSelected = -1;

    if (ht == ITEM)
    {
        m_nLastDragItemDraw = -1;
        BOOL bHigh = TRUE, bDragging = FALSE;
        CRect rc;
        GetItemRect(m_nSelFolder, iIndex, rc);

        HCURSOR hCur = GetCursor();

        if (::GetCapture() == NULL)
        {
            SetCapture();
            ASSERT(this == GetCapture());
            CClientDC dc(this);
            HighlightItem(iIndex, TRUE);
            AfxLockTempMaps();
            for (;;)
            {
                MSG msg;
                VERIFY(::GetMessage(&msg, NULL, 0, 0));

                if (CWnd::GetCapture() != this) break;

                switch (msg.message)
                {
                case WM_MOUSEMOVE:
                    {
                        CPoint pt(msg.lParam);
                        int idx, ht1 = HitTestEx(pt, idx);
                        if (bDragging)
                        {
                            if (ht1 == ITEM) 
                            {
                                DrawDragArrow(&dc, iIndex, idx);
                            }
                            else 
                            {
                                CRect rcItem;
                                GetItemRect(m_nSelFolder, GetItemCount() - 1, rcItem);
                                if (pt.y > rcItem.bottom && pt.y < inRc.bottom)
                                {
                                    DrawDragArrow(&dc, iIndex, GetItemCount());
                                }
                                else
                                {
                                    DrawDragArrow(&dc, iIndex, -1);
                                }
                            }
                        }
                        else
                        {
                            if (ht1 == ITEM && idx == iIndex)
                            {
                                if (!bHigh)
                                {
                                    HighlightItem(iIndex, TRUE);
                                    bHigh = TRUE;
                                    m_bPressedHighlight = TRUE;
                                }
                            }
                            else 
                            {
                                if (ht1 == ITEM)
                                {
                                    if (bHigh)
                                    {
                                        HighlightItem(iIndex, FALSE);
                                        bHigh = FALSE;
                                        m_bPressedHighlight = FALSE;
                                    }
                                }
                                else
                                {
                                    if (m_dwFlags & OBS_XT_DRAGITEMS)
                                    {
                                        HighlightItem(iIndex, TRUE);
                                        bHigh = TRUE;
                                        bDragging = TRUE;

										XT_OUTBAR_INFO obi;
                                        obi.nIndex    = iIndex;
                                        obi.nDragFrom = -1;
                                        obi.nDragTo   = -1;
                                        obi.lpszText  = GetItemText(iIndex);
										obi.bFolder   = false;

                                        CWnd* pOwner = GetOwner();
                                        ASSERT_VALID(pOwner);
                                        
                                        pOwner->SendMessage(XTWM_OUTBAR_NOTIFY,
											OBN_XT_BEGINDRAG, (LPARAM)&obi);

                                        m_bPressedHighlight = TRUE;
                                    }
                                }
                            }
                        }
                    }
                    break;

                case WM_SETCURSOR:
                    ::SetCursor(hCur);
                    break;

                case WM_LBUTTONUP:
                    {
                        ReleaseCapture();

                        if (bHigh)
                        {
                            HighlightItem(-1);
                            bHigh = FALSE;
                        }
                        CPoint pt(msg.lParam);
                        int idx, ht1 = HitTestEx(pt, idx);
                        if (!bDragging)
                        {
                            if (ht1 == ITEM && idx == iIndex) 
                            {
                                XT_OUTBAR_INFO obi;
                                obi.nIndex    = idx;
                                obi.nDragFrom = -1;
                                obi.nDragTo   = -1;
                                obi.lpszText  = GetItemText(idx);
								obi.bFolder   = false;

                                CWnd* pOwner = GetOwner();
                                ASSERT_VALID(pOwner);

								LRESULT lResult = pOwner->SendMessage(XTWM_OUTBAR_NOTIFY,
                                    OBN_XT_ITEMCLICK, (LPARAM)&obi);

                                if (lResult == TRUE)
                                {
                                    if (m_nSelAnimTiming > 0 && iIndex != m_nLastItemSelected && m_nLastItemSelected >= 0)
                                    {
                                        DrawAnimItem(0, 0, m_nLastItemSelected);
                                    }
                                    
                                    if ((m_dwFlags & OBS_XT_SELHIGHLIGHT) && (m_nLastItemSelected >= 0))
                                    {
                                        CRect rc;
                                        GetIconRect(m_nSelFolder, m_nLastItemSelected, rc);
                                        rc.InflateRect(1,1);
                                        InvalidateRect(rc);
                                    }
                                    
                                    m_nLastItemSelected = iIndex;
                                    
                                    if ((m_dwFlags & OBS_XT_SELHIGHLIGHT) && (m_nLastItemSelected >= 0))
                                    {
                                        CRect rc;
                                        GetIconRect(m_nSelFolder, m_nLastItemSelected, rc);
                                        rc.InflateRect(1,1);
                                        InvalidateRect(rc);
                                    }
                                }
                            }
                        }
                        else
                        {
                            if (ht1 == ITEM)
                            {
                                if (idx != iIndex)
                                {
                                    CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(m_nSelFolder);
                                    POSITION pos1 = pBarFolder->m_barItems.FindIndex(iIndex);

                                    CXTBarItem* pItemFrom = (CXTBarItem*)pBarFolder->m_barItems.GetAt(pos1);
                                    POSITION pos2 = pBarFolder->m_barItems.FindIndex(idx);

                                    XT_OUTBAR_INFO obi;
                                    obi.nIndex    = iIndex;
                                    obi.nDragFrom = iIndex;
                                    obi.nDragTo   = idx;
                                    obi.lpszText  = GetItemText(iIndex);
									obi.bFolder   = false;

                                    CWnd* pOwner = GetOwner();
                                    ASSERT_VALID(pOwner);

									LRESULT lResult = pOwner->SendMessage(XTWM_OUTBAR_NOTIFY,
                                        OBN_XT_DRAGITEM, (LPARAM)&obi);

                                    if (lResult == TRUE)
                                    {
                                        if (m_arFolder.GetSize() != 0)
                                        {
                                            pBarFolder->m_barItems.RemoveAt(pos1);
                                            pBarFolder->m_barItems.InsertBefore(pos2, pItemFrom);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                CRect rcItem;
                                GetItemRect(m_nSelFolder, GetItemCount() - 1, rcItem);
                                if (pt.y > rcItem.bottom && pt.y < inRc.bottom)
                                {
                                    CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt( m_nSelFolder );
                                    POSITION pos1 = pBarFolder->m_barItems.FindIndex(iIndex);
                                    
                                    CXTBarItem* pItemFrom = (CXTBarItem*)pBarFolder->m_barItems.GetAt( pos1 );
                                    POSITION pos2 = pBarFolder->m_barItems.FindIndex(pBarFolder->m_barItems.GetCount());

                                    XT_OUTBAR_INFO obi;
                                    obi.nIndex    = pBarFolder->m_barItems.GetCount();
                                    obi.nDragFrom = iIndex;
                                    obi.nDragTo   = obi.nIndex;
                                    obi.lpszText  = GetItemText(iIndex);
									obi.bFolder   = false;

                                    CWnd* pOwner = GetOwner();
                                    ASSERT_VALID(pOwner);

									LRESULT lResult = pOwner->SendMessage(XTWM_OUTBAR_NOTIFY,
                                        OBN_XT_DRAGITEM, (LPARAM)&obi);

                                    if (lResult == TRUE)
                                    {
                                        pBarFolder->m_barItems.RemoveAt( pos1 );
                                        pBarFolder->m_barItems.InsertAfter( pos2, pItemFrom );
                                    }
                                }
                            }
                        }
                    }
                    goto ExitLoop4;
                default:
                    DispatchMessage(&msg);
                    break;
                }
            }

        ExitLoop4:
            ReleaseCapture();
            AfxUnlockTempMaps(FALSE);
            if (bDragging) 
            {
                Invalidate();
            }
        }
        m_bPressedHighlight = FALSE;
        if (bHigh) InvalidateRect(rc, FALSE);
    }
    else 
		m_nItemHilighted = -1;

    if (ht == DNSCROLL)
    {
        m_bLooping = TRUE;
        BOOL bHigh = TRUE;

        if (::GetCapture() == NULL)
        {
            SetCapture();
            ASSERT(this == GetCapture());
            CClientDC dc(this);
            dc.DrawFrameControl(m_rcDownArrow, DFC_SCROLL, DFCS_SCROLLDOWN|DFCS_PUSHED);
            SetTimer(2,300,NULL);
            AfxLockTempMaps();
            for (;;)
            {
                MSG msg;
                VERIFY(::GetMessage(&msg, NULL, 0, 0));

                if (CWnd::GetCapture() != this) break;

                switch (msg.message)
                {
                case WM_MOUSEMOVE:
                    {
                        CPoint pt(msg.lParam);
                        if (m_rcDownArrow.PtInRect(pt))
                        {
                            if (bHigh == FALSE)
                            {
                                dc.DrawFrameControl(m_rcDownArrow, DFC_SCROLL, DFCS_SCROLLDOWN|DFCS_PUSHED);
                                bHigh = TRUE;
                                m_bDownArrow = TRUE;
                                m_bDownPressed = TRUE;
                            }
                        }
                        else
                        {
                            if (bHigh == TRUE)
                            {
                                dc.DrawFrameControl(m_rcDownArrow, DFC_SCROLL, DFCS_SCROLLDOWN);
                                bHigh = FALSE;
                                m_bDownArrow = FALSE;
                                m_bDownPressed = FALSE;
                            }
                        }
                    }
                    break;
                case WM_LBUTTONUP:
                    {
                        ReleaseCapture();

                        if (bHigh)
                        {
                            dc.DrawFrameControl(m_rcDownArrow, DFC_SCROLL, DFCS_SCROLLDOWN);
                            bHigh = FALSE;
                        }
                        m_bDownArrow = FALSE;
                        CPoint pt(msg.lParam);
                        if (m_rcDownArrow.PtInRect(pt))
                        {
                            CRect itrc;
                            GetItemRect(m_nSelFolder, GetItemCount() - 1, itrc);
                            CRect crc;
                            GetInsideRect(crc);
                            if (itrc.bottom > crc.bottom)
                            {
                                m_nFirstItem ++;
                                InvalidateRect(crc, TRUE);
                            }
                        }
                    }
                    goto ExitLoop3;
                case WM_TIMER:
                    {
                        if (msg.wParam == 2)
                        {
                            if (bHigh)
                            {
                                CPoint pt(msg.pt);
                                ScreenToClient(&pt);
                                if (m_rcDownArrow.PtInRect(pt))
                                {
                                    m_bDownPressed = TRUE;
                                    CRect itrc;
                                    GetItemRect(m_nSelFolder, GetItemCount() - 1, itrc);
                                    CRect crc;
                                    GetInsideRect(crc);
                                    if (itrc.bottom > crc.bottom)
                                    {
                                        m_nFirstItem ++;
                                        InvalidateRect(crc, TRUE);
                                    }
                                    else goto ExitLoop3;
                                }
                                else m_bDownPressed = FALSE;
                            }
                        }
                        break;
                    }
                case WM_KEYDOWN:    
                    if (msg.wParam != VK_ESCAPE) 
                        break;
                default:
                    DispatchMessage(&msg);
                    break;
                }
            }
        ExitLoop3:
            KillTimer(2);
            ReleaseCapture();
            AfxUnlockTempMaps(FALSE);
            m_bLooping = FALSE;
            m_bDownPressed = FALSE;
            m_bDownArrow = FALSE;
            CRect crc;
            GetInsideRect(crc);
            InvalidateRect(crc, TRUE);
        }
    }

    if (ht == UPSCROLL)
    {
        m_bLooping = TRUE;
        BOOL bHigh = TRUE;

        if (::GetCapture() == NULL)
        {
            SetCapture();
            ASSERT(this == GetCapture());
            CClientDC dc(this);
            dc.DrawFrameControl(m_rcUpArrow, DFC_SCROLL, DFCS_SCROLLUP|DFCS_PUSHED);
            SetTimer(2,300,NULL);
            AfxLockTempMaps();
            for (;;)
            {
                MSG msg;
                VERIFY(::GetMessage(&msg, NULL, 0, 0));

                if (CWnd::GetCapture() != this) break;

                switch (msg.message)
                {
                case WM_MOUSEMOVE:
                    {
                        CPoint pt(msg.lParam);
                        if (m_rcUpArrow.PtInRect(pt))
                        {
                            if (bHigh == FALSE)
                            {
                                dc.DrawFrameControl(m_rcUpArrow, DFC_SCROLL, DFCS_SCROLLUP|DFCS_PUSHED);
                                bHigh = TRUE;
                                m_bUpArrow = TRUE;
                                m_bUpPressed = TRUE;
                            }
                        }
                        else
                        {
                            if (bHigh == TRUE)
                            {
                                dc.DrawFrameControl(m_rcUpArrow, DFC_SCROLL, DFCS_SCROLLUP);
                                bHigh = FALSE;
                                m_bUpArrow = FALSE;
                                m_bUpPressed = FALSE;
                            }
                        }
                    }
                    break;

                case WM_LBUTTONUP:
                    {
                        ReleaseCapture();

                        if (bHigh)
                        {
                            dc.DrawFrameControl(m_rcUpArrow, DFC_SCROLL, DFCS_SCROLLUP);
                            bHigh = FALSE;
                        }
                        m_bUpArrow = FALSE;
                        CPoint pt(msg.lParam);
                        if (m_rcUpArrow.PtInRect(pt))
                        {
                            if (m_nFirstItem > 0 ) 
                            {
                                m_nFirstItem --;
                                CRect crc;
                                GetInsideRect(crc);
                                InvalidateRect(crc, TRUE);
                            }
                        }
                    }

                    goto ExitLoop;

                case WM_TIMER:
                    {
                        if (msg.wParam == 2)
                        {
                            if (bHigh)
                            {
                                CPoint pt(msg.pt);
                                ScreenToClient(&pt);
                                if (m_rcUpArrow.PtInRect(pt))
                                {
                                    m_bUpPressed = TRUE;
                                    if (m_nFirstItem > 0) 
                                    {
                                        m_nFirstItem --;
                                        CRect crc;
                                        GetInsideRect(crc);
                                        InvalidateRect(crc, TRUE);
                                    }
                                    else goto ExitLoop;
                                }
                                else m_bUpPressed = FALSE;
                            }
                        }
                        break;
                    }
                case WM_KEYDOWN:    
                    if (msg.wParam != VK_ESCAPE) break;

                default:
                    DispatchMessage(&msg);
                    break;
                }
            }

        ExitLoop:
            KillTimer(2);
            ReleaseCapture();
            AfxUnlockTempMaps(FALSE);
            m_bLooping = FALSE;
            m_bUpPressed = FALSE;
            m_bUpArrow = FALSE;
            CRect crc;
            GetInsideRect(crc);
            InvalidateRect(crc, TRUE);
        }
    }
    
    CWnd::OnLButtonDown(nFlags, point);
}

int CXTOutBarCtrl::InsertItem(const int iFolder, const int iIndex, LPCTSTR lpszItemName, const int iImage, const DWORD dwData)
{
    if (m_arFolder.GetSize() != 0)
    {
        ASSERT(iFolder >= 0 && iFolder < GetFolderCount());

        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(iFolder);

        return pBarFolder->InsertItem(iIndex, lpszItemName, iImage, dwData);
    }
    return -1;
}

int CXTOutBarCtrl::GetItemCount() const
{
    if (m_arFolder.GetSize() != 0)
    {
        ASSERT(m_nSelFolder >= 0 && m_nSelFolder < GetFolderCount());
        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(m_nSelFolder);
        return  pBarFolder->GetItemCount();
    }
    return 0;
}

void CXTOutBarCtrl::SetSelFolder(const int iIndex)
{
    if (m_arFolder.GetSize() != 0)
    {
        ASSERT(iIndex >= 0 && iIndex < GetFolderCount());

        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(iIndex);
        
        CWnd* pOwner = (CWnd*)GetOwner();
        ASSERT_VALID(pOwner);
        
        XT_OUTBAR_INFO obi;
        obi.nIndex    = iIndex;
        obi.nDragFrom = -1;
        obi.nDragTo   = -1;
        obi.lpszText  = (LPCTSTR)pBarFolder->GetName();
		obi.bFolder   = true;
        
		LRESULT lResult = pOwner->SendMessage(XTWM_OUTBAR_NOTIFY,OBN_XT_FOLDERCHANGE, (LPARAM)&obi);

        if (lResult == TRUE)
        {
            CWnd* pWnd = GetFolderChild();
            if (pWnd) 
			{
                pWnd->ShowWindow(SW_HIDE);
            }

            if (iIndex != m_nSelFolder && m_nSelFolder >= 0)
            {
                if ((m_dwFlags & OBS_XT_ANIMATION) && (m_nAnimationTickCount >= 0)) 
				{
                    AnimateFolderScroll(m_nSelFolder, iIndex);
                }
            }
            
            m_nSelFolder = iIndex;
            m_nFirstItem = 0;
            m_nLastItemSelected = -1;
            
            pWnd = GetFolderChild();
            if (pWnd)
            {
                CRect rc;
                GetInsideRect(rc);
                pWnd->MoveWindow(rc);
                pWnd->ShowWindow(SW_SHOW);
            }

            InvalidateRect(NULL);
        }
    }
}

int CXTOutBarCtrl::GetFolderCount() const
{
    return m_arFolder.GetSize();
}

int CXTOutBarCtrl::GetSelFolder() const
{
    return m_nSelFolder;
}

void CXTOutBarCtrl::RemoveFolder(const int iIndex)
{
    if (m_arFolder.GetSize() != 0)
    {
        ASSERT(iIndex >= 0 && iIndex < GetFolderCount());
        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(iIndex);

        CWnd* pOwner = (CWnd*)GetOwner();
        ASSERT_VALID(pOwner);

        XT_OUTBAR_INFO obi;
        obi.nIndex    = iIndex;
        obi.nDragFrom = -1;
        obi.nDragTo   = -1;
        obi.lpszText  = (LPCTSTR)pBarFolder->GetName();
		obi.bFolder   = true;

		LRESULT lResult = pOwner->SendMessage(XTWM_OUTBAR_NOTIFY,
            OBN_XT_DELETEFOLDER, (LPARAM)&obi);

        if (lResult == TRUE)
        {
            if (m_nSelFolder >= iIndex) {
                m_nSelFolder = iIndex - 1;
            }
            if (m_nSelFolder < 0 && GetFolderCount() > 0) {
                m_nSelFolder = 0;
            }
            
            m_arFolder.RemoveAt(iIndex);
            delete(pBarFolder);

            InvalidateRect(NULL);
        }
    }
}

CImageList* CXTOutBarCtrl::SetImageList(CImageList* pImageList, DWORD dwImageList)
{
    CImageList* pOldImageList = NULL;
    
    if (dwImageList & OBS_XT_SMALLICON) {
        pOldImageList = m_pSmallImageList;
        m_pSmallImageList = pImageList;
    }
    else if (dwImageList & OBS_XT_LARGEICON) {
        pOldImageList = m_pLargeImageList;
        m_pLargeImageList = pImageList;
    }
    
    return pOldImageList;
}

CImageList* CXTOutBarCtrl::GetImageList(DWORD dwImageList)
{
    if (dwImageList & OBS_XT_SMALLICON) {
        return m_pSmallImageList;
    }
    else if (dwImageList & OBS_XT_LARGEICON) {
        return m_pLargeImageList;
    }

    return NULL;
}

CImageList* CXTOutBarCtrl::SetFolderImageList(const int iFolder, CImageList* pImageList, DWORD dwImageList)
{
    if (m_arFolder.GetSize() != 0)
    {
        ASSERT(iFolder >= 0 && iFolder < GetFolderCount());
        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(iFolder);
        CImageList* pOldImageList = NULL;
        
        if (dwImageList & OBS_XT_SMALLICON) {
            pOldImageList = pBarFolder->GetSmallImageList();
            pBarFolder->SetSmallImageList(pImageList);
        }
        
        else if (dwImageList & OBS_XT_LARGEICON) {
            pOldImageList = pBarFolder->GetLargeImageList();
            pBarFolder->SetLargeImageList(pImageList);
        }
        
        return pOldImageList;
    }
    return NULL;
}

void CXTOutBarCtrl::PaintItems(CDC* pDC, const int iFolder, CRect rc)
{
    if (m_arFolder.GetSize() != 0)
    {
        ASSERT(iFolder >= 0 && iFolder < GetFolderCount());
        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(iFolder);
        int max = pBarFolder->GetItemCount(), t;
        CRect irc;
        GetInsideRect(irc);
        
        int nSavedDC = pDC->SaveDC();
        
        CRgn rgn;
        rgn.CreateRectRgnIndirect(&irc);
        pDC->SelectClipRgn(&rgn);

        for (t = m_nFirstItem; t < max; t++)
        {
            CRect rcItem;
            GetItemRect(iFolder, t, rcItem);
            if (rcItem.top > rc.bottom) break;
            else DrawItem(pDC, iFolder, rcItem, t);
        }
        
        pDC->RestoreDC(nSavedDC);
        rgn.DeleteObject();
    }
}

CSize CXTOutBarCtrl::GetItemSize(const int iFolder, const int iIndex, const int iType)
{
    if (m_arFolder.GetSize() != 0)
    {
        ASSERT(iFolder >= 0 && iFolder < GetFolderCount());
        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(iFolder);

        ASSERT(iIndex >= 0 && iIndex < pBarFolder->GetItemCount());
        CXTBarItem* pBarItem = pBarFolder->GetItemAt(iIndex);

        CSize szImage(0,0);
        CSize szLabel(0,0);
        CSize szAll(0,0);

        if (pBarItem->GetIndex() >= 0)
        {
            if (iType != RC_OB_LABEL)
            {
                CImageList* il = GetFolderImageList(iFolder, IsSmallIconView(iFolder));
                ASSERT(il);
                if (il)
                {
                    IMAGEINFO ii;
                    il->GetImageInfo(pBarItem->GetIndex(), &ii);
                    szImage = CRect(ii.rcImage).Size();
                }
            }
        }
        if (pBarItem->GetName().GetLength())
        {
            if (iType != RC_OB_ICON)
            {
                CClientDC dc(this);
                CFont* oft = (CFont*)dc.SelectObject(GetFontX());
                if (IsSmallIconView(iFolder))
                {
                    szLabel = dc.GetTextExtent(pBarItem->GetName(), pBarItem->GetName().GetLength());
                }
                else
                {
                    CRect rc;
                    GetInsideRect(rc);
                    rc.bottom = rc.top;
                    dc.DrawText(pBarItem->GetName(), pBarItem->GetName().GetLength(), rc,
                        DT_CALCRECT | DT_CENTER | DT_VCENTER | DT_WORDBREAK | DT_END_ELLIPSIS);
                    szLabel = rc.Size();
                }
                dc.SelectObject(oft);
            }
        }
        if (IsSmallIconView(iFolder))
        {
            if (iType == RC_OB_ICON) szAll = szImage;
            else if (iType == RC_OB_LABEL) szAll = szLabel;
            else if (iType == RC_OB_BOTH) szAll = CSize(szImage.cx + szLabel.cx + m_sizeOffset.cx, szImage.cy > szLabel.cy ? szImage.cy : szLabel.cy);
        }
        else
        {
            if (iType == RC_OB_ICON) szAll = szImage;
            else if (iType == RC_OB_LABEL) szAll = szLabel;
            else if (iType == RC_OB_BOTH) 
            {
                szAll = CSize(szImage.cx > szLabel.cx ? szImage.cx : szLabel.cx, szLabel.cy + szImage.cy + m_sizeOffset.cy + m_nIconSpacingLarge);
            }
        }
        return szAll;
    }
    return 0;
}

CImageList* CXTOutBarCtrl::GetFolderImageList(const int iIndex, const BOOL bSmall) const
{
    if (m_arFolder.GetSize() != 0)
    {
        ASSERT(iIndex >= 0 && iIndex < GetFolderCount());
        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(iIndex);
        if (bSmall)
        {
            if (pBarFolder->GetSmallImageList())
                return pBarFolder->GetSmallImageList();
            else
                return m_pSmallImageList;
        }
        if (pBarFolder->GetLargeImageList())
            return pBarFolder->GetLargeImageList();
        else
            return m_pLargeImageList;
    }
    return NULL;
}

void CXTOutBarCtrl::DrawItem(CDC* pDC, const int iFolder, CRect rc, const int iIndex, const BOOL bOnlyImage)
{
    if (m_arFolder.GetSize() != 0)
    {
        CImageList* pImageList = GetFolderImageList(iFolder, IsSmallIconView(iFolder));

        ASSERT(iFolder >= 0 && iFolder < GetFolderCount());
        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(iFolder);

        ASSERT(iIndex >= 0 && iIndex < pBarFolder->GetItemCount());
        CXTBarItem* pBarItem = pBarFolder->GetItemAt(iIndex);

        ASSERT(pBarItem && pImageList);

        CFont* oft = (CFont*)pDC->SelectObject(GetFontX());
        COLORREF ocr = pDC->SetTextColor(clrWindow);
        int obk = pDC->SetBkMode(TRANSPARENT);

        CRect rcFolder;
        GetFolderRect(iFolder, rcFolder);
        rc.left  = rcFolder.left +4;
        rc.right = rcFolder.right-4;

        // Exclude the up and down arrows from painting.
        if (m_bUpArrow)
            pDC->ExcludeClipRect(&m_rcUpArrow);
        if (m_bDownArrow)
            pDC->ExcludeClipRect(&m_rcDownArrow);

        // set the items text color.
        pDC->SetTextColor(m_clrText);

        if (IsSmallIconView(iFolder))
        {
            if (pImageList)
            {
                IMAGEINFO ii;
                pImageList->GetImageInfo(pBarItem->GetIndex(), &ii);
                CSize szImage = CRect(ii.rcImage).Size();
                CPoint pt;
                pt.x = rc.left + 2;
                pt.y = rc.top + (rc.Height() - szImage.cy) / 2;
                pImageList->Draw(pDC, pBarItem->GetIndex(), pt, ILD_NORMAL);

                if (!bOnlyImage)
                {
                    rc.left  += (szImage.cx + m_sizeOffset.cx);
                    pDC->DrawText(pBarItem->GetName(), rc,
                        DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
                }
            }
        }
        else
        {
            if (pImageList)
            {
                IMAGEINFO ii;
                pImageList->GetImageInfo(pBarItem->GetIndex(), &ii);
                CSize szImage = CRect(ii.rcImage).Size();
                CPoint pt;
                pt.x = rc.left + (rc.Width() - szImage.cx) / 2;
                pt.y = rc.top;// + (rc.Height() - szImage.cy) / 2;
                pImageList->Draw(pDC, pBarItem->GetIndex(), pt, ILD_NORMAL);

                if (!bOnlyImage)
                {
                    rc.top += szImage.cy + m_sizeOffset.cy;
                    rc.bottom += 4;
                    pDC->DrawText(pBarItem->GetName(), rc,
                        DT_CENTER | DT_VCENTER | DT_WORDBREAK | DT_END_ELLIPSIS);

                }
            }
        }
        
        if ((m_dwFlags & OBS_XT_SELHIGHLIGHT) && (m_nLastItemSelected == iIndex) && (m_nLastItemSelected >= 0))
        {
            CRect rc;
            GetIconRect(m_nSelFolder, m_nLastItemSelected, rc);
            rc.InflateRect(1,1);
            if (bXPMode) {
                pDC->Draw3dRect(rc, clrHighlight, clrHighlight);
            }
            else {
                pDC->Draw3dRect(rc, clrWindowFrame, clr3DFace);
            }
        }

        pDC->SetTextColor(ocr);
        pDC->SelectObject(oft);
        pDC->SetBkMode(obk);
    }
}

BOOL CXTOutBarCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
    CPoint pt(GetMessagePos());
    ScreenToClient(&pt);
    int iIndex, ht = HitTestEx(pt, iIndex);
    if (ht == FOLDER)
        return TRUE;
        
    return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CXTOutBarCtrl::GetVisibleRange(const int iFolder, int& iFirst, int& iLast)
{
    iFirst = m_nFirstItem;
    CRect rc;
    GetInsideRect(rc);

    if (m_arFolder.GetSize() != 0)
    {
        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(iFolder);
        int max = pBarFolder->GetItemCount(), t;
        for (t = m_nFirstItem; t < max; t++)
        {
            CRect rcItem;
            GetItemRect(iFolder, t, rcItem);
            if (rcItem.bottom > rc.bottom) 
            {
                iLast = t - 1;
                break;
            }
            else iLast = t;
        }
    }
}

void CXTOutBarCtrl::OnSize(UINT nType, int cx, int cy) 
{
    m_bUpArrow = m_bDownArrow = FALSE;

    CWnd::OnSize(nType, cx, cy);

    int t, max = GetFolderCount();
    CRect rc;
    GetInsideRect(rc);

    for (t = 0; t < max; t++)
    {
        CWnd* pWnd = GetFolderChild(t);
        if (pWnd) pWnd->SetWindowPos(0, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER);
    }
}

void CXTOutBarCtrl::DrawIcon(CDC* pDC, int iIcon, int iFolder, bool bHilight)
{
    // Clear the icon background
    CRect rcIcon;
    GetIconRect(iFolder, iIcon, rcIcon);

	if (!IsSmallIconView(iFolder))
	{
		rcIcon.InflateRect(1,1);
	}
	else
	{
		rcIcon.left--;
		rcIcon.top--;
	}

    pDC->FillSolidRect(rcIcon, m_clrBack);

    // Should we hilight the icon?
    if (bHilight)
    {
        if (bXPMode)
        {
            if (m_bIconPressed) {
                pDC->FillSolidRect(rcIcon, clrXPHighlightLight);
            }
            else {
                pDC->FillSolidRect(rcIcon, clrXPHighlight);
            }
            pDC->Draw3dRect(rcIcon, clrHighlight, clrHighlight);
        }
        else
        {
            if (m_bIconPressed) {
                pDC->Draw3dRect(rcIcon, clrWindowFrame, clr3DFace);
            }
            else {
                pDC->Draw3dRect(rcIcon, clr3DFace, clrWindowFrame);
            }
        }
    }
    
    // Draw the icon using the image list.
    ASSERT(iFolder >= 0 && iFolder < GetFolderCount());
    CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(iFolder);
    
    ASSERT(iIcon >= 0 && iIcon < pBarFolder->GetItemCount());
    CXTBarItem* pBarItem = pBarFolder->GetItemAt(iIcon);
    
    CImageList* pImageList = GetFolderImageList(iFolder, IsSmallIconView(iFolder));
    ASSERT(pBarItem && pImageList);

	CRect rc;
    GetItemRect(iFolder, iIcon, rc);
    
	CRect rcFolder;
	GetFolderRect(iFolder, rcFolder);
	rc.left  = rcFolder.left +4;
	rc.right = rcFolder.right-4;
	
	if (IsSmallIconView(iFolder))
	{
		if (pImageList)
		{
			IMAGEINFO ii;
			pImageList->GetImageInfo(pBarItem->GetIndex(), &ii);
			CSize szImage = CRect(ii.rcImage).Size();
			CPoint pt;
			pt.x = rc.left + 2;
			pt.y = rc.top + (rc.Height() - szImage.cy) / 2;
			pImageList->Draw(pDC, pBarItem->GetIndex(), pt, ILD_NORMAL);
		}
	}
	else
	{
		if (pImageList)
		{
			IMAGEINFO ii;
			pImageList->GetImageInfo(pBarItem->GetIndex(), &ii);
			CSize szImage = CRect(ii.rcImage).Size();
			CPoint pt;
			pt.x = rc.left + (rc.Width() - szImage.cx) / 2;
			pt.y = rc.top;// + (rc.Height() - szImage.cy) / 2;
			pImageList->Draw(pDC, pBarItem->GetIndex(), pt, ILD_NORMAL);
		}
	}
}

void CXTOutBarCtrl::HighlightItem(const int iIndex, const BOOL bPressed)
{
    static BOOL bOldPressed = FALSE;

    CWnd* pFocus = GetFocus();
    if (pFocus != NULL && pFocus != this && IsChild(pFocus)) {
        return;
    }

    if (m_nItemHilighted == iIndex && bOldPressed == bPressed) {
        return;
    }

    m_bIconPressed = FALSE;
    bOldPressed = bPressed;

    XT_OUTBAR_INFO obi;
    obi.nIndex    = iIndex;
    obi.nDragFrom = -1;
    obi.nDragTo   = -1;
    obi.lpszText  = GetItemText(iIndex);
	obi.bFolder   = false;

    CWnd* pOwner = GetOwner();
    ASSERT_VALID(pOwner);

	LRESULT lResult = pOwner->SendMessage(XTWM_OUTBAR_NOTIFY,
		OBN_XT_ITEMHOVER, (LPARAM)&obi);

    if (lResult == TRUE)
    {
        CClientDC dc(this);
        
        CRect irc;
        GetInsideRect(irc);
        int nSavedDC = dc.SaveDC();
        
        CRgn rgn;
        rgn.CreateRectRgnIndirect(&irc);
        dc.SelectClipRgn(&rgn);

        // Exclude the up and down arrows from painting.
        if (m_bUpArrow)
            dc.ExcludeClipRect(&m_rcUpArrow);
        if (m_bDownArrow)
            dc.ExcludeClipRect(&m_rcDownArrow);
        
        if (m_nItemHilighted >= 0 && IsValidItem(m_nItemHilighted))
        {
            DrawIcon(&dc, m_nItemHilighted, m_nSelFolder, false);
        }
        if (m_nSelAnimTiming > 0 && iIndex == m_nLastItemSelected)
        {
            m_nItemHilighted = -1;
            dc.RestoreDC(nSavedDC);
            rgn.DeleteObject();
            return;
        }
        if ((m_dwFlags & OBS_XT_SELHIGHLIGHT) && (m_nLastItemSelected == iIndex)) {
            m_nItemHilighted = -1;
            dc.RestoreDC(nSavedDC);
            rgn.DeleteObject();
            return;
        }
        
        m_nItemHilighted = iIndex;
        m_bIconPressed = bPressed;

        if (m_nItemHilighted >= 0 && IsValidItem(m_nItemHilighted))
        {
            DrawIcon(&dc, m_nItemHilighted, m_nSelFolder, true);
        }

        dc.RestoreDC(nSavedDC);
        rgn.DeleteObject();
    }
}

void CXTOutBarCtrl::GetIconRect(const int iFolder, const int iIndex, CRect&  rect)
{
    CRect rc;
    GetInsideRect(rc);
    int top = rc.top;
    CSize sz(0,0);
    int y = 0;
    int spacing = IsSmallIconView(iFolder) ? m_nIconSpacingSmall : m_nIconSpacingLarge;

    for (int t = 0; t < iIndex; t++)
    {
        sz = GetItemSize(iFolder, t, RC_OB_BOTH);
        top += sz.cy;
        top += spacing;
        if (t == m_nFirstItem - 1) y = top - rc.top;
    }
    top += spacing;

    sz = GetItemSize(iFolder, iIndex, RC_OB_ICON);
    if (IsSmallIconView(iFolder))
    {
        rect.SetRect(rc.left, top, rc.left + sz.cx, top + sz.cy);
        rect.left += m_sizeMargin.cx + 2;
        rect.right += m_sizeMargin.cx + 2;
        rect.top -= m_sizeMargin.cy;
        rect.bottom -= m_sizeMargin.cy;

        rect.top -= y;
        rect.bottom -= y;
    }
    else
    {
        rect.SetRect(rc.left + (rc.Width() - sz.cx)/2, top, rc.left + (rc.Width() - sz.cx)/2 + sz.cx, top + sz.cy);
        rect.top -= y + 3;
        rect.bottom -= y + 2;
    }
}

void CXTOutBarCtrl::GetLabelRect(const int iFolder, const int iIndex, CRect&  rect)
{
    CRect rc;
    GetInsideRect(rc);
    int top = rc.top;
    CSize sz(0,0);
    int y = 0;
    int spacing = IsSmallIconView(iFolder) ? m_nIconSpacingSmall : m_nIconSpacingLarge;

    for (int t = 0; t < iIndex; t++)
    {
        sz = GetItemSize(iFolder, t, RC_OB_BOTH);
        top += sz.cy;
        top += spacing;
        if (t == m_nFirstItem - 1) y = top - rc.top;
    }
    top += spacing;

    sz = GetItemSize(iFolder, iIndex, RC_OB_BOTH);

    CSize szLabel = GetItemSize(iFolder, iIndex, RC_OB_LABEL);

    if (IsSmallIconView(iFolder))
    {
        rect.SetRect(rc.left, top, rc.left + sz.cx, top + sz.cy);
        rect.left += m_sizeMargin.cx + 2;
        rect.right += m_sizeMargin.cx + 2;
        rect.top  += m_sizeMargin.cy - 5;
        rect.bottom += m_sizeMargin.cy - 5;
    }
    else
    {
        rect.SetRect(rc.left + (rc.Width() - sz.cx)/2, top, rc.left + (rc.Width() - sz.cx)/2 + sz.cx, top + sz.cy);
        rect.top -= y + 3;
        rect.bottom -= y + 2;

        rect.bottom -=  m_nIconSpacingLarge;
        rect.top    = rect.bottom - szLabel.cy;
    }
}

void CXTOutBarCtrl::StartGroupEdit(const int iIndex)
{
    CRect rc;
    GetFolderRect(iIndex, rc);
    rc.InflateRect(-2,-2);

    if (m_arFolder.GetSize() != 0)
    {
        ASSERT(iIndex >= 0 && iIndex < GetFolderCount());
        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(iIndex);
        
        CXTEditItem* pEdit = new CXTEditItem;
        pEdit->m_nIndex = iIndex;

        pEdit->Create(WS_CHILD|WS_VISIBLE|ES_CENTER|ES_MULTILINE,rc,this,1);
        pEdit->m_wParam = OBN_XT_ONGROUPENDEDIT;
        pEdit->ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);

        pEdit->SetWindowText(pBarFolder->GetName());
        pEdit->SetFocus();
        pEdit->SetSel(0,-1);
    }
}

void CXTOutBarCtrl::StartItemEdit(const int iIndex)
{
    if (m_arFolder.GetSize() != 0)
    {
        ASSERT(m_nSelFolder >= 0 && m_nSelFolder < GetFolderCount());
        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(m_nSelFolder);
        ASSERT(iIndex >= 0 && iIndex < pBarFolder->GetItemCount());
        CXTBarItem* pBarItem = pBarFolder->GetItemAt(iIndex);

        CRect rc, crc;
        GetLabelRect(m_nSelFolder, iIndex, rc);
        GetInsideRect(crc);
        
        DWORD dwFlag = WS_CHILD|WS_VISIBLE|WS_BORDER;

        CXTEditItem* pEdit = new CXTEditItem;

        if (!IsSmallIconView(m_nSelFolder))
        {
            rc.left = crc.left + 5;
            rc.right = crc.right - 5;
            rc.bottom += 5;
            dwFlag |= ES_CENTER|ES_MULTILINE;
        }
        else 
        {
            CRect irc;
            GetIconRect(m_nSelFolder, iIndex, irc);
            dwFlag |= ES_AUTOHSCROLL;
            rc.OffsetRect(irc.Width()+1, -6);
            pEdit->m_bNoDown = TRUE;
        }

        pEdit->m_nIndex = iIndex;
        pEdit->Create(dwFlag, rc, this, 1);
        pEdit->m_wParam = OBN_XT_ONLABELENDEDIT;
        pEdit->SetWindowText(pBarItem->GetName());
        pEdit->SetFocus();
        pEdit->SetSel(0,-1);
    }
}

void CXTOutBarCtrl::OnLargeIcon() 
{
    SetSmallIconView(FALSE, m_nSelFolder);
    Invalidate();
}

void CXTOutBarCtrl::OnUpdateLargeIcon(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable(IsSmallIconView(m_nSelFolder));
}

void CXTOutBarCtrl::OnSmallIcon() 
{
    SetSmallIconView(TRUE, m_nSelFolder); 
    Invalidate();   
}

void CXTOutBarCtrl::OnUpdateSmallIcon(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable(!IsSmallIconView(m_nSelFolder));
}

void CXTOutBarCtrl::OnRemoveItem() 
{
    if (m_nHitInternal1 == FOLDER)
    {
        RemoveFolder(m_nHitInternal2);
    }
    else if (m_nHitInternal1 == ITEM)
    {
        RemoveItem(m_nHitInternal2);
    }
}

void CXTOutBarCtrl::OnUpdateRemoveItem(CCmdUI* /*pCmdUI*/) 
{
        
}

void CXTOutBarCtrl::OnRenameItem() 
{
    if (m_nHitInternal1 == FOLDER)
    {
        StartGroupEdit(m_nHitInternal2);
    }
    else if (m_nHitInternal1 == ITEM)
    {
        StartItemEdit(m_nHitInternal2);
    }
}

void CXTOutBarCtrl::OnUpdateRenameItem(CCmdUI* /*pCmdUI*/) 
{

}

void CXTOutBarCtrl::RemoveItem(const int iIndex)
{
    if (m_arFolder.GetSize() != 0)
    {
        ASSERT(m_nSelFolder >= 0 && m_nSelFolder < GetFolderCount());
        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(m_nSelFolder);
        ASSERT(iIndex >= 0 && iIndex < pBarFolder->GetItemCount());

        if (IsValidItem(iIndex))
        {
            XT_OUTBAR_INFO obi;
            obi.nIndex    = iIndex;
            obi.nDragFrom = -1;
            obi.nDragTo   = -1;
            obi.lpszText  = GetItemText(iIndex);
			obi.bFolder   = false;
            
            CWnd* pOwner = GetOwner();
            ASSERT_VALID(pOwner);

			LRESULT lResult = pOwner->SendMessage(XTWM_OUTBAR_NOTIFY,
                OBN_XT_DELETEITEM, (LPARAM)&obi);
            
            if (lResult == TRUE)
            {
                CXTBarItem* pBarItem = pBarFolder->RemoveItemAt(iIndex);
                delete(pBarItem);

				if (m_nLastItemSelected == iIndex)
					m_nLastItemSelected = -1;

                InvalidateRect(NULL);
            }
        }
    }
}

BOOL CXTOutBarCtrl::IsValidItem(const int iIndex) const
{
    if (m_arFolder.GetSize() != 0)
    {
        ASSERT(m_nSelFolder >= 0 && m_nSelFolder < GetFolderCount());
        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(m_nSelFolder);
        return (iIndex >= 0 && iIndex < pBarFolder->GetItemCount());
    }
    return FALSE;
}

DWORD CXTOutBarCtrl::GetItemData(const int iIndex) const
{
    if (m_arFolder.GetSize() != 0)
    {
        ASSERT(m_nSelFolder >= 0 && m_nSelFolder < GetFolderCount());
        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(m_nSelFolder);
        if (IsValidItem(iIndex))
        {
            CXTBarItem* i = pBarFolder->GetItemAt(iIndex);
            return i->GetData();
        }
    }
    return 0;
}

int CXTOutBarCtrl::GetItemImage(const int iIndex) const
{
    if (m_arFolder.GetSize() != 0)
    {
        ASSERT(m_nSelFolder >= 0 && m_nSelFolder < GetFolderCount());
        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(m_nSelFolder);
        if (IsValidItem(iIndex))
        {
            CXTBarItem* i = pBarFolder->GetItemAt(iIndex);
            return i->GetIndex();
        }
    }
    return 0;
}

void CXTOutBarCtrl::SetItemData(const int iIndex, const DWORD dwData)
{
    if (m_arFolder.GetSize() != 0)
    {
        ASSERT(m_nSelFolder >= 0 && m_nSelFolder < GetFolderCount());
        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(m_nSelFolder);
        if (IsValidItem(iIndex))
        {
            CXTBarItem* i = pBarFolder->GetItemAt(iIndex);
            i->SetData( dwData );
        }
    }
}

void CXTOutBarCtrl::SetItemImage(const int iIndex, const int iImage)
{
    if (m_arFolder.GetSize() != 0)
    {
        ASSERT(m_nSelFolder >= 0 && m_nSelFolder < GetFolderCount());
        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(m_nSelFolder);
        if (IsValidItem(iIndex))
        {
            CXTBarItem* i = pBarFolder->GetItemAt(iIndex);
            i->SetIndex( iImage );
        }
    }
}

void CXTOutBarCtrl::DrawDragArrow(CDC* pDC, const int /*iFrom*/, const int iTo)
{
    if (iTo == m_nLastDragItemDraw) return;
    CRect rc1;
    if (m_nLastDragItemDraw >= 0)
    {
        GetDragItemRect(m_nLastDragItemDraw, rc1);
        if (m_nLastDragItemDrawType == 0) rc1.bottom = rc1.top + 5;
        else if (m_nLastDragItemDrawType == 1) { rc1.top -= 4; rc1.bottom = rc1.top + 9; }
        else if (m_nLastDragItemDrawType == 2) { rc1.top -= 4; rc1.bottom = rc1.top + 5; }
        InvalidateRect(rc1, TRUE);
        UpdateWindow();
    }
    m_nLastDragItemDraw = iTo;

    if (m_nLastDragItemDraw >= 0)
    {
        m_nLastDragItemDrawType = GetDragItemRect(m_nLastDragItemDraw, rc1);

        CPen* op = pDC->SelectObject(&m_penBlack);

        pDC->MoveTo(rc1.left, rc1.top);
        pDC->LineTo(rc1.right, rc1.top);

        if (m_nLastDragItemDrawType != 2)
        {
            pDC->MoveTo(rc1.left, rc1.top+1);
            pDC->LineTo(rc1.left + 7, rc1.top+1);
            pDC->MoveTo(rc1.left, rc1.top+2);
            pDC->LineTo(rc1.left + 5, rc1.top+2);
            pDC->MoveTo(rc1.left, rc1.top+3);
            pDC->LineTo(rc1.left + 3, rc1.top+3);
            pDC->MoveTo(rc1.left, rc1.top+4);
            pDC->LineTo(rc1.left + 1, rc1.top+4);

            pDC->MoveTo(rc1.right-1, rc1.top+1);
            pDC->LineTo(rc1.right - 7, rc1.top+1);
            pDC->MoveTo(rc1.right-1, rc1.top+2);
            pDC->LineTo(rc1.right - 5, rc1.top+2);
            pDC->MoveTo(rc1.right-1, rc1.top+3);
            pDC->LineTo(rc1.right - 3, rc1.top+3);
        }
        if (m_nLastDragItemDrawType != 0)
        {
            pDC->MoveTo(rc1.left, rc1.top-1);
            pDC->LineTo(rc1.left + 7, rc1.top-1);
            pDC->MoveTo(rc1.left, rc1.top-2);
            pDC->LineTo(rc1.left + 5, rc1.top-2);
            pDC->MoveTo(rc1.left, rc1.top-3);
            pDC->LineTo(rc1.left + 3, rc1.top-3);
            pDC->MoveTo(rc1.left, rc1.top-4);
            pDC->LineTo(rc1.left + 1, rc1.top-4);

            pDC->MoveTo(rc1.right-1, rc1.top-1);
            pDC->LineTo(rc1.right - 7, rc1.top-1);
            pDC->MoveTo(rc1.right-1, rc1.top-2);
            pDC->LineTo(rc1.right - 5, rc1.top-2);
            pDC->MoveTo(rc1.right-1, rc1.top-3);
            pDC->LineTo(rc1.right - 3, rc1.top-3);
            pDC->MoveTo(rc1.right-1, rc1.top-4);
            pDC->LineTo(rc1.right - 1, rc1.top-4);
        }

        pDC->SelectObject(op);
    }
}

int CXTOutBarCtrl::GetDragItemRect(const int iIndex, CRect&  rect)
{
    CRect rc, crc;
    GetInsideRect(crc);
    crc.InflateRect(-2, 0);

    GetItemRect(m_nSelFolder, iIndex < GetItemCount() ? iIndex : iIndex - 1, rc);
    int line = 0;
    if (iIndex < GetItemCount())
    {
        line = rc.top - 4;
    }
    else
    {
        line = rc.bottom + 6;
    }
    int tpe;
    if (iIndex == 0)
    {
        rect.SetRect(crc.left, line+2, crc.right, line + 7);
        tpe = 0;
    }
    else if (iIndex < GetItemCount())
    {
        rect.SetRect(crc.left, line - 9, crc.right, line);
        tpe = 1;
        if (IsSmallIconView(m_nSelFolder)) 
        {
            rect.top += 8;
            rect.bottom += 8;
        }
    }
    else
    {
        rect.SetRect(crc.left, line, crc.right, line + 5);
        tpe = 2;
    }
    return tpe;
}

void CXTOutBarCtrl::AnimateFolderScroll(const int iFrom, const int iTo)
{
    if (m_arFolder.GetSize() != 0)
    {
        ASSERT(iFrom >= 0 && iFrom < GetFolderCount());
        ASSERT(iTo >= 0 && iTo < GetFolderCount());

        CRect rc, rc1, frc, frc1;
        GetInsideRect(rc);
        rc1.SetRect(0,0,rc.Width(),rc.Height());
        GetFolderRect(iTo, frc);
        frc1 = frc;

        CClientDC dc(this);
        CDC memDC;
        memDC.CreateCompatibleDC(&dc);

        int nSavedDC = memDC.SaveDC();
        
        CBitmap bmpFrom, bmpTo;
        bmpFrom.CreateCompatibleBitmap(&dc, rc1.Width(), rc1.Height() + frc.Height()* 2);
        bmpTo.CreateCompatibleBitmap(&dc, rc1.Width(), rc1.Height() + frc.Height()* 2);

        CWnd* pWnd1 = GetFolderChild(iFrom);
        CWnd* pWnd2 = GetFolderChild(iTo);

        memDC.SelectObject(&bmpFrom);
        if (iTo > iFrom)
        {
            rc1.bottom += frc.Height()* 2;
            memDC.FillSolidRect(rc1, m_clrBack);
            memDC.Draw3dRect(rc1, clrWindowFrame, clr3DShadow);

            if (pWnd1) 
            {
                BOOL bPrev = pWnd1->ShowWindow(SW_SHOW);
                pWnd1->SendMessage(WM_ERASEBKGND, (WPARAM) memDC.GetSafeHdc(), (LPARAM) 0);
                pWnd1->SendMessage(WM_PAINT, (WPARAM) memDC.GetSafeHdc(), (LPARAM) 0);
                if (!bPrev) pWnd1->ShowWindow(SW_HIDE);
            }
            else PaintItems(&memDC, iFrom, rc1);

            memDC.SelectObject(&bmpTo);
            frc.SetRect(0,0,frc.Width(),frc.Height());
            rc1.SetRect(0,frc.Height(),rc.Width(), rc.Height()+frc.Height()* 2);
            memDC.FillSolidRect(rc1, m_clrBack);
            memDC.Draw3dRect(rc1, clrWindowFrame, clr3DShadow);

            if (pWnd2) 
            {
                CPoint ovpt = memDC.SetViewportOrg(0, frc.Height());
                BOOL bPrev = pWnd2->ShowWindow(SW_SHOW);
                pWnd2->SendMessage(WM_ERASEBKGND, (WPARAM) memDC.GetSafeHdc(), (LPARAM) 0);
                pWnd2->SendMessage(WM_PAINT, (WPARAM) memDC.GetSafeHdc(), (LPARAM) 0);
                if (!bPrev) pWnd2->ShowWindow(SW_HIDE);
                memDC.SetViewportOrg(ovpt);
            }
            else PaintItems(&memDC, iTo, rc1);
            DrawFolder(&memDC, iTo, frc, F_NORMAL);
        }
        else
        {
            frc.SetRect(0,0,frc.Width(),frc.Height());
            rc1.top += frc.Height();
            rc1.bottom += frc.Height()* 2;
            memDC.FillSolidRect(rc1, m_clrBack);
            memDC.Draw3dRect(rc1, clrWindowFrame, clr3DShadow);

            if (pWnd1) 
            {
                CPoint ovpt = memDC.SetViewportOrg(0, frc.Height());
                BOOL bPrev = pWnd1->ShowWindow(SW_SHOW);
                pWnd1->SendMessage(WM_ERASEBKGND, (WPARAM) memDC.GetSafeHdc(), (LPARAM) 0);
                pWnd1->SendMessage(WM_PAINT, (WPARAM) memDC.GetSafeHdc(), (LPARAM) 0);
                if (!bPrev) pWnd1->ShowWindow(SW_HIDE);
                memDC.SetViewportOrg(ovpt);
            }
            else PaintItems(&memDC, iFrom, rc1);

            DrawFolder(&memDC, iFrom, frc, F_NORMAL);

            memDC.SelectObject(&bmpTo);
            rc1.SetRect(0,0,rc.Width(), rc.Height() + frc.Height()* 2);
            memDC.FillSolidRect(rc1, m_clrBack);
            memDC.Draw3dRect(rc1, clrWindowFrame, clr3DShadow);

            if (pWnd2) 
            {
                BOOL bPrev = pWnd2->ShowWindow(SW_SHOW);
                pWnd2->SendMessage(WM_ERASEBKGND, (WPARAM) memDC.GetSafeHdc(), (LPARAM) 0);
                pWnd2->SendMessage(WM_PAINT, (WPARAM) memDC.GetSafeHdc(), (LPARAM) 0);
                if (!bPrev) pWnd2->ShowWindow(SW_HIDE);
            }
            else PaintItems(&memDC, iTo, rc1);
        }

        if (iTo > iFrom)
        {
            CRect rcFrom, rcTo;
            GetFolderRect(iFrom, rcFrom);
            GetFolderRect(iTo, rcTo);
            int fh = rcFrom.Height();
            for (int y = rcTo.top - fh; y > rcFrom.bottom; y -= fh)
            {
                memDC.SelectObject(&bmpFrom);
                dc.BitBlt(rc.left, rcFrom.bottom + 1, rc.Width(), y - rcFrom.bottom - 1, &memDC, 0,fh, SRCCOPY);
                memDC.SelectObject(&bmpTo);
                dc.BitBlt(rc.left, y, rc.Width(), rc.bottom - y + fh, &memDC, 0,0, SRCCOPY);
                Sleep(m_nAnimationTickCount);
            }
        }
        else
        {
            CRect rcFrom, rcTo;
            GetFolderRect(iFrom, rcFrom);
            int fh = rcFrom.Height();
            rcTo.SetRect(rc.left, rc.bottom, rc.right, rc.bottom - fh);
            for (int y = rcFrom.top + 1; y < rcTo.top - fh; y += fh)
            {
                memDC.SelectObject(&bmpTo);
                dc.BitBlt(rc.left, rcFrom.top + 1, rc.Width(), y - rcFrom.top - 1, &memDC, 0, fh*2, SRCCOPY);
                memDC.SelectObject(&bmpFrom);
                dc.BitBlt(rc.left, y, rc.Width(), rc.bottom - y, &memDC, 0,0, SRCCOPY);
                Sleep(m_nAnimationTickCount);
            }
        }

        memDC.RestoreDC(nSavedDC);
        memDC.DeleteDC();

        bmpFrom.DeleteObject();
        bmpTo.DeleteObject();
    }
}

CString CXTOutBarCtrl::GetItemText(const int iIndex)
{
    CString item = _T("");

    if (m_arFolder.GetSize() != 0)
    {
        ASSERT(m_nSelFolder >= 0 && m_nSelFolder < GetFolderCount());
        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(m_nSelFolder);
        if (IsValidItem(iIndex))
        {
            CXTBarItem* pBarItem = pBarFolder->GetItemAt(iIndex);
            if (pBarItem->GetName().GetLength())
                item = pBarItem->GetName();
        }
    }
    return item;
}

int CXTOutBarCtrl::AddFolderBar( LPCTSTR lpszFolderName, CWnd* pWndChild, const DWORD dwData )
{
    CXTBarFolder* pBarFolder = new CXTBarFolder( lpszFolderName, dwData );
    ASSERT( pBarFolder );
    
    pBarFolder->SetChild(pWndChild);
    return  m_arFolder.Add((void*)pBarFolder);
}

CWnd* CXTOutBarCtrl::GetFolderChild(int iFolder)
{
    if (GetFolderCount())
    {
        if (iFolder < 0 && m_nSelFolder < 0)
            return NULL;

        if (iFolder < 0)
            iFolder = m_nSelFolder;

        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(iFolder);
        return pBarFolder->GetChild();
    }
    return NULL;
}

DWORD CXTOutBarCtrl::GetFolderData(int iFolder)
{
    if (iFolder < 0) iFolder = m_nSelFolder;
    CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(iFolder);
    return pBarFolder->GetData();
}

void CXTOutBarCtrl::SetAnimSelHighlight(const int iTime)
{
    if (iTime <= 0) KillTimer(3);
    else SetTimer(3, iTime, NULL);
    m_nSelAnimTiming = iTime;
}

/////////////////////////////////////////////////////////////////////////////
// CXTBarItem

CXTBarItem::CXTBarItem(LPCTSTR lpszName, const int nImageIndex, DWORD dwData) :
    m_strName(lpszName), m_nIndex(nImageIndex), m_dwData(dwData)
{

}

CXTBarItem::~CXTBarItem()
{

}

/////////////////////////////////////////////////////////////////////////////
// CXTBarFolder

CXTBarFolder::CXTBarFolder(LPCTSTR lpszName, DWORD dwData) :
    m_strName(lpszName), m_dwData(dwData), m_pLargeList(NULL), m_pSmallList(NULL), m_pChild(NULL)
{

}

CXTBarFolder::~CXTBarFolder()
{
    while(!m_barItems.IsEmpty()) 
	{
        CXTBarItem* pBarItem = m_barItems.RemoveTail();
        delete(pBarItem);
    }
}

int CXTBarFolder::InsertItem(int iIndex, LPCTSTR lpszName, const int nImage, const DWORD dwData)
{
    if(( iIndex < 0 ) || (iIndex > GetItemCount())) {
        iIndex = GetItemCount();
    }
    
    CXTBarItem* pBarItem = new CXTBarItem(lpszName, nImage, dwData);
    ASSERT(pBarItem);
    
    InsertItemAt(iIndex, pBarItem);
    
    return iIndex;
}

CXTBarItem* CXTBarFolder::GetItemAt(int iIndex)
{
    POSITION pos = m_barItems.FindIndex(iIndex);
    return m_barItems.GetAt(pos);
}

void CXTBarFolder::InsertItemAt(int iIndex, CXTBarItem* pBarItem)
{
    POSITION pos = m_barItems.FindIndex(iIndex);
    m_barItems.InsertAfter(pos, pBarItem);
}

CXTBarItem* CXTBarFolder::RemoveItemAt(int iIndex)
{
    POSITION pos = m_barItems.FindIndex(iIndex);
    CXTBarItem* pBarItem = m_barItems.GetAt(pos);
    m_barItems.RemoveAt(pos);
    return pBarItem;
}

void CXTOutBarCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	XT_OUTBAR_INFO obi = { 0 };

	int ht = HitTestEx(point, obi.nIndex);
	if (ht != ITEM)
	{
		obi.bFolder = true;

		if (ht != FOLDER)
		{
			obi.nIndex = -1;
		}
	}
	
	CWnd* pOwner = GetOwner();
	ASSERT_VALID(pOwner);

	// allow users to add their own right click handlers.
	LRESULT lResult = pOwner->SendMessage(XTWM_OUTBAR_NOTIFY,
		OBN_XT_ITEMRCLICK, (LPARAM)&obi);
	
	if (lResult == TRUE)
	{
		m_nHitInternal1 = HitTestEx(point, m_nHitInternal2);
		
		CPoint pt = point;
		ClientToScreen(&pt);

		CMenu menu;
		menu.CreatePopupMenu();
		menu.AppendMenu(MF_ENABLED,XT_IDC_SMALLICON,"Ð¡Í¼±ê");
		menu.AppendMenu(MF_ENABLED,XT_IDC_LARGEICON,"´óÍ¼±ê");
		if (m_nHitInternal1 == ITEM)
		{
			if ((m_dwFlags & OBS_XT_REMOVEITEMS) || (m_dwFlags & OBS_XT_EDITITEMS))
			{
				menu.AppendMenu(MF_SEPARATOR);
				if (m_dwFlags & OBS_XT_REMOVEITEMS) 
				{
					menu.AppendMenu(MF_STRING, XT_IDC_REMOVEITEM, "É¾³ý");
				}
				if (m_dwFlags & OBS_XT_EDITITEMS) 
				{
					menu.AppendMenu(MF_STRING, XT_IDC_RENAMEITEM, "ÖØÃüÃû");
				}
			}
		}
		else if (m_nHitInternal1 == FOLDER)
		{
			if ((m_dwFlags & OBS_XT_REMOVEGROUPS) || (m_dwFlags & OBS_XT_EDITGROUPS))
			{
				menu.AppendMenu(MF_SEPARATOR);
				
				if (m_dwFlags & OBS_XT_REMOVEGROUPS) 
				{
					menu.AppendMenu(MF_STRING, XT_IDC_REMOVEITEM, "É¾³ý");
				}
				if (m_dwFlags & OBS_XT_EDITGROUPS) 
				{
					menu.AppendMenu(MF_STRING, XT_IDC_RENAMEITEM, "ÖØÃüÃû");
				}
			}
		}
		
		menu.CheckMenuItem( IsSmallIconView(m_nSelFolder) ? XT_IDC_SMALLICON : XT_IDC_LARGEICON, MF_CHECKED );		
		menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y,this);
	}
	
	CWnd::OnRButtonUp(nFlags, point);
}

void CXTOutBarCtrl::OnDestroy() 
{
    CWnd* pOwner = (CWnd*)GetOwner();
    ASSERT_VALID(pOwner);

	// send notification for each folder / item that is deleted.
    for (int iFolder = 0; iFolder < m_arFolder.GetSize(); iFolder++)
    {
		m_nSelFolder = iFolder;
        CXTBarFolder* pBarFolder = (CXTBarFolder*)m_arFolder.GetAt(iFolder);

		XT_OUTBAR_INFO obif;
		obif.nIndex    = iFolder;
		obif.nDragFrom = -1;
		obif.nDragTo   = -1;
		obif.lpszText  = pBarFolder->GetName();
		obif.bFolder   = true;

		if (pBarFolder != NULL)
		{
			int iCount = pBarFolder->m_barItems.GetCount();
			
			for (int iItem = 0; iItem < iCount; iItem++)
			{
				CXTBarItem* pBarItem = pBarFolder->GetItemAt(iItem);

				XT_OUTBAR_INFO obi;
				obi.nIndex    = iItem;
				obi.nDragFrom = -1;
				obi.nDragTo   = -1;
				obi.lpszText  = pBarItem->GetName();
				obi.bFolder   = false;

				pOwner->SendMessage(XTWM_OUTBAR_NOTIFY,
					OBN_XT_DELETEITEM, (LPARAM)&obi);
			}

			pOwner->SendMessage(XTWM_OUTBAR_NOTIFY,
				OBN_XT_DELETEFOLDER, (LPARAM)&obif);
		}
    }

	CWnd::OnDestroy();
}
