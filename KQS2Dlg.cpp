// KQS2Dlg.cpp : implementation file

#include "stdafx.h"
#include "tdxw.h"
#include "KQS2Dlg.h"
#include "IniFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKQS2Dlg dialog

CKQS2Dlg::CKQS2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKQS2Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKQS2Dlg)
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDI_TDXHAND);
	m_bStartFClient_OK = FALSE;
	m_XsFlag = XS2;
	memset(&m_HQ,0,sizeof(FASTHQ_10));
	memset(&m_Last_HQ,0,sizeof(FASTHQ_10));
	memset(m_pTick,0,KQS_TICK_NUM*sizeof(TICKDATA));
	m_nTickNum = 0;
	m_pStkInfo = NULL;
	m_bLevinModal = FALSE;
	m_bDrawToken = FALSE;
	for(int i=0;i<10;i++)
	{
		m_BuyRect_Sell[i].SetRectEmpty();
		m_BuyRect_Buy[i].SetRectEmpty();
		m_SellRect_Sell[i].SetRectEmpty();
		m_SellRect_Buy[i].SetRectEmpty();
	}
}

BOOL CKQS2Dlg::Create(CWnd *pParent)
{
	return CDialog::Create(CKQS2Dlg::IDD,pParent);
}

void CKQS2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKQS2Dlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKQS2Dlg, CDialog)
	//{{AFX_MSG_MAP(CKQS2Dlg)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(UM_KQS_ACK,OnKQSACK)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CKQS2Dlg::StartFClient()
{
	if(!LoadFClientDLL()) 
		return FALSE;
	BOOL bRet=GetFromComteFile(COMTE_FHOST,FALSE,FALSE,COMTE_PASSWORD);
	if(!bRet || strlen(g_FHostIP)<8)
		GetFromComteFile(COMTE_FHOST,FALSE,TRUE,COMTE_PASSWORD);
	
	FHOSTINFO hqinfo;
	FPROXYINFO proxyinfo;

	memset(&proxyinfo,0,sizeof(FPROXYINFO));
	proxyinfo.bProxy=g_ProxyInfo.m_bProxy;
	proxyinfo.nSockType=g_ProxyInfo.m_nSockType;
	proxyinfo.nProxyPort=g_ProxyInfo.m_nProxyPort;
	strcpy(proxyinfo.strProxyIP,g_ProxyInfo.m_strProxyIP);
	strcpy(proxyinfo.strProxyPass,g_ProxyInfo.m_strProxyPass);
	strcpy(proxyinfo.strProxyUser,g_ProxyInfo.m_strProxyUser);

	strcpy(hqinfo.strIPAddress,g_FHostIP);
	hqinfo.nPort=g_nFHostPort;

	FClient_Start_(hqinfo,proxyinfo,m_hWnd,UM_KQS_ACK);
	return TRUE;
}

void CKQS2Dlg::EndFClient()
{
	if(FClient_End_)
		FClient_End_();
	UnLoadFClientDLL();
}

BOOL CKQS2Dlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon,TRUE);
	SetIcon(m_hIcon,FALSE);
	//读出上次的位置
	CRect rc;
	GetWindowRect(&rc);
	CIniFile inif;
	inif.SetFileName(g_WSXHStr+g_strUserCfg);
	inif.SetAppName("KQS2Dlg");
	int x=inif.GetInt("left",(ScreenCx-rc.Width())/2);
	int y=inif.GetInt("top",(ScreenCy-rc.Height())/2);	
	if(x<-30 || x>ScreenCx-30)
		x=(ScreenCx-rc.Width())/2;
	if(y<-30 || y>ScreenCy-30)
		y=(ScreenCy-rc.Height())/2;	
	SetWindowPos(NULL,x,y,0,0,SWP_NOSIZE);
	//
	if(!OnLine)
	{
		TDX_MessageBox(m_hWnd,"本地行情没有连接,初始化失败!",MB_ICONINFORMATION|MB_OK);
		return FALSE;
	}
	m_bStartFClient_OK = StartFClient();
	if(!m_bStartFClient_OK)
		TDX_MessageBox(m_hWnd,"初始化高速盘口模块失败!",MB_OK|MB_ICONERROR);

#ifdef OEM_NEWJY
	SetWindowText(_F("权证盘口王 点击BS图标下单"));
	m_bDrawToken=TRUE;
#endif

	SetTimer(0x100,500,NULL);

	return TRUE;
}

#define BSTOKEN_WID		18
#define BSTOKEN_HIG		18

extern char *buysellstr_l2[2][10];

void CKQS2Dlg::DrawIt(CRect rcIn,CDC *pDC)
{
	int i,j;
	for(j=0;j<10;j++)
	{
		m_BuyRect_Sell[j].SetRectEmpty();
		m_BuyRect_Buy[j].SetRectEmpty();
		m_SellRect_Sell[j].SetRectEmpty();
		m_SellRect_Buy[j].SetRectEmpty();
	}	
	if(!m_pStkInfo)
	{
		CFont *oldfont = g_d.SelectFont(pDC,GRAPH_FONT);
		g_d.DisplayText(pDC,rcIn,VipColor.TXTCOLOR, DT_VCENTER|DT_CENTER|DT_SINGLELINE, _F("请用键盘精灵选择Level2品种"));
		g_d.RestoreFont(pDC,oldfont);
		return;
	}
	BOOL m_bEmpty = m_HQ.Now+m_HQ.Max+m_HQ.Min+m_HQ.Buyp[0]+m_HQ.Sellp[0] < COMPPREC;

	int tmpX,tmpY;
	int TextH=HQ_TEXTH+1;
	int nFixedWidth=rcIn.right;
	int YFrom=1,XFrom1=2*HQ_XSPACE-2,XFrom2=nFixedWidth/6+7,XFrom3=nFixedWidth/2+2*HQ_XSPACE,XFrom4=4*nFixedWidth/6,XWidth=2*nFixedWidth/6-2*HQ_XSPACE;
	CSize size;
	int mmpnum=10;
	for(i = 0;i < mmpnum;i++)
	{
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(buysellstr_l2[0][mmpnum-1-i]));
		if(!m_bEmpty)
		{
			if(m_HQ.Sellp[mmpnum-1-i]>COMPPREC)
			{
				g_d.BjPrintf(pDC,XFrom2,YFrom,m_HQ.Sellp[mmpnum-1-i],m_pStkInfo->Close,m_XsFlag,XWidth);
				if(m_bDrawToken)
				{
					tmpX=XFrom2+XWidth+8;
					tmpY=YFrom+2;
					m_BuyRect_Sell[mmpnum-1-i].SetRect(tmpX,tmpY,tmpX+BSTOKEN_WID,tmpY+BSTOKEN_HIG);
					g_LevinImage.Draw(pDC,TOKEN_BUY,CPoint(tmpX,tmpY),ILD_NORMAL);

					tmpX+=BSTOKEN_WID+2;
					m_SellRect_Sell[mmpnum-1-i].SetRect(tmpX,tmpY,tmpX+BSTOKEN_WID,tmpY+BSTOKEN_HIG);
					g_LevinImage.Draw(pDC,TOKEN_SELL,CPoint(tmpX,tmpY),ILD_NORMAL);
				}
			}
			if(m_HQ.Sellv[mmpnum-1-i]>COMPPREC)
				g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_HQ.Sellv[mmpnum-1-i]);
		}
		YFrom+=TextH;
	}
	YFrom += 2;
	g_d.DrawLine(pDC,0,YFrom-1,rcIn.right,YFrom-1);
	YFrom += 2;
	for(i = 0;i < mmpnum;i++)
	{
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(buysellstr_l2[1][i]));
		if(!m_bEmpty)
		{
			if(m_HQ.Buyp[i]>COMPPREC)
			{
				g_d.BjPrintf(pDC,XFrom2,YFrom,m_HQ.Buyp[i],m_pStkInfo->Close,m_XsFlag,XWidth);
				if(m_bDrawToken)
				{
					tmpX=XFrom2+XWidth+8;
					tmpY=YFrom+2;
					m_BuyRect_Buy[i].SetRect(tmpX,tmpY,tmpX+BSTOKEN_WID,tmpY+BSTOKEN_HIG);
					g_LevinImage.Draw(pDC,TOKEN_BUY,CPoint(tmpX,tmpY),ILD_NORMAL);

					tmpX+=BSTOKEN_WID+2;
					m_SellRect_Buy[i].SetRect(tmpX,tmpY,tmpX+BSTOKEN_WID,tmpY+BSTOKEN_HIG);
					g_LevinImage.Draw(pDC,TOKEN_SELL,CPoint(tmpX,tmpY),ILD_NORMAL);
				}
			}
			if(m_HQ.Buyv[i]>COMPPREC)
				g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_HQ.Buyv[i]);
		}
		YFrom+=TextH;
	}
	YFrom += 2;
	g_d.DrawLine(pDC,0,YFrom-1,rcIn.right,YFrom-1);
	g_d.Bar(pDC,0,YFrom,rcIn.right,rcIn.bottom,GRAYBACK_BRUSH);
	for(i=0;i<m_nTickNum;i++)
	{
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,0,"%02d:%02d",(m_pTick[i].Minute/60)%24,m_pTick[i].Minute%60);
		g_d.BjPrintf(pDC,XFrom2+18,YFrom,m_pTick[i].Now/1000.0,m_pStkInfo->Close,m_XsFlag,0);
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.VOLCOLOR,"%d",m_pTick[i].NowVol);
		YFrom+=TextH-4;
	}
}

void CKQS2Dlg::OnPaint() 
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
	CFont *oldfont = g_d.SelectFont(&memDC,BIGHQ2_FONT);
	CPen  *oldpen = g_d.SelectPen(&memDC,AXIS_PEN);
	
	DrawIt(rect,&memDC);

	dc.BitBlt (0,0,rect.Width(),rect.Height(),&memDC,0,0,SRCCOPY);//从内存DC拷贝到屏幕DC	
	g_d.RestorePen(&memDC,oldpen);
	g_d.RestoreFont(&memDC,oldfont);
	memDC.SelectObject(pOldBitmap);
	tempBitmap.DeleteObject();
	memDC.DeleteDC();
}

void CKQS2Dlg::OnOK() //不允许用户按OK
{
}

void CKQS2Dlg::OnCancel() 
{
	if(m_bLevinModal)
	{
		TDX_MessageBox(m_hWnd,"请先关闭闪电下单对话框",MB_ICONINFORMATION);
		return;
	}
	if(m_bStartFClient_OK)
		EndFClient();
	//
	CIniFile inif;
	inif.SetFileName(g_WSXHStr+g_strUserCfg);
	CRect rc;
	GetWindowRect(rc);
	inif.SetAppName("KQS2Dlg");
	inif.PutInt(rc.left,"left");
	inif.PutInt(rc.top,"top");
	//
	if(g_pMainWin)
		g_pMainWin->SendMessage(WM_CANCELPRESSED,(WPARAM)this,0);
}

//不擦除背景
BOOL CKQS2Dlg::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CKQS2Dlg::OnChangeCode(MemStkInfo *pInfo)
{
	m_pStkInfo = pInfo;
	if(!m_pStkInfo || m_pStkInfo->setcode!=SH) 
		return;

	memset(&m_HQ,0,sizeof(FASTHQ_10));
	memset(&m_Last_HQ,0,sizeof(FASTHQ_10));
	m_nTickNum=0;

	m_XsFlag = GetXSFlag(m_pStkInfo);
	if(m_bStartFClient_OK)
		FClient_SubScrible_(SH,m_pStkInfo->Code,0,0,g_L2UserName,SST_10BS);
}

void CKQS2Dlg::OnKQSACK(WPARAM wParam,LPARAM lParam)
{
	if(!m_pStkInfo) return;
	m_Last_HQ=m_HQ;
	memset(&m_HQ,0,sizeof(FASTHQ_10));
	BOOL bRet=FClient_10_DataACK_(SH,m_pStkInfo->Code,&m_HQ);
	if(bRet)
	{
		CString tmpTitle;
		tmpTitle.Format("权证盘口王 %s",m_pStkInfo->Name);
		SetWindowText(_F(tmpTitle));
		
		if(m_HQ.Volume>m_Last_HQ.Volume && m_Last_HQ.Volume>0)
		{
			if(m_nTickNum>=KQS_TICK_NUM)
			{
				memmove(m_pTick,m_pTick+1,(KQS_TICK_NUM-1)*sizeof(TICKDATA));
				m_nTickNum--;
			}
			m_pTick[m_nTickNum].Minute=GetCorrespondingMin();
			m_pTick[m_nTickNum].Now=m_HQ.Now*1000;
			m_pTick[m_nTickNum].NowVol=m_HQ.Volume-m_Last_HQ.Volume;
			m_nTickNum++;
		}
	}
	Invalidate(FALSE);
}


void CKQS2Dlg::DoLevin(int which,BOOL bIsBuy)
{
	if(!m_pStkInfo || !TC_DoLevinJy_) 
		return;
	short xsflag=GetXSFlag(m_pStkInfo);
	float tmpPrice=0.0;
	switch(which)
	{
	case 0: 
		tmpPrice=m_HQ.Buyp[0];
		break;
	case 1: 
		tmpPrice=m_HQ.Buyp[1];
		break;
	case 2: 
		tmpPrice=m_HQ.Buyp[2];
		break;
	case 3: 
		tmpPrice=m_HQ.Buyp[3];
		break;
	case 4: 
		tmpPrice=m_HQ.Buyp[4];
		break;
	case 5: 
		tmpPrice=m_HQ.Buyp[5];
		break;
	case 6: 
		tmpPrice=m_HQ.Buyp[6];
		break;
	case 7: 
		tmpPrice=m_HQ.Buyp[7];
		break;
	case 8: 
		tmpPrice=m_HQ.Buyp[8];
		break;
	case 9: 
		tmpPrice=m_HQ.Buyp[9];
		break;

	case 10: 
		tmpPrice=m_HQ.Sellp[0];
		break;
	case 11: 
		tmpPrice=m_HQ.Sellp[1];
		break;
	case 12: 
		tmpPrice=m_HQ.Sellp[2];
		break;
	case 13: 
		tmpPrice=m_HQ.Sellp[3];
		break;
	case 14: 
		tmpPrice=m_HQ.Sellp[4];
		break;
	case 15: 
		tmpPrice=m_HQ.Sellp[5];
		break;
	case 16: 
		tmpPrice=m_HQ.Sellp[6];
		break;
	case 17: 
		tmpPrice=m_HQ.Sellp[7];
		break;
	case 18: 
		tmpPrice=m_HQ.Sellp[8];
		break;
	case 19: 
		tmpPrice=m_HQ.Sellp[9];
		break;
	}
	m_bLevinModal=TRUE;
	TC_DoLevinJy_(bIsBuy?0:1,-1,m_pStkInfo->setcode,m_pStkInfo->Code,m_pStkInfo->Name,FloatToStr2(tmpPrice,xsflag),"",LF_NORMAIL);
	m_bLevinModal=FALSE;
}

void CKQS2Dlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int k,i=-1,j=-1,m=-1,n=-1;
	for(k=0;k<10;k++)
	{
		if(m_BuyRect_Sell[k].PtInRect(point))
		{	i=k;break;	}
		if(m_BuyRect_Buy[k].PtInRect(point))
		{	j=k;break;	}
		if(m_SellRect_Sell[k].PtInRect(point))
		{	m=k;break;	}
		if(m_SellRect_Buy[k].PtInRect(point))
		{	n=k;break;	}
	}
	if(i>=0 || j>=0 || m>=0 || n>=0)
	{
		if(!CanUseEmbedWTFunc(m_hWnd)) 
			return;	
		if(i>=0)
			DoLevin(10+i,TRUE);
		else if(j>=0)
			DoLevin(j,TRUE);
		else if(m>=0)
			DoLevin(10+m,FALSE);
		else if(n>=0)
			DoLevin(n,FALSE);
		return;
	}
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CKQS2Dlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	int k,i=-1,j=-1,m=-1,n=-1;
	for(k=0;k<10;k++)
	{
		if(m_BuyRect_Sell[k].PtInRect(point))
		{	i=k;break;	}
		if(m_BuyRect_Buy[k].PtInRect(point))
		{	j=k;break;	}
		if(m_SellRect_Sell[k].PtInRect(point))
		{	m=k;break;	}
		if(m_SellRect_Buy[k].PtInRect(point))
		{	n=k;break;	}
	}
	if(i>=0 || j>=0 || m>=0 || n>=0)
		SetCursor(m_hFxtMove);
	else
		SetCursor(LoadCursor(NULL,IDC_ARROW));

	CDialog::OnMouseMove(nFlags, point);
}

void CKQS2Dlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==0x100)	
	{
		int nTmpCurGP=GetSYSCurrentGPIndex();
		MemStkInfo *pInfo = (*g_pStockIO)[nTmpCurGP];
		if(pInfo && pInfo->setcode==SH && pInfo!=m_pStkInfo)
			OnChangeCode(pInfo);
	}	
	CDialog::OnTimer(nIDEvent);
}

BOOL CKQS2Dlg::PreTranslateMessage(MSG* pMsg) 
{
	//将除ESC外的所有键盘输入都发给激活的视图
	if(pMsg->message==WM_KEYDOWN && theViewManager.pActiveView)
		pMsg->hwnd = theViewManager.pActiveView->m_hWnd;
	
	return CDialog::PreTranslateMessage(pMsg);
}
