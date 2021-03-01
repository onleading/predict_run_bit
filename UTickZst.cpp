#include "stdafx.h"
#include "UTickZst.h"
#include "ComView.h"
#include "SelectGPDlg.h"
#include "ZstTickDlg.h"
#include "MainFrm.h"

#define		DEF_SHOWNUM		9999999	

float TickZstRatio[]={6.0f/9,3.0f/9,4.0f/8,2.0f/8,2.0f/8};
UTickZst::UTickZst(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_bSelected = FALSE;
	m_bDrawCross = FALSE;
	m_nCurXTickPos = 0;
	fBasePrice = 0.0;

	nSecLens = 0;

	m_bInSwim = FALSE;
	m_bHasCross = FALSE;
	m_bCrossMode = FALSE;
	m_CrossPoint = CPoint(0,0);
	m_RealMove = CPoint(0,0);

	m_lLastVolume = 0;
	m_lLastTickNum = 0;
	m_fLastPrice = 0.0;
	m_fLastZs = 0.0;
	m_nAddedTickNum = 0;

	m_bDataReady = FALSE;
	m_ReqingFlag = -1;
	m_bIsHead = FALSE;
	memset(m_bManyDayIsHead, 0, MAX_ZST_MANYDAY*sizeof(BOOL));

	m_bManyDayZST = FALSE;
	m_bYestodayFalg = g_bYesterdayFlag;
	
	m_pXTick = m_pTotolXTick = NULL; 
	m_nXMaxTickNum = m_nXTickNum = 0;
	m_pTick = NULL; m_nTickNum = 0;
	m_nFirstShow = 0; m_nShowNum = DEF_SHOWNUM; m_bShowStepSync = TRUE;
	for(int i=0;i<MAX_ZST_MANYDAY;i++) 
	{
		m_pManyDayTickData[i]=NULL;
		m_ManyDayTickNum[i] = 0;
	}
	memset(m_ManyDayDate,0,MAX_ZST_MANYDAY*sizeof(long));
	m_nDays = 0;

	m_bBeginRectangle = FALSE;

	fPointPerXTick = 0.0;
	memset(m_CriticalTime, -1, 9*sizeof(long));
}


UTickZst::~UTickZst()
{
	TDEL(m_pXTick); TDEL(m_pTotolXTick);
	TDEL(m_pTick);
	m_nXMaxTickNum = m_nXTickNum = m_nTotalXTick = 0;
	m_nTickNum = 0;
	for(int i=0;i<MAX_ZST_MANYDAY;i++)
	{
		TDEL(m_pManyDayTickData[i]);
		m_ManyDayTickNum[i] = 0;
	}
	memset(m_ManyDayDate,0,MAX_ZST_MANYDAY*sizeof(long));
}

#include "ZstTickDlg.h"
int  UTickZst::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
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
	case UM_READDATA:					//全部重新读
		m_nFirstShow = 0;
		m_nShowNum = 9999999;
		ReadData(FALSE,wParam==0);

		return 1;
	case UM_GETDATA_ACK://对于远程版,请求和返回分开
		switch(wParam)
		{
		case 0:
			m_ReqingFlag = -1;
			m_bDataReady = TRUE;
			ReadData(TRUE);
			return 1;
		case 1:	//今天的进一步数据
		case 2: //多日的进一步数据
			{
				short lastRegFlag = m_ReqingFlag;
				m_ReqingFlag = -1;
				m_bDataReady = TRUE;
				int nLastTickNum = 0;
				if(lastRegFlag==0) nLastTickNum = m_nTickNum;
				else if(lastRegFlag>0&&lastRegFlag<=MAX_ZST_MANYDAY)		//多日
					nLastTickNum = m_ManyDayTickNum[lastRegFlag-1];
				else 
					break;
				TickData *tmpTickBuf = new TickData[WANT_ZSTTICKS+nLastTickNum+ADDED_ZSTTICKS];
				short tmpTickNum=0;
				if(wParam==1)			//当日
				{
					if(m_bDSFlag)
						tmpTickNum = g_pGetData->DSGetTickACK(tmpTickBuf);
					else
						tmpTickNum = g_pGetData->GetTickDataACK(m_pStkInfo,tmpTickBuf,WANT_ZSTTICKS);

					if(tmpTickNum == 0)
					{
						m_bIsHead = TRUE;
						TDEL(tmpTickBuf);

						//继续
						if(!g_bManyDayZST||g_nZSTManyDay<1) break;					//当日的，不继续
						if(m_ReqingFlag>0&&MAX_ZST_MANYDAY-m_ReqingFlag+1>g_nZSTManyDay) break;
						//继续要
						if(m_bDataReady&&((m_nShowNum==DEF_SHOWNUM)||(m_nShowNum==m_nTotalXTick)))				//数据准备好了，才继续申请
						{
							if(m_nFirstShow==0&&m_ReqingFlag<0&&m_bManyDayIsHead[MAX_ZST_MANYDAY-1]==FALSE)		//开始历史的第一天
							{
								m_bManyDayZST = TRUE;
								m_ReqingFlag = MAX_ZST_MANYDAY;
								ManyDay_ReqData(g_lOpenRq);
								ReadMore();
								break;
							}
						}
						else
						{
							m_nFirstShow = 0;
							m_nShowNum = m_nTotalXTick;
							CalcIt();
							ProcessMsg(WM_PAINT);
						}
					}
					else
					{
						memcpy(tmpTickBuf+tmpTickNum,m_pTick,nLastTickNum*sizeof(TickData));
						TDEL(m_pTick);
						m_pTick = tmpTickBuf;
						m_nTickNum += tmpTickNum;

						CalcIt();
						
						int nCurPos = m_nXTickNum-1;
						if(m_bCrossMode) nCurPos = GetXTickPosFromX(m_CrossPoint.x);
						
						long nLastShow = min(m_nFirstShow+m_nShowNum-1, m_nTotalXTick-1);
						for(int i=m_nTotalXTick-1;m_pTotolXTick[i].nRealTickDataXh<0&&i>=0;i--);
						m_bShowStepSync = (i<=nLastShow);
						long nLastShowNum = nLastShow-m_nFirstShow+1;
						
						m_nShowNum = m_nTotalXTick;
						if(nLastShowNum==m_nShowNum) break;
						
						CalcIt();
						ProcessMsg(WM_PAINT);

						ReadMore();
						return 1;
					}	
				}
				else			//多日
				{
				 	if(m_bDSFlag)
						tmpTickNum = g_pGetData->DSGetHisTick2ACK(tmpTickBuf,&m_fManyDayClose[lastRegFlag-1],&m_fManyDayVolin[lastRegFlag-1]);
					else
						tmpTickNum = g_pGetData->GetHisTickDataACK(m_pStkInfo,tmpTickBuf,WANT_ZSTTICKS,&m_fManyDayClose[lastRegFlag-1]);

					if(tmpTickNum == 0)
					{
						m_bManyDayIsHead[lastRegFlag-1] = TRUE;
						TDEL(tmpTickBuf);
						m_ReqingFlag = lastRegFlag;

						//继续
						if(!g_bManyDayZST||g_nZSTManyDay<1) break;					//当日的，不继续
						if(m_ReqingFlag>0&&MAX_ZST_MANYDAY-m_ReqingFlag+1>g_nZSTManyDay) break;
						//继续要
						if(m_bDataReady&&((m_nShowNum==DEF_SHOWNUM)||(m_nShowNum==m_nTotalXTick)))				//数据准备好了，才继续申请
						{
							if(m_nFirstShow==0&&m_ReqingFlag>0&&m_bManyDayIsHead[m_ReqingFlag-1]==TRUE)
							{
								--m_ReqingFlag;
								ReadMore();
								break;
							}
						}
						else
						{
							m_nFirstShow = 0;
							m_nShowNum = m_nTotalXTick;
							CalcIt();
							ProcessMsg(WM_PAINT);
						}
					}
					else
					{
						memcpy(tmpTickBuf+tmpTickNum,m_pManyDayTickData[lastRegFlag-1],nLastTickNum*sizeof(TickData));
						TDEL(m_pManyDayTickData[lastRegFlag-1]);
						m_pManyDayTickData[lastRegFlag-1] = tmpTickBuf;	
						m_ManyDayTickNum[lastRegFlag-1] += tmpTickNum;
						m_ReqingFlag = lastRegFlag;

						CalcIt();
						
						int nCurPos = m_nXTickNum-1;
						if(m_bCrossMode) nCurPos = GetXTickPosFromX(m_CrossPoint.x);
						
						long nLastShow = min(m_nFirstShow+m_nShowNum-1, m_nTotalXTick-1);
						for(int i=m_nTotalXTick-1;m_pTotolXTick[i].nRealTickDataXh<0&&i>=0;i--);
						m_bShowStepSync = (i<=nLastShow);
						long nLastShowNum = nLastShow-m_nFirstShow+1;
						
						m_nShowNum = m_nTotalXTick;
						if(nLastShowNum==m_nShowNum) break;
						
						CalcIt();
						ProcessMsg(WM_PAINT);

						ReadMore();
						return 1;
					}
				}
			}
			break;
		}
		return 1;
	case ASYN_HQ_MESSAGE:
		{
			if(!m_pStkInfo)	
				return 1;
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
					if(m_nAddedTickNum >= ADDED_ZSTTICKS)
					{
						struct TickData *tmpTickBuf = new TickData[m_nTickNum+ADDED_ZSTTICKS];
						memcpy(tmpTickBuf,m_pTick,m_nTickNum*sizeof(TickData));
						TDEL(m_pTick);
						m_pTick = tmpTickBuf;
					}
					m_pTick[m_nTickNum] = tmpTick;
					m_nTickNum++;
					m_nAddedTickNum++;

					++m_nShowNum;

					CalcIt();
					ProcessMsg(WM_PAINT);
				}
			}
		}
		return 1;
	case WM_PAINT:
		{
			if(wParam == 1)
				ProcessMsg(UM_READDATA,lParam);

			if(m_bBeginRectangle && abs(m_OrgPoint.x-m_ArchorPoint.x)>2*SELECT_ON) break;

			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);
			m_MyDC.SetBkMode(TRANSPARENT);

			CFont *oldfont = g_d.SelectFont(&m_MyDC,GRAPH_FONT);
			CPen  *oldpen = g_d.SelectPen(&m_MyDC,AXIS_PEN);
			DrawIt(&m_MyDC);
			g_d.RestorePen(&m_MyDC,oldpen);
			g_d.RestoreFont(&m_MyDC,oldfont);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC*/
			DESTORY_TMPDC
			
			//如果需要，顺便画信息区
			CPoint TmpPoint=m_CrossPoint;
			m_CrossPoint=CPoint(0,0);
			if(m_bHasCross)
			{
				CRect rect(xStart,yStart,xEnd,yEndBot);
				if(rect.PtInRect(TmpPoint))
					InSwimArea(TmpPoint);
				else
					LeaveSwimArea();
			}
		}
		return 1;
	case UM_CALCANDPAINT:
		CalcIt();
		ProcessMsg(WM_PAINT);
		return 1;
	case UM_MANYDAYZST:
		{
			if(g_bManyDayZST&&m_ReqingFlag<0&&m_bManyDayIsHead[MAX_ZST_MANYDAY-g_nZSTManyDay]==FALSE)		//开始历史的第一天
			{
				m_bManyDayZST = TRUE;
				m_ReqingFlag = MAX_ZST_MANYDAY;
				ManyDay_ReqData(g_lOpenRq);
				m_nFirstShow = 0;
				ReadMore();
				break;
			}
			else if(g_bManyDayZST&&m_ReqingFlag>0&&m_bManyDayIsHead[MAX_ZST_MANYDAY-g_nZSTManyDay]==FALSE)	//继续要历史的
			{
				m_bManyDayZST = TRUE;
				int ii=MAX_ZST_MANYDAY-1;
				for(ii=MAX_ZST_MANYDAY-1;ii>=0&&m_bManyDayIsHead[ii]==TRUE;ii--);
				m_ReqingFlag = ii+1;
				m_nFirstShow = 0;
				ReadMore();
				break;
			}
			else if(!g_bManyDayZST)			//回归当天
			{
				m_bManyDayZST = FALSE;
				m_nFirstShow = 0;
				CalcIt();
				if(m_nShowNum != m_nTotalXTick)
				{
					m_nShowNum = m_nTotalXTick;
					CalcIt();
				}
				ProcessMsg(WM_PAINT);
			}
			else							//回归历史
			{
				m_bManyDayZST = TRUE;
				m_nFirstShow = 0;
				CalcIt();
				if(m_nShowNum != m_nTotalXTick)
				{
					m_nShowNum = m_nTotalXTick;
					CalcIt();
				}
				ProcessMsg(WM_PAINT);
			}

			return 1;
		}
	case UM_TICKZSTUNDO:
		{
			CalcIt();
			m_nFirstShow = 0;
			m_nShowNum = m_nTotalXTick;
			CalcIt();
			ProcessMsg(WM_PAINT);
		}
		return 1;
	case WM_KEYDOWN:
		{
			UINT nChar = (UINT)wParam;
			SHORT CtrlDown =::GetKeyState(VK_CONTROL);
			SHORT ShiftDown=::GetKeyState(VK_SHIFT);
			switch(nChar)
			{
			case VK_DOWN:
				{
					int nWantManyDay = 1;
					if(CtrlDown<0)
					{
						int nManyParam = nWantManyDay+m_nDays+ID_MANYDAYTICKZST0;
						if(m_nDays==1&&!m_bManyDayZST) nManyParam = ID_MANYDAYTICKZST0+1;
						nManyParam = min(nManyParam, ID_MANYDAYTICKZST9);
						nManyParam = max(nManyParam, ID_MANYDAYTICKZST0);
						if(nManyParam<=ID_MANYDAYTICKZST9)
							m_pView->SendMessage(WM_COMMAND, nManyParam);	
					}
				}
				break;
			case VK_UP:
				{
					int nWantManyDay = -1;
					if(CtrlDown<0)
					{
						int nManyParam = nWantManyDay+m_nDays+ID_MANYDAYTICKZST0;
						nManyParam = min(nManyParam, ID_MANYDAYTICKZST9);
						nManyParam = max(nManyParam, ID_MANYDAYTICKZST0);
						if(nManyParam>=ID_MANYDAYTICKZST0)
							m_pView->SendMessage(WM_COMMAND, nManyParam);	
					}
				}
				break;				
			case VK_LEFT:
				if(CtrlDown<0)	ToNextMin(0,5);
				else			ToNextMin(0,1);
				break;
			case VK_RIGHT:
				if(CtrlDown<0)	ToNextMin(1,5);
				else			ToNextMin(1,1);
				break;
			case VK_RETURN:
				if(!m_bHasCross && !m_nInCom2 && !m_nInHisDlg) //进入分析图
				{
					g_pMainWin->PostMessage(WM_COMMAND,ID_300);
					return 1;
				}
				if(m_bHasCross && m_CrossPoint!=CPoint(0,0))
				{
					int NowAbsTime=GetXTickTimeFromX(m_CrossPoint.x);
					if(NowAbsTime>0) 
						m_pView->SendMessage(UM_REFLECT,Reflect_ToZstime,NowAbsTime);
				}
				return 1;
			case VK_ESCAPE://如果Escape被处理，则返回1,否则返回0
				if(m_bHasCross)
				{
					m_bCrossMode = FALSE;
					m_bHasCross = FALSE;
					ProcessMsg(WM_PAINT);
					return 1;
				}
				else
					return 0;
			case VK_HOME:
				{
					int nCurPos = m_nXTickNum-1;
					if(m_bCrossMode) nCurPos = GetXTickPosFromX(m_CrossPoint.x);

					long nLastShow = min(m_nFirstShow+m_nShowNum-1, m_nTotalXTick-1);
					for(int i=m_nTotalXTick-1;m_pTotolXTick[i].nRealTickDataXh<0&&i>=0;i--);
					m_bShowStepSync = (i<=nLastShow);
					long nLastShowNum = nLastShow-m_nFirstShow+1;					
					m_nShowNum = max(0.5f*nLastShowNum, 16);
					if(nLastShowNum==m_nShowNum) break;
					
					long nLastPos = nCurPos;
					nCurPos = (float)nCurPos*fabs((float)m_nShowNum-(float)nLastShowNum)/(float)nLastShowNum;
					nCurPos = max(0, nCurPos);

					int nLastFirstShow = m_nFirstShow;
					m_nFirstShow += (nLastPos-nCurPos);
					m_nFirstShow = max(0, m_nFirstShow);
					m_nFirstShow = min(m_nFirstShow, m_nTotalXTick-m_nShowNum);
					nCurPos = nLastPos+(nLastFirstShow-m_nFirstShow);
					nCurPos = max(0, nCurPos);

					CalcIt();
					ProcessMsg(WM_PAINT);
					m_CrossPoint.x = GetXFromXTick(nCurPos);
					ProcessMsg(WM_PAINT);
				}
				break;
			case VK_END:
				{
					int nCurPos = m_nXTickNum-1;
					if(m_bCrossMode) nCurPos = GetXTickPosFromX(m_CrossPoint.x);

					long nLastShow = min(m_nFirstShow+m_nShowNum-1, m_nTotalXTick-1);
					for(int i=m_nTotalXTick-1;m_pTotolXTick[i].nRealTickDataXh<0&&i>=0;i--);
					m_bShowStepSync = (i<=nLastShow);
					long nLastShowNum = nLastShow-m_nFirstShow+1;
					
					m_nShowNum = min(1.5f*nLastShowNum, m_nTotalXTick);
					if(nLastShowNum==m_nShowNum) break;
					
					long nLastPos = nCurPos;
					nCurPos = (float)nLastPos*(float)m_nShowNum/(float)nLastShowNum;
					nCurPos = max(0, nCurPos);
					
					int nLastFirstShow = m_nFirstShow;
					m_nFirstShow += (nLastPos-nCurPos);
					m_nFirstShow = max(0, m_nFirstShow);
					m_nFirstShow = min(m_nFirstShow, m_nTotalXTick-m_nShowNum);
					nCurPos = nLastPos+(nLastFirstShow-m_nFirstShow);
					nCurPos = max(0, nCurPos);
					
					CalcIt();
					ProcessMsg(WM_PAINT);
					m_CrossPoint.x = GetXFromXTick(nCurPos);
					ProcessMsg(WM_PAINT);
				}
				break;
			default:
				break;
			}
		}
		break;
	case UM_MOUSELEAVE:
		{
			if(m_bBeginRectangle)
			{
				m_bBeginRectangle = FALSE;
				ProcessMsg(WM_PAINT);
			}
			LeaveSwimArea();
		}
		return 1;
	case WM_MOUSEMOVE:
		{
			CPoint pt(LOWORD(lParam),HIWORD(lParam));
			if(m_RealMove == pt)	return 1;	//如果没有变化，则不算是MouseMove，因为有些软件如Winamp会频繁调MouseMove
			if(m_RealMove == CPoint(0,0))	{ m_RealMove = pt; return 1; }
			if(m_RealMove.x == xStart)		{ m_RealMove = pt; return 1; }
			m_RealMove = pt;
			pt.x-=left;pt.y-=top;
			CRect rect(xStart,yStart,xEnd,yEndBot);
			if(rect.PtInRect(pt))
				InSwimArea(pt);
			else
			{
				if(m_bBeginRectangle)
				{
					m_bBeginRectangle = FALSE;
					ProcessMsg(WM_PAINT);
				}
				LeaveSwimArea();
			}
			if ( m_bBeginRectangle && abs(m_OrgPoint.x-pt.x)>2*SELECT_ON )
			{  
				CDC *pDC = m_pView->GetDC();
				int tempMode = pDC->SetROP2 (R2_XORPEN);
				if ( pt != m_ArchorPoint && m_ArchorPoint.y !=0 )
					g_d.DrawRect(pDC,left+m_OrgPoint.x,top+m_OrgPoint.y,left+m_ArchorPoint.x,top+m_ArchorPoint.y,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
				m_ArchorPoint = pt;
				g_d.DrawRect(pDC,left+m_OrgPoint.x,top+m_OrgPoint.y,left+m_ArchorPoint.x,top+m_ArchorPoint.y,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
				pDC->SetROP2 (tempMode);
			}
		}
		return 1;
	case WM_LBUTTONDBLCLK:
		{
			CPoint pt;
			pt.x=LOWORD(lParam)-left;
			pt.y=HIWORD(lParam)-top;
			if(pt.x>xEnd && pt.y<yEndBot && !m_nInCom2 && !m_nInHisDlg) //双击右轴进入分析图
			{
				g_pMainWin->PostMessage(WM_COMMAND,ID_300);
				return 1;
			}			
			m_bCrossMode = !m_bCrossMode;
			m_bHasCross = m_bCrossMode;
			m_CrossPoint = pt;
			ProcessMsg(WM_PAINT);
		}
		return 1;
	case WM_LBUTTONDOWN:
		{
			if(!m_bDataReady) return 1;
			CPoint tempPoint(LOWORD(lParam),HIWORD(lParam));
			tempPoint.x-=left;tempPoint.y-=top;
			memset(&m_ArchorPoint,0,sizeof(CPoint));
			memset(&m_OrgPoint ,0,sizeof(CPoint));
			m_OrgPoint = tempPoint;
			m_bBeginRectangle = TRUE;
		}
		return 1;
	case WM_LBUTTONUP:
		{
			if(!m_bDataReady) return 1;
			CPoint tempPoint(LOWORD(lParam),HIWORD(lParam));
			m_pView->ClientToScreen (&tempPoint);
			if ( m_bBeginRectangle  )
			{
				m_bBeginRectangle = FALSE;
				if ( m_ArchorPoint.x+m_ArchorPoint.y>0 && m_ArchorPoint.x - m_OrgPoint.x > 16*fPointPerXTick)
				{
					m_nFirstShow += GetXTickPosFromX(m_OrgPoint.x);
					m_nShowNum = ( m_ArchorPoint.x - m_OrgPoint.x )/fPointPerXTick;
					m_nShowNum = max(m_nShowNum, 16);
					CalcIt();
				}
				ProcessMsg(WM_PAINT);
			}
		}
		return 1;
	case WM_RBUTTONUP:
		{
			if(!m_bDataReady) return 1;
			CPoint tempPoint(LOWORD(lParam),HIWORD(lParam));
			m_pView->ClientToScreen (&tempPoint);
			//先装入已有的菜单项
			CNewMenu menu;
			menu.LoadMenu (IDR_LONELY_MENU);
			CNewMenu *pPopup = (CNewMenu *)menu.GetSubMenu(27);	
			pPopup->TrackPopupMenu (TPM_LEFTALIGN,tempPoint.x,tempPoint.y,AfxGetMainWnd());
		}
		return 1;
	case UM_SETSEL:
		m_bSelected = wParam;
		return 1;
	case UM_GETSEL:
		return m_bSelected?1:0;
	case UM_SET_STOCK: //对于同步回忆状态,如果重新设置股票，则需要重新预读
		{
			UBase::ProcessMsg(message,wParam,lParam);
			
			TDEL(m_pXTick); TDEL(m_pTotolXTick);
			TDEL(m_pTick);
			m_nXMaxTickNum = m_nXTickNum = m_nTotalXTick = 0; 
			m_nTickNum = 0;
			m_nFirstShow = 0; m_nShowNum = 9999999; m_bShowStepSync = TRUE;
			m_bIsHead = FALSE;
			for(int i=0;i<MAX_ZST_MANYDAY;i++)
			{
				TDEL(m_pManyDayTickData[i]);
				m_ManyDayTickNum[i] = 0;
				m_bManyDayIsHead[i] = FALSE;

				m_fManyDayClose[i] = -999999.99f;
				m_fManyDayVolin[i] = 0;
			}
			memset(m_ManyDayDate,0,MAX_ZST_MANYDAY*sizeof(long));
			m_nDays = 0;
		}
		return 1;
	case UM_POPZSTICKDLG:
		break;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

long UTickZst::GetDate(int whichHis)
{
	if(m_bManyDayZST&&whichHis<MAX_ZST_MANYDAY&&m_ManyDayDate[MAX_ZST_MANYDAY-g_nZSTManyDay+whichHis]>0) 
		return m_ManyDayDate[MAX_ZST_MANYDAY-g_nZSTManyDay+whichHis];
	return g_lOpenRq;
}

void UTickZst::SetStyle(int style)
{
	m_nStyle = style;
	ProcessMsg(WM_PAINT);
}

void UTickZst::ReadMore()
{
	short lastRegFlag = m_ReqingFlag;
	if(m_bIsHead==FALSE)		//当日还未要完
	{
		m_bDataReady = FALSE;
		m_ReqingFlag = 0;
		m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
		g_nAssisID = SetComAssisID(m_nWhichUnit,1);
		if(m_bDSFlag)
			g_pGetData->DSGetTick(m_pStkInfo,m_nTickNum,WANT_ZSTTICKS);
		else 
			g_pGetData->GetTickData(m_pStkInfo,m_nTickNum,WANT_ZSTTICKS,TRUE);
		return;
	}
	if(lastRegFlag>0&&m_bManyDayZST&&m_bManyDayIsHead[lastRegFlag-1]==FALSE)
	{
		m_bDataReady = FALSE;
		m_ReqingFlag = lastRegFlag;
		m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
		g_nAssisID = SetComAssisID(m_nWhichUnit,2);
		if(m_bDSFlag)
			g_pGetData->DSGetHisTick2(m_pStkInfo, m_ManyDayDate[lastRegFlag-1], m_ManyDayTickNum[lastRegFlag-1],WANT_ZSTTICKS);
		else 
			g_pGetData->GetHisTickData(m_pStkInfo,m_ManyDayDate[lastRegFlag-1],m_ManyDayTickNum[lastRegFlag-1],WANT_ZSTTICKS);
		return;
	}
}

void UTickZst::ReadData(BOOL bDataReady,BOOL bReqData)
{
	memset(&m_Hq,0,sizeof(CurrStockData));
	memset(&m_HqEx,0,sizeof(EXCURRDATA));
	
	fBasePrice = 0.0;
	m_nTickNum = 0;
	TDEL(m_pTick);

	if(m_lDate == 0)	//当天Tick
	{
		g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq);
		if(!bDataReady)
		{
			if(!OnLine)
			{
				m_pTick = new TickData[WANT_ZSTTICKS+ADDED_ZSTTICKS];
				memset(m_pTick,0,sizeof(TickData)*(WANT_ZSTTICKS+ADDED_ZSTTICKS));
			}
			if(OnLine && bReqData)
			{
				m_bDataReady = FALSE;
				m_pTick = new TickData[WANT_ZSTTICKS+ADDED_ZSTTICKS];
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
					g_pGetData->DSGetTick(m_pStkInfo,0,WANT_ZSTTICKS);
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
					g_pGetData->GetTickData(m_pStkInfo,0,WANT_ZSTTICKS,TRUE);
				}
			}
		}
		else
		{
			m_pTick = new TickData[WANT_ZSTTICKS+ADDED_ZSTTICKS];
			if(m_bDSFlag)	
				m_nTickNum = g_pGetData->DSGetTickACK(m_pTick);
			else	
				m_nTickNum = g_pGetData->GetTickDataACK(m_pStkInfo,m_pTick,WANT_ZSTTICKS);

			ReadMore();
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
				m_pTick = new TickData[WANT_ZSTTICKS+ADDED_ZSTTICKS];
				m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
				m_bDataReady = FALSE;
				g_nAssisID = SetComAssisID(m_nWhichUnit,0);
				if(m_bDSFlag)
					g_pGetData->DSGetHisTick2(m_pStkInfo, m_lDate, 0, WANT_ZSTTICKS);
				else g_pGetData->GetHisTickData(m_pStkInfo,m_lDate,0,WANT_ZSTTICKS);
			}
		}
		else
		{
			m_pTick = new TickData[WANT_ZSTTICKS+ADDED_ZSTTICKS];
			if(m_bDSFlag)
			{
				long Volin = 0;
				m_nTickNum = g_pGetData->DSGetHisTick2ACK(m_pTick,&m_Hq.Close,&Volin);
			}
			else 
				m_nTickNum = g_pGetData->GetHisTickDataACK(m_pStkInfo,m_pTick,WANT_ZSTTICKS,&m_Hq.Close);

			ReadMore();
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
	fBasePrice = m_Hq.Close;
	if (fBasePrice<COMPPREC && m_Hq.Now>COMPPREC)
		fBasePrice=m_Hq.Now;

	CalcIt();
	
	int nCurPos = m_nXTickNum-1;
	if(m_bCrossMode) nCurPos = GetXTickPosFromX(m_CrossPoint.x);
	
	long nLastShow = min(m_nFirstShow+m_nShowNum-1, m_nTotalXTick-1);
	for(i=m_nTotalXTick-1;m_pTotolXTick[i].nRealTickDataXh<0&&i>=0;i--);
	m_bShowStepSync = (i<=nLastShow);
	long nLastShowNum = nLastShow-m_nFirstShow+1;
	
	m_nShowNum = m_nTotalXTick;
	if(nLastShowNum==m_nShowNum)
	{
		CalcIt();
		ProcessMsg(WM_PAINT);
		return;
	}
	
	long nLastPos = nCurPos;
	nCurPos = (float)nLastPos*(float)m_nShowNum/(float)nLastShowNum;
	nCurPos = max(0, nCurPos);
	
	int nLastFirstShow = m_nFirstShow;
	m_nFirstShow += (nLastPos-nCurPos);
	m_nFirstShow = max(0, m_nFirstShow);
	m_nFirstShow = min(m_nFirstShow, m_nTotalXTick-m_nShowNum);
	nCurPos = nLastPos+(nLastFirstShow-m_nFirstShow);
	nCurPos = max(0, nCurPos);
	
	CalcIt();
	ProcessMsg(WM_PAINT);
}

void UTickZst::CalcIt()
{
	GetCriticalTime(m_CriticalTime, TimeSec, 4);

	double fMaxPrice = (m_Hq.Max>0.001)?m_Hq.Max:m_Hq.Close;
	double fMinPrice = (m_Hq.Min>0.001)?m_Hq.Min:m_Hq.Close;
	lMaxVol = 30;
	lMaxVolInStock = lMinVolInStock = m_Hq.Yield_VolInStock.VolInStock;
	long nAllTickNum = 0, nAllMinute = 0, nStepMin = -1;
	long nStartMin = -1, nEndMin = -1;
	for(int i=0,j=0;i<m_nTickNum;i++)
	{
		nAllTickNum++;
		if(nStepMin!=m_pTick[i].Minute)
		{
			nAllMinute++;
			nStepMin = m_pTick[i].Minute;
		}

		if(i==0) nStartMin = m_pTick[i].Minute;
		if(i==m_nTickNum-1) nEndMin = m_pTick[i].Minute;

		fMaxPrice = max(fMaxPrice,GetAbsPrice(m_pStkInfo,m_pTick[i].Now,m_XsFlag));
		fMinPrice = min(fMinPrice,GetAbsPrice(m_pStkInfo,m_pTick[i].Now,m_XsFlag));
		if(m_pTick[i].NowVol > 2000000000l)
			m_pTick[i].NowVol=0;		
		lMaxVol   = max(lMaxVol,m_pTick[i].NowVol);
	}
	m_nDays = 0;
	if(m_bManyDayZST) //如果是多日走势图
	{
		for(i=0;i<MAX_ZST_MANYDAY;i++)
		{
			nStepMin = -1;
			if(m_fManyDayClose[i]<0-COMPPREC) continue;
			//可选是否显示
			if(MAX_ZST_MANYDAY-i>g_nZSTManyDay) continue;
			
			for(j=0;j<m_ManyDayTickNum[i];j++)
			{
				nAllTickNum++;
				if(nStepMin!=m_pManyDayTickData[i][j].Minute)
				{
					nAllMinute++;
					nStepMin = m_pManyDayTickData[i][j].Minute;
				}

				if(j==0) nStartMin = m_pManyDayTickData[i][j].Minute;
				
				fMaxPrice = max(fMaxPrice,GetAbsPrice(m_pStkInfo,m_pManyDayTickData[i][j].Now,m_XsFlag));
				fMinPrice = min(fMinPrice,GetAbsPrice(m_pStkInfo,m_pManyDayTickData[i][j].Now,m_XsFlag));
				lMaxVol   = max(lMaxVol,m_pManyDayTickData[i][j].NowVol);
			}
			++m_nDays;
		}
	}
	lMaxVol *= 1.1;
	fMaxDelta=max(fabs(fMaxPrice-fBasePrice),fabs(fMinPrice-fBasePrice));
	fMaxDelta=max(fMaxDelta,fBasePrice*(m_bZs?0.01f:0.004f)); //将指数的最小绝对范围改为1%

	short nTickNumPerMin = 1;
	if(nAllMinute>0) nTickNumPerMin = nAllTickNum/nAllMinute+1;

	double fMaxPrice2 = 0.0,fMinPrice2 = 0.0;
	float ftmpclose = 0.0;
	if(g_bTPAxis) //如果是TP坐标,需重新计算fMaxDelta和fMaxDelta
	{
		fMaxDelta = max(fMaxDelta,GetTPPrice(m_pStkInfo,m_Hq.Close,TRUE)-fBasePrice);
		fMaxDelta = max(fMaxDelta,GetTPPrice(m_pStkInfo,m_Hq.Close,FALSE)-fBasePrice);
	}

	//申请空间填充
	int nTmpMinNum = GetRealTotalMinute(TimeSec)*(1+m_nDays);
	int XMaxTickNum = max(nTmpMinNum, nTmpMinNum*nTickNumPerMin)+100;
	if(XMaxTickNum>=m_nXMaxTickNum-1)			//空间不购了
	{
		m_nXMaxTickNum = max(nTmpMinNum, XMaxTickNum);
		TDEL(m_pXTick); m_nXTickNum = 0;
		m_pXTick = new TICKZSTUNIT[m_nXMaxTickNum];
		memset(m_pXTick, 0, m_nXMaxTickNum*sizeof(TICKZSTUNIT));

		TDEL(m_pTotolXTick); m_nTotalXTick = 0;
		m_pTotolXTick = new TICKZSTUNIT[m_nXMaxTickNum];
		memset(m_pTotolXTick, 0, m_nXMaxTickNum*sizeof(TICKZSTUNIT));
	}

	//重置
	m_nXTickNum = 0;
	memset(m_pXTick, 0, m_nXMaxTickNum*sizeof(TICKZSTUNIT));
	//填充历史数据
	if(m_bManyDayZST) //如果是多日走势图
	{
		for(i=0;i<MAX_ZST_MANYDAY;i++)
		{
			TickData *pTick = m_pManyDayTickData[i];
			int nTickNum = m_ManyDayTickNum[i];
			if(m_fManyDayClose[i]<0-COMPPREC) continue;
			if(MAX_ZST_MANYDAY-i>g_nZSTManyDay) continue;

			//填充m_pXTick前面的空白区域
			int nFirstValidMin = TimeSec[0];
			if(pTick) nFirstValidMin = pTick[0].Minute;
			int nCurMin=min(TimeSec[0], nFirstValidMin);
			for(;m_bManyDayIsHead[i]&&nCurMin<nFirstValidMin&&m_nXTickNum<m_nXMaxTickNum;)
			{
				long nNextMin = nCurMin+1;
				if(BeCriticalTime(nCurMin, nNextMin)&&nNextMin == nCurMin+1)
					m_pXTick[m_nXTickNum].Flag |= CRITICAL_MIN;
				for(int j=0;j<1/*nTickNumPerMin*/;j++)
				{
					m_pXTick[m_nXTickNum].nDataDate = m_ManyDayDate[i];
					m_pXTick[m_nXTickNum].nMinute = nCurMin;
					m_pXTick[m_nXTickNum].nRealTickDataXh = -1;
					m_nXTickNum++;
				}
				nCurMin = nNextMin;
			}
			//填充m_pXTick有效数据区
			if(pTick&&nTickNum>0)
			{
				float TotalAmo = 0.0f, TotalVol = 0.0f;
				long LastVolInStock = (long)m_HqEx.PreVolIn;
				for(j=0;j<nTickNum&&nCurMin<pTick[nTickNum-1].Minute+1&&m_nXTickNum<m_nXMaxTickNum;)
				{
					long nNextMin = nCurMin+1;
					if(BeCriticalTime(nCurMin, nNextMin)&&nNextMin == nCurMin+1)
					{
						if(!m_bIsHead&&nCurMin<m_pTick[0].Minute) 
						{
							nCurMin = nNextMin;
							continue;
						}

						m_pXTick[m_nXTickNum].Flag |= CRITICAL_MIN;
						if(nCurMin!=pTick[j].Minute)
						{
							m_pXTick[m_nXTickNum].nDataDate = m_ManyDayDate[i];
							m_pXTick[m_nXTickNum].nMinute = nCurMin;
							m_pXTick[m_nXTickNum].nRealTickDataXh = -1;
							m_nXTickNum++;
						}
					}
					int iii = j;
					while (pTick[iii].Minute<nCurMin&&iii<nTickNum)
						pTick[iii++].Minute = nCurMin;
					while(pTick[j].Minute>=nCurMin&&pTick[j].Minute<nNextMin&&
						(j<nTickNum&&nCurMin<pTick[nTickNum-1].Minute+1&&m_nXTickNum<m_nXMaxTickNum))
					{
						m_pXTick[m_nXTickNum].nDataDate = m_ManyDayDate[i];
						m_pXTick[m_nXTickNum].nMinute = pTick[j].Minute;
		
						m_pXTick[m_nXTickNum].Now = pTick[j].Now;
						m_pXTick[m_nXTickNum].NowVol = pTick[j].NowVol;
						TotalAmo += GetAbsPrice(m_pStkInfo,pTick[j].Now,m_XsFlag)*pTick[j].NowVol;
						TotalVol += pTick[j].NowVol;
						m_pXTick[m_nXTickNum].InOutFlag = pTick[j].InOutFlag;
						if(TotalVol>0) 
						{
							long fAver = GetRelPrice(m_pStkInfo,TotalAmo/TotalVol,m_XsFlag);
							if(m_nXTickNum==0) 
								m_pXTick[m_nXTickNum].Average = fAver;
							else if(fAver<1.1f*max(m_pXTick[m_nXTickNum].Now,m_pXTick[m_nXTickNum-1].Average)&&
								fAver>0.9f*min(m_pXTick[m_nXTickNum].Now,m_pXTick[m_nXTickNum-1].Average))
								m_pXTick[m_nXTickNum].Average = fAver;
							else 
								m_pXTick[m_nXTickNum].Average = m_pXTick[m_nXTickNum-1].Average;
						}
						m_pXTick[m_nXTickNum].nRealTickDataXh = j++;
						
						m_nXTickNum++;
					}
					nCurMin = nNextMin;
				}
			}
			//填充m_pXTick后面的空白区域
			for(;nCurMin<=TimeSec[2*GetTdxSectNum(TimeSec)-1]&&m_nXTickNum<m_nXMaxTickNum;)
			{
				long nNextMin = nCurMin+1;
				if(BeCriticalTime(nCurMin, nNextMin)&&nNextMin == nCurMin+1)
					m_pXTick[m_nXTickNum].Flag |= CRITICAL_MIN;
				for(int j=0;j<1/*nTickNumPerMin*/;j++)
				{
					m_pXTick[m_nXTickNum].nDataDate = m_ManyDayDate[i];
					m_pXTick[m_nXTickNum].nMinute = nCurMin;
					m_pXTick[m_nXTickNum].nRealTickDataXh = -1;
					m_nXTickNum++;
				}
				nCurMin = nNextMin;
			}
		}
	}
	//填充m_pXTick前面的空白区域
	int nFirstValidMin = TimeSec[0];
	if(m_pTick&&m_nTickNum>0) nFirstValidMin = m_pTick[0].Minute;
	int nCurMin=min(TimeSec[0], nFirstValidMin);
	for(;m_bIsHead&&nCurMin<nFirstValidMin&&m_nXTickNum<m_nXMaxTickNum;)
	{
		long nNextMin = nCurMin+1;
		if(BeCriticalTime(nCurMin, nNextMin)&&nNextMin == nCurMin+1)
		{
			m_pXTick[m_nXTickNum].Flag |= CRITICAL_MIN;
			short nSecStart = TimeSec[2*GetNowTdxSect(nCurMin,TimeSec)];
			if((nCurMin-nSecStart)%60==0) 
				m_pXTick[m_nXTickNum].Flag |= DRAW_TIME; 
		}
		for(int j=0;j<1/*nTickNumPerMin*/;j++)
		{
			m_pXTick[m_nXTickNum].nDataDate = g_lOpenRq;
			m_pXTick[m_nXTickNum].nMinute = nCurMin;
			m_pXTick[m_nXTickNum].nRealTickDataXh = -1;
			m_nXTickNum++;
		}
		nCurMin = nNextMin;
	}
	//填充m_pXTick有效数据区
	if(m_pTick&&m_nTickNum>0)
	{
		float TotalAmo = 0.0f, TotalVol = 0.0f;
		long LastVolInStock = (long)m_HqEx.PreVolIn;
		for(i=0;i<m_nTickNum&&nCurMin<m_pTick[m_nTickNum-1].Minute+1&&m_nXTickNum<m_nXMaxTickNum;)
		{
			long nNextMin = nCurMin+1;
			if(BeCriticalTime(nCurMin, nNextMin)&&nNextMin == nCurMin+1)
			{
				if(!m_bIsHead&&nCurMin<m_pTick[0].Minute) 
				{
					nCurMin = nNextMin;
					continue;
				}

				m_pXTick[m_nXTickNum].Flag |= CRITICAL_MIN;
				short nSecStart = TimeSec[2*GetNowTdxSect(nCurMin,TimeSec)];
				if((nCurMin-nSecStart)%60==0) 
					m_pXTick[m_nXTickNum].Flag |= DRAW_TIME;
				if(nCurMin!=m_pTick[i].Minute)
				{
					m_pXTick[m_nXTickNum].nDataDate = g_lOpenRq;
					m_pXTick[m_nXTickNum].nMinute = nCurMin;
					m_pXTick[m_nXTickNum].nRealTickDataXh = -1;
					m_nXTickNum++;
				}
			}
			int iii = i;
			while (m_pTick[iii].Minute<nCurMin&&iii<m_nTickNum)
				m_pTick[iii++].Minute = nCurMin;
			while(m_pTick[i].Minute>=nCurMin&&m_pTick[i].Minute<nNextMin&&
				(i<m_nTickNum&&nCurMin<m_pTick[m_nTickNum-1].Minute+1&&m_nXTickNum<m_nXMaxTickNum))
			{
				m_pXTick[m_nXTickNum].nDataDate = g_lOpenRq;
				m_pXTick[m_nXTickNum].nMinute = m_pTick[i].Minute;
				m_pXTick[m_nXTickNum].Now = m_pTick[i].Now;
				m_pXTick[m_nXTickNum].NowVol = m_pTick[i].NowVol;
				TotalAmo += GetAbsPrice(m_pStkInfo,m_pTick[i].Now,m_XsFlag)*m_pTick[i].NowVol;
				TotalVol += m_pTick[i].NowVol;
				m_pXTick[m_nXTickNum].InOutFlag = m_pTick[i].InOutFlag;
				if(TotalVol>0) 
				{
					float fAver = GetRelPrice(m_pStkInfo,TotalAmo/TotalVol,m_XsFlag);
					if(m_nXTickNum==0) 
						m_pXTick[m_nXTickNum].Average = fAver;
					else if(fAver<1.1f*max(m_pXTick[m_nXTickNum].Now,m_pXTick[m_nXTickNum-1].Average)&&
						fAver>0.9f*min(m_pXTick[m_nXTickNum].Now,m_pXTick[m_nXTickNum-1].Average))
						m_pXTick[m_nXTickNum].Average = fAver;
					else 
						m_pXTick[m_nXTickNum].Average = m_pXTick[m_nXTickNum-1].Average;
				}
				m_pXTick[m_nXTickNum].nRealTickDataXh = i++;
				
				m_nXTickNum++;
			}
			nCurMin = nNextMin;
		}
	}
	//填充m_pXTick后面的空白区域
	for(;nCurMin<=TimeSec[2*GetTdxSectNum(TimeSec)-1]&&m_nXTickNum<m_nXMaxTickNum;)
	{
		long nNextMin = nCurMin+1;
		if(BeCriticalTime(nCurMin, nNextMin)&&nNextMin == nCurMin+1)
		{
			m_pXTick[m_nXTickNum].Flag |= CRITICAL_MIN;
			short nSecStart = TimeSec[2*GetNowTdxSect(nCurMin,TimeSec)];
			if((nCurMin-nSecStart)%60==0) 
				m_pXTick[m_nXTickNum].Flag |= DRAW_TIME; 
		}
		for(int j=0;j<1/*nTickNumPerMin*/;j++)
		{
			m_pXTick[m_nXTickNum].nDataDate = g_lOpenRq;
			m_pXTick[m_nXTickNum].nMinute = nCurMin;
			m_pXTick[m_nXTickNum].nRealTickDataXh = -1;
			m_nXTickNum++;
		}
		nCurMin = nNextMin;
	}
	
	//平滑均价,如果有特别异常的Average(转码机不可避免的错误)
	BOOL bLastValid = FALSE;
	long LastDate = 0;
	long nNowStepVolIn = m_Hq.Yield_VolInStock.VolInStock, diff = 0;
	for(i=m_nXTickNum-1; i>= 0;i--)
	{
		if(m_pXTick[i].nRealTickDataXh>=0&&m_pTick&&m_nTickNum>0&&m_pXTick[i].nRealTickDataXh<m_nTickNum)
		{
			TickData *pTick = &m_pTick[m_pXTick[i].nRealTickDataXh];
			if(bLastValid==FALSE) 
			{
				m_pXTick[i].VolInStock = nNowStepVolIn;
				diff = pTick->VolInStock_dif;
				bLastValid = TRUE;
				LastDate = m_pXTick[i].nDataDate;
			}
			else if(LastDate!=m_pXTick[i].nDataDate) 
			{
				for(int ii=MAX_ZST_MANYDAY-1;ii>=0;ii--)
				{
					if(m_ManyDayDate[ii]==m_pXTick[i].nDataDate)
					{
						m_pXTick[i].VolInStock = nNowStepVolIn = m_fManyDayVolin[ii];
						diff = pTick->VolInStock_dif;
						LastDate = m_pXTick[i].nDataDate;
						break;
					}
				}
				if(ii<0)
				{
					m_pXTick[i].VolInStock = nNowStepVolIn = m_Hq.Yield_VolInStock.VolInStock;
					diff = pTick->VolInStock_dif;
				}
			}
			else 
			{
				nNowStepVolIn -= diff;
				if(nNowStepVolIn<1&&i<m_nXTickNum-1) nNowStepVolIn = m_pXTick[i+1].VolInStock;
				m_pXTick[i].VolInStock = nNowStepVolIn;
				diff = pTick->VolInStock_dif;
			}
			lMaxVolInStock = max(lMaxVolInStock,m_pXTick[i].VolInStock);
			lMinVolInStock = min(lMinVolInStock,m_pXTick[i].VolInStock);
		}
		else 
			m_pXTick[i].VolInStock = nNowStepVolIn;
	}
	long nGap = lMaxVolInStock-lMinVolInStock;
	lMinVolInStock -= max(nGap,100)*0.1;
	lMaxVolInStock += max(nGap,100)*0.1;
	//根据均价重新计算fMaxDelta
	for(i = 0; i < m_nXTickNum; i++)
	{
		if(m_pXTick[0].nRealTickDataXh>=0 && m_pXTick[i].Average>0 &&
			fabs(GetAbsPrice(m_pStkInfo,m_pXTick[i].Average,m_XsFlag) - fBasePrice) > fMaxDelta)
			fMaxDelta = fabs(GetAbsPrice(m_pStkInfo,m_pXTick[i].Average,m_XsFlag)-fBasePrice);
	}
	fMaxDelta=max(fMaxDelta,(m_XsFlag==2)?0.07:0.007); //保证每格刻度不重复

	//填充显示的
	int nCurPos = m_nXTickNum-1;
	if(m_bCrossMode) nCurPos = GetXTickPosFromX(m_CrossPoint.x);
	m_nTotalXTick = m_nXTickNum;
	memcpy(m_pTotolXTick, m_pXTick, m_nTotalXTick*sizeof(TICKZSTUNIT));
	m_nXTickNum = min(m_nShowNum, m_nXTickNum); 
	memset(m_pXTick, 0, m_nXMaxTickNum*sizeof(TICKZSTUNIT));
	long nLastShow = min(m_nFirstShow+m_nShowNum-1, m_nTotalXTick-1);
	if(m_bShowStepSync)
	{
		for(i=m_nTotalXTick-1;m_pTotolXTick[i].nRealTickDataXh<0&&i>=0;i--);
		nLastShow = max(i, nLastShow);
	}

	lMaxVol = 0;
	memcpy(m_pXTick, &m_pTotolXTick[m_nFirstShow], (nLastShow-m_nFirstShow+1)*sizeof(TICKZSTUNIT));
	for(j=0;j<nLastShow-m_nFirstShow+1;j++)
	{
		if(m_pXTick[j].nRealTickDataXh<0)
			continue;
		fMaxPrice = max(fMaxPrice,GetAbsPrice(m_pStkInfo,m_pXTick[j].Now,m_XsFlag));
		fMinPrice = min(fMinPrice,GetAbsPrice(m_pStkInfo,m_pXTick[j].Now,m_XsFlag));
		lMaxVol   = max(lMaxVol,m_pXTick[j].NowVol);
	}
	if(m_Hq.Now>COMPPREC)
	{
		fMaxPrice = max(fMaxPrice, m_Hq.Now);
		fMinPrice = min(fMinPrice, m_Hq.Now);
	}
	lMaxVol *= 1.1;
	fMaxDelta=max(fabs(fMaxPrice-fBasePrice),fabs(fMinPrice-fBasePrice));
	fMaxDelta=max(fMaxDelta,fBasePrice*(m_bZs?0.01:0.004)); //将指数的最小绝对范围改为1%
	if(g_bTPAxis) //如果是TP坐标,需重新计算fMaxDelta和fMaxDelta
	{
		fMaxDelta = max(fMaxDelta,GetTPPrice(m_pStkInfo,m_Hq.Close,TRUE)-fBasePrice);
		fMaxDelta = max(fMaxDelta,GetTPPrice(m_pStkInfo,m_Hq.Close,FALSE)-fBasePrice);
	}
	if(m_nFirstShow>0) 
		m_pXTick[0].Flag |= DRAW_TIME;
}

int UTickZst::GetXTickPosFromX(double x)
{
	int nPos = min(int((float)(x-xStart-fPointPerXTick+0.5f)/fPointPerXTick), m_nXTickNum-1);
	return max(0,nPos);
}

int UTickZst::GetXTickTimeFromX(double x)
{
	int XTickPos = GetXTickPosFromX(x);
	if(m_pXTick[XTickPos].nRealTickDataXh>=0)
		return  m_pXTick[XTickPos].nMinute;
	return -1;
}

int UTickZst::GetYFromValue(double value)	//根据价位得到Y坐标
{
	return (int)((yVol-yStart)*(fMaxDelta+fBasePrice-value)/(2*fMaxDelta)+yStart);
}

int UTickZst::GetYVolFromValue(double value,BOOL bVolume)
{
	if(bVolume)	return((int)(yEndVol-(yEndVol-yVol)*value/lMaxVol));
	return((int)(yEndVol+yVol-(yEndVol-yVol)*
		(value-lMinVolInStock)/(lMaxVolInStock-lMinVolInStock))/2);
}

int UTickZst::GetYLbFromValue(double value)
{
	return yEndBot-(yEndBot-yEndVol)*(value-fBotMinValue)/(fBotMaxValue-fBotMinValue);
}

int UTickZst::GetYMmldFromValue(long value)
{
	return yEndBot-(yEndBot-yEndVol)*value/(nBotMaxValue);
}

int UTickZst::GetYDiffFromValue(long value) //value有正负之分,为buyv-sellv
{
	value/=2;	//为了共用坐标，/2
	return (yEndBot+yEndVol)/2-(yEndBot-yEndVol)*value/(nBotMaxValue);
}

//一般情况，当前为-1,其它日期的数据按0,1,2
int UTickZst::GetXFromXTick(int xh,int flag)	//根据绝对分钟数求X坐标
{
	int ntmp = xh+1;
	ntmp=min(nSecLens,ntmp);
	return xStart+(int)(ntmp*fPointPerXTick+0.5f);
}

void UTickZst::DrawFrame(CDC *pDC)
{
	CSize size;
	int i;
	//画边框线，并计算yStart,xEnd等尺寸
	yStart = 20;
	xStart  = TICKZST_LWIDTH;
	yEndBot = max(yStart,m_rcIn.bottom-TICKZST_BHEIGHT);
	xEnd	= max(TICKZST_LWIDTH,m_rcIn.right-TICKZST_RWIDTH);
	g_d.DrawLine(pDC,xStart,yStart,xEnd,yStart);
	g_d.DrawLine(pDC,xStart,yStart,xStart,yEndBot);
	g_d.DrawLine(pDC,xEnd,0,xEnd,m_rcIn.bottom);
	g_d.DrawLine(pDC,0,yEndBot,m_rcIn.right,yEndBot);
	
	yVol   =yStart+TickZstRatio[0]*(yEndBot-yStart);
	yEndVol=yEndBot;
	yBasePrice = (yVol-yStart)/2+yStart;
	nPriceLnum=max(1,min(7,(yVol-yStart)/32));
	nVolLnum  =max(1,min(8,(yEndVol-yVol)/18));
	nBotLnum  =max(1,min(5,(yEndBot-yEndVol)/18));
	
	//画横轴线
	for(i=1;i<=nPriceLnum;i++)
		g_d.DrawLine(pDC,xStart,yStart+i*(yBasePrice-yStart)/nPriceLnum,xEnd,yStart+i*(yBasePrice-yStart)/nPriceLnum);
	g_d.DrawLine(pDC,xStart,yBasePrice+1,xEnd,yBasePrice+1);
	for(i=1;i<=nPriceLnum;i++)
		g_d.DrawLine(pDC,xStart,yBasePrice+i*(yVol-yBasePrice)/nPriceLnum,xEnd,yBasePrice+i*(yVol-yBasePrice)/nPriceLnum);
	g_d.DrawLine(pDC,xStart,yVol+1,xEnd,yVol+1);
	for(i=1;i<=nVolLnum;i++)
		g_d.DrawLine(pDC,xStart,yVol+i*(yEndVol-yVol)/nVolLnum,xEnd,yVol+i*(yEndVol-yVol)/nVolLnum);
	short *stmp = TimeSec;
	nSecLens = m_nXTickNum+1;
	fPointPerXTick = (float)(xEnd-xStart)/(float)nSecLens; //时间轴的单元
	//画标题区，分几种风格
	int   pos=xStart+2,XWidth=40;
	size = g_d.DisplayText(pDC,pos-2,3,VipColor.TXTCOLOR,"%8s ",m_pStkInfo->Name);
	pDC->SetBkMode(TRANSPARENT);
		
	pos+=size.cx+1;
	if(m_bManyDayZST) //显示多日走势图
		size = g_d.DisplayText(pDC,pos,3,VipColor.ZBCOLOR[0],_F("最近%d日闪电图"),m_nDays+1);
	else
		size = g_d.DisplayText(pDC,pos,3,VipColor.ZBCOLOR[0],_F("闪电图"));
	pos+=size.cx+4;
	if(!m_bZs || (m_bZs && g_GH.bZsShowAverage))
	{
		size = g_d.DisplayText(pDC,pos,3,VipColor.ZBCOLOR[1],_F("均线")); pos+=size.cx+4;
	}
	if(Width() > 300)
	{
		if(g_GH.bDiffVolColor)
		{
			if(m_bZs)
			{
				if(g_bDiffVolMode==0)
				{
					DC_DRAW_TEXT(KPCOLOR,"成",0);
					DC_DRAW_TEXT(DOWNCOLOR,"交",0);
				}
				else
				{
					DC_DRAW_TEXT(VOLCOLOR,"成",0);
					DC_DRAW_TEXT(KNCOLOR,"交",0);
				}
				DC_DRAW_TEXT(LEVELCOLOR,"额(万元)",4);
			}
			else
			{
				if(g_bDiffVolMode==0)
				{
					DC_DRAW_TEXT(KPCOLOR,"成",0);
					DC_DRAW_TEXT(DOWNCOLOR,"交",0);
				}
				else
				{
					DC_DRAW_TEXT(VOLCOLOR,"成",0);
					DC_DRAW_TEXT(KNCOLOR,"交",0);
				}
				DC_DRAW_TEXT(LEVELCOLOR,"量",4);
			}
		}
		else
		{
			DC_DRAW_TEXT(VOLCOLOR,m_bZs?"成交额(万元)":"成交量",4);
		}
		if(m_pStkInfo->bigmarket==QH_TYPE)
		{
			size = g_d.DisplayText(pDC,pos,3,VipColor.LEVELCOLOR,_F("持仓量"));
			pos+=size.cx+4;
		}
		if(m_lDate == 0)
		{
			if(m_bYestodayFalg && m_HqEx.HqDate>0 && m_bDSFlag)
			{
				size = g_d.DisplayText(pDC,pos,3,GridColor.Grid_Other_Color,"Date:%08d",m_HqEx.HqDate);
				pos+=size.cx+4;
			}
		}
		if(Width() > 400)
			g_d.DisplayText(pDC,m_rcIn.right-185,3,VipColor.TXTCOLOR,_F("Home键放大 End键缩小"));
	}
}

void UTickZst::DrawIt(CDC *pDC)
{
	//如果空间太小，不画
	if( Width() < TICKZST_LWIDTH+TICKZST_RWIDTH+20 || Height() < TICKZST_BHEIGHT+20+20)
		return;
	if(!m_pXTick) CalcIt();
	DrawFrame(pDC);
	if(fBasePrice < 0.001) return;//如果fBasePrice为0,则只画框架
	
	DrawAxisArea(pDC,CPoint(0,yEndBot),0);
	DrawAxisArea(pDC,CPoint(xEnd,0),2);
	DrawAxisArea(pDC,CPoint(0,0),1);
	//开始画线
	int *ntmp1=new int[m_nXTickNum]; int *ntmp2=new int[m_nXTickNum];
	memset(ntmp1, 0, m_nXTickNum*sizeof(int)); memset(ntmp2, 0, m_nXTickNum*sizeof(int));
	CPen *oldpen = g_d.SelectPen(pDC,ZB1_PEN);
	for(int i=0,j=0;i<m_nXTickNum-1;i++)
	{
		if(m_pXTick[i].nRealTickDataXh<0) continue;
		ntmp1[j] = GetXFromXTick(i);
		for(int ii=i+1;ii<m_nXTickNum&&m_pXTick[ii].nRealTickDataXh<0;ii++);
		if(ii==m_nXTickNum) break;
		ntmp2[j] = GetXFromXTick(ii);
		if(ntmp1[j] > ntmp2[j])  //及时修正，防止反画，不能是>=
		{
			ntmp1[j] = ntmp2[j];
			ntmp1[j] = max(xStart,ntmp1[j]);
			ntmp1[j] = min(xEnd,ntmp1[j]);
			ntmp2[j] = ntmp1[j];
		}
		//画价线
		if(m_pXTick[i].nDataDate==m_pXTick[ii].nDataDate)
			g_d.DrawLine(pDC,ntmp1[j],GetYFromValue(GetAbsPrice(m_pStkInfo,m_pXTick[i].Now,m_XsFlag)),
					 ntmp2[j],GetYFromValue(GetAbsPrice(m_pStkInfo,m_pXTick[ii].Now,m_XsFlag))
					);
		j++;
	}
	if(j>0) ntmp1[j] = ntmp2[j-1];
	g_d.SelectPen(pDC,ZB2_PEN);

	for(i=j=0;i<m_nXTickNum-1;i++)
	{
		if(m_pXTick[i].nRealTickDataXh<0) continue;
		for(int ii=i+1;ii<m_nXTickNum&&m_pXTick[ii].nRealTickDataXh<0;ii++);
		if(ii==m_nXTickNum) break;
		if(!m_bZs && m_pXTick[i].Average>0 && m_pXTick[ii].Average>0)
		{
			//画均价线
			if(m_pXTick[i].nDataDate==m_pXTick[ii].nDataDate)
				g_d.DrawLine(pDC,ntmp1[j],GetYFromValue(GetAbsPrice(m_pStkInfo,m_pXTick[i].Average,m_XsFlag)),
				ntmp2[j],GetYFromValue(GetAbsPrice(m_pStkInfo,m_pXTick[ii].Average ,m_XsFlag))
				);
		}
		j++;
	}

	if(!g_GH.bDiffVolColor)	//如果没有颜色区分的成交量显示
		g_d.SelectPen(pDC,VOL_PEN);
	for(i=j=0;i<m_nXTickNum;i++)
	{
		if(m_pXTick[i].nRealTickDataXh<0) continue;
		for(int ii=i-1;ii>=0&&m_pXTick[ii].nRealTickDataXh<0;ii--);
		if(ii<=0) ii=i;
		if(g_GH.bDiffVolColor)
		{
			if(g_bDiffVolMode==0)
			{
				//画成交量
				if( (m_pXTick[i].nRealTickDataXh==0 && GetAbsPrice(m_pStkInfo,m_pXTick[i].Now,m_XsFlag) > m_Hq.Close) || 
					(m_pXTick[i].nRealTickDataXh>0 && GetAbsPrice(m_pStkInfo,m_pXTick[i].Now,m_XsFlag) > GetAbsPrice(m_pStkInfo,m_pXTick[ii].Now,m_XsFlag)) )
					g_d.SelectPen(pDC,KP_PEN);
				else if( (m_pXTick[i].nRealTickDataXh==0 && GetAbsPrice(m_pStkInfo,m_pXTick[i].Now,m_XsFlag) < m_Hq.Close) ||
					(m_pXTick[i].nRealTickDataXh>0 && GetAbsPrice(m_pStkInfo,m_pXTick[i].Now,m_XsFlag)< GetAbsPrice(m_pStkInfo,m_pXTick[ii].Now,m_XsFlag)))
					g_d.SelectPen(pDC,DOWN_PEN);
				else
					g_d.SelectPen(pDC,LEVEL_PEN);
			}
			else
			{
				//画成交量
				if( (m_pXTick[i].nRealTickDataXh==0 && GetAbsPrice(m_pStkInfo,m_pXTick[i].Now,m_XsFlag) > m_Hq.Close) || 
					(m_pXTick[i].nRealTickDataXh>0 && GetAbsPrice(m_pStkInfo,m_pXTick[i].Now,m_XsFlag) > GetAbsPrice(m_pStkInfo,m_pXTick[ii].Now,m_XsFlag)) )
					g_d.SelectPen(pDC,VOL_PEN);
				else if( (m_pXTick[i].nRealTickDataXh==0 && GetAbsPrice(m_pStkInfo,m_pXTick[i].Now,m_XsFlag) < m_Hq.Close) ||
					(m_pXTick[i].nRealTickDataXh>0 && GetAbsPrice(m_pStkInfo,m_pXTick[i].Now,m_XsFlag) < GetAbsPrice(m_pStkInfo,m_pXTick[ii].Now,m_XsFlag)))
					g_d.SelectPen(pDC,KN_PEN);
				else
					g_d.SelectPen(pDC,LEVEL_PEN);
			}
		}
		g_d.DrawLine(pDC,ntmp1[j],GetYVolFromValue(m_pXTick[i].NowVol),ntmp1[j],yEndVol);
		j++;
	}
	g_d.RestorePen(pDC,oldpen);

	if(m_bDSFlag)
	{
		if(m_pStkInfo->bigmarket==QH_TYPE)				//持仓量
		{
			g_d.SelectPen(pDC,LEVEL_PEN);
			for(int i=0,j=0;i<m_nXTickNum-1;i++)
			{
				if(m_pXTick[i].nRealTickDataXh<0) continue;
				ntmp1[j] = GetXFromXTick(i);
				for(int ii=i+1;ii<m_nXTickNum&&m_pXTick[ii].nRealTickDataXh<0;ii++);
				if(ii==m_nXTickNum) break;
				ntmp2[j] = GetXFromXTick(ii);
				if(ntmp1[j] > ntmp2[j])
				{
					ntmp1[j] = ntmp2[j];
					ntmp1[j] = max(xStart,ntmp1[j]);
					ntmp1[j] = min(xEnd,ntmp1[j]);
					ntmp2[j] = ntmp1[j];
				}
				g_d.DrawLine(pDC,ntmp1[j],GetYVolFromValue(m_pXTick[i].VolInStock,FALSE),
						 ntmp2[j],GetYVolFromValue(m_pXTick[ii].VolInStock,FALSE));
				j++;
			}
			if(j>0) ntmp1[j] = ntmp2[j-1];
			g_d.RestorePen(pDC,oldpen);
		}
	}

	TDEL(ntmp1); TDEL(ntmp2);
}

//nFlag 0左,1右,2最左,3最右
void UTickZst::ToNextMin(int nFlag,int nSpace)
{
	if(!m_bCrossMode)
	{
		m_bCrossMode = TRUE;
		m_bHasCross = TRUE;
		ProcessMsg(WM_PAINT);
	}
	CPoint pt;
	int EndPos = xEnd;
	int nCurPos = m_nXTickNum-1;
	if(m_bCrossMode) nCurPos = GetXTickPosFromX(m_CrossPoint.x);
	if(nFlag==2)		//最左
		pt.x = xStart+fPointPerXTick;
	else if(nFlag==3)	//最右
		pt.x = EndPos-fPointPerXTick;
	else if(nFlag==0)	//左
	{
		if(m_CrossPoint==CPoint(0,0))
			pt.x = EndPos-max(1,nSpace*fPointPerXTick);
		else
		{
			pt.x = GetXFromXTick(nCurPos-max(1, nSpace));
			if(pt.x==m_CrossPoint.x)
				pt.x = m_CrossPoint.x-max(1,nSpace*fPointPerXTick);
		}
	}
	else if(nFlag==1)	//右
	{
		if(m_CrossPoint==CPoint(0,0))
			pt.x = xStart+max(1,nSpace*fPointPerXTick);
		else
		{
			pt.x = GetXFromXTick(nCurPos+max(1, nSpace));
			if(pt.x==m_CrossPoint.x)
				pt.x = m_CrossPoint.x+max(1,nSpace*fPointPerXTick);
		}
	}
	if(pt.x < int(xStart+fPointPerXTick)) 
	{
		if(m_nFirstShow>0) 
		{
			m_nFirstShow-=nSpace;
			m_nFirstShow = max(0, m_nFirstShow);
			CalcIt();
			ProcessMsg(WM_PAINT);
			pt.x = int(xStart+fPointPerXTick);
		}
		else if(m_nFirstShow==0&&m_nShowNum>=m_nTotalXTick) 
			pt.x = EndPos-fPointPerXTick;
		else pt.x = int(xStart+fPointPerXTick);
	}
	if(pt.x > int(EndPos-fPointPerXTick)) 
	{
		if(m_nFirstShow+m_nShowNum<m_nTotalXTick) 
		{
			m_nFirstShow+=nSpace;
			m_nFirstShow = min(m_nFirstShow, m_nTotalXTick-m_nShowNum);
			CalcIt();
			ProcessMsg(WM_PAINT);
			pt.x = int(EndPos-fPointPerXTick);
		}
		else if(m_nFirstShow==0&&m_nShowNum>=m_nTotalXTick)
			pt.x = xStart+fPointPerXTick;
		else pt.x = int(EndPos-fPointPerXTick);
	}
	int nPos = GetXTickPosFromX(pt.x);
	pt.y = GetYFromValue(GetAbsPrice(m_pStkInfo,m_pXTick[nPos].Now,m_XsFlag));
	InSwimArea(pt);
}

void UTickZst::InSwimArea(CPoint pt)
{
	//防止绘图越界
	CPoint topleft(xStart, yStart), rightbuttom(xEnd,yEndBot);
	if(!m_rcIn.PtInRect(topleft)||!m_rcIn.PtInRect(rightbuttom)) return;	

	if(m_bCrossMode)
	{
		CDC *pDC = m_pView->GetDC();
		int oldrop = pDC->SetROP2(R2_XORPEN);
		int nLeft = left, nTop = top;
		if(m_CrossPoint!=CPoint(0,0))
		{
			//为数值说明作准备
			g_d.DrawLine(pDC,nLeft+m_CrossPoint.x,nTop+yStart,nLeft+m_CrossPoint.x,nTop+yEndVol,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
			if(yEndBot > yEndVol)
				g_d.DrawLine(pDC,nLeft+m_CrossPoint.x,nTop+yStart,nLeft+m_CrossPoint.x,nTop+yEndBot,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
			if(m_CrossPoint.y<yEndVol || m_CrossPoint.y>yStart)
				g_d.DrawLine(pDC,nLeft+xStart, nTop+m_CrossPoint.y, nLeft+xEnd, nTop+m_CrossPoint.y,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
		}
		//为数值说明作准备
		g_d.DrawLine(pDC,nLeft+pt.x,nTop+yStart,nLeft+pt.x,nTop+yEndVol,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
		if(yEndBot > yEndVol)
			g_d.DrawLine(pDC,nLeft+pt.x,nTop+yStart,nLeft+pt.x,nTop+yEndBot,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
		if(pt.y<yEndVol || pt.y>yStart)
			g_d.DrawLine(pDC,nLeft+xStart, nTop+pt.y, nLeft+xEnd, nTop+pt.y,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
		m_CrossPoint = pt;
		pDC->SetROP2(oldrop);
		DrawInfoArea(pt.x);
		m_bHasCross = TRUE;
	}
	LonelyDrawAxis(0,TRUE,pt.x);
	LonelyDrawAxis(2,TRUE,pt.y);
	m_bInSwim = TRUE;
}

void UTickZst::LeaveSwimArea()
{
	if(m_bCrossMode && m_bHasCross)
	{
		LonelyDrawAxis(1);
		if(m_CrossPoint != CPoint(0,0))
		{
			CDC *pDC = m_pView->GetDC();
			int oldrop = pDC->SetROP2(R2_XORPEN);
			g_d.DrawLine(pDC,left+m_CrossPoint.x,top+yStart,left+m_CrossPoint.x,top+yEndBot,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
			pDC->SetROP2(oldrop);
			m_pView->ReleaseDC(pDC);
		}
		m_CrossPoint=CPoint(0,0);
		m_bHasCross = FALSE;
	}
	else if(!m_bCrossMode && m_bInSwim)
	{
		LonelyDrawAxis(0);
		LonelyDrawAxis(2);
		m_bInSwim = FALSE;
	}
}

//画信息区
void UTickZst::DrawInfoArea(int xPos)
{
	if(fBasePrice < 0.001 || (!m_bDataReady && OnLine)) //如果fBasePrice为0,则不画,如果在线状态，数据没有到，也不画
		return;
	CDC *pDC = m_pView->GetDC();
	CDC m_MyDC;
	CBitmap m_MyBitmap,*m_pOldBitmap;
	m_MyDC.CreateCompatibleDC(pDC);
	m_MyBitmap.CreateCompatibleBitmap(pDC,TICKZST_LWIDTH,Height()-TICKZST_BHEIGHT);
	m_pOldBitmap = m_MyDC.SelectObject(&m_MyBitmap);
	g_d.Bar(&m_MyDC,0,0,TICKZST_LWIDTH,Height()-TICKZST_BHEIGHT,BACK_BRUSH);
	m_MyDC.SetBkMode(TRANSPARENT);
	CFont *oldfont = g_d.SelectFont(&m_MyDC,LIST_FONT);
	CPen  *oldpen = g_d.SelectPen(&m_MyDC,AXIS_PEN);
	int nHeight=2;
	CSize size;
	int XTickPos = GetXTickPosFromX(xPos);
	int tmpXsFlag=m_XsFlag;
	float maxprice=0.0f;
	if(m_pXTick[XTickPos].nRealTickDataXh>=0)
		maxprice=max(GetAbsPrice(m_pStkInfo,m_pXTick[XTickPos].Now,m_XsFlag),
		GetAbsPrice(m_pStkInfo,m_pXTick[XTickPos].Average,m_XsFlag));
	if(maxprice>=100000.0)		tmpXsFlag=0;
	else if(maxprice>=10000.0)	tmpXsFlag=1;

	BOOL bHisInfo = FALSE;
	int  whichHis = 0;
	TICKZSTUNIT tmpTickUnit = m_pXTick[XTickPos];
	float tmpClose = m_Hq.Close;
	long  tmpDate = g_lOpenRq;
	if(m_bManyDayZST && tmpTickUnit.nDataDate!=g_lOpenRq)
	{
		bHisInfo = TRUE;
		for(int i=1;i<MAX_ZST_MANYDAY&&m_ManyDayDate[i]!=tmpTickUnit.nDataDate;i++);
		whichHis = min(i, MAX_ZST_MANYDAY-1);
		tmpClose = m_fManyDayClose[whichHis];
		tmpDate =  m_ManyDayDate[whichHis];
	}
	float tmpNow = GetAbsPrice(m_pStkInfo,tmpTickUnit.Now,m_XsFlag);
	DWORD tmpNowVol = tmpTickUnit.NowVol;
	if(m_bManyDayZST)
	{
		size=g_d.DisplayText(&m_MyDC,1,nHeight,VipColor.TXTCOLOR,0,_F("日期"));nHeight+=size.cy+2;
		size=g_d.DisplayText(&m_MyDC,0,nHeight,VipColor.LEVELCOLOR,TICKZST_LWIDTH,GetTdxDateStr(tmpDate));nHeight+=size.cy+2;
	}
	size=g_d.DisplayText(&m_MyDC,1,nHeight,VipColor.TXTCOLOR,0,_F("时间"));nHeight+=size.cy+2;
	size=g_d.DisplayText(&m_MyDC,0,nHeight,VipColor.LEVELCOLOR,TICKZST_LWIDTH,GetTdxTimeStr(tmpTickUnit.nMinute));nHeight+=size.cy+2;

	size=g_d.DisplayText(&m_MyDC,1,nHeight,VipColor.TXTCOLOR,0,_F("价位"));nHeight+=size.cy+2;
	size=g_d.BjPrintf(&m_MyDC,0,nHeight,tmpNow,tmpClose,tmpXsFlag,TICKZST_LWIDTH);nHeight+=size.cy+2;

	size=g_d.DisplayText(&m_MyDC,1,nHeight,VipColor.TXTCOLOR,0,_F("均价"));nHeight+=size.cy+2;
	size=g_d.BjPrintf(&m_MyDC,0,nHeight,GetAbsPrice(m_pStkInfo,tmpTickUnit.Average,m_XsFlag),tmpClose,tmpXsFlag,TICKZST_LWIDTH);nHeight+=size.cy+2;

	size=g_d.DisplayText(&m_MyDC,1,nHeight,VipColor.TXTCOLOR,0,_F("涨跌"));nHeight+=size.cy+2;
	if(tmpNow!=0.0)
		size=g_d.BjPrintf(&m_MyDC,0,nHeight,tmpNow-tmpClose,0.0,m_XsFlag,TICKZST_LWIDTH);
	else
		size=g_d.BjPrintf(&m_MyDC,0,nHeight,0.0,0.0,m_XsFlag,TICKZST_LWIDTH,0);
	nHeight+=size.cy+2;
	size=g_d.DisplayText(&m_MyDC,1,nHeight,VipColor.TXTCOLOR,0,m_bZs?_F("成交额"):_F("成交量"));nHeight+=size.cy+2;
	size=g_d.DisplayText(&m_MyDC,0,nHeight,VipColor.VOLCOLOR,TICKZST_LWIDTH,"%9d",tmpNowVol);nHeight+=size.cy+2;
	if(m_pStkInfo->bigmarket==QH_TYPE)
	{
		size=g_d.DisplayText(&m_MyDC,1,nHeight,VipColor.TXTCOLOR,0,_F("持仓量"));nHeight+=size.cy+2;
		size=g_d.DisplayText(&m_MyDC,0,nHeight,VipColor.VOLCOLOR,TICKZST_LWIDTH,"%9d",tmpTickUnit.VolInStock);nHeight+=size.cy+2;
	}

	g_d.DrawRect(&m_MyDC,0,0,TICKZST_LWIDTH-1,nHeight-1);
	g_d.RestorePen(&m_MyDC,oldpen);
	g_d.RestoreFont(&m_MyDC,oldfont);
	pDC->BitBlt (left,top,TICKZST_LWIDTH,nHeight,&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC*/
	m_MyDC.SelectObject(m_pOldBitmap);
	m_MyBitmap.DeleteObject();
	m_MyDC.DeleteDC();
	m_pView->ReleaseDC(pDC);
}

//单独画轴
void UTickZst::LonelyDrawAxis(int nWhich,BOOL bSwim,int DrawPos)
{
	if(fBasePrice < 0.001) //如果fBasePrice为0,则不画
		return;
	CRect tmpRect;
	if(nWhich==0)		tmpRect.SetRect(0,0,Width()-TICKZST_RWIDTH,TICKZST_BHEIGHT);
	else if(nWhich==1)	tmpRect.SetRect(0,0,TICKZST_LWIDTH,Height()-TICKZST_BHEIGHT);
	else				tmpRect.SetRect(0,0,TICKZST_RWIDTH,Height()-TICKZST_BHEIGHT);
	CDC *pDC = m_pView->GetDC();
	CDC m_MyDC;
	CBitmap m_MyBitmap,*m_pOldBitmap;
	m_MyDC.CreateCompatibleDC(pDC);
	m_MyBitmap.CreateCompatibleBitmap(pDC,tmpRect.Width(),tmpRect.Height());
	m_pOldBitmap = m_MyDC.SelectObject(&m_MyBitmap);
	g_d.Bar(&m_MyDC,&tmpRect,BACK_BRUSH);
	m_MyDC.SetBkMode(TRANSPARENT);

	CFont *oldfont = g_d.SelectFont(&m_MyDC,GRAPH_FONT);
	CPen  *oldpen = g_d.SelectPen(&m_MyDC,AXIS_PEN);

	if(nWhich==0)
		DrawAxisArea(&m_MyDC,CPoint(0,-1),nWhich,bSwim,DrawPos);
	else if(nWhich==1)
		DrawAxisArea(&m_MyDC,CPoint(0,0),nWhich,bSwim,DrawPos);
	else 
		DrawAxisArea(&m_MyDC,CPoint(-1,0),nWhich,bSwim,DrawPos);

	g_d.RestorePen(&m_MyDC,oldpen);
	g_d.RestoreFont(&m_MyDC,oldfont);

	if(nWhich==0)
		pDC->BitBlt (left,top+yEndBot+1,Width(),TICKZST_BHEIGHT-1,&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC*/
	else if(nWhich==1)
		pDC->BitBlt (left,top,TICKZST_LWIDTH,Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC*/
	else
		pDC->BitBlt (left+xEnd+1,top,TICKZST_RWIDTH-1,Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC*/
		 
	m_MyDC.SelectObject(m_pOldBitmap);
	m_MyBitmap.DeleteObject();
	m_MyDC.DeleteDC();
	m_pView->ReleaseDC(pDC);
}

//nWhich=0:横坐标 1:左纵坐标 2:右纵坐标 3:小图的右纵坐标, 
//RelaPoint为相对坐标点,DrawPos为Unit坐标点(nWhich=0表示x,其它为y)
void UTickZst::DrawAxisArea(CDC *pDC,CPoint RelPt,int nWhich,BOOL bSwim,int DrawPos)
{
	if(nWhich == 0)	//横坐标
	{
		CSize size=pDC->GetTextExtent("00:00");
		for(int i=0,j=0;i<m_nXTickNum;i++)
		{
			int nXPos = RelPt.x+xStart+(i+1)*fPointPerXTick;
			if(m_bManyDayZST &&
				(i==0||m_pXTick[i].nDataDate!=m_pXTick[i-1].nDataDate))
			{
				if(m_pXTick[i].nDataDate!=g_lOpenRq) 
					g_d.DisplayText(pDC,nXPos-size.cx,RelPt.y,VipColor.TIMECOLOR,GetTdxDateStr(m_pXTick[i].nDataDate)); //显示日期
				if(i>0) g_d.DrawLine(pDC,nXPos,yStart,nXPos,yEndVol,VipColor.AXISCOLOR,PS_SOLID,2);
			}
			if(m_pXTick[i].Flag&CRITICAL_MIN)
			{
				g_d.DotLine(pDC,nXPos,yStart,nXPos,yEndVol,VipColor.AXISCOLOR,2);
				for(int j=0;j<2*GetTdxSectNum(TimeSec);j+=2)
				{
					if(m_pXTick[i].nMinute==TimeSec[j]&&j>0)
						g_d.DrawLine(pDC,nXPos,yStart,nXPos,yEndVol,VipColor.AXISCOLOR,PS_SOLID,1);
				}
			}
			if(m_pXTick[i].Flag&DRAW_TIME) 
				g_d.DisplayText(pDC,nXPos-size.cx,RelPt.y,VipColor.TIMECOLOR,GetTdxTimeStr(m_pXTick[i].nMinute));
		}
		int nPos = GetXTickPosFromX(DrawPos);
		if(nPos>0 && m_pXTick[nPos].nRealTickDataXh>=0 && bSwim && (m_bDataReady || !OnLine))
			g_d.DrawSwim(pDC,CRect(RelPt.x+DrawPos-size.cx-2,RelPt.y,RelPt.x+DrawPos,RelPt.y+TICKZST_BHEIGHT),GetTdxTimeStr(m_pXTick[nPos].nMinute));
	}
	else
	{
		int i,tmpxsflag = m_XsFlag;
		if(m_bZs) tmpxsflag = 0;
		double fPer = fMaxDelta/nPriceLnum;
		int	   nPer = lMaxVol/nVolLnum;
		int	   nPerVi = (lMaxVolInStock-lMinVolInStock)/nVolLnum;
		float  fLBPer = (fBotMaxValue-fBotMinValue)/nBotLnum;
		int    nMmldPer = nBotMaxValue/nBotLnum;
		if(nWhich==3)	//小图的右纵坐标
		{
			g_d.BjPrintf(pDC,RelPt.x,RelPt.y+yBasePrice-7,fBasePrice,m_Hq.Close,tmpxsflag,0);
			for(i=0;i<nPriceLnum;i++)
				g_d.BjPrintf(pDC,RelPt.x,RelPt.y+yStart+i*(yBasePrice-yStart)/nPriceLnum-7,fBasePrice+fPer*(nPriceLnum-i),m_Hq.Close,tmpxsflag,0);
			for(i=1;i<nPriceLnum;i++)
				g_d.BjPrintf(pDC,RelPt.x,RelPt.y+yBasePrice+i*(yVol-yBasePrice)/nPriceLnum-7,fBasePrice-fPer*i,m_Hq.Close,tmpxsflag,0);
			for(i=1;i<=nVolLnum;i++)
				g_d.DisplayText(pDC,RelPt.x+2,RelPt.y+yEndVol-i*(yEndVol-yVol)/nVolLnum-7,VipColor.VOLCOLOR,0,"%9d",nPer*i);
			return;
		}
		//左右纵坐标
		if(nWhich == 1)
			g_d.BjPrintf(pDC,RelPt.x,RelPt.y+yBasePrice-7,fBasePrice,m_Hq.Close,tmpxsflag,TICKZST_LWIDTH-2);
		else
			g_d.DisplayText(pDC,RelPt.x+2,RelPt.y+yBasePrice-7,VipColor.LEVELCOLOR,0,"0.00%%");
		for(i=0;i<nPriceLnum;i++)
		{
			if(nWhich == 1)
				g_d.BjPrintf(pDC,RelPt.x,RelPt.y+yStart+i*(yBasePrice-yStart)/nPriceLnum-7,fBasePrice+fPer*(nPriceLnum-i),m_Hq.Close,tmpxsflag,TICKZST_LWIDTH-2);
			else
				g_d.DisplayText(pDC,RelPt.x+2,RelPt.y+yStart+i*(yBasePrice-yStart)/nPriceLnum-7,VipColor.KPCOLOR,0,"%6.2f%%",(fPer*(nPriceLnum-i)*100/*-0.5*/)/fBasePrice);//减0.5
		}
		for(i=1;i<nPriceLnum;i++)
		{
			if(nWhich == 1)
				g_d.BjPrintf(pDC,RelPt.x,RelPt.y+yBasePrice+i*(yVol-yBasePrice)/nPriceLnum-7,fBasePrice-fPer*i,m_Hq.Close,tmpxsflag,TICKZST_LWIDTH-2);
			else
				g_d.DisplayText(pDC,RelPt.x+2,RelPt.y+yBasePrice+i*(yVol-yBasePrice)/nPriceLnum-7,VipColor.DOWNCOLOR,0,"%6.2f%%",(fPer*i*100/*-0.5*/)/fBasePrice);//减0.5
		}
		for(i=1;i<=nVolLnum;i++)
		{
			if(nWhich == 1)
				g_d.DisplayText(pDC,RelPt.x,RelPt.y+yEndVol-i*(yEndVol-yVol)/nVolLnum-7,VipColor.VOLCOLOR,TICKZST_LWIDTH-2,MakeVol2(nPer*i));
			else
			{
				if(m_pStkInfo->bigmarket==QH_TYPE)
					g_d.DisplayText(pDC,RelPt.x+2,RelPt.y+yEndVol-i*(yEndVol-yVol)/nVolLnum-7,VipColor.LEVELCOLOR,0,MakeVol2(lMinVolInStock+nPerVi*i));
				else g_d.DisplayText(pDC,RelPt.x+2,RelPt.y+yEndVol-i*(yEndVol-yVol)/nVolLnum-7,VipColor.VOLCOLOR,0,MakeVol2(nPer*i));
			}
		}
		if(bSwim && (m_bDataReady || !OnLine))
		{
			char Content[50];
			if(m_XsFlag<2)
				sprintf(Content,"%9.0f",fBasePrice+fMaxDelta/(yBasePrice-yStart)*(yBasePrice-DrawPos));
			else if(DrawPos <= yVol)
				sprintf(Content,m_XsFlag==3?"%9.3f":"%9.2f",fBasePrice+fMaxDelta/(yBasePrice-yStart)*(yBasePrice-DrawPos));
			else if(DrawPos <= yEndVol)
				sprintf(Content,"%9.0f",lMaxVol*1.0/(yEndVol-yVol)*(yEndVol-DrawPos));
			else if(DrawPos < yEndVol)
				Content[0]=0;				
			else
				sprintf(Content,"%9.0f",(float)nBotMaxValue/(yEndBot-yEndVol)*(yEndBot-DrawPos));
			AllTrim(Content);
			if(Content[0]!=0 && nWhich==1)	//左纵轴游标
				g_d.DrawSwim(pDC,CRect(RelPt.x,RelPt.y+DrawPos-14,RelPt.x+TICKZST_LWIDTH,RelPt.y+DrawPos),Content);
			if(Content[0]!=0 && nWhich==2)	//右纵轴游标
				g_d.DrawSwim(pDC,CRect(RelPt.x,RelPt.y+DrawPos-14,RelPt.x+TICKZST_RWIDTH,RelPt.y+DrawPos),Content);
		}
	}
}

void UTickZst::GetCriticalTime(long *pCriticalTime, short *fz, short nCriSecNum)
{
	int nTotalMin = GetRealTotalMinute(fz);
	int nMinPerSec = nTotalMin/nCriSecNum;
	pCriticalTime[0] = fz[0];
	for(int i=1,j=1;i<=nCriSecNum&&j<2*GetTdxSectNum(fz);i++)
	{
		if(pCriticalTime[i-1]+nMinPerSec>=fz[j])
		{
			if(j%2&&j!=2*GetTdxSectNum(fz)-1)
				pCriticalTime[i] = fz[++j];
			else pCriticalTime[i] = fz[j];
			j++;
		}
		else pCriticalTime[i] = pCriticalTime[i-1]+nMinPerSec;
	}
}

BOOL UTickZst::BeCriticalTime(long nCurMin, long &nNextMin)
{
	nNextMin=nCurMin+1;
	for(int i=0;i<8;i++)
	{
		if(nCurMin==TimeSec[i])
		{
			if(i%2&&i<2*GetTdxSectNum(TimeSec)-1) 
				nNextMin = TimeSec[(i+1)%(2*GetTdxSectNum(TimeSec))];
			return TRUE;
		}
	}
	for(i=0;i<=8;i++)
	{
		if(m_CriticalTime[i]>=0&&nCurMin==m_CriticalTime[i])
			return TRUE;
	}
	return FALSE;
}

extern long GetManyDayBackDate(struct AnalyData * AnalyDatap,short nCacheNum,long date);
void UTickZst::ManyDay_ReqData(long lNowRq)
{
	if(m_bManyDayZST && m_pStkInfo)
	{
		struct AnalyData * AnalyDatap = new struct AnalyData[RECENTNUM+50];
		short nCacheNum = ReadCacheData(m_pStkInfo,m_pStkInfo->setcode,AnalyDatap,RECENTNUM,PER_DAY,1);
		if(nCacheNum<1)
			nCacheNum = g_pStockIO->ReadVipDocData(m_pStkInfo,AnalyDatap,RECENTNUM,PER_DAY,1,0);
		long curDate = GetManyDayBackDate(AnalyDatap,nCacheNum,lNowRq);
		for(int i=MAX_ZST_MANYDAY-1;i >= 0;i--)
		{
			m_ManyDayDate[i] = curDate;
			curDate = GetManyDayBackDate(AnalyDatap,nCacheNum,curDate);
		}
		delete AnalyDatap;
	}
}
