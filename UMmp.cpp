#include "stdafx.h"
#include "UMmp.h"
#include "ComView.h"

UMmp::UMmp(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_pMmp = NULL;
	m_nMmpNum = m_nPos = m_nStart = m_nMaxItem = m_nAddedMmpNum = 0;
	m_nDrawMmp = 0;

	m_lLastMmpVolume = 0;

	m_bDataReady = FALSE;
	m_bReqing = FALSE;
	m_bIsHead = FALSE;
}

UMmp::~UMmp()
{
	TDEL(m_pMmp);
}

int  UMmp::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_nCurGP==-1) //如果Unit为空，则另行处理
	{
		if(message==WM_PAINT)
		{
			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
			DESTORY_TMPDC
		}
		return UBase::ProcessMsg(message,wParam,lParam);
	}
	switch(message)
	{
	case UM_READDATA:
		{
			if(m_bZs) return 1;
			m_bReqing = TRUE;
			m_bIsHead = FALSE;
			m_bDataReady = FALSE;
			ReadData(FALSE,wParam==0);
		}
		return 1;
	case UM_GETDATA_ACK://对于远程版,请求和返回分开
		switch(wParam)
		{
		case 0:
			m_bReqing = FALSE;
			m_bDataReady = TRUE;
			ReadData(TRUE);
			break;
		case 1:
		case 2:
			{
				m_bReqing = FALSE;
				TICK_MMP * tmpMmpBuf = new TICK_MMP[WANT_MMPS+m_nMmpNum+ADDED_MMPS];
				int tmpMmpNum;
				if(wParam==1)
					tmpMmpNum = g_pGetData->GetBsp1DataACK(m_pStkInfo,tmpMmpBuf,WANT_MMPS);
				else
				{
					float tmpClose;
					tmpMmpNum = g_pGetData->GetHisBsp1DataACK(m_pStkInfo,tmpMmpBuf,WANT_MMPS,&tmpClose);
				}
				if(tmpMmpNum == 0)
				{
					m_bIsHead = TRUE;
					TDEL(tmpMmpBuf);
				}
				else
				{
					memcpy(tmpMmpBuf+tmpMmpNum,m_pMmp,m_nMmpNum*sizeof(TICK_MMP));
					TDEL(m_pMmp);
					m_pMmp = tmpMmpBuf;

					m_nMmpNum += tmpMmpNum;
					m_nDrawMmp = m_nMmpNum;
					m_nStart+=tmpMmpNum;
					m_nPos = max(max(m_nStart,m_nMaxItem)-1,0);
					m_nPos = min(m_nPos, m_nMmpNum-1);
				}
			}
			break;
		}
		ProcessMsg(WM_PAINT);
		return 1;
	case ASYN_HQ_MESSAGE:
		{
			if(m_lDate != 0)	return 1;
			if(!m_bDataReady)	return 1;
			if(m_bZs)			return 1;
			if(!m_pStkInfo)		return 1;
			short nTmpCurGp = g_pStockIO->GetIndex(m_pStkInfo->Code, m_pStkInfo->setcode);
			if(wParam == nTmpCurGp)
			{
				g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq);
				if(m_Hq.Volume>m_lLastMmpVolume)
				{			 
					TICK_MMP tmpMmp;
					memset(&tmpMmp,0,sizeof(TICK_MMP));
					tmpMmp.Minute = GetCorrespondingMin();
					tmpMmp.Now = m_Hq.Now;
					tmpMmp.NowVol = m_Hq.Volume-m_lLastMmpVolume;
					m_lLastMmpVolume=m_Hq.Volume;
					tmpMmp.InOutFlag = m_Hq.Flag.InOutFlag;
					tmpMmp.BuyP[0] = m_Hq.Buyp[0];
					tmpMmp.BuyP[1] = m_Hq.Buyp[1];
					tmpMmp.BuyP[2] = m_Hq.Buyp[2];
					tmpMmp.BuyP[3] = m_Hq.Buyp[3];
					tmpMmp.BuyP[4] = m_Hq.Buyp[4];
					tmpMmp.BuyV[0] = m_Hq.Buyv[0];
					tmpMmp.BuyV[1] = m_Hq.Buyv[1];
					tmpMmp.BuyV[2] = m_Hq.Buyv[2];
					tmpMmp.BuyV[3] = m_Hq.Buyv[3];
					tmpMmp.BuyV[4] = m_Hq.Buyv[4];
					tmpMmp.SellP[0] = m_Hq.Sellp[0];
					tmpMmp.SellP[1] = m_Hq.Sellp[1];
					tmpMmp.SellP[2] = m_Hq.Sellp[2];
					tmpMmp.SellP[3] = m_Hq.Sellp[3];
					tmpMmp.SellP[4] = m_Hq.Sellp[4];
					tmpMmp.SellV[0] = m_Hq.Sellv[0];
					tmpMmp.SellV[1] = m_Hq.Sellv[1];
					tmpMmp.SellV[2] = m_Hq.Sellv[2];
					tmpMmp.SellV[3] = m_Hq.Sellv[3];
					tmpMmp.SellV[4] = m_Hq.Sellv[4];
					if(m_nAddedMmpNum >= ADDED_MMPS)
					{
						TICK_MMP *tmpMmpBuf = new TICK_MMP[m_nMmpNum+ADDED_MMPS];
						memcpy(tmpMmpBuf,m_pMmp,m_nMmpNum*sizeof(TICK_MMP));
						TDEL(m_pMmp);
						m_pMmp = tmpMmpBuf;
					}
					m_pMmp[m_nMmpNum] = tmpMmp;
					m_nMmpNum++;
					m_nAddedMmpNum++;

					m_nStart = m_nMaxItem  = 0;
					m_nDrawMmp = m_nMmpNum;
					m_nPos = m_nMmpNum-1;
					ProcessMsg(WM_PAINT);
				}
			}
		}
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
			g_d.DrawLine(&m_MyDC,0,m_rcIn.bottom,m_rcIn.right,m_rcIn.bottom);
			DrawIt(&m_MyDC);
			g_d.RestorePen(&m_MyDC,oldpen);
			g_d.RestoreFont(&m_MyDC,oldfont);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
			DESTORY_TMPDC
		}
		return 1;
	case WM_KEYDOWN:
		{
			if(m_bZs) return 0;
			UINT nChar = (UINT)wParam;
			switch(nChar)
			{
			case VK_ESCAPE: 
				if(m_nStyle==MMP_SMALL) return 0;
				m_pView->PostMessage(WM_COMMAND,ID_ZOOMTOCORNER);
				return 1;
			case VK_UP:
			case VK_PRIOR:
				if(nChar==VK_PRIOR && m_nStyle==MMP_SMALL)	return 1;
				if(m_nStart > 0)
				{
					m_nPos = max(max(m_nStart,m_nMaxItem)-1,0);
					m_nPos = min(m_nPos, m_nMmpNum-1);
					ProcessMsg(WM_PAINT);
				}
				else if(OnLine && !m_bReqing && !m_bIsHead && m_nStart == 0) //如果没有到头,则继续请求，应该有else,因为ProcessMsg(WM_PAINT)可能m_nStart已经变了
				{
					m_bReqing = TRUE;
					m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
					if(m_lDate==0)	//当日进一步数据
					{
						g_nAssisID = SetComAssisID(m_nWhichUnit,1);
						g_pGetData->GetBsp1Data(m_pStkInfo,m_nMmpNum,WANT_MMPS);
					}
					else			//历史进一步数据
					{
						g_nAssisID = SetComAssisID(m_nWhichUnit,2);
						g_pGetData->GetHisBsp1Data(m_pStkInfo,m_lDate,m_nMmpNum,WANT_MMPS);
					}
				}
				return 1;
			case VK_DOWN:
			case VK_NEXT:
				if(nChar==VK_NEXT && m_nStyle==MMP_SMALL)	return 1;
				if(m_nPos < m_nDrawMmp-1)
				{
					m_nPos = min(m_nDrawMmp-1,m_nPos+m_nMaxItem);
					ProcessMsg(WM_PAINT);
				}
				return 1;
			default:				
				break;
			}
			break;
		}
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

void UMmp::ReadData(BOOL bDataReady,BOOL bReqData)
{
	memset(&m_Hq,0,sizeof(CurrStockData));
	m_nStart = m_nMaxItem = 0;
	m_nAddedMmpNum = 0;
	m_nMmpNum = 0;
	TDEL(m_pMmp);


	if(m_lDate == 0)	//当天Tick
	{
		g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq);
		if(!bDataReady)
		{
			if(!OnLine) //试读缓存
				m_pMmp = new TICK_MMP[WANT_MMPS+ADDED_MMPS];
			if(OnLine && bReqData)
			{
				m_pMmp = new TICK_MMP[WANT_MMPS+ADDED_MMPS];
				m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
				g_nAssisID = SetComAssisID(m_nWhichUnit,0);
				g_pGetData->GetBsp1Data(m_pStkInfo,0,(m_nStyle==MMP_BIG)?WANT_MMPS:WANT_MMPS/2);
			}
		}
		else
		{
			m_pMmp = new TICK_MMP[WANT_MMPS+ADDED_MMPS];
			m_nMmpNum = g_pGetData->GetBsp1DataACK(m_pStkInfo,m_pMmp,WANT_MMPS,TRUE);
		}
		m_lLastMmpVolume=m_Hq.Volume;
	}
	else
	{
		if(!bDataReady)
		{
			if(bReqData)
			{
				m_pMmp = new TICK_MMP[WANT_MMPS+ADDED_MMPS];
				m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
				g_nAssisID = SetComAssisID(m_nWhichUnit,0);
				g_pGetData->GetHisBsp1Data(m_pStkInfo,m_lDate,0,WANT_MMPS);
			}
		}
		else
		{
			m_pMmp = new TICK_MMP[WANT_MMPS+ADDED_MMPS];
			m_nMmpNum = g_pGetData->GetHisBsp1DataACK(m_pStkInfo,m_pMmp,WANT_MMPS,&m_Hq.Close);
		}
	}

	m_nPos = m_nMmpNum-1;
	m_nDrawMmp = m_nMmpNum;
}

char * MMP_MakeVol(long ltmp)  //特大长整形的处理
{
	static char stri[50];
	stri[0] = 0; 
	if (abs(ltmp) > 4000000000) 
		return MEANLESS_STR;
	if(abs(ltmp) < 10000)
		sprintf(stri,"%d",ltmp);
	else if(abs(ltmp) < 100000)
		sprintf(stri,_F("%7.1fW"),ltmp/10000.0);
	else 
		sprintf(stri,_F("%6dW"),ltmp/10000);
	return stri;
}

void UMmp::DrawOneCell(CDC *pDC,int YFrom,int XFrom2,int XFrom3,int i,int j)
{
	if(m_pMmp[i].SellP[j]>COMPPREC)
	{
		g_d.BjPrintf(pDC,XFrom2,YFrom,m_pMmp[i].SellP[j],m_Hq.Close,m_XsFlag,0);
		g_d.DisplayText(pDC,XFrom2+21,YFrom,VipColor.VOLCOLOR,g_nFixedWidth/4+2,"%s",MMP_MakeVol(m_pMmp[i].SellV[j]));				
	}
	if(m_pMmp[i].BuyP[4-j]>COMPPREC)
	{
		g_d.BjPrintf(pDC,XFrom3,YFrom,m_pMmp[i].BuyP[4-j],m_Hq.Close,m_XsFlag,0);
		g_d.DisplayText(pDC,XFrom3+21,YFrom,VipColor.VOLCOLOR,g_nFixedWidth/4+2,"%s",MMP_MakeVol(m_pMmp[i].BuyV[4-j]));				
	}
}

void UMmp::DrawIt(CDC *pDC)
{
	if(Height() < 20) return;	//如果空间太小，不画
	if(m_bZs)
	{
		CFont *oldfont = g_d.SelectFont(pDC,GRAPH_FONT);
		g_d.DisplayText(pDC,m_rcIn,VipColor.TXTCOLOR,DT_CENTER|DT_VCENTER|DT_SINGLELINE,_F("指数无买卖盘数据"));
		g_d.RestoreFont(pDC,oldfont);
		return;
	}
	int i,TextH=14;
	if(m_nStyle == MMP_SMALL)
	{
		if(m_nDrawMmp <= 0) return;
		int YFrom=1,XFrom1=2,XFrom2=g_nFixedWidth/3-20,XFrom3=2*g_nFixedWidth/3-10;

		int rownum = (m_rcIn.Height()-2)/(5*TextH+4);
		m_nMaxItem = rownum;
		m_nStart = max(0,m_nPos-rownum+1);
		for(i = m_nStart; i <= m_nPos;i++)
		{
			//第一行
			g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,g_nFixedWidth/5+1,"%02d:%02d",m_pMmp[i].Minute/60,m_pMmp[i].Minute%60);
			DrawOneCell(pDC,YFrom,XFrom2,XFrom3,i,4);
			//第二行
			YFrom+=TextH;
			g_d.BjPrintf(pDC,XFrom1,YFrom,m_pMmp[i].Now,m_Hq.Close,m_XsFlag,g_nFixedWidth/5+1); //Tick现价
			DrawOneCell(pDC,YFrom,XFrom2,XFrom3,i,3);			
			//第三行
			YFrom+=TextH;
			g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.VOLCOLOR,g_nFixedWidth/5+1,"%9d",m_pMmp[i].NowVol);//Tick现量
			DrawOneCell(pDC,YFrom,XFrom2,XFrom3,i,2);
			//第四行
			YFrom+=TextH;
			if(m_pMmp[i].InOutFlag==0 && !m_bZs) //BS标志
				g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.KPCOLOR,g_nFixedWidth/5+1,"B");
			else if(m_pMmp[i].InOutFlag==1 && !m_bZs)
				g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.DOWNCOLOR,g_nFixedWidth/5+1,"S");
			DrawOneCell(pDC,YFrom,XFrom2,XFrom3,i,1);
			//第五行
			YFrom+=TextH;
			DrawOneCell(pDC,YFrom,XFrom2,XFrom3,i,0);
			//
			YFrom+=TextH+4;
			g_d.DrawLine(pDC,XFrom1,YFrom,g_nFixedWidth,YFrom);
		}
	}
	//详细
	else 
	{
		int YFrom=3;
		int nRatio = 4;

		CSize size=g_d.DisplayText(pDC,18,YFrom,VipColor.BIGTITLECOLOR,_F("%6s %8s   详细买卖盘"),m_pStkInfo->Code,m_pStkInfo->Name);
		g_d.DisplayText(pDC,size.cx+42,YFrom,GridColor.Grid_Level_Color,_F("  Up/PageUp:上翻 Down/PageDown:下翻"));

		int nTotolWidth = 4+6+12*g_nFixedWidth/nRatio+6*2;
		YFrom+=23;
		g_d.DrawLine(pDC,0,YFrom,nTotolWidth,YFrom);
		YFrom+=2;

		int XFrom = 0,XFrom_tmp1,XFrom_tmp2,YFrom_tmp;
		XFrom+=3; g_d.DisplayText(pDC,XFrom,YFrom,VipColor.TXTCOLOR,0,_F("时间"));
		XFrom+=g_nFixedWidth/nRatio; g_d.DisplayText(pDC,XFrom,YFrom,VipColor.TXTCOLOR,g_nFixedWidth/nRatio,_F("卖五"));
		XFrom+=g_nFixedWidth/nRatio; g_d.DisplayText(pDC,XFrom,YFrom,VipColor.TXTCOLOR,g_nFixedWidth/nRatio,_F("卖四"));
		XFrom+=g_nFixedWidth/nRatio; g_d.DisplayText(pDC,XFrom,YFrom,VipColor.TXTCOLOR,g_nFixedWidth/nRatio,_F("卖三"));
		XFrom+=g_nFixedWidth/nRatio; g_d.DisplayText(pDC,XFrom,YFrom,VipColor.TXTCOLOR,g_nFixedWidth/nRatio,_F("卖二"));
		XFrom+=g_nFixedWidth/nRatio; g_d.DisplayText(pDC,XFrom,YFrom,VipColor.TXTCOLOR,g_nFixedWidth/nRatio,_F("卖一"));
		XFrom+=g_nFixedWidth/nRatio+6;
		
		XFrom_tmp1=XFrom+9;
		YFrom_tmp=YFrom;

		g_d.DisplayText(pDC,XFrom,YFrom,VipColor.TXTCOLOR,g_nFixedWidth/nRatio,_F("成交"));
		XFrom+=g_nFixedWidth/nRatio+6;
		
		XFrom_tmp2=XFrom+7;

		g_d.DisplayText(pDC,XFrom,YFrom,VipColor.TXTCOLOR,g_nFixedWidth/nRatio,_F("买一"));
		XFrom+=g_nFixedWidth/nRatio; g_d.DisplayText(pDC,XFrom,YFrom,VipColor.TXTCOLOR,g_nFixedWidth/nRatio,_F("买二"));
		XFrom+=g_nFixedWidth/nRatio; g_d.DisplayText(pDC,XFrom,YFrom,VipColor.TXTCOLOR,g_nFixedWidth/nRatio,_F("买三"));
		XFrom+=g_nFixedWidth/nRatio; g_d.DisplayText(pDC,XFrom,YFrom,VipColor.TXTCOLOR,g_nFixedWidth/nRatio,_F("买四"));
		XFrom+=g_nFixedWidth/nRatio; g_d.DisplayText(pDC,XFrom,YFrom,VipColor.TXTCOLOR,g_nFixedWidth/nRatio,_F("买五"));

		YFrom+=20;
		g_d.DrawLine(pDC,0,YFrom,nTotolWidth,YFrom);
		YFrom+=2;

		if(m_nDrawMmp <= 0) return;
		int rownum = (m_rcIn.Height()-YFrom)/(2*TextH+6);
		m_nMaxItem = rownum;
		m_nStart = max(0,m_nPos-rownum+1);
		for(i = m_nStart; i <= m_nPos;i++)
		{
			//第一行
			XFrom = 4;
			g_d.DisplayText(pDC,XFrom,YFrom+TextH/2+1,VipColor.TXTCOLOR,0,"%02d:%02d",m_pMmp[i].Minute/60,m_pMmp[i].Minute%60);
			XFrom+=g_nFixedWidth/nRatio; 
			if(m_pMmp[i].SellP[4]>COMPPREC)
				g_d.BjPrintf(pDC,XFrom,YFrom,m_pMmp[i].SellP[4],m_Hq.Close,m_XsFlag,g_nFixedWidth/nRatio);
			XFrom+=g_nFixedWidth/nRatio; 
			if(m_pMmp[i].SellP[3]>COMPPREC)
				g_d.BjPrintf(pDC,XFrom,YFrom,m_pMmp[i].SellP[3],m_Hq.Close,m_XsFlag,g_nFixedWidth/nRatio);
			XFrom+=g_nFixedWidth/nRatio; 
			if(m_pMmp[i].SellP[2]>COMPPREC)
				g_d.BjPrintf(pDC,XFrom,YFrom,m_pMmp[i].SellP[2],m_Hq.Close,m_XsFlag,g_nFixedWidth/nRatio);
			XFrom+=g_nFixedWidth/nRatio; 
			if(m_pMmp[i].SellP[1]>COMPPREC)
				g_d.BjPrintf(pDC,XFrom,YFrom,m_pMmp[i].SellP[1],m_Hq.Close,m_XsFlag,g_nFixedWidth/nRatio);
			XFrom+=g_nFixedWidth/nRatio; 
			if(m_pMmp[i].SellP[0]>COMPPREC)
				g_d.BjPrintf(pDC,XFrom,YFrom,m_pMmp[i].SellP[0],m_Hq.Close,m_XsFlag,g_nFixedWidth/nRatio);

			XFrom+=6;
			XFrom+=g_nFixedWidth/nRatio; g_d.BjPrintf(pDC,XFrom,YFrom,m_pMmp[i].Now,m_Hq.Close,m_XsFlag,g_nFixedWidth/nRatio);
			XFrom+=6;

			XFrom+=g_nFixedWidth/nRatio; 
			if(m_pMmp[i].BuyP[0]>COMPPREC)
				g_d.BjPrintf(pDC,XFrom,YFrom,m_pMmp[i].BuyP[0],m_Hq.Close,m_XsFlag,g_nFixedWidth/nRatio);
			XFrom+=g_nFixedWidth/nRatio; 
			if(m_pMmp[i].BuyP[1]>COMPPREC)
				g_d.BjPrintf(pDC,XFrom,YFrom,m_pMmp[i].BuyP[1],m_Hq.Close,m_XsFlag,g_nFixedWidth/nRatio);
			XFrom+=g_nFixedWidth/nRatio; 
			if(m_pMmp[i].BuyP[2]>COMPPREC)
				g_d.BjPrintf(pDC,XFrom,YFrom,m_pMmp[i].BuyP[2],m_Hq.Close,m_XsFlag,g_nFixedWidth/nRatio);
			XFrom+=g_nFixedWidth/nRatio; 
			if(m_pMmp[i].BuyP[3]>COMPPREC)
				g_d.BjPrintf(pDC,XFrom,YFrom,m_pMmp[i].BuyP[3],m_Hq.Close,m_XsFlag,g_nFixedWidth/nRatio);
			XFrom+=g_nFixedWidth/nRatio; 
			if(m_pMmp[i].BuyP[4]>COMPPREC)
				g_d.BjPrintf(pDC,XFrom,YFrom,m_pMmp[i].BuyP[4],m_Hq.Close,m_XsFlag,g_nFixedWidth/nRatio);
			
			//第二行
			XFrom = 0; YFrom+=(TextH+1);
			XFrom+=3;
			
			//卖盘
			for(int k=4;k>=0;k--)
			{
				XFrom+=g_nFixedWidth/nRatio;
				if(m_pMmp[i].SellP[k]<COMPPREC) continue;
				g_d.DisplayText(pDC,XFrom,YFrom,VipColor.VOLCOLOR,g_nFixedWidth/nRatio,"%s",MMP_MakeVol(m_pMmp[i].SellV[k]));
			}
			
			//成交
			XFrom+=6;
			XFrom+=g_nFixedWidth/nRatio; 
			g_d.DisplayText(pDC,XFrom,YFrom,VipColor.VOLCOLOR,g_nFixedWidth/nRatio,"%s",MMP_MakeVol(m_pMmp[i].NowVol));
			if(m_pMmp[i].InOutFlag==0 && !m_bZs) //BS标志
				g_d.DisplayText(pDC,XFrom+g_nFixedWidth/nRatio,YFrom,VipColor.KPCOLOR,0,"B");
			else if(m_pMmp[i].InOutFlag==1 && !m_bZs)
				g_d.DisplayText(pDC,XFrom+g_nFixedWidth/nRatio,YFrom,VipColor.KNCOLOR,0,"S");

			XFrom+=6;

			//买盘
			for(k=0;k<5;k++)
			{
				XFrom+=g_nFixedWidth/nRatio;
				if(m_pMmp[i].BuyP[k]<COMPPREC) continue;
				g_d.DisplayText(pDC,XFrom,YFrom,VipColor.VOLCOLOR,g_nFixedWidth/nRatio,"%s",MMP_MakeVol(m_pMmp[i].BuyV[k]));
			}

			XFrom+=g_nFixedWidth/nRatio+3;
			YFrom+=TextH+5;
			g_d.DrawLine(pDC,0,YFrom-1,nTotolWidth,YFrom-1);
			g_d.DrawLine(pDC,nTotolWidth,25,nTotolWidth,YFrom);
		}
		g_d.DotLine(pDC,XFrom_tmp1,YFrom_tmp,XFrom_tmp1,YFrom,VipColor.AXISCOLOR,2);
		g_d.DotLine(pDC,XFrom_tmp2,YFrom_tmp,XFrom_tmp2,YFrom,VipColor.AXISCOLOR,2);
	}
}
