#include "stdafx.h"
#include "UHq10.h"
#include "ComView.h"

#ifdef	BIG5
	char *buysellstr_l2[2][10] = {{"卖一","卖二","卖三","卖四","卖五","卖六","卖七","卖八","卖九","卖十"},{"买一","买二","买三","买四","买五","买六","买七","买八","买九","买十"}};
#else
	char *buysellstr_l2[2][10] = {{"卖①","卖②","卖③","卖④","卖⑤","卖⑥","卖⑦","卖⑧","卖⑨","卖⑩"},{"买①","买②","买③","买④","买⑤","买⑥","买⑦","买⑧","买⑨","买⑩"}};
#endif

UHq10::UHq10(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_bEmpty = TRUE;
	m_bDataReady = FALSE;
	for(int i=0;i<10;i++)
	{
		m_BuyRect[i].SetRectEmpty();
		m_SellRect[i].SetRectEmpty();
	}
	memset(&m_Hq,0,sizeof(CurrStockData));
	memset(&m_Hq_L2,0,sizeof(CurrStockData_L2));
	m_pStkInfoBak	= NULL;
	m_bChangeStk	= TRUE;

	m_nBuyAvpFlag = 2;
	m_nSellAvpFlag = 2;
	m_nTotalBuyvFlag = 2;
	m_nTotalSellvFlag = 2;
}

UHq10::~UHq10()
{

}

int  UHq10::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_nCurGP==-1) //如果Unit为空，则另行处理
	{
		if(message==WM_PAINT)
		{
			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
			DESTORY_TMPDC
			return 1;
		}
		return UBase::ProcessMsg(message,wParam,lParam);
	}
	switch(message)
	{
	case UM_READDATA:
		m_bDataReady = FALSE;
		ReadData(FALSE,wParam==0);
		return 1;
	case UM_GETDATA_ACK://对于远程版,请求和返回分开
		switch(wParam)
		{
		case 0:	
			m_bDataReady = TRUE;
			ReadData(TRUE);
			break;
		}
		ProcessMsg(WM_PAINT);
		return 1;
	case ASYN_HQ_MESSAGE:
		{
			if(!m_pStkInfo)	return 1;
			short nTmpCurGp = g_pStockIO->GetIndex(m_pStkInfo->Code, m_pStkInfo->setcode);
			if(wParam == nTmpCurGp)
				ProcessMsg(UM_UPDATE);
		}
		return 1;
	case UM_UPDATE:
		if(!m_bDataReady)	return 1;
		g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq,&m_Hq_L2);
		CalcIt();
		ProcessMsg(WM_PAINT);
		return 1;
	case WM_PAINT:
		{
			if(wParam == 1)
				ProcessMsg(UM_READDATA,lParam);

			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);
			m_MyDC.SetBkMode(TRANSPARENT);

			CFont *oldfont = g_d.SelectFont(&m_MyDC,INFO_FONT);
			CPen *oldpen = g_d.SelectPen(&m_MyDC,AXIS_PEN);
			g_d.DrawLine(&m_MyDC,0,0,0,m_rcIn.bottom);
			if(g_GH.bLeftHQInfo && !m_nInCom2)
				g_d.DrawLine(&m_MyDC,m_rcIn.right-1,0,m_rcIn.right-1,m_rcIn.bottom);
			if(g_GH.bHq10Bottom)
				g_d.DrawLine(&m_MyDC,0,0,m_rcIn.right,0);
			else
				g_d.DrawLine(&m_MyDC,0,m_rcIn.bottom,m_rcIn.right,m_rcIn.bottom);
			DrawIt(&m_MyDC);
			g_d.RestorePen(&m_MyDC,oldpen);
			g_d.RestoreFont(&m_MyDC,oldfont);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
			DESTORY_TMPDC
		}
		return 1;
	case WM_MOUSEMOVE:
		{
			CPoint pt(LOWORD(lParam),HIWORD(lParam));
			pt.x-=left;pt.y-=top;
			if(g_OEM.bEmbedWTVersion)
			{
				int n,i=-1,j=-1;
				for(n=0;n<10;n++)
				{
					if(m_SellRect[n].PtInRect(pt))
					{	i=n;break;	}
					if(m_BuyRect[n].PtInRect(pt))
					{	j=n;break;	}
				}
				if(i>=0 || j>=0)
				{
					SetCursor(m_hFxtMove);
					return 1;
				}
				else
					SetCursor(LoadCursor(NULL,IDC_ARROW));
			}
		}
		return 1;
	case WM_LBUTTONDBLCLK:
		{
			CPoint pt(LOWORD(lParam),HIWORD(lParam));
			pt.x-=left;pt.y-=top;			
			if(g_OEM.bEmbedWTVersion && g_ExternLevinCfg.bCanDbClickPrice)
			{
				int n,i=-1,j=-1;
				for(n=0;n<10;n++)
				{
					if(m_SellRect[n].PtInRect(pt))
					{	i=n;break;	}
					if(m_BuyRect[n].PtInRect(pt))
					{	j=n;break;	}
				}
				if(i>=0 || j>=0)
				{
					if(i>=0)
						AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_LEVINBUY11+i,0);
					else if(j>=0)
						AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_LEVINSELL1+j,0);						
				}
			}
		}
		return 1;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONUP:
		{
			CPoint pt(LOWORD(lParam),HIWORD(lParam));
			pt.x-=left;pt.y-=top;			
			if(g_OEM.bEmbedWTVersion)
			{
				int n,i=-1,j=-1;
				if(message==WM_RBUTTONUP || !g_ExternLevinCfg.bCanDbClickPrice)
				{
					for(n=0;n<10;n++)
					{
						if(m_SellRect[n].PtInRect(pt))
						{	i=n;break;	}
						if(m_BuyRect[n].PtInRect(pt))
						{	j=n;break;	}
					}
				}
				if(i>=0 || j>=0)
				{
					//先装入已有的菜单项
					CNewMenu menu;
					menu.CreatePopupMenu();
					
					CPoint tempPoint(LOWORD(lParam),HIWORD(lParam));
					m_pView->ClientToScreen (&tempPoint);
					ProcessMsg(WM_PAINT);
					menu.InsertMenu(0,MF_BYPOSITION,IDC_PRICEWARN,"设置预警委托");
					menu.InsertMenu(0,MF_BYPOSITION,IDC_WTCANCEL,"撤单查询\t22");
					//
					if(i>=0)
					{
						menu.InsertMenu(0,MF_BYPOSITION,IDC_SELL11+i,"普通卖出\t223");
						menu.InsertMenu(0,MF_BYPOSITION,IDC_BUY11+i,"普通买入\t221");
						menu.InsertMenu(0,MF_BYPOSITION,IDC_LEVINSELL11+i,"闪电卖出\t23");
						menu.InsertMenu(0,MF_BYPOSITION,IDC_LEVINBUY11+i,"闪电买入\t21");
					}
					else if(j>=0)
					{
						menu.InsertMenu(0,MF_BYPOSITION,IDC_SELL1+j,"普通卖出\t223");
						menu.InsertMenu(0,MF_BYPOSITION,IDC_BUY1+j,"普通买入\t221");
						menu.InsertMenu(0,MF_BYPOSITION,IDC_LEVINSELL1+j,"闪电卖出\t23");
						menu.InsertMenu(0,MF_BYPOSITION,IDC_LEVINBUY1+j,"闪电买入\t21");
					}
					//
					menu.TrackPopupMenu (TPM_LEFTALIGN|TPM_RIGHTBUTTON,tempPoint.x,tempPoint.y,AfxGetMainWnd());
					SetCursor(m_hFxtMove);
					return 1;
				}
			}
		}
		return 1;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

void UHq10::ReadData(BOOL bDataReady,BOOL bReqData)
{
	g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq,&m_Hq_L2);
	if(OnLine)
	{
		if(!bDataReady)
		{
			if(bReqData)
			{
				m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
				g_nAssisID = SetComAssisID(m_nWhichUnit,0);
				g_pGetData->GetCurrStockData(m_pStkInfo);
			}
		}
		else
			g_pGetData->GetCurrStockDataACK(m_pStkInfo,&m_Hq,&m_Hq_L2);
	}
	CalcIt();
}

void UHq10::CalcIt()
{
	memset(&m_Hq10Content,0,sizeof(UHq10Content));
	m_Hq10Content.Close = m_Hq.Close;
	m_Hq10Content.Now   = m_Hq.Now;
	m_Hq10Content.Max   = m_Hq.Max;
	m_Hq10Content.Min   = m_Hq.Min;
	for(int i=0;i<5;i++)
	{
		m_Hq10Content.Buyprice[i]	= m_Hq.Buyp[i];
		m_Hq10Content.Buyvol[i]		= m_Hq.Buyv[i];
		m_Hq10Content.Sellprice[i]	= m_Hq.Sellp[i];
		m_Hq10Content.Sellvol[i]	= m_Hq.Sellv[i];
		m_Hq10Content.Buyprice[5+i]	= m_Hq_L2.Buyp[i];
		m_Hq10Content.Buyvol[5+i]	= m_Hq_L2.Buyv[i];
		m_Hq10Content.Sellprice[5+i]= m_Hq_L2.Sellp[i];
		m_Hq10Content.Sellvol[5+i]	= m_Hq_L2.Sellv[i];
	}
	m_Hq10Content.BuyAvp	= m_Hq_L2.BuyAvp;
	m_Hq10Content.SellAvp	= m_Hq_L2.SellAvp;
	m_Hq10Content.TotalBuyv	= m_Hq_L2.TotalBuyv;
	m_Hq10Content.TotalSellv= m_Hq_L2.TotalSellv;

	if(m_pStkInfoBak==NULL || m_pStkInfo==NULL || memcmp(m_pStkInfoBak,m_pStkInfo,sizeof(MemStkInfo))!=0)	//换股
	{
		memcpy(&m_Hq10ContentBak,&m_Hq10Content,sizeof(UHq10Content));
		memcpy(&m_Hq10ContentShow,&m_Hq10Content,sizeof(UHq10Content));
		m_bChangeStk = TRUE;
		memcpy(&m_Hq10AveBak,&m_Hq10Content,sizeof(UHq10Content));
		m_nBuyAvpFlag		= 2;
		m_nSellAvpFlag		= 2;
		m_nTotalBuyvFlag	= 2;
		m_nTotalSellvFlag	= 2;
	}
	else m_bChangeStk = FALSE;
	m_pStkInfoBak = m_pStkInfo;
}

void UHq10::DrawPriceToken(CDC *pDC,int XFrom,int YFrom,BOOL bBuy,BOOL bPrice10,float fPrice)
{
	if(bBuy)
	{
		if(fabs(fPrice-m_Hq10Content.BuyAvp)<COMPPREC)
			pDC->DrawIcon(XFrom,YFrom,m_hIconPBuy[0]);
		else if(bPrice10 && m_Hq10Content.BuyAvp<fPrice)
		{
			if((2*m_Hq10Content.Buyprice[9]-m_Hq10Content.Buyprice[0])<m_Hq10Content.BuyAvp)
				pDC->DrawIcon(XFrom,YFrom,m_hIconPBuy[1]);
			else
				pDC->DrawIcon(XFrom,YFrom,m_hIconPBuy[2]);
		}
	}
	else
	{
		if(fabs(fPrice-m_Hq10Content.SellAvp)<COMPPREC)
			pDC->DrawIcon(XFrom,YFrom,m_hIconPSell[0]);
		else if(bPrice10 && m_Hq10Content.SellAvp>fPrice)
		{
			if((2*m_Hq10Content.Sellprice[9]-m_Hq10Content.Sellprice[0])>m_Hq10Content.SellAvp)
				pDC->DrawIcon(XFrom,YFrom,m_hIconPSell[1]);
			else
				pDC->DrawIcon(XFrom,YFrom,m_hIconPSell[2]);
		}
	}
}

void UHq10::DrawIt(CDC *pDC)
{
	if(Height() < 20) return;	//如果空间太小，不画
	if(m_bZs || m_bDSFlag) return;
	for(int j=0;j<10;j++)
	{
		m_BuyRect[j].SetRectEmpty();
		m_SellRect[j].SetRectEmpty();
	}
	//对于新股申购,只有最高最低值，也要显示出来,在早市时，现价为0不代表停牌
	m_bEmpty = m_Hq10Content.Now+m_Hq10Content.Max+m_Hq10Content.Min+m_Hq10Content.Buyprice[0]+m_Hq10Content.Sellprice[0] < COMPPREC;
	int nFixedWidth=m_rcIn.right;
	int TextH=HQ_TEXTH-1;
	int YFrom=1,XFrom1=HQ_XSPACE,XFrom2=nFixedWidth/6-1,XFrom3=nFixedWidth/2+HQ_XSPACE,XFrom4=4*nFixedWidth/6,XWidth=2*nFixedWidth/6-HQ_XSPACE,i;
	COLORREF cn = RGB(255,255,0);
	if(m_nInCom2)
	{
		g_d.SelectFont(pDC,MEDIUM_FONT);
		CSize tmpSize=g_d.DisplayText(pDC,36,YFrom,cn,"%6s ",m_pStkInfo->Code);
		g_d.DisplaySameWidtchText(pDC,36+tmpSize.cx,YFrom,cn,"%s",m_pStkInfo->Name);
		g_d.SelectFont(pDC,INFO_FONT);	
		YFrom+=19;
		g_d.DrawLine(pDC,0,YFrom,m_rcIn.right,YFrom);
		YFrom+=2;
	}
	int mmpnum=10;
	CFont *oldfont = NULL;
	if(!g_GH.bPillarBSChart)
	{
		BOOL bBspChg = memcmp(&(m_Hq10Content.Buyprice[0]),&(m_Hq10ContentBak.Buyprice[0]),20*sizeof(float)+20*sizeof(DWORD));
		if(bBspChg)
		{
			memcpy(&m_Hq10ContentShow,&m_Hq10ContentBak,sizeof(UHq10Content));
			memcpy(&m_Hq10ContentBak,&m_Hq10Content,sizeof(UHq10Content));
		}
		for(i = 0;i <mmpnum;i++)
		{
			g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(buysellstr_l2[0][mmpnum-1-i]));
			if(!m_bEmpty)
			{
				if(m_Hq10Content.Sellprice[mmpnum-1-i]>COMPPREC)
				{
					if(i==9 && g_GH.bBigHq)
					{
						CFont *oldfont = g_d.SelectFont(pDC,BIGHQ2_FONT);
						g_d.BjPrintf(pDC,XFrom2,YFrom-3,m_Hq10Content.Sellprice[mmpnum-1-i],m_Hq10Content.Close,m_XsFlag,XWidth+3);
						g_d.RestoreFont(pDC,oldfont);
					}
					else
						g_d.BjPrintf(pDC,XFrom2,YFrom,m_Hq10Content.Sellprice[mmpnum-1-i],m_Hq10Content.Close,m_XsFlag,XWidth);
					m_SellRect[mmpnum-1-i].SetRect(XFrom1,YFrom+2,XFrom2+XWidth,YFrom+16);
					DrawPriceToken(pDC,XFrom2+XWidth+4,YFrom,FALSE,(i==0),m_Hq10Content.Sellprice[mmpnum-1-i]);
				}
				if(m_Hq10Content.Sellvol[mmpnum-1-i]>COMPPREC)
				{
					if(g_GH.bShowMmpChg)
					{
						if(m_Hq10Content.Sellprice[mmpnum-1-i]>COMPPREC)		//当前价位非0
						{
							for(j=0;j<mmpnum;j++)
								if(fabs(m_Hq10ContentBak.Sellprice[mmpnum-1-i]-m_Hq10ContentShow.Sellprice[j])<COMPPREC)	
									break;
							if(m_bChangeStk || j==mmpnum
								|| fabs(m_Hq10ContentShow.Sellprice[0]-m_Hq10ContentShow.Buyprice[0])<COMPPREC)		//找不到对应价位,或者上一笔集合竞价
							{
								if(m_bChangeStk)	//集中撮合
									g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_Hq10Content.Sellvol[mmpnum-1-i]);
								else if(fabs(m_Hq10ContentShow.Sellprice[0]-m_Hq10ContentShow.Buyprice[0])<COMPPREC && (fabs(m_Hq10Content.Sellprice[0]-m_Hq10Content.Buyprice[0])>COMPPREC)) //集合竞价最后一笔
									g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_Hq10Content.Sellvol[mmpnum-1-i]);
								else
								{
									g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_Hq10Content.Sellvol[mmpnum-1-i]);
									if(fabs(m_Hq10Content.Sellprice[0]-m_Hq10Content.Buyprice[0])>COMPPREC)			//集合竞价期间不显示成交变动
									{
										oldfont = g_d.SelectFont(pDC,GRAPH_FONT);
										g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.KPCOLOR,XWidth,"+%u",m_Hq10ContentBak.Sellvol[mmpnum-1-i]);
										g_d.RestoreFont(pDC,oldfont);
									}
								}
							}
							else if(m_Hq10ContentBak.Sellvol[mmpnum-1-i]==m_Hq10ContentShow.Sellvol[j])
								g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_Hq10Content.Sellvol[mmpnum-1-i]);
							else 
							{
								g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_Hq10Content.Sellvol[mmpnum-1-i]);
								oldfont = g_d.SelectFont(pDC,GRAPH_FONT);
								if(m_Hq10ContentBak.Sellvol[mmpnum-1-i]>m_Hq10ContentShow.Sellvol[j])	//量增加
									g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.KPCOLOR,XWidth,"+%u",m_Hq10ContentBak.Sellvol[mmpnum-1-i]-m_Hq10ContentShow.Sellvol[j]);
								else
									g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.DOWNCOLOR,XWidth,"-%u",m_Hq10ContentShow.Sellvol[j]-m_Hq10ContentBak.Sellvol[mmpnum-1-i]);
								g_d.RestoreFont(pDC,oldfont);
							}
							memcpy(&m_Hq10ContentBak,&m_Hq10Content,sizeof(UHq10Content));
						}
						else 
							g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_Hq10Content.Sellvol[mmpnum-1-i]);
					}
					else 
						g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_Hq10Content.Sellvol[mmpnum-1-i]);
				}
			}
			YFrom+=TextH;
		}
		g_d.DrawLine(pDC,0,YFrom-1,m_rcIn.right,YFrom-1);
		YFrom += 1;
		for(i = 0;i < mmpnum;i++)
		{
			g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(buysellstr_l2[1][i]));
			if(!m_bEmpty)
			{
				if(m_Hq10Content.Buyprice[i]>COMPPREC)
				{
					if(i==0 && g_GH.bBigHq)
					{
						CFont *oldfont = g_d.SelectFont(pDC,BIGHQ2_FONT);
						g_d.BjPrintf(pDC,XFrom2,YFrom,m_Hq10Content.Buyprice[i],m_Hq10Content.Close,m_XsFlag,XWidth+3);
						g_d.RestoreFont(pDC,oldfont);
					}
					else
						g_d.BjPrintf(pDC,XFrom2,YFrom,m_Hq10Content.Buyprice[i],m_Hq10Content.Close,m_XsFlag,XWidth);
					m_BuyRect[i].SetRect(XFrom1,YFrom+2,XFrom2+XWidth,YFrom+16);
					DrawPriceToken(pDC,XFrom2+XWidth+4,YFrom,TRUE,(i==mmpnum-1),m_Hq10Content.Buyprice[i]);
				}
				if(m_Hq10Content.Buyvol[i]>COMPPREC)
				{
					if(g_GH.bShowMmpChg)
					{
						if(m_Hq10Content.Buyprice[i]>COMPPREC)		//买卖盘有变化,并且当前价位非0
						{
							for(j=0;j<mmpnum;j++)
							{
								if(fabs(m_Hq10ContentBak.Buyprice[i]-m_Hq10ContentShow.Buyprice[j])<COMPPREC)
									break;
							}
							if(m_bChangeStk || j==mmpnum
								|| fabs(m_Hq10ContentShow.Sellprice[0]-m_Hq10ContentShow.Buyprice[0])<COMPPREC)		//切换股票,没有收到对比行情,找不到对应价位,或者集合竞价
							{
								if(m_bChangeStk)	//集中撮合
									g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_Hq10Content.Buyvol[i]);
								else if(fabs(m_Hq10ContentShow.Sellprice[0]-m_Hq10ContentShow.Buyprice[0])<COMPPREC && (fabs(m_Hq10Content.Sellprice[0]-m_Hq10Content.Buyprice[0])>COMPPREC))	//集合竞价最后一笔
									g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_Hq10Content.Buyvol[i]);
								else
								{
									g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_Hq10Content.Buyvol[i]);
									if(fabs(m_Hq10Content.Sellprice[0]-m_Hq10Content.Buyprice[0])>COMPPREC)			//集合竞价期间不显示成交变动
									{
										oldfont = g_d.SelectFont(pDC,GRAPH_FONT);
										g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.KPCOLOR,XWidth,"+%u",m_Hq10ContentBak.Buyvol[i]);
										g_d.RestoreFont(pDC,oldfont);
									}
								}
							}
							else if(m_Hq10ContentBak.Buyvol[i]==m_Hq10ContentShow.Buyvol[j])
								g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_Hq10Content.Buyvol[i]);
							else 
							{
								g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_Hq10Content.Buyvol[i]);
								oldfont = g_d.SelectFont(pDC,GRAPH_FONT);
								if(m_Hq10ContentBak.Buyvol[i]>m_Hq10ContentShow.Buyvol[j])	//量增加 
									g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.KPCOLOR,XWidth,"+%u",m_Hq10ContentBak.Buyvol[i]-m_Hq10ContentShow.Buyvol[j]);
								else
									g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.DOWNCOLOR,XWidth,"-%u",m_Hq10ContentShow.Buyvol[j]-m_Hq10ContentBak.Buyvol[i]);
								g_d.RestoreFont(pDC,oldfont);
							}
							memcpy(&m_Hq10ContentBak,&m_Hq10Content,sizeof(UHq10Content));
						}
						else 
							g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_Hq10Content.Buyvol[i]);
					}
					else 
						g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_Hq10Content.Buyvol[i]);
				}
			}
			YFrom+=TextH;
		}
	}
	else
	{
		DWORD	MaxVolume = 1,endBarX = 0,totalbarlen = 0;
		for(i = 0;i < mmpnum;i++)
		{
			if(m_Hq10Content.Sellvol[mmpnum-1-i] > MaxVolume)
				MaxVolume = m_Hq10Content.Sellvol[mmpnum-1-i];
		}
		for(i = 0;i < mmpnum;i++)
		{
			if(m_Hq10Content.Buyvol[i] > MaxVolume)
				MaxVolume = m_Hq10Content.Buyvol[i];
		}
		totalbarlen = XFrom4+18-50;
		if(MaxVolume > 99999)	totalbarlen = XFrom4-50; //如果是基金大量,totalbarlen变短
		for(i = 0;i < mmpnum;i++)
		{
			if(!m_bEmpty)
			{
				if(m_Hq10Content.Sellprice[mmpnum-1-i]>COMPPREC)
				{
					g_d.BjPrintf(pDC,XFrom1,YFrom,m_Hq10Content.Sellprice[mmpnum-1-i],m_Hq10Content.Close,m_XsFlag,42);
					m_SellRect[mmpnum-1-i].SetRect(XFrom1,YFrom+2,XFrom1+42,YFrom+16);
				}
				if(m_Hq10Content.Sellvol[mmpnum-1-i]>COMPPREC)
				{
					endBarX = m_Hq10Content.Sellvol[mmpnum-1-i]*totalbarlen/MaxVolume+50;
					if(endBarX < 48) endBarX = 48;
					if(m_Hq10Content.Sellprice[mmpnum-1-i] > m_Hq10Content.Close)
						g_d.Bar(pDC,50,YFrom+6,endBarX,YFrom+9,KP_BRUSH);
					else
						g_d.Bar(pDC,50,YFrom+6,endBarX,YFrom+9,DOWN_BRUSH);
					g_d.DisplayText(pDC,endBarX+3,YFrom,VipColor.VOLCOLOR,0,"%9u",m_Hq10Content.Sellvol[mmpnum-1-i]);
				}
			}
			YFrom+=TextH;
		}
		g_d.DrawLine(pDC,0,YFrom,m_rcIn.right,YFrom);
		YFrom += 1;
		for(i = 0;i < mmpnum;i++)
		{
			if(!m_bEmpty)
			{
				if(m_Hq10Content.Buyprice[i]>COMPPREC)
				{
					g_d.BjPrintf(pDC,XFrom1,YFrom,m_Hq10Content.Buyprice[i],m_Hq10Content.Close,m_XsFlag,42);
					m_BuyRect[i].SetRect(XFrom1,YFrom+2,XFrom1+42,YFrom+16);
				}
				if(m_Hq10Content.Buyvol[i]>COMPPREC)
				{
					endBarX = m_Hq10Content.Buyvol[i]*totalbarlen/MaxVolume+50;
					if(endBarX < 48) endBarX = 48;
					if(m_Hq10Content.Buyprice[i] > m_Hq10Content.Close)
						g_d.Bar(pDC,50,YFrom+6,endBarX,YFrom+9,KP_BRUSH);
					else
						g_d.Bar(pDC,50,YFrom+6,endBarX,YFrom+9,DOWN_BRUSH);
					g_d.DisplayText(pDC,endBarX+3,YFrom,VipColor.VOLCOLOR,0,"%9u",m_Hq10Content.Buyvol[i]);
				}
			}
			YFrom+=TextH;
		}
	}
	g_d.DrawLine(pDC,0,YFrom-1,m_rcIn.right,YFrom-1);
	YFrom+=1;
	if(m_Hq10Content.BuyAvp-m_Hq10AveBak.BuyAvp > COMPPREC && m_Hq10AveBak.BuyAvp>COMPPREC)		
		m_nBuyAvpFlag		= 0;
	else if(m_Hq10Content.BuyAvp-m_Hq10AveBak.BuyAvp < -COMPPREC && m_Hq10AveBak.BuyAvp>COMPPREC)	
		m_nBuyAvpFlag		= 1;
	if(m_Hq10Content.SellAvp-m_Hq10AveBak.SellAvp > COMPPREC && m_Hq10AveBak.SellAvp>COMPPREC)		
		m_nSellAvpFlag		= 0;
	else if(m_Hq10Content.SellAvp-m_Hq10AveBak.SellAvp<-COMPPREC && m_Hq10AveBak.SellAvp>COMPPREC)	
		m_nSellAvpFlag		= 1;
	if(m_Hq10Content.TotalBuyv > m_Hq10AveBak.TotalBuyv && m_Hq10AveBak.TotalBuyv>0)		
		m_nTotalBuyvFlag	= 0;
	else if(m_Hq10Content.TotalBuyv < m_Hq10AveBak.TotalBuyv && m_Hq10AveBak.TotalBuyv>0)	
		m_nTotalBuyvFlag	= 1;
	if(m_Hq10Content.TotalSellv > m_Hq10AveBak.TotalSellv && m_Hq10AveBak.TotalSellv>0)		
		m_nTotalSellvFlag	= 0;
	else if(m_Hq10Content.TotalSellv < m_Hq10AveBak.TotalSellv && m_Hq10AveBak.TotalSellv>0)	
		m_nTotalSellvFlag	= 1;
	memcpy(&m_Hq10AveBak,&m_Hq10Content,sizeof(UHq10Content));

	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("卖均"));
	if(m_nSellAvpFlag==0)
		pDC->DrawIcon(XFrom1+32,YFrom+2,m_hIconUp);
	else if(m_nSellAvpFlag==1)
		pDC->DrawIcon(XFrom1+32,YFrom+2,m_hIconDown);

	g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("总卖"));
	if(!m_bEmpty)
	{
		if(m_Hq10Content.SellAvp>COMPPREC)
			g_d.BjPrintf(pDC,XFrom2,YFrom,m_Hq10Content.SellAvp,m_Hq10Content.Close,m_XsFlag,XWidth);
		if(m_Hq10Content.TotalSellv>COMPPREC)
			g_d.DisplayColorVol(pDC,XFrom4,YFrom,m_nTotalSellvFlag,XWidth,m_Hq10Content.TotalSellv);
	}
	YFrom+=TextH;
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("买均"));
	if(m_nBuyAvpFlag==0)
		pDC->DrawIcon(XFrom1+32,YFrom+2,m_hIconUp);
	else if(m_nBuyAvpFlag==1)
		pDC->DrawIcon(XFrom1+32,YFrom+2,m_hIconDown);
	g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("总买"));
	if(!m_bEmpty)
	{
		if(m_Hq10Content.BuyAvp>COMPPREC)
			g_d.BjPrintf(pDC,XFrom2,YFrom,m_Hq10Content.BuyAvp,m_Hq10Content.Close,m_XsFlag,XWidth);
		if(m_Hq10Content.TotalBuyv>COMPPREC)
			g_d.DisplayColorVol(pDC,XFrom4,YFrom,m_nTotalBuyvFlag,XWidth,m_Hq10Content.TotalBuyv);
	}
	g_d.DrawLine(pDC,0,m_rcIn.bottom-1,m_rcIn.right,m_rcIn.bottom-1);
}
