#include "stdafx.h"
#include "UFsb.h"
#include "ComView.h"

UFsb::UFsb(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_bYestodayFalg = g_bYesterdayFlag;

	m_lLastVolume = 0.0;
	m_nStart = m_nPos = m_nMaxItem = 0;
	m_nTotalMinute = 0;
	m_nDrawMinute = 0;

	m_bDataReady = FALSE;
}

UFsb::~UFsb()
{

}

int  UFsb::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_nCurGP==-1) //���UnitΪ�գ������д���
	{
		if(message==WM_PAINT)
		{
			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //���ڴ�DC��������ĻDC
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
	case UM_GETDATA_ACK://����Զ�̰�,����ͷ��طֿ�
		{
			m_bDataReady = TRUE;
			ReadData(TRUE);
			ProcessMsg(WM_PAINT);
		}
		return 1;
	case WM_TIMER:
		{
			int tmpTotalMin = 0;
			if(m_bDSFlag) tmpTotalMin = GetDSMinute(m_pTimer, m_bYestodayFalg);
			else tmpTotalMin = GetTdxMinute(TimeSec);
			if(tmpTotalMin-m_nTotalMinute > 0)
				ProcessMsg(UM_UPDATE);
		}
		return 1;
	case ASYN_HQ_MESSAGE:
		{
			if(!m_pStkInfo)		return 1;
			short nTmpCurGp = g_pStockIO->GetIndex(m_pStkInfo->Code, m_pStkInfo->setcode);
			if(wParam == nTmpCurGp)
			{
				g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq);
				if(m_Hq.Volume>m_lLastVolume)
				{
					m_lLastVolume=m_Hq.Volume;
					ProcessMsg(UM_UPDATE);
				}
			}
		}
		return 1;
	case UM_UPDATE:
		if(m_lDate != 0)  return 1;
		if(!m_bDataReady) return 1;
		g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq,NULL,&m_HqEx);
		if(m_bDSFlag)
		{
			m_bYestodayFalg = (m_HqEx.HqDate!=GetDSCorrespondingDate(m_pTimer));
			m_nTotalMinute = GetDSMinute(m_pTimer,m_bYestodayFalg);
		}
		else m_nTotalMinute=GetTdxMinute(m_Domain==SZ?sz:sh);
		if (m_Hq.Close<COMPPREC && m_Hq.Open>COMPPREC)
			m_Hq.Close=m_Hq.Open;
		
		DTWH_currMinute();

		m_nStart = m_nMaxItem = 0;
		m_nDrawMinute = m_nTotalMinute;
		m_nPos = m_nTotalMinute-1;
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
			CPen  *oldpen = g_d.SelectPen(&m_MyDC,AXIS_PEN);
			g_d.DrawLine(&m_MyDC,0,0,0,m_rcIn.bottom);
			if(g_GH.bLeftHQInfo && !m_nInCom2)
				g_d.DrawLine(&m_MyDC,m_rcIn.right-1,0,m_rcIn.right-1,m_rcIn.bottom);
			g_d.DrawLine(&m_MyDC,0,m_rcIn.bottom,m_rcIn.right,m_rcIn.bottom);
			DrawIt(&m_MyDC);
			g_d.RestorePen(&m_MyDC,oldpen);
			g_d.RestoreFont(&m_MyDC,oldfont);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //���ڴ�DC��������ĻDC
			DESTORY_TMPDC
		}
		return 1;
	case WM_KEYDOWN:
		{
			UINT nChar = (UINT)wParam;
			switch(nChar)
			{
			case VK_ESCAPE: 
				if(m_nStyle==FSB_SMALL) return 0;
				m_pView->PostMessage(WM_COMMAND,ID_ZOOMTOCORNER);
				return 1;
			case VK_UP:
				if(m_nStart <= 0) return 1;
				m_nPos = max(max(m_nStart,m_nMaxItem)-1,0);
				ProcessMsg(WM_PAINT);
				return 1;
			case VK_DOWN:
				if(m_nPos >= m_nDrawMinute-1) return 1;
				m_nPos = min(m_nDrawMinute-1,m_nPos+m_nMaxItem);
				ProcessMsg(WM_PAINT);
				return 1;
			case VK_PRIOR:
				if(m_nStyle==FSB_SMALL || m_nStart <= 0) return 1;
				m_nPos = max(max(m_nStart,m_nMaxItem)-1,0);
				ProcessMsg(WM_PAINT);
				return 1;
			case VK_NEXT:
				if(m_nStyle==FSB_SMALL || m_nPos >= m_nDrawMinute-1) return 1;
				m_nPos = min(m_nDrawMinute-1,m_nPos+m_nMaxItem);
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

//��ZST��������,���Ķ�Uzst����ش���
void UFsb::DTWH_currMinute()	//�ڵ��ô˺���ǰ��Ӧ���¶�ȡ�����m_nTotalMinute
{
	if(m_bYestodayFalg) return;
	//��̬ά����ʱ����
	m_MinuteData[m_nTotalMinute-1].Minute=GetMinuteFromFZ(m_nTotalMinute-1,1,TimeSec);
	if( m_Hq.Now < COMPPREC )	
	{
		if(m_nTotalMinute < 2)	m_Hq.Now = m_Hq.Close;
		else					m_Hq.Now = m_MinuteData[m_nTotalMinute-2].Now;
	}
	m_MinuteData[m_nTotalMinute-1].Now=m_Hq.Now;
	m_MinuteData[m_nTotalMinute-1].VolInStock_Lead.Lead=m_Hq.Lead;
	if(m_bZs)
	{
		m_MinuteData[m_nTotalMinute-1].Buyv=m_Hq.Buyv[0];
		m_MinuteData[m_nTotalMinute-1].Sellv=m_Hq.Sellv[0];
	}
	else
	{
		m_MinuteData[m_nTotalMinute-1].Buyv=m_Hq.Buyv[0]+m_Hq.Buyv[1]+m_Hq.Buyv[2]+m_Hq.Buyv[3]+m_Hq.Buyv[4];
		m_MinuteData[m_nTotalMinute-1].Sellv=m_Hq.Sellv[0]+m_Hq.Sellv[1]+m_Hq.Sellv[2]+m_Hq.Sellv[3]+m_Hq.Sellv[4];
	}
	m_MinuteData[m_nTotalMinute-1].Average=cal_averagetmp(m_pStkInfo,&m_Hq);
	if (m_bZs&&m_Hq.Sellp[2]>COMPPREC)
		m_MinuteData[m_nTotalMinute-1].Average*=m_Hq.Close/m_Hq.Sellp[2];

	//����÷��ӵ���
	long ltmp = 0;
	for(int i=0;i < m_nTotalMinute-1;i++)
		ltmp+=m_MinuteData[i].NowVol;
	if(m_bZs)	m_MinuteData[m_nTotalMinute-1].NowVol = max(0,(long)(m_Hq.Amount/10000-ltmp));
	else		m_MinuteData[m_nTotalMinute-1].NowVol = max(0,(long)(m_Hq.Volume-ltmp));
}

//��ZST��������,���Ķ�Uzst����ش���
void UFsb::ReadData(BOOL bDataReady,BOOL bReqData)
{
	int i;
	memset(&m_Hq,0,sizeof(CurrStockData));
	memset(&m_HqEx,0,sizeof(EXCURRDATA));
	memset(m_MinuteData,0,1500*sizeof(MinuteData));
	m_nTotalMinute = 0;
	m_nStart = m_nMaxItem = 0;
	short ItemNum;
	if(m_lDate == 0)	//����MinuteData
	{
		if(m_bDSFlag) 
		{
			g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq,NULL,&m_HqEx);
			m_bYestodayFalg = (m_HqEx.HqDate!=GetDSCorrespondingDate(m_pTimer));
			ItemNum=GetDSMinute(m_pTimer, m_bYestodayFalg);
		}
		else ItemNum=GetTdxMinute(TimeSec);
		if(ItemNum > 1500) return;
		g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq,NULL,&m_HqEx);
		if(m_bDSFlag) 
			m_bYestodayFalg = (m_HqEx.HqDate!=GetDSCorrespondingDate(m_pTimer));
		if(!bDataReady)
		{
			if(!OnLine) //������ѻ�״̬��ֱ�Ӷ�ȡ�����
			{
				if(GetCahceMinuteData(m_pStkInfo,m_MinuteData))
				{
					m_nTotalMinute = ItemNum;
					bDataReady = TRUE;
				}
			}
			if(OnLine && bReqData)	//���Ҫ��������
			{
				m_pView->SendMessage(UM_GETMAINID); //ֱ���ø�������дg_nMainID
				if(m_bDSFlag)
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,1);
					g_pGetData->DSGetMinData(m_pStkInfo);
				}
				else
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,0);
					short nNT = GetNowTMinute();
					g_pGetData->GetMinuteData(m_pStkInfo,FALSE,nNT,m_MinuteData,TimeSec);
				}
			}
		}
		else
		{
			if(m_bDSFlag) g_pGetData->DSGetMinDataACK(m_MinuteData);
			else	g_pGetData->GetMinuteDataACK(m_pStkInfo,m_MinuteData,NULL);
			m_nTotalMinute = ItemNum;
		}
		//���¼���Minute,��Ϊ����ʱ�䲻ƥ��ԭ��,Minute��׼ȷ
		for(i=0;i<m_nTotalMinute;i++)
			m_MinuteData[i].Minute = GetMinuteFromFZ(i,1,TimeSec);
		//ƽ��
		if (m_Hq.Close<COMPPREC && m_Hq.Open>COMPPREC)
			m_Hq.Close=m_Hq.Open;
		if(m_MinuteData[0].Now < COMPPREC)
		{
			memset(m_MinuteData,0,sizeof(MinuteData));
			m_MinuteData[0].Minute = GetMinuteFromFZ(0,1,TimeSec);
			m_MinuteData[0].Now = m_Hq.Close;
			m_MinuteData[0].Average = m_Hq.Close;
		}
		for(i = 1;i < ItemNum;i++)
		{
			if(m_MinuteData[i].Now < COMPPREC)
			{
				memset(m_MinuteData+i,0,sizeof(MinuteData));
				m_MinuteData[i].Minute = GetMinuteFromFZ(i,1,TimeSec);
				m_MinuteData[i].Now = m_MinuteData[i-1].Now;
				m_MinuteData[i].Average = m_MinuteData[i-1].Average;
			}
		}
	}
	else
	{
		if(m_bDSFlag)	ItemNum = GetRealTotalMinute(TimeSec);
		else ItemNum = GetRealTotalMinute(m_Domain==SZ?sz:sh);
		if(!bDataReady)
		{
			if(bReqData)	//���Ҫ��������
			{
				//�������ʱ����
				m_pView->SendMessage(UM_GETMAINID); //ֱ���ø�������дg_nMainID
				g_nAssisID = SetComAssisID(m_nWhichUnit,0);
				if(m_bDSFlag)
					g_pGetData->DSGetHisMinData(m_pStkInfo, m_lDate);
				else g_pGetData->GetHisMinuteData(m_pStkInfo,m_lDate);
			}
		}
		else
		{
			m_nTotalMinute = ItemNum;
			if(m_bDSFlag)
			{
				g_pGetData->DSGetHisMinDataACK(m_MinuteData, &(m_Hq.Close));
				//���¼���Minute,��Ϊ����ʱ�䲻ƥ��ԭ��,Minute��׼ȷ
				for(i=0;i<m_nTotalMinute;i++)
					m_MinuteData[i].Minute = GetMinuteFromFZ(i,1,TimeSec);
				//ƽ��
				if (m_Hq.Close<COMPPREC && m_Hq.Open>COMPPREC)
					m_Hq.Close=m_Hq.Open;
				if(m_MinuteData[0].Now < COMPPREC)
				{
					memset(m_MinuteData,0,sizeof(MinuteData));
					m_MinuteData[0].Minute = GetMinuteFromFZ(0,1,TimeSec);
					m_MinuteData[0].Now = m_Hq.Close;
					m_MinuteData[0].Average = m_Hq.Close;
				}
				if(m_MinuteData[0].Average < COMPPREC)
					m_MinuteData[0].Average = m_Hq.Close;
				if(m_pStkInfo->bigmarket==QH_TYPE && m_MinuteData[0].VolInStock_Lead.VolInStock<1)
				{
					m_MinuteData[0].VolInStock_Lead.VolInStock = m_HqEx.PreVolIn;
					if(m_MinuteData[0].VolInStock_Lead.VolInStock<1)
						m_MinuteData[0].VolInStock_Lead.VolInStock = m_Hq.Yield_VolInStock.VolInStock;
				}
				for(i = 1;i < ItemNum;i++)
				{
					if(m_MinuteData[i].Now < COMPPREC)
					{
						memset(m_MinuteData+i,0,sizeof(MinuteData));
						m_MinuteData[i].Minute = GetMinuteFromFZ(i,1,TimeSec);
						m_MinuteData[i].Now = m_MinuteData[i-1].Now;
						m_MinuteData[i].Average = m_MinuteData[i-1].Average;
					}
					if(m_MinuteData[i].Average < COMPPREC)
						m_MinuteData[i].Average = m_MinuteData[i-1].Average;
					if(m_pStkInfo->bigmarket==QH_TYPE && m_MinuteData[i].VolInStock_Lead.VolInStock<1)
						m_MinuteData[i].VolInStock_Lead.VolInStock = m_MinuteData[i-1].VolInStock_Lead.VolInStock;
				}
			}
			else
				g_pGetData->GetHisMinuteDataACK(m_pStkInfo,m_MinuteData,&(m_Hq.Close),m_lDate);
		}
	}
	//У��ָ���ĳɽ���(���ɽ����)
	if(m_bZs)
	{
		for(i=0;i < ItemNum;i++)
			m_MinuteData[i].NowVol /= 100; //���ɽ����������Ϊ��λ����ֵ
	}

	m_lLastVolume = m_Hq.Volume;
	m_nPos = m_nTotalMinute-1;
	m_nDrawMinute = m_nTotalMinute;

	if(m_lDate==0 && bDataReady && m_nTotalMinute>0)
		DTWH_currMinute();
}

char *pGpFsbStr = "ʱ��      �۸�        �ǵ� �Ƿ�(%%)     ����      �ɽ���";
char *pZsFsbStr = "ʱ��     �۸�         �ǵ� �Ƿ�(%%)     ����      �ɽ���";

void UFsb::DrawIt(CDC *pDC)
{
	if(Height() < 20) return;	//����ռ�̫С������
	int nFixedWidth = g_nFixedWidth;
	if(m_nStyle==TICK_SMALL)
		nFixedWidth = m_rcIn.right;
	int TextH=HQ_TEXTH-2;
	int YFrom=2,XFrom1=HQ_XSPACE,XFrom2=2*nFixedWidth/9,XFrom3=5*nFixedWidth/9,XFrom4,VolWidth=3*nFixedWidth/9+20-HQ_XSPACE,i;
	if(m_nStyle == FSB_SMALL)
	{
		if(m_nDrawMinute <= 0) return;

		int rownum = (m_rcIn.Height()-2)/(TextH+1);
		m_nMaxItem = rownum;
		m_nStart = max(0,m_nPos-rownum+1);
		for(i = m_nStart; i <= m_nPos;i++)
		{
			if(m_MinuteData[i].Minute==780) m_MinuteData[i].Minute=690; //����
			if(m_lDate == 0 )
				g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,0,"%02d:%02d",((m_MinuteData[i].Minute-1)/60)%24,(m_MinuteData[i].Minute-1)%60);
			else
				g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,0,"%02d:%02d",((m_MinuteData[i].Minute)/60)%24,(m_MinuteData[i].Minute)%60);
			g_d.BjPrintf(pDC,XFrom2,YFrom,m_MinuteData[i].Now,m_Hq.Close,m_XsFlag,3*nFixedWidth/9);
			if(!m_bZs)
				g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.VOLCOLOR,VolWidth,MakeVol2(m_MinuteData[i].NowVol));
			else
				g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.VOLCOLOR,VolWidth,MakeJE3(m_MinuteData[i].NowVol*10000.0));
			YFrom+=TextH+1;
		}
	}
	else
	{
		XFrom1=16,XFrom2=2*FSB_WIDTH/18,XFrom3=5*FSB_WIDTH/18,XFrom4=8*FSB_WIDTH/18;
		int XFrom5=10*FSB_WIDTH/18,XFrom6=13*FSB_WIDTH/18;
		int colnum,rownum,totalnum,drawnum,drawcol;
		char *TopicStr;
		if(m_bZs) TopicStr = pZsFsbStr;
		else	  TopicStr = pGpFsbStr;
		CSize size = g_d.DisplayText(pDC,16,YFrom,VipColor.BIGTITLECOLOR,_F("%6s %8s   ÿ���ӳɽ�"),m_pStkInfo->Code,m_pStkInfo->Name);
		g_d.DisplayText(pDC,size.cx+42,YFrom,GridColor.Grid_Level_Color,_F("Up/PageUp:�Ϸ� Down/PageDown:�·�"));

		colnum = max(1,m_rcIn.Width()/(FSB_WIDTH));
		YFrom+=23;
		g_d.DrawLine(pDC,0,YFrom,colnum*(FSB_WIDTH)+1,YFrom);
		YFrom+=2;
		for(i = 0;i < colnum;i++)
		{
			g_d.DisplayText(pDC,FSB_WIDTH*i+XFrom1,YFrom,VipColor.TXTCOLOR,_F(TopicStr));
			g_d.DrawLine(pDC,(FSB_WIDTH)*(i+1),YFrom-2,(FSB_WIDTH)*(i+1),m_rcIn.bottom);
		}
		YFrom+=20;
		g_d.DrawLine(pDC,0,YFrom,colnum*(FSB_WIDTH)+1,YFrom);
		YFrom+=4;
		
		if(m_nDrawMinute <= 0) return;

		rownum = (m_rcIn.Height()-YFrom)/(TextH+1);
		totalnum = rownum*colnum;	//totalnumΪ�������ɵ�����
		m_nMaxItem = totalnum;
		drawnum = drawcol = 0;
		int oldYFrom = YFrom;
		m_nStart = max(0,m_nPos-totalnum+1);
		for(i = m_nStart; i <= m_nPos;i++)
		{
			if(m_MinuteData[i].Minute==780) 
				m_MinuteData[i].Minute=690; //����
			if(m_lDate == 0 )
				g_d.DisplayText(pDC,FSB_WIDTH*drawcol+XFrom1,YFrom,VipColor.TXTCOLOR,0,"%02d:%02d",((m_MinuteData[i].Minute-1)/60)%24,(m_MinuteData[i].Minute-1)%60);
			else
				g_d.DisplayText(pDC,FSB_WIDTH*drawcol+XFrom1,YFrom,VipColor.TXTCOLOR,0,"%02d:%02d",((m_MinuteData[i].Minute)/60)%24,(m_MinuteData[i].Minute)%60);
			g_d.BjPrintf(pDC,FSB_WIDTH*drawcol+XFrom2,YFrom,m_MinuteData[i].Now,m_Hq.Close,m_XsFlag,3*FSB_WIDTH/18);
			g_d.BjPrintf(pDC,FSB_WIDTH*drawcol+XFrom3,YFrom,m_MinuteData[i].Now-m_Hq.Close,0.0,m_XsFlag,3*FSB_WIDTH/18);
			if(m_Hq.Close > COMPPREC)
				g_d.BjPrintf(pDC,FSB_WIDTH*drawcol+XFrom4,YFrom,(m_MinuteData[i].Now-m_Hq.Close)/m_Hq.Close*100,0.0,m_XsFlag,2*FSB_WIDTH/18);
			g_d.BjPrintf(pDC,FSB_WIDTH*drawcol+XFrom5,YFrom,m_MinuteData[i].Average,m_Hq.Close,m_XsFlag,3*FSB_WIDTH/18);
			if(!m_bZs)
				g_d.DisplayText(pDC,FSB_WIDTH*drawcol+XFrom6,YFrom,VipColor.VOLCOLOR,3*FSB_WIDTH/18,MakeVol2(m_MinuteData[i].NowVol));
			else
				g_d.DisplayText(pDC,FSB_WIDTH*drawcol+XFrom6,YFrom,VipColor.VOLCOLOR,3*FSB_WIDTH/18,MakeJE3(m_MinuteData[i].NowVol*10000.0));
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
