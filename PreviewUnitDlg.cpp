// PreviewUnitDlg.cpp : implementation file

#include "stdafx.h"
#include "tdxw.h"
#include "PreviewUnitDlg.h"
#include "MainFrm.h"
#include "UZhSort.h"
#include "UWeb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CPreviewUnitDlg dialog

CPreviewUnitDlg::CPreviewUnitDlg()
{
	m_pPreviewTool = NULL;
	m_pOwnerWnd = NULL;
	m_Rect.SetRect(0,0,0,0);
	memset(&m_Units,0,sizeof(UNITS));
	//远程使用
	m_nMainID = 0;

	Create(AfxGetMainWnd());
}

CPreviewUnitDlg::~CPreviewUnitDlg()
{
	int i= 0;
	FOR_UNIT(i)
		TDEL(m_Units.pUnit[i]);
}


BEGIN_MESSAGE_MAP(CPreviewUnitDlg, CWnd)
	ON_MESSAGE(UM_GETDATA_ACK,OnGetDataAck)
	ON_MESSAGE(UM_GETMAINID,GetMainID)
	//{{AFX_MSG_MAP(CPreviewUnitDlg)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND_RANGE(ID_SCSTART, ID_SCSTART+200, OnChangeZhSortSC)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CPreviewUnitDlg::Create(CWnd* pParentWnd /*= NULL*/)
{

	BOOL bCreateSuc = CreateEx(0,
		AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(IDC_CANPRESS))),
		NULL, WS_POPUP | WS_VISIBLE | WS_DLGFRAME, 0, 0, 0, 0, pParentWnd->GetSafeHwnd(), NULL);
	
	if(bCreateSuc)
		return TRUE;
	return FALSE;
}

BOOL CPreviewUnitDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
//	AnimateWindow(GetSafeHwnd(), 1000, AW_BLEND);
	SetTimer(1, 1000, NULL);

	return TRUE;
}

//创建一个Unit
void CPreviewUnitDlg::CreateUnit(int which,int initGPNo,int type,int style)
{
	if(m_Units.Type[which]!=type||m_Units.Style[which]!=style)
	{
		TDEL(m_Units.pUnit[which]);	//先删除原来的Unit

		m_Units.Type[which] = type;
		m_Units.Style[which] = style;
		m_Units.nStatus[which] = 0;	//为0表示显示出来
		COMINFO ComInfo;
		ComInfo.pWnd = this;
		ComInfo.nNowPad = 0;		//随便
		ComInfo.pUnits = &m_Units;
		ComInfo.nWhichUnit = which;
		ComInfo.nUnitType = type;
		ComInfo.nUnitStyle = style;
		ComInfo.nInCom2 = 0;
		ComInfo.nInMultiPeriod = 0;
		switch(type)
		{
		case ZST_UNIT:	//走势图组件
			m_Units.pUnit[which] = new UZst(&ComInfo);
			m_Units.Num++;
			break;
		case FXT_UNIT:	//分析图组件
			m_Units.pUnit[which] = new UFxt(&ComInfo);
			m_Units.Num++;
			break;
		case ZHSORT_UNIT:
			m_Units.pUnit[which] = new UZhSort(&ComInfo);
			m_Units.Num++;
			break;
		case WEB_UNIT:	//通用浏览组件
			m_Units.pUnit[which] = new UWeb(&ComInfo);
			m_Units.Num++;
			break;
		default:
			break;
		}
	}
	if(m_Units.pUnit[which])
		ChangeUnitGP(m_Units.pUnit[which],initGPNo);
	ResizePad(COM_OnlyRead);	//第一次要读数据
}

BOOL CPreviewUnitDlg::ChangeUnitGP(UBase *pUnit,int iNo)
{
	if(!pUnit) return FALSE;
	int iUnit = -1;
	for(int i=0;i<m_Units.Num;i++)
	{
		if(m_Units.pUnit[i]==pUnit)
		{
			iUnit = i;
			pUnit->ProcessMsg(UM_SET_STOCK,iNo);
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
void CPreviewUnitDlg::OnPaint() 
{
	CPaintDC dc(this);
	int i;
	FOR_UNIT(i)
		m_Units.pUnit[i]->ProcessMsg(WM_PAINT);
}


void CPreviewUnitDlg::OnClose() 
{
//	AnimateWindow(GetSafeHwnd(), 1000, AW_BLEND | AW_HIDE);
	CWnd::OnClose();
}

//不擦除背景
BOOL CPreviewUnitDlg::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

//为-1,没有点到任何Unit
int	CPreviewUnitDlg::HitTest(CPoint point)
{
	int i,nRet = -1;
	FOR_UNIT(i)
	{
		if((m_Units.Rect[i]).PtInRect(point))
		{
			nRet = i;
			break;
		}
	}
	return nRet;
}

void CPreviewUnitDlg::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	m_Rect.SetRect(0,0,cx,cy);
	ResizePad(COM_OnlySize);
}

//改变版面尺寸 flag=0,局部打位图,=1,读数据并局部打位图,=2,仅读数据,=3,不画
void CPreviewUnitDlg::ResizePad(int flag)
{
	long wParam,lParam;
	//MainPad
	wParam = MAKELONG(0,0);
	lParam = MAKELONG(m_Rect.Width(),m_Rect.Height());
	if(m_Units.pUnit[0])
	{
		m_Units.Rect[0] = CRect(0,0,m_Rect.Width(),m_Rect.Height());
		m_Units.pUnit[0]->ProcessMsg(WM_SIZE,wParam,lParam);	
		if(flag==COM_ReadDraw || flag==COM_OnlyDraw)
 			m_Units.pUnit[0]->ProcessMsg(WM_PAINT,flag);
		if(flag==COM_OnlyRead)
			m_Units.pUnit[0]->ProcessMsg(WM_PAINT,1);
	}
}

//////////////////////////////////////////////////////////////////////////
BOOL CPreviewUnitDlg::PreTranslateMessage(MSG* pMsg) 
{
	switch(pMsg->message)
	{
	case WM_KEYDOWN:	//因为VK_DOWN消息传不到OnKeyDown事件里，故用此方法
		switch(pMsg->wParam)
		{	
		case VK_UP:
		case VK_DOWN:
		case VK_LEFT:
		case VK_RIGHT:
			{
				int i;
				FOR_UNIT(i)
					m_Units.pUnit[i]->ProcessMsg(WM_KEYDOWN,pMsg->wParam,1);
			}
			break;
		case VK_RETURN:
			OnGpVip(m_Units.pUnit[0]->ProcessMsg(UM_GET_STOCK));
			return TRUE;
		default:
			{
				int i;
				FOR_UNIT(i)
					m_Units.pUnit[i]->ProcessMsg(WM_KEYDOWN,pMsg->wParam,1);
			}
			break;
		}
		break;
	}

	//将除ESC外的所有键盘输入都发给激活的视图
	if(pMsg->message==WM_KEYDOWN && theViewManager.pActiveView)
	{
		if(pMsg->wParam==96) //对于小键盘上的0作特殊处理
			pMsg->wParam=48;
		if((pMsg->wParam>=0x30&&pMsg->wParam<=0x39)||(pMsg->wParam>='a'&&pMsg->wParam<='z')
			||(pMsg->wParam>='A'&&pMsg->wParam<='Z')||pMsg->wParam== '.'||pMsg->wParam== '?')
		{
			pMsg->hwnd = theViewManager.pActiveView->m_hWnd;
			AfxGetMainWnd()->SetFocus();
		}
	}
	return CWnd::PreTranslateMessage(pMsg);
}

void CPreviewUnitDlg::OnRButtonUp(UINT nFlags, CPoint point) 
{
	int i = HitTest(point);
	if(i != -1) 
		m_Units.pUnit[0]->ProcessMsg(WM_RBUTTONUP,nFlags,MAKELONG(point.x,point.y));
}

void CPreviewUnitDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_Units.Type[0]==ZST_UNIT||m_Units.Type[0]==FXT_UNIT)
	{
		OnGpVip(m_Units.pUnit[0]->ProcessMsg(UM_GET_STOCK), m_Units.Type[0]==ZST_UNIT);
		PostMessage(WM_CLOSE);
		return;
	}
	int i = HitTest(point);
	if(i != -1) 
		m_Units.pUnit[0]->ProcessMsg(WM_LBUTTONDOWN,nFlags,MAKELONG(point.x,point.y));
}

void CPreviewUnitDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	::SetCursor(m_hCanPress);
	int i = HitTest(point);
	if(i != -1) 
		m_Units.pUnit[0]->ProcessMsg(WM_MOUSEMOVE,nFlags,MAKELONG(point.x,point.y));
}

void CPreviewUnitDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	int i = HitTest(point);
	if(i != -1) 
		m_Units.pUnit[0]->ProcessMsg(WM_RBUTTONDOWN,nFlags,MAKELONG(point.x,point.y));
}

//////////////////////////////////////////////////////////////////////////
void CPreviewUnitDlg::OnGpVip(int iIndex, BOOL bZST)
{
	MemStkInfo *ps = (*g_pStockIO)[iIndex];
	if(!ps)	return;
	ToZst(ps->Code,ps->setcode,bZST);
}

void CPreviewUnitDlg::SetMainID()
{
	if(OnLine)
	{
		g_pComm->SetSendTop(0);
		MainIDPlus(m_nMainID,PREVIEWUNIT_WIN);
	}
}

void CPreviewUnitDlg::GetMainID(WPARAM wParam,LPARAM lParam)	//采用消息的方式，直接填写g_nMainID
{
	if(m_nMainID == 0)	SetMainID();
	g_nMainID = m_nMainID;
}

void CPreviewUnitDlg::OnGetDataAck(WPARAM wParam,LPARAM lParam)
{
	if (m_nMainID==g_AnsHeader.MainID)
	{
		short whichunit,whichreq;
		GetComAssisID(g_AnsHeader.AssisID,whichunit,whichreq);
		if(whichunit >= 0 && whichunit < m_Units.Num)
			m_Units.pUnit[whichunit]->ProcessMsg(UM_GETDATA_ACK,whichreq);
	}
	if (m_nMainID==g_AnsHeader3.MainID)
	{
		short whichunit,whichreq;
		GetComAssisID(g_AnsHeader3.AssisID,whichunit,whichreq);
		if(whichunit >= 0 && whichunit < m_Units.Num)
			m_Units.pUnit[whichunit]->ProcessMsg(UM_GETDATA_ACK,whichreq);
	}
}

//////////////////////////////////////////////////////////////////////////
void CPreviewUnitDlg::OnTimer(UINT nIDEvent)
{
	if(nIDEvent==1)
	{
		POINT pt;
		GetCursorPos(&pt);
		CRect rectm(0,0,0,0), recto(0,0,0,0);
		GetWindowRect(&rectm);
		recto = m_pPreviewTool->GetInToolRect();
		m_pOwnerWnd->ClientToScreen(recto);
		if((rectm.Width()>0&&PtInRect(&rectm,pt)))
		{	
		}
		else if((recto.Width()>0&&PtInRect(&recto,pt)))
		{	
		}
		else
		{
			ShowWindow(SW_HIDE);
			if(m_pPreviewTool)
				m_pPreviewTool->DelPreviewTool(m_pOwnerWnd);
		}
		return;
	}

	CWnd::OnTimer(nIDEvent);
}
//////////////////////////////////////////////////////////////////////////
void CPreviewUnitDlg::OnChangeZhSortSC(UINT nID)
{
	if(m_Units.Type[0]!=ZHSORT_UNIT) return;
	
	UZhSort * pUzhSort = (UZhSort*)m_Units.pUnit[0];
	if(!pUzhSort) return;
	pUzhSort->WhichType = nID-ID_SCSTART;
	
	pUzhSort->ProcessMsg(WM_CHANGESCTYPE);
}