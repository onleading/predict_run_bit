// SDSDlg.cpp : implementation file

#include "stdafx.h"
#include "tdxw.h"
#include "SDSDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSDSDlg dialog

CSDSDlg::CSDSDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSDSDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSDSDlg)
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDI_TDXHAND);
	m_bStartFClient_OK = FALSE;
	m_XsFlag = XS2;
	memset(&m_HQ,0,sizeof(FASTHQ));
	m_NowFlag = 0;
	m_nCodeNum = 0;
	m_nSel = -1;
	m_bLevinModal = FALSE;
	m_bDrawToken = FALSE;
	for(int i=0;i<5;i++)
	{
		m_BuyRect_Sell[i].SetRectEmpty();
		m_BuyRect_Buy[i].SetRectEmpty();
		m_SellRect_Sell[i].SetRectEmpty();
		m_SellRect_Buy[i].SetRectEmpty();
	}
}

BOOL CSDSDlg::Create(CWnd *pParent)
{
	return CDialog::Create(CSDSDlg::IDD,pParent);
}

void CSDSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSDSDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSDSDlg, CDialog)
	//{{AFX_MSG_MAP(CSDSDlg)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(UM_SDS_ACK,OnSDSACK)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_COMMAND_RANGE(ID_SHQZ_1,ID_SHQZ_100,OnSHQZ)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSDSDlg::StartFClient()
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

	FClient_Start_(hqinfo,proxyinfo,m_hWnd,UM_SDS_ACK);
	return TRUE;
}

void CSDSDlg::EndFClient()
{
	if(FClient_End_)
		FClient_End_();
	UnLoadFClientDLL();
}

BOOL CSDSDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon,TRUE);
	SetIcon(m_hIcon,FALSE);
#ifdef LEVEL2
	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
#endif

	m_MainMenu.CreateMenu();
	m_CodeMenu.CreatePopupMenu();
	CString tmpMenuStr;
	m_nCodeNum = 0;
	MemStkInfo *pInfo;
	for(int i=0;i<g_pStockIO->GetStockNum(ALL_ALL);i++)
	{
		pInfo = (*g_pStockIO)[i];
		if(pInfo && strncmp(pInfo->Code,"580",3)==0)
		{
			tmpMenuStr.Format("%s %s",pInfo->Code,pInfo->Name);
			m_CodeMenu.AppendMenu( MF_STRING,ID_SHQZ_1+m_nCodeNum,_F(tmpMenuStr));
			strncpy(m_strCode[m_nCodeNum],pInfo->Code,SH_CODE_LEN);
			m_strCode[m_nCodeNum][SH_CODE_LEN]=0;
			m_nCodeNum++;
			if(m_nCodeNum>=MAX_USE_SHQZ_NUM)
				break;
		}
	}
	m_MainMenu.AppendMenu(MF_POPUP,(UINT)m_CodeMenu.m_hMenu,_F("选择权证"));
	SetMenu(&m_MainMenu);   

	m_bStartFClient_OK = StartFClient();
	if(!m_bStartFClient_OK)
		TDX_MessageBox(m_hWnd,"初始化闪电手模块失败!",MB_OK|MB_ICONERROR);

#ifdef OEM_NEWJY
	CString tmpTitle;
	GetWindowText(tmpTitle);
	tmpTitle+=_F("(可点击BS标记闪电下单)");
	SetWindowText(tmpTitle);
	m_bDrawToken=TRUE;
#endif
	return TRUE;
}

#define BSTOKEN_WID		18
#define BSTOKEN_HIG		18

extern char *buysellstr_l2[2][10];

void CSDSDlg::DrawIt(CRect rcIn,CDC *pDC)
{
	if(m_nSel<0 && g_pStockIO) 
		return;
	MemStkInfo *pInfo = g_pStockIO->Get_MemStkInfo(m_strCode[m_nSel],SH);
	if(!pInfo)
		return;
	int i,j;
	for(j=0;j<5;j++)
	{
		m_BuyRect_Sell[j].SetRectEmpty();
		m_BuyRect_Buy[j].SetRectEmpty();
		m_SellRect_Sell[j].SetRectEmpty();
		m_SellRect_Buy[j].SetRectEmpty();
	}	
	BOOL m_bEmpty = m_HQ.Now+m_HQ.Max+m_HQ.Min+m_HQ.Buyp[0]+m_HQ.Sellp[0] < COMPPREC;

	int tmpX,tmpY;
	int TextH=HQ_TEXTH+8;
	int nFixedWidth=rcIn.right;
	int YFrom=6,XPos,XFrom1=2*HQ_XSPACE-2,XFrom2=nFixedWidth/6+7,XFrom3=nFixedWidth/2+2*HQ_XSPACE,XFrom4=4*nFixedWidth/6,XWidth=2*nFixedWidth/6-2*HQ_XSPACE;
	CSize size;
	int mmpnum=5;
	for(i = 0;i < mmpnum;i++)
	{
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(buysellstr_l2[0][mmpnum-1-i]));
		if(!m_bEmpty)
		{
			if(m_HQ.Sellp[mmpnum-1-i]>COMPPREC)
			{
				g_d.BjPrintf(pDC,XFrom2,YFrom,m_HQ.Sellp[mmpnum-1-i],pInfo->Close,m_XsFlag,XWidth);
				if(m_bDrawToken)
				{
					tmpX=XFrom2+XWidth+8;
					tmpY=YFrom+8;
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
	YFrom += 6;
	g_d.DrawLine(pDC,0,YFrom-1,rcIn.right,YFrom-1);
	YFrom += 5;
	for(i = 0;i < mmpnum;i++)
	{
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(buysellstr_l2[1][i]));
		if(!m_bEmpty)
		{
			if(m_HQ.Buyp[i]>COMPPREC)
			{
				g_d.BjPrintf(pDC,XFrom2,YFrom,m_HQ.Buyp[i],pInfo->Close,m_XsFlag,XWidth);
				if(m_bDrawToken)
				{
					tmpX=XFrom2+XWidth+8;
					tmpY=YFrom+8;
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
	YFrom+=5;
	g_d.DrawLine(pDC,0,YFrom,rcIn.right,YFrom);

	CFont *oldfont = g_d.SelectFont(pDC,MEDIUM_FONT);
	YFrom+=12;

	size=g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,"%6s   %8s      最新价:",pInfo->Code,pInfo->Name);
	XPos=XFrom1+size.cx+4;
	if(!m_bEmpty && m_HQ.Now>COMPPREC)
	{
		size = g_d.BjPrintf(pDC,XPos,YFrom,m_HQ.Now,pInfo->Close,m_XsFlag,0);
		XPos+=size.cx+2;
		if(m_NowFlag==1)
			pDC->DrawIcon(XPos,YFrom+2,m_hIconUp);
		else if(m_NowFlag==2)
			pDC->DrawIcon(XPos,YFrom+2,m_hIconDown);		
	}
	g_d.RestoreFont(pDC,oldfont);
}

void CSDSDlg::OnPaint() 
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
	CFont *oldfont = g_d.SelectFont(&memDC,BIGHQ4_FONT);
	CPen  *oldpen = g_d.SelectPen(&memDC,AXIS_PEN);
	
	DrawIt(rect,&memDC);

	dc.BitBlt (0,0,rect.Width(),rect.Height(),&memDC,0,0,SRCCOPY);//从内存DC拷贝到屏幕DC	
	g_d.RestorePen(&memDC,oldpen);
	g_d.RestoreFont(&memDC,oldfont);
	memDC.SelectObject(pOldBitmap);
	tempBitmap.DeleteObject();
	memDC.DeleteDC();
}

void CSDSDlg::OnOK() //不允许用户按OK
{
}

void CSDSDlg::OnCancel() 
{
	if(m_bLevinModal)
	{
		TDX_MessageBox(m_hWnd,"请先关闭闪电下单对话框",MB_ICONINFORMATION);
		return;
	}
	if(m_bStartFClient_OK)
		EndFClient();
	if(g_pMainWin)
		g_pMainWin->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);
}

//不擦除背景
BOOL CSDSDlg::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CSDSDlg::OnSDSACK(WPARAM wParam,LPARAM lParam)
{
	if(m_nSel<0) return;
	float fLastPrice=m_HQ.Now;
	memset(&m_HQ,0,sizeof(FASTHQ));
	BOOL bRet=FClient_DataACK_(SH,m_strCode[m_nSel],&m_HQ);
	if(bRet)
	{
		if(fLastPrice<COMPPREC)
			m_NowFlag=0;
		else if(m_HQ.Now-fLastPrice>COMPPREC)
			m_NowFlag=1;
		else if(fLastPrice-m_HQ.Now>COMPPREC)
			m_NowFlag=2;
	}
	else
		m_NowFlag=0;
	Invalidate(FALSE);
}

void CSDSDlg::OnSHQZ(UINT nID)
{
	if(m_bLevinModal)
	{
		TDX_MessageBox(m_hWnd,"请先关闭闪电下单对话框",MB_ICONINFORMATION);
		return;
	}
	m_nSel = nID-ID_SHQZ_1;
	if(m_nSel<0 || m_nSel>=100)
		return;
	MemStkInfo *pInfo = g_pStockIO->Get_MemStkInfo(m_strCode[m_nSel],SH);
	if(!pInfo) return;
	if(!OnLine || !CanLevel2())
	{
		TDX_MessageBox(m_hWnd,"只有使用Level2行情主站才能使用权证闪电手功能!",MB_ICONINFORMATION|MB_OK);
		return;
	}

	memset(&m_HQ,0,sizeof(FASTHQ));
	m_NowFlag = 0;
	Invalidate(FALSE);

	m_XsFlag = GetXSFlag(pInfo);
}

void CSDSDlg::DoLevin(int which,BOOL bIsBuy)
{
	if(m_nSel<0 || m_nSel>=100)
		return;
	MemStkInfo *pInfo = g_pStockIO->Get_MemStkInfo(m_strCode[m_nSel],SH);
	if(!pInfo || !TC_DoLevinJy_) 
		return;
	short xsflag=GetXSFlag(pInfo);
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
		tmpPrice=m_HQ.Sellp[0];
		break;
	case 6: 
		tmpPrice=m_HQ.Sellp[1];
		break;
	case 7: 
		tmpPrice=m_HQ.Sellp[2];
		break;
	case 8: 
		tmpPrice=m_HQ.Sellp[3];
		break;
	case 9: 
		tmpPrice=m_HQ.Sellp[4];
		break;
	}
	m_bLevinModal=TRUE;
	TC_DoLevinJy_(bIsBuy?0:1,-1,pInfo->setcode,pInfo->Code,pInfo->Name,FloatToStr2(tmpPrice,xsflag),"",LF_NORMAIL);
	m_bLevinModal=FALSE;
}

void CSDSDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int k,i=-1,j=-1,m=-1,n=-1;
	for(k=0;k<5;k++)
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
			DoLevin(5+i,TRUE);
		else if(j>=0)
			DoLevin(j,TRUE);
		else if(m>=0)
			DoLevin(5+m,FALSE);
		else if(n>=0)
			DoLevin(n,FALSE);
		return;
	}
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CSDSDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	int k,i=-1,j=-1,m=-1,n=-1;
	for(k=0;k<5;k++)
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
