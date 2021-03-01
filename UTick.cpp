#include "stdafx.h"
#include "UTick.h"
#include "ComView.h"
#include "MainFrm.h"

UTick::UTick(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_bSyncStatus = FALSE;
	m_nSyncMin = 0;

	m_pTick = NULL;
	m_nTickNum = m_nPos = m_nStart = m_nMaxItem = m_nAddedTickNum = 0;
	m_nDrawTick = 0;

	m_lLastVolume = 0;
	m_lLastTickNum = 0;
	m_fLastPrice = 0.0;
	
	m_nZstAbsMin = -1;

	m_bDataReady = FALSE;
	m_bReqing = FALSE;
	m_bIsHead = FALSE;

	memset(&m_ToDo, 0, sizeof(HasDataToDo));
}

UTick::~UTick()
{
	TDEL(m_pTick);
}

int  UTick::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
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
		case 2:	//历史的进一步数据
			{
				m_bReqing = FALSE;
				m_bDataReady = TRUE;
				struct TickData *tmpTickBuf = new TickData[WANT_TICKS+m_nTickNum+ADDED_TICKS];
				int tmpTickNum;
				if(wParam==1)
				{
					if(m_bDSFlag)
						tmpTickNum = g_pGetData->DSGetTickACK(tmpTickBuf);
					else
						tmpTickNum = g_pGetData->GetTickDataACK(m_pStkInfo,tmpTickBuf,WANT_TICKS);
				}
				else
				{
					float tmpClose;
					if(m_bDSFlag)
						tmpTickNum = g_pGetData->DSGetHisTickACK(tmpTickBuf);
					else tmpTickNum = g_pGetData->GetHisTickDataACK(m_pStkInfo,tmpTickBuf,WANT_TICKS,&tmpClose);
				}
				if(tmpTickNum == 0)
				{
					m_bIsHead = TRUE;
					TDEL(tmpTickBuf);
				}
				else
				{
					memcpy(tmpTickBuf+tmpTickNum,m_pTick,m_nTickNum*sizeof(struct TickData));
					TDEL(m_pTick);
					m_pTick = tmpTickBuf;

					m_nTickNum += tmpTickNum;
					m_nDrawTick = m_nTickNum;
					m_nStart+=tmpTickNum;
					if(m_nStyle == TICK_SMALL)
						m_nPos = max(max(m_nStart,m_nMaxItem)-1,0);
					else						//大版面有点特殊
						m_nPos = max(max(m_nStart,min(m_nTickNum,m_nMaxItem))-1,0);
				}

				if(m_nZstAbsMin>=0)
					ProcessMsg(UM_TOZSTIME, m_nZstAbsMin);
				if(m_ToDo.nDoType>0)
					SSMRegTickData();
			}
			break;
		}
		ProcessMsg(WM_PAINT);
		return 1;
	case ASYN_HQ_MESSAGE:
		{
			if(m_lDate!=0)	return 1;
			if(!OnLine && !m_bDSFlag)
				return 1;
			if(!DSOnLine && m_bDSFlag)
				return 1;
			if(m_bDSFlag && !m_bDataReady) //如果是三方行情，如果Tick未到，不能维护
				return 1;
			if(!m_pTick)	return 1;
			if(!m_pStkInfo)	return 1;
			short nTmpCurGp = g_pStockIO->GetIndex(m_pStkInfo->Code, m_pStkInfo->setcode);
			if(wParam == nTmpCurGp)
			{
				g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq);
				if(g_pStockIO->CanMakeOneTick(m_pStkInfo,m_Hq,m_bDSFlag,m_bZs,m_fLastPrice,m_lLastVolume))
				{
					struct TickData tmpTick;
					if(m_bDSFlag) 
					{
						BOOL bLast = FALSE;
						PEXCURRDATA pHqEx = (PEXCURRDATA)m_Hq.Reserved;
						if(pHqEx && pHqEx->HqTime>0)
							tmpTick.Minute = pHqEx->HqTime/60;
						else 
							tmpTick.Minute = GetNowWorkMinute(bLast, m_pTimer, m_pStkInfo->setcode);
					}
					else 
						tmpTick.Minute = GetCorrespondingMin(m_bDSFlag);
					tmpTick.VolInStock_dif = 0;
					tmpTick.Now = GetRelPrice(m_pStkInfo,m_Hq.Now,m_XsFlag);
					if(m_bZs)
					{
						tmpTick.NowVol=(long)(m_Hq.Amount/10000)-m_lLastVolume;
						m_lLastVolume=(long)(m_Hq.Amount/10000);
						m_lLastTickNum=0;
						m_fLastPrice=m_Hq.Now;
					}
					else
					{
						tmpTick.NowVol = m_Hq.Volume-m_lLastVolume;
						tmpTick.VolInStock_dif = m_Hq.CJBS-m_lLastTickNum;
						m_lLastVolume=m_Hq.Volume;
						m_lLastTickNum=m_Hq.CJBS;
						m_fLastPrice=m_Hq.Now;
					}
					if(m_bMinuteTick_Special)
					{
						if(m_Hq.Jjjz<COMPPREC && m_bSZKFSJJ)
							tmpTick.VolInStock_dif=m_Hq.Yield_VolInStock.Yield/100.0;
						else
							tmpTick.VolInStock_dif=m_Hq.Jjjz*1000+0.5;
					}
					if(m_pStkInfo->bigmarket==QH_TYPE && m_Hq.Reserved) 
					{
						PEXCURRDATA pHqEx = (PEXCURRDATA)m_Hq.Reserved;
						short nQHFlag = 10000*(short)m_Hq.Flag.InOutFlag+pHqEx->lSanRatio;
						memcpy(&tmpTick.InOutFlag, &nQHFlag, sizeof(short));
					}
					else if(m_pStkInfo->bigmarket==HKGP_TYPE)
					{
						tmpTick.HKFlag.HKTTFlag = m_Hq.HKTTFlag;
						tmpTick.HKFlag.HKInOutFlag = m_Hq.Flag.InOutFlag;
					}
					else if(m_bZs)	tmpTick.InOutFlag=2;
					else tmpTick.InOutFlag=m_Hq.Flag.InOutFlag;
					if(m_nAddedTickNum >= ADDED_TICKS)
					{
						struct TickData *tmpTickBuf = new TickData[m_nTickNum+ADDED_TICKS];
						memcpy(tmpTickBuf,m_pTick,m_nTickNum*sizeof(TickData));
						TDEL(m_pTick);
						m_pTick = tmpTickBuf;
					}
					BOOL bToLast = (m_nPos==m_nTickNum-1);
					m_pTick[m_nTickNum] = tmpTick;
					m_nTickNum++;
					m_nAddedTickNum++;
					if(!m_bSyncStatus)
					{
						m_nStart = m_nMaxItem  = 0;
						m_nDrawTick = m_nTickNum;
						if(bToLast) m_nPos = m_nTickNum-1;

						m_nZstAbsMin = -1;
						
						ProcessMsg(WM_PAINT);
					}
				}
			}
		}
		return 1;
	case WM_PAINT:
		{
			if(wParam == 1)
				ProcessMsg(UM_READDATA,lParam);

			CREATE_TMPDC
			if(!m_nInHisDlg && m_nTickNum>0 && m_nPos!=m_nTickNum-1)
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
						m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
						if(m_lDate==0)	//当日进一步数据
						{
							g_nAssisID = SetComAssisID(m_nWhichUnit,1);
							if(m_bDSFlag)
							{
								g_pGetData->DSGetTick(m_pStkInfo,m_nTickNum,WANT_TICKS);
							}
							else
							{
								g_pGetData->GetTickData(m_pStkInfo,m_nTickNum,WANT_TICKS,TRUE);
							}
						}
						else			//历史进一步数据
						{
							g_nAssisID = SetComAssisID(m_nWhichUnit,2);
							if(m_bDSFlag)
								g_pGetData->DSGetHisTick(m_pStkInfo, m_lDate, m_nTickNum, WANT_TICKS);
							else g_pGetData->GetHisTickData(m_pStkInfo,m_lDate,m_nTickNum,WANT_TICKS);
						}
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
	case UM_TOZSTIME:
		{
			if(m_nStyle==TICK_BIG) return 1;
			int i,AbsMins = (int)wParam,LastMin=0;
			m_nZstAbsMin = AbsMins;
			for(i=m_nDrawTick-1;i>=0;i--)
			{
				if(m_pTick[i].Minute <= AbsMins)
					break;
			}
			if(i>=0)
			{
				for(;i>=0&&m_pTick[i].Minute==AbsMins;i--);
				if(i<0||m_pTick[i].Minute!=AbsMins) ++i;
				m_nPos = min(m_nDrawTick-1,i+m_nMaxItem-1);
				ProcessMsg(WM_PAINT);
			}
			else if(OnLine && !m_bReqing && !m_bIsHead) //继续请求
			{
				m_bReqing = TRUE;
				m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
				if(m_lDate==0)	//当日进一步数据
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,1);
					if(m_bDSFlag)
					{
						g_pGetData->DSGetTick(m_pStkInfo,m_nTickNum,WANT_TICKS);
					}
					else
					{
						g_pGetData->GetTickData(m_pStkInfo,m_nTickNum,WANT_TICKS,TRUE);
					}
				}
				else			//历史进一步数据
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,2);
					if(m_bDSFlag)
						g_pGetData->DSGetHisTick(m_pStkInfo, m_lDate, m_nTickNum,WANT_TICKS);
					else g_pGetData->GetHisTickData(m_pStkInfo,m_lDate,m_nTickNum,WANT_TICKS);
				}
			}
		}
		return 1;
	case UM_SET_PLAY:
		m_bSyncStatus = (wParam==1)?TRUE:FALSE;
		if(!m_bSyncStatus)
		{
			m_nDrawTick = m_nTickNum;
			m_nPos = m_nTickNum-1;
			ProcessMsg(WM_PAINT);
		}
		return 1;
	case UM_GET_PLAY:
		return m_bSyncStatus?1:0;
	case UM_ZSTSYNCH:
		{
			if(!m_bSyncStatus) //如果不是同步状态，则启动它
				ProcessMsg(UM_SET_PLAY,1);

			m_nSyncMin = (short)wParam;
			if(m_nTickNum <= 0) return 1;
			
			//重新计算得到m_nDrawTick
			int LastMin=0;
			for(int i=0;i<m_nTickNum;i++) //此处与ToZstTime有所改变
			{
				if(m_pTick[i].Minute > m_nSyncMin && LastMin <= m_nSyncMin)
					break;
				LastMin = m_pTick[i].Minute;
			}
			if(i < m_nTickNum) m_nDrawTick=max(1,i);
			else m_nDrawTick=max(1,m_nTickNum);
			m_nPos = m_nDrawTick-1;
			ProcessMsg(WM_PAINT);
		}
		return 1;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

void UTick::ReadData(BOOL bDataReady,BOOL bReqData)
{
	memset(&m_Hq,0,sizeof(CurrStockData));
	m_nStart = m_nMaxItem = 0;
	m_nAddedTickNum = 0;
	m_nTickNum = 0;
	TDEL(m_pTick);

	if(m_lDate == 0)	//当天Tick
	{
		g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq);
		if(!bDataReady)
		{
			if(!OnLine) //试读缓存
			{
				m_pTick = new TickData[WANT_TICKS+ADDED_TICKS];
				memset(m_pTick,0,sizeof(TickData)*(WANT_TICKS+ADDED_TICKS));
				int nCache = GetCahceTickData(m_pStkInfo,m_pTick);
				if(nCache > 0)
				{
					m_nTickNum = nCache;
					bDataReady = TRUE;
				}
			}
			if(OnLine && bReqData)
			{
				m_pTick = new TickData[WANT_TICKS+ADDED_TICKS];
				m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
				if(m_bDSFlag)	
				{
					if(m_nInCom2)	//在定制版面中的TICK画面，需要先取行情
					{
						g_nAssisID = JUSTCURRHQ;
						g_nPriority = 2;
						char CodeHead3[100]={0};
						memcpy(CodeHead3,&m_pStkInfo->setcode,1);
						memcpy(CodeHead3+1,m_pStkInfo->Code,CODE_SIZE);
						g_pGetData->DSGetMulCurrData((PMULIDX)CodeHead3,1);
						g_nPriority = 1;
					}
					//
					g_nAssisID = SetComAssisID(m_nWhichUnit,0);
					g_pGetData->DSGetTick(m_pStkInfo,0,WANT_TICKS);
				}
				else	
				{
					if(m_nInCom2)	//在定制版面中的TICK画面，需要先取行情
					{
						g_nAssisID = JUSTCURRHQ;
						g_nPriority = 2;
						char CodeHead[200]={0};
						memcpy(CodeHead,&m_pStkInfo->setcode,1);
						memcpy(CodeHead+1,m_pStkInfo->Code,SH_CODE_LEN);
						g_pGetData->GetMultiCurrStockData(CodeHead,1);
						g_nPriority = 1;
					}
					//
					g_nAssisID = SetComAssisID(m_nWhichUnit,0);
					int wantnum=WANT_TICKS;
					if(m_nStyle!=TICK_BIG)
					{
						if(g_GC.bIsSHLevel2 || g_GC.bIsSZLevel2)
							wantnum=WANT_TICKS/6;
						else
							wantnum=WANT_TICKS/9;
						if(ScreenCy<650 && !m_nInCom2) 
							wantnum=WANT_TICKS/12;
						if(ScreenCy>800) 
							wantnum=WANT_TICKS/6+20;
					}
					g_pGetData->GetTickData(m_pStkInfo,0,wantnum,TRUE);
				}
			}
		}
		else
		{
			m_pTick = new TickData[WANT_TICKS+ADDED_TICKS];
			if(m_bDSFlag)	m_nTickNum = g_pGetData->DSGetTickACK(m_pTick);
			else	m_nTickNum = g_pGetData->GetTickDataACK(m_pStkInfo,m_pTick,WANT_TICKS,TRUE);//第一次请求
		}
		if (m_bZs)
		{
			m_lLastVolume=(long)(m_Hq.Amount/10000);	//变成万元
			m_lLastTickNum=0;
			m_fLastPrice=m_Hq.Now;
		}
		else 
		{
			m_lLastVolume=m_Hq.Volume;
			m_lLastTickNum=m_Hq.CJBS;
			m_fLastPrice=m_Hq.Now;
		}
	}
	else				//历史某天Tick
	{
		if(!bDataReady)
		{
			if(bReqData)
			{
				m_pTick = new TickData[WANT_TICKS+ADDED_TICKS];
				m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
				g_nAssisID = SetComAssisID(m_nWhichUnit,0);
				if(m_bDSFlag)
					g_pGetData->DSGetHisTick(m_pStkInfo, m_lDate, 0, WANT_TICKS);
				else g_pGetData->GetHisTickData(m_pStkInfo,m_lDate,0,WANT_TICKS);
			}
		}
		else
		{
			m_pTick = new TickData[WANT_TICKS+ADDED_TICKS];
			if(m_bDSFlag)
				m_nTickNum = g_pGetData->DSGetHisTickACK(m_pTick);
			else m_nTickNum = g_pGetData->GetHisTickDataACK(m_pStkInfo,m_pTick,WANT_TICKS,&m_Hq.Close);
		}
	}
	//去掉异常数据
	for(int i=0;i<m_nTickNum;i++)
	{
		if(m_pTick[i].Minute<=0 || m_pTick[i].Now<=0 || m_pTick[i].NowVol<0)
		{
			memmove(m_pTick+i,m_pTick+i+1,(m_nTickNum-i-1)*sizeof(struct TickData));
			m_nTickNum--;
		}
	}
	m_nPos = m_nTickNum-1;
	m_nDrawTick = m_nTickNum;
}

char * GetQHTickDescription(struct TickData *pTick)
{
	short	nQHFlag = 0;
	memcpy(&nQHFlag, &pTick->InOutFlag, sizeof(short));
	short	nInOutFlag = nQHFlag/10000;
	short	nOpenCloseRatio = nQHFlag%10000;
	long	nCloseVol = pTick->NowVol*nOpenCloseRatio/1000;
	long	nOpenVol = pTick->NowVol - nCloseVol;
	long	zc = nOpenVol-nCloseVol;			
	char *	ocStr=" ";
	if(pTick->NowVol==zc) ocStr = _F("双开");
	else if(pTick->NowVol+zc==0) ocStr = _F("双平");
	else if(zc==0&&nInOutFlag==0) ocStr = _F("多换");
	else if(zc==0&&nInOutFlag==1) ocStr = _F("空换");
	else if(zc>0&&nInOutFlag==0) ocStr = _F("多开");
	else if(zc>0&&nInOutFlag==1) ocStr = _F("空开");
	else if(zc<0&&nInOutFlag==0) ocStr = _F("空平");
	else if(zc<0&&nInOutFlag==1) ocStr = _F("多平");
	return ocStr;
}

void UTick::DrawIt(CDC *pDC)
{
	if(Height() < 20) return;	//如果空间太小，不画

	CSize tmpSize;
	int nFixedWidth = g_nFixedWidth;
	if(m_nStyle==TICK_SMALL)
		nFixedWidth = m_rcIn.right;
	int TextH=HQ_TEXTH-1;
	int YFrom=1;
	int XFrom1=HQ_XSPACE;
	int XFrom2=2*nFixedWidth/9+4;
	int XFrom3=5*nFixedWidth/9-HQ_XSPACE;
	int XFrom4=8*nFixedWidth/9-HQ_XSPACE;
	int XFrom5=0;
	int i;
	int nPriceWidth=3*nFixedWidth/9;
	int nVolWidth=3*nFixedWidth/9+5;
	int nOtherWidth=nFixedWidth/9;
	int nStrWidth=0;
	int TickNumSpace1=0,TickNumSpace2=0;
	if(m_pStkInfo->bigmarket==QH_TYPE)
	{
		XFrom2=2*nFixedWidth/11+12;
		XFrom3=4*nFixedWidth/11+4;
		XFrom4=6*nFixedWidth/11+3;
		XFrom5=8*nFixedWidth/11;
		nPriceWidth=2*nFixedWidth/11+14;
		nVolWidth=3*nFixedWidth/11;
		nOtherWidth=3*nFixedWidth/11;
		nStrWidth = 3*nFixedWidth/11;
	}
	if(m_nStyle == TICK_SMALL)
	{
		if(m_nDrawTick <= 0) return;
		int rownum = (m_rcIn.Height()-1)/TextH;		
		if(m_pStkInfo->bigmarket==QH_TYPE) 
		{
			CFont *tmpfont = g_d.SelectFont(pDC,SMALL_FONT);
			g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,0,_F(_Time));
			g_d.DisplayText(pDC,XFrom2-12,YFrom,VipColor.TXTCOLOR,nPriceWidth,_F(_Price));
			g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,nVolWidth,_F(_Cur_Volume));
			g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,nOtherWidth,_F(_OCVol));
			g_d.DisplayText(pDC,XFrom5+1,YFrom,VipColor.TXTCOLOR,nStrWidth,_F(_OCString));
			YFrom+=18;
			rownum = (m_rcIn.Height()-YFrom-1)/TextH;
			g_d.RestoreFont(pDC,tmpfont);
		}

		m_nMaxItem = rownum;
		m_nStart = max(0,m_nPos-rownum+1);
		for(i = m_nStart; i <= m_nPos;i++)
		{
			if(m_pTick[i].Minute<=0 || m_pTick[i].Now<=0 || m_pTick[i].NowVol<0) //如有异常数据,不画
				continue;
			if(i&&m_pTick[i].Minute<m_pTick[i-1].Minute)
				m_pTick[i].Minute = m_pTick[i-1].Minute;
			if(i&&m_pTick[i].Minute != m_pTick[i-1].Minute)
				g_d.DotLine(pDC, m_rcIn.left, YFrom, m_rcIn.right, YFrom, GridColor.GridLineColor);
			TickNumSpace1=TickNumSpace2=0;
			if(!m_bDSFlag && !m_nInCom2 && m_lDate==0 && !m_bZs && !m_bMinuteTick_Special
				&& (m_pTick[i].VolInStock_dif>0||m_Domain==SZ||CanLevel2()))
			{
				TickNumSpace1=14;
				TickNumSpace2=10;
			}
			if(m_Domain==SZ && strncmp(m_pStkInfo->Code,"150",3)==0)
			{
				TickNumSpace1=TickNumSpace2=0;
			}
			g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,0,"%02d:%02d",(m_pTick[i].Minute/60)%24,m_pTick[i].Minute%60);
			if(m_bMinuteTick_Special && m_pTick[i].VolInStock_dif>0)
			{
				tmpSize=g_d.BjPrintf(pDC,XFrom2+4,YFrom,GetAbsPrice(m_pStkInfo,m_pTick[i].Now,m_XsFlag),m_Hq.Close,m_XsFlag,0);
				if(m_pTick[i].Now>m_pTick[i].VolInStock_dif)
					g_d.DisplayText(pDC,XFrom2+4+tmpSize.cx+1,YFrom,VipColor.KPCOLOR,0,"/%03d +",m_pTick[i].VolInStock_dif%1000);
				else if(m_pTick[i].Now<m_pTick[i].VolInStock_dif)
					g_d.DisplayText(pDC,XFrom2+4+tmpSize.cx+1,YFrom,VipColor.DOWNCOLOR,0,"/%03d -",m_pTick[i].VolInStock_dif%1000);
				else
					g_d.DisplayText(pDC,XFrom2+4+tmpSize.cx+1,YFrom,VipColor.LEVELCOLOR,0,"/%03d",m_pTick[i].VolInStock_dif%1000);
			}
			else
				g_d.BjPrintf(pDC,XFrom2-TickNumSpace1,YFrom,GetAbsPrice(m_pStkInfo,m_pTick[i].Now,m_XsFlag),m_Hq.Close,m_XsFlag,nPriceWidth);
			if(!m_bZs)
			{
				if(g_GH.bShowBS || m_pStkInfo->bigmarket==QH_TYPE)
				{
					if(m_bHKZs)
						g_d.DisplayText(pDC,XFrom3-TickNumSpace1-TickNumSpace2,YFrom,VipColor.VOLCOLOR,nVolWidth,"%s",MakeJE3(m_pTick[i].NowVol*100.0f));
					else if(m_pStkInfo->bigmarket==HKGP_TYPE)
					{
						g_d.DisplayText(pDC,CRect(XFrom3+10,YFrom,XFrom3+nVolWidth,YFrom+TextH),VipColor.TXTCOLOR,DT_LEFT,"%c",m_pTick[i].HKFlag.HKTTFlag);
						g_d.DisplayText(pDC,XFrom3-TickNumSpace1,YFrom,IsHligtNowVol(m_pTick[i].NowVol,m_pStkInfo)?RGB(192,0,192):VipColor.VOLCOLOR,nVolWidth,"%s",MakeVol3(m_pTick[i].NowVol));
					}
					else 
						g_d.DisplayText(pDC,XFrom3-TickNumSpace1-TickNumSpace2,YFrom,IsHligtNowVol(m_pTick[i].NowVol,m_pStkInfo)?RGB(192,0,192):VipColor.VOLCOLOR,nVolWidth,"%s",MakeVol3(m_pTick[i].NowVol));
				}
				else
				{
					COLORREF txtcor=VipColor.VOLCOLOR;
					if(m_pTick[i].InOutFlag==0 && m_pStkInfo->bigmarket!=QH_TYPE)
						txtcor=VipColor.KPCOLOR;
					else if(m_pTick[i].InOutFlag==1 && m_pStkInfo->bigmarket!=QH_TYPE)
						txtcor=VipColor.DOWNCOLOR;
					if(m_bHKZs)
						g_d.DisplayText(pDC,XFrom3-TickNumSpace1-TickNumSpace2+nOtherWidth/2,YFrom,txtcor,nVolWidth,"%s",MakeJE3(m_pTick[i].NowVol*100.0f));
					else if(m_pStkInfo->bigmarket==HKGP_TYPE)
					{
						g_d.DisplayText(pDC,CRect(XFrom3+nOtherWidth/2+10,YFrom,XFrom3+nOtherWidth/2+nVolWidth,YFrom+TextH),VipColor.TXTCOLOR,DT_LEFT,"%c",m_pTick[i].HKFlag.HKTTFlag);
						g_d.DisplayText(pDC,XFrom3-TickNumSpace1-TickNumSpace2+nOtherWidth/2,YFrom,txtcor,nVolWidth,"%s",MakeVol3(m_pTick[i].NowVol));
					}
					else 
						g_d.DisplayText(pDC,XFrom3-TickNumSpace1-TickNumSpace2+nOtherWidth/2,YFrom,txtcor,nVolWidth,"%s",MakeVol3(m_pTick[i].NowVol));
				}
			}
			else
				g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.VOLCOLOR,4*nFixedWidth/9,MakeJE3(m_pTick[i].NowVol*10000.0));
			if(g_GH.bShowBS && m_pTick[i].NowVol>0)
			{
				if(((m_pStkInfo->bigmarket==HKGP_TYPE&&m_pTick[i].HKFlag.HKInOutFlag==0)||(m_pStkInfo->bigmarket!=HKGP_TYPE&&m_pTick[i].InOutFlag==0)) 
					&& !m_bZs && m_pStkInfo->bigmarket!=QH_TYPE)
					g_d.DisplayText(pDC,XFrom4-TickNumSpace1-TickNumSpace2,YFrom,VipColor.KPCOLOR,nOtherWidth,"B");
				else if(((m_pStkInfo->bigmarket==HKGP_TYPE&&m_pTick[i].HKFlag.HKInOutFlag==1)||(m_pStkInfo->bigmarket!=HKGP_TYPE&&m_pTick[i].InOutFlag==1)) 
					&& !m_bZs && m_pStkInfo->bigmarket!=QH_TYPE)
					g_d.DisplayText(pDC,XFrom4-TickNumSpace1-TickNumSpace2,YFrom,VipColor.DOWNCOLOR,nOtherWidth,"S");
			}
			if(TickNumSpace1>0)
			{
				if(m_pTick[i].VolInStock_dif>999)
					g_d.DisplayText(pDC,XFrom4,YFrom,RGB(128,128,128),nOtherWidth+2,MEANLESS_STR);
				else if(m_pTick[i].VolInStock_dif>=0)
					g_d.DisplayText(pDC,XFrom4,YFrom,RGB(128,128,128),nOtherWidth+2,"%2d",m_pTick[i].VolInStock_dif);
			}
			if(m_pStkInfo->bigmarket==QH_TYPE)
			{
				short	nQHFlag = 0;
				memcpy(&nQHFlag, &m_pTick[i].InOutFlag, sizeof(short));
				short	nInOutFlag = nQHFlag/10000;
				short	nOpenCloseRatio = nQHFlag%10000;
				long	nCloseVol = m_pTick[i].NowVol*nOpenCloseRatio/1000;
				long	nOpenVol = m_pTick[i].NowVol - nCloseVol;
				g_d.DisplayText(pDC,XFrom4-5,YFrom,
					nInOutFlag==0?VipColor.KPCOLOR:(nInOutFlag==1?VipColor.DOWNCOLOR:VipColor.LEVELCOLOR),
					nOtherWidth,"%d",nOpenVol-nCloseVol);

				char *ocStr=GetQHTickDescription(m_pTick+i);
				CFont *tmpfont = g_d.SelectFont(pDC,SMALL_FONT);
				g_d.DisplayText(pDC,XFrom5,YFrom,
					nInOutFlag==0?VipColor.KPCOLOR:(nInOutFlag==1?VipColor.DOWNCOLOR:VipColor.LEVELCOLOR),
					nStrWidth,"%s",ocStr);
				g_d.RestoreFont(pDC,tmpfont);
			}
			YFrom+=TextH;
		}
	}
	else
	{
		int colnum,rownum,totalnum,drawnum,drawcol;
		YFrom=3;
		CSize size=g_d.DisplayText(pDC,18,YFrom,VipColor.BIGTITLECOLOR,_F("%6s %8s 分时成交明细"),m_pStkInfo->Code,m_pStkInfo->Name);
		g_d.DisplayText(pDC,size.cx+42,YFrom,GridColor.Grid_Level_Color,_F("  Up/PageUp:上翻 Down/PageDown:下翻"));
		int Col_Width=nFixedWidth+9;
		colnum = max(1,m_rcIn.Width()/Col_Width);
		Col_Width = (m_rcIn.Width()-1)/colnum;
		YFrom+=23;
		g_d.DrawLine(pDC,0,YFrom,colnum*Col_Width,YFrom);
		g_d.DrawLine(pDC,colnum*Col_Width,0,colnum*Col_Width,m_rcIn.bottom);
		YFrom+=2;
		for(i = 0;i < colnum;i++)
		{
			g_d.DisplayText(pDC,Col_Width*i+XFrom1,YFrom,VipColor.TXTCOLOR,0,_F("时间"));
			g_d.DisplayText(pDC,Col_Width*i+XFrom2-8,YFrom,VipColor.TXTCOLOR,nPriceWidth,_F("价格"));
			g_d.DisplayText(pDC,Col_Width*i+XFrom3-3,YFrom,VipColor.TXTCOLOR,nVolWidth,_F("现量"));
			if(m_pStkInfo->bigmarket==QH_TYPE) 
			{
				g_d.DisplayText(pDC,Col_Width*i+XFrom4,YFrom,VipColor.TXTCOLOR,nOtherWidth,_F(_OCVol));
				g_d.DisplayText(pDC,Col_Width*i+XFrom5+4,YFrom,VipColor.TXTCOLOR,nStrWidth,_F(_OCString));
			}
			g_d.DrawLine(pDC,Col_Width*(i+1),YFrom-2,Col_Width*(i+1),m_rcIn.bottom);
		}
		YFrom+=20;
		g_d.DrawLine(pDC,0,YFrom,colnum*Col_Width,YFrom);
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
			if(m_pTick[i].Minute<=0 || m_pTick[i].Now<=0 || m_pTick[i].NowVol<0)  //如有异常数据,不画
				continue;
			if(i&&m_pTick[i].Minute<m_pTick[i-1].Minute)
				m_pTick[i].Minute = m_pTick[i-1].Minute;
			TickNumSpace1=TickNumSpace2=0;
			if(!m_bDSFlag && !m_nInCom2 && m_lDate==0 && !m_bZs && !m_bMinuteTick_Special
				&& (m_pTick[i].VolInStock_dif>0||m_Domain==SZ||CanLevel2()))
			{
				TickNumSpace1=14;
				TickNumSpace2=8;
			}
			g_d.DisplayText(pDC,Col_Width*drawcol+XFrom1,YFrom,VipColor.TXTCOLOR,0,"%02d:%02d",(m_pTick[i].Minute/60)%24,m_pTick[i].Minute%60);
			if(m_bMinuteTick_Special && m_pTick[i].VolInStock_dif>0)
			{
				tmpSize=g_d.BjPrintf(pDC,Col_Width*drawcol+XFrom2+4,YFrom,GetAbsPrice(m_pStkInfo,m_pTick[i].Now,m_XsFlag),m_Hq.Close,m_XsFlag,0);
				if(m_pTick[i].Now>m_pTick[i].VolInStock_dif)
					g_d.DisplayText(pDC,Col_Width*drawcol+XFrom2+4+tmpSize.cx+1,YFrom,VipColor.KPCOLOR,0,"/%03d +",m_pTick[i].VolInStock_dif%1000);
				else if(m_pTick[i].Now<m_pTick[i].VolInStock_dif)
					g_d.DisplayText(pDC,Col_Width*drawcol+XFrom2+4+tmpSize.cx+1,YFrom,VipColor.DOWNCOLOR,0,"/%03d -",m_pTick[i].VolInStock_dif%1000);
				else
					g_d.DisplayText(pDC,Col_Width*drawcol+XFrom2+4+tmpSize.cx+1,YFrom,VipColor.LEVELCOLOR,0,"/%03d",m_pTick[i].VolInStock_dif%1000);
			}
			else
				g_d.BjPrintf(pDC,Col_Width*drawcol+XFrom2-TickNumSpace1,YFrom,GetAbsPrice(m_pStkInfo,m_pTick[i].Now,m_XsFlag),m_Hq.Close,m_XsFlag,nPriceWidth);
			
			if(!m_bZs)
			{
				if(g_GH.bShowBS || m_pStkInfo->bigmarket==QH_TYPE)
				{
					if(m_bHKZs)
						g_d.DisplayText(pDC,Col_Width*drawcol+XFrom3-TickNumSpace1-TickNumSpace2,YFrom,VipColor.VOLCOLOR,nVolWidth,MakeJE3(m_pTick[i].NowVol*100.0));
					else if(m_pStkInfo->bigmarket==HKGP_TYPE)
					{
						g_d.DisplayText(pDC,CRect(Col_Width*drawcol+XFrom3+10,YFrom,Col_Width*drawcol+XFrom3+nVolWidth,YFrom+TextH),VipColor.TXTCOLOR,DT_LEFT,"%c",m_pTick[i].HKFlag.HKTTFlag,m_pTick[i].NowVol);
						g_d.DisplayText(pDC,Col_Width*drawcol+XFrom3-TickNumSpace1-TickNumSpace2,YFrom,IsHligtNowVol(m_pTick[i].NowVol,m_pStkInfo)?RGB(192,0,192):VipColor.VOLCOLOR,nVolWidth,"%s",MakeVol3(m_pTick[i].NowVol));
					}
					else 
						g_d.DisplayText(pDC,Col_Width*drawcol+XFrom3-TickNumSpace1-TickNumSpace2,YFrom,IsHligtNowVol(m_pTick[i].NowVol,m_pStkInfo)?RGB(192,0,192):VipColor.VOLCOLOR,nVolWidth,"%s",MakeVol3(m_pTick[i].NowVol));
				}
				else
				{
					COLORREF txtcor=VipColor.VOLCOLOR;
					if(m_pTick[i].InOutFlag==0 && m_pStkInfo->bigmarket!=QH_TYPE)
						txtcor=VipColor.KPCOLOR;
					else if(m_pTick[i].InOutFlag==1 && m_pStkInfo->bigmarket!=QH_TYPE)
						txtcor=VipColor.DOWNCOLOR;
					if(m_bHKZs)
						g_d.DisplayText(pDC,Col_Width*drawcol+XFrom3-TickNumSpace1-TickNumSpace2+nOtherWidth/2,YFrom,txtcor,nVolWidth,MakeJE3(m_pTick[i].NowVol*100.0));
					else if(m_pStkInfo->bigmarket==HKGP_TYPE)
					{
						g_d.DisplayText(pDC,CRect(Col_Width*drawcol+XFrom3+nOtherWidth/2+10,YFrom,Col_Width*drawcol+XFrom3+nOtherWidth/2+nVolWidth,YFrom+TextH),VipColor.TXTCOLOR,DT_LEFT,"%c",m_pTick[i].HKFlag.HKTTFlag);
						g_d.DisplayText(pDC,Col_Width*drawcol+XFrom3-TickNumSpace1-TickNumSpace2+nOtherWidth/2,YFrom,txtcor,nVolWidth,"%s",MakeVol3(m_pTick[i].NowVol));
					}
					else
						g_d.DisplayText(pDC,Col_Width*drawcol+XFrom3-TickNumSpace1-TickNumSpace2+nOtherWidth/2,YFrom,txtcor,nVolWidth,"%s",MakeVol3(m_pTick[i].NowVol));
				}
			}
			else
				g_d.DisplayText(pDC,Col_Width*drawcol+XFrom3,YFrom,VipColor.VOLCOLOR,nVolWidth,MakeJE3(m_pTick[i].NowVol*10000.0));
			if(g_GH.bShowBS && m_pTick[i].NowVol>0)
			{
				if(((m_pStkInfo->bigmarket==HKGP_TYPE&&m_pTick[i].HKFlag.HKInOutFlag==0)||(m_pStkInfo->bigmarket!=HKGP_TYPE&&m_pTick[i].InOutFlag==0)) 
					&& !m_bZs && m_pStkInfo->bigmarket!=QH_TYPE)
					g_d.DisplayText(pDC,Col_Width*drawcol+XFrom4-TickNumSpace1-TickNumSpace2,YFrom,VipColor.KPCOLOR,nOtherWidth,"B");
				else if(((m_pStkInfo->bigmarket==HKGP_TYPE&&m_pTick[i].HKFlag.HKInOutFlag==1)||(m_pStkInfo->bigmarket!=HKGP_TYPE&&m_pTick[i].InOutFlag==1)) 
					&& !m_bZs && m_pStkInfo->bigmarket!=QH_TYPE)
					g_d.DisplayText(pDC,Col_Width*drawcol+XFrom4-TickNumSpace1-TickNumSpace2,YFrom,VipColor.DOWNCOLOR,nOtherWidth,"S");
			}
			if(TickNumSpace1>0)
			{
				if(m_pTick[i].VolInStock_dif>999)
					g_d.DisplayText(pDC,Col_Width*drawcol+XFrom4+10,YFrom,RGB(128,128,128),nOtherWidth,MEANLESS_STR);
				else if(m_pTick[i].VolInStock_dif>=0)
					g_d.DisplayText(pDC,Col_Width*drawcol+XFrom4+10,YFrom,RGB(128,128,128),nOtherWidth,"%2d",m_pTick[i].VolInStock_dif);
			}
			if(m_pStkInfo->bigmarket==QH_TYPE)
			{
				short	nQHFlag = 0;
				memcpy(&nQHFlag, &m_pTick[i].InOutFlag, sizeof(short));
				short	nInOutFlag = nQHFlag/10000;
				short	nOpenCloseRatio = nQHFlag%10000;
				long	nCloseVol = m_pTick[i].NowVol*nOpenCloseRatio/1000;
				long	nOpenVol = m_pTick[i].NowVol - nCloseVol;
				g_d.DisplayText(pDC,Col_Width*drawcol+XFrom4,YFrom,
					nInOutFlag==0?VipColor.KPCOLOR:(nInOutFlag==1?VipColor.DOWNCOLOR:VipColor.LEVELCOLOR),
					nOtherWidth,"%d",nOpenVol-nCloseVol);

				char *ocStr=GetQHTickDescription(m_pTick+i);
				CFont *tmpfont = g_d.SelectFont(pDC,SMALL_FONT);
				g_d.DisplayText(pDC,Col_Width*drawcol+XFrom5+5,YFrom,
					nInOutFlag==0?VipColor.KPCOLOR:(nInOutFlag==1?VipColor.DOWNCOLOR:VipColor.LEVELCOLOR),
					nStrWidth,"%s",ocStr);
				g_d.RestoreFont(pDC,tmpfont);
			}
			drawnum++;
			if(drawnum >= rownum*(drawcol+1))
			{
				drawcol++;
				YFrom=oldYFrom;
			}
			else	
				YFrom+=TextH;
		}
	}
}

BOOL UTick::SSMRegTickData()
{
	if(m_ToDo.nDoType==0) return FALSE;
	for(int i=m_nDrawTick-1;i>=0;i--)
	{
		if(m_pTick[i].Minute>6*60&&(m_pTick[i].Minute <= m_ToDo.nAbsMin))
			break;
	}
	if(i>=0||m_bIsHead)			//数据到位!do something
	{
		DoSomethingUseData();
		return TRUE;
	}
	else if(OnLine && !m_bReqing && !m_bIsHead) //继续请求
	{
		m_bReqing = TRUE;
		m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
		if(m_lDate==0)	//当日进一步数据
		{
			g_nAssisID = SetComAssisID(m_nWhichUnit,1);
			if(m_bDSFlag)
			{
				g_pGetData->DSGetTick(m_pStkInfo,m_nTickNum,WANT_TICKS);
			}
			else
			{
				g_pGetData->GetTickData(m_pStkInfo,m_nTickNum,WANT_TICKS,TRUE);
			}
		}
		else			//历史进一步数据
		{
			g_nAssisID = SetComAssisID(m_nWhichUnit,2);
			if(m_bDSFlag)
				g_pGetData->DSGetHisTick(m_pStkInfo, m_lDate, m_nTickNum,WANT_TICKS);
			else g_pGetData->GetHisTickData(m_pStkInfo,m_lDate,m_nTickNum,WANT_TICKS);
		}
	}
	return FALSE;
}

void UTick::WriteToTxtFile(CString filename,BOOL bWriteExcel, int nAbsMin)
{
	//请求全部
	m_ToDo.nDoType = 1;
	m_ToDo.nAbsMin = nAbsMin;
	strncpy(m_ToDo.filename, (char*)(LPCSTR)filename, MAX_PATH-1);
	m_ToDo.bWriteExcel = bWriteExcel;
	SSMRegTickData();
}

void UTick::DoSomethingUseData()
{
	if(m_ToDo.nDoType==1)			//写文件
	{
		//开始写咯
		CFileException ex;
		CStdioFile  File;
		if ( File.Open(m_ToDo.filename,CFile::modeCreate|CFile::modeWrite,&ex) )
		{
			char  Buffer[1024]={0};
			long tmpDate=g_lOpenRq;
			if(m_lDate!=0)	
				tmpDate=m_lDate;
			sprintf(Buffer,"                  %d %s (%s)\n",tmpDate,m_pStkInfo->Name,m_pStkInfo->Code);
			File.Write(Buffer,strlen(Buffer));
			File.Write("\r\n",2);
			if(m_bDSFlag)
				sprintf(Buffer,"%5s\t%8s\t%8s\t%s\t","时间","价格","现量","增仓");
			else
				sprintf(Buffer,"%5s\t%8s\t%8s\t%s\t","时间","价格","现量","笔数");

			File.Write(_F(Buffer),strlen(Buffer));
			File.Write("\r\n",2);

			for(int i=0;i<m_nTickNum;i++)
			{
				strcpy(Buffer,"          \t");
				
				sprintf(Buffer,"%02d:%02d\t%8.3f\t%8u\t%s\t",
					(m_pTick[i].Minute/60)%24,m_pTick[i].Minute%60,
					GetAbsPrice(m_pStkInfo,m_pTick[i].Now,m_XsFlag),
					m_pTick[i].NowVol,IntToStr(m_pTick[i].VolInStock_dif));
				File.Write(Buffer,strlen(Buffer));
				if(m_pTick[i].InOutFlag==0 && !m_bZs && m_pStkInfo->bigmarket!=QH_TYPE)
					File.Write("B\t", 2);
				else if(m_pTick[i].InOutFlag==1 && !m_bZs && m_pStkInfo->bigmarket!=QH_TYPE)
					File.Write("S\t", 2);
				//对于期货,需写入描述
				if(m_pStkInfo->bigmarket==QH_TYPE)
				{
					char *ocStr=GetQHTickDescription(m_pTick+i);
					sprintf(Buffer,"%s\t",ocStr);
					File.Write(_F(Buffer),strlen(Buffer));
				}
				File.Write("\n",1);
			}
			File.Close();			
		}
		else
		{
			char strError[1024]={0};
			ex.GetErrorMessage(strError,1024);
			TDX_MessageBox(m_pView->m_hWnd,"文件创建错误!\n"+(CString)strError,MB_ICONEXCLAMATION|MB_OK);
		}
	}
	memset(&m_ToDo, 0, sizeof(HasDataToDo));
}
