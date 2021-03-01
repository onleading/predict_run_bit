/*
* Copyright (c) 2002,通达信开发部
* All rights reserved.
* 
* 文件名称：UFxsub.cpp
* 文件标识：
* 摘    要：完成分析图的相关局部函数，工具等.
*
*/
#include "stdafx.h"
#include "Ufxt.h"
#include "SelectGPDlg.h"
#include "MainFrm.h"
//////////////////////////////////////////////////////////////////////////
//					私有函数
//////////////////////////////////////////////////////////////////////////

int		l_LastSHOWNUM = 130;
int		l_LastSetSHOWNUM = 0;
int		l_LastTabSel = 0;

void UFxt::GetLastValue()
{
	if(l_LastSetSHOWNUM>0)
		m_nSHOWNUM=l_LastSetSHOWNUM;
	else
		m_nSHOWNUM=l_LastSHOWNUM;
	m_nSHOWNUM=min(RECENTNUM,m_nSHOWNUM);
	m_TabSel = l_LastTabSel;
}

void UFxt::SaveLastValue()
{
	if(m_nDrawNum>0)
	{
		l_LastSHOWNUM=max(MIN_SHOWNUM,m_nDrawNum);
		l_LastSHOWNUM=min(RECENTNUM,l_LastSHOWNUM);
	}
	l_LastTabSel = m_TabSel;	
}

BOOL UFxt::IsFxtShowTab()
{
	return g_bFxtShowTab&&!m_nInCom2;
}

void UFxt::SetFormulaPack()
{
	int i=0;
	m_pSwitch->ClearAllLabel();
	m_nValidFreqZB = 0;
	m_pSwitch->InsertSwitchLabel(m_nValidFreqZB++,_F("管理"),WM_COMMAND,ID_FMLPACKMNG,0,0,1);//everclick为1
	m_pSwitch->InsertSwitchLabel(m_nValidFreqZB++,_F("另存为"),WM_COMMAND,ID_ADDFMLPACK,0,0,1);//everclick为1
	for(i=0;i<g_nFmlPack;i++)
		m_pSwitch->InsertSwitchLabel(m_nValidFreqZB++,g_FmlPack[i].aPackName,WM_COMMAND,ID_FMLPACK00+i);
	SetSwitchSel();
	if(IsFxtShowTab()) 
		m_pSwitch->ShowWindow(SW_SHOW);
	else 
		m_pSwitch->ShowWindow(SW_HIDE);
}

void UFxt::SetFrequentZB()
{
	int		n=0,i=0;
	m_nValidFreqZB = 0;
	TINDEXINFO  * TIndex;
	m_pSwitch->ClearAllLabel();
	m_pSwitch->InsertSwitchLabel(m_nValidFreqZB++,_F("全部"),WM_COMMAND,ID_HXZBDLG,0,0,1);//everclick为1
	for ( n=0;n<g_nOftenUseZb;++n )
	{
		TIndex = g_pCalc->_CallDTopViewCalc_8(ZB_OBJECT,g_strOftenUseZb[n]);
		if ( TIndex )
			m_pSwitch->InsertSwitchLabel(m_nValidFreqZB++,g_strOftenUseZb[n],UM_CHANGEZB,TIndex->nIndexNo);
		else
		{
			for(i = 0;i < g_nSpeclIndexNum;i++)
			{
				if(stricmp(g_strOftenUseZb[n],g_SpeclIndex[i].acCode)==0)
					break;
			}
			if(i<g_nSpeclIndexNum)
				m_pSwitch->InsertSwitchLabel(m_nValidFreqZB++,g_strOftenUseZb[n],UM_CHANGESPEZB,i);								
		}
	}
	if(m_nValidFreqZB==0) 
		g_bFxtShowTab = FALSE;
	SetSwitchSel();
	if (IsFxtShowTab())
		m_pSwitch->ShowWindow(SW_SHOW);
	else
		m_pSwitch->ShowWindow(SW_HIDE);
}

void UFxt::SetSwitchSel()
{
	return;
	if(!IsFxtShowTab()) 
		return;
	switch(m_pTabSwitch->GetCurLabel())
	{
	case 0:
		{
			int i=0,j=0;
			if(m_nSelRegion<=0 || m_nSelRegion>=m_nRegion)
				m_nSelRegion = m_nRegion-1;
			for(i=0;i<m_aRegion[m_nSelRegion].nObject;i++)
			{
				if(m_aRegion[m_nSelRegion].aIndexInfo[i]->nSet==ZB_TYPE) 
					break;
			}
			if(i<m_aRegion[m_nSelRegion].nObject)
			{
				for(j=0;j<m_pSwitch->GetTabNum();j++)
				{
					if(strncmp(m_aRegion[m_nSelRegion].aIndexInfo[i]->acCode,m_pSwitch->GetCaption(j),ZBNAME_LEN)==0) 
						break;
				}
				if(j<m_pSwitch->GetTabNum()) 
					m_pSwitch->SetCurLabel(j);
				else 
					m_pSwitch->SetCurLabel(0);
			}
			else 
				m_pSwitch->SetCurLabel(0);
		}
		break;
	case 1:
		if(m_lBandPackNo>=0 && m_lBandPackNo<g_nFmlPack) 
			m_pSwitch->SetCurLabel(m_lBandPackNo+2);
		else if(m_lCurPackNo>=0 && m_lCurPackNo<g_nFmlPack) 
			m_pSwitch->SetCurLabel(m_lCurPackNo+2);
		else 
			m_pSwitch->SetCurLabel(0);
		break;
	default:
		return;
	}
}

void UFxt::ResetAllSwitch()
{
	SetFrequentZB();
	if(m_pSwitch) m_pSwitch->SetWindowPos(0,left+TABSWITCH_WID,bottom,right-left-TABSWITCH_WID,SWITCH_HEIGHT2,SWP_SHOWWINDOW);
}

BOOL UFxt::LoadPriv()
{
	memcpy(m_aRatio,l_aRatio,RATIONNUM*sizeof(float));
	return (l_aRatio[0]>=COMPPREC);
}

void UFxt::SavePriv()
{
	memcpy(l_aRatio,m_aRatio,RATIONNUM*sizeof(float));
}

void UFxt::CreateInfoDlg()
{
	if(!g_bFxtShowInfoDlg) return;
	if(m_pInfoDlg) return;
	m_InfoDlgCreating  = TRUE;
	//创建有模式对话框，一定要用指针动态创建
	m_pInfoDlg = new CInfoDlg(m_pView, m_nWhichUnit);
	m_pInfoDlg->Create(IDD_INFO_DIALOG);
	if(g_InfoDlgPosx>-40 && g_InfoDlgPosx<ScreenCx-40 && g_InfoDlgPosy>-60 && g_InfoDlgPosy<ScreenCy-60)
		m_pInfoDlg->SetWindowPos(NULL,g_InfoDlgPosx,g_InfoDlgPosy,0,0,SWP_NOSIZE);
	else
	{
		m_pInfoDlg->SetWindowPos(NULL,600,500,0,0,SWP_NOSIZE);
		g_InfoDlgPosx=600;g_InfoDlgPosy=500;
	}
	if(g_bInfoMinimize) //如是是最小化状态
	{
		WINDOWPLACEMENT m_windowplacement;
		m_pInfoDlg->GetWindowPlacement (&m_windowplacement);
		m_windowplacement.showCmd=SW_MINIMIZE;
		m_pInfoDlg->SetWindowPlacement (&m_windowplacement);
	}
	m_pInfoDlg->ShowWindow (SW_SHOW);
	AfxGetMainWnd()->SetActiveWindow();
	m_InfoDlgCreating = FALSE;
}

void UFxt::DeleteInfoDlg()
{
	if(m_pInfoDlg && !m_InfoDlgCreating) 
	{
		WINDOWPLACEMENT tmpPlace;
		m_pInfoDlg->GetWindowPlacement(&tmpPlace);
		g_InfoDlgPosx = tmpPlace.rcNormalPosition.left;
		g_InfoDlgPosy = tmpPlace.rcNormalPosition.top;
		g_bInfoMinimize = tmpPlace.showCmd & SW_SHOWMINIMIZED;
		m_pInfoDlg->DestroyWindow();
		TDEL(m_pInfoDlg);
	}
}

void UFxt::FillDlgInfo(int nNo)
{
	if ( m_nDataNum<1 ) return;
	struct AnalyData * pAnaly = m_pObject->pHisDay+nNo;
	//精确换手
	float fRealLTGB = 0,fRealZGB = 0;
	switch ( m_nPeriod )
	{
	case PER_MIN1:	
	case PER_MIN5:	
	case PER_MIN15:	
	case PER_MIN30:	
	case PER_HOUR:	
	case PER_MINN:	
		{
			_snprintf(g_DlgInfo.time1,20,"%d/%d/%02d:%02d",
			pAnaly->Time.Daye.Day/100,
			pAnaly->Time.Daye.Day%100,
			pAnaly->Time.Daye.Minute/60,
			pAnaly->Time.Daye.Minute%60);
			//猜测时间
			int nYear = g_lOpenRq/10000;
			for(int i=m_nDataNum-2;i>=nNo;i--)
			{
				if(m_pObject->pHisDay[i].Time.Daye.Day>m_pObject->pHisDay[i+1].Time.Daye.Day)
					--nYear;
			}
			long nDate = 10000*nYear+pAnaly->Time.Daye.Day;
			g_pStockIO->ReadLTGB_ZGB_OneDate(m_pStkInfo,nDate, &fRealLTGB, &fRealZGB, m_pStkInfo->ActiveCapital, m_pStkInfo->J_zgb);
		}
		break;
	case PER_DAY:
	case PER_WEEK:	
	case PER_MONTH:	
	case PER_DAYN:	
	case PER_SEASON:
	case PER_YEAR:
		_snprintf(g_DlgInfo.time1,20,"%02d/%02d/%02d/%s",pAnaly->Time.Date/10000,(pAnaly->Time.Date%10000)/100,pAnaly->Time.Date%100,_F(strWeekInfo[GetWeek(pAnaly->Time.Date)]));
		g_pStockIO->ReadLTGB_ZGB_OneDate(m_pStkInfo,pAnaly->Time.Date, &fRealLTGB, &fRealZGB, m_pStkInfo->ActiveCapital, m_pStkInfo->J_zgb);
		break;
	default:
		break;
	}
	if ( m_fYVal < 10000 )
		_snprintf(g_DlgInfo.value,20,"%9.3f",m_fYVal);
	else if ( m_fYVal < 10000*10000 )
		_snprintf(g_DlgInfo.value,20,_F("%9.3f万"),m_fYVal/10000);
	else 
		_snprintf(g_DlgInfo.value,20,_F("%9.3f亿"),m_fYVal/(10000*10000));
	_snprintf(g_DlgInfo.name,20,"%s",m_pStkInfo->Name);
	int nTmpXS=GetXSFlag(m_pStkInfo);
	if(nTmpXS==2)
	{
		_snprintf(g_DlgInfo.open,20,"%9.2f",pAnaly->Open);
		_snprintf(g_DlgInfo.max,20,"%9.2f",pAnaly->High);
		_snprintf(g_DlgInfo.close,20,"%9.2f",pAnaly->Close);
		_snprintf(g_DlgInfo.min,20,"%9.2f",pAnaly->Low);
	}
	else if(nTmpXS==3)
	{
		_snprintf(g_DlgInfo.open,20,"%9.3f",pAnaly->Open);
		_snprintf(g_DlgInfo.max,20,"%9.3f",pAnaly->High);
		_snprintf(g_DlgInfo.close,20,"%9.3f",pAnaly->Close);
		_snprintf(g_DlgInfo.min,20,"%9.3f",pAnaly->Low);
	}
	else
	{
		_snprintf(g_DlgInfo.open,20,"%9.1f",pAnaly->Open);
		_snprintf(g_DlgInfo.max,20,"%9.1f",pAnaly->High);
		_snprintf(g_DlgInfo.close,20,"%9.1f",pAnaly->Close);
		_snprintf(g_DlgInfo.min,20,"%9.1f",pAnaly->Low);
	}
	//三方行情
	if(m_pStkInfo->bigmarket==QH_TYPE)
	{
		strcpy(g_DlgInfo.volinstk,MakeVol3(pAnaly->QHVolInStock));
		g_DlgInfo.amount[0] = 0;
	}
	else 
		strcpy(g_DlgInfo.amount,MakeJE(pAnaly->Amount/m_pStkInfo->AmoUnit));
	if(!g_pStockIO->BeDSStock(m_pStkInfo))
	{
		if(!m_bZs)
		{
			if(need_justcjl(m_pStkInfo->setcode,m_pStkInfo->Code) )
				_snprintf(g_DlgInfo.volume,20,_F("%.1f"),1.0*(pAnaly->Volume/m_pStkInfo->Unit));
			else
				_snprintf(g_DlgInfo.volume,20,"%u",pAnaly->Volume);
		}
		else
		{
			if(m_nPeriod==PER_DAY||m_nPeriod==PER_WEEK||m_nPeriod==PER_MONTH||m_nPeriod==PER_DAYN||m_nPeriod==PER_SEASON||m_nPeriod==PER_YEAR)
				_snprintf(g_DlgInfo.volume,20,_F("%.1f万"),1.0*pAnaly->Volume/10000);
			else
				g_DlgInfo.volume[0] = 0;
		}
		if(!m_bZs && fRealLTGB > 1)
		{
			float fHsl = CalcHSL(m_pStkInfo,pAnaly->Volume,fRealLTGB,m_nPeriod);
			_snprintf(g_DlgInfo.hsl,20,"%6.2f%%",fHsl);
			_snprintf(g_DlgInfo.ltgb,20,"%s",MakeGB(fRealLTGB));
		}
		else
		{
			g_DlgInfo.hsl[0] = 0;
			g_DlgInfo.ltgb[0] = 0;
		}
	}
	else
	{
		//三方行情的Unit为1
		_snprintf(g_DlgInfo.volume,20,"%s",MakeVol3((float)(pAnaly->Volume/m_pStkInfo->Unit)));
		if(m_pStkInfo->bigmarket==QH_TYPE)
		{
			if(nTmpXS==2)
				_snprintf(g_DlgInfo.hsl,20,"%9.2f",pAnaly->YClose);
			else if(nTmpXS==3)
				_snprintf(g_DlgInfo.hsl,20,"%9.3f",pAnaly->YClose);
			else
				_snprintf(g_DlgInfo.hsl,20,"%9.1f",pAnaly->YClose);
		}
		else
		{
			g_DlgInfo.hsl[0] = 0;
			g_DlgInfo.ltgb[0] = 0;
		}
	}

	float tmpClose = pAnaly->Open;
	if(nNo > 0 && m_pStkInfo->bigmarket==QH_TYPE && m_nPeriod==PER_DAY && (pAnaly-1)->YClose > COMPPREC)
		tmpClose = (pAnaly-1)->YClose;
	else if(nNo > 0 && (pAnaly-1)->Close > COMPPREC)
		tmpClose = (pAnaly-1)->Close;
	else if(nNo==0)
	{
		if( pAnaly->Time.Date==g_lOpenRq && 
		   (m_nPeriod==PER_DAY||m_nPeriod==PER_WEEK||m_nPeriod==PER_MONTH||m_nPeriod==PER_DAYN||m_nPeriod==PER_SEASON||m_nPeriod==PER_YEAR)
		  )
			tmpClose = m_pStkInfo->Close;
		else
			tmpClose = 0;
	}
	if(tmpClose > COMPPREC)
	{
		if(nTmpXS==2)
			_snprintf(g_DlgInfo.zhangdie,20,"%6.2f",(pAnaly->Close-tmpClose));
		else if(nTmpXS==3)
			_snprintf(g_DlgInfo.zhangdie,20,"%6.3f",(pAnaly->Close-tmpClose));
		else
			_snprintf(g_DlgInfo.zhangdie,20,"%6.1f",(pAnaly->Close-tmpClose));			
		_snprintf(g_DlgInfo.zhangfu,20,"%6.2f%%",100*(pAnaly->Close-tmpClose)/tmpClose);
		_snprintf(g_DlgInfo.zhenfu,20,"%6.2f%%",100*(pAnaly->High-pAnaly->Low)/tmpClose);
	}
	else
	{
		g_DlgInfo.zhangdie[0] = 0;
		g_DlgInfo.zhangfu[0] = 0;
		g_DlgInfo.zhenfu[0] = 0;
	}
	_snprintf(g_DlgInfo.lastclose,20,"%9.3f",tmpClose);
	//
	AllTrim(g_DlgInfo.open);
	AllTrim(g_DlgInfo.max);
	AllTrim(g_DlgInfo.close);
	AllTrim(g_DlgInfo.min);
	AllTrim(g_DlgInfo.volume);
	AllTrim(g_DlgInfo.amount);
	AllTrim(g_DlgInfo.zhangdie);
	AllTrim(g_DlgInfo.zhangfu);
	AllTrim(g_DlgInfo.zhenfu);
	AllTrim(g_DlgInfo.hsl);
	AllTrim(g_DlgInfo.ltgb);
}

void UFxt::FillCbInfo(int nY,int nNo)
{	
	//任何信息，保证CBINFO及时知道
	g_CbInfo.m_nDataNum = m_nDataNum;
	g_CbInfo.m_nTop		= GetYPosOfRegion(0,m_aRegion[0].dMaxValEx);
	g_CbInfo.m_nBottom  = GetYPosOfRegion(0,m_aRegion[0].dMinValEx);//m_aRegion[0].DrawRc.bottom;
	g_CbInfo.m_nWhichPeriod= m_nPeriod;
	g_CbInfo.m_pData    = m_pObject->pHisDay;
	//////////////////////////////
	if(!m_bDataReady)		return;
	if(!m_pObject->pHisDay) return;
	if ( m_nDataNum<1 )		return;
	///////////////////////////////
	g_CbInfo.m_nCurr    = nNo;
	g_CbInfo.m_nY		= nY;
	g_CbInfo.m_nYC		= GetYPosOfRegion(0,m_pObject->pHisDay[nNo].Close);
	//如果叠加，必须取原始股票的最值 不是m_aRegion[0].dMaxValEx;
	if (  (m_bOverStk && m_nMainAxisStatus == R100_AXIS )
		|| !m_bOverStk )
	{
		g_CbInfo.m_dMaxPrice= m_aRegion[0].dMaxValEx;
		g_CbInfo.m_dMinPrice= m_aRegion[0].dMinValEx;
	}
	else
	{
		g_CbInfo.m_dMaxPrice= m_pObject->pHisDay[m_nLeftNo].High;
		g_CbInfo.m_dMinPrice= m_pObject->pHisDay[m_nLeftNo].Low;
		for ( int i=0;i<m_nDrawNum;++i )
		{
			if ( g_CbInfo.m_dMinPrice > m_pObject->pHisDay[m_nLeftNo+i].Low )
				g_CbInfo.m_dMinPrice = m_pObject->pHisDay[m_nLeftNo+i].Low;
			if ( g_CbInfo.m_dMaxPrice < m_pObject->pHisDay[m_nLeftNo+i].High )
				g_CbInfo.m_dMaxPrice = m_pObject->pHisDay[m_nLeftNo+i].High;
		}
	}
	g_CbInfo.pStk = m_pStkInfo;
}		

void UFxt::PopupInfoDlg()
{
	if ( m_nDataNum<1 ) 
		return;
	if(!g_bFxtShowInfoDlg && m_pInfoDlg) //如果已经改变了显示行情风格,则删除原来的行情小窗
	{
		DeleteInfoDlg();
		m_bChgRegion = TRUE;
	}
	if(m_pInfoDlg)
	{
		if(m_bDSFlag)
		{
			if(m_pStkInfo->bigmarket==QH_TYPE)
			{
				m_pInfoDlg->GetDlgItem(IDC_STATIC_HSL)->SetWindowText(_T("结算价"));
				::ShowWindow(m_pInfoDlg->GetDlgItem(IDC_STATIC_LTG)->GetSafeHwnd(), SW_HIDE);
			}
			else
			{
				::ShowWindow(m_pInfoDlg->GetDlgItem(IDC_STATIC_HSL)->GetSafeHwnd(), SW_HIDE);
				::ShowWindow(m_pInfoDlg->GetDlgItem(IDC_STATIC_LTG)->GetSafeHwnd(), SW_HIDE);
			}
		}
		else 
		{
			::ShowWindow(m_pInfoDlg->GetDlgItem(IDC_STATIC_HSL)->GetSafeHwnd(), SW_SHOW);
			::ShowWindow(m_pInfoDlg->GetDlgItem(IDC_STATIC_LTG)->GetSafeHwnd(), SW_SHOW);
		}
		m_pInfoDlg->SendMessage(UM_NOTIFY_INFODLG);
	}
}

void UFxt::GetSelectMany()
{
	m_pExpertIndex	= g_pCalc->_CallDTopViewCalc_8(EXP_TYPE,g_strLastExp);
	m_pColorKIndex	= g_pCalc->_CallDTopViewCalc_8(KL_TYPE,g_strLastKLine);
	if(m_pColorKIndex==NULL && m_pExpertIndex==NULL) return;

	if (m_pColorKIndex != NULL)
	{
		if ( m_pColorKIndex->nDelStatus & ERR_INDEX )
		{
			TDX_MessageBox(NULL,"公式编译未通过!",MB_OK|MB_ICONEXCLAMATION);
			return;
		}
		for (int i=0;i<m_aRegion[0].nObject;i++ )
		{
			if ( m_aRegion[0].aIndexInfo[i]->nSet == KL_OBJECT )
			{	//只会有一个指示
				RegionDelOne(0,m_aRegion[0].aIndexInfo[i]);
				break;
			}
		}
		m_bKColor = TRUE;
		AppendRg0(m_pColorKIndex);
	}
	
	if (m_pExpertIndex != NULL)
	{
		if ( m_pExpertIndex->nDelStatus & ERR_INDEX )
		{
			TDX_MessageBox(NULL,"公式编译未通过!",MB_OK|MB_ICONEXCLAMATION);
			return;
		}
		for (int i=0;i<m_aRegion[0].nObject;i++ )
		{
			if ( m_aRegion[0].aIndexInfo[i]->nSet == EXP_OBJECT )
			{
				RegionDelOne(0,m_aRegion[0].aIndexInfo[i]);
				break;
			}
		}
		AppendRg0(m_pExpertIndex);
	}
}

void UFxt::SaveSelectMany()
{
	g_strLastExp[0]=0;
	if(m_pExpertIndex)
	{
		TINDEXINFO *tmpInfo;
		for(int i=0;i<g_pCalc->_CallDTopViewCalc_7(EXP_TYPE);i++)
		{
			tmpInfo=g_pCalc->_CallDTopViewCalc_8(EXP_TYPE,i);
			if(tmpInfo==m_pExpertIndex)
			{
				strncpy(g_strLastExp,tmpInfo->acCode,ZBCODE_LEN-1);
				g_strLastExp[ZBCODE_LEN-1]=0;
				break;
			}
		}
	}
	g_strLastKLine[0]=0;
	if(m_pColorKIndex)
	{
		TINDEXINFO *tmpInfo;
		for(int i=0;i<g_pCalc->_CallDTopViewCalc_7(KL_TYPE);i++)
		{
			tmpInfo=g_pCalc->_CallDTopViewCalc_8(KL_TYPE,i);
			if(tmpInfo==m_pColorKIndex)
			{
				strncpy(g_strLastKLine,tmpInfo->acCode,ZBCODE_LEN-1);
				g_strLastKLine[ZBCODE_LEN-1]=0;
				break;
			}
		}
	}
}

BOOL UFxt::GetTipInfo(int x,int y)
{
	TOBJECT * p = NULL;
	struct AnalyData * pAnaly = NULL;
	if( OnLine && (m_bReqing || !m_bDataReady) )
		return FALSE;
	if( m_bDrawLine )
		return FALSE;
	if( m_nDrawNum<=0 || m_nDataNum<=0)
		return FALSE;
	int ItemNo = GetItemNoFromX(x)+m_nLeftNo , aimY = 0, nRegionNo =  0 ,i ;
	if( x>m_pPosOfX[m_nDrawNum-1]+m_dWidth/2.0 )
		return FALSE;
	CString  StkName;
	StkName.Empty();
	for ( i=0;i<m_nRegion;i++ )
	{	CRect tmpRc(m_aRegion[i].Area);
		POINT pt;	pt.x = x;  pt.y = y;
		if ( tmpRc.PtInRect(pt) )
			break;
	}
	if ( i == m_nRegion )	
		return FALSE;
	nRegionNo = i;
	m_TipInfo.Empty();
	for ( i=0;i<m_aRegion[nRegionNo].nObject;i++ )
	{	
		p = m_aRegion[nRegionNo].aIndexInfo[i];
		pAnaly = p->pHisDay+ItemNo;
		if ( p->nSet == HIS_DAT )
		{	
			long vol = pAnaly->Volume;
			float amo = pAnaly->Amount;
			FillDlgInfo(ItemNo);
			int aimYex = GetYPosOfRegion(nRegionNo,pAnaly->Low,i);
			aimY =  GetYPosOfRegion(nRegionNo,pAnaly->High,i);
			if ( y<max(aimY,aimYex) && y>min(aimY,aimYex) )
			{	
				if(strcmp((*g_pStockIO)[p->nCurGP]->Name, g_DlgInfo.name))
				{
					CString	lpFormat;
					lpFormat.Format("\n开盘:%%6.%df\n最高:%%6.%df\n最低:%%6.%df\n收盘:%%6.%df\n",
						m_XsFlag,m_XsFlag,m_XsFlag,m_XsFlag);
					if ( m_bOverStk )
						StkName.Format("%s\n",(*g_pStockIO)[p->nCurGP]->Name);
					if ( m_nPeriod < PER_DAY || m_nPeriod == PER_MINN || PER_MIN1 == m_nPeriod )
					{	
						long LittleTime = pAnaly->Time.Daye.Day;
						CString LittleStr;
						LittleStr.Format("%d%s%d%s%02d:%02d",LittleTime/100,
							"月",
							LittleTime%100,
							"日",
							pAnaly->Time.Daye.Minute/60,
							pAnaly->Time.Daye.Minute%60);
						m_TipInfo.Format(StkName+LittleStr+lpFormat+"总量:%9d",
							pAnaly->Open,pAnaly->High,
							pAnaly->Low,pAnaly->Close,vol);
						return TRUE;
					}
					else
					{	
						m_TipInfo.Format(StkName+"%04d/%02d/%02d"+lpFormat+"总量:%9d",
							pAnaly->Time.Date/10000,
							(pAnaly->Time.Date%10000-pAnaly->Time.Date%100)/100,
							pAnaly->Time.Date%100,
							pAnaly->Open,pAnaly->High,
							pAnaly->Low,pAnaly->Close,vol);
						return TRUE;
					}
				}
				else
				{
					CString	lpFormat = "%s\n开盘:%s\n最高:%s\n最低:%s\n收盘:%s\n总量:%s\n总额:%s\n涨幅:%s";
					if ( m_bOverStk )
						StkName.Format("%s\n",(*g_pStockIO)[p->nCurGP]->Name);
					m_TipInfo.Format(StkName+lpFormat,g_DlgInfo.time1,g_DlgInfo.open,
						g_DlgInfo.max,g_DlgInfo.min,g_DlgInfo.close,
						g_DlgInfo.volume,g_DlgInfo.amount,g_DlgInfo.zhangfu);
					return TRUE;
				}
			}
		}
		else if ( p->nSet == GBZS_OBJECT )
		{
			int ItemNo1 = GetItemNoFromX(max(0,x-4))+m_nLeftNo; 
			int ItemNo2 = GetItemNoFromX(x+1)+m_nLeftNo, nAt = 0; 
			float fAdd = 0;
			for ( int l = ItemNo1;l<=ItemNo2;++l )
			{
				fAdd += p->pCalcData[0][l];
				if ( p->pCalcData[0][l] > COMPPREC )
					nAt = l;
			}
			if ( y>=m_aRegion[0].DrawRc.bottom - 18 
				&& y<=m_aRegion[0].Area.bottom
				&& fAdd > COMPPREC )
			{
				m_TipInfo="";
				l = nAt;
				nAt = p->pCalcData[1][nAt];
				int nRawAt=nAt;
				CString tmpTipInfo;
				for(;nAt<m_nCwNum; ++nAt)
				{
					if(CWData[nAt].Date>m_pAnalyData[l].Time.Date) break;
					tmpTipInfo="";
					switch ( CWData[nAt].Type )
					{
					case 1:	//类型1=1、除权,除息
						{
							CString Str;
							tmpTipInfo.Format("除权,除息:%04d/%02d/%02d",
								CWData[nAt].Date/10000,(CWData[nAt].Date%10000)/100,CWData[nAt].Date%100);
							if ( CWData[nAt].B01 > COMPPREC )
							{
								if((m_bSZKFSJJ || m_bSHKFSJJ) && m_pStkInfo && m_pStkInfo->Close>50.00)
									Str.Format("\n每10股派现金%5.3f元",CWData[nAt].B01/100.0);
								else
									Str.Format("\n每10股派现金%5.3f元",CWData[nAt].B01);
								tmpTipInfo += Str;
							}
							if ( CWData[nAt].B02 > COMPPREC )
							{
								Str.Format("\n每股配股价%5.3f元",CWData[nAt].B02);
								tmpTipInfo += Str;
							}
							if ( CWData[nAt].B03 > COMPPREC )
							{
								Str.Format("\n每10股送股比例%5.3f股",CWData[nAt].B03);
								tmpTipInfo += Str;
							}
							if ( CWData[nAt].B04 > COMPPREC )
							{
								Str.Format("\n每10股配股比例%5.3f股",CWData[nAt].B04);
								tmpTipInfo += Str;
							}
						}
						break;
					case 2: //类型2=2、送配股上市
						tmpTipInfo.Format("送配股上市:%04d/%02d/%02d\n上市前流通股本%5.3f万股\n上市前总股本%5.3f万股\n上市后流通股本%5.3f万股\n上市后总股本%5.3f万股",
							CWData[nAt].Date/10000,(CWData[nAt].Date%10000)/100,CWData[nAt].Date%100,
							CWData[nAt].B01,CWData[nAt].B02,CWData[nAt].B03,CWData[nAt].B04);
						break;
					case 3: //类型3=3、非流通股上市
						tmpTipInfo.Format("非流通股上市:%04d/%02d/%02d\n上市前流通股本%5.3f万股\n上市前总股本%5.3f万股\n上市后流通股本%5.3f万股\n上市后总股本%5.3f万股",
							CWData[nAt].Date/10000,(CWData[nAt].Date%10000)/100,CWData[nAt].Date%100,
							CWData[nAt].B01,CWData[nAt].B02,CWData[nAt].B03,CWData[nAt].B04);
						break;
					case 5: //类型5=5、股本变化
						tmpTipInfo.Format("股本变化:%04d/%02d/%02d\n上市前流通股本%5.3f万股\n上市前总股本%5.3f万股\n上市后流通股本%5.3f万股\n上市后总股本%5.3f万股",
							CWData[nAt].Date/10000,(CWData[nAt].Date%10000)/100,CWData[nAt].Date%100,
							CWData[nAt].B01,CWData[nAt].B02,CWData[nAt].B03,CWData[nAt].B04);
						break;
					case 6: //类型6=6、增发新股
						tmpTipInfo.Format("增发新股:%04d/%02d/%02d\n增发价格%5.3f元\n增发额度%5.3f股",
							CWData[nAt].Date/10000,(CWData[nAt].Date%10000)/100,CWData[nAt].Date%100,
							CWData[nAt].B02,CWData[nAt].B03);
						break;
					case 7: //类型7=7、上市公司从市场回购本公司的股份
						tmpTipInfo.Format("回购:%04d/%02d/%02d\n回购前流通股本%5.3f万股\n回购前总股本%5.3f万股\n回购后流通股本%5.3f万股\n回购后总股本%5.3f万股",
							CWData[nAt].Date/10000,(CWData[nAt].Date%10000)/100,CWData[nAt].Date%100,
							CWData[nAt].B01,CWData[nAt].B02,CWData[nAt].B03,CWData[nAt].B04);
						break;
					case 8: //类型8=8、增发新股上市
						tmpTipInfo.Format("增发新股上市:%04d/%02d/%02d\n上市前流通股本%5.3f万股\n上市前总股本%5.3f万股\n上市后流通股本%5.3f万股\n上市后总股本%5.3f万股",
							CWData[nAt].Date/10000,(CWData[nAt].Date%10000)/100,CWData[nAt].Date%100,
							CWData[nAt].B01,CWData[nAt].B02,CWData[nAt].B03,CWData[nAt].B04);
						break;
					case 9: //类型9=9、转配股上市
						tmpTipInfo.Format("转配股上市:%04d/%02d/%02d\n上市前流通股本%5.3f万股\n上市前总股本%5.3f万股\n上市后流通股本%5.3f万股\n上市后总股本%5.3f万股",
							CWData[nAt].Date/10000,(CWData[nAt].Date%10000)/100,CWData[nAt].Date%100,
							CWData[nAt].B01,CWData[nAt].B02,CWData[nAt].B03,CWData[nAt].B04);
						break;
					case 10: //类型10=10、可转债上市
						tmpTipInfo.Format("可转债上市:%04d/%02d/%02d\n上市前流通股本%5.3f万股\n上市前总股本%5.3f万股\n上市后流通股本%5.3f万股\n上市后总股本%5.3f万股",
							CWData[nAt].Date/10000,(CWData[nAt].Date%10000)/100,CWData[nAt].Date%100,
							CWData[nAt].B01,CWData[nAt].B02,CWData[nAt].B03,CWData[nAt].B04);
						break;
					case 11:
						{
							CString Str;
							tmpTipInfo.Format("流通股扩股:%04d/%02d/%02d",
								CWData[nAt].Date/10000,(CWData[nAt].Date%10000)/100,CWData[nAt].Date%100);
							Str.Format("\n扩股比例:%.3f倍",CWData[nAt].B03);
							tmpTipInfo += Str;
						}
						break;
					case 12:
						{
							CString Str;
							tmpTipInfo.Format("非流通股缩股:%04d/%02d/%02d",
								CWData[nAt].Date/10000,(CWData[nAt].Date%10000)/100,CWData[nAt].Date%100);
							Str.Format("\n缩股比例:%.3f倍",CWData[nAt].B03);
							tmpTipInfo += Str;
						}
						break;
					case 13:
					case 14:
						{
							CString Str;
							if(CWData[nAt].Type==13)
							{
								tmpTipInfo.Format("送认购权证:%04d/%02d/%02d",
									CWData[nAt].Date/10000,(CWData[nAt].Date%10000)/100,CWData[nAt].Date%100);
							}
							else
							{
								tmpTipInfo.Format("送认沽权证:%04d/%02d/%02d",
									CWData[nAt].Date/10000,(CWData[nAt].Date%10000)/100,CWData[nAt].Date%100);
							}
							Str.Format("\n每10股送:%.3f份",CWData[nAt].B03);
							tmpTipInfo += Str;
							Str.Format("\n行权价:%.3f",CWData[nAt].B01);
							tmpTipInfo += Str;
						}
						break;					
					}
					if(tmpTipInfo.GetLength()>0)
					{
						if(m_TipInfo.GetLength()<1)
							m_TipInfo+="权息资料变动\n"+tmpTipInfo;
						else
							m_TipInfo+="\r\n\r\n"+tmpTipInfo;
					}
				}
				return TRUE;
			}
		}
		else if ( p->nSet == FC_OBJECT )
		{
			int ItemNo1 = GetItemNoFromX(max(0,x-4))+m_nLeftNo; 
			int ItemNo2 = GetItemNoFromX(x+1)+m_nLeftNo, nAt = 0; 
			float fAdd = 0;
			for ( int l = ItemNo1;l<=ItemNo2;++l )
			{
				fAdd += p->pCalcData[0][l];
				if ( p->pCalcData[0][l] > COMPPREC )
					nAt = l;
			}
			if ( y>=m_aRegion[0].DrawRc.bottom - 18 
				&& y<=m_aRegion[0].Area.bottom
				&& fAdd > COMPPREC )
			{
				nAt = p->pCalcData[1][nAt];
				char TmpName[31]={0};
				if(nAt<m_nFCNum-1) strcpy(TmpName, FCData[nAt+1].Name);
				else strcpy(TmpName, m_pStkInfo->Name);
				CString Str;
				Str.Format("更名:%d/%d/%d\n旧名:%s\n新名:%s",FCData[nAt].Date/10000,(FCData[nAt].Date/100)%100,FCData[nAt].Date%100, 
					FCData[nAt].Name, TmpName);
				if(strlen(FCData[nAt].Reason)>0) 
				{
					Str += _T("\n原因:");
					Str += _T(FCData[nAt].Reason);
				}
				m_TipInfo += Str;
				return TRUE;
			}
		}
		else if ( p->nSet == ZB_OBJECT )
		{	
			for ( int j=0;j<p->nOutNum;j++ )
			{	
				aimY =  GetYPosOfRegion(nRegionNo,p->pCalcData[j][ItemNo],i);
				if ( abs(aimY-y)<=SELECT_ON )
					break;
			}
			if ( j<p->nOutNum )
			{	
				TINDEXINFO * pSomeIndex = g_pCalc->_CallDTopViewCalc_8(p->nSet,p->nGSIndex);	//不仅仅是指标
				if ( TBE_MEANLESS(p->pCalcData[j][ItemNo]) != 0 )
					m_TipInfo.Format((m_XsFlag==3)?("%s(%s):%-6.3f"):("%s(%s):%-6.2f"),pSomeIndex->acCode,pSomeIndex->aLine[j].acLineName,p->pCalcData[j][ItemNo]);
				else
					m_TipInfo.Empty();
				return TRUE;
			}
		}
		else if ( m_bExistZS && (p->nSet == KL_OBJECT || p->nSet == EXP_OBJECT) )
		{	
			pAnaly = m_pObject->pHisDay+ItemNo;
			aimY = GetYPosOfRegion(nRegionNo,pAnaly->High) - 8;
			/////////////////////////////////////////////
			if ( m_ExpZS.p && TBE_MEANLESS(p->pCalcData[EXITLONG][ItemNo])!=0 
				&& fabs(p->pCalcData[EXITLONG][ItemNo])>0.1 
				&&  abs(y-aimY) <= SELECT_ON )
			{	
				m_TipInfo.Format("%s:%s",p->acCode,"多头卖出");
				return TRUE;
			}
			if ( m_KLZS.p && TBE_MEANLESS(p->pCalcData[1][ItemNo])!=0 
				&& fabs(p->pCalcData[1][ItemNo])>0.1 
				&&  abs(y-aimY) <= SELECT_ON )
			{	
				m_TipInfo.Format("%s",m_KLZS.strInfo);
				return TRUE;
			}
			aimY = GetYPosOfRegion(nRegionNo,pAnaly->Low) + 8;
			///////////////////////////////
			if ( m_ExpZS.p && TBE_MEANLESS(p->pCalcData[ENTERLONG][ItemNo])!=0
				&& fabs(p->pCalcData[ENTERLONG][ItemNo])>0.1 
				&&  abs(y-aimY) <= SELECT_ON )
			{	
				m_TipInfo.Format("%s:%s",p->acCode,"多头买入");
				return TRUE;
			}
			if ( m_KLZS.p && TBE_MEANLESS(p->pCalcData[0][ItemNo])!=0
				&& fabs(p->pCalcData[0][ItemNo])>0.1 
				&&  abs(y-aimY) <= SELECT_ON )
			{	
				m_TipInfo.Format("%s",m_KLZS.strInfo);
				return TRUE;
			}
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
//					与股票叠加相关的函数
//////////////////////////////////////////////////////////////////////////

#define GPOverlap(Code,Setcode,nRegion,nOut)	\
{\
	if (m_nOverlapNum < 3)\
	{\
		TOBJECT * p = NULL;	\
		p = AddObjectNameCode(HIS_DAT,"STANDK",Code,Setcode);		\
		m_aRegion[nRegion].aIndexInfo[nOut] = p;			\
		m_aRegion[nRegion].nObject++;	m_bChgRatio = TRUE;	\
		RejustOneObject(p,m_nDataNum);		\
		ReadSomeStk(p);		CalcOne(p);		\
		CalcOneGSRatio(p,(m_nLastNo+1)-m_nDrawNum);	\
		m_bOverStk = TRUE;					\
		m_nOverlapNum++;\
	}\
}

void UFxt::OverlapGP()
{
	if(m_nSpecProcDraw>0) return;
	if(m_nPeriod!=PER_DAY && m_nPeriod!=PER_WEEK && m_nPeriod!=PER_MONTH && m_nPeriod!=PER_SEASON && m_nPeriod!=PER_YEAR)
	{
		TDX_MessageBox(m_pView->m_hWnd,"当前周期不支持叠加!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	int iGPNo = -1;
	CSelectGPDlg dlg;//打开选择股票对话框
	dlg.m_pStkInfo=m_pStkInfo;
	dlg.bSingleSelect = TRUE;
	dlg.bOverlapGpType = 1;//叠加类型
	dlg.bCurDomain = m_Domain;
	if(dlg.DoModal() == IDOK)
	{
		if(dlg.SelectedGPNum > 0)
			iGPNo = dlg.SelectedGPIndex[0];
		if(iGPNo!=-1)
		{
			int analynum = g_pStockIO->GetAnalyDataNum((*g_pStockIO)[iGPNo],m_nPeriod,GetMulNum(m_nPeriod));
			if(analynum < 2) //判断有没有本地数据
				TDX_MessageBox(m_pView->m_hWnd,"本地数据不全,若需要完整的叠加信息,请先用[数据下载]功能补充日线数据",MB_OK|MB_ICONEXCLAMATION);
			for (int i = 0; i < m_aRegion[0].nObject; i++)
				if (m_aRegion[0].aIndexInfo[i]->nCurGP == iGPNo) return;
			if ( m_nMainAxisStatus != R100_AXIS &&
				TDX_MessageBox(m_pView->m_hWnd,"是否换成百分比坐标 ?",MB_YESNO ) == IDYES )
			{
				m_bNowOverlap = TRUE;
				GPOverlap((*g_pStockIO)[iGPNo]->Code,(*g_pStockIO)[iGPNo]->setcode,0,m_aRegion[0].nObject);
				m_bNowOverlap = FALSE;
				ProcessMsg(UM_SETAXIS,R100_AXIS);
			}
			else
			{
				m_bNowOverlap = TRUE;
				GPOverlap((*g_pStockIO)[iGPNo]->Code,(*g_pStockIO)[iGPNo]->setcode,0,m_aRegion[0].nObject);
				m_bNowOverlap = FALSE;
				ProcessMsg(WM_PAINT);
			}
		}
	}
}

void UFxt::AutoOverlapGP(BOOL bToOverlap)
{
	if(m_nSpecProcDraw>0) return;
	if(bToOverlap && m_nPeriod!=PER_DAY && m_nPeriod!=PER_WEEK && m_nPeriod!=PER_MONTH && m_nPeriod!=PER_SEASON && m_nPeriod!=PER_YEAR)
	{
		TDX_MessageBox(m_pView->m_hWnd,"当前周期不支持叠加!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}	
	MemStkInfo* pInfo[5]={0};
	short nOverNum = GetOverlapGp(pInfo, 5);
	
	BOOL bUseNewAxis = FALSE; 
	int nNewAxis = g_Overlap[0].nAxis;
	if(g_Overlap[0].nAutoType==0) 
	{
		if(m_nOverlapNum>=nOverNum) return;		//已经over了

		for(int i=0;i<nOverNum;i++)
		{
			if ( bToOverlap && m_nMainAxisStatus != R100_AXIS &&
				TDX_MessageBox(m_pView->m_hWnd,"是否换成百分比坐标 ?",MB_YESNO ) == IDYES )
			{
				m_bNowOverlap = TRUE;
				if(pInfo[i]!=m_pStkInfo&&g_pStockIO->GetIndex(pInfo[i]->Code,pInfo[i]->setcode)!=m_aRegion[0].aIndexInfo[0]->nCurGP) 
					GPOverlap(pInfo[i]->Code,pInfo[i]->setcode,0,m_aRegion[0].nObject);
				m_bNowOverlap = FALSE;
				
				if(!bUseNewAxis) 
				{
					nNewAxis = R100_AXIS;
					bUseNewAxis = TRUE;
				}
			}
			else if(g_Overlap[0].nAxis!=m_nMainAxisStatus)
			{
				m_bNowOverlap = TRUE;
				if(pInfo[i]!=m_pStkInfo&&g_pStockIO->GetIndex(pInfo[i]->Code,pInfo[i]->setcode)!=m_aRegion[0].aIndexInfo[0]->nCurGP) 
					GPOverlap(pInfo[i]->Code,pInfo[i]->setcode,0,m_aRegion[0].nObject);
				m_bNowOverlap = FALSE;

				if(!bUseNewAxis) 
				{
					nNewAxis = g_Overlap[0].nAxis;
					bUseNewAxis = TRUE;
				}
			}
			else
			{
				m_bNowOverlap = TRUE;
				if(pInfo[i]!=m_pStkInfo&&g_pStockIO->GetIndex(pInfo[i]->Code,pInfo[i]->setcode)!=m_aRegion[0].aIndexInfo[0]->nCurGP) 
					GPOverlap(pInfo[i]->Code,pInfo[i]->setcode,0,m_aRegion[0].nObject);
				m_bNowOverlap = FALSE;
			}
		}
		if(bUseNewAxis) m_nMainAxisStatus = nNewAxis;
	}
	else
	{
		if(m_nOverlapNum>=nOverNum&&m_pStkInfo==g_Overlap[0].pNowStk) return;	//已经over了		
		int			i;
		TOBJECT	*	p;
		for ( i=1;i<m_aRegion[0].nObject;i++ )
		{
			if ( m_aRegion[0].aIndexInfo[i]->nSet == HIS_DAT )
			{	
				p = m_aRegion[0].aIndexInfo[i];
				RegionDelOne(0,p);
				--i;
			}
		}
		g_Overlap[0].nNum = 0;

		for(i=0;i<nOverNum;i++)
		{
			if ( bToOverlap && m_nMainAxisStatus != R100_AXIS &&
				TDX_MessageBox(m_pView->m_hWnd,"是否换成百分比坐标 ?",MB_YESNO ) == IDYES )
			{
				m_bNowOverlap = TRUE;
				if(pInfo[i]!=m_pStkInfo&&g_pStockIO->GetIndex(pInfo[i]->Code,pInfo[i]->setcode)!=m_aRegion[0].aIndexInfo[0]->nCurGP) 
					GPOverlap(pInfo[i]->Code,pInfo[i]->setcode,0,m_aRegion[0].nObject);
				m_bNowOverlap = FALSE;
				
				if(!bUseNewAxis) 
				{
					nNewAxis = R100_AXIS;
					bUseNewAxis = TRUE;
				}
			}
			else if(g_Overlap[0].nAxis!=m_nMainAxisStatus)
			{
				m_bNowOverlap = TRUE;
				if(pInfo[i]!=m_pStkInfo&&g_pStockIO->GetIndex(pInfo[i]->Code,pInfo[i]->setcode)!=m_aRegion[0].aIndexInfo[0]->nCurGP) 
					GPOverlap(pInfo[i]->Code,pInfo[i]->setcode,0,m_aRegion[0].nObject);
				m_bNowOverlap = FALSE;

				if(!bUseNewAxis) 
				{
					nNewAxis = g_Overlap[0].nAxis;
					bUseNewAxis = TRUE;
				}
			}
			else
			{
				m_bNowOverlap = TRUE;
				if(pInfo[i]!=m_pStkInfo&&g_pStockIO->GetIndex(pInfo[i]->Code,pInfo[i]->setcode)!=m_aRegion[0].aIndexInfo[0]->nCurGP) 
					GPOverlap(pInfo[i]->Code,pInfo[i]->setcode,0,m_aRegion[0].nObject);
				m_bNowOverlap = FALSE;
			}
		}
		if(bUseNewAxis) m_nMainAxisStatus = nNewAxis;
		
		g_Overlap[0].pNowStk = m_pStkInfo;
		g_Overlap[0].nAxis = m_nMainAxisStatus;
	}
	if(g_Overlap[0].nAutoType==0&&m_nOverlapNum<1)
		ProcessMsg(UM_SETAXIS,NORMAL_AXIS);
}

short	UFxt::GetOverlapGp(MemStkInfo **pInfo, short nMaxGp)
{
	BOOL bFindDuplicate=FALSE;
	short nOverNum = 0;
	if(g_Overlap[0].nAutoType==0)
	{
		for(int i=0;i<g_Overlap[0].nNum&&nOverNum<nMaxGp;i++)
			pInfo[nOverNum++] = g_Overlap[0].pOverlapStk[i] = g_pStockIO->Get_MemStkInfo(g_Overlap[0].StkCode[i],g_Overlap[0].StkSetCode[i]);
	}
	else
	{
		g_Overlap[0].nNum = 0;
		if((g_Overlap[0].nAutoType&AUTO_OVERLAP_IDX)&&nMaxGp>nOverNum)			//叠加指数
		{
			if(m_pStkInfo->setcode==0&&nMaxGp>nOverNum) 
			{
				MemStkInfo *pInf = g_pStockIO->Get_MemStkInfo(SZ_DPCODE, SZ);
				pInfo[nOverNum++] = pInf;
			}
			if(m_pStkInfo->setcode==1&&nMaxGp>nOverNum) 
			{
				MemStkInfo *pInf = g_pStockIO->Get_MemStkInfo(SH_DPCODE, SH);
				pInfo[nOverNum++] = pInf;
			}
		}
		if((g_Overlap[0].nAutoType&AUTO_OVERLAP_MG)&&nMaxGp>nOverNum)			//叠加权证正股
		{
			short nCurType = GetStockType(m_pStkInfo->setcode, m_pStkInfo->Code);
			if(nCurType==CODE_SZQZ||nCurType==CODE_SHQZ)
			{
				STKRELATION  LinkData[100] = {0};
				short nLinkNum = g_pStockIO->GetRLTData(m_pStkInfo, LinkData, 100);
				for(int i=0;i<nLinkNum;i++)
				{
					if(strlen(LinkData[i].Code)<1) continue;
					MemStkInfo *pInfo0 = g_pStockIO->Get_MemStkInfo(LinkData[i].Code, LinkData[i].SetCode);
					if(!pInfo0) continue;
					short Type=GetStockType(pInfo0->setcode,pInfo0->Code);
					if(Type==CODE_SHAG||Type==CODE_SZAG||Type==CODE_ZXB ||Type==CODE_CYB)
					{
						pInfo[nOverNum++] = pInfo0;
					}
				}
			}
		}
	}
	return nOverNum;
}
//////////////////////////////////////////////////////////////////////////
//					其它私有函数
//////////////////////////////////////////////////////////////////////////

BOOL UFxt::UseLocaldata()
{
	if(m_bLcIdx || m_bLcExt)
		return TRUE;
	return ((m_nPeriod == PER_DAY || m_nPeriod == PER_WEEK || m_nPeriod == PER_MONTH 
			|| m_nPeriod == PER_DAYN || m_nPeriod == PER_SEASON || m_nPeriod == PER_YEAR) && g_GH.bLocalData);
}

void UFxt::ModifyProperty(TOBJECT *p)
{
	strcpy(p->acCode,(*g_pCalc)[p->nSet][p->nGSIndex].acCode);
	p->nOutNum = (*g_pCalc)[p->nSet][p->nGSIndex].nLineNum;
	if ( (*g_pCalc)[p->nSet][p->nGSIndex].nAddXAxisNum > 0 )
		p->Property |= EXTRA_Y;
	if ( (*g_pCalc)[p->nSet][p->nGSIndex].nAxisType != 0 )
		p->Property |= DEFINE_Y;
	if ( (*g_pCalc)[p->nSet][p->nGSIndex].nDrawMode & PUCULINE )
		p->Property |= PUCU_LINE;
	else if ( (*g_pCalc)[p->nSet][p->nGSIndex].nDrawMode & SARLINE )
		p->Property |= SAR_LINE;
	CalcOne(p);
}

//////////////////////////////////////////////////////////////////////////
//					共有函数
//////////////////////////////////////////////////////////////////////////

void UFxt::WriteToTxtFile(CString filename,BOOL bWriteExcel)
{
	CFileException ex;
	CStdioFile  File;
	TOBJECT * pObject = m_pObject;
	if ( File.Open(filename,CFile::modeCreate|CFile::modeWrite,&ex) )
	{
		char  Buffer[1024],Cat[200];
		sprintf(Buffer,"                  %s (%s)\n",m_pStkInfo->Name,m_pStkInfo->Code);
		File.Write(Buffer,strlen(Buffer));
		File.Write("\r\n",2);
		sprintf(Buffer,"%10s\t%8s\t%8s\t%8s\t%8s\t%15s\t","时间","开盘","最高","最低","收盘","成交量");
		File.Write(_F(Buffer),strlen(Buffer));
		while( pObject->next )
		{
			if ( pObject->next->nSet == ZB_TYPE )
			{
				memset(Buffer,0,1024);
				TINDEXINFO	*pIndex = g_pCalc->_CallDTopViewCalc_8(ZB_TYPE,pObject->next->acCode);
				for ( int i=0;i<pObject->next->nOutNum;i++ )
				{
					sprintf(Cat,"%5s.%-6s\t",pIndex->acCode,pIndex->aLine[i].acLineName);
					strcat(Buffer,Cat);
				}
				File.Write(_F(Buffer),strlen(Buffer));
			}
			pObject = pObject->next;
		}
		File.Write("\r\n",2);
		for ( int i=0;i<m_nDataNum;i++ )
		{
			pObject = m_pObject;
			strcpy(Buffer,"          \t");
			switch ( m_nPeriod )
			{
			case PER_DAY:
			case PER_WEEK:
			case PER_MONTH:
			case PER_DAYN:
			case PER_SEASON:
			case PER_YEAR:
				sprintf(Buffer," %04d/%02d/%02d\t",
					m_pObject->pHisDay[i].Time.Date/10000,(m_pObject->pHisDay[i].Time.Date%10000)/100,
					m_pObject->pHisDay[i].Time.Date%100);
				break;
			default:
				sprintf(Buffer, " %02d/%02d-%02d:%02d\t",
					m_pObject->pHisDay[i].Time.Daye.Day/100, m_pObject->pHisDay[i].Time.Daye.Day%100,
					m_pObject->pHisDay[i].Time.Daye.Minute/60, m_pObject->pHisDay[i].Time.Daye.Minute%60);
				break;
			}
			File.Write(Buffer,strlen(Buffer));
			while ( pObject )
			{
				if ( pObject->nSet == HIS_DAT )
				{
					sprintf(Buffer,"%8.3f\t%8.3f\t%8.3f\t%8.3f\t%15u\t",
						pObject->pHisDay[i].Open,pObject->pHisDay[i].High,
						pObject->pHisDay[i].Low,pObject->pHisDay[i].Close,
						pObject->pHisDay[i].Volume);
					File.Write(Buffer,strlen(Buffer));
				}
				else if ( pObject->nSet == ZB_OBJECT )
				{
					for ( int k=0;k<pObject->nOutNum;k++ )
					{
						if ( TBE_MEANLESS(pObject->pCalcData[k][i]) != 0 ) 
						{
							sprintf(Buffer,"%11.3f\t",pObject->pCalcData[k][i]);
							File.Write(Buffer,strlen(Buffer));
						}
						else
						{
							sprintf(Buffer,"%11s\t"," ");
							File.Write(Buffer,strlen(Buffer));
						}
					}
				}
				pObject = pObject->next;
			}
			File.Write("\n",1);
		}
		File.Close();
	}
	else
	{
		char strError[1024];
		ex.GetErrorMessage(strError,1024);
		TDX_MessageBox(m_pView->m_hWnd,"文件创建错误!\n"+(CString)strError,MB_ICONEXCLAMATION|MB_OK);
	}
}
