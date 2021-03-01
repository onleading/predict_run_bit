// DeepWarnDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "DeepWarnDlg.h"
#include "IniFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////////

static char l_LineFlag[2][5] = {"委买","委卖"};
static char l_OrderFlag[4][9]= {"挂买(估)","撤买(估)","挂卖(估)","撤卖(估)"};

static char *l_DeepWarnTypeStr[12]= {"其它类型","拖拉机单(Level2)","委量异动(Level2)","主力大单","加速拉升",
									 "加速下跌","低位反弹","高位回落","撑杆跳高","平台跳水",
									 "单笔冲击","区间放量"};

//客户端可以显示的类型
int l_CanShowWarnAtom[11]={	WARN_LINE,WARN_ORDERSHOCK,WARN_LARGEVOL,WARN_QUICKRISE,WARN_QUICKFALL,
						WARN_REBOUND,WARN_QUICKDIVE,WARN_PRICEUPSHOCK,WARN_PRICEDNSHOCK,WARN_TICKSHOCK,
						WARN_VOLUMESHOCK};

BOOL IsCanShowWarnAtom(short WarnType)
{
	int i;
	for(i=0;i<11;i++)
		if(l_CanShowWarnAtom[i]==WarnType) break;
	if(i<11)
		return TRUE;
	return FALSE;
}

int CopyToMyWarnAtom(LPWARNATOM & pWarnAtom,int nFlag,int &nCurGP)
{
	if(g_DWarnInf.nWarnNum<0) 
		return 0;
	TDEL(pWarnAtom);
	pWarnAtom	= new WARNATOM[g_DWarnInf.nWarnNum+1];
	nCurGP		= GetSYSCurrentGPIndex();
	//
	int			iNo,nCnt = 0;
	MemStkInfo *pInfo = NULL;
	BOOL		bOK = FALSE;
	for(int i=0;i<g_DWarnInf.nWarnNum;i++)
	{
		bOK=FALSE;
		if(IsCanShowWarnAtom(g_DWarnInf.pWarnAtom[i].WarnType)) //可以支持的类型
		{
			switch(nFlag)
			{
			case 0:	
				bOK = TRUE;
				break;
			case 1:
			//	iNo = g_pStockIO->GetIndex(g_DWarnInf.pWarnAtom[i].Code,g_DWarnInf.pWarnAtom[i].SetCode);
			//	if(BelongHS300(iNo))
			//		bOK = TRUE;
				break;
			case 2:
				pInfo = g_pStockIO->Get_MemStkInfo(g_DWarnInf.pWarnAtom[i].Code,g_DWarnInf.pWarnAtom[i].SetCode);
				if(pInfo && BelongZXG(pInfo))
					bOK = TRUE;
				break;
			case 3:
				iNo = g_pStockIO->GetIndex(g_DWarnInf.pWarnAtom[i].Code,g_DWarnInf.pWarnAtom[i].SetCode);
				if(iNo == nCurGP)
					bOK = TRUE;
			}
		}
		if(bOK)
		{
			pWarnAtom[nCnt]=g_DWarnInf.pWarnAtom[i];
			nCnt++;
		}
	}
	return nCnt;
}

int l_YSPACE=15;
int l_HeadSingleWid=28;
void DrawDeepWarnList(BOOL bInDlg,long nWarnNum,LPWARNATOM pWarnAtom,int nFlag,CDC *pDC,CRect rc,int & nCurSel,int & nLastPos,int & nDrawPos,int & nDrawNum,int & nRowNum)
{
	CPen	*oldpen = g_d.SelectPen(pDC,AXIS_PEN);
	g_d.Draw3DBar3(pDC,CRect(1,rc.top,rc.Width(),rc.top+HQ_TEXTH),RGB(64,0,0),FALSE);
	/*
	g_d.DrawLine(pDC,0,rc.top+HQ_TEXTH,rc.Width(),rc.top+HQ_TEXTH);
	g_d.DrawLine(pDC,l_HeadSingleWid,rc.top,l_HeadSingleWid,rc.top+HQ_TEXTH);	
	g_d.DrawLine(pDC,2*l_HeadSingleWid,rc.top,2*l_HeadSingleWid,rc.top+HQ_TEXTH);	
	g_d.DrawLine(pDC,3*l_HeadSingleWid,rc.top,3*l_HeadSingleWid,rc.top+HQ_TEXTH);
	g_d.DrawLine(pDC,4*l_HeadSingleWid,rc.top,4*l_HeadSingleWid,rc.top+HQ_TEXTH);
	if(!bInDlg)
		g_d.DrawLine(pDC,5*l_HeadSingleWid,rc.top,5*l_HeadSingleWid,rc.top+HQ_TEXTH);
		*/
	CFont *oldfont = g_d.SelectFont(pDC,LIST_FONT);
	/*
	g_d.DisplayText(pDC,2,rc.top+1,(nFlag==0)?VipColor.KNCOLOR:VipColor.TXTCOLOR,0,_F("全部"));
	g_d.DisplayText(pDC,l_HeadSingleWid+2,rc.top+1,(nFlag==1)?VipColor.KNCOLOR:VipColor.TXTCOLOR,0,_F("300"));
	g_d.DisplayText(pDC,2*l_HeadSingleWid+2,rc.top+1,(nFlag==2)?VipColor.KNCOLOR:VipColor.TXTCOLOR,0,_F("自选"));
	g_d.DisplayText(pDC,3*l_HeadSingleWid+2,rc.top+1,(nFlag==3)?VipColor.KNCOLOR:VipColor.TXTCOLOR,0,_F("当前"));
	if(!bInDlg)
	{
		g_d.DisplayText(pDC,4*l_HeadSingleWid+2,rc.top+1,VipColor.TXTCOLOR,0,_F("浮出"));
		g_d.DisplayText(pDC,4*l_HeadSingleWid+2,rc.top+1,VipColor.BIGTITLECOLOR,rc.Width()-4*l_HeadSingleWid-2,_F("主力监控"));
	}
	*/
	g_d.DisplayText(pDC,CRect(1,rc.top,rc.Width(),rc.top+HQ_TEXTH),VipColor.TXTCOLOR,DT_CENTER,_F("异动监控"));
	g_d.RestoreFont(pDC,oldfont);

	if(g_GC.bOnlyHK || nWarnNum<1) 
		return;
	oldfont = g_d.SelectFont(pDC,SMALL2_FONT);
	nRowNum = (rc.Height()-3-HQ_TEXTH)/l_YSPACE;
	if(nLastPos<0)
		nLastPos=nWarnNum-1;
	nDrawPos = max(0,nLastPos+1-nRowNum);
	nDrawNum = (nLastPos+1<nRowNum)?nLastPos+1:nRowNum;

	int xFrom1=rc.left+1,xFrom2=rc.left+36,xFrom3=rc.left+90,xFrom4=rc.left+145;
	int yFrom=rc.top+HQ_TEXTH+3;

	int nTmp;
	int XSFlag = XS2;
	MemStkInfo *pStock;
	COLORREF	crColor;
	CString		tmpPreFix;
	for(int i=nDrawPos;i<nDrawPos+nDrawNum;i++)
	{
		if(i>0&&pWarnAtom[i].MinSec/100!=pWarnAtom[i-1].MinSec/100)
			g_d.DrawLine(pDC,xFrom1,yFrom-1,rc.Width(),yFrom-1,GridColor.GridLineColor);
		if(nCurSel==i)
			g_d.Bar(pDC,xFrom1,yFrom,rc.Width(),yFrom+l_YSPACE-1,INVERSE_BRUSH);
		pStock = g_pStockIO->Get_MemStkInfo(pWarnAtom[i].Code,pWarnAtom[i].SetCode);
		if(pStock)
		{
			crColor = VipColor.TXTCOLOR;
			XSFlag = GetXSFlag(pStock);
			switch(pWarnAtom[i].WarnType)
			{				
			case WARN_LINE:			//拖拉机单
				crColor = VipColor.VOLCOLOR;
				g_d.DisplayText(pDC,xFrom3,yFrom,crColor,"%-10s",_F("拖拉机单"));
				nTmp = pWarnAtom[i].uDetail.uTractor.WarnFlag;
				if(nTmp==0 || nTmp==1)
				{
					crColor=VipColor.KPCOLOR;
					if(nTmp==1)
						crColor=VipColor.DOWNCOLOR;
					tmpPreFix="%.2f/(%d-%d)×%d";
					if(XSFlag==XS3)
						tmpPreFix="%.3f/(%d-%d)×%d";
					//
					g_d.DisplayText(pDC,xFrom4,yFrom,crColor,tmpPreFix,
						pWarnAtom[i].uDetail.uTractor.Price,
						pWarnAtom[i].uDetail.uTractor.FirstQty,
						pWarnAtom[i].uDetail.uTractor.FirstQty
						+pWarnAtom[i].uDetail.uTractor.DiffQty*(pWarnAtom[i].uDetail.uTractor.OrderNum-1),
						pWarnAtom[i].uDetail.uTractor.OrderNum);
				}
				break;
			case WARN_ORDERSHOCK:	//委量异动
				nTmp = pWarnAtom[i].uDetail.uOrderShock.WarnFlag;
				if(nTmp>=0 && nTmp<4)
				{
					if(nTmp==1 || nTmp==2)
					{
						crColor = VipColor.DOWNCOLOR;
						g_d.DisplayText(pDC,xFrom3,yFrom,crColor,"%-10s",_F(l_OrderFlag[nTmp]));
					}
					else 
					{
						crColor = VipColor.KPCOLOR;
						g_d.DisplayText(pDC,xFrom3,yFrom,crColor,"%-10s",_F(l_OrderFlag[nTmp]));
					}
					//
					tmpPreFix="%.3f/%.0f";
					//
					g_d.DisplayText(pDC,xFrom4,yFrom,crColor,tmpPreFix,pWarnAtom[i].uDetail.uOrderShock.Amount/pWarnAtom[i].uDetail.uOrderShock.Volume,pWarnAtom[i].uDetail.uOrderShock.Volume/pStock->Unit);
				}
				break;
			case WARN_LARGEVOL:		//主力大单
				nTmp = pWarnAtom[i].uDetail.uLargeVol.WarnFlag;
				if(nTmp==0 || nTmp==1 || nTmp==2)
				{
					if(nTmp==0)
					{
						crColor = VipColor.KPCOLOR;
						g_d.DisplayText(pDC,xFrom3,yFrom,crColor,"%-10s",_F("主力买入"));
					}
					else if(nTmp==1)
					{
						crColor = VipColor.DOWNCOLOR;
						g_d.DisplayText(pDC,xFrom3,yFrom,crColor,"%-10s",_F("主力卖出"));
					}
					else
					{
						crColor = VipColor.LEVELCOLOR;
						g_d.DisplayText(pDC,xFrom3,yFrom,crColor,"%-10s",_F("主力买卖"));
					}
					//
					tmpPreFix="%.2f/%.0f";
					if(XSFlag==XS3)
						tmpPreFix="%.3f/%.0f";
					//
					g_d.DisplayText(pDC,xFrom4,yFrom,crColor,tmpPreFix,pWarnAtom[i].uDetail.uLargeVol.Price,pWarnAtom[i].uDetail.uLargeVol.Volume/pStock->Unit);
				}
				break;
			case WARN_QUICKRISE:	//加速拉升
				crColor = VipColor.KPCOLOR;
				g_d.DisplayText(pDC,xFrom3,yFrom,crColor,"%-10s",_F("加速拉升"));
				g_d.DisplayText(pDC,xFrom4,yFrom,crColor,"%.3f%%",pWarnAtom[i].uDetail.uQuickChg.QuickChgRate*100);
				break;
			case WARN_QUICKFALL:	//加速下跌
				crColor = VipColor.DOWNCOLOR;
				g_d.DisplayText(pDC,xFrom3,yFrom,crColor,"%-10s",_F("加速下跌"));
				g_d.DisplayText(pDC,xFrom4,yFrom,crColor,"%.3f%%",pWarnAtom[i].uDetail.uQuickChg.QuickChgRate*100);
				break;
			case WARN_REBOUND:		//低位反弹
				crColor = VipColor.KPCOLOR;
				g_d.DisplayText(pDC,xFrom3,yFrom,crColor,"%-10s",_F("低位反弹"));
				g_d.DisplayText(pDC,xFrom4,yFrom,crColor,"%.3f%%",pWarnAtom[i].uDetail.uReversal.ReversalRate*100);
				break;
			case WARN_QUICKDIVE:	//高位回落
				crColor = VipColor.DOWNCOLOR;
				g_d.DisplayText(pDC,xFrom3,yFrom,crColor,"%-10s",_F("高位回落"));
				g_d.DisplayText(pDC,xFrom4,yFrom,crColor,"%.3f%%",pWarnAtom[i].uDetail.uReversal.ReversalRate*100);
				break;
			case WARN_PRICEUPSHOCK:	//撑杆跳高
				crColor = VipColor.KPCOLOR;
				g_d.DisplayText(pDC,xFrom3,yFrom,crColor,"%-10s",_F("撑杆跳高"));
				g_d.DisplayText(pDC,xFrom4,yFrom,crColor,"%.3f%%",pWarnAtom[i].uDetail.uPxShock.PxShockRate*100);
				break;
			case WARN_PRICEDNSHOCK:	//平台跳水
				crColor = VipColor.DOWNCOLOR;
				g_d.DisplayText(pDC,xFrom3,yFrom,crColor,"%-10s",_F("平台跳水"));
				g_d.DisplayText(pDC,xFrom4,yFrom,crColor,"%.3f%%",pWarnAtom[i].uDetail.uPxShock.PxShockRate*100);
				break;
			case WARN_TICKSHOCK:	//单笔冲击
				if(pWarnAtom[i].uDetail.uTickShock.TickPxRate>0)
				{
					crColor = VipColor.KPCOLOR;
					g_d.DisplayText(pDC,xFrom3,yFrom,crColor,"%-10s",_F("单笔冲击"));
				}
				else 
				{
					crColor = VipColor.DOWNCOLOR;
					g_d.DisplayText(pDC,xFrom3,yFrom,crColor,"%-10s",_F("单笔冲击"));
				}
				g_d.DisplayText(pDC,xFrom4,yFrom,crColor,"%.3f%%",pWarnAtom[i].uDetail.uTickShock.TickPxRate*100);
				break;
			case WARN_VOLUMESHOCK:	//区间放量
				crColor = VipColor.VOLCOLOR;
				g_d.DisplayText(pDC,xFrom3,yFrom,crColor,"%-10s",_F("区间放量"));
				g_d.DisplayText(pDC,xFrom4,yFrom,crColor,"%.3f倍",pWarnAtom[i].uDetail.uVolShock.VolumeRate);
				break;
			default:
				break;
			}
			crColor=GetBlockGPColor(pStock,crColor,(nFlag==2)?TYPE_ZXG:-1);
			if(strncmp(pStock->Code,"887",3)==0) //如果是板块指数,用特别颜色
				crColor=VipColor.LEVELCOLOR;
			g_d.DisplayText(pDC,xFrom1,yFrom,crColor,"%02d:%02d",pWarnAtom[i].Hour,pWarnAtom[i].MinSec/100);
			g_d.DisplayText(pDC,xFrom2,yFrom,crColor,"%-8s",pStock->Name);

			yFrom+=l_YSPACE;				
		}
	}

	g_d.RestoreFont(pDC,oldfont);
	g_d.RestorePen(pDC,oldpen);
}


int DeepWarnHitTest(long nWarnNum,LPWARNATOM pWarnAtom,CPoint point,CRect rc,int & nLastPos,int & nDrawPos,int & nDrawNum)
{
	/*
	if(point.y<rc.top+HQ_TEXTH)
	{
		if(point.x<rc.left+5*l_HeadSingleWid && point.x>rc.left+4*l_HeadSingleWid)
			return -14;
		if(point.x<rc.left+4*l_HeadSingleWid && point.x>rc.left+3*l_HeadSingleWid)
			return -13;
		if(point.x<rc.left+3*l_HeadSingleWid && point.x>rc.left+2*l_HeadSingleWid)
			return -12;
		else if(point.x<rc.left+2*l_HeadSingleWid && point.x>rc.left+l_HeadSingleWid)
			return -11;
		else if(point.x<rc.left+l_HeadSingleWid && point.x>rc.left)
			return -10;
	}
	*/
	if(nWarnNum<1 || nLastPos<0) 
		return -1;
	int nRet=-1;
	CRect focus_rc;
	int xFrom1=rc.left+2;
	int yFrom=rc.top+HQ_TEXTH+3;
	for(int i=nDrawPos;i<nDrawPos+nDrawNum;i++)
	{
		focus_rc.SetRect(xFrom1,yFrom,rc.Width(),yFrom+l_YSPACE);
		if(focus_rc.PtInRect(point)) 
		{
			nRet=i;
			break;
		}
		yFrom+=l_YSPACE;
	}
	return nRet;
}

/////////////////////////////////////////////////////////////////////////////
// CDeepWarnDlg dialog

extern int l_nBigVol_Flag;

CDeepWarnDlg::CDeepWarnDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeepWarnDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeepWarnDlg)
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDI_DEEPWARN);
	m_nCurSel = -1;
	m_nLastPos = -1;
	m_nRowNum = m_nDrawPos = m_nDrawNum = 0;

	m_nFlag = l_nBigVol_Flag;
	m_nCurGP = -1;
	m_pWarnAtom = NULL;
	m_nWarnNum = CopyToMyWarnAtom(m_pWarnAtom,m_nFlag,m_nCurGP);
}

CDeepWarnDlg::~CDeepWarnDlg()
{
	TDEL(m_pWarnAtom);	
	l_nBigVol_Flag = m_nFlag;
}

void CDeepWarnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeepWarnDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeepWarnDlg, CDialog)
	//{{AFX_MSG_MAP(CDeepWarnDlg)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_MESSAGE(UPDATE_WARNGP,OnUpdateWarnGPMessage)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeepWarnDlg message handlers

static int DeepWarnDlg_Cx=0,DeepWarnDlg_Cy=0;
BOOL CDeepWarnDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon,TRUE);
	SetIcon(m_hIcon,FALSE);
	
    //从ini读出相应的配置
	int x,y,nCx,nCy, nCxw, nCyw;
	CRect rect, rcw;
	GetClientRect(&rect);
	nCx=rect.Width();
	nCy=rect.Height();
	GetWindowRect(&rcw);
	nCxw=rcw.Width();
	nCyw=rcw.Height();
	CIniFile inif;
	inif.SetFileName(g_WSXHStr+"user.ini");
	inif.SetAppName("DeepWarnDlg");
	x=inif.GetInt("left",ScreenCx-nCx-208);
	y=inif.GetInt("top",ScreenCy-nCy-80);
	if(DeepWarnDlg_Cx>0) nCxw=DeepWarnDlg_Cx;
	if(DeepWarnDlg_Cy>0) nCyw=DeepWarnDlg_Cy;
	if(x>-180 && x<ScreenCx-30 && y>-80 && y<ScreenCy-30)
		SetWindowPos(&wndTop,x,y,nCxw,nCyw,SWP_SHOWWINDOW);

	SetTimer(0x100,500,NULL);

	return TRUE;
}

BOOL CDeepWarnDlg::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CDeepWarnDlg::OnUpdateWarnGPMessage(WPARAM wParam,LPARAM lParam)
{
	m_nLastPos = -1;
	m_nWarnNum = CopyToMyWarnAtom(m_pWarnAtom,m_nFlag,m_nCurGP);
	Invalidate(FALSE);
}

void CDeepWarnDlg::OnPaint() 
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(&rect);
	CBitmap		tempBitmap;				//作出临时位图
	CDC memDC;							//作出内存DC
	memDC.CreateCompatibleDC (&dc);
	tempBitmap.CreateCompatibleBitmap (&dc,rect.Width (),rect.Height());
	CBitmap *pOldBitmap = memDC.SelectObject (&tempBitmap);
	g_d.Bar(&memDC,&rect,BACK_BRUSH);
	memDC.SetBkMode (TRANSPARENT);
	
	DrawDeepWarnList(TRUE,m_nWarnNum,m_pWarnAtom,m_nFlag,&memDC,rect,m_nCurSel,m_nLastPos,m_nDrawPos,m_nDrawNum,m_nRowNum);

	dc.BitBlt (0,0,rect.Width(),rect.Height(),&memDC,0,0,SRCCOPY);//从内存DC拷贝到屏幕DC

	memDC.SelectObject(pOldBitmap);
	tempBitmap.DeleteObject();
	memDC.DeleteDC();
}

void CDeepWarnDlg::OnOK() 
{
}

void CDeepWarnDlg::OnCancel() 
{
    //将ini读入相应的配置
	if(!IsIconic())
	{
		CRect rc;
		GetWindowRect(rc);

		CIniFile inif;
		inif.SetFileName(g_WSXHStr+"user.ini");
		int nCxw=rc.Width();
		int nCyw=rc.Height();
		DeepWarnDlg_Cx=nCxw;
		DeepWarnDlg_Cy=nCyw;
		inif.SetAppName("DeepWarnDlg");
		inif.PutInt(rc.left,"left");
		inif.PutInt(rc.top,"top");
	}
	//
	GetParent()->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);
}

void CDeepWarnDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	Invalidate(FALSE);
}

void CDeepWarnDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rc;
	GetClientRect(&rc);
	int nSel = DeepWarnHitTest(m_nWarnNum,m_pWarnAtom,point,rc,m_nLastPos,m_nDrawPos,m_nDrawNum);
	if(nSel==-14)
		return;
	else if(nSel<=-10)
	{
		m_nFlag = (-nSel)-10;
		m_nLastPos = -1;
		m_nWarnNum = CopyToMyWarnAtom(m_pWarnAtom,m_nFlag,m_nCurGP);
	}
	else
		m_nCurSel = nSel;
	Invalidate(FALSE);
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CDeepWarnDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CRect rc;
	GetClientRect(&rc);
	int nSel = DeepWarnHitTest(m_nWarnNum,m_pWarnAtom,point,rc,m_nLastPos,m_nDrawPos,m_nDrawNum);
	if(nSel<=-10) return;
	m_nCurSel = nSel;
	Invalidate(FALSE);

	if(m_nCurSel>=0 && m_nCurSel<m_nWarnNum)
	{
		MemStkInfo *tmpInfo = g_pStockIO->Get_MemStkInfo(m_pWarnAtom[m_nCurSel].Code,m_pWarnAtom[m_nCurSel].SetCode);
		if(tmpInfo)
			ToZst(tmpInfo->Code,tmpInfo->setcode,FALSE);
	}

	CDialog::OnLButtonDblClk(nFlags, point);
}

BOOL CDeepWarnDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN && m_nLastPos>0)
	{
		if(pMsg->wParam == VK_UP || pMsg->wParam == VK_PRIOR)
		{
			if(m_nCurSel<m_nDrawPos || m_nCurSel>=m_nDrawPos+m_nDrawNum)
			{
				m_nCurSel=m_nDrawPos+m_nDrawNum-1;
				Invalidate(FALSE);
			}
			else if(m_nCurSel>0)
			{
				if(pMsg->wParam == VK_UP)
				{
					if(m_nCurSel-1<m_nDrawPos)
						m_nLastPos-=1;
					m_nCurSel-=1;
				}
				else
				{
					if(m_nCurSel-m_nRowNum<m_nDrawPos)
						m_nLastPos=max(m_nRowNum-1,m_nLastPos-m_nRowNum);
					m_nCurSel=max(0,m_nCurSel-m_nRowNum);
				}
				Invalidate(FALSE);
			}
			return TRUE;
		}
		if(pMsg->wParam == VK_DOWN || pMsg->wParam == VK_NEXT)
		{
			if(m_nCurSel<m_nDrawPos || m_nCurSel>=m_nDrawPos+m_nDrawNum)
			{
				m_nCurSel=m_nDrawPos;
				Invalidate(FALSE);
			}
			else if(m_nCurSel<m_nWarnNum-1)
			{
				if(pMsg->wParam == VK_DOWN)
				{
					if(m_nCurSel+1>m_nLastPos)
						m_nLastPos++;
					m_nCurSel++;
				}
				else
				{
					if(m_nCurSel+m_nRowNum>m_nLastPos)
						m_nLastPos=min(m_nWarnNum-1,m_nLastPos+m_nRowNum);
					m_nCurSel=min(m_nWarnNum-1,m_nCurSel+m_nRowNum);
				}
				Invalidate(FALSE);
			}
			return TRUE;
		}
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
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CDeepWarnDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if(zDelta < 0)
		PostMessage(WM_KEYDOWN,VK_NEXT,1);
	else
		PostMessage(WM_KEYDOWN,VK_PRIOR,1);

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CDeepWarnDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==0x100)	
	{
		int nTmpCurGP=GetSYSCurrentGPIndex();
		if(nTmpCurGP!=-1 && nTmpCurGP!=m_nCurGP && m_nFlag==3)
			PostMessage(UPDATE_WARNGP);
	}	
	CDialog::OnTimer(nIDEvent);
}