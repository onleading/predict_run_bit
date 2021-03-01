// ProgressWnd.cpp : implementation file

#include "stdafx.h"
#include "ProgressWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_CANCEL   10
#define IDC_TEXT     11

/////////////////////////////////////////////////////////////////////////////
// CProgressWnd

CProgressWnd::CProgressWnd()
{
    CommonConstruct();
}

CProgressWnd::CProgressWnd(CWnd* pParent, LPCTSTR pszTitle, BOOL bSmooth /* = FALSE */)
{
    CommonConstruct();
    m_strTitle = pszTitle;

    Create(pParent, pszTitle, bSmooth);
}

void CProgressWnd::CommonConstruct()
{
    m_nNumTextLines  = 2;
    m_nPrevPos       = 0;
    m_nPrevPercent   = 0;
    m_nStep          = 1;
    m_nMinValue      = 0;
    m_nMaxValue      = 100;

    m_strTitle       = _F("Progress");
    m_strCancelLabel = _F(" 取消 ");
    m_bCancelled     = FALSE;
    m_bModal         = FALSE;
}

CProgressWnd::~CProgressWnd()
{
    DestroyWindow();
}

BOOL CProgressWnd::Create(CWnd* pParent, LPCTSTR pszTitle, BOOL bSmooth /* = FALSE */)
{
    BOOL bSuccess;

    // Register window class
    CString csClassName = AfxRegisterWndClass(CS_OWNDC|CS_HREDRAW|CS_VREDRAW,
                                              ::LoadCursor(NULL, IDC_APPSTARTING),
                                              CBrush(::GetSysColor(COLOR_BTNFACE)));

    // Get the system window message font for use in the cancel button and text area
    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0));
    m_font.CreateFontIndirect(&(ncm.lfMessageFont)); 

    if (!pParent)	pParent = AfxGetMainWnd();

    // Create popup window
    bSuccess = CreateEx(WS_EX_DLGMODALFRAME, // Extended style
                        csClassName,                       // Classname
                        pszTitle,                          // Title
                        WS_POPUP|WS_BORDER|WS_CAPTION,     // style
                        0,0,                               // position - updated soon.
                        200,130,                           // Size - updated soon
                        pParent->GetSafeHwnd(),            // handle to parent
                        0,                                 // No menu
                        NULL);    
    if (!bSuccess) return FALSE;

    // Now create the controls
    CRect TempRect(0,0,10,10);

    bSuccess = m_Text.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_NOPREFIX|SS_LEFTNOWORDWRAP,
                             TempRect, this, IDC_TEXT);
    if (!bSuccess) return FALSE;

    DWORD dwProgressStyle = WS_CHILD|WS_VISIBLE;
#ifdef PBS_SMOOTH    
    if (bSmooth)
       dwProgressStyle |= PBS_SMOOTH;
#endif
    bSuccess = m_wndProgress.Create(dwProgressStyle, TempRect, this, IDC_PROGRESS);
    if (!bSuccess) return FALSE;

    bSuccess = m_CancelButton.Create(m_strCancelLabel, 
                                     WS_CHILD|WS_VISIBLE|WS_TABSTOP| BS_PUSHBUTTON, 
                                     TempRect, this, IDC_CANCEL);
    if (!bSuccess) return FALSE;

    m_CancelButton.SetFont(&m_font, TRUE);
    m_Text.SetFont(&m_font, TRUE);

    // Resize the whole thing according to the number of text lines, desired window
    // width and current font.
    SetWindowSize(m_nNumTextLines, 210);

	CenterWindow(pParent);

    Show();

    return TRUE;
}

BOOL CProgressWnd::GoModal(LPCTSTR pszTitle /*=_T("Progress")"*/, BOOL bSmooth /*=FALSE*/)
{
    CWnd *pMainWnd = AfxGetMainWnd();

    if (!::IsWindow(m_hWnd) && !Create(pMainWnd, pszTitle, bSmooth))
        return FALSE;

    // Disable main window
    if (pMainWnd)	pMainWnd->EnableWindow(FALSE);

    // Re-enable this window
    EnableWindow(TRUE);

    m_bModal = TRUE;

    return TRUE;
}
    
void CProgressWnd::SetWindowSize(int nNumTextLines, int nWindowWidth /*=390*/)
{
    int nMargin = 10;
    CSize EdgeSize(::GetSystemMetrics(SM_CXEDGE), ::GetSystemMetrics(SM_CYEDGE));

    CRect TextRect, CancelRect, ProgressRect;
    CSize CancelSize;

    TextRect.SetRect(nMargin,nMargin, nWindowWidth-2*nMargin, 100+2*nMargin);

    // Get DrawText to tell us how tall the text area will be (while we're at
    // it, we'll see how big the word "Cancel" is)
    CDC* pDC = GetDC();
    if (pDC) {
        CFont* pOldFont = pDC->SelectObject(&m_font);
        CString str = _T("M");
        for (int i = 0; i < nNumTextLines-1; i++) str += _T("\nM");
        pDC->DrawText(str, TextRect, DT_CALCRECT|DT_NOCLIP|DT_NOPREFIX);
        TextRect.right = TextRect.left + nWindowWidth;
        CancelSize = pDC->GetTextExtent(m_strCancelLabel + _T("  ")) +
                                             CSize(EdgeSize.cx*5, EdgeSize.cy*4);
        pDC->SelectObject(pOldFont);
        ReleaseDC(pDC);
    }
    
    CancelRect.SetRect(TextRect.right-CancelSize.cx, TextRect.bottom+nMargin, 
                       TextRect.right, TextRect.bottom+nMargin + CancelSize.cy);

    ProgressRect.SetRect(TextRect.left, CancelRect.top + EdgeSize.cy, 
                         CancelRect.left-nMargin, CancelRect.bottom - EdgeSize.cy);

    CSize ClientSize(nMargin + TextRect.Width() + nMargin,
                     nMargin + TextRect.Height() + nMargin + CancelRect.Height() + nMargin);

    CRect WndRect, ClientRect;
    GetWindowRect(WndRect); GetClientRect(ClientRect);
    WndRect.right = WndRect.left + WndRect.Width()-ClientRect.Width()+ClientSize.cx;
    WndRect.bottom = WndRect.top + WndRect.Height()-ClientRect.Height()+ClientSize.cy;
    MoveWindow(WndRect);

    m_wndProgress.MoveWindow(ProgressRect);
    m_CancelButton.MoveWindow(CancelRect);
    m_Text.MoveWindow(TextRect);
}

void CProgressWnd::Hide()  
{ 
    if (!::IsWindow(GetSafeHwnd())) 
        return;

    if (IsWindowVisible())
    {
        ShowWindow(SW_HIDE);
        ModifyStyle(WS_VISIBLE, 0);
    }
}

void CProgressWnd::Show()  
{ 
    if (!::IsWindow(GetSafeHwnd()))
        return;

    if (!IsWindowVisible())
    {
        ModifyStyle(0, WS_VISIBLE);
        ShowWindow(SW_SHOWNA);
        RedrawWindow(NULL,NULL,RDW_ERASE|RDW_FRAME|RDW_INVALIDATE);
    }
}

void CProgressWnd::SetRange(int nLower, int nUpper, int nStep /* = 1 */)    
{
    if (!::IsWindow(GetSafeHwnd())) 
        return;
    m_wndProgress.SetRange32(nLower, nUpper);

    m_nMaxValue = nUpper;
    m_nMinValue = nLower;
    m_nStep     = nStep;

    m_wndProgress.SetStep(nStep);
}

int CProgressWnd::OffsetPos(int nPos)
{ 
    if (!::IsWindow(GetSafeHwnd())) 
        return m_nPrevPos;

    Show();

    return SetPos(m_nPrevPos + nPos);  
}

int CProgressWnd::StepIt()                
{
    if (!::IsWindow(GetSafeHwnd())) 
        return m_nPrevPos;

    Show();

    return SetPos(m_nPrevPos + m_nStep); 
}

int CProgressWnd::SetStep(int nStep)
{
    int nOldStep = m_nStep;
    m_nStep = nStep;
    if (!::IsWindow(GetSafeHwnd())) 
        return nOldStep;

    return m_wndProgress.SetStep(nStep); 
}

int CProgressWnd::SetPos(int nPos)                    
{
#ifdef PBM_SETRANGE32
    ASSERT(-0x7FFFFFFF <= nPos && nPos <= 0x7FFFFFFF);
#else
    ASSERT(0 <= nPos && nPos <= 65535);
#endif

    if (!::IsWindow(GetSafeHwnd())) 
        return m_nPrevPos;

    Show();

    CString strTitle;
    int nPercentage;
    
    m_nPrevPos = nPos;

    if (m_nMaxValue > m_nMinValue)
        nPercentage = (nPos*100)/(m_nMaxValue - m_nMinValue);
    else
        nPercentage = 0;

    if (nPercentage != m_nPrevPercent) 
    {
        m_nPrevPercent = nPercentage;
        strTitle.Format(_T("%s [%d%%]"),m_strTitle,nPercentage);
        SetWindowText(strTitle);
    }
    return m_wndProgress.SetPos(nPos);        
}

void CProgressWnd::SetText(LPCTSTR fmt, ...)
{
    if (!::IsWindow(GetSafeHwnd())) 
        return;

    va_list args;
    TCHAR buffer[512];

    va_start(args, fmt);
    _vstprintf(buffer, fmt, args);
    va_end(args);

    m_Text.SetWindowText(buffer);
}

BEGIN_MESSAGE_MAP(CProgressWnd, CWnd)
    //{{AFX_MSG_MAP(CProgressWnd)
    ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_CANCEL, OnCancel)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CProgressWnd message handlers

BOOL CProgressWnd::OnEraseBkgnd(CDC* pDC) 
{
    // Fill background with Catchment background colour
    CBrush backBrush(GetSysColor(COLOR_BTNFACE));
    CBrush* pOldBrush = pDC->SelectObject(&backBrush);
    CRect rect;
    pDC->GetClipBox(&rect);     // Erase the area needed
    pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
    pDC->SelectObject(pOldBrush);
	backBrush.DeleteObject();

    return TRUE;
}

void CProgressWnd::OnCancel()
{
    m_bCancelled = TRUE;
    CWnd *pWnd = AfxGetMainWnd();
    if (pWnd && ::IsWindow(pWnd->m_hWnd))
        pWnd->SetForegroundWindow();

    Hide();

    if (m_bModal)
		SendMessage(WM_CLOSE);
}

BOOL CProgressWnd::DestroyWindow() 
{
    if (m_bModal)
    {
        m_bModal = FALSE;
        CWnd *pMainWnd = AfxGetMainWnd();

        if (pMainWnd)
		{
	        pMainWnd->EnableWindow(TRUE);
	        pMainWnd->SetForegroundWindow();//将主窗口提到前台
		}
    }
	return CWnd::DestroyWindow();
}

void CProgressWnd::PeekAndPump(BOOL bCancelOnESCkey /*= TRUE*/)
{
    if (m_bModal && ::GetFocus() != m_hWnd)
        SetFocus();

    MSG msg;
    while (!m_bCancelled && ::PeekMessage(&msg, NULL,0,0,PM_NOREMOVE))
    {
        if (bCancelOnESCkey && (msg.message == WM_CHAR) && (msg.wParam == VK_ESCAPE))
            OnCancel();

        if (m_bModal && (msg.message == WM_LBUTTONUP))
        {
            CRect rect;
            m_CancelButton.GetWindowRect(rect);
            if (rect.PtInRect(msg.pt))
                OnCancel();
        }
  
        if (!AfxGetApp()->PumpMessage())
        {
            ::PostQuitMessage(0);
            return;
        } 
    }
}