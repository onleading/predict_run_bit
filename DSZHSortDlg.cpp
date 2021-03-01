// DSZHSortDlg.cpp : implementation file

#include "stdafx.h"
#include "tdxw.h"
#include "DSZHSortDlg.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDSZHSortDlg dialog

CDSZHSortDlg::CDSZHSortDlg(CWnd* pParent /*=NULL*/)
	: CCaptionButtonDlg(CDSZHSortDlg::IDD, pParent)
{
	m_Rect.SetRect(0,0,0,0);
	memset(&m_Units,0,sizeof(UNITS));

	m_hIcon = AfxGetApp()->LoadIcon(IDI_CALENDAR);
	//远程使用
	m_nMainID = 0;
}

CDSZHSortDlg::~CDSZHSortDlg()
{
	int i= 0;
	FOR_UNIT(i)
		TDEL(m_Units.pUnit[i]);
}

void CDSZHSortDlg::DoDataExchange(CDataExchange* pDX)
{
	CCaptionButtonDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDSZHSortDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDSZHSortDlg, CCaptionButtonDlg)
	ON_MESSAGE(UM_GETDATA_ACK,OnGetDataAck)
	ON_MESSAGE(UM_GETMAINID,GetMainID)
	//{{AFX_MSG_MAP(CDSZHSortDlg)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_COMMAND_RANGE(ID_SCSTART, ID_SCSTART+200, OnChangeZhSortSC)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SCSTART, ID_SCSTART+200, OnUpdateChangeZhSortSC)
	ON_COMMAND(ID_SETUNITOPTION, OnSetunitoption)
	ON_WM_SHOWWINDOW()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

extern void MakeZHSortTitle(char (*TypeStr)[25]);

BOOL CDSZHSortDlg::OnInitDialog() 
{
	CCaptionButtonDlg::OnInitDialog();
	
	CreateUnit(0,-1,ZHSORT_UNIT,0);
	m_Units.Num = 1;
	ResizePad(COM_OnlyRead);	//第一次要读数据

	SetTimer(1,(HQRefreshTime+2)*1000,NULL);

	return TRUE;
}

//创建一个Unit
void CDSZHSortDlg::CreateUnit(int which,int initGPNo,int type,int style)
{
	TDEL(m_Units.pUnit[which]);	//先删除原来的Unit

	m_Units.Type[which] = type;
	m_Units.Style[which] = style;
	m_Units.nStatus[which] = 0;	//为0表示显示出来
	COMINFO ComInfo;
	ComInfo.pWnd = this;
	ComInfo.nNowPad = ZHSORT_UNIT; //????,随便
	ComInfo.pUnits = &m_Units;
	ComInfo.nWhichUnit = which;
	ComInfo.nUnitType = type;
	ComInfo.nUnitStyle = style;
	ComInfo.nInCom2 = 0;
	ComInfo.nInDlg = 1;
	ComInfo.nInMultiPeriod = 0;
	switch(type)
	{
	case ZHSORT_UNIT:
		m_Units.pUnit[which] = new UZhSort(&ComInfo);
	//	m_Units.pUnit[which] = new USortGrid(&ComInfo);
		break;
	default:
		break;
	}
}
//////////////////////////////////////////////////////////////////////////

void CDSZHSortDlg::OnChangeZhSortSC(UINT nID)
{
	if(m_Units.Type[0]!=ZHSORT_UNIT) return;

	UZhSort * pUzhSort = (UZhSort*)m_Units.pUnit[0];
	if(!pUzhSort) return;
//	if(pUzhSort->WhichType != nID - ID_SCSTART)
//	{
		pUzhSort->WhichType = nID - ID_SCSTART;
		pUzhSort->ProcessMsg(WM_CHANGESCTYPE);
//	}
}

void CDSZHSortDlg::OnUpdateChangeZhSortSC(CCmdUI *pCmdUI)
{
	if(m_Units.Type[0]!=ZHSORT_UNIT) return;
	
	UZhSort * pUzhSort = (UZhSort*)m_Units.pUnit[0];
	if(!pUzhSort) return;
	if(pUzhSort->m_nDSFlag==0) 
		pCmdUI->SetRadio(pCmdUI->m_nID==pUzhSort->WhichType+ID_SCSTART);
	else if(pUzhSort->m_nDSFlag==1)
		pCmdUI->SetRadio(pCmdUI->m_nID==pUzhSort->WhichType+ID_SCSTART);
}

/*
void CDSZHSortDlg::OnChangeZhSortSC(UINT nID)
{
	if(m_Units.Type[0]!=ZHSORT_UNIT) return;
	
	USortGrid * pUzhSort = (USortGrid*)m_Units.pUnit[0];
	if(!pUzhSort) return;
	if(pUzhSort->WhichType != nID - ID_SCSTART)
	{
		pUzhSort->WhichType = nID - ID_SCSTART;
		pUzhSort->ProcessMsg(WM_CHANGESCTYPE);
	}
}

void CDSZHSortDlg::OnUpdateChangeZhSortSC(CCmdUI *pCmdUI)
{
	if(m_Units.Type[0]!=ZHSORT_UNIT) return;
	
	USortGrid * pUzhSort = (USortGrid*)m_Units.pUnit[0];
	if(!pUzhSort) return;
	if(pUzhSort->m_nDSFlag==0) 
		pCmdUI->SetRadio(pCmdUI->m_nID==pUzhSort->WhichType+ID_SCSTART);
	else if(pUzhSort->m_nDSFlag==1)
		pCmdUI->SetRadio(pCmdUI->m_nID==pUzhSort->WhichType+ID_SCSTART);
}
*/
//////////////////////////////////////////////////////////////////////////
void CDSZHSortDlg::OnPaint() 
{
	CPaintDC dc(this);
	int i;
	FOR_UNIT(i)
		m_Units.pUnit[i]->ProcessMsg(WM_PAINT);
}

void CDSZHSortDlg::OnOK() //不允许用户按OK
{
}

void CDSZHSortDlg::OnCancel() 
{
	GetParent() ->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);
}

//不擦除背景
BOOL CDSZHSortDlg::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}


void CDSZHSortDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	m_Rect.SetRect(0,0,cx,cy);
	ResizePad(COM_OnlySize);
}

//改变版面尺寸 flag=0,局部打位图,=1,读数据并局部打位图,=2,仅读数据,=3,不画
void CDSZHSortDlg::ResizePad(int flag)
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
BOOL CDSZHSortDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_SYSKEYDOWN && ((pMsg->wParam>=0x31 && pMsg->wParam<=0x39) || pMsg->wParam==0x30) )
	{
		if(m_Units.Type[0]==ZST_UNIT && m_Units.pUnit[0])
		{
			if(pMsg->wParam==0x30)
				PostMessage(WM_COMMAND,ID_MANYDAYZST9);		
			else
				PostMessage(WM_COMMAND,ID_MANYDAYZST0+pMsg->wParam-0x31);	
			return TRUE;
		}
	}
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
		case VK_DELETE:
			PostMessage(WM_COMMAND,IDCANCEL);
			return TRUE;
		case 'W'://截获Ctrl+W快捷键
		case 'w':
			PostMessage(WM_COMMAND,ID_ALLQJSTAT);
			return TRUE;
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
	case WM_CHAR:
		{
			int i;
			FOR_UNIT(i)
				m_Units.pUnit[i]->ProcessMsg(WM_CHAR,pMsg->wParam,1);
		}
		break;
	}

	//将除ESC外的所有键盘输入都发给激活的视图
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==96) //对于小键盘上的0作特殊处理
			pMsg->wParam=48;
		if((pMsg->wParam>=0x30&&pMsg->wParam<=0x39)||(pMsg->wParam>='a'&&pMsg->wParam<='z')
			||(pMsg->wParam>='A'&&pMsg->wParam<='Z')||pMsg->wParam== '.'||pMsg->wParam== '?')
			pMsg->hwnd = theViewManager.pActiveView->m_hWnd;
	}
	return CCaptionButtonDlg::PreTranslateMessage(pMsg);
}

void CDSZHSortDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	SHOW_NOMODAL_CORNER

	CCaptionButtonDlg::OnShowWindow(bShow, nStatus);	
}

void CDSZHSortDlg::OnRButtonUp(UINT nFlags, CPoint point) 
{
	m_Units.pUnit[0]->ProcessMsg(WM_RBUTTONUP,nFlags,MAKELONG(point.x,point.y));
}

void CDSZHSortDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_Units.pUnit[0]->ProcessMsg(WM_LBUTTONDOWN,nFlags,MAKELONG(point.x,point.y));
}

void CDSZHSortDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_Units.pUnit[0]->ProcessMsg(WM_MOUSEMOVE,nFlags,MAKELONG(point.x,point.y));
}

void CDSZHSortDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	m_Units.pUnit[0]->ProcessMsg(WM_LBUTTONDBLCLK,nFlags,MAKELONG(point.x,point.y));
	OnGpVip(m_Units.pUnit[0]->ProcessMsg(UM_GET_STOCK));
}

void CDSZHSortDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_Units.pUnit[0]->ProcessMsg(WM_RBUTTONDOWN,nFlags,MAKELONG(point.x,point.y));
}
//////////////////////////////////////////////////////////////////////////
void CDSZHSortDlg::OnGpVip(int iIndex)
{
	MemStkInfo *ps = (*g_pStockIO)[iIndex];
	if(!ps)	return;
	ToZst(ps->Code,ps->setcode,FALSE);
}

void CDSZHSortDlg::SetMainID()
{
	if(OnLine)
	{
		g_pComm->SetSendTop(0);
		MainIDPlus(m_nMainID,ZHSORT_WIN);
	}
}

void CDSZHSortDlg::GetMainID(WPARAM wParam,LPARAM lParam)	//采用消息的方式，直接填写g_nMainID
{
	if(m_nMainID == 0)	SetMainID();
	g_nMainID = m_nMainID;
}

void CDSZHSortDlg::OnGetDataAck(WPARAM wParam,LPARAM lParam)
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
void CDSZHSortDlg::OnCaptionButton1()
{
	m_bt1.SetCheck(TRUE);
	m_bt2.SetCheck(FALSE);

	if(m_Units.Type[0]!=ZHSORT_UNIT) return;
	UZhSort *pUZhSort = (UZhSort*)m_Units.pUnit[0];
	pUZhSort->SetStyle(STYLE_1X5);
}

void CDSZHSortDlg::OnCaptionButton2()
{
	m_bt1.SetCheck(FALSE);
	m_bt2.SetCheck(TRUE);

	if(m_Units.Type[0]!=ZHSORT_UNIT) return;
	UZhSort *pUZhSort = (UZhSort*)m_Units.pUnit[0];
	pUZhSort->SetStyle(STYLE_3X3);
}

void CDSZHSortDlg::InitCaptionButtons()
{
	m_bt1.Init(IDB_BITMAP_CAPTIONBUTTON,8,RGB(0,255,0),1,this,g_nZHStyle==STYLE_1X5,"1行4列");
	m_bt2.Init(IDB_BITMAP_CAPTIONBUTTON,8,RGB(0,255,0),1,this,g_nZHStyle==STYLE_3X3||g_nZHStyle==STYLE_2X2,"N行N列");
	m_bt1.ShowWindow(TRUE);
	m_bt2.ShowWindow(TRUE);
}

#include "ZhSortOptDlg.h"
void CDSZHSortDlg::OnSetunitoption()
{
	if(m_Units.Type[0]!=ZHSORT_UNIT) return;

	ZhSortOptDlg dlg;
	UZhSort *pUZhSort = (UZhSort*)m_Units.pUnit[0];
	dlg.m_nShowNum = pUZhSort->m_nUnitRow==3?0:1;
	pUZhSort->SetShowSort(&dlg.m_aShowSort);
	if(dlg.DoModal()==IDOK)
	{
		pUZhSort->SetStyle(dlg.m_nShowNum==0?STYLE_3X3:STYLE_2X2);
		pUZhSort->ProcessMsg(WM_PAINT, 1);
	}
}