#include "stdafx.h"
#include "UFjb.h"
#include "ComView.h"

extern DWORD GetRelPrice(MemStkInfo *pStkInfo,float fPrice,short XSFlag);

UFjb::UFjb(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_nFjNum = m_nPos = m_nStart = m_nMaxItem = 0;
	m_lMaxVolume = 0;
	m_lLastVolume = 0;
	m_fLastZs = 0.0;

	m_bDataReady = FALSE;
}

UFjb::~UFjb()
{

}

int  UFjb::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
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
		case 1:
			{
				struct PriceVolTable tmpPriceVolTable[MAX_FJNUM];
				if(m_lDate == 0)
					m_nFjNum = g_pGetData->DSGetFJBDataACK(tmpPriceVolTable);
				for(int i=0;i < m_nFjNum;i++)
				{
					m_FjContent[i].price = tmpPriceVolTable[i].Price;
					m_FjContent[i].volume = tmpPriceVolTable[i].Volume;
					m_FjContent[i].buyvol = tmpPriceVolTable[i].Kcl;
					m_FjContent[i].sellvol = tmpPriceVolTable[i].Pcl;
					m_FjContent[i].buyrate = 100.0*tmpPriceVolTable[i].Kcl/tmpPriceVolTable[i].Volume; //竞买率
					m_FjContent[i].buyrate = max(0.0,m_FjContent[i].buyrate);
					m_FjContent[i].buyrate = min(100.0,m_FjContent[i].buyrate);
				}
				for(i=0;i<m_nFjNum;i++)
					if(m_FjContent[i].volume > m_lMaxVolume) m_lMaxVolume=m_FjContent[i].volume;
				qsort((void *)m_FjContent,m_nFjNum,sizeof(struct UFjContent),FjSort);
				m_nPos = m_nFjNum-1;
			}
			break;
		}
		ProcessMsg(WM_PAINT);
		return 1;
	case ASYN_HQ_MESSAGE:
		{
			if(m_lDate != 0)	return 1;
			if(!m_pStkInfo)		return 1;
			short nTmpCurGp = g_pStockIO->GetIndex(m_pStkInfo->Code, m_pStkInfo->setcode);
			if(wParam == nTmpCurGp)
			{
				g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq);
				if( (m_bZs && (fabs(m_Hq.Now-m_fLastZs)>=COMPPREC || (long)(m_Hq.Amount/100)>m_lLastVolume))
					||(!m_bZs && m_Hq.Volume>m_lLastVolume)
					
					)
				{
					long tmp_price=0,tmp_volume=0,tmp_buyvol=0,tmp_sellvol=0;
					int tmp_InOutFlag=2;
					tmp_price = GetRelPrice(m_pStkInfo,m_Hq.Now,m_XsFlag);
					if(m_bZs)
					{
						tmp_volume=(long)(m_Hq.Amount/100)-m_lLastVolume;
						m_lLastVolume=(long)(m_Hq.Amount/100);
						m_fLastZs=m_Hq.Now;
					}
					else
					{
						tmp_volume = m_Hq.Volume-m_lLastVolume;
						m_lLastVolume=m_Hq.Volume;
					}
					if(!m_bZs)	
						tmp_InOutFlag=m_Hq.Flag.InOutFlag;
					if(tmp_InOutFlag==0)
						tmp_buyvol=tmp_volume;
					else if(tmp_InOutFlag==1)
						tmp_sellvol=tmp_volume;
					//加入到m_FjContent重新计算
					for(int i=0;i<m_nFjNum;i++)
						if(m_FjContent[i].price==tmp_price) break;
					if(i<m_nFjNum)
					{
						m_FjContent[i].volume+=tmp_volume;
						m_FjContent[i].buyvol+=tmp_buyvol;
						m_FjContent[i].sellvol+=tmp_sellvol;
					}
					else if(m_nFjNum<MAX_FJNUM) 
					{
						m_FjContent[m_nFjNum].price=tmp_price;
						m_FjContent[m_nFjNum].volume=tmp_volume;
						m_FjContent[m_nFjNum].buyvol=tmp_buyvol;
						m_FjContent[m_nFjNum].sellvol=tmp_sellvol;
						m_nFjNum++;
					}
					m_lMaxVolume = 0;
					for(i=0;i<m_nFjNum;i++)
					{
						m_FjContent[i].buyrate = 100.0*m_FjContent[i].buyvol/m_FjContent[i].volume; //竞买率
						m_FjContent[i].buyrate = max(0.0,m_FjContent[i].buyrate);
						m_FjContent[i].buyrate = min(100.0,m_FjContent[i].buyrate);
						if(m_FjContent[i].volume > m_lMaxVolume) m_lMaxVolume=m_FjContent[i].volume;
					}
					//排序
					qsort((void *)m_FjContent,m_nFjNum,sizeof(struct UFjContent),FjSort);
					m_nPos = m_nFjNum-1;
					m_nStart = m_nMaxItem = 0;
					ProcessMsg(WM_PAINT);
				}
			}
		}
		return 1;
	case UM_UPDATE:
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
			UINT nChar = (UINT)wParam;
			switch(nChar)
			{
			case VK_ESCAPE: 
				if(m_nStyle==FJB_SMALL) return 0;
				m_pView->PostMessage(WM_COMMAND,ID_ZOOMTOCORNER);
				return 1;
			case VK_UP:
				if(m_nStart <= 0) 	return 1;;
				m_nPos = max(max(m_nStart,m_nMaxItem)-1,0);
				ProcessMsg(WM_PAINT);
				return 1;
			case VK_DOWN:
				if(m_nPos >= m_nFjNum-1) return 1;
				m_nPos = min(m_nFjNum-1,m_nPos+m_nMaxItem);
				ProcessMsg(WM_PAINT);
				return 1;
			case VK_PRIOR:
				if(m_nStyle==FJB_SMALL || m_nStart <= 0) return 1;
				m_nPos = max(max(m_nStart,m_nMaxItem)-1,0);
				ProcessMsg(WM_PAINT);
				return 1;
			case VK_NEXT:
				if(m_nStyle==FJB_SMALL || m_nPos >= m_nFjNum-1) return 1;
				m_nPos = min(m_nFjNum-1,m_nPos+m_nMaxItem);
				ProcessMsg(WM_PAINT);
				return 1;
			default:				
				break;
			}
		}
		break;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

void UFjb::ReadData(BOOL bDataReady,BOOL bReqData)
{
	m_nFjNum = m_nPos = m_nStart = m_nMaxItem = 0;
	memset(&m_Hq,0,sizeof(CurrStockData));
	if(OnLine)
	{
		memset(m_FjContent,0,sizeof(UFjContent)*MAX_FJNUM);
		m_lMaxVolume = 0;
		if(m_lDate == 0)
			g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq);
		if(!bDataReady)
		{
			if(bReqData)
			{
				//先请求分时数据
				m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
				if(m_bDSFlag)
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,1);
					g_pGetData->DSGetFJBData(m_pStkInfo);
				}
				else
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,0);
					if(m_lDate == 0)
						g_pGetData->GetPriceVolTable(m_pStkInfo);
					else
						g_pGetData->GetHisPriceVolTable(m_pStkInfo,m_lDate);
				}
			}
		}
		else
		{
			struct PriceVolTable tmpPriceVolTable[MAX_FJNUM];
			if(m_lDate == 0)
				m_nFjNum = g_pGetData->GetPriceVolTableACK(m_pStkInfo,tmpPriceVolTable,MAX_FJNUM);
			else
				m_nFjNum = g_pGetData->GetHisPriceVolTableACK(m_pStkInfo,tmpPriceVolTable,MAX_FJNUM,&m_Hq.Close);
			for(int i=0;i < m_nFjNum;i++)
			{
				m_FjContent[i].price = tmpPriceVolTable[i].Price;
				m_FjContent[i].volume = tmpPriceVolTable[i].Volume;
				m_FjContent[i].buyvol = tmpPriceVolTable[i].Kcl;
				m_FjContent[i].sellvol = tmpPriceVolTable[i].Pcl;
				m_FjContent[i].buyrate = 100.0*tmpPriceVolTable[i].Kcl/tmpPriceVolTable[i].Volume; //竞买率
				m_FjContent[i].buyrate = max(0.0,m_FjContent[i].buyrate);
				m_FjContent[i].buyrate = min(100.0,m_FjContent[i].buyrate);
			}
			for(i=0;i<m_nFjNum;i++)
				if(m_FjContent[i].volume > m_lMaxVolume) m_lMaxVolume=m_FjContent[i].volume;
			qsort((void *)m_FjContent,m_nFjNum,sizeof(struct UFjContent),FjSort);
			m_nPos = m_nFjNum-1;
		}
		if(m_bZs)
			m_lLastVolume=(long)(m_Hq.Amount/100);
		else
			m_lLastVolume=m_Hq.Volume;
		return;
	}
}

void UFjb::DrawIt(CDC *pDC)
{
	if(Height() < 20) return;	//如果空间太小，不画
	int nFixedWidth = g_nFixedWidth;
	if(m_nStyle==TICK_SMALL)
		nFixedWidth = m_rcIn.right;
	int TextH=17,YFrom=2,XFrom1=2,XFrom2=nFixedWidth/3-4,XFrom3=2*nFixedWidth/3-2,i;
	int nLongPrice = 0;
	if(m_Hq.Now < 100.0) nLongPrice = 18;
	int nBrush;
	if(m_nStyle == FJB_SMALL)
	{
		if(m_nFjNum <= 0 || m_lMaxVolume < 1) return;
		int rownum = (m_rcIn.Height()-2)/(TextH+1);
		m_nMaxItem = rownum;
		m_nStart = max(0,m_nPos-rownum+1);
		for(i = m_nStart; i <= m_nPos;i++)
		{
			if(m_FjContent[i].volume<0) 
				m_FjContent[i].volume=0;
			//价位
			g_d.BjPrintf(pDC,XFrom1,YFrom,m_FjContent[i].price/1000.0,m_Hq.Close,m_XsFlag,nFixedWidth/3-nLongPrice);
			//成交量
			g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.VOLCOLOR,nFixedWidth/3,m_bZs?(MakeJE3(1.0*m_FjContent[i].volume*100)):(MakeVol3(m_FjContent[i].volume)));
			//比较Bar
			if(m_FjContent[i].price/1000.0 > m_Hq.Close)		nBrush = KP_BRUSH;
			else if(m_FjContent[i].price/1000.0 < m_Hq.Close)	nBrush = KN_BRUSH;
			else												nBrush = LEVEL_BRUSH;
			g_d.Bar(pDC,XFrom3,YFrom+3,XFrom3+(nFixedWidth/3)*(float)m_FjContent[i].volume/m_lMaxVolume,YFrom+13,nBrush);
			YFrom+=TextH+1;
		}
	}
	else
	{
		int colnum,rownum,totalnum,drawnum,drawcol;
		YFrom=3;
		CSize size = g_d.DisplayText(pDC,18,YFrom,VipColor.BIGTITLECOLOR,_F("%6s %8s   分价表"),m_pStkInfo->Code,m_pStkInfo->Name);
		g_d.DisplayText(pDC,size.cx+42,YFrom,GridColor.Grid_Level_Color,_F("  Up/PageUp:上翻 Down/PageDown:下翻"));
		int Col_Width=nFixedWidth+58;
		colnum = max(1,m_rcIn.Width()/Col_Width);
		Col_Width = (m_rcIn.Width()-1)/colnum;
		YFrom+=23;
		g_d.DrawLine(pDC,0,YFrom,colnum*Col_Width+1,YFrom);
		g_d.DrawLine(pDC,colnum*Col_Width,0,colnum*Col_Width,m_rcIn.bottom);
		YFrom+=2;
		for(i = 0;i < colnum;i++)
		{
			g_d.DisplayText(pDC,Col_Width*i+XFrom1,YFrom,VipColor.TXTCOLOR,nFixedWidth/3-nLongPrice,_F("价格"));
			g_d.DisplayText(pDC,Col_Width*i+XFrom2,YFrom,VipColor.TXTCOLOR,nFixedWidth/3+5,m_bZs?_F("成交额"):_F("成交量"));
			g_d.DisplayText(pDC,Col_Width*i+XFrom3+10,YFrom,VipColor.TXTCOLOR,_F("比例"));
			g_d.DisplayText(pDC,Col_Width*i+nFixedWidth-5,YFrom,VipColor.TXTCOLOR,_F("竞买率%%"));
			g_d.DrawLine(pDC,Col_Width*(i+1),YFrom-2,Col_Width*(i+1),m_rcIn.bottom);
		}
		YFrom+=20;
		g_d.DrawLine(pDC,0,YFrom,colnum*Col_Width+1,YFrom);
		YFrom+=4;

		if(m_nFjNum <= 0 || m_lMaxVolume < 1) return;

		rownum = (m_rcIn.Height()-YFrom)/(TextH+1);
		totalnum = rownum*colnum;	//totalnum为最多可容纳的条数
		m_nMaxItem = totalnum;
		drawnum = drawcol = 0;
		int oldYFrom = YFrom;
		m_nStart = max(0,m_nPos-totalnum+1);
		for(i = m_nStart; i <= m_nPos;i++)
		{
			//价位
			g_d.BjPrintf(pDC,Col_Width*drawcol+XFrom1,YFrom,m_FjContent[i].price/1000.0,m_Hq.Close,m_XsFlag,nFixedWidth/3-nLongPrice);
			//成交量或成交额
			g_d.DisplayText(pDC,Col_Width*drawcol+XFrom2,YFrom,VipColor.VOLCOLOR,nFixedWidth/3,m_bZs?(MakeJE3(1.0*m_FjContent[i].volume*100)):(MakeVol3(m_FjContent[i].volume)));
			//比较Bar
			if(m_FjContent[i].price/1000.0 > m_Hq.Close)		nBrush = KP_BRUSH;
			else if(m_FjContent[i].price/1000.0 < m_Hq.Close)	nBrush = KN_BRUSH;
			else												nBrush = LEVEL_BRUSH;
			g_d.Bar(pDC,Col_Width*drawcol+XFrom3+10,YFrom+4,Col_Width*drawcol+XFrom3+10+nFixedWidth/3*(float)m_FjContent[i].volume/m_lMaxVolume,YFrom+14,nBrush);
			if(!m_bZs) //竞买率
				g_d.DisplayText(pDC,Col_Width*drawcol+nFixedWidth-12,YFrom,VipColor.TXTCOLOR,nFixedWidth/3,"%5.2f",m_FjContent[i].buyrate);
			drawnum++;
			if(drawnum >= rownum*(drawcol+1))
			{
				drawcol++;
				YFrom=oldYFrom;
			}
			else	
				YFrom+=TextH+1;
		}
	}
}
