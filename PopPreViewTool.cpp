#include "stdafx.h"
#include "PopPreViewTool.h"
#include "PreviewUnitDlg.h"
#include "MainFrm.h"

// CPopPreViewTool
 
CPopPreViewTool::CPopPreViewTool()
{
	m_NeedZoom	= FALSE;
	m_Activate	= FALSE;
	m_pWnd = NULL;
	m_InToolRect.SetRect(0,0,0,0);
}
CPopPreViewTool::CPopPreViewTool(CWnd *pParent)
{
	m_NeedZoom	= FALSE;
	m_Activate	= FALSE;
	m_pWnd		= pParent;
}

CPopPreViewTool::~CPopPreViewTool()
{

}
 

BEGIN_MESSAGE_MAP(CPopPreViewTool, CToolTipCtrl)
	//{{AFX_MSG_MAP(CPopPreViewTool)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPopPreViewTool message handlers
int CPopPreViewTool::SetDelayTime(DWORD dwType, int nTime)
{
	int nDuration;
	switch(dwType)
	{
	case TTDT_AUTOPOP:
	case TTDT_INITIAL:
	case TTDT_RESHOW:
		nDuration = SendMessage(TTM_GETDELAYTIME, (DWORD)dwType, 0);
		SendMessage(TTM_SETDELAYTIME, (WPARAM)(DWORD)dwType, (LPARAM)(INT)MAKELONG(nTime, 0));
		return nDuration;
	}
	return -1;
}

int CPopPreViewTool::GetDelayTime(DWORD dwType)
{
	switch(dwType)
	{
	case TTDT_AUTOPOP:
	case TTDT_INITIAL:
	case TTDT_RESHOW:
		return SendMessage(TTM_GETDELAYTIME, (DWORD)dwType, 0);
	}
	return -1;
}

void CPopPreViewTool::GetMargin(LPRECT lpRect)
{
	SendMessage(TTM_GETMARGIN, 0, (LPARAM)(LPRECT)lpRect);
}

int CPopPreViewTool::GetMaxTipWidth()
{
	return SendMessage(TTM_GETMAXTIPWIDTH, 0, 0);
}

RECT CPopPreViewTool::SetMargin(LPRECT lpRect)
{
	RECT TempRect;
	GetMargin(&TempRect);
	SendMessage(TTM_SETMARGIN, 0, (LPARAM)(LPRECT)lpRect);

	return TempRect;
}

int CPopPreViewTool::SetMaxTipWidth(int nWidth)
{
	return SendMessage(TTM_SETMAXTIPWIDTH, 0, (LPARAM)(INT)nWidth);
}

void CPopPreViewTool::TrackActivate(BOOL bActivate, LPTOOLINFO lpti)
{
	SendMessage(TTM_TRACKACTIVATE, (WPARAM)(BOOL)bActivate,
		(LPARAM)(LPTOOLINFO)lpti);
}

void CPopPreViewTool::TrackPosition(LPPOINT lppt)
{
	SendMessage(TTM_TRACKPOSITION, 0, (LPARAM)(DWORD)MAKELONG(lppt->x, lppt->y));
}

void CPopPreViewTool::TrackPosition(int xPos, int yPos)
{
	SendMessage(TTM_TRACKPOSITION, 0, (LPARAM)(DWORD)MAKELONG(xPos, yPos));
}

void CPopPreViewTool::Activate( BOOL bActivate )
{
	m_Activate=bActivate;
	CToolTipCtrl::Activate(bActivate );
}

BOOL CPopPreViewTool::IsActive()
{
	return m_Activate;
}

int CPopPreViewTool::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CToolTipCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	MakeWindowTransparent(GetSafeHwnd(), 0);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
void CPopPreViewTool::DealText(LPCSTR lpszText)
{
	CString str=lpszText;
	str.Replace("\r","");
	int i =str.Find("\n");
	if(i==-1)
	{
		m_title_str="";
		m_text_str=str;
	}
	else
	{
		m_title_str=str.Left(i);
		m_text_str=str.Mid(i+1);
	}
}

void CPopPreViewTool::UpdateTipText(LPCTSTR lpszText, CWnd* pWnd, UINT nIDTool)
{
	CToolTipCtrl::UpdateTipText(lpszText,pWnd,nIDTool);
	DealText(lpszText);
}

void CPopPreViewTool::UpdateTipText(UINT nIDText, CWnd* pWnd, UINT nIDTool)
{
	ASSERT(nIDText != 0);
	
	CString str;
	VERIFY(str.LoadString(nIDText));
	UpdateTipText(str, pWnd, nIDTool);
}

BOOL CPopPreViewTool::AddTool(CWnd* pWnd, LPCTSTR lpszText, LPCRECT lpRectTool,
						   UINT nIDTool)
{
	DealText(lpszText);
	return CToolTipCtrl::AddTool(pWnd,lpszText,lpRectTool,nIDTool);
}

BOOL CPopPreViewTool::AddTool(CWnd* pWnd, UINT nIDText, LPCRECT lpRectTool,
						   UINT nIDTool)
{
	DealText((LPTSTR)MAKEINTRESOURCE(nIDText));
	return CToolTipCtrl::AddTool(pWnd,nIDText,lpRectTool,nIDTool);
}

void CPopPreViewTool::InitPreviewTool(CWnd *pWnd)
{
	Create(pWnd);
	AddTool(pWnd,"");
	m_pWnd = pWnd;
	SetMaxTipWidth(500);
	SetDelayTime(TTDT_INITIAL,500);
	Activate(FALSE);
}

void CPopPreViewTool::UpdatePreviewTool(CWnd *pWnd,CString strTipInfo)
{
	if(!pWnd || !::IsWindow(pWnd->m_hWnd))
		return;
	UpdateTipText(strTipInfo,pWnd);
	if(strTipInfo.GetLength()==0)
		Activate(FALSE);
	else
		Activate(TRUE);

}

void CPopPreViewTool::AddPreviewTool(CWnd *pWnd,CString strTipInfo,LPCRECT pRect,UINT nID)
{
	if(!pWnd || !::IsWindow(pWnd->m_hWnd))
		return;
	if(m_pWnd) 
		m_pWnd->SetFocus();
	AddTool(pWnd,strTipInfo,pRect,nID);
	memcpy(&m_InToolRect, pRect, sizeof(CRect));
	Activate(TRUE);
}

void CPopPreViewTool::DelPreviewTool(CWnd *pWnd)
{
	if(!pWnd || !::IsWindow(pWnd->m_hWnd))
		return;
	DelTool(pWnd);
	Activate(FALSE);
}

LRESULT CPopPreViewTool::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message==WM_PAINT)
	{
		MakeWindowTransparent(GetSafeHwnd(), 0);
		LRESULT r=CToolTipCtrl::WindowProc(message, wParam, lParam);
		ShowPreviewTool();
		return r;
	}
	return CToolTipCtrl::WindowProc(message, wParam, lParam);
}

void CPopPreViewTool::ShowPreviewTool()
{
	//解析参数
	long nGPNo = -1;
	
	int nUnitType = atoi((char*)(LPCSTR)m_title_str);
	int nUnitStyle = 0;
	char *strParams = new char[m_text_str.GetLength()+10];
	strcpy(strParams, (char*)(LPCSTR)m_text_str);
	char *argv[10] = {0};
	GetCmdLine(strParams, argv, 10, "\r\n");
	//第1行style
	if(argv[0]&&argv[0][0])
		nUnitStyle = atoi(argv[0]);
	//第2行股票信息
	if(argv[1]&&argv[1][0])
	{
		int nSetCode = argv[1][0]-'0';
		char *Code = &argv[1][1];
		nGPNo = g_pStockIO->GetIndex(Code, nSetCode);
	}
	CRect rect;
	GetWindowRect(rect);
	
	if(nGPNo<0||rect.Width()<1||rect.Height()<1) return;
	
	if(g_pPreviewUnitDlg&&g_pPreviewUnitDlg->m_hWnd)
		g_pPreviewUnitDlg->SendMessage(WM_CLOSE, 0, 0);
	TDEL(g_pPreviewUnitDlg);
	
	if(!g_pPreviewUnitDlg)
		g_pPreviewUnitDlg = new CPreviewUnitDlg();
	MakeWindowTransparent(g_pPreviewUnitDlg->GetSafeHwnd(), 0.764f*MWT_MAX_FACTOR);
	::SetWindowPos(g_pPreviewUnitDlg->GetSafeHwnd(), HWND_TOPMOST, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
	g_pPreviewUnitDlg->SetToolTarget(m_pWnd, this);
	g_pPreviewUnitDlg->CreateUnit(0, nGPNo, nUnitType, nUnitStyle);
	
	if(g_pPreviewUnitDlg->GetMainUnitType()==ZHSORT_UNIT) 
		g_pPreviewUnitDlg->SendMessage(WM_COMMAND, ID_SCSTART+6);
	
	if(m_pWnd) 
		m_pWnd->SetFocus();
}