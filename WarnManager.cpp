#include "stdafx.h"
#include "WarnManager.h"
#include "MainFrm.h"
#include "WarnGPDlg.h"
#include "Comview.h"
#include "Comview2.h"

CWarnManager::CWarnManager()
{
	m_pHqCache = NULL;
	m_pMainFrame = NULL;
	m_pWarnGPDlg = NULL;
	m_pTJGPDlg = NULL;
}

CWarnManager::~CWarnManager()
{
	TDEL(m_pHqCache);

	TDEL(m_pWarnGPDlg);
	TDEL(m_pTJGPDlg);
}

BOOL CWarnManager::InitWarnManager(CMainFrame *pMainFrame)
{
	m_pMainFrame=pMainFrame;
	return TRUE;
}

void CWarnManager::DeleteDlg(CDialog *tempDialog)
{
	if( tempDialog == (CDialog *)m_pWarnGPDlg )
		m_pWarnGPDlg = NULL;
	if( tempDialog == (CDialog *)m_pTJGPDlg )
		m_pTJGPDlg = NULL;
}

void CWarnManager::OnAllWarngp() 
{
	if(m_pWarnGPDlg != NULL) 
	{
		if (m_pWarnGPDlg->IsIconic()) 
			m_pWarnGPDlg->ShowWindow(SW_RESTORE); 
		return;
	}
	if(!g_bAllWarnSwitch && OnLine)
	{
		if(TDX_MessageBox(m_pMainFrame->m_hWnd,"�г��״ﻹû�д�,���Ƿ����?",MB_YESNO|MB_ICONINFORMATION ) == IDNO)
			return;
		g_bAllWarnSwitch = TRUE;
		GetWarnGpSet();
	}
	m_pWarnGPDlg = new CWarnGPDlg(m_pMainFrame);
	m_pWarnGPDlg->m_nWarnType = WARN_RADAR;
	m_pWarnGPDlg->Create(IDD_WARNGP_DIALOG);
	m_pWarnGPDlg->ShowWindow (SW_SHOW);
}

void CWarnManager::OnTjWarngp() 
{
	if(m_pTJGPDlg != NULL) 
	{
		if (m_pTJGPDlg->IsIconic()) 
			m_pTJGPDlg->ShowWindow(SW_RESTORE); 
		return;
	}
	if(!g_bTjWarnSwitch && OnLine)
	{
		if(TDX_MessageBox(m_pMainFrame->m_hWnd,"����Ԥ����û�д�,���Ƿ����?",MB_YESNO|MB_ICONINFORMATION ) == IDNO)
			return;
		g_bTjWarnSwitch = TRUE;
	}
	m_pTJGPDlg = new CWarnGPDlg(m_pMainFrame);
	m_pTJGPDlg->m_nWarnType = WARN_TJ;
	m_pTJGPDlg->Create(IDD_WARNGP_DIALOG);
	m_pTJGPDlg->ShowWindow (SW_SHOW);
}

void CWarnManager::UpdateWarnGP() 
{
	if(m_pWarnGPDlg)
		m_pWarnGPDlg->PostMessage(UPDATE_WARNGP);
}

void CWarnManager::UpdateTJGP() 
{
	if(m_pTJGPDlg)
		m_pTJGPDlg->PostMessage(UPDATE_WARNGP);	
}

void CWarnManager::StartAllWarn(struct MemStkInfo *info_ptr)
{
	if((!g_bAllWarnSwitch && g_nUnitRadarFlag<1) || !LayoutFlag) return;

	int nStockType = RefreshAllBuf(g_pStockIO->GetIndex(info_ptr->Code,info_ptr->setcode));
	if(nStockType != -1)	//�����Ԥ��
	{
		//��������
		if(g_WarnParam.nBeepFlag==1)
			TDXBeep(0);
		else if(g_WarnParam.nBeepFlag==2)
			::PlaySound(g_WarnParam.WavFile,NULL,SND_ASYNC);
		//��ʼ��ʾ
		if(g_bAllWarnSwitch)
		{
			if(m_pWarnGPDlg)
				m_pWarnGPDlg->PostMessage(UPDATE_WARNGP,nStockType);
			else if(g_WarnParam.bPopup)
			{
				if(m_pMainFrame->IsIconic())	
					m_pMainFrame->ShowWindow(SW_RESTORE); //�������С��״̬����ָ�
				m_pWarnGPDlg = new CWarnGPDlg(m_pMainFrame);
				m_pWarnGPDlg->m_nWarnType = WARN_RADAR;
				m_pWarnGPDlg->Create(IDD_WARNGP_DIALOG);
				m_pWarnGPDlg->ShowWindow (SW_SHOW);
			}
		}
		if(g_nUnitRadarFlag>0 && theViewManager.pActiveView && theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView2)))
			theViewManager.pActiveView->PostMessage(UPDATE_WARNGP);			
	}
}

void CWarnManager::StartTJWatch(struct MemStkInfo *info_ptr)
{
	if(!g_bTjWarnSwitch || !LayoutFlag) return;

	if(!m_pHqCache) 
		m_pHqCache = new WarnHQCache[g_pStockIO->GetStockNum(ALL_ALL)];
	
	long nStockNo = g_pStockIO->GetIndex(info_ptr->Code,info_ptr->setcode);
	int	nConditionNo = InAbsTJBuffer(nStockNo);
	if(nConditionNo == -1)	return;
	
	struct CurrStockData CurrStockInf;
    g_pStockIO->ReadCurrStockData(info_ptr,&CurrStockInf);
	//����ּۣ����գ���һ�μ�λΪ0���򲻱���
	if(CurrStockInf.Now < COMPPREC || CurrStockInf.Close < COMPPREC)
		return;

	CalcPriceWarn(info_ptr, CurrStockInf, nConditionNo);
	CalcTjWarn(info_ptr, nConditionNo);
}

void CWarnManager::CalcPriceWarn(MemStkInfo *pMemStockInf, CurrStockData &CurrStockInf, int nConditionNo)
{
	int		tempReason = -1;
	long nStockNo = g_pStockIO->GetIndex(pMemStockInf->Code,pMemStockInf->setcode);

	if(g_pAbsCondition[nConditionNo].fHighPrice>COMPPREC && CurrStockInf.Now >= g_pAbsCondition[nConditionNo].fHighPrice)
		tempReason = ABSHIGHWARN;	//��λ����
	else
	{
		for(int i=0;i < g_nTJWarnNum;i++)
		{
			if(nStockNo==g_TJWarnBuffer[i].nStockNo && ABSHIGHWARN==g_TJWarnBuffer[i].nReason)
				g_TJWarnBuffer[i].nReason=ABSHIGHWARN2;
		}
	}
	if(g_pAbsCondition[nConditionNo].fLowPrice>COMPPREC && CurrStockInf.Now <= g_pAbsCondition[nConditionNo].fLowPrice)
		tempReason = ABSLOWWARN;	//��λ����
	else
	{
		for(int i=0;i < g_nTJWarnNum;i++)
		{
			if(nStockNo==g_TJWarnBuffer[i].nStockNo && ABSLOWWARN==g_TJWarnBuffer[i].nReason)
				g_TJWarnBuffer[i].nReason=ABSLOWWARN2;
		}
	}
	//����ͻ��(�����ͨ�ɱ�
	if(pMemStockInf->ActiveCapital > 1)
	{
		if(g_pAbsCondition[nConditionNo].TurnOverRate>COMPPREC && 
			(CurrStockInf.Volume*100.0/pMemStockInf->ActiveCapital)>g_pAbsCondition[nConditionNo].TurnOverRate)
			tempReason = ABSHANDCHG;
		else
		{
			for(int i=0;i < g_nTJWarnNum;i++)
			{
				if(nStockNo==g_TJWarnBuffer[i].nStockNo && ABSHANDCHG==g_TJWarnBuffer[i].nReason)
					g_TJWarnBuffer[i].nReason=ABSHANDCHG2;
			}
		}

	}
	//ETF�������
	if(IsETF(pMemStockInf) && CurrStockInf.Volume>0 && g_pAbsCondition[nConditionNo].ETFAPRate>COMPPREC)
	{
		if(CurrStockInf.Now/CurrStockInf.Jjjz*10000-10000>g_pAbsCondition[nConditionNo].ETFAPRate*1.0
			|| 10000-CurrStockInf.Now/CurrStockInf.Jjjz*10000>g_pAbsCondition[nConditionNo].ETFAPRate*1.0)
			tempReason = ABSAPRATE;
		else
		{
			for(int i=0;i < g_nTJWarnNum;i++)
			{
				if(nStockNo==g_TJWarnBuffer[i].nStockNo && ABSAPRATE==g_TJWarnBuffer[i].nReason)
					g_TJWarnBuffer[i].nReason=ABSAPRATE2;
			}
		}
	}
	if(tempReason>0) ProcessPopWarn(pMemStockInf, tempReason, nConditionNo);
}

BOOL CWarnManager::NeedCalcNowKa(short nTjXh, long nIDIndex, CurrStockData *pNewHq)
{
	MemStkInfo *pInfo = (*g_pStockIO)[nIDIndex];
	if(!pInfo) return FALSE;
	BOOL bTimeNeedCalc = TRUE, bHqNeedCalc = TRUE;

	CTime ct = CTime::GetCurrentTime();
	long nCurrMinute = 60*ct.GetHour()+ct.GetMinute();
	if(m_pHqCache&&g_aWarnTjEx[nTjXh].nPeriod<DAY_ST)
	{
		short nFZ[8]={0};
		InitStkTime(pInfo->setcode,nFZ);
		DWORD DataType = CalcPeriodToTdxPeriod(g_aWarnTjEx[nTjXh].nPeriod);
		int nTodayDataNum = GetFZnoFromMinute(nCurrMinute, DataType, nFZ, 1);
		if(nTodayDataNum==GetFZnoFromMinute(m_pHqCache[nIDIndex].nLastCalcMin[nTjXh], DataType, nFZ, 1)) 
			bTimeNeedCalc = FALSE;
	}

	if(m_pHqCache&&fabs(m_pHqCache[nIDIndex].iHq.Now-pNewHq->Now)<COMPPREC&&
		m_pHqCache[nIDIndex].iHq.Volume==pNewHq->Volume&&m_pHqCache[nIDIndex].iHq.ItemNum==pNewHq->ItemNum)
		bHqNeedCalc = FALSE;
	
	if((bTimeNeedCalc||bHqNeedCalc)&&m_pHqCache) 
	{
		m_pHqCache[nIDIndex].nLastCalcMin[nTjXh] = nCurrMinute;
		return TRUE;
	}

	return FALSE;
}

void CWarnManager::CalcTjWarn(MemStkInfo *pInfo, int nConditionNo)
{
	if(!pInfo||!PeekMsg()) 
		return;
	CurrStockData CurrStockInf = {0};
    g_pStockIO->ReadCurrStockData(pInfo,&CurrStockInf);
	long nIDIndex = g_pStockIO->GetIndex(pInfo->Code, pInfo->setcode);
	if(nIDIndex>=DS_FROM)	//��չ�����ݲ�֧������Ԥ��
		return;

	int	 i, j;
	float * fPut[MAX_LINEOUT];
	memset(fPut,0,MAX_LINEOUT*sizeof(float*));
	int ReadNum = 0;
	BOOL bDataReady = FALSE;
	int nLastPeriod = -1;
	TINDEXINFO	* pTJIndex = NULL;
	for(i=0;i<MAX_TJWARN;i++)
	{
		if(g_aWarnTjEx[i].nTjIndexNo==-1||g_aWarnTjEx[i].bUsed==FALSE) continue;
		pTJIndex = g_pCalc->_CallDTopViewCalc_8(TJ_TYPE,g_aWarnTjEx[i].acCode);
		if(!pTJIndex) 
		{
			g_aWarnTjEx[i].nTjIndexNo = -1;
			g_aWarnTjEx[i].nPeriod = DAY_ST;
			continue;
		}
		g_aWarnTjEx[i].nTjIndexNo=pTJIndex->nIndexNo;
		if(!NeedCalcNowKa(i,nIDIndex,&CurrStockInf)) continue;
		bDataReady = bDataReady&&(g_aWarnTjEx[i].nPeriod==nLastPeriod);
		if(!bDataReady)
		{
			ReadNum = g_pCalc->_CallDTopViewCalc_4(pInfo->Code,pInfo->setcode,CalcPeriodToTdxPeriod(g_aWarnTjEx[i].nPeriod), ASK_ALL, 1);
			bDataReady = TRUE;
			nLastPeriod = g_aWarnTjEx[i].nPeriod;
		}
		if(ReadNum>0)
		{
			PeekMsg();
			g_pCalc->_CallDTopViewCalc_14(fPut[0]);
			memset(fPut,0,MAX_LINEOUT*sizeof(float*));
			fPut[0]  = g_pCalc->_CallDTopViewCalc_13(ReadNum);
			TINDEXINFO *pIndex = g_pCalc->_CallDTopViewCalc_8(TJ_TYPE, g_aWarnTjEx[i].nTjIndexNo);
			if(!pIndex) break;
			for(j=0;j<6;j++) pIndex->aPara[j].nValue[g_aWarnTjEx[i].nPeriod] = g_aWarnTjEx[i].aTjParam[j];
			if ( -1  != g_pCalc->_CallDTopViewCalc_5(TJ_TYPE,g_aWarnTjEx[i].nTjIndexNo,fPut) )
			{
				if ((TBE_MEANLESS(fPut[0][ReadNum-1]) != 0) && fabs(fPut[0][ReadNum-1]) > 0.1 )
				{
					ProcessPopWarn(pInfo, TJWARNBASE + g_aWarnTjEx[i].nTjIndexNo, nConditionNo);
				}
			}
		}
	}
	//������ϸ���Cache
	memcpy(&m_pHqCache[nIDIndex].iHq, &CurrStockInf, sizeof(CurrStockData));
	g_pCalc->_CallDTopViewCalc_14(fPut[0]);
}

void CWarnManager::ProcessPopWarn(MemStkInfo *pMemStockInf, int nReasonID, int nConditionNo)
{
	if(!pMemStockInf) return;
	WARN_STRU     WarnStru = {0};
	CurrStockData CurrStockInf = {0};
	g_pStockIO->ReadCurrStockData(pMemStockInf,&CurrStockInf);
	long nStockNo = g_pStockIO->GetIndex(pMemStockInf->Code,pMemStockInf->setcode);
	
	int tempReason = nReasonID;
	//��Ԥ�������Ϣ
	WarnStru.nStockNo = nStockNo;
	SYSTEMTIME correspondtime;	//�õ�ʱ��
	GetCorrespondingTime(&correspondtime);
    WarnStru.time     = correspondtime;	
    WarnStru.fPrice   = CurrStockInf.Now;
    WarnStru.nVol     = CurrStockInf.NowVol;
	WarnStru.InOutFlag= CurrStockInf.Flag.InOutFlag;
	WarnStru.nStockType = 1;//ָ��Ԥ��
	if(tempReason==ABSHIGHWARN &&  g_pAbsCondition[nConditionNo].bDoLevin1==1 && g_pAbsCondition[nConditionNo].bEverDo1==0)
		WarnStru.nStockType = 2;
	else if(tempReason==ABSHIGHWARN && g_pAbsCondition[nConditionNo].bDoLevin1==2 && g_pAbsCondition[nConditionNo].bEverDo1==0)
		WarnStru.nStockType = 3;
	else if(tempReason==ABSLOWWARN && g_pAbsCondition[nConditionNo].bDoLevin2==1 && g_pAbsCondition[nConditionNo].bEverDo1==0)
		WarnStru.nStockType = 4;
	else if(tempReason==ABSLOWWARN && g_pAbsCondition[nConditionNo].bDoLevin2==2 && g_pAbsCondition[nConditionNo].bEverDo1==0)
		WarnStru.nStockType = 5;
	
	for(int i=0;i < g_nTJWarnNum;i++) //��ֹ�ظ�Ԥ��
	{
		if(WarnStru.nStockNo==g_TJWarnBuffer[i].nStockNo && tempReason==g_TJWarnBuffer[i].nReason) 
			break;
	}
	if(i<g_nTJWarnNum)
		tempReason=-1;
	if(tempReason>0&&g_nTJWarnNum<WARN_BUFFER_SIZE-1)
	{
		WarnStru.nReason = tempReason;
		memcpy(&g_TJWarnBuffer[g_nTJWarnNum], &WarnStru, sizeof(WARN_STRU));
		g_nTJWarnNum++;

		//����pop
		if(g_WarnParam2.nTJBeepFlag==1)
			TDXBeep(0);
		else if(g_WarnParam2.nTJBeepFlag==2)
			::PlaySound(g_WarnParam2.TJWavFile,NULL,SND_ASYNC);
		//��ʼ��ʾ
		if(m_pTJGPDlg)
			m_pTJGPDlg->PostMessage(UPDATE_WARNGP,WarnStru.nStockType);
		else if(g_WarnParam2.bTJPopup)
		{
			if(m_pMainFrame->IsIconic())	
				m_pMainFrame->ShowWindow(SW_RESTORE); //�������С��״̬����ָ�
			m_pTJGPDlg = new CWarnGPDlg(m_pMainFrame);
			m_pTJGPDlg->m_nWarnType = WARN_TJ;
			m_pTJGPDlg->Create(IDD_WARNGP_DIALOG);
			m_pTJGPDlg->ShowWindow (SW_SHOW);
		}
	}
}

//�õ�Ԥ����Ϣ:��ͣ����ͣ,����ͣ,�򿪵�ͣ,��ͣ����ͣ,��ͣ����ͣ
void CWarnManager::GetStopCloseReason(int &nRet,struct MemStkInfo* pInfo,struct CurrStockData* pHq)
{
	if(!g_WarnParam.bStopClose) return;

	if(pHq->Sellp[0] < COMPPREC && pHq->Buyp[0] > COMPPREC && pHq->Now>pHq->Close*1.04)
	{
		if(pInfo->CloseStopFlag == 0)
		{
			pInfo->CloseStopFlag = 1;
			nRet = UPCLOSE;			//��ͣ
		}
		if(pInfo->CloseStopFlag == 2)
		{
			pInfo->CloseStopFlag = 1;
			nRet = DOWNUP;			//��ͣ����ͣ
		}
	}
	else if(pHq->Sellp[0] > COMPPREC && pHq->Buyp[0] < COMPPREC && pHq->Now > COMPPREC && pHq->Now<pHq->Close*0.96)
	{ 
		if(pInfo->CloseStopFlag == 0)
		{
			pInfo->CloseStopFlag = 2;
			nRet = DOWNCLOSE;		//��ͣ
		}
		if(pInfo->CloseStopFlag == 1)
		{
			pInfo->CloseStopFlag = 2;
			nRet = UPDOWN;			//��ͣ����ͣ
		}
	}
	else
	{
		if(pInfo->CloseStopFlag == 1)
		{
			pInfo->CloseStopFlag = 0;
			nRet = UPOPEN;			//����ͣ
		}
		if(pInfo->CloseStopFlag == 2)
		{
			pInfo->CloseStopFlag = 0;
			nRet = DOWNOPEN;		//�򿪵�ͣ
		}
	}
}

int	CWarnManager::GetReason(short nStockNo,WARN_STRU &WarnStru)
{
    struct  CurrStockData CurrStockInf;
    struct  MemStkInfo*   pMemStockInf;
	int		tempReason = -1;
	pMemStockInf = (*g_pStockIO)[nStockNo];
	if(!pMemStockInf) return -1;
	BOOL bZS = testzs(pMemStockInf);
	if(nStockNo>=DS_FROM)
	{

	}
	else if(g_WarnParam2.nWarnFlag==0)		//����A�ɺ���С��
	{
		short nGPType = GetStockType(pMemStockInf->setcode, pMemStockInf->Code);
		if(nGPType!=CODE_SZAG && nGPType!=CODE_SHAG && nGPType!=CODE_ZXB && nGPType!=CODE_CYB) return -1;
	}
	else if(g_WarnParam2.nWarnFlag==2) //�Զ����Ʊ
	{
		BOOL bInGpSet = FALSE;
		for(int i=0;i<g_aSelfWarnGp.size();i++)
		{
			if(g_aSelfWarnGp[i].setcode==pMemStockInf->setcode&&strcmp(g_aSelfWarnGp[i].code,pMemStockInf->Code)==0)
			{
				bInGpSet = TRUE;
				break;
			}
		}
		if(!bInGpSet) return -1;
	}
	else if(!BelongZXG(pMemStockInf))	//��ѡ��
		return -1;
	//
    g_pStockIO->ReadCurrStockData(pMemStockInf,&CurrStockInf);
	//����ּۣ����������գ���һ�μ�λΪ0���򲻱���
	if(CurrStockInf.Now < COMPPREC || CurrStockInf.Close < COMPPREC)
		return -1;
	float ftmpPrevPrice = CurrStockInf.Now - CurrStockInf.TickDiff;
	float ftmp,ftmp2;
	DWORD CriticalVol=500;
	if(pMemStockInf->ActiveCapital > COMPPREC)
	{
		CriticalVol = pMemStockInf->ActiveCapital*10000*g_WarnParam.fTickVol/100;
		if(CriticalVol<500) CriticalVol=500;
	}

    WarnStru.nStockNo = nStockNo;
	SYSTEMTIME correspondtime;	//�õ�ʱ��
	GetCorrespondingTime(&correspondtime);
    WarnStru.time     = correspondtime;	
    WarnStru.fPrice   = CurrStockInf.Now;
    WarnStru.nVol     = CurrStockInf.NowVol;
	WarnStru.InOutFlag= CurrStockInf.Flag.InOutFlag;
	WarnStru.nStockType = 0;	//ϵͳԤ��

	if(fabs(CurrStockInf.Max-CurrStockInf.Min) > COMPPREC)
	{
		if(g_WarnParam.bNewMaxWarn && CurrStockInf.NowVol>1 && CurrStockInf.Now > CurrStockInf.Close && fabs(CurrStockInf.Now-CurrStockInf.Max) < COMPPREC && fabs(CurrStockInf.TickDiff) > COMPPREC)
			tempReason = NEWMAX;						//���¸�
	    if(g_WarnParam.bNewMinWarn && CurrStockInf.NowVol>1 && CurrStockInf.Now < CurrStockInf.Close && fabs(CurrStockInf.Now-CurrStockInf.Min) < COMPPREC && fabs(CurrStockInf.TickDiff) > COMPPREC)
			tempReason = NEWMIN;						//���µ�
	}
	if(!bZS && (g_WarnParam.bHasBigBuy || g_WarnParam.bHasBigSell))
	{		
		long wlc = 0,wl = 0;
		for (int ii = 0;ii < 5;ii++)
		{   
			wlc = wlc+(long)CurrStockInf.Buyv[ii]-(long)CurrStockInf.Sellv[ii];
			wl = wl +(long)CurrStockInf.Buyv[ii]+(long)CurrStockInf.Sellv[ii];
		}
		float fWTB=(float)100.0*wlc/wl;
		if(g_WarnParam.bHasBigBuy)
		{
			if(pMemStockInf->BuySellFlag!=1 && fWTB>75.0 && fabs(CurrStockInf.Sellp[0]-CurrStockInf.Buyp[0])>COMPPREC && CurrStockInf.Sellp[0]>COMPPREC && (CurrStockInf.Buyv[0]>=4*CriticalVol||CurrStockInf.Buyv[1]>=4*CriticalVol||CurrStockInf.Buyv[2]>=4*CriticalVol||CurrStockInf.Buyv[3]>=4*CriticalVol||CurrStockInf.Buyv[4]>=4*CriticalVol))
			{
				tempReason = HASBIGBUY;	//�д���,�ھ�������²���
				pMemStockInf->BuySellFlag = 1;
			}
			else
				pMemStockInf->BuySellFlag = 0;
		}
		if(g_WarnParam.bHasBigSell)
		{
			if(pMemStockInf->BuySellFlag!=-1 && fWTB<-25.0 && fabs(CurrStockInf.Sellp[0]-CurrStockInf.Buyp[0])>COMPPREC && CurrStockInf.Buyp[0]>COMPPREC && (CurrStockInf.Sellv[0]>=4*CriticalVol||CurrStockInf.Sellv[1]>=4*CriticalVol||CurrStockInf.Sellv[2]>=4*CriticalVol||CurrStockInf.Sellv[3]>=4*CriticalVol||CurrStockInf.Sellv[4]>=4*CriticalVol))
			{
				tempReason = HASBIGSELL;//�д�����,�ھ�������²���
				pMemStockInf->BuySellFlag = -1;
			}
			else
				pMemStockInf->BuySellFlag = 0;
		}
	}
	ftmp = (1+0.03)*CurrStockInf.Close;
	ftmp2 = (1-0.02)*CurrStockInf.Close;
	if(g_WarnParam.bLowHigh && CurrStockInf.Now >= ftmp && ftmpPrevPrice < ftmp && CurrStockInf.Open <= ftmp2)
		tempReason = LOWHIGH;							//�Ϳ�����
	ftmp = (1-0.03)*CurrStockInf.Close;
	ftmp2 = (1+0.02)*CurrStockInf.Close;
	if(g_WarnParam.bHighLow && CurrStockInf.Now <= ftmp && ftmpPrevPrice > ftmp && CurrStockInf.Open >= ftmp2)
		tempReason = HIGHLOW;							//�߿�����
	if(g_WarnParam.bTickDiff && CurrStockInf.TickDiff > g_WarnParam.fTickDiff*CurrStockInf.Close)
		tempReason = TICKDIFFHIGH;						//���ʼ۲��
	if(g_WarnParam.bTickDiff && 0 - CurrStockInf.TickDiff > g_WarnParam.fTickDiff*CurrStockInf.Close)
		tempReason = TICKDIFFLOW;						//���ʼ۲��
	if(!bZS && (g_WarnParam.bTickVol || g_WarnParam.bTickHugeAmo))
	{			
		if(CurrStockInf.NowVol >= CriticalVol)
			tempReason = HUGEVOL;						//�����쳣
	}
	if(!bZS && g_WarnParam.bTickAmount && CurrStockInf.NowVol*CurrStockInf.Now*100 > g_WarnParam.fTickAmount)
		tempReason = HUGEAMOUNT;						//���쳣
	if(!bZS && g_WarnParam.bLB && pMemStockInf->VolBase>0 && CurrStockInf.NowVol > g_WarnParam.nLB*pMemStockInf->VolBase)
		tempReason = VOLRATIOBREAK;						//����ͻ��
	ftmp = (1+g_WarnParam.fUpRatio)*CurrStockInf.Close;
	if(g_WarnParam.bUpRatio && CurrStockInf.Now >= ftmp && ftmpPrevPrice < ftmp)
		tempReason = PRANGEBREAK;						//�Ƿ�ͻ��
	ftmp = (1-g_WarnParam.fDownRatio)*CurrStockInf.Close;
	if(g_WarnParam.bDownRatio && CurrStockInf.Now <= ftmp && ftmpPrevPrice > ftmp)
		tempReason = PRANGEDROP;						//��������
	//ͣ��ʾ��
	if(!bZS)
		GetStopCloseReason(tempReason,pMemStockInf,&CurrStockInf);

	return tempReason;
}

BOOL CWarnManager::ProcTimeRangeWarn(WARN_STRU &WarnStru)
{
	switch(WarnStru.nReason) 
	{
	case HUGEVOL:
		{
			g_TimeRangeWarnBuffer.push_back(WarnStru);
			while ((3600*WarnStru.time.wHour+60*WarnStru.time.wMinute+WarnStru.time.wSecond)-
					(3600*g_TimeRangeWarnBuffer[0].time.wHour+60*g_TimeRangeWarnBuffer[0].time.wMinute+g_TimeRangeWarnBuffer[0].time.wSecond)>
					60*g_nWarnRangeMinute)
					g_TimeRangeWarnBuffer.pop_front();
			int nTimeRangeWarnCount = 0;
			for(int i=g_TimeRangeWarnBuffer.size()-1;i>=0;i--)
			{
				if((3600*WarnStru.time.wHour+60*WarnStru.time.wMinute+WarnStru.time.wSecond)-
					(3600*g_TimeRangeWarnBuffer[i].time.wHour+60*g_TimeRangeWarnBuffer[i].time.wMinute+g_TimeRangeWarnBuffer[i].time.wSecond)>
					60*g_WarnParam.nTickAmoMinute)	break;
				if(g_TimeRangeWarnBuffer[i].nStockNo==WarnStru.nStockNo&&
					g_TimeRangeWarnBuffer[i].nReason==WarnStru.nReason&&
					g_TimeRangeWarnBuffer[i].nStockType==WarnStru.nStockType)
					if(++nTimeRangeWarnCount>=g_WarnParam.nTickAmoTime)
					{
						for(int j=0;j<g_TimeRangeWarnBuffer.size();j++)
						{
							if(g_TimeRangeWarnBuffer[j].nStockNo==WarnStru.nStockNo&&
								(g_TimeRangeWarnBuffer[j].nReason==HUGEVOL||g_TimeRangeWarnBuffer[j].nReason==TICKHUGEAMO))
								g_TimeRangeWarnBuffer[j].nReason = -1;
						} 
						WarnStru.nReason = TICKHUGEAMO;
						return TRUE;
					}
			}
		}
		break;
	default:
		break;
	}
	return FALSE;
}

int CWarnManager::RefreshAllBuf(short nStockNo)
{
    WARN_STRU   WarnStru;
    short nReason = GetReason(nStockNo,WarnStru);
    if(nReason < 0) return -1;
    WarnStru.nReason = nReason;

	for(int i=0;i < g_nWarnNum;i++) //��ֹ�ظ�Ԥ��
	{
		if((WarnStru.nStockNo==g_WarnBuffer[i].nStockNo && WarnStru.nReason==g_WarnBuffer[i].nReason) && 
			(WarnStru.nVol==g_WarnBuffer[i].nVol || fabs(WarnStru.fPrice-g_WarnBuffer[i].fPrice)<COMPPREC)) 
			return -1;
	}
	if(g_WarnParam.bTickHugeAmo&&ProcTimeRangeWarn(WarnStru))
	{
		for(i=0;i < g_nWarnNum;i++) //�ٴη�ֹ�ظ�Ԥ��
		{
			if((WarnStru.nStockNo==g_WarnBuffer[i].nStockNo && WarnStru.nReason==g_WarnBuffer[i].nReason) && 
				(WarnStru.nVol==g_WarnBuffer[i].nVol || fabs(WarnStru.fPrice-g_WarnBuffer[i].fPrice)<COMPPREC)) 
				return -1;
		}
	}
    if(g_nWarnNum >= WARN_BUFFER_SIZE)
		memmove(&g_WarnBuffer[0],&g_WarnBuffer[1],(WARN_BUFFER_SIZE-1)*sizeof(WARN_STRU));
	g_nWarnNum = MIN(g_nWarnNum+1,WARN_BUFFER_SIZE);
	g_WarnBuffer[g_nWarnNum-1]= WarnStru;
	//����Buffer��,ֻ������Ԥ������ʾ
	if(!g_WarnParam.bTickVol && WarnStru.nReason==HUGEVOL)
		return -1;
	return WarnStru.nStockType;
}

void CWarnManager::GetWarnGpSet()
{
	MemStkInfo *tmpStkInfo;
	int nGPType,i,j,iIndex;
	BOOL bInGpSet;

	for(i=0;i<MAXGPNUM;i++) g_aWarnGpSet[i] = -1;
	g_nWarnGpNum = 0;
	if(g_bAllWarnSwitch||g_nUnitRadarFlag>0)	
	{
		//�г��״�ֻ��AB����Ч
		for(i=0;i<g_pStockIO->GetStockNum(ALL_ALL);i++)
		{
			if(g_nWarnGpNum>=MAXGPNUM) continue;
			tmpStkInfo = (*g_pStockIO)[i];
			if(!tmpStkInfo||tmpStkInfo->Close<COMPPREC) 
				continue;
			if(g_WarnParam2.nWarnFlag==0)
			{
				nGPType = GetStockType(tmpStkInfo->setcode, tmpStkInfo->Code);
				if(nGPType!=CODE_SZAG && nGPType!=CODE_SHAG && nGPType!=CODE_ZXB && nGPType!=CODE_CYB)
					continue;
			}
			else if(g_WarnParam2.nWarnFlag==2)
			{
				bInGpSet=FALSE;
				for(j=0;j<g_aSelfWarnGp.size();j++)
				{
					if(g_aSelfWarnGp[j].setcode==tmpStkInfo->setcode&&strcmp(g_aSelfWarnGp[j].code,tmpStkInfo->Code)==0)
					{
						bInGpSet = TRUE;
						break;
					}
				}
				if(!bInGpSet) continue;
			}
			else if(!BelongZXG(tmpStkInfo))
				continue;			
			g_aWarnGpSet[g_nWarnGpNum++]=i;
		}
	}
	if(g_bTjWarnSwitch&&g_pAbsConditionNum>0)
	{
		for(i=0;i<g_pAbsConditionNum&&g_nWarnGpNum<MAXGPNUM;i++)
		{
			iIndex=g_pStockIO->GetIndex(g_pAbsCondition[i].code,g_pAbsCondition[i].setcode);
			for(j=0;j<g_nWarnGpNum;j++)
				if(g_aWarnGpSet[j]==iIndex) break;
			if(j<g_nWarnGpNum) continue;
			tmpStkInfo=(*g_pStockIO)[iIndex];
			if(!tmpStkInfo||tmpStkInfo->Close<COMPPREC) 
				continue;
			g_aWarnGpSet[g_nWarnGpNum++] = iIndex;
		}
	}
}

int CWarnManager::InAbsTJBuffer(short nStockNo)
{
	MemStkInfo *tmpInfo=(*g_pStockIO)[nStockNo];
    for(int i=0; i<g_pAbsConditionNum; i++)
	{
		if(tmpInfo->setcode==g_pAbsCondition[i].setcode && memcmp(tmpInfo->Code,g_pAbsCondition[i].code,SH_CODE_LEN)==0)
			return i;
	}
    return -1;
}

char *WarnReasonStr[] = 
{
	"�������","��������","����ͻ��","�۷�ͻ��","�۷�����",
	"�����ɽ�","�޶�ɽ�","��������","�������","�Ϳ�����",
	"�߿�����","�����¸�","�����µ�","��λ����","��λ����",
	"����ͻ��","��������","����ͣ��","���ͣ��","����ͣ",
	"�򿪵�ͣ","��ͣ����ͣ","��ͣ����ͣ","��������","��λ����",
	"��λ����","����ͻ��","��������"
};

int WarnReasonColor[] =
{
	0,0,0,1,-1,
	0,0,1,-1,1,
	-1,1,-1,1,-1,
	0,0,1,-1,0,
	1,0,1,0,0,
	0,0,0
};

char * CWarnManager::GetStrFromReason(short nReason)
{
	return WarnReasonStr[nReason];
}

int CWarnManager::GetColorFromReason(short nReason,short InOutFlag)
{
	if(nReason<HASBIGBUY || nReason>ABSAPRATE2) 
		return 2;
	if(WarnReasonColor[nReason] == 1)		return 0;
	else if(WarnReasonColor[nReason]==-1)	return 1;
	else if(nReason==HUGEVOL || nReason==HUGEAMOUNT || nReason==VOLRATIOBREAK || nReason==TICKHUGEAMO)
	{
		if(InOutFlag==0)		return 0;
		else if(InOutFlag==1)	return 1;
	}
	return 2;
}
