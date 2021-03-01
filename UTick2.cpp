#include "stdafx.h"
#include "UTick2.h"
#include "ComView.h"

#define WANT_L2_TICKS	1800

UTick2::UTick2(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_pTick = NULL;
	m_pTickNoInSecond = NULL;
	m_pTickZdFlag = NULL;
	m_nTickNum = m_nPos = m_nStart = m_nMaxItem = 0;
	m_nDrawTick = 0;

	m_bDataReady = FALSE;
	m_bReqing = FALSE;
	m_bIsHead = FALSE;
	m_NowTicSeq = 0;
	m_SrvTicSeq = 0;
}

UTick2::~UTick2()
{
	TDEL(m_pTick);
	TDEL(m_pTickNoInSecond);
	TDEL(m_pTickZdFlag);
}

int  UTick2::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
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
		m_bReqing = TRUE;
		m_bIsHead = FALSE;
		m_bDataReady = FALSE;
		ReadData(FALSE,wParam==0);
		return 1;
	case UM_GETDATA_ACK://对于远程版,请求和返回分开
		switch(wParam)
		{
		case 0:
			m_bReqing = FALSE;
			m_bDataReady = TRUE;
			ReadData(TRUE);
			break;
		case 1:	//今天的进一步数据
			{
				m_bReqing = FALSE;
				m_bDataReady = TRUE;
				struct TickData_L2 *tmpTickBuf = new TickData_L2[WANT_L2_TICKS+m_nTickNum+ADDED_TICKS];
				DWORD tmpMaxTicSeq;
				int tmpTickNum = g_pGetData->GetTickDataL2ACK(m_pStkInfo,tmpTickBuf,WANT_L2_TICKS,tmpMaxTicSeq);
				if(tmpTickNum <= 0)
				{
					m_bIsHead = TRUE;
					TDEL(tmpTickBuf);
				}
				else
				{
					memcpy(tmpTickBuf+tmpTickNum,m_pTick,m_nTickNum*sizeof(struct TickData_L2));
					TDEL(m_pTick);
					m_pTick = tmpTickBuf;

					BOOL bToLast = (m_nPos==m_nTickNum-1);
					m_nTickNum += tmpTickNum;
					m_nDrawTick = m_nTickNum;
					m_nStart+=tmpTickNum;
					if(bToLast)
					{
						if(m_nStyle == TICK_SMALL)
							m_nPos = max(max(m_nStart,m_nMaxItem)-1,0);
						else						//大版面有点特殊
							m_nPos = max(max(m_nStart,min(m_nTickNum,m_nMaxItem))-1,0);
					}
					CalcTickInfo();
				}
			}
			break;
		case 2:	//正向请求的数据
			{
				m_bReqing = FALSE;
				m_bDataReady = TRUE;
				struct TickData_L2 *tmpTickBuf = new TickData_L2[WANT_L2_TICKS+m_nTickNum+ADDED_TICKS];
				int tmpTickNum = g_pGetData->GetTickDataL2ACK(m_pStkInfo,tmpTickBuf+m_nTickNum,WANT_L2_TICKS,m_NowTicSeq);
				if(tmpTickNum <= 0)
				{
					TDEL(tmpTickBuf);
				}
				else
				{
					memcpy(tmpTickBuf,m_pTick,m_nTickNum*sizeof(struct TickData_L2));
					TDEL(m_pTick);
					m_pTick = tmpTickBuf;

					BOOL bToLast = (m_nPos==m_nTickNum-1);
					m_nTickNum += tmpTickNum;
					m_nDrawTick = m_nTickNum;
					if(bToLast)
						m_nPos = m_nTickNum-1;
					CalcTickInfo();
				}
			}
			break;
		case 3:	//分笔总数
			{
				m_bReqing = FALSE;
				g_pGetData->GetTickNumL2ACK(m_pStkInfo,m_SrvTicSeq);
			}
			break;
		}
		ProcessMsg(WM_PAINT);
		return 1;
	case WM_TIMER:
		if(wParam==0x1112)
		{
			if(OnLine && !m_bReqing && m_bDataReady)
			{
				m_bReqing = TRUE;
				if(m_NowTicSeq<m_SrvTicSeq)
				{
					m_pView->SendMessage(UM_GETMAINID);
					g_nAssisID = SetComAssisID(m_nWhichUnit,2);
					g_pGetData->GetTickDataL2(m_pStkInfo,m_NowTicSeq,WANT_L2_TICKS,FALSE);//正向请求
				}
				else
				{
					m_pView->SendMessage(UM_GETMAINID);
					g_nAssisID = SetComAssisID(m_nWhichUnit,3);
					g_pGetData->GetTickNumL2(m_pStkInfo,m_NowTicSeq);
				}
			}
		}
		return 1;
	case WM_PAINT:
		{
			if(wParam == 1)
				ProcessMsg(UM_READDATA,lParam);

			CREATE_TMPDC
			if(m_nTickNum>0 && m_nPos!=m_nTickNum-1)
				g_d.Bar(&m_MyDC,&m_rcIn,GRAYBACK_BRUSH);
			else
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
				if(m_nStyle==TICK_SMALL) return 0;
				m_pView->PostMessage(WM_COMMAND,ID_ZOOMTOCORNER);
				return 1;
			case VK_UP:
			case VK_PRIOR:
				{
					SHORT CtrlDown =::GetKeyState(VK_CONTROL);
					if(nChar==VK_UP&&CtrlDown<0) return 1;				
					if(nChar==VK_PRIOR && m_nStyle==TICK_SMALL) return 1;
					if(m_nStart > 0)
					{
						m_nPos = max(max(m_nStart,m_nMaxItem)-1,0);
						ProcessMsg(WM_PAINT);
					}
					else if(OnLine && !m_bReqing && !m_bIsHead && m_nStart == 0) //如果没有到头,则继续请求，应该有else,因为ProcessMsg(WM_PAINT)可能m_nStart已经变了
					{
						m_bReqing = TRUE;
						m_pView->SendMessage(UM_GETMAINID);
						g_nAssisID = SetComAssisID(m_nWhichUnit,1);
						g_pGetData->GetTickDataL2(m_pStkInfo,m_nTickNum,WANT_L2_TICKS);
					}
				}
				return 1;
			case VK_DOWN:
			case VK_NEXT:
				{
					SHORT CtrlDown =::GetKeyState(VK_CONTROL);
					if(nChar==VK_DOWN&&CtrlDown<0) return 1;
					if(nChar==VK_NEXT && m_nStyle==TICK_SMALL)	return 1;
					if(m_nPos < m_nDrawTick-1)
					{
						m_nPos = min(m_nDrawTick-1,m_nPos+m_nMaxItem);
						ProcessMsg(WM_PAINT);
					}
				}
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

void UTick2::ReadData(BOOL bDataReady,BOOL bReqData)
{
	memset(&m_MarkInfo,0,sizeof(struct MarkInfo));
	memset(&m_Hq,0,sizeof(struct CurrStockData));
	m_nStart = m_nMaxItem = 0;
	m_nTickNum = 0;
	TDEL(m_pTick);

	g_pStockIO->ReadCurrMarkInfo(m_pStkInfo,&m_MarkInfo);
	g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq);
	if(!bDataReady)
	{
		if(OnLine && bReqData)
		{
			m_pTick = new TickData_L2[WANT_L2_TICKS+ADDED_TICKS];
			m_pView->SendMessage(UM_GETMAINID);
			g_nAssisID = SetComAssisID(m_nWhichUnit,0);
			int wantnum=WANT_TICKS;	//第一次要还是采用UTick中的少量数
			if(m_nStyle!=TICK_BIG)
			{
				wantnum=WANT_TICKS/6;
				if(ScreenCy<650 && !m_nInCom2) 
					wantnum=WANT_TICKS/12;
				if(ScreenCy>800) 
					wantnum=WANT_TICKS/6+10;
			}
			g_pGetData->GetTickDataL2(m_pStkInfo,0,wantnum);
		}
	}
	else
	{
		m_pTick = new TickData_L2[WANT_L2_TICKS+ADDED_TICKS];
		m_nTickNum = g_pGetData->GetTickDataL2ACK(m_pStkInfo,m_pTick,WANT_L2_TICKS,m_NowTicSeq);//第一次请求
		CalcTickInfo();
	}
	m_nPos = m_nTickNum-1;
	m_nDrawTick = m_nTickNum;
}

void UTick2::CalcTickInfo()
{
	if(m_nTickNum<=0) return;
	TDEL(m_pTickNoInSecond);
	TDEL(m_pTickZdFlag)
	m_pTickNoInSecond = new DWORD[m_nTickNum];
	m_pTickZdFlag = new char[m_nTickNum];
	memset(m_pTickNoInSecond,0,m_nTickNum*sizeof(DWORD));
	memset(m_pTickZdFlag,0,m_nTickNum*sizeof(char));
	DWORD nRealSecond=0,nLastRealSecond=0,nCnt=0;
	DWORD nLastPrice=0;
	for(int i=0;i<m_nTickNum;i++)
	{
		nRealSecond=(DWORD)(m_pTick[i].nSecond)+6*3600;
		if(nRealSecond==nLastRealSecond && nLastRealSecond!=0)
		{
			nCnt++;
			m_pTickNoInSecond[i]=nCnt;
		}
		else if(nRealSecond!=nLastRealSecond)
			nCnt=0;
		nLastRealSecond=nRealSecond;
		//
		if(m_pTick[i].Now>nLastPrice && nLastPrice!=0)
			m_pTickZdFlag[i]=1;
		else if(m_pTick[i].Now<nLastPrice && nLastPrice!=0)
			m_pTickZdFlag[i]=2;
		nLastPrice=m_pTick[i].Now;
	}
}

void UTick2::DrawIt(CDC *pDC)
{
	if(Height() < 20) return;	//如果空间太小，不画
	if(!CanLevel2() && OnLine && !g_OEM.bNoAdvert)
	{
		CFont *oldfont = g_d.SelectFont(pDC,GRAPH_FONT);
		CRect tmpRC=m_rcIn;
		tmpRC.DeflateRect(6,6,6,6);
		g_d.DisplayText(pDC,tmpRC,VipColor.TXTCOLOR,DT_VCENTER|DT_WORDBREAK,_F("逐笔成交明细只能用于高级行情（Level2收费行情）如需要此功能请联系我们了解并开通"));
		g_d.RestoreFont(pDC,oldfont);	
		return;
	}
	CSize tmpSize;
	DWORD nRealSecond=0,nLastRealSecond=0;
	DWORD nRealMin=0,nLastRealMin=0;
	int nFixedWidth = g_nFixedWidth;
	if(m_nStyle==TICK_SMALL)
		nFixedWidth = m_rcIn.right;
	int TextH=HQ_TEXTH-1;
	int YFrom=2,XFrom1=HQ_XSPACE,XFrom2=(nFixedWidth-2*HQ_XSPACE)/3+HQ_XSPACE,XFrom3=(nFixedWidth-2*HQ_XSPACE)*2/3+HQ_XSPACE-2,i;
	int XWidth=(nFixedWidth-2*HQ_XSPACE)/3;
	if(m_nStyle == TICK_SMALL)
	{
		if(m_nDrawTick <= 0) return;
		int rownum = (m_rcIn.Height()-2)/TextH;		
		m_nMaxItem = rownum;
		m_nStart = max(0,m_nPos-rownum+1);
		for(i = m_nStart; i <= m_nPos;i++)
		{
			if(m_pTick[i].nSecond<=0 || m_pTick[i].Now<=0) //如有异常数据,不画
				continue;
			if(i&&m_pTick[i].nSecond<m_pTick[i-1].nSecond)
				m_pTick[i].nSecond = m_pTick[i-1].nSecond;
			nRealSecond=(DWORD)(m_pTick[i].nSecond)+6*3600;
			nRealMin=nRealSecond/60;
			if(nLastRealMin!=0 && nLastRealMin!=nRealMin)
				g_d.DotLine(pDC,m_rcIn.left,YFrom-1,m_rcIn.right,YFrom-1,VipColor.AXISCOLOR,4);
			if(nLastRealSecond==0 || nLastRealSecond!=nRealSecond)
				g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,0,"%s",GetTdxTimeStrFromSecond(nRealSecond));
			else
				g_d.DisplayText(pDC,XFrom1,YFrom,RGB(128,128,128),56,"%d",m_pTickNoInSecond[i]+1);				
			g_d.BjPrintf(pDC,XFrom2,YFrom,m_pTick[i].Now/10000.0,m_Hq.Close,m_XsFlag,XWidth);

			if(m_pTickZdFlag[i]==1)
				pDC->DrawIcon(XFrom3,YFrom+3,m_hIconUp);
			else if(m_pTickZdFlag[i]==2)
				pDC->DrawIcon(XFrom3,YFrom+3,m_hIconDown);
			g_d.DisplayText(pDC,XFrom3,YFrom,IsHligtNowVol(m_pTick[i].NowVol,m_pStkInfo)?RGB(192,0,192):VipColor.VOLCOLOR,XWidth-8,"%9d",m_pTick[i].NowVol);
			if(m_pTick[i].InOutFlag==0 && g_bSupportDDE)
				g_d.DisplayText(pDC,nFixedWidth-16,YFrom,VipColor.KPCOLOR,14,"B");
			else if(m_pTick[i].InOutFlag==1 && g_bSupportDDE)
				g_d.DisplayText(pDC,nFixedWidth-16,YFrom,VipColor.DOWNCOLOR,14,"S");

			YFrom+=TextH;
			nLastRealSecond=nRealSecond;
			nLastRealMin=nRealMin;
		}
	}
	else
	{
		int colnum,rownum,totalnum,drawnum,drawcol;
		YFrom=3;
		CSize size=g_d.DisplayText(pDC,18,YFrom,VipColor.BIGTITLECOLOR,_F("%6s %8s 逐笔成交明细"),m_pStkInfo->Code,m_pStkInfo->Name);
		g_d.DisplayText(pDC,size.cx+42,YFrom,GridColor.Grid_Level_Color,_F("  Up/PageUp:上翻 Down/PageDown:下翻"));
		int Col_Width = nFixedWidth+19;
		colnum = max(1,m_rcIn.Width()/Col_Width);
		Col_Width = (m_rcIn.Width()-1)/colnum;
		YFrom+=23;
		g_d.DrawLine(pDC,0,YFrom,colnum*Col_Width+1,YFrom);
		g_d.DrawLine(pDC,colnum*Col_Width,0,colnum*Col_Width,m_rcIn.bottom);
		YFrom+=2;
		for(i = 0;i < colnum;i++)
		{
			g_d.DisplayText(pDC,Col_Width*i+XFrom1,YFrom,VipColor.TXTCOLOR,0,_F("成交时间"));
			g_d.DisplayText(pDC,Col_Width*i+XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,_F("价格"));
			g_d.DisplayText(pDC,Col_Width*i+XFrom3+2,YFrom,VipColor.TXTCOLOR,XWidth,_F("成交量"));
			g_d.DrawLine(pDC,Col_Width*(i+1),YFrom-2,Col_Width*(i+1),m_rcIn.bottom);
		}
		YFrom+=20;
		g_d.DrawLine(pDC,0,YFrom,colnum*Col_Width+1,YFrom);
		YFrom+=4;

		if(m_nDrawTick <= 0) return;

		rownum = (m_rcIn.Height()-YFrom)/TextH;
		totalnum = rownum*colnum;	//totalnum为最多可容纳的条数
		m_nMaxItem = totalnum;
		drawnum = drawcol = 0;
		int oldYFrom = YFrom;
		m_nStart = max(0,m_nPos-totalnum+1);
		for(i = m_nStart; i <= m_nPos;i++)
		{
			if(m_pTick[i].nSecond<=0 || m_pTick[i].Now<=0)  //如有异常数据,不画
				continue;
			if(i&&m_pTick[i].nSecond<m_pTick[i-1].nSecond)
				m_pTick[i].nSecond = m_pTick[i-1].nSecond;
			nRealSecond=(DWORD)(m_pTick[i].nSecond)+6*3600;
			nRealMin=nRealSecond/60;
			if(nLastRealMin!=0 && nLastRealMin!=nRealMin)
				g_d.DotLine(pDC,Col_Width*drawcol,YFrom-1,Col_Width*(drawcol+1),YFrom-1,VipColor.AXISCOLOR,4);
			if(nLastRealSecond==0 || nLastRealSecond!=nRealSecond)
				g_d.DisplayText(pDC,Col_Width*drawcol+XFrom1,YFrom,VipColor.TXTCOLOR,0,"%s",GetTdxTimeStrFromSecond(nRealSecond));
			else
				g_d.DisplayText(pDC,Col_Width*drawcol+XFrom1,YFrom,RGB(128,128,128),56,"%d",m_pTickNoInSecond[i]+1);
			g_d.BjPrintf(pDC,Col_Width*drawcol+XFrom2,YFrom,m_pTick[i].Now/10000.0,m_Hq.Close,m_XsFlag,XWidth);

			if(m_pTickZdFlag[i]==1)
				pDC->DrawIcon(Col_Width*drawcol+XFrom3,YFrom+3,m_hIconUp);
			else if(m_pTickZdFlag[i]==2)
				pDC->DrawIcon(Col_Width*drawcol+XFrom3,YFrom+3,m_hIconDown);
			g_d.DisplayText(pDC,Col_Width*drawcol+XFrom3,YFrom,
				IsHligtNowVol(m_pTick[i].NowVol,m_pStkInfo)?RGB(192,0,192):VipColor.VOLCOLOR,XWidth+14,
				(m_pStkInfo->Unit==1 && !m_bQz)?"%12d":((m_pStkInfo->Unit==10 && !m_bQz)?"%12d.%d":"%12d.%02d"),
				m_pTick[i].NowVol,m_pTick[i].RestVol);
			if(m_pTick[i].InOutFlag==0 && g_bSupportDDE)
				g_d.DisplayText(pDC,Col_Width*drawcol+Col_Width-16,YFrom,VipColor.KPCOLOR,14,"B");
			else if(m_pTick[i].InOutFlag==1 && g_bSupportDDE)
				g_d.DisplayText(pDC,Col_Width*drawcol+Col_Width-16,YFrom,VipColor.DOWNCOLOR,14,"S");

			drawnum++;
			if(drawnum >= rownum*(drawcol+1))
			{
				drawcol++;
				YFrom=oldYFrom;
			}
			else	
				YFrom+=TextH;
			nLastRealSecond=nRealSecond;
			nLastRealMin=nRealMin;
		}
	}
}
