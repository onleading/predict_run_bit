// HisDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "HisDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHisDlg dialog


CHisDlg::CHisDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHisDlg::IDD, pParent)
{
	m_Rect.SetRect(0,0,0,0);
	m_bSplitting = FALSE;

	memset(&m_Units,0,sizeof(UNITS));
	
	m_nCurSwitch = g_nWhichHisSwitch;
	m_pView = NULL;

	m_hIcon = AfxGetApp()->LoadIcon(IDI_CALENDAR);
	//远程使用
	m_nMainID = 0;
	m_pSwitchBar = NULL;

}

CHisDlg::~CHisDlg()
{
	g_nWhichHisSwitch = m_nCurSwitch; //保存运行期的Switch版面
	TDEL(m_pSwitchBar);
}

void CHisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHisDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHisDlg, CDialog)
	ON_MESSAGE(UM_GETDATA_ACK,OnGetDataAck)
	ON_MESSAGE(UM_GETMAINID,GetMainID)
	ON_COMMAND(ID_QJSTAT, OnQjStat)
	ON_COMMAND(ID_ALLQJSTAT,OnAllQjStat)
	ON_COMMAND(ID_WRITETOTXTFILE,OnWritetotxtfile)
	ON_COMMAND(ID_OVERLAPZS,OnOverlapZS)
	ON_COMMAND_RANGE(ID_HISCORNER_1,ID_HISCORNER_4,OnCornerClick)
	ON_UPDATE_COMMAND_UI_RANGE(ID_HISCORNER_1,ID_HISCORNER_4,OnUpdateCornerClick)
	
	ON_COMMAND_RANGE(ID_MANYDAYZST0,ID_MANYDAYZST9,OnManyDayZst)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MANYDAYZST0,ID_MANYDAYZST9,OnUpdateManyDayZst)

	//{{AFX_MSG_MAP(CHisDlg)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHisDlg message handlers

char *HisSwitchUnitStr[3] = {"分笔","分钟","信息"};
UINT  HisAssisUnit[3] = {TICK_UNIT,FSB_UNIT,HISK_UNIT};

#define FOR_UNIT(i) for( i=0;i<m_Units.Num && m_Units.pUnit[i];i++)

int CHisDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_pSwitchBar = new CNewZfjsTab(m_hWnd);
	m_pSwitchBar->SetStyle(SwitchStyle_samebutton);
	if(!m_pSwitchBar || !m_pSwitchBar->Create(_T("NewZfjsTab"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,ID_COMSWITCHBAR1))
		return -1;
	m_pSwitchBar->SetFontColorSchema(VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.AXISCOLOR,VipColor.TXTCOLOR,VipColor.KNCOLOR,VipColor.LEVELCOLOR,RGB(128,0,0),12,SONG_FONT);
	for(int i=0;i<3;i++)
		m_pSwitchBar->InsertSwitchLabel(i,_F(HisSwitchUnitStr[i]),WM_COMMAND,ID_HISCORNER_1+i);

	return 0;
}

BOOL CHisDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon,TRUE);
	SetIcon(m_hIcon,TRUE);

	m_pStkInfo = (*g_pStockIO)[m_nCurGP];
	//
	CreateUnit(0,m_nCurGP,ZST_UNIT,ZST_BIG);
	int tmpStyle = 0;
	switch(m_nCurSwitch)
	{
	case 0:tmpStyle = TICK_SMALL;break;
	case 1:tmpStyle = FSB_SMALL;break;
	case 2:tmpStyle = MMP_SMALL;break;
	case 3:tmpStyle = 0;break;
	}
	CreateUnit(1,m_nCurGP,HisAssisUnit[m_nCurSwitch],tmpStyle);
	m_Units.Num = 2;
	ResizePad(COM_OnlyRead);	//第一次要读数据

	return TRUE;
}

//创建一个Unit
void CHisDlg::CreateUnit(int which,int initGPNo,int type,int style)
{
	TDEL(m_Units.pUnit[which]);	//先删除原来的Unit

	m_Units.Type[which] = type;
	m_Units.Style[which] = style;
	m_Units.nStatus[which] = 0;	//为0表示显示出来
	COMINFO ComInfo;
	ComInfo.pWnd = this;
	ComInfo.nNowPad = ZST_PAD; //????,随便
	ComInfo.pUnits = &m_Units;
	ComInfo.nWhichUnit = which;
	ComInfo.nUnitType = type;
	ComInfo.nUnitStyle = style;
	ComInfo.nInCom2 = 0;
	ComInfo.nInDlg = 1;
	ComInfo.nInMultiPeriod = 0;
	switch(type)
	{
	case ZST_UNIT:
		m_Units.pUnit[which] = new UZst(&ComInfo);
		break;
	case TICK_UNIT:
		m_Units.pUnit[which] = new UTick(&ComInfo);
		break;
	case FJB_UNIT:
		m_Units.pUnit[which] = new UFjb(&ComInfo);
		break;
	case FSB_UNIT:
		m_Units.pUnit[which] = new UFsb(&ComInfo);
		break;
	case MMP_UNIT:
		m_Units.pUnit[which] = new UMmp(&ComInfo);
		break;
	case HISK_UNIT:
		m_Units.pUnit[which] = new UHisk(&ComInfo);
		break;
	default:
		break;
	}
	ChangeUnitGP(which,initGPNo);
	ChangeUnitDate(which,m_nCurDate);
}

BOOL CHisDlg::ChangeUnitGP(int which,int iNo)
{
	MemStkInfo *tmpInfo = (*g_pStockIO)[iNo];
	if(!tmpInfo) return FALSE;
	m_Units.pUnit[which]->ProcessMsg(UM_SET_STOCK,iNo);
	return TRUE;
}

BOOL  CHisDlg::ChangeUnitDate(int which,long date)
{
	m_Units.pUnit[which]->ProcessMsg(UM_SET_DATE,date);
	CString tmpStr,tmpMark=g_strPreName;
	tmpStr.Format((CString)"%s"+_F("(%s) %04d年%02d月%02d日 %s PageUp/Down:前后日 %s"),m_pStkInfo->Name,m_pStkInfo->Code,m_nCurDate/10000,(m_nCurDate%10000)/100,m_nCurDate%100,_F(WeekInfo[GetWeek(m_nCurDate)]),tmpMark);
	SetWindowText(tmpStr);
	return TRUE;
}

void CHisDlg::OnPaint() 
{
	CPaintDC dc(this);
	int i;
	FOR_UNIT(i)
		m_Units.pUnit[i]->ProcessMsg(WM_PAINT);
}

void CHisDlg::OnOK() 
{	
}

void CHisDlg::OnCancel() 
{
	int i;
	FOR_UNIT(i)
		TDEL(m_Units.pUnit[i]);
	CDialog::OnCancel();
}

void CHisDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	m_Rect.SetRect(0,0,cx,cy-8);
	ResizePad(COM_OnlySize);
	if(m_pSwitchBar)
		m_pSwitchBar->SetWindowPos(NULL,m_Rect.Width()-g_nFixedWidth,m_Rect.Height()-SWITCH_HEIGHT2,g_nFixedWidth,SWITCH_HEIGHT2,SWP_SHOWWINDOW);
}

//改变版面尺寸 flag=0,局部打位图,=1,读数据并局部打位图,=2,仅读数据,=3,不画
void CHisDlg::ResizePad(int flag)
{
	long wParam,lParam;
	//MainPad
	wParam = MAKELONG(0,0);
	lParam = MAKELONG(m_Rect.Width()-g_nFixedWidth,m_Rect.Height());
	if(m_Units.pUnit[0])
	{
		m_Units.Rect[0] = CRect(0,0,m_Rect.Width()-g_nFixedWidth,m_Rect.Height());
		m_Units.pUnit[0]->ProcessMsg(WM_SIZE,wParam,lParam);	
		if(flag==COM_ReadDraw || flag==COM_OnlyDraw)
 			m_Units.pUnit[0]->ProcessMsg(WM_PAINT,flag);
		if(flag==COM_OnlyRead)
			m_Units.pUnit[0]->ProcessMsg(UM_READDATA);
	}
	//AssisPad
 	wParam = MAKELONG(m_Rect.Width()-g_nFixedWidth,0);
 	lParam = MAKELONG(g_nFixedWidth,m_Rect.Height()-SWITCH_HEIGHT2);
	if(m_Units.pUnit[1])
	{
		m_Units.Rect[1] = CRect(m_Rect.Width()-g_nFixedWidth,0,m_Rect.Width(),m_Rect.Height()-SWITCH_HEIGHT2);
		m_Units.pUnit[1]->ProcessMsg(WM_SIZE,wParam,lParam);
		if(flag==COM_ReadDraw || flag==COM_OnlyDraw)
			m_Units.pUnit[1]->ProcessMsg(WM_PAINT,flag);
		if(flag==COM_OnlyRead)
			m_Units.pUnit[1]->ProcessMsg(UM_READDATA);
	}
	if(m_pSwitchBar) m_pSwitchBar->SetCurLabel(m_nCurSwitch);
}

void CHisDlg::SwitchPad(int whichSwitch)
{
	if(m_nCurSwitch == whichSwitch) return;
	m_nCurSwitch = whichSwitch;
	
	SetMainID();

	int style = 0;
	switch(HisAssisUnit[m_nCurSwitch])
	{
	case TICK_UNIT:	style = TICK_SMALL; break;
	case FJB_UNIT:	style = FJB_SMALL; break;
	case FSB_UNIT:	style = FSB_SMALL; break;
	case MMP_UNIT:	style = MMP_SMALL; break;
	}
	CreateUnit(1,m_nCurGP,HisAssisUnit[m_nCurSwitch],style);
	m_Units.pUnit[1]->ProcessMsg(UM_READDATA);
	ResizePad();

	SetCursor(LoadCursor(NULL,IDC_ARROW));
}

LRESULT CHisDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch(message)
	{
	case UM_REFLECT:	//消息反射,对于此版面的消息反映,因为只有两个Unit,故有所不同.
		switch(wParam)
		{
		case Reflect_ToZstime:
			if(m_Units.pUnit[1])
				m_Units.pUnit[1]->ProcessMsg(UM_TOZSTIME,lParam);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}	
	return CDialog::WindowProc(message, wParam, lParam);
}

int CHisDlg::EscHandle()
{
	return m_Units.pUnit[0]->ProcessMsg(WM_KEYDOWN,VK_ESCAPE);
}

BOOL CHisDlg::PreTranslateMessage(MSG* pMsg) 
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
	case WM_KEYUP:
		switch(pMsg->wParam)
		{
		case VK_ADD:
			{
				int tmp = (m_nCurSwitch+1)%4;
				SHORT ShiftDown=::GetKeyState(VK_SHIFT);
				if(ShiftDown < 0)
				{
					tmp=m_nCurSwitch;
					tmp--;
					if(tmp<0) tmp = 3;
				}
				SwitchPad(tmp);
			}
			break;
		case VK_NEXT:
		case VK_PRIOR:
			if(m_pView)
			{
				long lDate = m_pView->SendMessage(UM_GET_FXTDATE,(pMsg->wParam==VK_NEXT)?1:0);
				if(lDate >= 0 && lDate != m_nCurDate)
				{
					int i;
					SetMainID();
					m_nCurDate = lDate;
					FOR_UNIT(i)
					{
						ChangeUnitDate(i,lDate);
						m_Units.pUnit[i]->ProcessMsg(WM_PAINT,1);
					}
				}
			}
			break;
		}
		break;
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
		case VK_ESCAPE:
			if(EscHandle() == 1) 
				return TRUE;
			break;
		case VK_DELETE:
			PostMessage(WM_COMMAND,IDCANCEL);
			return TRUE;
		case 'W'://截获Ctrl+W快捷键
		case 'w':
			PostMessage(WM_COMMAND,ID_ALLQJSTAT);
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
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CHisDlg::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

//为-1,没有点到任何Unit
int	CHisDlg::HitTest(CPoint point)
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

void CHisDlg::OnRButtonUp(UINT nFlags, CPoint point) 
{
	int i = HitTest(point);
	if(i != -1) m_Units.pUnit[i]->ProcessMsg(WM_RBUTTONUP,nFlags,MAKELONG(point.x,point.y));
}

void CHisDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	//停止分割
	if(m_bSplitting)
	{
		m_bSplitting = FALSE;
		ReleaseCapture();
		SetCursor(LoadCursor(NULL,IDC_ARROW));
		g_nFixedWidth = max(178,m_Rect.right-point.x);
		g_nFixedWidth = min(300,g_nFixedWidth);
		ResizePad();
		SetCursor(m_hVerSplit);
		return;
	}
	CDialog::OnLButtonUp(nFlags, point);
}

void CHisDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//开始分割
	if(!m_bSplitting && point.x > m_Rect.right-g_nFixedWidth-5 && point.x < m_Rect.right-g_nFixedWidth+5)
	{
		m_bSplitting = TRUE;
		SetCapture();
		m_nOldSplitX = -10; //置为无效
		return;
	}
	int i = HitTest(point);
	if(i != -1) m_Units.pUnit[i]->ProcessMsg(WM_LBUTTONDOWN,nFlags,MAKELONG(point.x,point.y));
}

void CHisDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	//显示分割窗口光标
	if(point.x > m_Rect.right-g_nFixedWidth-5 && point.x < m_Rect.right-g_nFixedWidth+5)
	{
		SetCursor(m_hVerSplit);
		return;
	}
	//分割窗口过程中
	if(m_bSplitting)
	{
		CDC *pDC = GetDC();		
		int oldrop = pDC->SetROP2(R2_XORPEN);
		g_d.DrawLine(pDC,m_nOldSplitX,m_Rect.top,m_nOldSplitX,m_Rect.bottom,VipColor.LEVELCOLOR^VipColor.BACKCOLOR);
		g_d.DrawLine(pDC,point.x,m_Rect.top,point.x,m_Rect.bottom,VipColor.LEVELCOLOR^VipColor.BACKCOLOR);
		m_nOldSplitX = point.x;
		pDC->SetROP2(oldrop);
		ReleaseDC(pDC);
		return;
	}
	int iHit = HitTest(point);
	if(iHit != -1) m_Units.pUnit[iHit]->ProcessMsg(WM_MOUSEMOVE,nFlags,MAKELONG(point.x,point.y));
}

void CHisDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int i = HitTest(point);
	if(i != -1) m_Units.pUnit[i]->ProcessMsg(WM_LBUTTONDBLCLK,nFlags,MAKELONG(point.x,point.y));
}

void CHisDlg::OnCornerClick(UINT nID)
{
	SwitchPad(nID-ID_HISCORNER_1);
}

void CHisDlg::OnUpdateCornerClick(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID-ID_HISCORNER_1==m_nCurSwitch);
}

void CHisDlg::OnManyDayZst(UINT nID)
{
	if(m_Units.Type[0]==ZST_UNIT)
	{
		//在数据没有到来之前,不要再进行多日分时,否则容易出现问题
		if(!OnLine || ((UZst *)(m_Units.pUnit[0]))->GetDataReady()) 
		{
			SetMainID();
			if(nID==ID_MANYDAYZST0)
			{
				g_bHisManyDayZST = FALSE;
				g_nHisZSTManyDay = 1;
			}
			else
			{
				g_bHisManyDayZST = TRUE;
				g_nHisZSTManyDay = nID-ID_MANYDAYZST0;
			}
			m_Units.pUnit[0]->ProcessMsg(UM_MANYDAYZST);
		}
	}
}

void CHisDlg::OnUpdateManyDayZst(CCmdUI *pCmdUI)
{
	if(pCmdUI->m_nID == ID_MANYDAYZST0)
		pCmdUI->SetCheck(!g_bHisManyDayZST);
	else
		pCmdUI->SetCheck(g_bHisManyDayZST && g_nHisZSTManyDay==pCmdUI->m_nID-ID_MANYDAYZST0);
}

void CHisDlg::SetMainID()
{
	if(OnLine)
	{
		g_pComm->SetSendTop(0);
		MainIDPlus(m_nMainID,HISFX_WIN);
	}
}

void CHisDlg::GetMainID(WPARAM wParam,LPARAM lParam)	//采用消息的方式，直接填写g_nMainID
{
	if(m_nMainID == 0)
		SetMainID();
	g_nMainID = m_nMainID;
}

void CHisDlg::OnGetDataAck(WPARAM wParam,LPARAM lParam)
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


void CHisDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	int i = HitTest(point);
	if(i != -1) m_Units.pUnit[i]->ProcessMsg(WM_RBUTTONDOWN,nFlags,MAKELONG(point.x,point.y));
}

short GetManyDayBackDateNum(struct AnalyData * AnalyDatap,short nCacheNum,long date)
{
	short nRet = 0;
	if(AnalyDatap && nCacheNum>0)
	{
		for(int i=nCacheNum-1;i>=0;i--,nRet++)
		{
			if(AnalyDatap[i].Time.Date==date)
			{
				if(i>0)	date=AnalyDatap[i-1].Time.Date;
				break;
			}
		}
	}
	return nRet;
}

void CHisDlg::OnQjStat()
{
	if(m_Units.Type[0]==ZST_UNIT) 
	{
		m_Units.pUnit[0]->ProcessMsg(UM_POPZSTICKDLG);
	}
}

void CHisDlg::OnAllQjStat()
{
	if(m_Units.Type[0]==ZST_UNIT) 
	{
		m_Units.pUnit[0]->ProcessMsg(UM_POPZSTICKDLG,1);
	}
}

void CHisDlg::OnOverlapZS()
{
	if(m_Units.Type[0]!=ZST_UNIT || !m_Units.pUnit[0] || !m_pStkInfo)
		return;
	if(m_pStkInfo->setcode!=SZ && m_pStkInfo->setcode!=SH)
	{
		TDX_MessageBox(m_hWnd,"扩展行情不支持同步叠加大盘!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	if(g_Overlap[1].bOverlapHisIndex)
	{
		g_Overlap[1].bOverlapHisIndex=FALSE;
		m_Units.pUnit[0]->ProcessMsg(UM_UNOVERLAP);
	}
	else
	{
		g_Overlap[1].bOverlapHisIndex=TRUE;
		m_Units.pUnit[0]->ProcessMsg(UM_AUTOVERLAP);
	}
}

#include "ExportDlg.h"
void CHisDlg::OnWritetotxtfile()
{
	if(!m_pStkInfo) return;
	CExportDlg dlg;
	dlg.m_strInitFileName.Format("%d-%s",m_nCurDate,m_pStkInfo->Code);
	if(dlg.DoModal() == IDOK)
	{
		if(dlg.m_nTabSel == 0 || dlg.m_nTabSel == 1)
		{
			if(m_Units.Type[1]==TICK_UNIT)
				((UTick*)m_Units.pUnit[1])->WriteToTxtFile(dlg.m_strFileName,dlg.m_nTabSel,9*60);
			else
			{
				TDX_MessageBox(m_hWnd,"仅分时成交明细能导出文本数据!",MB_OK|MB_ICONEXCLAMATION);
				return;
			}
		}
		else
		{
			CRect rect;
			GetClientRect(&rect);
			SendMessage(WM_PAINT);
			HDC hdcScreen = ::GetDC(m_hWnd);
			HDC hdc = CreateCompatibleDC(hdcScreen);
			HBITMAP hbmp = CreateCompatibleBitmap(hdcScreen,rect.Width(),rect.Height());
			SelectObject(hdc,hbmp);
			BitBlt(hdc,0,0,rect.Width(),rect.Height(),hdcScreen,rect.left,rect.top,SRCCOPY);
			CDC *pDC = CDC::FromHandle(hdc);
			CPen *oldpen = g_d.SelectPen(pDC,AXIS_PEN);
			g_d.DrawRect(pDC,&rect);
			g_d.RestorePen(pDC,oldpen);
			SaveToBMPFile(hbmp,dlg.m_strFileName);
			DeleteDC(hdc);
			::ReleaseDC(m_hWnd,hdcScreen);
		}
		if(TDX_MessageBox (m_hWnd,"导出成功,是否打开此文件?",MB_ICONQUESTION|MB_OKCANCEL)==IDOK)
		{
			CString tmpStr;
			tmpStr.Format("\"%s\"",dlg.m_strFileName);
			::ShellExecute(NULL,NULL,tmpStr,NULL,NULL,SW_SHOW);
		}		
	}
}
