/****************************************************
* Copyright (c) 2002,通达信开发部
* All rights reserved.
* 
* 文件名称：UFxt.cpp
* 摘    要：完成K线图显示、指标画线、平移等功能.
*
*****************************************************/
#include "stdafx.h"
#include "tdxw.h"
#include "MainFrm.h"
#include "Ufxt.h"
#include "AllZSDlg.h"
#include "DescZBDlg.h"
#include "AdjustParamDlg.h"
#include "OutStruct.h"
#include "PasswordDlg.h"
#include "HisDlg.h"
#include "QjStatDlg.h"
#include "EditTxtDlg.h"
#include "ComView.h"
#include "ComView2.h"
#include "FormulaPackDlg.h"

int			FXT_RWIDTH = 50;
const int   OVER_VAL = 10000,MIN_PIXLE = 30;
float		GoldLineRatio[7]={0.0f,19.1f,38.2f,50.0f,61.8f,80.9f,100.0f};

struct AnalyData	l_InValidHisDat;

extern CURFXTZB g_CurFxtZb;

UFxt::UFxt(LPCOMINFO pComInfo):UBase(pComInfo)
{
	int i,j;
	char * tmpp = (char *)&l_InValidHisDat;
	for(i=0;i<sizeof(AnalyData);i++)
		memset(tmpp+i,0x7A,sizeof(char));

	ResetSelfLine();

	m_last_rect = CRect(-1,-1,0,0);

	m_nValidFreqZB = 0;

	m_pInfoDlg = NULL;
	m_pOrignData = NULL;
	m_InfoDlgCreating = FALSE;
	m_nChgMouse  = 0;
	m_nMousePan	 = 0;
	m_nAtItem    = 0;
	m_nAtRegion  = 0;
	m_bXS3		 = 0;
	m_bBeginRectangle = FALSE;
	m_dNowVal    = 0.0;
	m_MainYRc.SetRect(0,0,0,0);
	m_bSelected = FALSE;
	m_PeriodRc.SetRect(0,0,0,0);
	m_pObject = NULL;
	m_nTodayNum= 0;
	m_nDataNum = 0;
	m_dWidth   = 0;
	m_dDistance= 0;
	m_bNotRedraw	= FALSE;

	m_nCommonPeriod	= m_nPeriod	= g_GH.nDefPeriod;
	m_PeriodKeyOrder= TdxPeriodToKeyOrder(m_nPeriod);
	m_nCalcPeriod	= TdxPeriodToCalcPeriod(m_nPeriod);

	m_pPosOfX	= NULL;
	m_pAllPosOfX= NULL;
	m_pXRule	= NULL;
	m_nKeyNo	= 0;
	m_nLeftNo	= 0;
	m_nLastNo	= 0;
	m_nTmpLeftNo = 0;
	m_nTmpLastNo = 0;
	m_bNowOverlap = FALSE;
	m_nOverlapNum = 0;
    m_bMouseCross = 0;
	m_bKeyCross   = 0;
	m_bLArchor	  = 0;
	m_bRArchor	  = 0;
	m_nSelRegion  = 0;
	m_bKColor	  = 0;
	m_YDy         = 0;
	m_dR100Max	  = 0;
	m_bR100Dot    = 0;
	m_bOverStk	  = 0;
	m_nSpecProcDraw = 0;
	m_nFilter	  = 0;
	m_bEvenSelected = FALSE;
	m_bKeepOver		= FALSE;
	m_pAnalyData	= NULL;
	m_pHisData		= NULL;
	m_pTmpAnalyData	= NULL;
	m_nTmpDataNum   = NULL;
	m_KLock			= -1;
	m_nToLeft		= 0;
	m_nToRight		= 0;
	m_nRightRgn     = 0;
	m_fYVal			= 0;
	m_pPoint		= NULL;
	///////////////////////
	m_bIsHead		  = 0;
	m_bDataReady      = 0;
	m_bReqing         = 0;
	m_bIsHead         = 0;
	m_nOverlapPenPos = MAX_PEN_NUM+5;
	m_nOverlapBrushPos = MAX_BRUSH_NUM+5;
	///////////////////////
	m_bChgRegion	= TRUE;
	m_bChgRatio		= TRUE;
	m_bDbClickFT	= FALSE;
	m_nOldSelRegion = -1;
	m_strDbSaveZb[0]= 0;
	m_pTabSwitch = m_pSwitch = NULL;

	m_ResetZSFlag	= 0;
	m_nQJPos1 = m_nQJPos2 = -1;
	///////////////////////
	memset(&m_Hq,0,sizeof(CurrStockData));
	memset(&m_HqEx,0,sizeof(EXCURRDATA));
	///////////////////////指示
	m_bExistZS	    = FALSE;
	m_pColorKIndex	= NULL;
	m_pExpertIndex	= NULL;
	memset(&m_ExpZS,0,sizeof(FXZSINFO));
	memset(&m_KLZS,0,sizeof(FXZSINFO));
	m_nMainAxisStatus = g_nMainAxis;
	memset(&m_OldMousePoint,0,sizeof(POINT));
	m_OldKeyPoint = m_OldMousePoint;
	m_ArchorPoint = m_OldMousePoint;
	m_nDownPoint  = m_OldMousePoint;
	m_OrgPoint    = m_OldMousePoint;
	m_TipArchor   = m_OldMousePoint;
	m_RealMove    = m_OldMousePoint;
	memset(&m_EageInfo,0,sizeof(m_EageInfo));
	memset(&m_SelectInfo,0,sizeof(FXTSELINFO));
	memset(m_aRegion,0,MAX_ZBT_NUM*sizeof(TREGION));
	memset(m_aRatio,0,RATIONNUM*sizeof(float));
	memset(&m_R100_AXIS,0,sizeof(m_R100_AXIS));
	m_nLastInfo = 0;
	memset(m_aLastInfo,0,MAX_ZBT_NUM*sizeof(ZBTINFO));
	//最多MAX_ZBT_NUM个区域，最少1个 不会从0开始 每个1/(n+1)，共n/(n+1)，剩余1/(n+1)给第零个
	if ( !LoadPriv() )
	{
		for ( i=1;i<=MAX_ZBT_NUM;i++ )
		{	int		Start = ((i-1)*i)>>1;
			double	dIncr = 0;
			dIncr = m_aRatio[Start] = 2.0/(i+2);
			for ( j=0;j<i;j++ )
			{
				m_aRatio[Start+j] = 1.0/(i+2) + dIncr;
				dIncr = m_aRatio[Start+j];
			}
		}
	}
	m_pDPFXIndex = g_pCalc->_CallDTopViewCalc_8(KL_TYPE, "大盘风向");
	//
	GetLastValue();
	//
	m_Com2WinNum	= g_nZBWinNum;
	m_Com2DrawMain	= g_GH.bDrawMainZb;
	m_bDrawMainZb	= g_GH.bDrawMainZb;
	strncpy(m_strZbMain,g_strZbMain,ZBNAME_LEN-1);
	strncpy(m_strCom2ZbMain,g_strZbMain,ZBNAME_LEN-1);
	m_strZbMain[ZBNAME_LEN-1] = 0;
	m_strCom2ZbMain[ZBNAME_LEN-1] = 0;
	for(i=0;i<11;i++)
	{
		strncpy(m_strFavUseZb[i],g_strFavUseZb[i],ZBNAME_LEN-1);
		m_strFavUseZb[i][ZBNAME_LEN-1] = 0;
		strncpy(m_strCom2FavUseZb[i],g_strFavUseZb[i],ZBNAME_LEN-1);
		m_strCom2FavUseZb[i][ZBNAME_LEN-1] = 0;
	}
	m_bUseFmlPack	= FALSE;
	m_lCurPackNo	= -1;
	m_lBandPackNo	= -1;
	if ( m_nStyle == FXT_MULTI )
	{
		m_nWillNum  = m_nSHOWNUM+3;
		m_nDrawNum  = m_nSHOWNUM;
		m_nRegion	= 2;	
		m_nOldRegion = 2;
		m_nMainAxisStatus = NORMAL_AXIS;
		m_aRatio[1] = 0.75;	m_aRatio[2] = 1;
		if ( (m_aRegion[0].aIndexInfo[m_aRegion[0].nObject] = AddObjectByName(HIS_DAT,"STANDK"))  )
			m_aRegion[0].nObject++;	
		if(m_bDrawMainZb)
		{
			if ( (m_aRegion[0].aIndexInfo[m_aRegion[0].nObject] = AddObjectByName(ZB_OBJECT,g_strZbMain))  )
				m_aRegion[0].nObject++;
		}
		if ( (m_aRegion[1].aIndexInfo[m_aRegion[1].nObject] = AddObjectByName(ZB_OBJECT,m_strFavUseZb[0])) )
			m_aRegion[1].nObject++;	
	}
	else //如果是第一次运行
	{
		m_nWillNum  = m_nSHOWNUM+3;
		m_nDrawNum  = m_nSHOWNUM;		
		if(g_GH.nInitKNum>=MIN_SHOWNUM && !m_nInCom2) 
			m_nDrawNum  = min(RECENTNUM, g_GH.nInitKNum);
		else if(g_GH.nInitKNum2>=MIN_SHOWNUM && m_nInCom2) 
			m_nDrawNum  = min(RECENTNUM, g_GH.nInitKNum2);
		m_nRegion	= g_nZBWinNum;	
		m_nOldRegion= m_nRegion;
		for ( i=0;i<m_nRegion;i++ )
		{	if ( i==0 )
			{
				if ( (m_aRegion[i].aIndexInfo[m_aRegion[i].nObject] = AddObjectByName(HIS_DAT,"STANDK")) )
					m_aRegion[i].nObject++;	
				if ( strlen(g_strZbMain)<=0 )
					strcpy(g_strZbMain,"MA2");
				if(m_bDrawMainZb)
				{
					if ( (m_aRegion[i].aIndexInfo[m_aRegion[i].nObject] = AddObjectByName(ZB_OBJECT,g_strZbMain)) )
						m_aRegion[i].nObject++;	
				}
				if ( (m_aRegion[i].aIndexInfo[m_aRegion[i].nObject] = AddObjectByIndex(GBZS_OBJECT,0)) )
					m_aRegion[i].nObject++;	
				if ( (m_aRegion[i].aIndexInfo[m_aRegion[i].nObject] = AddObjectByIndex(GGBW_OBJECT, 0)) )
					m_aRegion[i].nObject++;
				if ( (m_aRegion[i].aIndexInfo[m_aRegion[i].nObject] = AddObjectByIndex(FC_OBJECT,0)) )
					m_aRegion[i].nObject++;	
				if ( (m_aRegion[i].aIndexInfo[m_aRegion[i].nObject] = AddObjectByIndex(THUN_OBJECT,0)) )
					m_aRegion[i].nObject++;
			}
			else if ( i==1 )
			{
				if ( (m_aRegion[i].aIndexInfo[m_aRegion[i].nObject] = AddObjectNameCode(ZB_OBJECT,m_strFavUseZb[i-1])) == NULL )
					m_aRegion[i].aIndexInfo[m_aRegion[i].nObject] = AddObjectByName(ZB_OBJECT,"航标成交量");
				m_aRegion[i].nObject++;	
			}
			else
			{
				if ( (m_aRegion[i].aIndexInfo[m_aRegion[i].nObject] = AddObjectByName(ZB_OBJECT,m_strFavUseZb[i-1])) == NULL )
					m_aRegion[i].aIndexInfo[m_aRegion[i].nObject] = AddObjectByName(ZB_OBJECT,"MACD");
				m_aRegion[i].nObject++;
			}
		}
	}
	m_NormalRegion	= m_nRegion;
	m_nSelRegion	= m_nRegion-1;
	m_nSelRegionBak	= m_nSelRegion;

	//初始化ToolTip
	m_ToolTip.InitToolTip(m_pView,TTS_WITHTITLE);
	m_TipInfo.Empty();

	m_nCwNum  = 0;
	memset(CWData,0,MAX_CWDATA*sizeof(CWDATA));
	m_nFCNum = 0;
	memset(FCData,0,MAX_FCDATA*sizeof(TDXSTKOLDNAME));
	memset(&m_KAnalyData,0,sizeof(AnalyData));

	m_ViewZoomRect.SetRect(0, 0, 0, 0);

	//两个切换区
	m_pSwitch = new CNewZfjsTab(m_pView->m_hWnd);
	m_pSwitch->SetStyle(SwitchStyle_switchbutton);
	m_pSwitch->SetFontColorSchema(VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.AXISCOLOR,VipColor.TXTCOLOR,VipColor.KNCOLOR,VipColor.LEVELCOLOR,RGB(128,0,0),12,SONG_FONT);
	m_pSwitch->Create(_T("NewZfjsTab"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),m_pView,ID_FXTSWITCHBAR1);
}

UFxt::~UFxt()
{
	//删除大盘风向
	if (m_pDPFXIndex)
	{
		for (int i=0;i<m_nRegion;i++ )
		{
			for (int j=0;j<m_aRegion[i].nObject;j++ )
			{
				TOBJECT *p = m_aRegion[i].aIndexInfo[j];
				if ( p->nSet == KL_OBJECT )
				{//不用本区域的比例等参数
					TINDEXINFO * pIndex = g_pCalc->_CallDTopViewCalc_8(KL_OBJECT,p->nGSIndex);
					if(pIndex->nIndexNo==m_pDPFXIndex->nIndexNo)
						DelZs();
				}
			}
		}
	}
	//保存叠加
	int i=0;
	if(g_Overlap[0].nAutoType==0 && m_nOverlapNum>0)
	{
		g_Overlap[0].nAxis = m_nMainAxisStatus;
		g_Overlap[0].nNum = 0;
		for (i=1;i<m_aRegion[0].nObject;i++ )
		{
			if ( m_aRegion[0].aIndexInfo[i]->nSet == HIS_DAT )
			{	
				g_Overlap[0].pOverlapStk[g_Overlap[0].nNum] = (*g_pStockIO)[m_aRegion[0].aIndexInfo[i]->nCurGP];
				g_Overlap[0].StkSetCode[g_Overlap[0].nNum] = g_Overlap[0].pOverlapStk[g_Overlap[0].nNum]->setcode;
				strncpy(g_Overlap[0].StkCode[g_Overlap[0].nNum], g_Overlap[0].pOverlapStk[g_Overlap[0].nNum]->Code,SH_CODE_LEN);
				g_Overlap[0].nNum++;
			}
		}
	}
	
	TDEL(m_pSwitch);
	TDEL(m_pTabSwitch);

	DeleteInfoDlg();

	//删除ToolTip
	m_ToolTip.DelToolTip(m_pView);

	CWnd::CancelToolTips(TRUE);
	DelObject();
	ClipCursor(NULL);
	TDEL(m_pHisData);
	TDELA(m_pTmpAnalyData);
	delete []m_pPoint;
	m_pPoint = NULL;
	TDEL(m_pOrignData);
	//只有在非定制版面的第一个窗口中才保存这些状态
	if(!m_nInCom2 && m_nWhichUnit==0)
	{		
		SaveLastValue();
		SaveSelectMany();
	}
}

//从一个区域中删除一个部件，如果区域已经删到没有任何部件，则直接去掉这个区域
void UFxt::RegionDelOne(int nNo,TOBJECT	* p,BOOL bKillSelf)
{
	if(nNo==0 && p->nSet==ZB_OBJECT)
	{
		m_bDrawMainZb = FALSE;
		if(!m_bUseFmlPack)
		{
			if(m_nInCom2) 
				m_Com2DrawMain = FALSE;
			else 
				g_GH.bDrawMainZb = FALSE;
		}
	}
	for (int i=0;i<m_aRegion[nNo].nObject;i++ )
	{
		if ( p == m_aRegion[nNo].aIndexInfo[i] )
		{	
			if ( p == m_pObject && !bKillSelf)		
				return; //原始数据不得删除
			if ( !SubObject(m_aRegion[nNo].aIndexInfo[i]) )
				return;
			memmove(m_aRegion[nNo].aIndexInfo+i,m_aRegion[nNo].aIndexInfo+i+1,(m_aRegion[nNo].nObject-i-1)*sizeof(TOBJECT *));
			memset(m_aRegion[nNo].aIndexInfo+m_aRegion[nNo].nObject-1,0,sizeof(TOBJECT *));
			-- m_aRegion[nNo].nObject;
			--i;
		}
	}
	if ( m_aRegion[nNo].nObject == 0  )
	{	
		memmove(m_aRegion+nNo,m_aRegion+nNo+1,(m_nRegion-nNo-1)*sizeof(TREGION));
		memset(m_aRegion+m_nRegion-1,0,sizeof(TREGION));
		-- m_nRegion;
		if ( m_nSelRegion >= m_nRegion || m_nSelRegion<0 ) 
			m_nSelRegion = m_nRegion-1;
		SetSwitchSel();
		m_bChgRegion = TRUE;	//区域的划分即将改变
	}
	///现在是否仍旧处于叠加股票状态
	m_bOverStk = 0;
	for ( i=1;i<m_aRegion[0].nObject;i++ )
	{
		if ( m_aRegion[0].aIndexInfo[i]->nSet == HIS_DAT )
		{	
			m_bOverStk = TRUE;
			break;
		}
	}
}

void UFxt::DelOverStk()
{
	TOBJECT	*	p;
	for (int i=1;i<m_aRegion[0].nObject;i++ )
	{
		if ( m_aRegion[0].aIndexInfo[i]->nSet == HIS_DAT )
		{	
			p = m_aRegion[0].aIndexInfo[i];
			RegionDelOne(0,p);
			--i;
		}
	}
	ProcessMsg(UM_SETAXIS,NORMAL_AXIS);
	g_Overlap[0].nNum = 0;
}

//第零个恒为当前股票,指标等恒为当前股票的
TOBJECT* UFxt::LocateObject(BYTE nSet,long nGSIndex,short nGP)
{
	TOBJECT* p = m_pObject;
	while ( p )
	{
		if ( p->nCurGP == -1 )
			p->nCurGP = m_nCurGP;
		if ( nSet == p->nSet && nGSIndex == p->nGSIndex && nGP == p->nCurGP )
		{	p->nCurGP = nGP;
			return p;
		}
		p = p->next;
	}
	return NULL;
};

TOBJECT* UFxt::AddObjectNameCode(BYTE nSet,char * acCode,char *Code,short setcode)
{
	short nGP = (Code == NULL ? -1 : g_pStockIO->GetIndex(Code,setcode) );
	TINDEXINFO  *pOneIndex = NULL;
	if ( nSet == HIS_DAT && strcmp(acCode,"STANDK")==0 )
		return AddObjectByIndex(nSet,-1,nGP);
	pOneIndex =g_pCalc->_CallDTopViewCalc_8(nSet,acCode);
	if ( !pOneIndex )	
	{
		for ( int i=0;i<g_nSpeclIndexNum;++i )
		{
			if ( strcmp(g_SpeclIndex[i].acCode,acCode) == 0 )
			{
				pOneIndex = g_SpeclIndex+i;
				nSet      = g_SpeclIndex[i].nSet;
				break;
			}
		}
		if ( !pOneIndex )	
			return NULL;
	}
	return AddObjectByIndex(nSet,pOneIndex->nIndexNo,nGP);
}

TOBJECT* UFxt::AddObjectByName(BYTE nSet,char * acCode,short nGP )
{
	TINDEXINFO  *pOneIndex = NULL;
	if ( nSet == HIS_DAT && strcmp(acCode,"STANDK")==0 )
		return AddObjectByIndex(nSet,-1,nGP);
	pOneIndex = g_pCalc->_CallDTopViewCalc_8(nSet,acCode);
	if ( !pOneIndex )	
	{
		for ( int i=0;i<g_nSpeclIndexNum;++i )
		{
			if ( strcmp(g_SpeclIndex[i].acCode,acCode) == 0 )
			{
				pOneIndex = g_SpeclIndex+i;
				nSet      = g_SpeclIndex[i].nSet;
				break;
			}
		}
		if(!pOneIndex)
		{
			if(stricmp(acCode,m_strZbMain)==0) //如果是主图指标,缺省为MA
			{
				pOneIndex = g_pCalc->_CallDTopViewCalc_8(nSet,"MA");
				strcpy(m_strZbMain,"MA");
				if(m_nInCom2) strcpy(m_strCom2ZbMain,"MA");
				else strcpy(g_strZbMain,"MA");
			}
			else
				pOneIndex = g_pCalc->_CallDTopViewCalc_8(nSet,"VOL");
		}
	}
	return AddObjectByIndex(nSet,pOneIndex?(pOneIndex->nIndexNo):0,nGP);
}

TOBJECT* UFxt::AddObjectByIndex(BYTE nSet,long nGSIndex,short nGP )
{
	TOBJECT		*p = NULL,*pSrc= NULL;
	TINDEXINFO  *pOneIndex = NULL;
	if ( nGP == -1 )	
		nGP = m_nCurGP;		//最初构造时,m_nCurGP也可能为-1.
	//排除不明公式
	if ( nSet != HIS_DAT && nGSIndex == -1 )
		return NULL;
	if ( (p = LocateObject(nSet,nGSIndex,nGP)) )
	{	p->nRefTimes += 1;
		return p;
	}
	//增加新的部件
	p = new TOBJECT;	
	memset(p,0,sizeof(TOBJECT));
	p->nSet = nSet;		
	p->nGSIndex = nGSIndex;
	p->nCurGP = nGP;	//多股分析时，保存当前股票序号
	p->nRefTimes = 1;	
	p->next = NULL;
	if ( p->nSet == HIS_DAT )
	{	
		p->Property |= MainAble;
		if (m_bNowOverlap)
			strcpy(p->acCode,"OVERLAPK");	//股票迭加
		else
			strcpy(p->acCode,"STANDK");
		p->nOutNum = 1;
	}
	else if ( p->nSet == SPECLINE_OBJECT )
	{
		pOneIndex = g_SpeclIndex+nGSIndex;
		strcpy(p->acCode,pOneIndex->acCode);
	}
	else if ( p->nSet == GBZS_OBJECT )
	{
		strcpy(p->acCode,"GBBQ");
		p->nOutNum = 2;
	}
	else if( p->nSet == GGBW_OBJECT )
	{
		strcpy(p->acCode,"GGBW");
		p->nOutNum = 2;
	}
	else if( p->nSet == FC_OBJECT )
	{
		strcpy(p->acCode,"FC");
		p->nOutNum = 2;
	}
	else if ( p->nSet == THUN_OBJECT )
	{
		strcpy(p->acCode,"THUNDER");
		p->nOutNum = 3;
	}
	else 
	{	
		pOneIndex = g_pCalc->_CallDTopViewCalc_8(nSet,nGSIndex);
		strcpy(p->acCode,pOneIndex->acCode);
		p->Property = pOneIndex->nDrawMode;
		p->nOutNum = pOneIndex->nLineNum;
		if ( pOneIndex->nAddXAxisNum>0 )
			p->Property |= EXTRA_Y;
		if ( pOneIndex->nAxisType != 0 )
			p->Property |= DEFINE_Y;
		if ( pOneIndex->nDrawMode & PUCULINE )
			p->Property |= PUCU_LINE;
		else if ( pOneIndex->nDrawMode & SARLINE )
			p->Property |= SAR_LINE;
		else if ( pOneIndex->nDrawMode & BASEZERO )
			p->Property |= BASE_ZERO;
		if ( p->nSet == EXP_OBJECT )
		{	
			m_bExistZS = TRUE;
			m_ExpZS.p = p;
			_snprintf(m_ExpZS.strInfo,40,"交易系统:%s\r\n%s",pOneIndex->acCode,pOneIndex->acName);
		}
		else if ( p->nSet == KL_OBJECT )
		{	m_bExistZS = TRUE;
			m_KLZS.p = p ;
			_snprintf(m_KLZS.strInfo,40,"五彩K线:%s\r\n%s",pOneIndex->acCode,pOneIndex->acName);
		}
	}
	for ( pSrc=m_pObject; pSrc && pSrc->next != NULL; pSrc = pSrc->next );	
	if ( pSrc )
		pSrc->next = p;
	else
	{	
		m_pObject = p;
		m_pObject->nSet = HIS_DAT;		//第一个一定放数据
	}
	return p;
}

//删除以后，不要改变循序,Region 与 Object中的nIndex要保持
BOOL UFxt::SubObject(TOBJECT* p)					
{
	if ( !p )	return TRUE;
	if ( p == m_pObject )	//第一个部件不能删除,不论画什么，数据是历史数据
		return FALSE;
	//当所有的引用的除去后，方可删除
	if ( p->nRefTimes > 0 )			
		p->nRefTimes --;		//减少计数
	if ( p->nRefTimes == 0 )
	{   //当所有的区域都不引用此部件时，删除此部件
		TOBJECT* pSrc = m_pObject;
		while ( pSrc->next )	
		{	
			if ( pSrc->next == p )
			{	
				pSrc->next = p->next;	//断开链表
				if ( p->nSet == HIS_DAT || p->nSet == SEPCLINE_TYPE )
				{	
					TDEL(p->pHisDay);	
				}
				else
				{
					for ( int k=0;k<MAX_LINEOUT;k++ )
						g_pCalc->_CallDTopViewCalc_14(p->pCalcData[k]);
				}
				if (m_nOverlapNum != 0 && strcmp(p->acCode, "OVERLAPK") == 0) 
					m_nOverlapNum--;
				TDEL(p);
				break;
			}
			pSrc = pSrc->next;
		}
	}
	return TRUE;
}

//假设：数据个数、空间大小已经在RejustAllObject确定
void UFxt::RejustOneObject(TOBJECT * p,int nArrayLen,BOOL bTickCount)
{
	int nSpaceLen = nArrayLen;
	//数据预留空间
	nSpaceLen += RECENTNUM+250; //多预留空间 至少要保证RECENTNUM+250个空间
	if(p)
	{
		if(p->nSet == HIS_DAT || p->nSet == SEPCLINE_TYPE)
		{
			TDEL(p->pHisDay);
			p->pHisDay = new AnalyData[nSpaceLen];
			memset(p->pHisDay,0,nSpaceLen*sizeof(struct AnalyData));
		}
		else 
		{
			int k=0;
			for(k=0;k<MAX_LINEOUT;k++)
			{
				if(p->pCalcData[k]) 
					g_pCalc->_CallDTopViewCalc_14(p->pCalcData[k]);
			}
			for(k=0;k<max(6,p->nOutNum);k++)	//至少有6个输出，供交易系统使用
			{
				p->pCalcData[k] = g_pCalc->_CallDTopViewCalc_13(nSpaceLen);
				TSET_NULL(p->pCalcData[k],0,nSpaceLen);
			}
		}
		p->nArrayLen = nArrayLen;
		p->nPeriod = m_nPeriod;
	}
}

//换股票是调用的函数,只需预给一个m_nDataNum，
void UFxt::RejustAllObject(int nArrayLen)
{
	int i=0,nOldGP = m_pObject[0].nCurGP;
	TOBJECT * pSrc = m_pObject;
	////////////////////换当前股票
	m_pObject[0].nCurGP = m_nCurGP;		
	delete [] m_pAllPosOfX;m_pAllPosOfX=NULL;
	delete [] m_pXRule;m_pXRule=NULL;
	m_nXAxis = 0;
	m_pAllPosOfX = new long[nArrayLen];		
	memset(m_pAllPosOfX,0,nArrayLen*sizeof(long));
	m_pPosOfX = m_pAllPosOfX;
	m_pXRule  = new long[nArrayLen];
	memset(m_pXRule,0,nArrayLen*sizeof(long));
	while(pSrc)
	{	RejustOneObject(pSrc,nArrayLen);
		if ( pSrc->nCurGP == nOldGP && stricmp(pSrc->acCode,"OVERLAPK") !=0  )
			pSrc->nCurGP = m_nCurGP;
		pSrc = pSrc->next;
	}
}

void UFxt::DelObject()
{
	TOBJECT* p = m_pObject;
	while ( p )				
	{	//删除所有部件，包括其中的数据缓冲
		if ( p->nSet == HIS_DAT || p->nSet == SEPCLINE_TYPE )
		{	TDEL(p->pHisDay);	}
		else
		{	
			for ( int i=0;i<MAX_LINEOUT;i++ )
				g_pCalc->_CallDTopViewCalc_14(p->pCalcData[i]);
		}
		p = p->next;
		TDEL(m_pObject);
		m_pObject = p;
	}
	TDEL(m_pAllPosOfX);
	m_pPosOfX = NULL;
	TDEL(m_pXRule);
}

//仅仅计算某部件
BOOL UFxt::CalcOne(TOBJECT * p)
{
	int i = 0,j = 0;
	if ( p == NULL )	return FALSE;
	switch ( p->nSet )
	{
	case ZB_OBJECT:		 
	case TJ_OBJECT:
	case EXP_OBJECT:
	case KL_OBJECT:
		if ( p->nCurGP == -1 || p->nCurGP == m_nCurGP )
			g_pCalc->_CallDTopViewCalc_3(m_pStkInfo->Code,m_pStkInfo->setcode,m_nPeriod,m_pHisData,m_nDataNum); 
		else
		{
			TOBJECT * ppp;
			char	* Code = (*g_pStockIO)[p->nCurGP]->Code;
			short	setcode= (*g_pStockIO)[p->nCurGP]->setcode;
			HISDAT	* tmp = new HISDAT[m_nDataNum+600];
			memset(tmp,0,sizeof(HISDAT)*(m_nDataNum+600));
			ppp = m_pObject->next;
			while ( ppp )
			{
				if ( ppp->nSet == HIS_DAT && ppp->nCurGP == p->nCurGP )
				{
					RawData2CalcData(Code,setcode,m_nPeriod,ppp->pHisDay,tmp,m_nDataNum,g_NoTime);
					g_pCalc->_CallDTopViewCalc_3(Code,setcode,m_nPeriod,tmp,m_nDataNum); 
					break;
				}
				ppp = ppp->next;
			}
			delete  []tmp;tmp=NULL;
		}
		if ( -1 == g_pCalc->_CallDTopViewCalc_5(p->nSet,p->nGSIndex,p->pCalcData) )
			return FALSE;
		break;
	case SEPCLINE_TYPE:
		if(m_nDataNum>0 && m_pObject!=NULL && m_pObject->pHisDay!=NULL)
			memcpy(p->pHisDay,m_pObject->pHisDay,m_nDataNum*sizeof(AnalyData));
		break;
	}
	return TRUE;
}

//只是简单的计算现有公式
BOOL UFxt::CalcAllGS()
{
	int		i = 0;
	TOBJECT * p = m_pObject;
	if ( m_nDataNum <= g_RMSp )
		return TRUE;
	if(!m_pStkInfo) return FALSE;
	g_pCalc->_CallDTopViewCalc_3(m_pStkInfo->Code,m_pStkInfo->setcode,m_nPeriod,m_pHisData,m_nDataNum);
	while( p )
	{	CalcOne(p);
		p = p->next;
	}
	return TRUE;
}

void UFxt::CalcR100_AXIS(int nRegion)
{
	int		i,j,k;
	memset(&m_R100_AXIS,0,sizeof(m_R100_AXIS));
	m_R100_AXIS.dTopPercent = -99999;
	m_R100_AXIS.dTipPercent =  99999;
	if ( m_aRegion[nRegion].nObject < 1 )
		return;
	AnalyData	* pHis = NULL;
	TOBJECT	    * pObject = NULL;
	for ( i=0;i<m_aRegion[nRegion].nObject;++i )
	{
		pObject = m_aRegion[nRegion].aIndexInfo[i];
		if ( pObject->nSet == HIS_DAT )
		{
			k = 0;
			pHis = pObject->pHisDay+m_nLeftNo;
			while ( memcmp(pHis+k,&l_InValidHisDat,sizeof(AnalyData)) == 0 )
				++k;
			for ( j=0;j<m_nDrawNum;++j )
			{
				if ( memcmp(pHis+j,&l_InValidHisDat,sizeof(AnalyData)) == 0 )
					continue;
				if ( m_R100_AXIS.dTopPercent < (pHis[j].High-pHis[k].Open)/pHis[k].Open )
					m_R100_AXIS.dTopPercent = (pHis[j].High-pHis[k].Open)/pHis[k].Open;
				if ( m_R100_AXIS.dTipPercent > (pHis[j].Low-pHis[k].Open)/pHis[k].Open )
					m_R100_AXIS.dTipPercent = (pHis[j].Low-pHis[k].Open)/pHis[k].Open;
			}
		}
	}
	//在此修改一下原来算法的百分比问题
	double	dNowMax , dNowMin;
	pHis = m_aRegion[nRegion].aIndexInfo[0]->pHisDay+m_nLeftNo ;
	dNowMax = 1.01*(m_R100_AXIS.dTopPercent*pHis[0].Open + pHis[0].Open);
	dNowMin = 0.99*(m_R100_AXIS.dTipPercent*pHis[0].Open + pHis[0].Open);
	if ( m_aRegion[nRegion].dMaxValEx < dNowMax )
	{
		m_aRegion[nRegion].dMaxValEx = 
		m_aRegion[nRegion].dMaxVal   = dNowMax;
	}
	if ( m_aRegion[nRegion].dMinValEx > dNowMin )
	{
		m_aRegion[nRegion].dMinValEx = 
		m_aRegion[nRegion].dMinVal   = dNowMin;
	}
	m_aRegion[nRegion].dPerUnit = ( m_aRegion[nRegion].DrawRc.bottom - 
		m_aRegion[nRegion].DrawRc.top )/(m_aRegion[nRegion].dMaxVal - m_aRegion[nRegion].dMinVal);
	m_aRegion[nRegion].dPerUnitEx = ( m_aRegion[nRegion].DrawRc.bottom - 
		m_aRegion[nRegion].DrawRc.top )/(m_aRegion[nRegion].dMaxValEx - m_aRegion[nRegion].dMinValEx);
	m_R100_AXIS.dTopPercent = m_aRegion[nRegion].dMaxValEx/pHis[0].Open-1;
	m_R100_AXIS.dTipPercent = m_aRegion[nRegion].dMinValEx/pHis[0].Open-1;

	m_R100_AXIS.dRatio = ( m_aRegion[nRegion].DrawRc.bottom - 
		m_aRegion[nRegion].DrawRc.top )/(m_R100_AXIS.dTopPercent - m_R100_AXIS.dTipPercent);
}

int	 UFxt::GetYPosR100_AXIS(int nRgn,double fY,int nNo)
{
	int			y = 0,i=0;
	if(m_nDataNum<=0 || m_pObject==NULL || m_pObject->pHisDay==NULL) return m_aRegion[nRgn].DrawRc.bottom;
	AnalyData	* pHis = m_pObject->pHisDay+m_nLeftNo;
	if ( m_aRegion[nRgn].aIndexInfo[nNo]->nSet != HIS_DAT || nNo == 0 )
	{
		fY = m_R100_AXIS.dRatio * ( (fY -  pHis[0].Open)/pHis[0].Open - m_R100_AXIS.dTipPercent );
		y = m_aRegion[nRgn].DrawRc.bottom - fY +0.5;
	}
	else if ( m_aRegion[nRgn].aIndexInfo[nNo]->nSet == HIS_DAT )
	{
		i = 0;
		pHis = m_aRegion[nRgn].aIndexInfo[nNo]->pHisDay + m_nLeftNo;
		while ( memcmp(pHis+i,&l_InValidHisDat,sizeof(AnalyData)) == 0 && i<m_nDrawNum )
			++i;
		fY = m_R100_AXIS.dRatio * ( (fY -  pHis[i].Open)/pHis[i].Open - m_R100_AXIS.dTipPercent );
		y = m_aRegion[nRgn].DrawRc.bottom - fY +0.5;
	}
	return y;
}

//调整一个区域的比例等
BOOL UFxt::CalcOneRegionRatio(int nRegionNo)
{
	if ( m_aRegion[nRegionNo].nObject < 1 )
		return FALSE;
	m_aRegion[nRegionNo].dMaxVal = m_aRegion[nRegionNo].aIndexInfo[0]->dMaxVal;
	m_aRegion[nRegionNo].dMinVal = m_aRegion[nRegionNo].aIndexInfo[0]->dMinVal;
	TOBJECT  * p = m_aRegion[nRegionNo].aIndexInfo[0];
	TINDEXINFO * pIndex = g_pCalc->_CallDTopViewCalc_8(ZB_OBJECT,p->nGSIndex);
	if ( p->nSet == ZB_TYPE 
		&& ( (pIndex->nDrawMode & KOverlay) || (pIndex->nDrawMode & UOverlay) || (pIndex->nDrawMode & COverlay))
		 )
	{
		if ( m_aRegion[nRegionNo].dMaxVal < m_pObject->dMaxVal )
			m_aRegion[nRegionNo].dMaxVal =  m_pObject->dMaxVal;
		if ( m_aRegion[nRegionNo].dMinVal > m_pObject->dMinVal )
			m_aRegion[nRegionNo].dMinVal =  m_pObject->dMinVal;
	}
	for (int i=1;i<m_aRegion[nRegionNo].nObject;i++ )
	{ 
		if ( m_aRegion[nRegionNo].aIndexInfo[i]->nSet == HIS_DAT || 
			(m_aRegion[nRegionNo].aIndexInfo[i]->Property &OnlyMain ) )
		{	//主图叠加，必须调整坐标
			if ( m_nMainAxisStatus == R100_AXIS && m_aRegion[nRegionNo].aIndexInfo[i]->nSet == HIS_DAT )
				continue;
			else if ( m_aRegion[nRegionNo].aIndexInfo[i]->nSet == ZB_OBJECT
				|| m_aRegion[nRegionNo].aIndexInfo[i]->nSet == HIS_DAT )
			{	
				if ( m_aRegion[nRegionNo].dMaxVal < m_aRegion[nRegionNo].aIndexInfo[i]->dMaxVal
					&& fabs(m_aRegion[nRegionNo].aIndexInfo[i]->dMaxVal-DMAXVAL) > 1 )
					m_aRegion[nRegionNo].dMaxVal = m_aRegion[nRegionNo].aIndexInfo[i]->dMaxVal;
				if ( m_aRegion[nRegionNo].dMinVal > m_aRegion[nRegionNo].aIndexInfo[i]->dMinVal
					&& fabs(m_aRegion[nRegionNo].aIndexInfo[i]->dMinVal-DMINVAL)>1 )
					m_aRegion[nRegionNo].dMinVal = m_aRegion[nRegionNo].aIndexInfo[i]->dMinVal;
			}
		}
	}
	//提升VOLSTICK功能,削减BASE_ZERO属性
	m_aRegion[nRegionNo].dMinVal -= 0.02*(m_aRegion[nRegionNo].dMaxVal-m_aRegion[nRegionNo].dMinVal);
	//作一个规范,以便减小精度误差,成交量和成交金额没有必要进行这样的处理
	//座标范围:+1 ~ -1;或者+(abs(max)+abs(min))/2000 ~ -(abs(max)+abs(min))/2000
	double absmax=0,absmin=0;
	if(m_aRegion[nRegionNo].dMaxVal-m_aRegion[nRegionNo].dMinVal>-1 && m_aRegion[nRegionNo].dMaxVal-m_aRegion[nRegionNo].dMinVal < 2)
	{
		absmax = abs(m_aRegion[nRegionNo].dMaxVal);
		absmin = abs(m_aRegion[nRegionNo].dMinVal);
		if(absmax+absmin<2000)	//绝对值在1001以下
		{
			if(m_aRegion[nRegionNo].dMaxVal-m_aRegion[nRegionNo].dMinVal<(absmax+absmin)/1000)
			{
				m_aRegion[nRegionNo].dMaxVal = (m_aRegion[nRegionNo].dMaxVal+m_aRegion[nRegionNo].dMinVal)/2+max((absmax+absmin)/2000,COMPPREC);
				m_aRegion[nRegionNo].dMinVal = (m_aRegion[nRegionNo].dMaxVal+m_aRegion[nRegionNo].dMinVal)/2-max((absmax+absmin)/2000,COMPPREC);
			}
		}
		else	//这种情况下,最大最小值肯定同号
		{
			m_aRegion[nRegionNo].dMaxVal = (m_aRegion[nRegionNo].dMaxVal+m_aRegion[nRegionNo].dMinVal)/2+1;
			m_aRegion[nRegionNo].dMinVal = (m_aRegion[nRegionNo].dMaxVal+m_aRegion[nRegionNo].dMinVal)/2-1;
		}
	}

	if ( (m_aRegion[nRegionNo].dMaxVal - m_aRegion[nRegionNo].dMinVal) > COMPPREC )
		m_aRegion[nRegionNo].dPerUnit = ( m_aRegion[nRegionNo].DrawRc.bottom - m_aRegion[nRegionNo].DrawRc.top )/(m_aRegion[nRegionNo].dMaxVal - m_aRegion[nRegionNo].dMinVal);
	else
	{
		double	dV  = m_aRegion[nRegionNo].dMaxVal - m_aRegion[nRegionNo].dMinVal;
		if(dV < COMPPREC) dV = COMPPREC; //dV不能为0;
		m_aRegion[nRegionNo].dMaxVal += 0.1*dV;
		m_aRegion[nRegionNo].dMinVal -= 0.1*dV;
		m_aRegion[nRegionNo].dPerUnit = ( m_aRegion[nRegionNo].DrawRc.bottom - m_aRegion[nRegionNo].DrawRc.top )/(m_aRegion[nRegionNo].dMaxVal - m_aRegion[nRegionNo].dMinVal);
	}
	m_aRegion[nRegionNo].dMaxValEx = m_aRegion[nRegionNo].dMaxVal;	//备用
	m_aRegion[nRegionNo].dMinValEx = m_aRegion[nRegionNo].dMinVal;
	m_aRegion[nRegionNo].dPerUnitEx= m_aRegion[nRegionNo].dPerUnit;
	m_bR100Dot	=	0;
	//1、K线、美国线才能换坐标
	//2、主图才能换坐标
	if ( nRegionNo == 0	&& m_nMainAxisStatus != NORMAL_AXIS )
	{	//备用的不变
		switch ( m_nMainAxisStatus )
		{
		case LOG10_AXIS:
			m_aRegion[nRegionNo].dMaxVal   = log10(max(COMPPREC,m_aRegion[nRegionNo].dMaxVal));
			m_aRegion[nRegionNo].dMinVal   = log10(max(COMPPREC,m_aRegion[nRegionNo].dMinVal));
			if ( (m_aRegion[nRegionNo].dMaxVal - m_aRegion[nRegionNo].dMinVal) > COMPPREC )
				m_aRegion[nRegionNo].dPerUnit  = ( m_aRegion[nRegionNo].DrawRc.bottom - m_aRegion[nRegionNo].DrawRc.top )/(m_aRegion[nRegionNo].dMaxVal - m_aRegion[nRegionNo].dMinVal);
			else
			{
				if(m_aRegion[nRegionNo].dMaxVal<m_aRegion[nRegionNo].dMinVal)
					m_aRegion[nRegionNo].dMaxVal=m_aRegion[nRegionNo].dMinVal+COMPPREC;
				double	dV  = m_aRegion[nRegionNo].dMaxVal - m_aRegion[nRegionNo].dMinVal;
				if(dV < COMPPREC) dV = COMPPREC; //dV不能为0;
				m_aRegion[nRegionNo].dMaxVal += 0.1*dV;
				m_aRegion[nRegionNo].dMinVal -= 0.1*dV;
			}
			break;
		case R100_AXIS:
			{
				if(m_nDataNum>0 && m_pObject!=NULL && m_pObject->pHisDay!=NULL)
				{
					double tmpR100Max = 100*(m_aRegion[nRegionNo].dMaxVal - m_pObject->pHisDay[m_nLeftNo].Close)/m_aRegion[nRegionNo].dMaxVal;
					double tmpR100Min = 100*(m_aRegion[nRegionNo].dMinVal - m_pObject->pHisDay[m_nLeftNo].Close)/m_aRegion[nRegionNo].dMaxVal;
					if ( fabs(tmpR100Max)<10.0 && fabs(tmpR100Min)<10.0 )
						m_bR100Dot = 1;	//是否用小数点
					else
						m_bR100Dot = 0;

					CalcR100_AXIS(nRegionNo);
				}
			}
			break;
		}
	}
	return TRUE;
}

//找到每个部件的最值
BOOL UFxt::CalcOneGSRatio(TOBJECT * p,int Off)
{
	if(Off<0) return FALSE; //必须判断Off不为负,否则对于多股时会出问题
	HandleValue *phv = NULL;
	p->dMaxVal = DMAXVAL;		
	p->dMinVal = DMINVAL;
	for ( int i=0;i<m_nDrawNum;i++ )		//在当前所画的数据
	{	
		if ( p->nSet == HIS_DAT || p->nSet == SEPCLINE_TYPE )
		{
			if ( memcmp(p->pHisDay+Off+i,&l_InValidHisDat,sizeof(AnalyData)) != 0 )
			{
				if ( p->pHisDay[Off+i].High > p->dMaxVal )	
				{	p->dMaxVal = p->pHisDay[Off+i].High;
					p->nMaxPos[0] = i;			//以便直接从m_pPosOfX中取
				}
				if ( p->pHisDay[Off+i].Low  < p->dMinVal ) 
				{	p->dMinVal = p->pHisDay[Off+i].Low;
					p->nMinPos[0] = i;
				}
			}
		}
		else if ( p->nSet == ZB_TYPE && p->Property & PUCU_LINE )
		{	//仅仅依据第一个计算
			if ( TBE_MEANLESS(p->pCalcData[0][Off+i]) !=0 && p->pCalcData[0][Off+i] > p->dMaxVal )	//快速比较	
				p->dMaxVal = p->pCalcData[0][Off+i];
			if ( TBE_MEANLESS(p->pCalcData[0][Off+i]) !=0 && p->pCalcData[0][Off+i] < p->dMinVal ) 
				p->dMinVal = p->pCalcData[0][Off+i];
		}
		else
		{
			TINDEXINFO * pIndex = g_pCalc->_CallDTopViewCalc_8(ZB_OBJECT,p->nGSIndex);
			if(pIndex == NULL) continue;
			for (int j=0;j< p->nOutNum;j++ )
			{	
				if ( pIndex->aLine[j].nType == NODRAW )	//临时使用，解决李的不用显示的问题。
					continue;
				HandleValue * phv = NULL;
				if(TBE_MEANLESS(p->pCalcData[j][m_nDataNum])) 
				{
					DWORD pdVal;
					memcpy(&pdVal, &p->pCalcData[j][m_nDataNum], sizeof(DWORD));
					phv = (HandleValue *)pdVal;
					if(phv->nValidFlag!=TMEANLESS_INT)
						phv = NULL;
				}
				
				if( phv&&phv->nHandleType == 'k' )
				{
					if ( TBE_MEANLESS(phv->pFloat[Off+i]) !=0 && phv->pFloat[Off+i] > p->dMaxVal )	//快速比较	
						p->dMaxVal = phv->pFloat[Off+i];
					if ( TBE_MEANLESS(phv->pFloat[m_nDataNum+Off+i]) !=0 && phv->pFloat[m_nDataNum+Off+i] > p->dMaxVal )	//快速比较	
						p->dMaxVal = phv->pFloat[m_nDataNum+Off+i];
					if ( TBE_MEANLESS(phv->pFloat[2*m_nDataNum+Off+i]) !=0 && phv->pFloat[2*m_nDataNum+Off+i] > p->dMaxVal )	//快速比较	
						p->dMaxVal = phv->pFloat[2*m_nDataNum+Off+i];
					if ( TBE_MEANLESS(phv->pFloat[3*m_nDataNum+Off+i]) !=0 && phv->pFloat[3*m_nDataNum+Off+i] > p->dMaxVal )	//快速比较	
						p->dMaxVal = phv->pFloat[3*m_nDataNum+Off+i];
					if ( TBE_MEANLESS(phv->pFloat[Off+i]) !=0 && phv->pFloat[Off+i] < p->dMinVal ) 
						p->dMinVal = phv->pFloat[Off+i];
					if ( TBE_MEANLESS(phv->pFloat[m_nDataNum+Off+i]) !=0 && phv->pFloat[m_nDataNum+Off+i] < p->dMinVal ) 
						p->dMinVal = phv->pFloat[m_nDataNum+Off+i];
					if ( TBE_MEANLESS(phv->pFloat[2*m_nDataNum+Off+i]) !=0 && phv->pFloat[2*m_nDataNum+Off+i] < p->dMinVal ) 
						p->dMinVal = phv->pFloat[2*m_nDataNum+Off+i];
					if ( TBE_MEANLESS(phv->pFloat[3*m_nDataNum+Off+i]) !=0 && phv->pFloat[3*m_nDataNum+Off+i] < p->dMinVal ) 
						p->dMinVal = phv->pFloat[3*m_nDataNum+Off+i];
					continue;
				}
				else if( phv&&phv->nHandleType == 'K' )
				{
					if ( TBE_MEANLESS(phv->pFloat[m_nDataNum+Off+i]) !=0 && phv->pFloat[m_nDataNum+Off+i] > p->dMaxVal )	//快速比较	
						p->dMaxVal = phv->pFloat[m_nDataNum+Off+i];
					if ( TBE_MEANLESS(phv->pFloat[2*m_nDataNum+Off+i]) !=0 && phv->pFloat[2*m_nDataNum+Off+i] > p->dMaxVal )	//快速比较	
						p->dMaxVal = phv->pFloat[2*m_nDataNum+Off+i];
					if ( TBE_MEANLESS(phv->pFloat[3*m_nDataNum+Off+i]) !=0 && phv->pFloat[3*m_nDataNum+Off+i] > p->dMaxVal )	//快速比较	
						p->dMaxVal = phv->pFloat[3*m_nDataNum+Off+i];
					if ( TBE_MEANLESS(phv->pFloat[4*m_nDataNum+Off+i]) !=0 && phv->pFloat[4*m_nDataNum+Off+i] > p->dMaxVal )	//快速比较	
						p->dMaxVal = phv->pFloat[4*m_nDataNum+Off+i];
					if ( TBE_MEANLESS(phv->pFloat[m_nDataNum+Off+i]) !=0 && phv->pFloat[m_nDataNum+Off+i] < p->dMinVal ) 
						p->dMinVal = phv->pFloat[m_nDataNum+Off+i];
					if ( TBE_MEANLESS(phv->pFloat[2*m_nDataNum+Off+i]) !=0 && phv->pFloat[2*m_nDataNum+Off+i] < p->dMinVal ) 
						p->dMinVal = phv->pFloat[2*m_nDataNum+Off+i];
					if ( TBE_MEANLESS(phv->pFloat[3*m_nDataNum+Off+i]) !=0 && phv->pFloat[3*m_nDataNum+Off+i] < p->dMinVal ) 
						p->dMinVal = phv->pFloat[3*m_nDataNum+Off+i];
					if ( TBE_MEANLESS(phv->pFloat[4*m_nDataNum+Off+i]) !=0 && phv->pFloat[4*m_nDataNum+Off+i] < p->dMinVal ) 
						p->dMinVal = phv->pFloat[4*m_nDataNum+Off+i];
					continue;
				}
				else if( phv&&phv->nHandleType == 'l' )
				{
					if ( TBE_MEANLESS(phv->pFloat[m_nDataNum+Off+i]) !=0 && TBE_MEANLESS(phv->pFloat[2*m_nDataNum+Off+i]) !=0 
						&& max(phv->pFloat[m_nDataNum+Off+i],phv->pFloat[2*m_nDataNum+Off+i]) > p->dMaxVal )	//快速比较	
						p->dMaxVal = max(phv->pFloat[m_nDataNum+Off+i],phv->pFloat[2*m_nDataNum+Off+i]);
					if ( TBE_MEANLESS(phv->pFloat[m_nDataNum+Off+i]) !=0 && TBE_MEANLESS(phv->pFloat[2*m_nDataNum+Off+i]) !=0 
						&& min(phv->pFloat[m_nDataNum+Off+i],phv->pFloat[2*m_nDataNum+Off+i]) < p->dMinVal )	//快速比较	
						p->dMinVal = min(phv->pFloat[m_nDataNum+Off+i],phv->pFloat[2*m_nDataNum+Off+i]);
					continue;
				}
				else if( phv&&phv->nHandleType == 'i' )
				{
					if ( TBE_MEANLESS(phv->pFloat[m_nDataNum+Off+i]) !=0 && phv->pFloat[m_nDataNum+Off+i] > p->dMaxVal )	//快速比较	
						p->dMaxVal = phv->pFloat[m_nDataNum+Off+i];
					if ( TBE_MEANLESS(phv->pFloat[m_nDataNum+Off+i]) !=0 && phv->pFloat[m_nDataNum+Off+i] < p->dMinVal ) 
						p->dMinVal = phv->pFloat[m_nDataNum+Off+i];
					continue;
				}
				else if( phv&&phv->nHandleType == 'I' )
				{
					if ( TBE_MEANLESS(phv->pFloat[m_nDataNum+Off+i]) !=0 && phv->pFloat[m_nDataNum+Off+i] > p->dMaxVal )	//快速比较	
						p->dMaxVal = phv->pFloat[m_nDataNum+Off+i];
					if ( TBE_MEANLESS(phv->pFloat[m_nDataNum+Off+i]) !=0 && phv->pFloat[m_nDataNum+Off+i] < p->dMinVal ) 
						p->dMinVal = phv->pFloat[m_nDataNum+Off+i];
					continue;
				}
				else if( phv&&phv->nHandleType == 't' )
				{
					if ( TBE_MEANLESS(phv->pFloat[m_nDataNum+Off+i]) !=0 && phv->pFloat[m_nDataNum+Off+i] > p->dMaxVal )	//快速比较	
						p->dMaxVal = phv->pFloat[m_nDataNum+Off+i];
					if ( TBE_MEANLESS(phv->pFloat[m_nDataNum+Off+i]) !=0 && phv->pFloat[m_nDataNum+Off+i] < p->dMinVal ) 
						p->dMinVal = phv->pFloat[m_nDataNum+Off+i];
					continue;
				}
				else if( phv&&phv->nHandleType == 'D' )
				{
					if ( TBE_MEANLESS(phv->pFloat[Off+i]) !=0 && phv->pFloat[Off+i] > p->dMaxVal )	//快速比较	
						p->dMaxVal = phv->pFloat[Off+i];
					if ( TBE_MEANLESS(phv->pFloat[Off+i]) !=0 && phv->pFloat[Off+i] < p->dMinVal )	//快速比较	
						p->dMinVal = phv->pFloat[Off+i];
					if ( TBE_MEANLESS(phv->pFloat[2*m_nDataNum+Off+i]) !=0 && phv->pFloat[2*m_nDataNum+Off+i] > p->dMaxVal ) 
						p->dMaxVal = phv->pFloat[2*m_nDataNum+Off+i];
					if ( TBE_MEANLESS(phv->pFloat[2*m_nDataNum+Off+i]) !=0 && phv->pFloat[2*m_nDataNum+Off+i] < p->dMinVal ) 
						p->dMinVal = phv->pFloat[2*m_nDataNum+Off+i];
					continue;
				}
				else if( phv&&phv->nHandleType == 'T' )
				{
					continue;
				}
				if ( TBE_MEANLESS(p->pCalcData[j][Off+i]) !=0 && p->pCalcData[j][Off+i] > p->dMaxVal )	//快速比较	
					p->dMaxVal = p->pCalcData[j][Off+i];
				if ( TBE_MEANLESS(p->pCalcData[j][Off+i]) !=0 && p->pCalcData[j][Off+i] < p->dMinVal ) 
					p->dMinVal = p->pCalcData[j][Off+i];
				if ( pIndex->aLine[j].nType == VOLSTICK )	//临时使用，解决李的不用显示的问题
				{
					if(p->dMinVal>0) p->dMinVal=0;
					if(p->dMaxVal<0) p->dMaxVal=0;
				}
			}
		}
		/*
		else if(p->nSet == ZB_TYPE)
		{	
			TINDEXINFO * pIndex = g_pCalc->_CallDTopViewCalc_8(ZB_OBJECT,p->nGSIndex);
			for (int j=0;j< p->nOutNum;j++ )
			{	
				if ( pIndex->aLine[j].nType == NODRAW )	//临时使用，解决不用显示的问题
					continue;
				if( aDrawType[j]>0)
				{
					switch(aDrawType[j])
					{
					case DRAW_STICKLINE:						
						if ( TBE_MEANLESS(p->pCalcData[j][m_nDataNum+Off+i]) !=0 && TBE_MEANLESS(p->pCalcData[j][2*m_nDataNum+Off+i]) !=0 &&
							max(p->pCalcData[j][m_nDataNum+Off+i], p->pCalcData[j][2*m_nDataNum+Off+i]) > p->dMaxVal )	//快速比较	
							p->dMaxVal = max(p->pCalcData[j][m_nDataNum+Off+i], p->pCalcData[j][2*m_nDataNum+Off+i]);
						if ( TBE_MEANLESS(p->pCalcData[j][m_nDataNum+Off+i]) !=0 && TBE_MEANLESS(p->pCalcData[j][2*m_nDataNum+Off+i]) !=0 &&
							min(p->pCalcData[j][m_nDataNum+Off+i], p->pCalcData[j][2*m_nDataNum+Off+i]) < p->dMinVal ) 
							p->dMinVal = min(p->pCalcData[j][m_nDataNum+Off+i], p->pCalcData[j][2*m_nDataNum+Off+i]);
						break;
					case DRAW_ICON:
						if ( TBE_MEANLESS(p->pCalcData[j][m_nDataNum+Off+i]) !=0 && p->pCalcData[j][m_nDataNum+Off+i] > p->dMaxVal )	//快速比较	
							p->dMaxVal = p->pCalcData[j][m_nDataNum+Off+i];
						if ( TBE_MEANLESS(p->pCalcData[j][m_nDataNum+Off+i]) !=0 && p->pCalcData[j][m_nDataNum+Off+i] < p->dMinVal ) 
							p->dMinVal = p->pCalcData[j][m_nDataNum+Off+i];
						break;
					case DRAW_TEXT:
						if ( TBE_MEANLESS(p->pCalcData[j][m_nDataNum+Off+i]) !=0 && p->pCalcData[j][m_nDataNum+Off+i] > p->dMaxVal )	//快速比较	
							p->dMaxVal = p->pCalcData[j][m_nDataNum+Off+i];
						if ( TBE_MEANLESS(p->pCalcData[j][m_nDataNum+Off+i]) !=0 && p->pCalcData[j][m_nDataNum+Off+i] < p->dMinVal ) 
							p->dMinVal = p->pCalcData[j][m_nDataNum+Off+i];
						break;
					case DRAW_KLINE:
						if ( TBE_MEANLESS(p->pCalcData[j][Off+i]) !=0 && p->pCalcData[j][Off+i] > p->dMaxVal )	//快速比较	
							p->dMaxVal = p->pCalcData[j][Off+i];
						if ( TBE_MEANLESS(p->pCalcData[j][2*m_nDataNum+Off+i]) !=0 && p->pCalcData[j][2*m_nDataNum+Off+i] < p->dMinVal ) 
							p->dMinVal = p->pCalcData[j][2*m_nDataNum+Off+i];
						break;
					case DRAW_BAND:
						if ( TBE_MEANLESS(p->pCalcData[j][Off+i]) !=0 && p->pCalcData[j][Off+i] > p->dMaxVal )	//快速比较	
							p->dMaxVal = p->pCalcData[j][Off+i];
						if ( TBE_MEANLESS(p->pCalcData[j][Off+i]) !=0 && p->pCalcData[j][Off+i] < p->dMinVal )	//快速比较	
							p->dMinVal = p->pCalcData[j][Off+i];
						if ( TBE_MEANLESS(p->pCalcData[j][2*m_nDataNum+Off+i]) !=0 && p->pCalcData[j][2*m_nDataNum+Off+i] > p->dMaxVal ) 
							p->dMaxVal = p->pCalcData[j][2*m_nDataNum+Off+i];
						if ( TBE_MEANLESS(p->pCalcData[j][2*m_nDataNum+Off+i]) !=0 && p->pCalcData[j][2*m_nDataNum+Off+i] < p->dMinVal ) 
							p->dMinVal = p->pCalcData[j][2*m_nDataNum+Off+i];
						break;
					default:
						break;
					}
					continue;
				}
				if ( TBE_MEANLESS(p->pCalcData[j][Off+i]) !=0 && p->pCalcData[j][Off+i] > p->dMaxVal )	//快速比较	
					p->dMaxVal = p->pCalcData[j][Off+i];
				if ( TBE_MEANLESS(p->pCalcData[j][Off+i]) !=0 && p->pCalcData[j][Off+i] < p->dMinVal ) 
					p->dMinVal = p->pCalcData[j][Off+i];
				if ( pIndex->aLine[j].nType == VOLSTICK )	//临时使用，解决李的不用显示的问题
				{
					if(p->dMinVal>0) p->dMinVal=0;
					if(p->dMaxVal<0) p->dMaxVal=0;
				}
			}
		}
		*/
	}
	
	if ( p->nSet == HIS_DAT )
	{
		//注意：他是联合,不能乱改
		short	& nMax = p->nMM[0];
		short	& nMin = p->nMM[1];
		nMax = nMin = 1;
		for ( i=0;i<m_nDrawNum; ++i )
		{
			if ( memcmp(p->pHisDay+Off+i,&l_InValidHisDat,sizeof(AnalyData)) != 0 )
			{
				if ( nMax < 4 && fabs(p->pHisDay[Off+i].High - p->dMaxVal) < COMPPREC )	
					p->nMaxPos[nMax++] = i;			//以便直接从m_pPosOfX中取
				if ( nMin < 4 && fabs(p->pHisDay[Off+i].Low - p->dMinVal) < COMPPREC  ) 
					p->nMinPos[nMin++] = i;
			}
		}
	}
	return TRUE;
}

//根据本区域的大小，计算各种比例
//频繁调用,依据m_nWillNum : 希望能显示的个数
//1.计算基本公共信息
//2.计算每个的
BOOL UFxt::CalcAllRatio(BOOL bRegion,BOOL bKeepKPos)
{
	double    dXPer = 0.0;
	int		  i,Off;
	TOBJECT * p = m_pObject;
	////////////////////////////
	if ( m_nLastNo == m_nDataNum - 1 )
		m_nDrawNum = max(0,m_nWillNum-3);  //原为m_nWillNum-1,在最右端多预留一下空间
	else
		m_nDrawNum = max(0,m_nWillNum);
	////////////////////////////
	if(m_nWillNum>0) 
		dXPer = (m_rcIn.right - m_rcIn.left - FXT_RWIDTH )/(double)m_nWillNum;
	else 
		dXPer = m_rcIn.right - m_rcIn.left - FXT_RWIDTH;
	///////////////////////////////////////////////
	if ( m_nDataNum<m_nWillNum )		//数据总数不够多
	{   //这个函数没有改变WillNum，下次仍可恢复
		m_nDrawNum = m_nDataNum ;
		m_nLastNo  = max(0,m_nDataNum-1);
		//仅仅少数K线时，客户希望直接看缩小的
		dXPer = (m_rcIn.right - m_rcIn.left - FXT_RWIDTH)/(double)(m_nWillNum);
		if ( dXPer > 48 )
			dXPer = 48;
	}
	//按照比例变化,比较平滑,3:1的比例,和分析家接近
	if ( dXPer < 3 )	m_dWidth = min(1,dXPer) , m_dDistance = max(dXPer-m_dWidth,0);
	else m_dDistance = max(dXPer*0.25,2),m_dWidth = dXPer-m_dDistance;

	///////////////////////////////////////////////
	//如果仅仅处于画框架，不会进入下面分支
	if ( m_pAllPosOfX )
	{
		memset(m_pAllPosOfX,0,m_nDataNum*sizeof(long));		//全部晴空,不仅仅是要画的
		Off = (m_nLastNo+1) - m_nDrawNum;
		if( Off<0 )	//如果当前画面最终个数m_nLastNo不够
		{
			int Bak = m_nLastNo;
			m_nLastNo   = min(m_nDataNum-1,m_nLastNo-Off);	//加大右边界
			m_nDrawNum  = m_nLastNo+1;
			Off=0;
		}
		m_nLeftNo = Off;
		/////////////////////////////////////////////如果锁定
		if (!bKeepKPos && m_KLock != -1 && m_KLock < m_nDataNum )
		{
			int half = int(m_nDrawNum/2.0+0.5);			
			for ( i=0;i<m_nDrawNum;++i )
			{
				if ( memcmp(&m_KAnalyData.Time.Date,&m_pObject->pHisDay[i].Time.Date,sizeof(long)) == 0 )
				{
					m_KLock = i;
					m_nKeyNo = i;
					break;
				}
			}
			if ( m_KLock >= half && m_KLock+half < m_nDataNum )
			{	//足够定位在中间
				m_nLeftNo = m_KLock - half;
				if(!m_bKeyCross)
					m_nKeyNo  = m_KLock - half;
				m_nLastNo = m_nLeftNo + m_nDrawNum - 1;
			}
			else if ( m_KLock >= half  )
			{	//左边空间够,总数一定够 因为：m_nDataNum>=m_nWillNum
				m_nLeftNo = m_nDataNum-m_nDrawNum;
				if(!m_bKeyCross)
					m_nKeyNo  = m_nDataNum-m_nDrawNum;
				m_nLastNo = m_nDataNum - 1;
			}
			else if ( m_KLock+half < m_nDataNum )
			{	//右边空间够,总数一定够 因为：m_nDataNum>=m_nWillNum
				m_nLeftNo = 0;
				if(!m_bKeyCross)
					m_nKeyNo  = 0;
				m_nLastNo = m_nDrawNum-1;
			}
		}
		m_pPosOfX = m_pAllPosOfX + m_nLeftNo;
		for ( i=0;i<m_nDataNum-m_nLeftNo;++i ) 
			m_pPosOfX[i] = int(0.5*m_dWidth + dXPer*i);
		for ( i=0;i<m_nLeftNo;++i )
			m_pAllPosOfX[i] = int(0.5*m_dWidth - dXPer*(m_nLeftNo-i));
	}
	///////////////////////////////////////////////
	//先独立计算每个部件
	while ( p )
	{
		CalcOneGSRatio(p,m_nLeftNo);
		p = p->next;
	}
	//////////////////////////////////////////////
	if ( bRegion )
	{
		for ( i=0;i<m_nRegion;i++ )	//每个区域,考虑关系
			CalcOneRegionRatio(i);
	}
	return TRUE;
}

//平移函数 如果bNotQuick 为 FALSE，表示快速平移，不必太多计算!
BOOL UFxt::XPan(int ndNo,BOOL bNotQuick)
{
	if(!m_pAllPosOfX) return FALSE;

	int Old = m_nLeftNo,i;
	if ( ndNo == 0 )
		return FALSE;
	if ( ndNo<0 )
	{
		if ( m_nLeftNo == 0 )
			return FALSE;
		//减少左边界,同时减少右边界
		m_nLeftNo  = max(0,m_nLeftNo+ndNo);
		m_nLastNo  -= Old-m_nLeftNo;
		//如果有数据，且有空间显示数据.
		if ( m_nDataNum >= m_nWillNum && m_nWillNum > m_nDrawNum )	
		{
			Old = m_nLastNo;
			m_nLastNo   = min(m_nLastNo+m_nWillNum-m_nDrawNum,m_nDataNum-1);		//增加右边界
			m_nDrawNum += m_nLastNo - Old;
		}
	}
	else if ( ndNo>0 )
	{
		if ( m_nLeftNo+m_nDrawNum >= m_nDataNum  )
			return FALSE;
		//增加左边界,同时增加右边界
		m_nLeftNo = min(m_nDataNum-m_nDrawNum,m_nLeftNo+ndNo);	
		m_nLastNo  += m_nLeftNo - Old;
	}
	memset(m_pAllPosOfX,0,m_nDataNum*sizeof(long));		//全部晴空,不仅仅是要画的
	m_pPosOfX = m_pAllPosOfX + m_nLeftNo;
	for (  i=0;i<m_nDataNum-m_nLeftNo;++i ) 
		m_pPosOfX[i] = int(0.5*m_dWidth + (m_dWidth+m_dDistance)*i);
	for ( i=0;i<m_nLeftNo;++i )
		m_pAllPosOfX[i] = int(0.5*m_dWidth - (m_dWidth+m_dDistance)*(m_nLeftNo-i));
	//先独立计算每个部件
	TOBJECT * p = m_pObject;	
	int Off = (m_nLastNo+1)-m_nDrawNum;
	while ( p )
	{	CalcOneGSRatio(p,Off);
		p = p->next;
	}
	if ( bNotQuick )
	{
		for ( i=0;i<m_nRegion;i++ )	//每个区域,考虑关系
			CalcOneRegionRatio(i);
	}
	return TRUE;
}

//切换指标公式,主图不允许切换，只管第一个部件.
void UFxt::ChangGS(int nRegionNo,int nGSIndex,BYTE nSet,int bReCalc)
{
	//不能让不是主图指标的指标放在主图上
	if ( nRegionNo==0 && nSet == ZB_TYPE && !((*g_pCalc)[nSet][nGSIndex].nDrawMode & OnlyMain) )
		return;
	if(nSet == ZB_TYPE || nSet==SEPCLINE_TYPE)
	{
		m_lCurPackNo = -1;
		m_lBandPackNo= -1;
	}
	memset(&m_OldKeyPoint,0,sizeof(POINT));
	memset(&m_OldMousePoint,0,sizeof(POINT));
	m_bMouseCross = m_bKeyCross = 0;
	m_KLock = -1;
	int nOutNo = 0;
	BOOL bMain=FALSE;
	if ( nSet == ZB_TYPE && (*g_pCalc)[nSet][nGSIndex].nDrawMode & OnlyMain )
	{
		bMain=TRUE;
		nRegionNo = 0 , nOutNo = max(0,m_aRegion[0].nObject-1);
		if ( m_aRegion[nRegionNo].nObject > 1 )
		{	//多个输出时，才考虑
			for ( int k=0;k<m_aRegion[nRegionNo].nObject;k++ )
			{
				if ( m_aRegion[nRegionNo].aIndexInfo[k]->Property & OnlyMain && m_aRegion[nRegionNo].aIndexInfo[k]->nSet == ZB_TYPE )
				{
					int thIndex = m_aRegion[nRegionNo].aIndexInfo[k]->nGSIndex;
					RegionDelOne(nRegionNo,m_aRegion[nRegionNo].aIndexInfo[k]);
					if ( nGSIndex == thIndex  )
						return;
				}
			}
			//如果有多个，考虑加在后面
			for (; m_aRegion[nRegionNo].aIndexInfo[nOutNo]; nOutNo++);
		}
	}
	//去掉旧的公式（换公式，不换叠加的公式）
	if ( !SubObject(m_aRegion[nRegionNo].aIndexInfo[nOutNo]) )
		for (; m_aRegion[nRegionNo].aIndexInfo[nOutNo]; nOutNo++);
	TOBJECT * pobj  = AddObjectByIndex(nSet,nGSIndex,m_nCurGP);
	m_aRegion[nRegionNo].aIndexInfo[nOutNo] = pobj;
	//有可能找不到公式
	if ( m_aRegion[nRegionNo].aIndexInfo[nOutNo] )
	{	//有可能是新加的公式
		if ( ( pobj->Property & OnlyMain ) && nRegionNo == 0 )
		{
			strcpy(m_strZbMain,pobj->acCode);
			m_bDrawMainZb = TRUE;
			if(!m_bUseFmlPack)
			{
				if(m_nInCom2)
				{
					strcpy(m_strCom2ZbMain,pobj->acCode);
					m_Com2DrawMain	= TRUE;
				}
				else
				{
					strcpy(g_strZbMain,pobj->acCode);
					g_GH.bDrawMainZb = TRUE;
				}
			}
		}
		if ( !(pobj->Property&OnlyMain) && nRegionNo > 0 )
		{
			if(nRegionNo>0 && m_bDbClickFT) 
			{
				strcpy(m_strFavUseZb[m_nOldSelRegion-1],pobj->acCode);
				if(!m_bUseFmlPack)
				{
					if(m_nInCom2) strcpy(m_strCom2FavUseZb[m_nOldSelRegion-1],pobj->acCode);
					else strcpy(g_strFavUseZb[m_nOldSelRegion-1],pobj->acCode);
				}
				strcpy(m_strDbSaveZb,pobj->acCode);
			}
			else
			{
				strcpy(m_strFavUseZb[nRegionNo-1],pobj->acCode);
				if(!m_bUseFmlPack)
				{
					if(m_nInCom2) strcpy(m_strCom2FavUseZb[nRegionNo-1],pobj->acCode);
					else strcpy(g_strFavUseZb[nRegionNo-1],pobj->acCode);
				}
			}
		}
		if ( m_aRegion[nRegionNo].nObject < nOutNo+1 ) m_aRegion[nRegionNo].nObject = nOutNo+1;
		if(bReCalc)
		{
			//重新调整空间
			RejustOneObject(m_aRegion[nRegionNo].aIndexInfo[nOutNo],m_nDataNum);
			//调用Calc库，单独计算本公式
			CalcOne(m_aRegion[nRegionNo].aIndexInfo[nOutNo]);
			//计算本部件当前屏幕的最值
			CalcOneGSRatio(m_aRegion[nRegionNo].aIndexInfo[nOutNo],(m_nLastNo+1)-m_nDrawNum);
			//调整本Region的最值，并且重新计算比例
			CalcOneRegionRatio(nRegionNo);
		}
	}
	//要求将来重新计算Y轴等
	m_bChgRatio = TRUE;
	if(nSet==ZB_TYPE || nSet==SEPCLINE_TYPE) 
		SetSwitchSel();
}

int  UFxt::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	//如果Unit为空，则另行处理
	if(m_nCurGP==-1)
	{
		if(message==WM_PAINT)
		{
			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);

			CPen  *oldpen = g_d.SelectPen(&m_MyDC,AXIS_PEN);
			if((m_nInitPad&0xFF0000) == MULFXT_PAD)
				g_d.DrawRect(&m_MyDC,&m_rcIn);
			g_d.RestorePen(&m_MyDC,oldpen);
			
			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
			DESTORY_TMPDC
			return 1;
		}
		int nRet = UBase::ProcessMsg(message,wParam,lParam);
		if(message == UM_SET_STOCK && wParam!=DWORD(-1))
		{
			int i = 0,j = 0;
			for(i=0;i<g_nFml2Stk;i++)
			{
				if(g_Fml2Stk[i].setcode==m_pStkInfo->setcode && strcmp(g_Fml2Stk[i].code,m_pStkInfo->Code)==0)
					break;
			}
			if(i<g_nFml2Stk)
			{
				for(j=0;j<g_nFmlPack;j++)
				{
					if(stricmp(g_Fml2Stk[i].aPackName,g_FmlPack[j].aPackName)==0)
						break;
				}
				if(j<g_nFmlPack) 
					ProcessMsg(UM_USEFORMULAPACK,j);
				else 
					ProcessMsg(UM_UNUSEFMLPACK);
			}
			else 
				ProcessMsg(UM_UNUSEFMLPACK);
		}
		return nRet;
	}
	switch(message)
	{
	case WM_COMMAND:
		ProcessCommand(wParam, lParam);
		break;
	case UM_DYNCHGPARAM:
		if ( m_SelectInfo.bExist )
		{
			TOBJECT * p =m_aRegion[m_SelectInfo.nInRegion].aIndexInfo[m_SelectInfo.nInObject];
			CalcOne(p);
			CalcOneGSRatio(p,(m_nLastNo+1)-m_nDrawNum);
			CalcOneRegionRatio(m_SelectInfo.nInRegion);
			ProcessMsg(WM_PAINT);
		}
		break;
	case UM_AFTERFORMULA:
		{
			TOBJECT	* p = m_pObject;
			while( p )
			{
				if ( p->nSet == ZB_OBJECT || p->nSet == TJ_OBJECT
				  || p->nSet == EXP_OBJECT|| p->nSet == KL_OBJECT
					)
				{
					ModifyProperty(p);
				}
				p = p->next;
			}
			m_bChgRatio = TRUE;
		}
		break;
	case UM_DRAGZBTREE:
		//0:删除指示 1:交易系统指示 2:五彩K线 3:一般指标 4:特特画线指标
		switch ( wParam )
		{
		case 0:			
			DelZs();			
			break;
		case 1:
		case 2:
			ProcessMsg(UM_CHANGEZS,wParam,lParam);
			break;
		case 3:
		case 4:
			{
				for ( int l=0;l<m_nRegion;++l )
				{
					CRect r(m_aRegion[l].Area);
					if ( r.PtInRect(g_ZbDragPoint) )
					{
						ChangGS(l, lParam,wParam == 3 ? ZB_OBJECT : SPECLINE_OBJECT );
						ProcessMsg(WM_PAINT);
						break;
					}
				}
			}
			break;
		}
		break;
	case UM_READDATA:
		if(g_pComm->m_HQBufFlag == 2) 
			return 0;
		if (m_nStyle != FXT_MULTI && IsFxtShowTab())
		{
			if(m_last_rect!=m_rcIn)
			{
				ResetAllSwitch();
				m_last_rect = m_rcIn;
			}
		}
		if(!OnLine || UseLocaldata())
		{
			if(!OnLine || (wParam==0 && UseLocaldata()))
			{
				m_bDataReady = TRUE;
				ReadData(TRUE);
			}
		}
		else
		{	//第一次读入数据 以后，不要发此消息（仅ComView发）
			m_bReqing = TRUE;
			m_bIsHead = FALSE;
			m_bDataReady = FALSE;
			if(g_GH.nInitKNum>=MIN_SHOWNUM && !m_nInCom2) 
				m_nDrawNum  = min(RECENTNUM, g_GH.nInitKNum);
			else if(g_GH.nInitKNum2>=MIN_SHOWNUM && m_nInCom2) 
				m_nDrawNum  = min(RECENTNUM, g_GH.nInitKNum2);

			if(m_nDrawNum>0)
			{
				m_nSHOWNUM=max(MIN_SHOWNUM,m_nDrawNum+1);
				m_nSHOWNUM=min(RECENTNUM,m_nSHOWNUM);
			}
			//请求时,说明是第一次 以便将一些变量复位
			if ( wParam == 0 )
			{
				g_pComm->m_HQBufFlag = 0;
				m_nWillNum = 0;
				ReadData(FALSE);
			}
			else
			{	//只画框架
				m_nDataNum = 0;
				m_nWillNum = m_nSHOWNUM+3;
				m_nDrawNum = 0;
			}
		}
		return 1;
	case UM_GETDATA_ACK://对于远程版,请求和返回分开
		{
			if(wParam<100) return 1;
			wParam-=100;
			if(wParam == 3) //如果是行情
			{
				struct CurrStockData tmpHq;
				g_pGetData->GetCurrStockDataACK(m_pStkInfo,&tmpHq);
				return 1;
			}
			if(wParam==5)
			{
				struct CurrStockData tmpHq;
				g_pGetData->DSGetCurrDataACK(&tmpHq);
				return 1;
			}
			if(wParam == 9) //如果本地日线对齐最新行情
			{
				CurrStockData tmpHq={0};
				g_pGetData->GetCurrStockDataACK(m_pStkInfo,&tmpHq);
				ProcessMsg(UM_UPDATE);
				return 1;
			}
			BOOL  bUpdate = !m_bDataReady;
			if(!m_bReqing&&m_bDataReady)	
			{
				m_bReqing = TRUE;
				m_bIsHead = FALSE;
				m_bDataReady = FALSE;
				g_pComm->m_HQBufFlag = 0;
				m_nWillNum = 0;
				bUpdate = TRUE;
				ReadData();
			}
			else 
				ReadData();
			m_bDataReady = TRUE;
			m_bReqing = FALSE;
			if ( m_nWillNum  > 0 )
				m_nWillNum = m_nDataNum+3;
			else
				m_nWillNum = m_nSHOWNUM+2;
			CalcAllRatio();
			if ( bUpdate )
			{
				g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq,NULL,&m_HqEx);
				UpdateData();
				if ( !m_bBeginRectangle )
				{
					ProcessMsg(WM_PAINT);
				}
			}
			//咬定十指光标 
			if ( m_bKeyCross )
				m_OldKeyPoint = GetPointFromItem(m_nKeyNo-m_nLeftNo);
		}
		ProcessMsg(WM_PAINT);
		return 1;
	case WM_TIMER:
		if ( !m_bDataReady )  
			return 1;
		return 1;
	case ASYN_HQ_MESSAGE:
	case ASYN_STATHQ_MESSAGE:
		{
			if(!m_bDataReady)	
				return 1;
			if(!m_pStkInfo)		
				return 1;
			short nTmpCurGp = g_pStockIO->GetIndex(m_pStkInfo->Code, m_pStkInfo->setcode);
			if(wParam == nTmpCurGp)
			{
				if(message==ASYN_STATHQ_MESSAGE)
					ProcessMsg(UM_FORCEUPDATE);
				else
					ProcessMsg(UM_UPDATE);
			}
		}
		return 1;
	case WM_SIZE:
		m_bChgRegion= TRUE;
		m_bChgRatio = TRUE;
		if ( m_nStyle != FXT_MULTI && IsFxtShowTab() )
		{
			int x = LOWORD(wParam),y = HIWORD(wParam);
			int cx = LOWORD(lParam),cy = HIWORD(lParam);
			SetRect(x,y,x+cx,y+cy-SWITCH_HEIGHT2);
			m_rcIn.CopyRect(this);
			m_rcIn.OffsetRect(-left,-top);
			
			if(m_last_rect!=m_rcIn)
			{
				ResetAllSwitch();
				m_last_rect = m_rcIn;
			}
			return 1;
		}
		break;
	case UM_TQ:
		if(m_bMouseCross || m_bKeyCross)
			ProcessMsg(WM_KEYDOWN,VK_ESCAPE);
		TQdata(wParam);
		ProcessMsg(WM_PAINT);
		break;
	case UM_DELZS:
		DelZs();
		break;
	case UM_SETPERIOD:
		SetPeriod(wParam);
		break;
	case UM_ADJUSTPARAM:
		if(m_SelectInfo.bExist)	AdjustParam();
		else ProcessZBInRegion(UM_ADJUSTPARAM);
		break;
	case UM_SETAXIS:
		ChgAxis(wParam);
		break;
	case UM_CHANGEZS:
		switch ( wParam )
		{
		case 0:
			DelZs();
			break;
		case 1:
			{
				TINDEXINFO * pTmpIndex = g_pCalc->_CallDTopViewCalc_8(EXP_TYPE, lParam);
				if (pTmpIndex != NULL)
				{
					if ( pTmpIndex->nDelStatus & ERR_INDEX )
					{
						TDX_MessageBox(NULL,"公式编译未通过!",MB_OK|MB_ICONEXCLAMATION);
						return 0;
					}
					for (int i=0;i<m_aRegion[0].nObject;i++ )
					{
						if ( m_aRegion[0].aIndexInfo[i]->nSet == EXP_OBJECT )
						{
							RegionDelOne(0,m_aRegion[0].aIndexInfo[i]);
							break;
						}
					}
					AppendRg0(pTmpIndex);
					m_pExpertIndex = pTmpIndex;
				}
			}
			break;
		case 2:
			{
				TINDEXINFO * pTmpIndex = g_pCalc->_CallDTopViewCalc_8(KL_TYPE, lParam);
				if (pTmpIndex != NULL)
				{
					if ( pTmpIndex->nDelStatus & ERR_INDEX )
					{
						TDX_MessageBox(NULL,"公式编译未通过!",MB_OK|MB_ICONEXCLAMATION);
						return 0;
					}
					for (int i=0;i<m_aRegion[0].nObject;i++ )
					{
						if ( m_aRegion[0].aIndexInfo[i]->nSet == KL_OBJECT )
						{
							RegionDelOne(0,m_aRegion[0].aIndexInfo[i]);
							break;
						}
					}
					m_bKColor = TRUE;
					AppendRg0(pTmpIndex);
					m_pColorKIndex = pTmpIndex;
				}
			}
			break;
		}
		break;
	case UM_SET_FXPERIOD:
		if(m_lBandPackNo<0 || m_lBandPackNo>=g_nFmlPack)
			m_nCommonPeriod = wParam;		//未绑定模板时,修改周期时影响全部
		
		m_nPeriod		= wParam;
		m_nCalcPeriod	= TdxPeriodToCalcPeriod(m_nPeriod);
		m_PeriodKeyOrder= TdxPeriodToKeyOrder(m_nPeriod);

		FillCbInfo(m_OldKeyPoint.y,m_nLeftNo);
		m_pView->SendMessage(UM_REFLECT,Reflect_Tv_Period);
		break;
	case UM_SET_OFTENZB:
		ChangGS(m_nSelRegion, wParam,ZB_TYPE,FALSE);
		break;
	case UM_CHANGEZB:
		ChangGS(m_nSelRegion, wParam);
		ProcessMsg(WM_PAINT);
		break;
	case UM_CHANGESPEZB:
		ChangGS(m_nSelRegion,wParam,SEPCLINE_TYPE);
		ProcessMsg(WM_PAINT);
		break;
	case UM_MENUZB:
		ChangeMenuZB(wParam,TRUE);
		break;
	case UM_HXZBDLG:
		ChangeMenuZB(ID_MOREINDEX,FALSE); //借用MOREINDEX菜单项的功能
		break;
	case UM_CHANGECOLOR:
		ChangeSelfColor();
		break;
	case UM_SELECTMANY:
		SelectMany(wParam);
		break;
	case UM_MODIFYFORMULA:
		ProcessCommand(ID_MODIFY_FORMULA, lParam);
		break;
	case UM_INDEXHELP:
		ProcessCommand(ID_INDEXHELP, lParam);
		break;
	case UM_OVERLAP:
		if(g_Overlap[0].nAutoType!=0)
		{
			if(TDX_MessageBox(m_pView->m_hWnd,"是否删除自动叠加的股票?",MB_YESNO ) == IDNO)
				break;
			DelOverStk();
			g_Overlap[0].nAutoType = 0;
			g_Overlap[0].nNum = 0;
		}
		if(m_nOverlapNum>=3)
		{
			TDX_MessageBox(m_pView->m_hWnd,"最多只能叠加三只品种!",MB_OK|MB_ICONEXCLAMATION);
			break;
		}
		OverlapGP();
		break;
	case UM_AUTOVERLAP:
		AutoOverlapGP(TRUE);
		ProcessMsg(WM_PAINT);
		break;
	case UM_UNOVERLAP:
		DelOverStk();
		g_Overlap[0].nAutoType = 0;
		g_Overlap[0].nNum = 0;
		break;
	case UM_CHANGEPARAM:
		ChangGS(m_SelectInfo.nInRegion, m_SelectInfo.nGSIndex);
		break;
	case UM_HISDLG:
		{
		//	break;
			int Item = -1;
			if (m_bMouseCross)  
				Item = GetItemNoFromX(m_OldMousePoint.x-left)+m_nLeftNo;
			else if ( m_bKeyCross )				
				Item = m_nKeyNo;
			else if(wParam==1)
			{
				CDC *pDC = m_pView->GetDC();
				Item = GetItemNoFromX(m_RealMove.x-left)+m_nLeftNo;
				m_bKeyCross = 1;
				m_nKeyNo    = Item;
				m_OldKeyPoint = GetPointFromItem(Item-m_nLeftNo);
				DrawLastState(pDC);
				m_pView->ReleaseDC(pDC);
			}
			if(Item<0) 
			{
				if(wParam==0 && !m_nInCom2) //进入走势图
					g_pMainWin->PostMessage(WM_COMMAND,(g_bTickTStatus?ID_508:ID_501));
				break;
			}
			if(m_nPeriod != PER_DAY) 
			{
				TDX_MessageBox(m_pView->m_hWnd,"只能在日线上使用历史同步回忆功能!",MB_OK|MB_ICONEXCLAMATION);
				break; //只有日线才能使用此功能
			}
			//日期有误，则返回
			if(m_pObject->pHisDay[Item].Time.Date < 19900101 || m_pObject->pHisDay[Item].Time.Date > 20500101)
				break;
			g_pHisDlg = new CHisDlg;
			((CHisDlg *)g_pHisDlg)->m_nCurGP   = m_nCurGP;
			((CHisDlg *)g_pHisDlg)->m_nCurDate = m_pObject->pHisDay[Item].Time.Date;
			((CHisDlg *)g_pHisDlg)->m_pView    = m_pView;
			memcpy(&g_SyncAnalyData,m_pObject->pHisDay+Item,sizeof(AnalyData));
			g_SyncClose = 0.0;
			if(Item > 0)	
				g_SyncClose = m_pObject->pHisDay[Item-1].Close;
			((CHisDlg *)g_pHisDlg)->DoModal();
			TDEL(g_pHisDlg);
		}
		return 1;
	case UM_QJSTAT:
		{
			int   Item1,Item2,tmp;
			if(wParam==0) //wParam为零表示选中区间时的区间统计
			{
				Item1 = GetItemNoFromX(m_OrgPoint.x-left);
				tmp   = GetItemNoFromX(m_ArchorPoint.x-left);
				Item2 = max(tmp,Item1);
				Item1 = min(tmp,Item1);
				Item1 += m_nLeftNo;
				Item2 += m_nLeftNo;
			}
			else		 //本屏所有数据的区间统计
			{
				Item1 = max(0,m_nLeftNo);
				Item2 = min(m_nLastNo,m_nDataNum-1);
			}
			if ( abs(Item2-Item1) < 1 )
				break;
			CQjStatDlg dlg;
			dlg.SetDataAndPeriod(m_pStkInfo,m_pObject->pHisDay,m_nDataNum,Item1,Item2,m_nPeriod);
			dlg.DoModal();
			ProcessMsg(WM_PAINT);
		}
		return 1;
	case UM_FORCEUPDATE:
		if(!m_bDataReady)			
			return 1;
		if( !m_bDSFlag && g_bYesterdayFlag )		
			break;
		g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq,NULL,&m_HqEx);
		if( m_bDSFlag && m_HqEx.HqDate!=g_iToday) 
			break;
		UpdateData(TRUE);
		if ( !m_bBeginRectangle )
			ProcessMsg(WM_PAINT);
		return 1;
	case UM_UPDATE:
		if(!m_bDataReady)			
			return 1;
		if( !m_bDSFlag && g_bYesterdayFlag )		
			break;
		g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq,NULL,&m_HqEx);
		if( m_bDSFlag && m_HqEx.HqDate!=g_iToday) 
			break;
		UpdateData();
		if ( !m_bBeginRectangle )
			ProcessMsg(WM_PAINT);
		return 1;
	case WM_PAINT:	//wParam=1要读数据
		{
			if(m_bNotRedraw) break;
			if(wParam == 1)
				ProcessMsg(UM_READDATA,lParam);
			else
			{
				if(m_bDrawLine || (m_SelectInfo.nSet == SELFLINE_OBJECT && m_SelectInfo.IsMovable))
					break;
			}

			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);
			m_MyDC.SetBkMode(TRANSPARENT);

			CFont *oldfont = g_d.SelectFont(&m_MyDC,GRAPH_FONT);
			CPen  *oldpen = g_d.SelectPen(&m_MyDC,AXIS_PEN);
			if((m_nInitPad&0xFF0000) == MULFXT_PAD)
				g_d.DrawRect(&m_MyDC,&m_rcIn);
			DrawIt(&m_MyDC);
			//如果只处理三方行情，则对AB股行情进行提示
			if(g_GC.bOnlyHK && !m_nInCom2 && !m_bDSFlag)
				DrawNoABHintInfo(&m_MyDC,m_rcIn);
			//画那个什么区间
			DrawQJLine(&m_MyDC);
			///////////////////////////////
			g_d.RestorePen(&m_MyDC,oldpen);
			g_d.RestoreFont(&m_MyDC,oldfont);
			//放弃右边坐标
			if(m_nStyle==FXT_MULTI)
				pDC->BitBlt (left,top,Width()-FXT_RWIDTH,Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
			else
			{
				pDC->BitBlt (left,top,Width()-FXT_RWIDTH,Height()-FXT_BHEIGHT,&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
				pDC->BitBlt (right-FXT_RWIDTH,bottom-FXT_BHEIGHT,FXT_RWIDTH,FXT_BHEIGHT,&m_MyDC,
							m_rcIn.right-FXT_RWIDTH,m_rcIn.bottom-FXT_BHEIGHT,SRCCOPY); //从内存DC拷贝到屏幕DC
			}
			DrawLastState(pDC);
			if(m_bDataReady)
				ShowSelfLine(pDC);
			if ( m_bMouseCross && m_OldMousePoint.x>0 )
			{	
				Disp(pDC,m_OldMousePoint.x-left,m_OldMousePoint.y-top);
				DrawXRule(pDC,m_OldMousePoint.x-left,m_OldMousePoint.y-top);
				DrawYRule(pDC,m_OldMousePoint.x-left,m_OldMousePoint.y-top);
			}
			else if ( m_bKeyCross && ( m_OldKeyPoint.x>0 || m_OldKeyPoint.y>0 ))
			{
				Disp(pDC,m_OldKeyPoint.x-left,m_OldKeyPoint.y-top);
				DrawXRule(pDC,m_OldKeyPoint.x,m_OldKeyPoint.y);
				DrawYRule(pDC,m_OldKeyPoint.x,m_OldKeyPoint.y);
			}
			else if(m_nStyle != FXT_MULTI)
			{
				DrawXRule(pDC,m_RealMove.x-left,m_RealMove.y-top);
				DrawYRule(pDC,m_RealMove.x-left,m_RealMove.y-top);
			}
			else
			{
				DrawXRule(pDC,-1,-1);
				DrawYRule(pDC,-1,-1);
			}
			DESTORY_TMPDC
		}
		return 1;
	case UM_MOUSELEAVE:
		{
			CREATE_TMPDC
			DrawXRule(pDC,m_rcIn.right,m_rcIn.bottom);
			DrawYRule(pDC,m_rcIn.right,m_rcIn.bottom);
			ProcessMsg(WM_KEYDOWN,VK_ESCAPE);
			DESTORY_TMPDC
		}
		return 1;
	case UM_DELETE_INFODLG: 
		//wParam为1表示只删除信息框
		if(wParam==1)
			DeleteInfoDlg();
		else 
			ProcessMsg(WM_KEYDOWN,VK_ESCAPE);
		return 1;
	case WM_MOUSEMOVE:
		if(!m_bDataReady)		
			return 1;
		if(m_nStyle == FXT_MULTI) break;
		if(m_rcIn.Width()<5 || m_rcIn.Height()<5)
			break;
		ProcessMouse(MouseMove);
		break;
	case WM_LBUTTONDOWN:
		{
			if(!m_bDataReady)		
				return 1;
			if(m_nStyle == FXT_MULTI) 
				break;
			
			CPoint XDPt(LOWORD(lParam),HIWORD(lParam));
			XDPt.x -= left;
			XDPt.y -= top;
			if(m_PeriodRc.PtInRect(XDPt))
			{
				((CMainFrame*)AfxGetMainWnd())->OnCommCommand(CMD_MAIN,5);
				break;
			}
			
			ProcessMouse(LButtonDown);
		}
		break;
	case WM_LBUTTONUP:
		if(!m_bDataReady)		
			return 1;
		if(m_nStyle == FXT_MULTI) 
			break;
		ProcessMouse(LButtonUp);
		break;
	case WM_LBUTTONDBLCLK:
		if(m_nStyle == FXT_MULTI) 
			break;
		ProcessMouse(LButtonDblClk);
		break;
	case WM_RBUTTONDOWN:
		if (m_nStyle != FXT_MULTI)
			ProcessMouse(RButtonDown);
		break;
	case WM_RBUTTONUP:
		if (m_nStyle != FXT_MULTI)
			ProcessMouse(RButtonUp);
		break;
	case WM_CHAR:
		{
			UINT nChar = (UINT)wParam;
			if(!m_bDataReady) 
				break;
			//清空ToolTip
			m_TipInfo.Empty();
			m_ToolTip.UpdateToolTip(m_pView,"");

			switch(nChar)
			{
			case ']':
				++m_PeriodKeyOrder;
				m_PeriodKeyOrder = m_PeriodKeyOrder%11;
				m_bKeepOver = TRUE;
				SetPeriod(KeyOrderToTdxPeriod(m_PeriodKeyOrder));
				FillDlgInfo(m_nLastNo);
				PopupInfoDlg();
				break;
			case '[':
				--m_PeriodKeyOrder;
				if ( m_PeriodKeyOrder < 0 )
					m_PeriodKeyOrder = 10;
				m_PeriodKeyOrder = m_PeriodKeyOrder%11;
				m_bKeepOver = TRUE;
				SetPeriod(KeyOrderToTdxPeriod(m_PeriodKeyOrder));
				FillDlgInfo(m_nLastNo);
				PopupInfoDlg();
				break;
			}
		}
		break;
	case WM_KEYDOWN:
		{
			UINT nChar = (UINT)wParam;
			if(m_nStyle==FXT_MULTI && nChar!=VK_UP && nChar!=VK_DOWN && nChar!=VK_F8) break; //对于多股,仅处理上下箭头两个键
			if(!m_bDataReady && (nChar!=VK_ESCAPE && nChar!=VK_PRIOR && nChar!=VK_NEXT)) break;
			//清空ToolTip
			m_TipInfo.Empty();
			m_ToolTip.UpdateToolTip(m_pView,"");

			switch(nChar)
			{
			case VK_SPACE:
				{
					int nCurPos = m_nKeyNo;
					if ( m_bMouseCross  )
						nCurPos = GetItemNoFromX(m_OldMousePoint.x-left)+m_nLeftNo;

					if(m_nQJPos1<0)
					{
						m_nQJPos2 = -1;
						m_nQJPos1 = nCurPos;
					}
					else if(m_nQJPos1>=0&&m_nQJPos2<0)
						m_nQJPos2 = nCurPos;
					else 
					{
						m_nQJPos2 = -1;
						m_nQJPos1 = nCurPos;
					}
					CDC *pDC = m_pView->GetDC();
					DrawQJLine(pDC);
					if(m_nQJPos1>=0&&m_nQJPos2>=0)
					{
						CQjStatDlg dlg;
						int Item1 = min(m_nQJPos1, m_nQJPos2);
						int Item2 = max(m_nQJPos1, m_nQJPos2);
						dlg.SetDataAndPeriod(m_pStkInfo,m_pObject->pHisDay,m_nDataNum,Item1,Item2,m_nPeriod);
						dlg.DoModal();
						m_nQJPos1 = m_nQJPos2 = -1;
						ProcessMsg(WM_PAINT);
					}
				}
				break;
			case VK_TAB:
				{
					if(FiltTabKeyCom()) return TRUE;
					long iNo = -1;
					if(m_nInCom2) iNo = g_pCalc->_CallDTopViewCalc_9(ZB_TYPE,m_strCom2ZbMain);
					else iNo = g_pCalc->_CallDTopViewCalc_9(ZB_TYPE,g_strZbMain);
					if(iNo < 0)	break;
					ProcessMsg(UM_CHANGEZB,iNo);
				}
				break;
			case VK_F8:
				if ( (::GetKeyState(VK_SHIFT) & 0xFF00 ) )
					ProcessMsg(WM_CHAR,'[');
				else
					ProcessMsg(WM_CHAR,']');
				break;
			case VK_RETURN:
				if ( GetKeyState(VK_SHIFT) & 0xFF00)
				{
					if ( m_bKeyCross )
					{
					}
					else if ( m_bMouseCross  ) 
						QuickSelect(m_OldMousePoint);
					if ( m_SelectInfo.nSet == THUN_OBJECT )
					{
						ProcessMsg(UM_POPINFODLG,m_SelectInfo.nGSIndex);
						break;
					}
					ProcessMsg(UM_POPINFODLG,0);
				}
				else
					ProcessMsg(UM_HISDLG,0);//非强制,如果没有选中K线,再进入分时
				break;
			case VK_ESCAPE:
				DeleteInfoDlg();
				m_nQJPos1 = m_nQJPos2 = -1;
				if ( m_bKeyCross || m_bMouseCross )
				{
					CDC * pDC = m_pView->GetDC();
					DrawLastState(pDC);
					m_bMouseCross = FALSE ;
					m_bKeyCross = FALSE ;
					m_OldKeyPoint = GetPointFromItem(m_nDrawNum-1);
					m_KLock = -1;
					FillDlgInfo(m_nDataNum-1);
					Disp(pDC,m_OldKeyPoint.x-left,m_OldKeyPoint.y-top);
					m_pView->ReleaseDC(pDC);
				}
				else
					return 0;
				m_nToLeft = 0 , m_nToRight = 0;	//取消锁定
				break;
			case VK_HOME:
			case VK_END:
				{
					if ( !m_bKeyCross && !m_bMouseCross )
					{						
						ProcessMsg(WM_KEYDOWN,(nChar==VK_HOME)?VK_DIVIDE:VK_MULTIPLY,0);
						break;
					}
					///////////////弹出对话框
					if( !m_pInfoDlg )
					{
						CreateInfoDlg();
						m_nToLeft = 0 , m_nToRight = 0;
					}
					//////////////////
					if ( nChar == VK_HOME && (m_nToLeft & 0xFFFF) < 10 )
						m_nToLeft++ , m_nToRight = 0;
					else if( nChar == VK_END && (m_nToRight & 0xFFFF) < 10 )
						m_nToLeft = 0 , m_nToRight ++;
					///////如果在VK_CONTROL下快进
					if ( ::GetKeyState(VK_CONTROL) & 0xFF00 )
					{	int  Off = 0;
						if ( nChar == VK_HOME && m_nToLeft<= 0x10001 )		//没移动有到头
						{
							Off = -m_nLeftNo;
							m_nKeyNo = 0;
							m_nToLeft |= 0x10000;
						}
						else if ( nChar == VK_END && m_nToRight<= 0x10001 ) 					
						{
							Off = m_nDataNum-m_nLeftNo-m_nDrawNum; 
							m_nKeyNo = m_nDataNum-1;
							m_nToRight |= 0x10000;
						}
						/////////////////是否是有效移动
						if ( Off )		
						{
							XPan(Off,TRUE);
							CalcRuleDrawAxis();		//重新计算X、Y轴的坐标
							ProcessMsg(WM_PAINT);
						}
					}
					//////////////////////消除历史的十字光标
					BOOL   bDoIt = 0;		//与Ctrl+X的情况独立，不能与上述代码合并
					if ( (nChar == VK_HOME && (m_nToLeft &0xFFFF)<=4)
						|| (nChar == VK_END && (m_nToRight &0xFFFF)<=4) )		//没移动有到头
						bDoIt = TRUE;
					CDC *pDC = m_pView->GetDC();
					if ( bDoIt )
					{
						if ( m_bMouseCross  )
						//在鼠标状态下
							ResetCross(pDC) , m_bMouseCross = FALSE;
						else if ( m_bKeyCross )
							DrawLastState(pDC);
						///////////////////////重新定位，以键盘为主
						m_bKeyCross   = TRUE;
						m_OldKeyPoint = nChar == VK_HOME ? GetPointFromItem(0) : GetPointFromItem(m_nDrawNum-1);
						m_nKeyNo      = nChar == VK_HOME ? m_nLeftNo : m_nLastNo;

						Lock_K();		//锁定K线
						Lock_KData();

						///重新显示相应的数据信息,锁定K线等
						DrawXRule(pDC,m_OldKeyPoint.x,m_OldKeyPoint.y);
						DrawYRule(pDC,m_OldKeyPoint.x,m_OldKeyPoint.y);
						Disp(pDC,m_OldKeyPoint.x-left,m_OldKeyPoint.y-top);
						DrawLastState(pDC);						
					}
					if (g_CbInfo.m_pData )
					{
						FillCbInfo(m_OldKeyPoint.y,m_nLeftNo + max(0,GetItemNoFromX(m_OldKeyPoint.x-left)));
						m_pView->SendMessage(UM_REFLECT,Reflect_Cb_Date);
						m_pView->SendMessage(UM_REFLECT,Reflect_Tv_Date);
					}
					FillDlgInfo(m_nKeyNo);
					PopupInfoDlg();
					if(!g_bFxtShowInfoDlg)
						Disp(pDC,m_OldKeyPoint.x-left,m_OldKeyPoint.y-top);
					m_pView->ReleaseDC(pDC);
				}
				break;
			case VK_UP:
				if ( (::GetKeyState(VK_SHIFT) & 0xFF00 ) )
				{
					POINT * pt = &m_OldKeyPoint;
					if ( !m_bKeyCross )
						pt = &m_OldMousePoint;
					if ( pt->y < top + 20 )
						break;
					CDC * pDC = m_pView->GetDC();
					DrawLastState(pDC);
					pt->y -= 1;
					DrawXRule(pDC,pt->x,pt->y);
					DrawYRule(pDC,pt->x,pt->y);
					DrawLastState(pDC);
					m_pView->ReleaseDC(pDC);
					FillCbInfo(pt->y,m_nLeftNo + max(0,GetItemNoFromX(pt->x-left)));
					m_pView->SendMessage(UM_REFLECT,Reflect_Cb_Date);
					m_pView->SendMessage(UM_REFLECT,Reflect_Tv_Date);
					break;
				}
				if ( m_nWillNum <= 8 )		//至少10个
					return 1;
				m_nToLeft = 0 , m_nToRight = 0;	//取消锁定
				m_nWillNum = m_nWillNum - (m_nWillNum/1700 ? 500 : m_nWillNum/1200 ? 400:m_nWillNum/800 ? 300:m_nWillNum/500 ? 200:m_nWillNum/300 ? 100:m_nWillNum/200 ? 50:m_nWillNum/100 ? 40:m_nWillNum/60 ? 20:m_nWillNum/40 ? 20:m_nWillNum/20 ? 10:m_nWillNum/10 ? 5:5 );
				if ( m_nWillNum < 8 )	 m_nWillNum = 8;
				m_bChgRatio = TRUE;			//改变比例
				ProcessMsg(WM_PAINT);
				RejustCross();				//将鼠标在锁定的位置重画
				break;
			case VK_DOWN:
				if ( (::GetKeyState(VK_SHIFT) & 0xFF00 )  )
				{
					POINT * pt = &m_OldKeyPoint;
					if ( !m_bKeyCross )
						pt = &m_OldMousePoint;
					if ( pt->y > bottom - FXT_BHEIGHT )
						break;
					CDC * pDC = m_pView->GetDC();
					DrawLastState(pDC);
					pt->y += 1;
					DrawLastState(pDC);
					DrawXRule(pDC,pt->x,pt->y);
					DrawYRule(pDC,pt->x,pt->y);
					m_pView->ReleaseDC(pDC);
					FillCbInfo(pt->y,m_nLeftNo + max(0,GetItemNoFromX(pt->x-left)));
					m_pView->SendMessage(UM_REFLECT,Reflect_Cb_Date);
					m_pView->SendMessage(UM_REFLECT,Reflect_Tv_Date);
					break;
				}
				if(OnLine && m_nWillNum >= m_nDataNum+3 && !m_bIsHead && !UseLocaldata())	//最多
				{
					if ( m_bReqing )
						return 1;
					if ( ReadData(FALSE) )
					{
						CalcAllRatio();
						m_bChgRatio = TRUE;			//改变比例
						ProcessMsg(WM_PAINT);
						RejustCross();
					}
					return 1;
				}
				if((!OnLine || UseLocaldata()) && m_nWillNum == m_nDataNum+3)	//最多
					return 1;
				m_nToLeft = 0 , m_nToRight = 0;
				//对个数和K线的大小有一个最小的限制
				if ( m_nWillNum >= m_nDataNum+3 && m_dWidth < 4  )
					return 1;
				m_nWillNum = m_nWillNum + (m_nWillNum/1700 ? 1000 : m_nWillNum/1200 ? 500:
					m_nWillNum/800 ? 400:m_nWillNum/500 ? 300:m_nWillNum/300 ? 200:m_nWillNum/200 ? 100:
					m_nWillNum/100 ? 50:m_nWillNum/60 ? 40:m_nWillNum/40 ? 20:m_nWillNum/20 ? 20:m_nWillNum/10 ? 10:5 );
				m_bChgRatio = TRUE;			//改变比例
				ProcessMsg(WM_PAINT);
				RejustCross();
				break;
			case VK_LEFT:
			case VK_RIGHT:
				{	//确定方向
					int	 x;
					CRect	tmprc;
					m_pView->GetWindowRect(&tmprc);
					BOOL bMSkip = (::GetKeyState(VK_SHIFT) & 0xFF00);
					int OffVar = (nChar == VK_LEFT ? -1 : +1 );
					CDC *pDC = m_pView->GetDC();
					if ( ::GetKeyState(VK_CONTROL) & 0xFF00 )
					{
						int nRept = lParam+(lParam-1)*8;
						OffVar = (nChar == VK_LEFT ? -4 : +4 )*nRept;
					}

					if( !m_pInfoDlg  && lParam != 8 ) CreateInfoDlg();
					if ( !bMSkip && m_bMouseCross  )
					{
						SetCursorPos(tmprc.left+1,tmprc.top+20);
						x = m_OldMousePoint.x;
						ResetCross(pDC);
						m_bKeyCross = 1;
						m_nKeyNo  = GetItemNoFromX(x-left)+m_nLeftNo;
						m_OldKeyPoint = GetPointFromItem(m_nKeyNo-m_nLeftNo);
						Lock_K();
						Lock_KData();
						DrawLastState(pDC);
					}
					else if ( !bMSkip && !m_bKeyCross )
					{
						SetCursorPos(tmprc.left+1,tmprc.top+20);
						m_bKeyCross = 1;
						m_nKeyNo    = m_nLastNo;
						m_OldKeyPoint = GetPointFromItem(m_nKeyNo);
						Lock_K();
						Lock_KData();
						DrawLastState(pDC);
					}
					else 
					{
						if ( bMSkip )
						{
							if(nChar==VK_LEFT)
							{
								if(m_nLeftNo>0)
								{
									XPan(-1,TRUE);
									if(m_bKeyCross) m_nKeyNo--;
								}
							}
							else
							{
								if(m_nLastNo<m_nDataNum-1)
								{
									XPan(1,TRUE);
									if(m_bKeyCross) m_nKeyNo++;
								}
							}
							CalcRuleDrawAxis();
							ProcessMsg(WM_PAINT);
							Lock_K();
							Lock_KData();
							if(m_bMouseCross)
							{
								DrawXRule(pDC,m_OldMousePoint.x,m_OldMousePoint.y);
								DrawYRule(pDC,m_OldMousePoint.x,m_OldMousePoint.y);
								if (g_CbInfo.m_pData )
								{
									FillCbInfo(m_OldMousePoint.y,GetItemNoFromX(m_OldMousePoint.x-left)+m_nLeftNo);
									m_pView->SendMessage(UM_REFLECT,Reflect_Cb_Date);
									m_pView->SendMessage(UM_REFLECT,Reflect_Tv_Date);
								}
								FillDlgInfo(GetItemNoFromX(m_OldMousePoint.x-left)+m_nLeftNo);
								PopupInfoDlg();
								Disp(pDC,m_OldMousePoint.x-left,m_OldMousePoint.y-top);
							}
							else if(m_bKeyCross)
							{
								DrawXRule(pDC,m_OldKeyPoint.x,m_OldKeyPoint.y);
								DrawYRule(pDC,m_OldKeyPoint.x,m_OldKeyPoint.y);
								if (g_CbInfo.m_pData )
								{
									FillCbInfo(m_OldKeyPoint.y,m_nKeyNo);
									m_pView->SendMessage(UM_REFLECT,Reflect_Cb_Date);
									m_pView->SendMessage(UM_REFLECT,Reflect_Tv_Date);
								}
								FillDlgInfo(m_nKeyNo);
								PopupInfoDlg();
								Disp(pDC,m_OldKeyPoint.x-left,m_OldKeyPoint.y-top);
							}
							else
							{
								DrawXRule(pDC,m_RealMove.x,m_RealMove.y);
								DrawYRule(pDC,m_RealMove.x,m_RealMove.y);
								if (g_CbInfo.m_pData )
								{
									FillCbInfo(m_RealMove.y,GetItemNoFromX(m_RealMove.x-left)+m_nLeftNo);
									m_pView->SendMessage(UM_REFLECT,Reflect_Cb_Date);
									m_pView->SendMessage(UM_REFLECT,Reflect_Tv_Date);
								}
								FillDlgInfo(m_nLeftNo+m_nDrawNum-1);
								PopupInfoDlg();
								Disp(pDC);
							}
							m_pView->ReleaseDC(pDC);
							break;
						}
						else
						{
							SetCursorPos(tmprc.left+1,tmprc.top+20);
							int ntheNo = m_nKeyNo-m_nLeftNo; //从0开始
							///////////////////////////////////注意维护一头、一尾停止点 ! //向左
							if ( ntheNo+OffVar <= 0 && m_nLeftNo == 0 && (m_nToLeft & 0xFFFF) < 10 )
								m_nToLeft++ , m_nToRight = 0;		//到最左边
							else if ( OffVar < 0 && ( (m_nLeftNo >= 0 && OnLine)||(m_nLeftNo > 0 && !OnLine) ) )//没有到左边
								m_nToLeft = 0 , m_nToRight = 0;		
							////////////////////////////////////////////向右
							else if ( ntheNo+OffVar >= m_nDrawNum-1 && m_nLastNo == m_nDataNum-1 && (m_nToRight & 0xFFFF)<10)
								m_nToLeft = 0 , m_nToRight++;		//到最右边
							else if ( OffVar > 0 && m_nLastNo <= m_nDataNum-1 )	//没有到右边头
								m_nToLeft = 0 , m_nToRight = 0;
							///////////////////////////////////此时不考虑Ctrl键
							if (   ((m_nToLeft & 0xFFFF)<=1  && OffVar<0 ) 
								|| ((m_nToRight & 0xFFFF)<=1 && OffVar>0 && ntheNo< (m_nDataNum - m_nLeftNo)) )
							{
								if ( ntheNo+OffVar > m_nDrawNum-1 )
								{	//要额外增加到虚拟屏幕的个数
									XPan((ntheNo+OffVar+1) - m_nDrawNum,TRUE);
									CalcRuleDrawAxis();
									m_nKeyNo = m_nLastNo;
									ProcessMsg(WM_PAINT);
								}
								else if ( ntheNo+OffVar < 0 )
								{	//要额外增加到虚拟屏幕的个数
									XPan((ntheNo+OffVar) ,TRUE);
									CalcRuleDrawAxis();
									m_nKeyNo = m_nLeftNo;
									ProcessMsg(WM_PAINT);
								}
								else m_nKeyNo = min(m_nLastNo,ntheNo+OffVar+m_nLeftNo);
								DrawLastState(pDC);
								m_OldKeyPoint = GetPointFromItem(m_nKeyNo-m_nLeftNo);
								Lock_K();
								Lock_KData();
								DrawXRule(pDC,m_OldKeyPoint.x,m_OldKeyPoint.y);
								DrawYRule(pDC,m_OldKeyPoint.x,m_OldKeyPoint.y);
								Disp(pDC,m_OldKeyPoint.x-left,m_OldKeyPoint.y-top);
								DrawLastState(pDC);
							}
						}
					}
					if (g_CbInfo.m_pData )
					{
						FillCbInfo(m_OldKeyPoint.y,m_nKeyNo);
						m_pView->SendMessage(UM_REFLECT,Reflect_Cb_Date);
						m_pView->SendMessage(UM_REFLECT,Reflect_Tv_Date);
					}
					FillDlgInfo(m_nKeyNo);
					PopupInfoDlg();
					if(!g_bFxtShowInfoDlg)
						Disp(pDC,m_OldKeyPoint.x-left,m_OldKeyPoint.y-top);
					m_pView->ReleaseDC(pDC);
				}
				break;
			case VK_DIVIDE:
			case VK_MULTIPLY:
				{
					int OffVar = (nChar == VK_DIVIDE ? -1 : +1 );
					static  int nVarZB = 0;
					if ( m_nSelRegion<=0 || m_nSelRegion>=m_nRegion )
						m_nSelRegion = m_nRegion-1;
					if(OffVar==1)
						nVarZB = (nVarZB+OffVar)%g_nOftenUseZb;
					else
					{
						if(nVarZB==0) nVarZB=g_nOftenUseZb-1;
						else 		  nVarZB--;
					}
					TINDEXINFO * TIndex = g_pCalc->_CallDTopViewCalc_8(0,g_strOftenUseZb[abs(nVarZB)]);
					if ( TIndex )
						::PostMessage(m_pView->m_hWnd, UM_CHANGEZB, TIndex->nIndexNo, 0);
				}
				break;
			case VK_DELETE:
				if(m_SelectInfo.bExist && m_SelectInfo.nInRegion>=0 && m_SelectInfo.nInRegion<m_nRegion)
				{
					m_SelectInfo.bExist = 0;
					if(m_SelectInfo.nSet == KL_OBJECT)
					{
						m_bKColor = FALSE;
						memset(&m_KLZS,0,sizeof(FXZSINFO));
						m_pColorKIndex = NULL;
					}
					else if(m_SelectInfo.nSet == EXP_OBJECT)
					{
						memset(&m_ExpZS,0,sizeof(FXZSINFO));
						m_pExpertIndex = NULL;
					}
					else if(m_SelectInfo.nSet == SELFLINE_OBJECT)
					{
						SetSelfLine(ID_DEL);
						break;
					}
					TOBJECT * pOb = m_aRegion[m_SelectInfo.nInRegion].aIndexInfo[m_SelectInfo.nInObject];
					PushLastInfo(pOb);
					RegionDelOne(m_SelectInfo.nInRegion,pOb);
					m_bChgRatio = TRUE;
					ProcessMsg(WM_PAINT);
				}
				break;
			case VK_PRIOR:
				FillDlgInfo(m_nKeyNo);
				PopupInfoDlg();
				break;
			case VK_NEXT:
				FillDlgInfo(m_nKeyNo);
				PopupInfoDlg();
				break;
			default:				
				break;
			}
		}
		return 1;
	case UM_SELNUM:
		{
			if(m_nStyle == FXT_MULTI) break;
			m_lCurPackNo = -1;
			m_lBandPackNo= -1;
			if(lParam==0) //表示是用户来选择组合图数,1表示由本模块自己调用
				m_bDbClickFT=FALSE;
			m_nOldRegion = m_nRegion;
			m_bChgRegion = TRUE;	//改变区域大小
			for(unsigned int i = m_nRegion;i<wParam; i++ )
			{	
				if ( m_nRegion >0 )
					m_aRegion[i].aIndexInfo[0] = AddObjectByName(ZB_OBJECT,m_strFavUseZb[i-1]);
				if ( m_aRegion[i].aIndexInfo[0] == NULL || m_nRegion == 0 )
				{
					//弹出增经被删除的部件
					PopLastInfo(m_aRegion[i].aIndexInfo[0]);
				}
				m_aRegion[i].nObject++;
				//分配空间
				RejustOneObject(m_aRegion[i].aIndexInfo[0],m_nDataNum);
				if ( m_aRegion[i].aIndexInfo[0] && m_aRegion[i].aIndexInfo[0]->nSet == HIS_DAT ) 
					ReadSomeStk(m_aRegion[i].aIndexInfo[0]);
				//计算三部曲
				CalcOne(m_aRegion[i].aIndexInfo[0]);
				CalcOneGSRatio(m_aRegion[i].aIndexInfo[0],(m_nLastNo+1)-m_nDrawNum);
				CalcOneRegionRatio(i);
			}
			if(m_strDbSaveZb[0]) //要重新计算副图的第一个指标
			{
				m_aRegion[1].aIndexInfo[0] = AddObjectByName(ZB_OBJECT,m_bDbClickFT?m_strDbSaveZb:m_strFavUseZb[0]);
				//分配空间
				RejustOneObject(m_aRegion[1].aIndexInfo[0],m_nDataNum);
				if ( m_aRegion[1].aIndexInfo[0] && m_aRegion[1].aIndexInfo[0]->nSet == HIS_DAT ) 
					ReadSomeStk(m_aRegion[1].aIndexInfo[0]);
				//计算三部曲
				CalcOne(m_aRegion[1].aIndexInfo[0]);
				CalcOneGSRatio(m_aRegion[1].aIndexInfo[0],(m_nLastNo+1)-m_nDrawNum);
				CalcOneRegionRatio(1);
			}
			if ( m_nRegion>(signed)wParam )
			{	for(i = m_nRegion-1;i>=wParam; --i )
					while( m_aRegion[i].aIndexInfo[0] )
					{	//压入即将删除的部件
						if ( i > 0 && !(m_bDbClickFT&&i==1))
							strcpy(m_strFavUseZb[i-1],m_aRegion[i].aIndexInfo[0]->acCode);
						PushLastInfo(m_aRegion[i].aIndexInfo[0]);
						RegionDelOne(i,m_aRegion[i].aIndexInfo[0]);
					}
			}
			if ( m_nSelRegion <0 || m_nSelRegion >= m_nRegion )
				m_nSelRegion = m_nRegion-1;
			if ( m_nSelRegionBak <0 || m_nSelRegionBak >= m_nRegion )
				m_nSelRegionBak = m_nRegion-1;
			m_nRegion = wParam;
			if(!m_bUseFmlPack) m_NormalRegion = m_nRegion;
			if(!m_bDbClickFT)
			{
				if(m_nInCom2)	m_Com2WinNum = m_nRegion;
				else g_nZBWinNum = m_nRegion;
			}
			SetSwitchSel();
			ProcessMsg(WM_PAINT);
		}
		break;
	case UM_SETSEL:
		m_bSelected = wParam;
		return 1;
	case UM_GETSEL:
		return m_bSelected?1:0;
	case UM_GET_FXTDATE:
		{
			int turnflag = (int)wParam;
			if ( m_nPeriod == PER_DAY )
			{
				if(turnflag == 1) //下一个日期
					ProcessMsg(WM_KEYDOWN,VK_RIGHT,8);
				else
					ProcessMsg(WM_KEYDOWN,VK_LEFT,8);
				memcpy(&g_SyncAnalyData,m_pObject->pHisDay+m_nKeyNo,sizeof(AnalyData));
				g_SyncClose = 0.0; //重新计算g_SyncClose
				if(m_nKeyNo > 0)	
					g_SyncClose = m_pObject->pHisDay[m_nKeyNo-1].Close;
				return m_pObject->pHisDay[m_nKeyNo].Time.Date;
			}
		}
		break;
	case UM_GET_FXTDATA:
		{
			TOBJECT *  p = m_aRegion[0].aIndexInfo[0];
			if(m_pStkInfo)
			{
				g_CurFxtZb.setcode = m_pStkInfo->setcode;
				strncpy(g_CurFxtZb.strCode,m_pStkInfo->Code,19);
				g_CurFxtZb.strCode[19]=0;
			}
			g_CurFxtZb.fNow = m_Hq.Now;
			g_CurFxtZb.fBuyp = m_Hq.Buyp[0];
			g_CurFxtZb.fSellp = m_Hq.Sellp[0];
			int Begin = 0;
			int nRegion = 0;
			while( Begin<m_aRegion[nRegion].nObject )
			{
				if ( p->nSet != HIS_DAT )
				{
					for(int i=0;m_nDataNum>0 && i<min(6,p->nOutNum);i++)
						g_CurFxtZb.fOutLine[i]=p->pCalcData[i][m_nDataNum-1];
					break;
				}	
				Begin++;
				p = m_aRegion[nRegion].aIndexInfo[Begin];
			}
		}
		break;
	case UM_ADDFORMULAPACK:
		{
			FMLPACK tmpFmlPack;
			int i=0,j=0;
			int nRegion = 0;
			memset(&tmpFmlPack,0,sizeof(FMLPACK));

			if(m_bDbClickFT) nRegion = m_nOldSelRegion;
			else nRegion = m_nRegion;

			for(i=0;i<nRegion;i++)
			{
				for(j=0;j<m_aRegion[i].nObject;j++)
				{
					if(m_aRegion[i].aIndexInfo[j]->nSet==ZB_TYPE) break;
				}
				if(j<m_aRegion[i].nObject)
					strncpy(tmpFmlPack.aFormular[i],m_aRegion[i].aIndexInfo[j]->acCode,31);
			}
			tmpFmlPack.lRectNum = nRegion;
			tmpFmlPack.nPeriod	= m_nPeriod;

			CFormulaPackDlg dlg;
			dlg.SetMode(TRUE,FALSE);
			dlg.SetSvFmlPack(tmpFmlPack);
			dlg.DoModal();

			if(m_nStyle != FXT_MULTI && IsFxtShowTab())
				ResetAllSwitch();
			ProcessMsg(WM_PAINT);
		}
		break;
	case UM_USEFORMULAPACK:
		{
			if(m_nStyle == FXT_MULTI) break;
			int i=0,j=0;
			//先将全局指标载入
			if(m_nInCom2)
			{
				strncpy(m_strZbMain,m_strCom2ZbMain,ZBNAME_LEN-1);
				m_strZbMain[ZBNAME_LEN-1] = 0;
			}
			else
			{
				strncpy(m_strZbMain,g_strZbMain,ZBNAME_LEN-1);
				m_strZbMain[ZBNAME_LEN-1] = 0;
			}
			for(i=0;i<11;i++)
			{
				if(m_nInCom2) strncpy(m_strFavUseZb[i],m_strCom2FavUseZb[i],ZBNAME_LEN-1);
				else strncpy(m_strFavUseZb[i],g_strFavUseZb[i],ZBNAME_LEN-1);
				m_strFavUseZb[i][ZBNAME_LEN-1] = 0;
			}
			//替换
			if(g_FmlPack[wParam].aFormular[0][0]!=0) 
			{
				strncpy(m_strZbMain,g_FmlPack[wParam].aFormular[0],ZBNAME_LEN-1);
				m_strZbMain[ZBNAME_LEN-1] = 0;
			}
			for(i=1;i<g_FmlPack[wParam].lRectNum;i++)
			{
				if(g_FmlPack[wParam].aFormular[i][0]!=0) strncpy(m_strFavUseZb[i-1],g_FmlPack[wParam].aFormular[i],ZBNAME_LEN-1);
				m_strFavUseZb[i][ZBNAME_LEN-1] = 0;
			}
			if(!m_bUseFmlPack) 
				m_NormalRegion = m_nRegion;
			//删除旧对象
			m_bUseFmlPack	= TRUE;
			for(i=0;i<m_nRegion;i++)
			{
				if(i>=g_nFmlPack)
				{
					while(m_aRegion[i].aIndexInfo[0])
					{
						RegionDelOne(i,m_aRegion[i].aIndexInfo[0]);
					}
					continue;
				}
				for(j=0;j<m_aRegion[i].nObject;j++)
				{
					if(m_aRegion[i].aIndexInfo[j]->nSet==ZB_OBJECT)
					{
						RegionDelOne(i,m_aRegion[i].aIndexInfo[j]);
						j--;
					}
				}
			}
			m_lBandPackNo	= long(wParam);
			//需要直接替换全局指标
			if(BOOL(lParam))
			{
				m_lCurPackNo	= long(wParam);
				if(g_FmlPack[wParam].aFormular[0][0]!=0)
				{
					if(m_nInCom2)
					{
						strncpy(m_strCom2ZbMain,g_FmlPack[wParam].aFormular[0],ZBNAME_LEN-1);
						m_strCom2ZbMain[ZBNAME_LEN-1] = 0;
					}
					else
					{
						strncpy(g_strZbMain,g_FmlPack[wParam].aFormular[0],ZBNAME_LEN-1);
						g_strZbMain[ZBNAME_LEN-1] = 0;
					}
					if(m_nInCom2) m_Com2DrawMain	= TRUE;
					else g_GH.bDrawMainZb = TRUE;
				}
				else
				{
					if(m_nInCom2) m_Com2DrawMain	= FALSE;
					else g_GH.bDrawMainZb = FALSE;
				}
				for(i=1;i<g_FmlPack[wParam].lRectNum;i++)
				{
					if(g_FmlPack[wParam].aFormular[i][0]!=0)
					{
						if(m_nInCom2)
						{
							strncpy(m_strCom2FavUseZb[i-1],g_FmlPack[wParam].aFormular[i],ZBNAME_LEN-1);
							m_strCom2FavUseZb[i-1][ZBNAME_LEN-1] = 0;
						}
						else
						{
							strncpy(g_strFavUseZb[i-1],g_FmlPack[wParam].aFormular[i],ZBNAME_LEN-1);
							g_strFavUseZb[i-1][ZBNAME_LEN-1] = 0;
						}
					}
				}
				m_NormalRegion = g_FmlPack[wParam].lRectNum;
				if(!m_nInCom2) 
					g_nZBWinNum = m_NormalRegion;
				m_bNotRedraw = TRUE;
				if(g_FmlPack[wParam].nPeriod != m_nPeriod)			//设置周期
					SetPeriod(g_FmlPack[wParam].nPeriod,TRUE,FALSE,FALSE);
				if(g_FmlPack[wParam].nPeriod!=m_nPeriod && m_nCommonPeriod!=m_nPeriod)	//设置不成功,认为未绑定周期,取用通用周期
					SetPeriod(m_nCommonPeriod,TRUE,FALSE,TRUE);
				m_bNotRedraw = FALSE;
				ProcessMsg(UM_UNUSEFMLPACK);
				break;
			}
			//临时替换
			m_bDbClickFT	= FALSE;
			m_bChgRegion	= TRUE;
			for(i=0;i<g_FmlPack[wParam].lRectNum;i++)
			{
				if(i==0)
				{
					if(g_FmlPack[wParam].aFormular[i][0]==0) 
						continue;
					else 
						m_aRegion[i].aIndexInfo[m_aRegion[i].nObject] = AddObjectByName(ZB_OBJECT,m_strZbMain);
				}
				else m_aRegion[i].aIndexInfo[m_aRegion[i].nObject] = AddObjectByName(ZB_OBJECT,m_strFavUseZb[i-1]);
				RejustOneObject(m_aRegion[i].aIndexInfo[m_aRegion[i].nObject],m_nDataNum,TRUE);
				if(m_aRegion[i].aIndexInfo[m_aRegion[i].nObject] && m_aRegion[i].aIndexInfo[m_aRegion[i].nObject]->nSet == HIS_DAT ) 
					ReadSomeStk(m_aRegion[i].aIndexInfo[m_aRegion[i].nObject]);
				CalcOne(m_aRegion[i].aIndexInfo[m_aRegion[i].nObject]);
				CalcOneGSRatio(m_aRegion[i].aIndexInfo[m_aRegion[i].nObject],(m_nLastNo+1)-m_nDrawNum);
				CalcOneRegionRatio(i);
				m_aRegion[i].nObject++;
			}
			m_nRegion		= g_FmlPack[wParam].lRectNum;
			m_nOldRegion	= m_nRegion;
			if ( m_nSelRegion <0 || m_nSelRegion >= m_nRegion )
				m_nSelRegion = m_nRegion-1;
			if ( m_nSelRegionBak <0 || m_nSelRegionBak >= m_nRegion )
				m_nSelRegionBak = m_nRegion-1;
			m_bNotRedraw = TRUE;
			if(g_FmlPack[wParam].nPeriod != m_nPeriod) 
				SetPeriod(g_FmlPack[wParam].nPeriod,TRUE,FALSE,FALSE);
			if(g_FmlPack[wParam].nPeriod != m_nPeriod && m_nCommonPeriod!=m_nPeriod)	//设置不成功,认为未绑定周期,取用通用周期
				SetPeriod(m_nCommonPeriod,TRUE,FALSE,TRUE);
			m_bNotRedraw = FALSE;
		}
		SetSwitchSel();
		break;
	case UM_UNUSEFMLPACK:
		{
			if(m_nStyle == FXT_MULTI) break;
			if(!m_bUseFmlPack) break;			//已经是常用指标组了,不用再切换
			m_bNotRedraw = TRUE;
			if(m_nCommonPeriod!=m_nPeriod && (m_lCurPackNo<0 || m_lCurPackNo>=g_nFmlPack || m_lBandPackNo<0 || m_lBandPackNo>=g_nFmlPack))
				SetPeriod(m_nCommonPeriod,TRUE,FALSE,TRUE);
			m_bNotRedraw = FALSE;
			int i=0,j=0;
			//先将全局指标载入
			if(m_nInCom2)
			{
				strncpy(m_strZbMain,m_strCom2ZbMain,ZBNAME_LEN-1);
				m_strZbMain[ZBNAME_LEN-1] = 0;
			}
			else
			{
				strncpy(m_strZbMain,g_strZbMain,ZBNAME_LEN-1);
				m_strZbMain[ZBNAME_LEN-1] = 0;
			}
			for(i=0;i<11;i++)
			{
				if(m_nInCom2) 
					strncpy(m_strFavUseZb[i],m_strCom2FavUseZb[i],ZBNAME_LEN-1);
				else 
					strncpy(m_strFavUseZb[i],g_strFavUseZb[i],ZBNAME_LEN-1);
				m_strFavUseZb[i][ZBNAME_LEN-1] = 0;
			}
			//
			for(i=0;i<m_nRegion;i++)
			{
				if(i>=m_NormalRegion)
				{
					while(m_aRegion[i].aIndexInfo[0])
					{
						RegionDelOne(i,m_aRegion[i].aIndexInfo[0]);
					}
					continue;
				}
				for(j=0;j<m_aRegion[i].nObject;j++)
				{
					if(m_aRegion[i].aIndexInfo[j]->nSet==ZB_OBJECT)
					{
						RegionDelOne(i,m_aRegion[i].aIndexInfo[j]);
						j--;
					}
				}
			}
			//相关标志应该在删除部件后设置,以免在处理时影响其它标志
			m_bDbClickFT	= FALSE;
			m_bUseFmlPack	= FALSE;
			m_bChgRegion	= TRUE;
			m_lBandPackNo	= -1;
			//暂不考虑主图指标显隐问题
			if(m_nInCom2) m_bDrawMainZb = m_Com2DrawMain;
			else m_bDrawMainZb = g_GH.bDrawMainZb;
			for(i=0;i<m_NormalRegion;i++)
			{
				if(i==0)
				{
					if(m_bDrawMainZb) m_aRegion[i].aIndexInfo[m_aRegion[i].nObject] = AddObjectByName(ZB_OBJECT,m_strZbMain);
					else continue;
				}
				else m_aRegion[i].aIndexInfo[m_aRegion[i].nObject] = AddObjectByName(ZB_OBJECT,m_strFavUseZb[i-1]);
				RejustOneObject(m_aRegion[i].aIndexInfo[m_aRegion[i].nObject],m_nDataNum);
				if ( m_aRegion[i].aIndexInfo[m_aRegion[i].nObject] && m_aRegion[i].aIndexInfo[m_aRegion[i].nObject]->nSet == HIS_DAT ) 
					ReadSomeStk(m_aRegion[i].aIndexInfo[m_aRegion[i].nObject]);
				CalcOne(m_aRegion[i].aIndexInfo[m_aRegion[i].nObject]);
				CalcOneGSRatio(m_aRegion[i].aIndexInfo[m_aRegion[i].nObject],(m_nLastNo+1)-m_nDrawNum);
				CalcOneRegionRatio(i);
				m_aRegion[i].nObject++;
			}
			m_nRegion		= m_NormalRegion;
			m_nOldRegion	= m_nRegion;
			if ( m_nSelRegion <0 || m_nSelRegion >= m_nRegion )
				m_nSelRegion = m_nRegion-1;
			if ( m_nSelRegionBak <0 || m_nSelRegionBak >= m_nRegion )
				m_nSelRegionBak = m_nRegion-1;
		}
		SetSwitchSel();
		break;
	case UM_TABSWITCHCHG:
		ResetAllSwitch();
		ProcessMsg(WM_PAINT);
		break;
	case UM_EXOPENBLOCKHQ:
		{
			if(wParam>=ZJHHY_START&&lParam==-1)
			{
				//板块指数咯
				int nBlockNo = wParam-ZJHHY_START;
				char *Code = g_pLcIdxMng->GetIdxStkCode(nBlockNo);
				long nIndex = g_pStockIO->GetIndex(Code, SZ);
				if(nIndex>=0) 
				{
					ProcessMsg(UM_SET_STOCK, nIndex, 0);
					ProcessMsg(WM_PAINT, 1);
				}
			}
		}
		break;
	case UM_SET_STOCK:					//切换股票到这里来
		{
			int nRet = UBase::ProcessMsg(message,wParam,lParam);
			int i = 0,j = 0;
			for(i=0;i<g_nFml2Stk;i++)
			{
				if(g_Fml2Stk[i].setcode==m_pStkInfo->setcode && strcmp(g_Fml2Stk[i].code,m_pStkInfo->Code)==0)
					break;
			}
			if(i<g_nFml2Stk)
			{
				for(j=0;j<g_nFmlPack;j++)
				{
					if(stricmp(g_Fml2Stk[i].aPackName,g_FmlPack[j].aPackName)==0)
						break;
				}
				if(j<g_nFmlPack) ProcessMsg(UM_USEFORMULAPACK,j);
				else ProcessMsg(UM_UNUSEFMLPACK);
			}
			else ProcessMsg(UM_UNUSEFMLPACK);

			//删除大盘风向
			if (m_pDPFXIndex)
			{
				for (int i=0;i<m_nRegion;i++ )
				{
					for (int j=0;j<m_aRegion[i].nObject;j++ )
					{
						TOBJECT *p = m_aRegion[i].aIndexInfo[j];
						if ( p->nSet == KL_OBJECT )
						{//不用本区域的比例等参数
							TINDEXINFO * pIndex = g_pCalc->_CallDTopViewCalc_8(KL_OBJECT,p->nGSIndex);
							if(pIndex->nIndexNo==m_pDPFXIndex->nIndexNo)
								DelZs();
						}
					}
				}
			}

			return nRet;
		}
		break;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

_inline double UFxt::GetRegEndRatio(int nNo,int nRegion)
{
	int		Start = (nRegion-1)*nRegion>>1;
	return (m_aRatio[Start+nNo]);
}

void UFxt::InitRegion()
{
	short i;
	double	dSRate = 0.0 ,dERate = 0.0;
	///////////////////////////////////
	CRect   rc(m_rcIn.left,m_rcIn.top,m_rcIn.right-FXT_RWIDTH,m_rcIn.bottom-FXT_BHEIGHT );
	if(m_nStyle == FXT_MULTI) 
		rc.SetRect(m_rcIn.left,m_rcIn.top,m_rcIn.right-FXT_RWIDTH,m_rcIn.bottom );
	///////////////////////////////
	if(m_bDbClickFT)
	{
		int dH = rc.bottom - rc.top;
		dSRate = GetRegEndRatio(0,m_nOldRegion);
		m_aRegion[0].Area   = CRect(rc.left,m_aRegion[0].Area.top,rc.right,rc.top+dSRate*dH);
		m_aRegion[0].DrawRc	= m_aRegion[0].Area;
		m_aRegion[0].DrawRc.top += 20;
		if ( !g_bFxtShowInfoDlg ) m_aRegion[0].DrawRc.top += TEXTHIGH;
		m_aRegion[0].DrawRc.bottom -= 12;

		m_aRegion[1].Area   = CRect(rc.left,m_aRegion[0].Area.bottom,rc.right,rc.bottom);
		m_aRegion[1].DrawRc	= m_aRegion[1].Area;
		m_aRegion[1].DrawRc.top += 20;
		m_aRegion[1].DrawRc.bottom -= 2;
	}
	else
	{
		int dH = rc.bottom - rc.top;
		for (  i=0;i<m_nRegion;i++ )
		{
			dERate = GetRegEndRatio(i,m_nRegion);
			m_aRegion[i].Area   = CRect(rc.left,int(rc.top+dSRate*dH),rc.right,int(rc.top+dERate*dH));
			m_aRegion[i].DrawRc	= m_aRegion[i].Area;
			if(m_nStyle == FXT_MULTI) 
			{
				m_aRegion[i].DrawRc.top += 2;
				if ( i==0 )
					m_aRegion[i].DrawRc.top += 20;
				m_aRegion[i].DrawRc.bottom -= 2;
			}
			else
			{
				m_aRegion[i].DrawRc.top += 20;	
				if ( i==0 )
				{
					if ( !g_bFxtShowInfoDlg )
						m_aRegion[i].DrawRc.top += TEXTHIGH;	
					m_aRegion[i].DrawRc.bottom -= 12;
				}
				else
					m_aRegion[i].DrawRc.bottom -= 2;
			}
			dSRate = dERate;
		}
	}
	m_MainYRc.SetRect(m_rcIn.right-FXT_RWIDTH,m_rcIn.top,m_rcIn.right,m_aRegion[0].Area.bottom );
}

int  UFxt::GetYPosOfRegion(int nRegionNo,double fY,int nNo)
{
	int nY = 0;
	TOBJECT * p = m_aRegion[nRegionNo].aIndexInfo[nNo];
	TINDEXINFO * pIndex = NULL;
	if ( !p || !m_aRegion[nRegionNo].aIndexInfo[0] )
		return 0;
	if(m_nStyle==FXT_MULTI&&nRegionNo==1)
	{
		double dPerUnit = 1;
		if ( (p->dMaxVal- p->dMinVal) > COMPPREC )
			dPerUnit = (double( m_aRegion[nRegionNo].DrawRc.bottom-m_aRegion[nRegionNo].DrawRc.top))/(double(m_aRegion[nRegionNo].dMaxVal-m_aRegion[nRegionNo].dMinVal));
		fY = (fY-m_aRegion[nRegionNo].dMinVal) * dPerUnit;
		if(fY > -10000000.0 && fY < 10000000.0)
			nY = int( m_aRegion[nRegionNo].DrawRc.bottom-fY+0.5);
		return nY;
	}
	///////
	if ( nRegionNo == 0 && m_nMainAxisStatus == R100_AXIS )
		return GetYPosR100_AXIS(nRegionNo,fY,nNo);
	if ( ( (p->nSet == HIS_DAT && nNo == 0 ) || (p->nSet==ZB_TYPE && (p->Property & OnlyMain ))) 
		&& nRegionNo == 0 )
	{
		//都是在统一坐标的情况下，共用一个标准
		switch ( m_nMainAxisStatus )
		{
		case LOG10_AXIS:
			{
				fY = log10(max(COMPPREC,fY));
				fY = (fY - m_aRegion[nRegionNo].dMinVal ) * m_aRegion[nRegionNo].dPerUnit;
			}
			break;
		case R100_AXIS:	//百分比在上面处理,此处无效
			{	
				double dPerUnit = 1;
				if ( (m_aRegion[nRegionNo].dMaxValEx- m_aRegion[nRegionNo].dMinValEx) > COMPPREC )
					dPerUnit = ( m_aRegion[nRegionNo].DrawRc.bottom - m_aRegion[nRegionNo].DrawRc.top )/(m_aRegion[nRegionNo].dMaxValEx - m_aRegion[nRegionNo].dMinValEx);
				fY = (fY - m_aRegion[nRegionNo].dMinValEx) * dPerUnit;
			}
			break;
		default:
			fY = (fY - m_aRegion[nRegionNo].dMinValEx ) * m_aRegion[nRegionNo].dPerUnitEx;
			break;
		}
	}
	else if ( nNo == 0 ) 
		fY = (fY - m_aRegion[nRegionNo].dMinVal ) * m_aRegion[nRegionNo].dPerUnit;
	else
	{
		//自立门户
		double dPerUnit = 1;
		if ( (p->dMaxVal- p->dMinVal) > COMPPREC )
			dPerUnit = (m_aRegion[nRegionNo].DrawRc.bottom - m_aRegion[nRegionNo].DrawRc.top)/(p->dMaxVal - p->dMinVal);
		fY = (fY - p->dMinVal) * dPerUnit;
	}
	if(fY > -10000000.0 && fY < 10000000.0)
		nY = int( m_aRegion[nRegionNo].DrawRc.bottom - fY + 0.5);
	return nY;
}

double UFxt::GetValFromY(int nRegionNo,int y)
{
	//对数坐标
	if(nRegionNo==0 && m_nMainAxisStatus==LOG10_AXIS)
	{
		double dVal = 0;
		dVal = (m_aRegion[nRegionNo].DrawRc.bottom - y)/m_aRegion[nRegionNo].dPerUnit;
		dVal += m_aRegion[nRegionNo].dMinVal;
		return pow(10,dVal);
	}
	double dVal = 0;
	dVal = (m_aRegion[nRegionNo].DrawRc.bottom - y)/m_aRegion[nRegionNo].dPerUnitEx;
	dVal += m_aRegion[nRegionNo].dMinValEx;
	return dVal;
}

int UFxt::GetItemNoFromTime(long lTime)
{
	int		i;
	union long_short	lMinute;
	memcpy(&lMinute,&lTime,sizeof(long));
	if (  (m_nPeriod == PER_MIN5 
		|| m_nPeriod ==PER_MIN15 
		|| m_nPeriod ==PER_MIN30 
		|| m_nPeriod ==PER_HOUR 
		|| m_nPeriod ==PER_MIN1 
		|| m_nPeriod ==PER_MINN) )
	{
		for (i=0;i<m_nDataNum;i++)
		{	
			if ( lMinute.Daye.Day == m_pObject->pHisDay[i].Time.Daye.Day && 
				 lMinute.Daye.Minute == m_pObject->pHisDay[i].Time.Daye.Minute )
				return i;
		}
	}
	else
	{
		for (i=0;i<m_nDataNum;i++)
		{	
			if ( lTime >= m_pObject->pHisDay[i].Time.Date &&
				(i==m_nDataNum-1 || lTime < m_pObject->pHisDay[min(m_nDataNum,i+1)].Time.Date ) ) 
				return i;
		}
	}
	return -1;
}

///全部的Item都不计入leftNo
int UFxt::GetItemNoFromX(int x)
{
	int	  i;
	double dHalf = (m_dDistance+m_dWidth)/2.0;
	if ( x<=0 )
	{
		for ( i=m_nLeftNo;i>=0;--i )
		{
			if(i==m_nLeftNo)
			{
				if ( x >= m_pAllPosOfX[i]-dHalf && x <= m_pAllPosOfX[i]+dHalf )
					return (i-m_nLeftNo);
			}
			else
			{
				if ( x >= m_pAllPosOfX[i]-dHalf && x <= m_pAllPosOfX[i+1]-dHalf )
					return (i-m_nLeftNo);
			}
		}
		return 0;
	}
	for ( i=0;i<m_nDrawNum;i++ )
	{
		if(i==0)
		{
			if ( x >= m_pPosOfX[i]-dHalf && x < m_pPosOfX[i]+dHalf )
				return i;
		}
		else
		{
			if ( x >= m_pPosOfX[i-1]+dHalf && x < m_pPosOfX[i]+dHalf )
				return i;
		}
	}
	if ( i == m_nDrawNum )
	{
		//放大范围
		for ( i=0;i<m_nDrawNum-1;i++ )
		{
			if ( x >= m_pPosOfX[i]-dHalf && x <= m_pPosOfX[i]+ m_dDistance+m_dWidth  )
				return i;
		}
	}
	return m_nDrawNum-1;
}

int UFxt::GetAbsItemNoFromX(int x)
{
	if(m_nDataNum<=0) return 0;
	int i;
	double dHalf = (m_dDistance+m_dWidth)/2.0;
	if ( x<=0 )
	{
		for(i=m_nLeftNo-1;i>=0;--i) if( x>=m_pAllPosOfX[i]-dHalf) return i;
		return 0;
	}
	for(i=m_nLeftNo;i<m_nDataNum;i++) if( x<=m_pAllPosOfX[i]+dHalf) return i;
	return (m_nDataNum-1);
}

///全部的Item都不计入:leftNo
POINT UFxt::GetPointFromItem(int No)
{
	POINT pt;
	if ( No > m_nDrawNum-1 )
		No = m_nDrawNum-1;
	else if ( No <0 )
	{
		pt.x = m_pAllPosOfX[m_nLeftNo+No];
		pt.y = GetYPosOfRegion(0,(m_pObject->pHisDay+m_nLeftNo+No)->Close);
		return pt;
	}
	else if ( No > m_nDataNum-1 )
		No = m_nDataNum-1;
	pt.x = m_pPosOfX[No];
	pt.y = GetYPosOfRegion(0,(m_pObject->pHisDay+m_nLeftNo+No)->Close);
	return pt;
}

//显示标题的内核部分，一般都是在内存DC中实现
//调用较为频繁，定位内联
extern COLORREF CompareDlgInfoStr(CString tmpStr1,CString tmpStr2);
const char *strTQType[4] = {"", "-前复权", "-后复权","-定点复权"}; 
void UFxt::InnerDisp(CDC * pMDC,int nRegion,int ItemNo)
{
	//遇到K线图
	int   Begin = 0;
	int	  nOverlap = 0;
	CSize size  = 0;
	MemStkInfo  * scode = NULL;
	static  char  strInfo[200] , strParaInfo[81];		//将多次运用
	TOBJECT    *  p = m_aRegion[nRegion].aIndexInfo[Begin];
	memset(strInfo,0,200);
	///////////////////////////
	memset(m_aRegion[nRegion].aRct,0,58*sizeof(RECTINFO));
	m_aRegion[nRegion].nRect = 0;
	/////////////////////////////
	if ( p->nSet == HIS_DAT )
	{	
		scode = ( p->nCurGP>=0 ? (*g_pStockIO)[p->nCurGP] : m_pStkInfo );
		_snprintf(strInfo,80,"%s(%s)  ",scode->Name, scode->Code);
		size += g_d.DisplayText(pMDC,1,0,VipColor.TXTCOLOR,"%s",strInfo);

		if(m_pStkInfo->J_zjhhy>0)
		{
			ZJHHY_Cfg *pZcfg = g_pConstMng->GetHyFromHYID(m_pStkInfo->J_zjhhy);
			if(pZcfg)
			{
				char strhy[200]={0};
				sprintf(strhy, "  行业:%s  ", pZcfg->HYName);
				size += g_d.DisplayText(pMDC,size.cx,0,VipColor.TXTCOLOR,_F(strhy));
			}
		}
		/*
		size.cx-=2;
		if (strcmp(g_CalcPeriodStr[m_nCalcPeriod], "多日线") == 0)
			sprintf(strInfo,"(%d日线%s) ",g_nDaynNum,strTQType[g_nTQFlag]);
		else if (strcmp(g_CalcPeriodStr[m_nCalcPeriod], "多分钟") == 0)
			sprintf(strInfo,"(%d分钟%s) ",g_nMinnNum,strTQType[g_nTQFlag]);
		else
			sprintf(strInfo,"(%s%s) ",g_CalcPeriodStr[m_nCalcPeriod],strTQType[g_nTQFlag]);
		size += g_d.DisplayText(pMDC,size.cx,0,VipColor.TXTCOLOR,_F(strInfo));
		*/
		/////////////////////////////////
		if ( m_aRegion[nRegion].nObject == 1 )
			return;
		else
		{	
			Begin = 1;
			p = m_aRegion[nRegion].aIndexInfo[Begin];
		}
	}
	if ( !m_bOverStk && m_bExistZS && nRegion == 0)		//指示
	{
		if ( m_KLZS.p )
		{
			if ( m_SelectInfo.bExist && m_SelectInfo.nSet == KL_OBJECT )
				LocateZS(pMDC,m_KLZS,m_hKLineShow_Sel,nRegion);
			else
				LocateZS(pMDC,m_KLZS,m_hKLineShow,nRegion);
		}
		if ( m_ExpZS.p )
		{
			if ( m_SelectInfo.bExist && m_SelectInfo.nSet == EXP_OBJECT )
				LocateZS(pMDC,m_ExpZS,m_hExpUpShow_Sel,nRegion);
			else
				LocateZS(pMDC,m_ExpZS,m_hExpUpShow,nRegion);
		}
	}
	//////////////////////////指标
	while( Begin<m_aRegion[nRegion].nObject )
	{
		memset(strInfo,0,200);
		if ( p->nSet != HIS_DAT )
		{
			TINDEXINFO * pIndex = NULL;
			if ( p->nSet == SPECLINE_OBJECT )
				pIndex = g_SpeclIndex+p->nGSIndex;
			else if ( p->nSet == ZB_OBJECT )
				pIndex = g_pCalc->_CallDTopViewCalc_8(p->nSet,p->nGSIndex);
			if ( pIndex  )	
			{	////////////////////////////////////////////////////////////////
				
				if ( stricmp(pIndex->acCode,"MA") && stricmp(pIndex->acCode,"MA2") && stricmp(pIndex->acCode,"VOL")) 
				{
					if ( pIndex->nParaNum >0 )
					{
						if ( p->nCurGP != -1 && p->nCurGP != m_nCurGP )
							_snprintf(strParaInfo,80,"  %s%s(",(*g_pStockIO)[p->nCurGP]->Name,p->acCode);
						else
							_snprintf(strParaInfo,80,"  %s(",p->acCode);
						strcat(strInfo,strParaInfo);
						long lVal = 0;
						for (int k=0;k<pIndex->nParaNum-1;k++ )
						{	
							lVal = pIndex->aPara[k].nValue[m_nCalcPeriod];
							_snprintf(strParaInfo,80,"%d,",lVal);
							strcat(strInfo,strParaInfo);
						}
						lVal = pIndex->aPara[k].nValue[m_nCalcPeriod];
						_snprintf(strParaInfo,80,"%d) ",lVal);
						strcat(strInfo,strParaInfo);
						size += g_d.DisplayText(pMDC,size.cx,0,VipColor.TXTCOLOR,0,"%s",strInfo);
					}
					else
					{
						if ( p->nCurGP != -1 && p->nCurGP != m_nCurGP )
							_snprintf(strParaInfo,80,"  %s%s",(*g_pStockIO)[p->nCurGP]->Name,p->acCode);
						else
							_snprintf(strInfo,80,"  %s",p->acCode); 
						size += g_d.DisplayText(pMDC,size.cx,0,VipColor.TXTCOLOR,0,"%s",strInfo);
					}
				}
				else
					size.cx -= 5;
				memset(strInfo,0,200);
				///////////////////////////////////////////////
				//防止最后一根K线的指标值随放缩不一致
				int	nCurrNo = 0;
				if ( m_KLock != -1 && m_KLock < m_nDataNum )
					nCurrNo = m_KLock;
				else
					nCurrNo = m_nLeftNo + ItemNo;
				for (int j=0,color=0;j<p->nOutNum;j++ )
				{ 	
					int nIncre = m_aRegion[nRegion].nRect;
					if ( pIndex->nSet == ZB_OBJECT )
					{
						if ( stricmp(pIndex->acCode,"MA") == 0 || stricmp(pIndex->acCode,"MA2")==0 ) 
						{ 
							for ( int mtmpI=0;mtmpI<pIndex->nLineNum;++mtmpI )
							{
								if(mtmpI<pIndex->nParaNum)
									sprintf(pIndex->aLine[mtmpI].acLineName,"MA%.0f",
										pIndex->aPara[mtmpI].nValue[m_nCalcPeriod]);
							}
						}
						else if ( stricmp(pIndex->acCode,"VOL") == 0 )
						{
							for ( int mtmpI=1;mtmpI<pIndex->nLineNum;++mtmpI )
							{
								if(mtmpI<pIndex->nParaNum+2)
									sprintf(pIndex->aLine[mtmpI].acLineName,"MA%.0f",
										pIndex->aPara[mtmpI-1].nValue[m_nCalcPeriod]);
							}
						}
					}
					short nDataFlag = 0;
					if( TBE_MEANLESS(p->pCalcData[j][m_nDataNum])!=0 )
					{
						HandleValue * phv = NULL;
						if(TBE_MEANLESS(p->pCalcData[j][m_nDataNum])) 
						{
							DWORD pdVal;
							memcpy(&pdVal, &p->pCalcData[j][m_nDataNum], sizeof(DWORD));
							phv = (HandleValue *)pdVal;
							if(phv->nValidFlag!=TMEANLESS_INT)
								phv = NULL;
						}
						if(!phv) {}
						else if(phv->nHandleType=='k'||phv->nHandleType=='K'||phv->nHandleType=='l'||phv->nHandleType=='i'
							||phv->nHandleType=='I'||phv->nHandleType=='t'||phv->nHandleType=='T'||phv->nHandleType=='D')
						{
							strcpy(strInfo, "");
						}
					}
					else if(nCurrNo<0 || TBE_MEANLESS(p->pCalcData[j][nCurrNo])==0)
						_snprintf(strInfo,100,"%s:-",_F(pIndex->aLine[j].acLineName));
					else
					{
						if(nCurrNo>0&&TBE_MEANLESS(p->pCalcData[j][nCurrNo])!=0)
						{
							float fDatac = p->pCalcData[j][nCurrNo]-p->pCalcData[j][nCurrNo-1];
							if(fabs(fDatac)<COMPPREC) nDataFlag = 0;
							else if(fDatac>COMPPREC) nDataFlag = 1;
							else nDataFlag = -1;
						}
						if(nCurrNo>=0&&TBE_MEANLESS(p->pCalcData[j][nCurrNo])!=0)
							_snprintf(strInfo,100,(m_XsFlag==3)?"%s:%-8.3f":"%s:%-8.2f",_F(pIndex->aLine[j].acLineName),p->pCalcData[j][nCurrNo]);
					}
					size.cx += 5;
					///////////////////////
					m_aRegion[nRegion].aRct[nIncre].thObj = Begin;
					m_aRegion[nRegion].aRct[nIncre].thOut = j;
					m_aRegion[nRegion].aRct[nIncre].aRect.left = size.cx;
					m_aRegion[nRegion].aRct[nIncre].aRect.top  = 0;
					m_aRegion[nRegion].aRct[nIncre].aRect.bottom= 32;
					///////////////////////
					COLORREF nColor = pIndex->aLine[j].nColor;
					int nWidth = pIndex->aLine[j].nWidth;
					if(nWidth<1||nColor<RGB(0,0,0)||nColor>RGB(255,255,255)) nColor = VipColor.ZBCOLOR[color%6];
					size += g_d.DisplayText(pMDC,size.cx,0,nColor,0,"%s",strInfo);			
					if ( !m_bOverStk && 
						( pIndex->aLine[j].nType == VOLSTICK || pIndex->aLine[j].nType == COLORSTICK || pIndex->aLine[j].nType == POINTDOT) )
					{						
					}
					else
						++color;
					///////////////////////
					m_aRegion[nRegion].aRct[nIncre].aRect.right = size.cx;
					m_aRegion[nRegion].nRect ++;

					if(nDataFlag && g_bZBudFlag) 
					{
						CFont *oldfont = g_d.SelectFont(pMDC,INFO_FONT);
						COLORREF nColorFlag = nDataFlag>0?VipColor.KPCOLOR:VipColor.KNCOLOR;
						char *strFlag=" ";
						if(nDataFlag>0)
							strFlag="↑";
						else if(nDataFlag<0)
							strFlag="↓";
						size += g_d.DisplayText(pMDC,size.cx-3,(nDataFlag>0)?(-1):(-2),nColorFlag,0,"%s",strFlag);
						size.cx -= 5;
						g_d.RestoreFont(pMDC, oldfont);
					}
				}
			}
		}
		else if ( p->nSet == HIS_DAT )
		{
			scode = (*g_pStockIO)[p->nCurGP];
			_snprintf(strInfo,80,"[%s]",scode->Name);
			size.cx += 2;
			size += g_d.DisplayText(pMDC,size.cx,0,l_KLineColor[6+nOverlap],0,"%s",strInfo);
			nOverlap++;
		}
		Begin++;
		p = m_aRegion[nRegion].aIndexInfo[Begin];
	}
	//另外加入一行
	if ( !g_bFxtShowInfoDlg && nRegion == 0 && strcmp(g_DlgInfo.name, m_pStkInfo->Name)==0)
	{
		int tmpPos=0;
		size=g_d.DisplayText(pMDC,1,TEXTHIGH-2,VipColor.TXTCOLOR,0,"%s",g_DlgInfo.time1);
		tmpPos=1+size.cx+3;

		size=g_d.DisplayText(pMDC,tmpPos,TEXTHIGH-2,CompareDlgInfoStr(g_DlgInfo.open,g_DlgInfo.lastclose),0,"开%s",g_DlgInfo.open);
		tmpPos+=size.cx+3;

		size=g_d.DisplayText(pMDC,tmpPos,TEXTHIGH-2,CompareDlgInfoStr(g_DlgInfo.max,g_DlgInfo.lastclose),0,"高%s",g_DlgInfo.max);
		tmpPos+=size.cx+3;

		size=g_d.DisplayText(pMDC,tmpPos,TEXTHIGH-2,CompareDlgInfoStr(g_DlgInfo.min,g_DlgInfo.lastclose),0,"低%s",g_DlgInfo.min);
		tmpPos+=size.cx+3;

		size=g_d.DisplayText(pMDC,tmpPos,TEXTHIGH-2,CompareDlgInfoStr(g_DlgInfo.close,g_DlgInfo.lastclose),0,"收%s",g_DlgInfo.close);
		tmpPos+=size.cx+3;

		size=g_d.DisplayText(pMDC,tmpPos,TEXTHIGH-2,VipColor.VOLCOLOR,0,"量%s",g_DlgInfo.volume);
		tmpPos+=size.cx+3;

		size=g_d.DisplayText(pMDC,tmpPos,TEXTHIGH-2,VipColor.TXTCOLOR,0,"额%s",g_DlgInfo.amount);
		tmpPos+=size.cx+3;

		if(g_DlgInfo.zhangdie[0])
			size=g_d.DisplayText(pMDC,tmpPos,TEXTHIGH-2,CompareDlgInfoStr(g_DlgInfo.zhangfu,"0"),0,"幅%s(%s)",g_DlgInfo.zhangdie,g_DlgInfo.zhangfu);
		else
			size=g_d.DisplayText(pMDC,tmpPos,TEXTHIGH-2,CompareDlgInfoStr(g_DlgInfo.zhangfu,"0"),0,"幅-");
		tmpPos+=size.cx+3;
				
		if(g_DlgInfo.zhenfu[0])
			size=g_d.DisplayText(pMDC,tmpPos,TEXTHIGH-2,VipColor.TXTCOLOR,0,"振%s",g_DlgInfo.zhenfu);
		else
			size=g_d.DisplayText(pMDC,tmpPos,TEXTHIGH-2,VipColor.TXTCOLOR,0,"%s","振-");
		tmpPos+=size.cx+3;

		if(g_DlgInfo.hsl[0]!=0)
		{
			size=g_d.DisplayText(pMDC,tmpPos,TEXTHIGH-2,VipColor.TXTCOLOR,0,"换%s",g_DlgInfo.hsl);
			tmpPos+=size.cx+3;
		}
		if(g_DlgInfo.ltgb[0]!=0)
		{
			size=g_d.DisplayText(pMDC,tmpPos,TEXTHIGH-2,VipColor.TXTCOLOR,0,"流通%s",g_DlgInfo.ltgb);
			tmpPos+=size.cx+3;
		}

		AllTrim(g_DlgInfo.ltgb);
	}
}

//建议全部画的情况下，调下面函数，可以减少
//创建位图、内存DC等的时间
void UFxt::Disp(CDC * pDC,int nX,int nY,int IsMemDC)
{
	int i,No;
	if ( m_nDataNum <=0  || m_nStyle==FXT_MULTI )
		return;
	CRect   XRc(0,0,m_rcIn.right-FXT_RWIDTH,TEXTHIGH-2);
	//确保位图足够大
	XRc.bottom = 2*(TEXTHIGH-2);
	CREATE_MEMDC(XRc)
	if(nX<0)
		No = m_nDrawNum-1;
	else			 
		No = GetItemNoFromX(nX);
	for(i=0;i<m_nRegion;i++)
	{	//显示标题
		if ( !m_aRegion[i].aIndexInfo[0] )
			continue;
		//遇到K线图
		if ( !g_bFxtShowInfoDlg && i==0 )
			XRc.bottom = 2*(TEXTHIGH-2);
		else
			XRc.bottom = TEXTHIGH-2;
		//////////////////////////////////
	//	g_d.Bar(&MemDC,0,0,XRc.Width(),XRc.Height(),BACK_BRUSH);
		g_d.Draw3DBar3(&MemDC,XRc,Scheme_Hold.VipColor.COMVIEWINFOBC,FALSE);
		InnerDisp(&MemDC,i,No);
		if ( IsMemDC )
			pDC->BitBlt(m_aRegion[i].Area.left,m_aRegion[i].Area.top+1,XRc.Width(),XRc.Height(),&MemDC,0,0,SRCCOPY);
		else
			pDC->BitBlt(m_aRegion[i].Area.left+left,m_aRegion[i].Area.top+1+top,XRc.Width(),XRc.Height(),&MemDC,0,0,SRCCOPY);
	}
	DELETE_MEMDC
}

//把nX、nY当做相对点
void UFxt::DrawXRule(CDC * pDC,int nX,int nY)
{
	if(m_rcIn.right<=FXT_RWIDTH) return;
	int i,Oldx=0,OldOne = 0;
	CRect  XRc(0,0,m_rcIn.right-FXT_RWIDTH,FXT_BHEIGHT);
	CREATE_MEMDC(XRc)

	CPen  *oldpen = g_d.SelectPen(&MemDC,AXIS_PEN);
	if ( m_nDataNum >0 && m_nStyle != FXT_MULTI )
	{
		struct AnalyData * p = m_pObject->pHisDay+m_nLeftNo;
		const int nYear = 40 , nHour = 60;
		switch ( m_nPeriod )
		{
		case PER_DAY:
		case PER_WEEK:
		case PER_MONTH:
		case PER_DAYN:
		case PER_SEASON:
		case PER_YEAR:
			for ( i=0;i<m_nDrawNum;i++ )
			{
				if ( (p[i].Time.Date/100)%100 != OldOne )	//同一天，只显示一次
				{	
					if ( m_pPosOfX[i] < nYear )
						g_d.DisplayText(&MemDC,m_pPosOfX[0]+1,1,VipColor.TIMECOLOR,0,_F("%04d年"),p[0].Time.Date/10000);
					if ( m_pPosOfX[i]-Oldx > 15 )
					{
						if ( m_pPosOfX[i] > nYear )
							g_d.DisplayText(&MemDC,m_pPosOfX[i]+1,1,VipColor.TIMECOLOR,0,"%d",(p[i].Time.Date/100)%100);
						if ( Oldx >0 )
						{
							for (int k=1;k<=3;k++ )
								g_d.DrawLine(&MemDC,Oldx+int((m_pPosOfX[i]-Oldx)*k/4.0+0.5),0,Oldx+int((m_pPosOfX[i]-Oldx)*k/4.0+0.5),4);
						}
						if ( m_pPosOfX[i] > nYear )
							g_d.DrawLine(&MemDC,m_pPosOfX[i],0,m_pPosOfX[i],FXT_BHEIGHT);
						Oldx = m_pPosOfX[i];
					}
					OldOne = (p[i].Time.Date/100)%100;
				}
			}
			break;
		case PER_MIN1:
		case PER_MIN5:
		case PER_MIN15:
		case PER_MIN30:
		case PER_HOUR:
		case PER_MINN:
			for ( i=0;i<m_nDrawNum;i++ )
			{
				if ( p[i].Time.Daye.Minute != OldOne )	//同一分钟只显示一次
				{
					if ( m_pPosOfX[i] < nHour )
						g_d.DisplayText(&MemDC,m_pPosOfX[0]+1,1,VipColor.TIMECOLOR,0,"%02d月%02d日",p[0].Time.Daye.Day/100,p[0].Time.Daye.Day%100);
					if ( m_pPosOfX[i]-Oldx > 40 )
					{
						if ( m_pPosOfX[i] > nHour )
							g_d.DisplayText(&MemDC,m_pPosOfX[i]+1,1,VipColor.TIMECOLOR,0,"%02d:%02d",p[i].Time.Daye.Minute/60,p[i].Time.Daye.Minute%60);
						if ( Oldx >0 )
						{
							for (int k=1;k<=3;k++ )
								g_d.DrawLine(&MemDC,Oldx+int((m_pPosOfX[i]-Oldx)*k/4.0+0.5),0,Oldx+int((m_pPosOfX[i]-Oldx)*k/4.0+0.5),4);
						}
						if ( m_pPosOfX[i] > nHour )
							g_d.DrawLine(&MemDC,m_pPosOfX[i],0,m_pPosOfX[i],FXT_BHEIGHT);
						Oldx = m_pPosOfX[i];
					}
					OldOne = p[i].Time.Daye.Minute;
				}
			}
			break;
		default:
			break;
		}
		////////////////////////////
		if ( m_rcIn.PtInRect(CPoint(nX,nY)) && nY<bottom-FXT_BHEIGHT)
		{	
			char  Content[100];
			if ( m_bKeyCross )
				i = m_nKeyNo - m_nLeftNo;
			else
			{
				double dHalf = (m_dDistance+m_dWidth)/2.0;
				for ( i=0;i<m_nDrawNum;i++ )
				{
					if(i==0)
					{
						if ( nX >= m_pPosOfX[i]-dHalf && nX < m_pPosOfX[i]+dHalf )
							break;
					}
					else
					{
						if ( nX >= m_pPosOfX[i-1]+dHalf && nX < m_pPosOfX[i]+dHalf )
							break;
					}
				}
			}
			if ( i == m_nDrawNum )
				sprintf(Content,MEANLESS_STR);
			else if ( m_nPeriod == PER_DAY || m_nPeriod == PER_WEEK 
				|| m_nPeriod == PER_MONTH  || m_nPeriod == PER_DAYN 
				|| m_nPeriod == PER_SEASON  || m_nPeriod == PER_YEAR )
				sprintf(Content,"%4u-%02u-%02u(%s)",p[i].Time.Date/10000,(p[i].Time.Date/100)%100,p[i].Time.Date%100,
				strWeekInfo[GetWeek(p[i].Time.Date)]);
			else
			{
				sprintf(Content,"%02d/%02u %2u:%02u",
					p[i].Time.Daye.Day/100,p[i].Time.Daye.Day%100,
					p[i].Time.Daye.Minute/60,p[i].Time.Daye.Minute%60);
			}
			CSize size = MemDC.GetTextExtent(Content);
			CRect  SwinRc;
			if ( nX+size.cx+1 >  m_rcIn.right-FXT_RWIDTH )
				SwinRc.SetRect(nX-size.cx-1,0,nX,FXT_BHEIGHT);
			else
				SwinRc.SetRect(nX,0,nX+size.cx+1,FXT_BHEIGHT);
			g_d.DrawSwim(&MemDC,SwinRc,_F(Content));
		}
	}
	//多股同列
	g_d.DrawLine(&MemDC,0,0,0,FXT_BHEIGHT);
	g_d.DrawLine(&MemDC,0,0,m_rcIn.right,0);
	if(m_nStyle!=FXT_MULTI)
		pDC->BitBlt(left,bottom-FXT_BHEIGHT,XRc.Width(),XRc.Height(),&MemDC,0,0,SRCCOPY);
	g_d.RestorePen(&MemDC,oldpen);
	DELETE_MEMDC
}

void UFxt::InnerDrawY(CDC *pDC,CDC * pMDC,int nRegion,int nX,int nY)
{
	long j,y,yVal;
	TREGION    & theRg = m_aRegion[nRegion];
	CSize size;
	//如果是自定义坐标方式
	if ( theRg.aIndexInfo[0]->nSet == ZB_OBJECT 
		&& (theRg.aIndexInfo[0]->Property & DEFINE_Y) )
	{
		TINDEXINFO * pIndex = g_pCalc->_CallDTopViewCalc_8(ZB_OBJECT,theRg.aIndexInfo[0]->nGSIndex);
		double Ratio = 1.0;
		for ( j=0;j<7;j++ )
		{
			if ( fabs(pIndex->fAxisValue[j]) >= 10000000 )
			{
				Ratio = 10000000.0;
				break;
			}
			else if ( fabs(pIndex->fAxisValue[j]) >= 100000 && Ratio <= 100000 )
				Ratio = 100000.0;
			else if ( fabs(pIndex->fAxisValue[j]) >= 1000  && Ratio <= 1000 )
				Ratio = 1000.0;
		}
		for ( j=0;j<7;j++ )
		{	
			y = GetYPosOfRegion(nRegion,pIndex->fAxisValue[j],0);
			if ( y + m_YDy < theRg.Area.bottom && y - m_YDy > theRg.Area.top )
			{
				g_d.DisplayText(pMDC,0,y-m_YDy,VipColor.TIMECOLOR,FXT_RWIDTH-2,"%5.2f",pIndex->fAxisValue[j]/Ratio);
				if ( g_bHasYHidden )
				{
					theRg.nDotInfo[theRg.nDotNum] = y - theRg.DrawRc.top;
					++ theRg.nDotNum;					
				}
			}
		}
		if ( Ratio > 1.0 )
		{			
			char Info[100];
			if ( Ratio >= 100000.0 )
				sprintf(Info,_F("×10万"));
			else if ( Ratio >= 10000.0 )
				sprintf(Info,_F("×1万"));
			else
				sprintf(Info,_F("×1000"));
			size = pMDC->GetTextExtent(Info);
			g_d.Bar(pMDC,1,theRg.DrawRc.bottom-TEXTHIGH-1,size.cx+4,theRg.DrawRc.bottom-2,BACK_BRUSH);
			g_d.DisplayText(pMDC,3,theRg.DrawRc.bottom-TEXTHIGH-1,VipColor.TIMECOLOR,0,"%s",Info);
			g_d.DrawRect(pMDC,1,theRg.DrawRc.bottom-TEXTHIGH-1,size.cx+4,theRg.DrawRc.bottom-2);
		}
	}
	else
	{
		if ( !theRg.RulerInfo.bValid )
			return;
		if(nRegion == 0 && (m_nMainAxisStatus!=NORMAL_AXIS)) //如果是特别坐标
		{
			float tmpRatio=0.01f;
			float tmpClose;
			//先得到基准价位
			if(m_nMainAxisStatus==LOG10_AXIS || m_nMainAxisStatus==SAMESCALE_AXIS)
				tmpClose=m_pObject->pHisDay[max(m_nLastNo-1,0)].Close;
			else if(m_nMainAxisStatus==SAMESECT_AXIS || m_nMainAxisStatus==GOLDEN_AXIS)
				tmpClose=theRg.dMinVal;
			else
				tmpClose=m_pObject->pHisDay[m_nLeftNo].Open;
			tmpClose=max(COMPPREC,tmpClose);
			//先得到变动比率
			if(m_nMainAxisStatus==SAMESCALE_AXIS)
				tmpRatio=max(1,g_GH.nPerScale)*0.01;
			else if(m_nMainAxisStatus==SAMESECT_AXIS)
				tmpRatio=(theRg.dMaxVal-theRg.dMinVal)/(max(1,g_GH.nPerSect)*tmpClose);
			else if(m_nMainAxisStatus==LOG10_AXIS)
				tmpRatio=0.1f;
			else if(m_nMainAxisStatus==GOLDEN_AXIS) //对于GOLDEN_AXIS,tmpRatio和tmpStep无用
				tmpRatio=0.1f;
			else
			{
				for(tmpRatio=0.01f;(theRg.dMaxVal-theRg.dMinVal)/(tmpClose*tmpRatio)>20;tmpRatio*=2);
			}
			//得到变动步长
			double tmpStep=tmpRatio*tmpClose;
			//开始向上下两个方向画
			for(int iDirection=0;iDirection<2;iDirection++)
			{
				if((m_nMainAxisStatus==SAMESECT_AXIS||m_nMainAxisStatus==GOLDEN_AXIS) && iDirection!=0)
					break;
				int Oldy = 0,Lasty=0,tmpy=0,tmpSep=0,tmpCnt=0;
				int DispInfo1 = 0;
				int DispInfo2 = 0;
				int DispInfo3 = 0;
				double dLastFrom=tmpClose,tmpLastFrom=tmpClose;
				if(m_nMainAxisStatus==LOG10_AXIS)
					tmpSep=8;
				double dOldFrom=dLastFrom;			
				do
				{		
					y = GetYPosOfRegion(nRegion,dLastFrom);
					if ( theRg.Area.bottom > y+TEXTHIGH/2+tmpSep && theRg.Area.top < y-TEXTHIGH/2-12-tmpSep)
					{
						//Y轴象素相隔足够多才写文字
						if( Lasty<COMPPREC || fabs(Lasty-y)>10+tmpSep*2 )				
						{
							Lasty=y;
							double tmpVal = 100*(dLastFrom-tmpClose)/tmpClose;						
							tmpy=y-m_YDy;
							if(m_nMainAxisStatus!=R100_AXIS)
							{
								if(abs(dLastFrom)>=1000.0)
									g_d.DisplayText(pMDC,0,y-m_YDy,VipColor.TIMECOLOR,FXT_RWIDTH-2,"%5.0f",dLastFrom);
								else if(abs(dLastFrom)>=100.0)
									g_d.DisplayText(pMDC,0,y-m_YDy,VipColor.TIMECOLOR,FXT_RWIDTH-2,"%5.1f",dLastFrom);
								else if(m_XsFlag==3 && abs(dLastFrom)>=10.0)
									g_d.DisplayText(pMDC,0,y-m_YDy,VipColor.TIMECOLOR,FXT_RWIDTH-2,"%5.2f",dLastFrom);
								else
									g_d.DisplayText(pMDC,0,y-m_YDy,VipColor.TIMECOLOR,FXT_RWIDTH-2,(m_XsFlag==3)?"%5.3f":"%5.2f",dLastFrom);
								tmpy+=2*m_YDy-3;
							}
							if(  m_nMainAxisStatus==R100_AXIS || 
								(m_nMainAxisStatus==GOLDEN_AXIS && tmpCnt!=0 && tmpCnt!=4 && tmpCnt!=7) ||
								(m_nMainAxisStatus==LOG10_AXIS && fabs(tmpVal)>COMPPREC)
							  )
							{
								if(m_nMainAxisStatus==LOG10_AXIS) //对于对数坐标,要重新计算显示的百分比值
								{
									if(iDirection==0)
										tmpVal = 100*tmpCnt*0.1;
									else
										tmpVal = -100*tmpCnt*0.1;
								}
								//
								if(m_nMainAxisStatus==GOLDEN_AXIS)
									g_d.DisplayText(pMDC,0,tmpy,VipColor.TIMECOLOR,FXT_RWIDTH-2,"%4.1f%%",100*(dLastFrom-tmpClose)/(theRg.dMaxVal-theRg.dMinVal) );
								else if ( m_bR100Dot )
									g_d.DisplayText(pMDC,0,tmpy,VipColor.TIMECOLOR,FXT_RWIDTH-2,"%4.1f%%",tmpVal );
								else
								{	
									if(tmpVal>=0) yVal = long(tmpVal+0.5);
									else yVal = long(tmpVal-0.5);
									g_d.DisplayText(pMDC,0,tmpy,VipColor.TIMECOLOR,FXT_RWIDTH-2,"%5d%%",yVal);
								}
							}
							//标识基准线
							if(fabs(tmpVal)<COMPPREC && m_nMainAxisStatus!=SAMESECT_AXIS && m_nMainAxisStatus!=GOLDEN_AXIS)
								theRg.nBaseDot=theRg.nDotNum;
						}
					}
					if ( theRg.Area.bottom > y+TEXTHIGH/2+tmpSep && theRg.Area.top < y-TEXTHIGH/2-12-tmpSep)
					{	
						if( Oldy<COMPPREC || fabs(Oldy-y)>8)
						{
							Oldy = y;
							//在自动增长的刻度中间是否增加小刻度
							if ( m_nMainAxisStatus==R100_AXIS && Oldy>0 )
							{
								DispInfo1 = int((y - Oldy)/4.0+0.5);
								DispInfo2 = int((y - Oldy)/2.0+0.5);
								if (  DispInfo3 == 0 )	//如果画中轴,就都画
									DispInfo3 = abs(y - Oldy);
								if (  DispInfo3 > 3*TEXTHIGH )
								{
									double tmpVal = 100*((dOldFrom+dLastFrom)/2-tmpClose)/tmpClose;
									if ( m_bR100Dot )
										g_d.DisplayText(pMDC,0,Oldy+DispInfo2-m_YDy,VipColor.TIMECOLOR,FXT_RWIDTH-2,"%4.1f%%",tmpVal);
									else
									{	
										if(tmpVal>=0) yVal = long(tmpVal +0.5);
										else yVal = long(tmpVal -0.5);
										g_d.DisplayText(pMDC,0,Oldy+DispInfo2-m_YDy,VipColor.TIMECOLOR,FXT_RWIDTH-2,"%5d%%",yVal);
									}
								}
								//等分刻度线
								if(m_nStyle != FXT_MULTI) 
								{
									g_d.DrawLine(pMDC,1,int(Oldy+(y - Oldy)/2.0+0.5),4,int(Oldy+(y - Oldy)/2.0+0.5));
									g_d.DrawLine(pMDC,1,Oldy+DispInfo1,2,Oldy+DispInfo1);
									g_d.DrawLine(pMDC,1,y-DispInfo1,2,y-DispInfo1);
								}
							}
							if(m_nStyle != FXT_MULTI) 
								g_d.DrawLine(pMDC,1,y,6,y);	//为自动增长的坐标刻度画
							if ( g_bHasYHidden && y - theRg.DrawRc.top>0)
							{
								theRg.nDotInfo[theRg.nDotNum] = y - theRg.DrawRc.top;
								++ theRg.nDotNum;
							}
						}
					}
					dOldFrom=dLastFrom;
					if(m_nMainAxisStatus==SAMESCALE_AXIS || m_nMainAxisStatus==LOG10_AXIS)
					{
						if(iDirection==0)
							dLastFrom+=dLastFrom*tmpRatio;
						else
							dLastFrom-=dLastFrom*tmpRatio;
					}
					else if(m_nMainAxisStatus==GOLDEN_AXIS)
					{
						if(tmpCnt>=7) break;
						dLastFrom=tmpClose+GoldLineRatio[tmpCnt]*0.01*(theRg.dMaxVal-theRg.dMinVal)-0.0002; //要减一点,保证最大值可以画出来
					}
					else
					{
						if(iDirection==0)
							dLastFrom+=tmpStep;
						else if(iDirection==1) 
							dLastFrom-=tmpStep;
					}		
					tmpCnt++;
					tmpLastFrom=dLastFrom;
					if(m_nMainAxisStatus==LOG10_AXIS)
						tmpLastFrom=log10(max(COMPPREC,dLastFrom));
				}
				while((iDirection==0 && theRg.dMaxVal-tmpLastFrom>COMPPREC) || (iDirection==1 && tmpLastFrom-theRg.dMinVal>COMPPREC && dLastFrom-tmpClose*0.01>COMPPREC));
			}
		}
		else													 //如果是普通坐标
		{
			double dOldFrom,dLastFrom = max(COMPPREC,theRg.RulerInfo.dFrom);
			int Oldy = 0,Lasty=0;
			int DispInfo1 = 0;
			int DispInfo2 = 0;
			int DispInfo3 = 0;
			for ( j=0 ;; j++ )
			{
				dOldFrom=dLastFrom;
				dLastFrom=theRg.RulerInfo.dFrom+j*theRg.RulerInfo.dStep;
				y = GetYPosOfRegion(nRegion,dLastFrom);
				if ( theRg.Area.bottom > y+TEXTHIGH/2 && theRg.Area.top < y-TEXTHIGH/2-6
					&& (Lasty<COMPPREC || Lasty-y>6) ) //Y轴象素相隔足够多才写文字
				{	
					Lasty=y;
					if ( theRg.RulerInfo.nZoom )
					{
						if(theRg.DrawRc.bottom > y+m_YDy+TEXTHIGH/2)
						{
							//经过放大的刻度
							if(dLastFrom>=0)	yVal = long(dLastFrom/theRg.RulerInfo.nZoom+0.5);
							else yVal = long(dLastFrom/theRg.RulerInfo.nZoom-0.5);
							g_d.DisplayText(pMDC,0,y-m_YDy,VipColor.TIMECOLOR,FXT_RWIDTH-2,"%5ld",yVal);
						}
					}
					else
					{	//一般刻度
						if(abs(dLastFrom)>=1000.0)
							g_d.DisplayText(pMDC,0,y-m_YDy,VipColor.TIMECOLOR,FXT_RWIDTH-2,"%5.0f",dLastFrom);
						else if(abs(dLastFrom)>=100.0)
							g_d.DisplayText(pMDC,0,y-m_YDy,VipColor.TIMECOLOR,FXT_RWIDTH-2,"%5.1f",dLastFrom);
						else if(m_XsFlag==3 && abs(dLastFrom)>=10.0)
							g_d.DisplayText(pMDC,0,y-m_YDy,VipColor.TIMECOLOR,FXT_RWIDTH-2,"%5.2f",dLastFrom);
						else
							g_d.DisplayText(pMDC,0,y-m_YDy,VipColor.TIMECOLOR,FXT_RWIDTH-2,(m_XsFlag==3)?"%5.3f":"%5.2f",dLastFrom);
					}
				}
				if ( theRg.Area.bottom > y+TEXTHIGH/2 && theRg.Area.top < y-TEXTHIGH/2-6)
				{
					if ( Oldy>0 )
					{	
						DispInfo1 = int((y - Oldy)/4.0+0.5);
						DispInfo2 = int((y - Oldy)/2.0+0.5);
						if (  DispInfo3 == 0 )	//如果画中轴,就都画
							DispInfo3 = abs(y - Oldy);
						if (  DispInfo3 > 3*TEXTHIGH )
						{
							if ( theRg.RulerInfo.nZoom )
							{	
								if(theRg.DrawRc.bottom > y)
								{
									if(dOldFrom+dLastFrom>=0) yVal = long(((dOldFrom+dLastFrom)/2)/theRg.RulerInfo.nZoom+0.5);
									else yVal = long(((dOldFrom+dLastFrom)/2)/theRg.RulerInfo.nZoom-0.5);
									g_d.DisplayText(pMDC,0,Oldy+DispInfo2-m_YDy,VipColor.TIMECOLOR,FXT_RWIDTH-2,"%5ld",yVal);
								}
							}
							else
							{	
								if(abs(dOldFrom+dLastFrom)/2>=1000.0)
								{
									g_d.DisplayText(pMDC,0,Oldy+DispInfo2-m_YDy,VipColor.TIMECOLOR,FXT_RWIDTH-2,
										"%5.0f",(dOldFrom+dLastFrom)/2);
								}
								else if(m_XsFlag==3 && abs(dOldFrom+dLastFrom)/2>=10.0)
								{
									g_d.DisplayText(pMDC,0,Oldy+DispInfo2-m_YDy,VipColor.TIMECOLOR,FXT_RWIDTH-2,
										"%5.2f",(dOldFrom+dLastFrom)/2);
								}
								else if(abs(dOldFrom+dLastFrom)/2>=100.0)
								{
									g_d.DisplayText(pMDC,0,Oldy+DispInfo2-m_YDy,VipColor.TIMECOLOR,FXT_RWIDTH-2,
										"%5.1f",(dOldFrom+dLastFrom)/2);
								}
								else
								{
									g_d.DisplayText(pMDC,0,Oldy+DispInfo2-m_YDy,VipColor.TIMECOLOR,FXT_RWIDTH-2,
										(m_XsFlag==3)?"%5.3f":"%5.2f",(dOldFrom+dLastFrom)/2);
								}
							}
						}
						//等分刻度线
						if(m_nStyle != FXT_MULTI) 
						{
							g_d.DrawLine(pMDC,1,int(Oldy+(y - Oldy)/2.0+0.5),4,int(Oldy+(y - Oldy)/2.0+0.5));
							g_d.DrawLine(pMDC,1,Oldy+DispInfo1,2,Oldy+DispInfo1);
							g_d.DrawLine(pMDC,1,y-DispInfo1,2,y-DispInfo1);
						}
					}
					if(m_nStyle != FXT_MULTI) 
						g_d.DrawLine(pMDC,1,y,6,y);	//为自动增长的坐标刻度画
					if ( g_bHasYHidden  && y-theRg.DrawRc.top>0)
					{
						theRg.nDotInfo[theRg.nDotNum] = y - theRg.DrawRc.top;
						++ theRg.nDotNum;
					}
				}
				Oldy = y;
				//
				if(j*theRg.RulerInfo.dStep > theRg.RulerInfo.dDelta)
					break;
			}		
		}
		if ( theRg.RulerInfo.nZoom )
		{	char Info[100];
			if ( theRg.RulerInfo.nZoom == 1000000 )	sprintf(Info,_F("×100万"));
			else if ( theRg.RulerInfo.nZoom == 100000 )	sprintf(Info,_F("×10万"));
			else if ( theRg.RulerInfo.nZoom == 10000 )	sprintf(Info,_F("×1万"));
			else									sprintf(Info,"×%d",theRg.RulerInfo.nZoom);
			size = pMDC->GetTextExtent(Info);
			g_d.Bar(pMDC,1,theRg.DrawRc.bottom-TEXTHIGH-1,size.cx+4,theRg.DrawRc.bottom-2,BACK_BRUSH);
			g_d.DisplayText(pMDC,3,theRg.DrawRc.bottom-TEXTHIGH-1,VipColor.TIMECOLOR,0,"%s",Info);
			g_d.DrawRect(pMDC,1,theRg.DrawRc.bottom-TEXTHIGH-1,size.cx+4,theRg.DrawRc.bottom-2);
		}
	}
	/////////////////////////////////
	double dVal = 0;
	CRect  tmpRc = CRect( &theRg.DrawRc);
	if ( tmpRc.PtInRect(CPoint(nX,nY)) )
	{	
		char  Content[100];
		CRect  SwinRc(0,nY-16,FXT_RWIDTH,nY);
		dVal = GetValFromY(nRegion,nY);
		m_fYVal = dVal;
		if ( m_bXS3)
			sprintf(Content,"%6.3f",dVal);
		else if(m_pStkInfo->Close<900)
			sprintf(Content,"%6.2f",dVal);
		else
			sprintf(Content,"%6.1f",dVal);
		g_CbInfo.m_fNowVal  = GetValFromY(0,nY);
		if ( theRg.RulerInfo.nZoom )
		{	long uVal = (long)(dVal/(float)theRg.RulerInfo.nZoom);
			sprintf(Content,"%d",uVal);
		}
		g_d.DrawSwim(pMDC,SwinRc,Content);
	}
}

//传入坐标为相对坐标 PDC为实际DC
void UFxt::DrawYRule(CDC * pDC,int nX,int nY)
{
	if(m_rcIn.bottom<=FXT_BHEIGHT) 
		return;
	CRect	YRc(0,0,FXT_RWIDTH,m_rcIn.bottom-FXT_BHEIGHT);
	if(m_nStyle == FXT_MULTI) 
		YRc.SetRect(0,0,FXT_RWIDTH,m_rcIn.bottom);
	CREATE_MEMDC(YRc)
	//
	CSize size = pDC->GetTextExtent("Test");
	if ( m_YDy == 0 ) 
		m_YDy = int(size.cy/2+0.5);	//防止抖动
	//
	CPen  *oldpen = g_d.SelectPen(&MemDC,AXIS_PEN);
	for (int i=0,j=0;i<m_nRegion;i++ )
	{	
		if(m_nStyle != FXT_MULTI)	//画短的框
			g_d.DrawLine(&MemDC,0,m_aRegion[i].Area.bottom,YRc.right,m_aRegion[i].Area.bottom); 
		if ( m_nDataNum<=0 )
			continue;
		memset(m_aRegion[i].nDotInfo,0,MAXDOTLINE*sizeof(WORD));
		m_aRegion[i].nDotNum = 0;
		if ( !m_aRegion[i].RulerInfo.bValid ) continue;
		InnerDrawY(pDC,&MemDC,i,nX,nY);
	}
	if(m_nStyle == FXT_MULTI) 
		g_d.DrawLine(&MemDC,0,0,YRc.right,0);
	g_d.DrawLine(&MemDC,0,0,0,m_rcIn.bottom);
	if ( m_nStyle != FXT_MULTI )
	{
		if ( m_nSelRegion<=0 ) m_nSelRegion = m_nRegion-1;
		CRect  FixRc(1,m_aRegion[m_nSelRegion].Area.top+1,FXT_RWIDTH-1,m_aRegion[m_nSelRegion].Area.bottom-1);
		g_d.DrawRect(&MemDC,FixRc);
	}
	pDC->BitBlt(right-FXT_RWIDTH,top,YRc.Width(),YRc.Height(),&MemDC,0,0,SRCCOPY);
	g_d.RestorePen(&MemDC,oldpen);
	DELETE_MEMDC
}

void UFxt::DrawXGrid(CDC * pDC)
{
	int j,Oldx = 0,OldOne = 0,IgnoreNum = 0,IgnoreCnt=0;
	if ( !g_bHasXHidden && m_nPeriod!=PER_MIN1 && m_nPeriod!=PER_MIN5)
		return;
	struct AnalyData * p = m_pObject->pHisDay+m_nLeftNo;
	m_nXAxis = 0;
	memset(m_pXRule,0,m_nDrawNum*sizeof(long));

	g_nFixedXAxis = max(g_nFixedXAxis, 5);
	int nPos = 1;
	if(g_bFixedXAxis&&m_nDrawNum>g_nFixedXAxis)
	{
		for ( j=m_nDrawNum-2;j>=0;j--,nPos++ )
		{
			if ( nPos%g_nFixedXAxis==0 )
			{
				if ( m_pPosOfX[j]-Oldx > MIN_PIXLE )
					m_pXRule[m_nXAxis++] = 	m_pPosOfX[j];
			}
		}
		return;
	}

	switch ( m_nPeriod )
	{
	case PER_DAY:
	case PER_WEEK:
		if(m_nPeriod==PER_WEEK)
			IgnoreNum=2;
		else
			IgnoreNum=1;
		IgnoreCnt=0;
		for ( j=0;j<m_nDrawNum;j++ )
		{
			if ( (p[j].Time.Date/100)%100 != OldOne )
			{
				if(++IgnoreCnt==IgnoreNum)
				{
					if ( m_pPosOfX[j]-Oldx > MIN_PIXLE )
					{
						m_pXRule[m_nXAxis++] = 	m_pPosOfX[j];
						Oldx = m_pPosOfX[j];
					}
					IgnoreCnt=0;
				}
				OldOne = (p[j].Time.Date/100)%100;
			}
		}
		break;
	case PER_MONTH:
	case PER_SEASON:
	case PER_YEAR:
	case PER_DAYN:
		for ( j=0;j<m_nDrawNum;j++ )
		{
			if (p[j].Time.Date/10000 != OldOne )
			{
				if ( m_pPosOfX[j]-Oldx > MIN_PIXLE )
				{
					m_pXRule[m_nXAxis++] = 	m_pPosOfX[j];
					Oldx = m_pPosOfX[j];
				}
				OldOne = p[j].Time.Date/10000;
			}
		}
		break;
	case PER_MIN1:
	case PER_MINN:
	case PER_MIN5:
	case PER_MIN15:
	case PER_MIN30:
	case PER_HOUR:
		for ( j=0;j<m_nDrawNum;j++ )
		{
			if (p[j].Time.Daye.Day != OldOne )
			{
				if ( m_pPosOfX[j]-Oldx > MIN_PIXLE )
				{
					m_pXRule[m_nXAxis++] = 	m_pPosOfX[j];
					Oldx = m_pPosOfX[j];
				}
				OldOne = p[j].Time.Daye.Day;
			}
		}		
		break;
	}
}

void UFxt::CalcOneAxis(int i)
{
	int j=0, nZoom = 0 ;
	double dDelta,dStep = 0,dBase = 1.0,dExt = 0.5;
	m_YDy = 0;
	//如果无效
	memset(&(m_aRegion[i].RulerInfo),0,sizeof(RULERINFO));
	if(fabs(m_aRegion[i].dMaxValEx-DMAXVAL)<1 || fabs(m_aRegion[i].dMinValEx-DMINVAL)<1 || m_aRegion[i].Area.bottom-m_aRegion[i].Area.top<1)
		return;
	//是否加入比例因子
	nZoom =  (int)(fabs(m_aRegion[i].dMaxValEx)/OVER_VAL);
	nZoom =  max(nZoom,(int)(fabs(m_aRegion[i].dMinValEx)/OVER_VAL));
	if ( nZoom>=10 )		
	{
		nZoom = nZoom/1000000 ? 1000000 : nZoom/100000 ? 100000 : nZoom/10000 ? 10000 : nZoom/1000 ? 1000 : nZoom/100 ? 100 : 10;
		m_aRegion[i].RulerInfo.nZoom = nZoom;
	}
	else						
		m_aRegion[i].RulerInfo.nZoom = 0;
	//确定刻度间距
	dDelta = max(m_aRegion[i].dMaxValEx,GetValFromY(i,m_aRegion[i].DrawRc.top)) - min(m_aRegion[i].dMinValEx,GetValFromY(i,m_aRegion[i].DrawRc.bottom));
	if ( dDelta<COMPPREC )
		return;
	m_aRegion[i].RulerInfo.bValid = 1;
	if ( dDelta > 10.00 )
	{
		for ( dBase = 1;dDelta/(dBase*10) > 10.0; )
			dBase *= 10;
	}
	else 
	{	
		for ( dBase = 1;dDelta/dBase < 10.00; )
			dBase /= 10;
		dExt *= dBase;
	}
	dStep = 2*dBase;	
	for ( j=1;dStep*(m_aRegion[i].Area.bottom-m_aRegion[i].Area.top)/dDelta < MIN_PIXLE ;j++ ) 	
		dStep = 5*j*dBase;	
	double dFrom = (int((m_aRegion[i].dMinValEx+dExt)/dStep))*dStep;
	if(dFrom<m_aRegion[i].dMinValEx || dFrom<COMPPREC) 
		dFrom += dStep;
	//
	m_aRegion[i].RulerInfo.dDelta = dDelta;	
	m_aRegion[i].RulerInfo.dFrom = dFrom;		
	m_aRegion[i].RulerInfo.dStep = dStep;
}

void UFxt::CalcRuleDrawAxis()
{
	m_YDy = 0;
	for (int i=0;i<m_nRegion;i++ )
		CalcOneAxis(i);
}

void UFxt::ReadSomeStk(TOBJECT * p)
{
	if ( m_nDataNum <= 0 )
		return;
	MemStkInfo * scode = (*g_pStockIO)[p->nCurGP];
	if ( !scode )		
		return;
	//如果股票相同
	if ( stricmp(scode->Code,m_pStkInfo->Code) == 0 && scode->setcode == m_pStkInfo->setcode)
	{
		for ( int i=0;i<m_nDataNum;++i )
			p->pHisDay[i] =	l_InValidHisDat;
		return;
	}
	DuiQiStk(p);
}

void UFxt::DuiQiStk(TOBJECT * p)
{
	int	i,j,nRet;
	//直接读取数据
	MemStkInfo * scode = (*g_pStockIO)[p->nCurGP];
 	long TotalNum = g_pStockIO->GetAnalyDataNum(scode,m_nPeriod,GetMulNum(m_nPeriod));
	AnalyData * ptmp = new AnalyData[TotalNum+1];	//防止PT的叠加缺少股票
	memset(ptmp,0,sizeof(AnalyData)*(TotalNum+1));
	for ( i=0;i<m_nDataNum;++i )
		p->pHisDay[i] =	l_InValidHisDat;
	CurrStockData Hq;
	nRet = g_pStockIO->ReadAnalyDataAndCurrHQ(scode,ptmp,TotalNum,m_nPeriod,&Hq,m_nTodayNum,GetMulNum(m_nPeriod));
	if ( g_nTQFlag != 0 )
		g_pStockIO->TQData(scode, ptmp,0,nRet,nRet,g_nTQFlag,m_nPeriod);
	
	for ( i=m_nDataNum-1;i>=0;--i )
	{
		for (j=nRet-1;j>=0;--j )
		{
			if ( m_pObject->pHisDay[i].Time.Date == ptmp[j].Time.Date )
			{
				p->pHisDay[i] = ptmp[j];
				break;
			}
		}
	}
	delete []ptmp;
	//最多不会超过m_nDataNum
	p->nArrayLen = min(nRet,m_nDataNum);
}

//各种情况的读取数据
BOOL UFxt::ReadData(BOOL bDataReady)
{
	AutoOverlapGP(FALSE);
	//还原
	TransAnalyData(0);

	int i=0,j=0,k=0; 
	short nDataNum = 0, dataflag=1000;
	AnalyData	 * pBack = NULL;
	//将上次移动筹码干掉
	m_pView->SendMessage(UM_REFLECT,Reflect_Cb_FxtEnd);
	if(!OnLine || (OnLine && !m_bDataReady) || UseLocaldata())
	{
		//!!!
	//	if(m_bLcIdx && m_pStkInfo && g_pLcIdxMng)
	//		g_pLcIdxMng->CheckReCalc(m_pView,m_pStkInfo->Code);
		//请求时,说明是第一次 以便将一些变量复位
		m_nDataNum = 0;
		m_nDrawNum = 0;
		m_nLeftNo  = 0;
		m_nLastNo  = 0;
		//
		m_bIsHead  = 0;
		m_nFilter  = 0;
		m_bXS3 = GetXSFlag(m_pStkInfo) == XS3;
		m_bKeepOver = FALSE;
		m_KLock = -1;  //不要锁定了
	}
	if(m_bIsHead) 
		return FALSE;
	/////////////////////////
	if(OnLine && !UseLocaldata())
	{
		if(!m_bIsHead)
			nDataNum = m_nDataNum+RECENTNUM;
	}
	else
		nDataNum = g_pStockIO->GetAnalyDataNum(m_pStkInfo,m_nPeriod,GetMulNum(m_nPeriod));
	///////////////////////////////
	if(bDataReady) 
	{
		if(OnLine && !UseLocaldata())
		{
			//开始提取数据
			//暂时转移数据,腾出空间　空间已经申请好
			memmove(m_pOrignData+RECENTNUM,m_pOrignData,m_nDataNum*sizeof(struct AnalyData));
			//换成原始数据　除权过的数据，不能再除权
			memmove(m_pObject->pHisDay+RECENTNUM,m_pObject->pHisDay,m_nDataNum*sizeof(struct AnalyData));
			////////////////////
			if(m_bDSFlag)
				nDataNum = g_pGetData->DSGetHisDataACK(m_pStkInfo,m_pObject->pHisDay,m_nPeriod,m_nDataNum);
			else
				nDataNum = g_pGetData->GetAnalyDataACK(m_pStkInfo,m_pObject->pHisDay,m_nPeriod,m_nDataNum);
			if ( nDataNum < RECENTNUM )
			{	//如果没有足够数据，再次转移 此时还没有改变m_nDataNum 现在都是干净的未除权数据
				memmove(m_pObject->pHisDay+nDataNum,m_pOrignData+RECENTNUM,m_nDataNum*sizeof(struct AnalyData));
				memmove(m_pOrignData+nDataNum,m_pOrignData+RECENTNUM,m_nDataNum*sizeof(struct AnalyData));
			//	m_bIsHead = TRUE;
			}
			//修改m_nDataNum 
			m_nDataNum = nDataNum+m_nDataNum;
			memcpy(m_pOrignData,m_pObject->pHisDay,nDataNum*sizeof(AnalyData));
			//过滤TCP包里面为零的行情数据
			while ( m_nDataNum>0  &&  m_pObject->pHisDay[m_nDataNum-1].Open+m_pObject->pHisDay[m_nDataNum-1].High+m_pObject->pHisDay[m_nDataNum-1].Close+m_pObject->pHisDay[m_nDataNum-1].Low < COMPPREC )
			{
				--m_nDataNum;
				++m_nFilter;
			}
			m_bReqing = FALSE;
		}
		else
		{
			TDEL(m_pOrignData);
			RejustAllObject(nDataNum+600);	//预留1分钟线的空间
			m_pOrignData = new AnalyData[nDataNum+600];
			if ( m_pPoint )
				delete []m_pPoint, m_pPoint = NULL;
			m_pPoint = new CPoint[nDataNum+600];
			memset(m_pPoint,0,(nDataNum+600)*sizeof(CPoint));
			//按照nDataNum的个数来请求数据
			nDataNum  = g_pStockIO->ReadAnalyDataAndCurrHQ(m_pStkInfo,m_pObject->pHisDay,nDataNum,m_nPeriod,&m_Hq,m_nTodayNum,GetMulNum(m_nPeriod),dataflag);
			//备份原始数据
			memcpy(m_pOrignData,m_pObject->pHisDay,(nDataNum+600)*sizeof(AnalyData));
			m_bDataReady = TRUE;
			m_nDataNum = nDataNum;	//修改m_nDataNum
		}
	}
	else if(OnLine)
	{
		BOOL bGoBack = 0;
		m_bReqing = TRUE;  //在请求数据中，不能再请求同种数据
		m_nToLeft = m_nToRight = 0;
		///在真正得到数据之前申请好空间
		//备份原始数据
		pBack = m_pOrignData;
		m_pOrignData = NULL;
		m_pOrignData = new AnalyData[nDataNum+600];
		memset(m_pOrignData,0,(nDataNum+600)*sizeof(AnalyData));
		/////////////////////////////
		RejustAllObject(nDataNum+600);	//预留1分钟线的空间
		if ( m_pPoint )
			delete []m_pPoint , m_pPoint = NULL;
		m_pPoint = new CPoint[nDataNum+600];
		memset(m_pPoint,0,(nDataNum+600)*sizeof(CPoint));
		///////////////////////////////
		if ( pBack )
		{	//暂时转移数据,腾出空间
			memmove(m_pOrignData+RECENTNUM,pBack,m_nDataNum*sizeof(struct AnalyData));
			//换成原始数据 除权过的数据，不能再除权
			memmove(m_pObject->pHisDay+RECENTNUM,pBack,m_nDataNum*sizeof(struct AnalyData));
			TDEL(pBack);
			bGoBack = 1;
		}
		//
		m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
		//如果是多股同列，先读行情
		if(m_nStyle == FXT_MULTI)
		{
			if(m_bDSFlag)
			{
				g_nAssisID = SetComAssisID(m_nWhichUnit,100+5);
				g_pGetData->DSGetCurrData(m_pStkInfo);
			}
			else
			{
				g_nAssisID = SetComAssisID(m_nWhichUnit,100+3);
				g_pGetData->GetCurrStockData(m_pStkInfo);
			}
		}
		//先读具体分析数据
		if(m_bDSFlag)
		{
			g_nAssisID = SetComAssisID(m_nWhichUnit,100+6+(m_nDataNum!=0));
			nDataNum = g_pGetData->DSGetHisData(m_pStkInfo,
				m_nPeriod,GetMulNum(m_nPeriod),m_nDataNum+m_nFilter,RECENTNUM,m_pObject->pHisDay);
		}
		else
		{
			g_nAssisID = SetComAssisID(m_nWhichUnit,100+(m_nDataNum!=0));
			nDataNum = g_pGetData->GetAnalyData(m_pStkInfo,m_nDataNum+m_nFilter,RECENTNUM,m_nPeriod,GetMulNum(m_nPeriod),m_pObject->pHisDay);
		}
		if ( nDataNum > 0 )
		{
			if ( nDataNum < RECENTNUM )
			{	//如果没有足够数据，再次转移 此时还没有改变m_nDataNum
				memmove(m_pObject->pHisDay+nDataNum,
					   m_pOrignData+RECENTNUM,m_nDataNum*sizeof(struct AnalyData));
				memmove(m_pOrignData+nDataNum,
					   m_pOrignData+RECENTNUM,m_nDataNum*sizeof(struct AnalyData));
				/*
				m_bIsHead = TRUE;
				if(m_nPeriod==PER_DAY)
				{
					long VipDocLastDate=GetVipDocLastDate();
					long nLocalNum = g_pStockIO->GetAnalyDataNum(m_pStkInfo, PER_DAY);
					if((VipDocLastDate==g_lOpenRq&&nLocalNum==nDataNum+1)||(GetForwardDate(VipDocLastDate)==g_lOpenRq&&nLocalNum==nDataNum))
					{
						m_bIsHead = FALSE;
						g_nAssisID = SetComAssisID(m_nWhichUnit,100+9);
						g_pGetData->GetCurrStockData(m_pStkInfo);
					}
				}
				*/
			}
			//表明已经有数据了
			m_bDataReady = TRUE;
			m_nDataNum = nDataNum+m_nDataNum;
			/////////////////////////
			if(m_nWillNum)
				m_nWillNum = m_nDataNum+3;
			else
				m_nWillNum  = m_nSHOWNUM+2;
			memcpy(m_pOrignData,m_pObject->pHisDay,nDataNum*sizeof(AnalyData));
			//过滤TCP包里面为零的行情数据
			while ( m_nDataNum>0  && m_pObject->pHisDay[m_nDataNum-1].Open+m_pObject->pHisDay[m_nDataNum-1].High
				+m_pObject->pHisDay[m_nDataNum-1].Close+m_pObject->pHisDay[m_nDataNum-1].Low < COMPPREC )
			{
				--m_nDataNum;
				++m_nFilter;
			}
			m_bReqing = FALSE;
		}
		else
		{ 
			if ( bGoBack )
			{
				memmove(m_pObject->pHisDay,m_pOrignData+RECENTNUM,m_nDataNum*sizeof(struct AnalyData));
				memmove(m_pOrignData,m_pOrignData+RECENTNUM,m_nDataNum*sizeof(struct AnalyData));
			}
			if(nDataNum == -1 )
			{	//因为删除过，因此要参与计算,不能return掉
				m_bIsHead = TRUE;
			}
		}
	}
	if(!OnLine || UseLocaldata()) //对脱机状态,干掉不合理的K线
	{
		while(m_nDataNum>0  && m_pObject->pHisDay[m_nDataNum-1].Open+m_pObject->pHisDay[m_nDataNum-1].High
			+m_pObject->pHisDay[m_nDataNum-1].Close+m_pObject->pHisDay[m_nDataNum-1].Low < COMPPREC)
		{
			--m_nDataNum;
		}
	}
	m_nLastNo = max(0,m_nDataNum-1);
	//////////////////////////////////////////////////////////////////////////
	//除权移到这里				iwld 20050818
	if(m_pHisData)	delete []m_pHisData, m_pHisData = NULL;
	m_pHisData = new HISDAT[m_nDataNum+600];
	memset(m_pHisData,0,m_nDataNum*sizeof(HISDAT));
	if ( g_nTQFlag != 0  )
	{	//注意原始数据
		memcpy(m_pObject->pHisDay,m_pOrignData,m_nDataNum*sizeof(AnalyData));
		m_pAnalyData = m_pObject->pHisDay;
		g_pStockIO->TQData(m_pStkInfo, m_pAnalyData,0,m_nLastNo,m_nDataNum,g_nTQFlag,m_nPeriod);
		//数据画法类型转换
		TransAnalyData(1);
		RawData2CalcData(m_pStkInfo->Code,m_pStkInfo->setcode,m_nPeriod,m_pAnalyData,m_pHisData,m_nDataNum,g_NoTime);
	}
	else
	{
		//数据画法类型转换
		TransAnalyData(1);
		RawData2CalcData(m_pStkInfo->Code,m_pStkInfo->setcode,m_nPeriod,m_pObject->pHisDay,m_pHisData,m_nDataNum,g_NoTime);
	}	
	//基于假设：底层读到的数据是，到目前为止已经维护好的数据
	m_nTodayNum = GetMinuteXh(m_nPeriod,TimeSec,g_nMinnNum);
	if(m_bDSFlag) 
	{
		m_nTodayNum = GetDSMinute(m_pTimer,FALSE);
		int tmpRatio = anFZRatio[m_nPeriod];
		if(m_nPeriod == PER_MINN)	tmpRatio = g_nMinnNum;
		m_nTodayNum =  ( m_nTodayNum + tmpRatio - 1 )/( tmpRatio );
	}
	//不会大于500
	m_nCwNum = g_pStockIO->GetTQInfo(m_pStkInfo,CWData,MAX_CWDATA,TRUE);
	for(i=0;i<m_aRegion[0].nObject;++i)
	{
		if ( m_aRegion[0].aIndexInfo[i]->nSet == GBZS_OBJECT )
		{
			int nBase = 0;
			if(m_nPeriod==PER_MIN1||m_nPeriod==PER_MIN5||m_nPeriod==PER_MIN15||m_nPeriod==PER_MIN30||m_nPeriod==PER_MINN||m_nPeriod==PER_HOUR)
			{
				CTime t = CTime::GetCurrentTime();
				long lEndDate = 10000*t.GetYear()+100*t.GetMonth()+t.GetDay();				
				//今年的
				int nFrom_1 = 0, nEnd_1 = m_nDataNum;
				for(k=nEnd_1-1;k>nFrom_1;k--)
				{
					if(m_pObject->pHisDay[k].Time.Daye.Day<m_pObject->pHisDay[k-1].Time.Daye.Day)
					{
						nFrom_1 = k;
						break;
					}
				}
				for(j = m_nCwNum-1 ;j>=nBase; -- j)
				{
					if(lEndDate-CWData[j].Date>1130) break;
					for( k=nFrom_1;k<nEnd_1;++k)
					{
						if(m_pObject->pHisDay[k].Time.Daye.Day==CWData[j].Date%10000&&t.GetYear()==CWData[j].Date/10000)
						{
							m_aRegion[0].aIndexInfo[i]->pCalcData[0][k] = 1;
							m_aRegion[0].aIndexInfo[i]->pCalcData[1][k] = j;	//记下位置
							nBase = j ;
							break;
						}
						else
							m_aRegion[0].aIndexInfo[i]->pCalcData[0][k] = 0;
					}
				}
				//去年的
				int nFrom_2 = 0, nEnd_2 = nFrom_1;
				for(k=nEnd_2-1;k>nFrom_2;k--)
				{
					if(m_pObject->pHisDay[k].Time.Daye.Day<m_pObject->pHisDay[k-1].Time.Daye.Day)
					{
						nFrom_2 = k;
						break;
					}
				}
				for(j = m_nCwNum-1 ;j>=nBase; -- j)
				{
					if(lEndDate-CWData[j].Date>11130||lEndDate-CWData[j].Date<=1130) break;
					for( k=nFrom_2;k<nEnd_2;++k)
					{
						if(m_pObject->pHisDay[k].Time.Daye.Day==CWData[j].Date%10000&&t.GetYear()-1==CWData[j].Date/10000)
						{
							m_aRegion[0].aIndexInfo[i]->pCalcData[0][k] = 1;
							m_aRegion[0].aIndexInfo[i]->pCalcData[1][k] = j;	//记下位置
							nBase = j ;
							break;
						}
						else
							m_aRegion[0].aIndexInfo[i]->pCalcData[0][k] = 0;
					}
				}
			}
			else
			{
				for ( k=0;k<m_nDataNum; ++ k )
				{
					for (j = nBase ;j<m_nCwNum; ++ j )
					{
						if ((k>0 && m_pObject->pHisDay[k-1].Time.Date < CWData[j].Date && m_pObject->pHisDay[k].Time.Date >= CWData[j].Date) ||
							(k==0 && m_pObject->pHisDay[k].Time.Date == CWData[j].Date))
						{
							m_aRegion[0].aIndexInfo[i]->pCalcData[0][k] = 1;
							m_aRegion[0].aIndexInfo[i]->pCalcData[1][k] = j;	//记下位置
							nBase = j ;
							break;
						}
						else
							m_aRegion[0].aIndexInfo[i]->pCalcData[0][k] = 0;
					}
				}
			}
		}
		////////////////////////////////////////变名
		m_nFCNum = g_pStockIO->GetFaceData(m_pStkInfo, FCData, MAX_FCDATA);
		if ( m_nFCNum >0 && m_aRegion[0].aIndexInfo[i]->nSet == FC_OBJECT )
		{
			int nBase = 0;
			if(m_nPeriod==PER_MIN1||m_nPeriod==PER_MIN5||m_nPeriod==PER_MIN15||m_nPeriod==PER_MIN30||m_nPeriod==PER_MINN||m_nPeriod==PER_HOUR)
			{
				CTime t = CTime::GetCurrentTime();
				long lEndDate = 10000*t.GetYear()+100*t.GetMonth()+t.GetDay();
				
				for(j = m_nFCNum-1 ;j>=nBase; -- j)
				{
					if(lEndDate-FCData[j].Date>1208) break;
					for( k=0;k<m_nDataNum;++k)
					{
						if(m_pObject->pHisDay[k].Time.Daye.Day==FCData[j].Date%10000)
						{
							m_aRegion[0].aIndexInfo[i]->pCalcData[0][k] = 1;
							m_aRegion[0].aIndexInfo[i]->pCalcData[1][k] = j;	//记下位置
							nBase = j ;
							break;
						}
						else
							m_aRegion[0].aIndexInfo[i]->pCalcData[0][k] = 0;
					}
				}
			}
			else
			{
				for ( k=0;k<m_nDataNum; ++ k )
				{
					for (j = nBase ;j<m_nFCNum; ++ j )
					{
						if ((k>0 && m_pObject->pHisDay[k-1].Time.Date < FCData[j].Date && m_pObject->pHisDay[k].Time.Date >= FCData[j].Date) ||
							(k==0 && m_pObject->pHisDay[k].Time.Date == FCData[j].Date))
						{
							m_aRegion[0].aIndexInfo[i]->pCalcData[0][k] = 1;
							m_aRegion[0].aIndexInfo[i]->pCalcData[1][k] = j;	//记下位置
							nBase = j ;
							break;
						}
						else
							m_aRegion[0].aIndexInfo[i]->pCalcData[0][k] = 0;
					}
				}
			}
		}
	}
	//
	m_bChgRatio  = TRUE;		//画图的比例、宽度等要改变
	FillDlgInfo(m_nLastNo);
	PopupInfoDlg();
	//m_nKeyNo是为将来KeyCross设计，必须考虑定位 它可以防碍右边可以显示的数目
	if(m_nKeyNo>m_nDataNum-1)
		m_nKeyNo = max(0,m_nDataNum-1);
	else if ( m_nKeyNo<m_nLastNo-m_nDrawNum )
		m_nKeyNo = max(0,m_nDataNum-1);
	/////////////////////
	m_pObject->nArrayLen = m_nDataNum;
	////////////////////////////////////////////////////////////
	TOBJECT * p = m_pObject->next;
	while(p)
	{	
		if ( p->nSet == HIS_DAT )
			ReadSomeStk(p);
		p = p->next;
	}
	CalcAllGS();
	m_bChgRatio = TRUE;

	//恢复指示
	if(m_ResetZSFlag==0)
	{
		GetSelectMany();
		m_ResetZSFlag = 1;
	}

	if(!OnLine || UseLocaldata())
		ProcessMsg(WM_KEYDOWN,VK_ESCAPE,0);
	return true;
}
///////////////////////////////////////////////////
//nTransFlag==0,还原
void UFxt::TransAnalyData(BYTE nTransFlag)
{
//	if(m_nSpecProcDraw==0) 
//		return;
	if(nTransFlag==0)
	{
		if(m_pTmpAnalyData&&m_nTmpDataNum>0)
		{
			memset(m_pObject->pHisDay, 0, m_nDataNum*sizeof(AnalyData));
			memcpy(m_pObject->pHisDay, m_pTmpAnalyData, m_nTmpDataNum*sizeof(AnalyData));
			m_nDataNum = m_nTmpDataNum;
			int nTmp = m_nLeftNo;
			m_nLeftNo = m_nTmpLeftNo;
			m_nTmpLeftNo = nTmp;
			
			nTmp = m_nLastNo;
			m_nLastNo = m_nTmpLastNo;
			m_nTmpLastNo = nTmp;
		}
		return;
	}

	//
	{
		vector<AnalyData> AllHisData;
		GetDataItems_Bit("ETHUSDT", 300, AllHisData);
	//	GetDataItems_Au("207679","Minute","1","4",1,AllHisData);

		TDELA(m_pObject->pHisDay);
		m_nDataNum = AllHisData.size();
		m_pObject->pHisDay = new AnalyData[m_nDataNum];
		memset(m_pObject->pHisDay, 0, m_nDataNum*sizeof(AnalyData));
		memcpy(m_pObject->pHisDay, &AllHisData[0], m_nDataNum*sizeof(AnalyData));
		m_pAnalyData = m_pObject->pHisDay;
	//	m_nPeriod = PER_MIN15;

		if(m_nLastNo<1||m_nLastNo>m_nDataNum-1) m_nLastNo = max(0, m_nDataNum-1);
		return;
	}

	TDELA(m_pTmpAnalyData); m_nTmpDataNum = 0;
	m_pTmpAnalyData = new AnalyData[m_nDataNum];
	memset(m_pTmpAnalyData, 0, m_nDataNum*sizeof(AnalyData));
	memcpy(m_pTmpAnalyData, m_pObject->pHisDay, m_nDataNum*sizeof(AnalyData));
	m_nTmpDataNum = m_nDataNum;
	memset(m_pObject->pHisDay, 0, m_nDataNum*sizeof(AnalyData));

	int nTmp;
	nTmp = m_nLeftNo;
	m_nLeftNo = m_nTmpLeftNo;
	m_nTmpLeftNo = nTmp;
	
	nTmp = m_nLastNo;
	m_nLastNo = m_nTmpLastNo;
	m_nTmpLastNo = nTmp;

	short zdFlag = 0;		//0-待定 1-涨 -1-跌
	for(int i=0,j=0;i<m_nTmpDataNum&&j<m_nTmpDataNum;i++)
	{
		if(i==0) 
		{
			memcpy(&m_pObject->pHisDay[0], &m_pTmpAnalyData[0], sizeof(AnalyData));
			j = 1;
			zdFlag = 0;
		}
		else if(m_pTmpAnalyData[i].Close>m_pTmpAnalyData[i-1].Close)		//上涨
		{
			if(zdFlag==-1)
				memcpy(&m_pObject->pHisDay[j++], &m_pTmpAnalyData[i], sizeof(AnalyData));
			else if(zdFlag==0||zdFlag==1)			//涨，累加
			{
				m_pObject->pHisDay[j-1].Time = m_pTmpAnalyData[i].Time;
				m_pObject->pHisDay[j-1].High = max(m_pObject->pHisDay[j-1].High, m_pTmpAnalyData[i].High);
				m_pObject->pHisDay[j-1].Low = min(m_pObject->pHisDay[j-1].Low, m_pTmpAnalyData[i].Low);
				m_pObject->pHisDay[j-1].Close = m_pTmpAnalyData[i].Close;
				m_pObject->pHisDay[j-1].Volume += m_pTmpAnalyData[i].Volume;
				m_pObject->pHisDay[j-1].Amount += m_pTmpAnalyData[i].Amount;
			}
			zdFlag = 1;
		}
		else if(m_pTmpAnalyData[i].Close<m_pTmpAnalyData[i-1].Close)		//下跌
		{
			if(zdFlag==1)
				memcpy(&m_pObject->pHisDay[j++], &m_pTmpAnalyData[i], sizeof(AnalyData));
			else if(zdFlag==0||zdFlag==-1)			//跌，累加
			{
				m_pObject->pHisDay[j-1].Time = m_pTmpAnalyData[i].Time;
				m_pObject->pHisDay[j-1].High = max(m_pObject->pHisDay[j-1].High, m_pTmpAnalyData[i].High);
				m_pObject->pHisDay[j-1].Low = min(m_pObject->pHisDay[j-1].Low, m_pTmpAnalyData[i].Low);
				m_pObject->pHisDay[j-1].Close = m_pTmpAnalyData[i].Close;
				m_pObject->pHisDay[j-1].Volume += m_pTmpAnalyData[i].Volume;
				m_pObject->pHisDay[j-1].Amount += m_pTmpAnalyData[i].Amount;
			}
			zdFlag = -1;
		}
		else															//平盘，累加
		{
			m_pObject->pHisDay[j-1].Time = m_pTmpAnalyData[i].Time;
			m_pObject->pHisDay[j-1].High = max(m_pObject->pHisDay[j-1].High, m_pTmpAnalyData[i].High);
			m_pObject->pHisDay[j-1].Low = min(m_pObject->pHisDay[j-1].Low, m_pTmpAnalyData[i].Low);
			m_pObject->pHisDay[j-1].Close = m_pTmpAnalyData[i].Close;
			m_pObject->pHisDay[j-1].Volume += m_pTmpAnalyData[i].Volume;
			m_pObject->pHisDay[j-1].Amount += m_pTmpAnalyData[i].Amount;
		}
	}
	m_nDataNum = j;
	if(m_nLastNo<1||m_nLastNo>m_nDataNum-1) m_nLastNo = max(0, m_nDataNum-1);
}

void UFxt::DrawFrame(CDC *pDC)
{
	int	yStart,yEnd,xEnd;
	yStart = 0;
	yEnd = m_rcIn.bottom-FXT_BHEIGHT;
	xEnd = m_rcIn.right-FXT_RWIDTH;
	m_PeriodRc.SetRect(xEnd,yEnd,m_rcIn.right,m_rcIn.bottom);
//	g_d.Draw3DBar3(pDC, m_PeriodRc, RGB(128,128,128),FALSE);
//	g_d.DisplayText(pDC, m_PeriodRc, VipColor.FLOATCURSORCOLOR, DT_CENTER|DT_VCENTER, "看走势");

	if(m_nStyle == FXT_MULTI)
	{
		yEnd = m_rcIn.bottom;
		for ( int i=0;i<m_nRegion;i++ )
			g_d.DrawLine(pDC,m_aRegion[i].Area.left,m_aRegion[i].Area.bottom,m_rcIn.right-FXT_RWIDTH,m_aRegion[i].Area.bottom);
	}
	else
	{
		for ( int i=0;i<m_nRegion;i++ )
			g_d.DrawLine(pDC,m_aRegion[i].Area.left,m_aRegion[i].Area.bottom,m_rcIn.right,m_aRegion[i].Area.bottom);
		
		if(m_pView->IsKindOf(RUNTIME_CLASS(CComView)))
		{
			/*
			m_ViewZoomRect.SetRect(xEnd-13, m_rcIn.top+3, xEnd-3, m_rcIn.top+13);
			m_ViewZoomRect.InflateRect(3,4,3,4);
			CComView *pComView = (CComView*)m_pView;
			if(pComView->IsHQUnitShow())
			{
				g_d.DrawRect(pDC, xEnd-12, m_rcIn.top+3, xEnd-3, m_rcIn.top+12, VipColor.AXISCOLOR, PS_SOLID, 1);
				g_d.DrawLine(pDC, xEnd-6, m_rcIn.top+3, xEnd-6, m_rcIn.top+12, VipColor.AXISCOLOR);
			}
			else
			{
				g_d.DrawRect(pDC, xEnd-10, m_rcIn.top+3, xEnd-3, m_rcIn.top+10, VipColor.AXISCOLOR);
				g_d.DrawRect(pDC, xEnd-12, m_rcIn.top+5, xEnd-5, m_rcIn.top+12, VipColor.AXISCOLOR);
				g_d.Bar(pDC, xEnd-11, m_rcIn.top+6, xEnd-5, m_rcIn.top+11, VipColor.BACKCOLOR);
			}
			*/
		}
	}

	g_d.DrawLine(pDC,xEnd,yStart,xEnd,m_rcIn.bottom);
	//如果在多股同列版面(不在多周期同列)
	if(m_nStyle==FXT_MULTI)	
	{
		int		pos = 0;
		CSize	size(0,0);
		CurrStockData		m_Hq;
		g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq);
		if(m_bSelected)
		{
			pDC->SetBkMode(OPAQUE);
			pDC->SetBkColor(VipColor.FLOATCURSORCOLOR);
		}
		size = g_d.DisplayText(pDC,1,1,VipColor.TXTCOLOR,"%8s ",m_pStkInfo->Name);
		pos += size.cx+1;
		size = g_d.DisplayText(pDC, pos, 1, VipColor.TXTCOLOR,"(%s)",_F(g_CalcPeriodStr[m_nCalcPeriod]));
		pDC->SetBkMode(TRANSPARENT);	
	
		if ( m_Hq.Now > COMPPREC && !m_nInMultiPeriod)
		{
			pos+=size.cx+3;
			size = g_d.BjPrintf(pDC,pos,1,m_Hq.Now,m_Hq.Close,m_XsFlag);pos+=size.cx+3;
			if(m_Hq.Now > COMPPREC && Width() > 200)
			{
				float ftmp = (m_Hq.Now-m_Hq.Close)/m_Hq.Close*100.0;
				size = g_d.BjPrintf(pDC,pos,1,ftmp,0.0,2,0,1);pos+=size.cx+3;
				if(m_Hq.Close > COMPPREC && Width() > 240)
				{
					if(m_Hq.Now-m_Hq.Close > 0)
						size = g_d.DisplayText(pDC,pos,1,VipColor.KPCOLOR,_F("▲"));
					else if(m_Hq.Now-m_Hq.Close < 0)
						size = g_d.DisplayText(pDC,pos,1,VipColor.DOWNCOLOR,_F("▼"));
					else
						size = g_d.DisplayText(pDC,pos,1,VipColor.LEVELCOLOR,"  ");
					pos+=size.cx;
					size = g_d.BjPrintf(pDC,pos,1,m_Hq.Now-m_Hq.Close,0.0,m_XsFlag);pos+=size.cx+3;
					if(Width()>280)
					{
						if(!m_bZs)
						{
							size = g_d.DisplayText(pDC,pos,1,VipColor.TXTCOLOR,_F("现量")); pos+=size.cx+2;
							size = g_d.DisplayText(pDC,pos,1,VipColor.VOLCOLOR,0,"%9d",m_Hq.NowVol);pos+=size.cx+3;
							if(Width()>300 && m_pStkInfo->ActiveCapital > 1)
							{
								size = g_d.DisplayText(pDC,pos,1,VipColor.TXTCOLOR,_F("换手")); pos+=size.cx+2;
								ftmp = 1.0*m_Hq.Volume*m_pStkInfo->Unit/(m_pStkInfo->ActiveCapital*100.0);
								size = g_d.DisplayText(pDC,pos,1,VipColor.TXTCOLOR,0,"%9.2f%%",ftmp);pos+=size.cx+3;
							}
						}
						else if(m_Hq.Amount > 0)
						{
							size = g_d.DisplayText(pDC,pos,1,VipColor.TXTCOLOR,_F("成交额")); pos+=size.cx+2;
							size = g_d.DisplayText(pDC,pos,1,VipColor.VOLCOLOR,0,MakeJE2(m_Hq.Amount));pos+=size.cx+3;
						}
					}
				}
			}
		}
	}
}

void UFxt::DrawOneRegion(CDC * pDC,int nRegion)
{
	//准备阶段
	BOOL	bMoreVOL = 0;
	int nPen = 0,j,k;
	TOBJECT  * p;
	CDC       MemDC;
	CBitmap   BitMap,*OldBit;
	CRect	  MemRc;
	RECT    & theRc = m_aRegion[nRegion].DrawRc;
	MemDC.CreateCompatibleDC(pDC);
	MemRc = theRc;
	theRc.right -= theRc.left;	
	theRc.left = 0;
	theRc.bottom-= theRc.top;	
	theRc.top  = 0;
	BitMap.CreateCompatibleBitmap(pDC,theRc.right,theRc.bottom);	
	OldBit = (CBitmap* )MemDC.SelectObject(&BitMap);
	MemDC.FillSolidRect(&theRc,VipColor.BACKCOLOR);
	///////////////////////////////////
	if ( g_bHasYHidden )
	{//只有DrawYRule的DotNum计算出来才有
		for ( j=0;j<m_aRegion[nRegion].nDotNum;j++ )
			g_d.DotLine(&MemDC,
				m_aRegion[nRegion].DrawRc.left,
				m_aRegion[nRegion].nDotInfo[j],
				m_aRegion[nRegion].DrawRc.right,
				m_aRegion[nRegion].nDotInfo[j],
				VipColor.AXISCOLOR);
	}
	if ( g_bHasXHidden || m_nPeriod==PER_MIN1 || m_nPeriod==PER_MIN5)
	{
		for ( j=0;j<m_nXAxis;j++ )
			g_d.DotLine(&MemDC,m_pXRule[j],m_rcIn.top,m_pXRule[j],m_rcIn.bottom-FXT_BHEIGHT,VipColor.AXISCOLOR);
	}
	////////////////
	if  (  stricmp(m_aRegion[nRegion].aIndexInfo[0]->acCode,"VOL") == 0 )
	{
		for ( j=1;j<m_aRegion[nRegion].nObject;++j )
		{
			if ( stricmp(m_aRegion[nRegion].aIndexInfo[j]->acCode,"VOL") == 0 )
			{
				bMoreVOL = 1;
				break;
			}
		}
	}
	if(nRegion==0 && g_GH.bShowKGap) DrawKGap(&MemDC);
	for ( j=0;j<m_aRegion[nRegion].nObject;j++ )
	{
		p = m_aRegion[nRegion].aIndexInfo[j];
		if ( p->nSet == HIS_DAT )
		{
			//0:K线 1:BAR线 2:收盘站线
			DrawHisLine(&MemDC,nRegion,j,g_nMainZbFlag,-1,MAX_PEN_NUM+5+m_nOverlapNum,MAX_BRUSH_NUM+5+m_nOverlapNum);
		}
		else if ( p->nSet == ZB_OBJECT )
		{//不用本区域的比例等参数
			TINDEXINFO * pIndex = g_pCalc->_CallDTopViewCalc_8(ZB_OBJECT,p->nGSIndex);
			if ( m_aRegion[nRegion].nObject >= 1 && (pIndex->nDrawMode & KOverlay) )
			{
				DrawHisLine(&MemDC,0,0,0,nRegion,MAX_PEN_NUM+5+m_nOverlapNum,MAX_BRUSH_NUM+5+m_nOverlapNum);
			}
			else if ( m_aRegion[nRegion].nObject >= 1 && (pIndex->nDrawMode & UOverlay) )
			{
				DrawHisLine(&MemDC,0,0,1,nRegion,MAX_PEN_NUM+5+m_nOverlapNum,MAX_BRUSH_NUM+5+m_nOverlapNum);
			}
			else if ( m_aRegion[nRegion].nObject >= 1 && (pIndex->nDrawMode & COverlay) )
			{
				DrawHisLine(&MemDC,0,0,3,nRegion,MAX_PEN_NUM+5+m_nOverlapNum,MAX_BRUSH_NUM+5+m_nOverlapNum);
			}
			if ( pIndex->nDelStatus & HIDE_INDEX )
			{
				CFont *oldfont = g_d.SelectFont(&MemDC,GRAPH_FONT);
				g_d.DisplayText(&MemDC,
					(m_aRegion[nRegion].DrawRc.left+m_aRegion[nRegion].DrawRc.right)/2-18,
					(m_aRegion[nRegion].DrawRc.top +m_aRegion[nRegion].DrawRc.bottom)/2,
					VipColor.TXTCOLOR,0,_F("公式:%s 已被禁用"),pIndex->acCode);
				g_d.RestoreFont(&MemDC,oldfont);
				continue;
			}
			if ( pIndex->nDelStatus & ERR_INDEX )
			{
				CFont *oldfont = g_d.SelectFont(&MemDC,GRAPH_FONT);
				g_d.DisplayText(&MemDC,
					(m_aRegion[nRegion].DrawRc.left+m_aRegion[nRegion].DrawRc.right)/2-18,
					(m_aRegion[nRegion].DrawRc.top +m_aRegion[nRegion].DrawRc.bottom)/2,
					VipColor.TXTCOLOR,0,_F("公式:%s 编译未通过"),pIndex->acCode);
				g_d.RestoreFont(&MemDC,oldfont);
				continue;
			}
			if ( pIndex->nDelStatus & DEL_INDEX )
			{
				CFont *oldfont = g_d.SelectFont(&MemDC,GRAPH_FONT);
				g_d.DisplayText(&MemDC,
					(m_aRegion[nRegion].DrawRc.left+m_aRegion[nRegion].DrawRc.right)/2-18,
					(m_aRegion[nRegion].DrawRc.top +m_aRegion[nRegion].DrawRc.bottom)/2,
					VipColor.TXTCOLOR,0,_F("公式:%s 已经删除"),pIndex->acCode);
				g_d.RestoreFont(&MemDC,oldfont);
				continue;
			}
			if ( pIndex->nDrawMode & PUCULINE )
				DrawPUCU(&MemDC,nRegion,j);
			else if ( pIndex->nDrawMode & SARLINE )
				DrawSAR(&MemDC,nRegion,j);
			else
			{ 
				int nType, nWidth;
				DWORD nColor;
				for ( k=0;k<p->nOutNum;k++ )
				{	
					nType = pIndex->aLine[k].nType;
					nColor = pIndex->aLine[k].nColor;
					nWidth = pIndex->aLine[k].nWidth;
					BOOL bSelfColor = nColor>=RGB(0,0,0)&&nColor<=RGB(255,255,255)&&nWidth>0;
					float *fData = p->pCalcData[k];
					
					HandleValue * phv = NULL;
					if(TBE_MEANLESS(fData[m_nDataNum])) 
					{
						DWORD pdVal;
						memcpy(&pdVal, &fData[m_nDataNum], sizeof(DWORD));
						phv = (HandleValue *)pdVal;
						if(phv->nValidFlag!=TMEANLESS_INT)
							phv = NULL;
					}
					if ( !bMoreVOL && nType == 0  
						&& (fabs(m_aRegion[nRegion].dMaxValEx)+fabs(m_aRegion[nRegion].dMinValEx))/2 > COMPPREC )
					{
						if ( m_nStyle == FXT_MULTI && stricmp(pIndex->acCode,"VOL")==0 )
						{
						}
						else if(phv&&phv->nHandleType == 'k')
						{
							DrawKLine(&MemDC, nRegion, j, k, nWidth);
							nPen ++;nPen%=6;
						}
						else if(phv&&phv->nHandleType == 'K')
						{
							DrawKLineEx(&MemDC, nRegion, j, k, nWidth, bSelfColor, nColor);
							nPen ++;nPen%=6;
						}
						else if(phv&&phv->nHandleType == 'l')
						{
							nColor = bSelfColor?nColor:VipColor.ZBCOLOR[nPen];
							DrawStickLine(&MemDC, nRegion, j, k, nColor, nWidth);
							nPen ++;nPen%=6;
						}
						else if(phv&&phv->nHandleType == 'i')
						{
							DrawIcons(&MemDC, nRegion, j, k);
							nPen ++;nPen%=6;
						}
						else if(phv&&phv->nHandleType == 'I')
						{
							DrawIconsEx(&MemDC, nRegion, j, k);
							nPen ++;nPen%=6;
						}
						else if(phv&&phv->nHandleType == 't')
						{
							nColor = bSelfColor?nColor:VipColor.LEVELCOLOR;
							DrawTextLine(&MemDC, nRegion, j, k, nColor);
							nPen ++;nPen%=6;
						}
						else if(phv&&phv->nHandleType == 'T')
						{
							nColor = bSelfColor?nColor:VipColor.LEVELCOLOR;
							DrawTextLineEx(&MemDC, nRegion, j, k, nColor);
							nPen ++;nPen%=6;
						}
						else if(phv&&phv->nHandleType == 'n')
						{
							nColor = bSelfColor?nColor:VipColor.LEVELCOLOR;
							DrawNumLine(&MemDC, nRegion, j, k, nColor);
							nPen ++;nPen%=6;
						}
						else if(phv&&phv->nHandleType == 'D')
						{
							DrawBand(&MemDC, nRegion, j, k);
						}
						else if(phv&&phv->nHandleType == 'p')
						{
							nColor = bSelfColor?nColor:VipColor.ZBCOLOR[nPen];
							DrawPartLine(&MemDC, nRegion, j, k, nColor, nWidth);
						}
						else
						{
							nColor = bSelfColor?nColor:VipColor.ZBCOLOR[nPen];
							DrawZBLine(&MemDC,nRegion,j,k,nColor,nWidth);
							nPen ++;nPen%=6;
						}
					}
					else if ( !bMoreVOL && pIndex->aLine[k].nType == STICK )
					{
						nColor = bSelfColor?nColor:VipColor.ZBCOLOR[nPen];
						DrawStick(&MemDC, nRegion, j, k, nColor, nWidth);
						nPen++;nPen%=6;
					}
					else if ( !bMoreVOL && pIndex->aLine[k].nType == COLORSTICK )
					{	
						DrawColorStick(&MemDC,nRegion,j,k);
					}
					else if ( pIndex->aLine[k].nType == VOLSTICK )
					{	
						DrawVolStick(&MemDC,nRegion,j,k);
					}
					else if ( pIndex->aLine[k].nType == LINESTICK  )
					{
						nColor = bSelfColor?nColor:VipColor.ZBCOLOR[nPen];
						DrawLineStick(&MemDC,nRegion,j,k,nColor,nWidth);
						nPen++;nPen%=6;
					}
					else if ( pIndex->aLine[k].nType == ZBCIRCLEDOT )
					{
						nColor = bSelfColor?nColor:VipColor.ZBCOLOR[nPen];
						DrawCircleDot(&MemDC,nRegion,j,k,nColor,nWidth);
						nPen++;nPen%=6;
					}
					else if ( pIndex->aLine[k].nType == ZBCROSSDOT )
					{
						nColor = bSelfColor?nColor:VipColor.ZBCOLOR[nPen];
						DrawCrossDot(&MemDC,nRegion,j,k,nColor,nWidth);
						nPen++;nPen%=6;
					}
					else if ( pIndex->aLine[k].nType == POINTDOT )
					{
						nColor = bSelfColor?nColor:VipColor.ZBCOLOR[nPen];
						DrawPointDot(&MemDC, nRegion, j, k, nColor,nWidth);
					}
					else if ( pIndex->aLine[k].nType == NODRAW )
					{
						nPen ++;
						nPen%=6;
					}
				}
				if ( p->Property & EXTRA_Y  ) 
				{	
					int yy;
					for ( k=0;k<pIndex->nAddXAxisNum;k++ )
					{	
						yy = GetYPosOfRegion(nRegion,pIndex->afAddXAxis[k],j);
						g_d.DotLine(&MemDC,m_aRegion[nRegion].Area.left,yy,m_aRegion[nRegion].Area.right,yy,VipColor.TXTCOLOR,2);
					}
				}
			}
		}
		else if ( p->nSet == SEPCLINE_TYPE )
			DrawSpecialLine(&MemDC,nRegion,j);
		else if ( !m_bOverStk && p->nSet == EXP_OBJECT )
		{
			int y;
			HICON hIcon = m_hExpUpShow;
			if ( m_SelectInfo.bExist && m_SelectInfo.nSet == EXP_OBJECT )
			{
				hIcon = m_hExpUpShow_Sel;
				m_bEvenSelected = TRUE;
			}
			for ( k=0;k<m_nDrawNum;k++ )
			{
				if (TBE_MEANLESS( p->pCalcData[0][k+m_nLeftNo] ) != 0 && fabs(p->pCalcData[0][k+m_nLeftNo]) > 0.1 )
				{   
					y = GetYPosOfRegion(nRegion,m_pObject->pHisDay[m_nLeftNo+k].Low) +2;
					MemDC.DrawIcon(m_pPosOfX[k]-15,y,hIcon);
				}
			}
			hIcon = m_hExpDownShow;
			if ( m_SelectInfo.bExist && m_SelectInfo.nSet == EXP_OBJECT )
			{
				hIcon = m_hExpDownShow_Sel;
				m_bEvenSelected = TRUE;
			}
			for ( k=0;k<m_nDrawNum;k++ )
			{
				if (TBE_MEANLESS( p->pCalcData[1][k+m_nLeftNo] ) != 0 &&  fabs(p->pCalcData[1][k+m_nLeftNo]) > 0.1 )
				{	
					y = GetYPosOfRegion(nRegion,m_pObject->pHisDay[m_nLeftNo+k].High) - 32-2;
					MemDC.DrawIcon(m_pPosOfX[k]-15,y,hIcon);
				}
			}
		}		
	}
	//还原阶段
	pDC->BitBlt(MemRc.left+1,MemRc.top,theRc.right-1,theRc.bottom,&MemDC,0,0,SRCCOPY);
	theRc = MemRc;
	MemDC.SelectObject(OldBit);
	BitMap.DeleteObject();
	MemDC.DeleteDC();
}

void UFxt::DrawIt(CDC *pDC)
{
	int i = 0,j = 0,k=0;
	if ( m_rcIn.bottom-FXT_BHEIGHT <= 0 )
		return;
	if ( m_bChgRegion )
	{	
		InitRegion();
		m_bChgRatio  = TRUE;
		m_bChgRegion = FALSE;
	}
	//改变各种比例
	if ( m_bChgRatio )
	{	
		CalcAllRatio();
		CalcRuleDrawAxis();
		m_bChgRatio = FALSE;
		FillCbInfo(bottom,m_nLastNo);
		m_pView->SendMessage(UM_REFLECT,Reflect_Cb_FxtStart);
	}
	//
	DrawYRule(pDC);
	DrawXGrid(pDC);
	DrawFrame(pDC);
	if ( m_nDataNum > 0 && m_bDataReady )
	{
		for ( i=0;i<m_nRegion;i++ )
			DrawOneRegion(pDC,i);
	}
	if ( m_nStyle != FXT_MULTI )
	{
		
		//显示周期
		if (strcmp(g_CalcPeriodStr[m_nCalcPeriod], "多日线") == 0)
			g_d.DisplayText(pDC,m_PeriodRc,VipColor.TXTCOLOR,DT_CENTER,_F("%d日线"),g_nDaynNum);
		else
		if (strcmp(g_CalcPeriodStr[m_nCalcPeriod], "多分钟") == 0)
			g_d.DisplayText(pDC,m_PeriodRc,VipColor.TXTCOLOR,DT_CENTER,_F("%d分钟"),g_nMinnNum);
		else
			g_d.DisplayText(pDC,m_PeriodRc,VipColor.TXTCOLOR,DT_CENTER,"%s",_F(g_CalcPeriodStr[m_nCalcPeriod]));
		
		MarkInfo(pDC);
		if ( m_bMouseCross && m_OldMousePoint.x>0 )
			Disp(pDC,m_OldMousePoint.x-left,m_OldMousePoint.y-top,1);
		else if ( m_bKeyCross && ( m_OldKeyPoint.x>0 || m_OldKeyPoint.y>0 ))
			Disp(pDC,m_OldKeyPoint.x,m_OldKeyPoint.y,1);
		else
			Disp(pDC,-1,-1,1);
	}
	else
		g_d.DrawLine(pDC,m_rcIn.right-FXT_RWIDTH,m_rcIn.bottom,m_rcIn.right,m_rcIn.bottom,VipColor.AXISCOLOR);
	
	if(m_nStyle != FXT_MULTI)
	{
		if(m_pTabSwitch)	
			m_pTabSwitch->SendMessage(WM_PAINT);
		if(IsFxtShowTab() && m_pSwitch)	
			m_pSwitch->SendMessage(WM_PAINT);
	}
}

void UFxt::DrawKGap(CDC * pDC)
{
	if(m_nDataNum<=0) return;
	if(m_nLeftNo<0 || m_nLeftNo>=m_nDataNum || m_nLastNo<0 || m_nLastNo>=m_nDataNum) return;
	int i=0;
	struct AnalyData * pHisDay = m_pObject->pHisDay;
	float fLow=0,fHigh=0;
	int nLow=0,nHigh=0;
	int Off = int(m_dWidth*0.5);
	short GapFlag = 1;
	fLow	= pHisDay[m_nLastNo].Low;
	fHigh	= pHisDay[m_nLastNo].High;
	for(i=m_nLastNo;i>m_nLeftNo;i--)
	{
		if(fLow>pHisDay[i-1].High+COMPPREC)				//高跳
		{
			fHigh	= fLow;
			fLow	= pHisDay[i-1].High;
			GapFlag = 1;
			break;
		}
		else if(pHisDay[i-1].Low>fHigh+COMPPREC)		//低跳
		{
			fLow	= fHigh;
			fHigh	= pHisDay[i-1].Low;
			GapFlag = 2;
			break;
		}
		fLow	= min(fLow,pHisDay[i-1].Low);
		fHigh	= max(fHigh,pHisDay[i-1].High);
	}
	if(i>m_nLeftNo)
	{
		nLow = GetYPosOfRegion(0,fLow);
		nHigh = min(nLow-1,GetYPosOfRegion(0,fHigh)+1);
		g_d.Bar(pDC,m_pAllPosOfX[i-1]+Off,nLow,max(m_rcIn.right-m_rcIn.left-FXT_RWIDTH,m_pAllPosOfX[i-1]+Off+1),nHigh,OVERLAP_BRUSH);
		CFont *oldfont = g_d.SelectFont(pDC,GRAPH_FONT);
		if(GapFlag == 1)
		{
			if(GetXSFlag(m_pStkInfo)==XS3) 
				g_d.DisplayText(pDC,m_pAllPosOfX[i-1]+Off+2,nLow+2,VipColor.TXTCOLOR,0,"%.3f-%.3f",fLow,fHigh);
			else g_d.DisplayText(pDC,m_pAllPosOfX[i-1]+Off+2,nLow+2,VipColor.TXTCOLOR,0,"%.2f-%.2f",fLow,fHigh);
		}
		else
		{
			if(GetXSFlag(m_pStkInfo)==XS3) 
				g_d.DisplayText(pDC,m_pAllPosOfX[i-1]+Off+2,nHigh-16,VipColor.TXTCOLOR,0,"%.3f-%.3f",fLow,fHigh);
			else g_d.DisplayText(pDC,m_pAllPosOfX[i-1]+Off+2,nHigh-16,VipColor.TXTCOLOR,0,"%.2f-%.2f",fLow,fHigh);
		}
		g_d.RestoreFont(pDC, oldfont);
	}
}

void UFxt::MarkInfo(CDC * pDC)
{
	int i=0,nT;
	if ( m_nDataNum <= 0 || !m_bDataReady ) 	
		return;
	if ( m_aRegion[0].Area.bottom-m_aRegion[0].Area.top<50 )
		return;
	if ( m_nStyle != FXT_MULTI && fabs(m_aRegion[0].dMaxVal - DMAXVAL)>1 )
	{	
		CString	lpFormat;
		lpFormat.Format("%%10.%df",m_XsFlag);
		int Y,dOff = 12,rectwidth=0;
		for ( nT=0;nT<m_pObject[0].nMM[0]; ++nT )
		{
			//只画一个
			if(nT>0) break;
			//
			i = m_pObject[0].nMaxPos[nT];
			if ( nT > 0 && abs(m_pPosOfX[m_pObject[0].nMaxPos[nT-1]]-m_pPosOfX[i]) < 60 )
				continue;
			if(i<0 || i>=m_nDataNum) //防止原来的雷位置打破新的股票
				continue;
			dOff = 12,rectwidth=0;
			if ( right-FXT_RWIDTH-m_pPosOfX[i]-left < 60 )
				dOff = -20 , rectwidth = -dOff;	//左边,则右对齐
			Y = GetYPosOfRegion(0,m_pObject[0].dMaxVal);
			g_d.DisplayText(pDC,m_pPosOfX[i]+9*dOff/5-6,Y,VipColor.KPCOLOR,rectwidth,lpFormat,m_pObject[0].dMaxVal);
			g_d.DotLine(pDC,m_pPosOfX[i]+dOff,Y+4,m_pPosOfX[i],Y,VipColor.KPCOLOR);
		//	g_d.DrawLine(pDC,m_pPosOfX[i]+dOff/4,Y+4,m_pPosOfX[i],Y,VipColor.TXTCOLOR);
		//	g_d.DrawLine(pDC,m_pPosOfX[i]+dOff/3,Y-1,m_pPosOfX[i],Y,VipColor.TXTCOLOR);
		}
		for ( nT=0;nT<m_pObject[0].nMM[1];++nT )
		{
			//只画一个
			if(nT>0) 
				break;
			i = m_pObject[0].nMinPos[nT];
			if ( nT > 0 && abs(m_pPosOfX[m_pObject[0].nMinPos[nT-1]]-m_pPosOfX[i]) < 60 )
				continue;
			if(i<0 || i>=m_nDataNum)
				continue;
			if ( right-FXT_RWIDTH-m_pPosOfX[i]-left < 60 )
				dOff = -20 , rectwidth = -dOff;
			else if ( m_pPosOfX[i]+dOff-rectwidth < 0 )
				dOff = 15  , rectwidth = -5;
			Y = GetYPosOfRegion(0,m_pObject[0].dMinVal)+12;

			g_d.DisplayText(pDC,m_pPosOfX[i]+dOff-rectwidth,Y-TEXTHIGH,VipColor.KNCOLOR,rectwidth,lpFormat,m_pObject[0].dMinVal);
			g_d.DotLine(pDC,m_pPosOfX[i]+dOff,Y-8,m_pPosOfX[i],Y-8,VipColor.KNCOLOR);
		//	g_d.DrawLine(pDC,m_pPosOfX[i]+dOff/3,Y-10,m_pPosOfX[i],Y-8,VipColor.TXTCOLOR);
		//	g_d.DrawLine(pDC,m_pPosOfX[i]+dOff/3,Y-5,m_pPosOfX[i],Y-8,VipColor.TXTCOLOR);
		}
	}
	for ( i=0;i<m_aRegion[0].nObject;++i )
	{
		if ( m_aRegion[0].aIndexInfo[i]->nSet == GBZS_OBJECT )
		{
			for (int j=0;j<m_nDrawNum;++j )
			{
				if ( m_aRegion[0].aIndexInfo[i]->pCalcData[0][m_nLeftNo+j] > COMPPREC ) 
					pDC->DrawIcon(m_pPosOfX[j]-16,m_aRegion[0].Area.bottom-32,m_hGBBQShow);
			}
		}
		//变名
		if ( m_aRegion[0].aIndexInfo[i]->nSet == FC_OBJECT )
		{
			for (int j=0;j<m_nDrawNum;++j )
			{
				if ( m_aRegion[0].aIndexInfo[i]->pCalcData[0][m_nLeftNo+j] > COMPPREC ) 
					pDC->DrawIcon(m_pPosOfX[j]-16,m_aRegion[0].Area.bottom-32,m_hFCIcon);
			}
		}
		//个股备忘
		if( m_aRegion[0].aIndexInfo[i]->nSet == GGBW_OBJECT )
		{
			for(int j=0;j<m_nDrawNum;++j )
			{
				if(m_aRegion[0].aIndexInfo[i]->pCalcData[0][m_nLeftNo+j] > COMPPREC )
					pDC->DrawIcon(m_pPosOfX[j]-16,m_aRegion[0].Area.bottom-32,m_hGGBWShow);
			}
		}
		if ( m_aRegion[0].aIndexInfo[i]->nSet == THUN_OBJECT )
		{
			for (int j=0;j<m_nDrawNum;++j )
			{
				if ( m_aRegion[0].aIndexInfo[i]->pCalcData[0][m_nLeftNo+j] > COMPPREC ) 
				{
					if ( m_aRegion[0].aIndexInfo[i]->pCalcData[2][m_nLeftNo+j] > 1 )
						pDC->DrawIcon(m_pPosOfX[j]-16,m_aRegion[0].DrawRc.top-28,m_hMoreThunder);
					else
						pDC->DrawIcon(m_pPosOfX[j]-16,m_aRegion[0].DrawRc.top-28,m_hOneThunder);
				}
			}
		}
	}
}

void UFxt::DrawLastState(CDC * pDC)
{
	int nMode = pDC->SetROP2(R2_XORPEN);
	if ( m_bMouseCross && m_OldMousePoint.x>0 )
	{	
		for(int i=0;i<m_nRegion;i++)
		{
			g_d.DrawLine(pDC, m_OldMousePoint.x, top+m_aRegion[i].Area.top+((i==0)?2:1)*TEXTHIGH, 
				m_OldMousePoint.x, top+m_aRegion[i].Area.bottom, VipColor.TXTCOLOR^VipColor.BACKCOLOR);
		}
		g_d.DrawLine(pDC,m_rcIn.left+left,m_OldMousePoint.y,
			right-FXT_RWIDTH,m_OldMousePoint.y,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
		m_nToLeft = 0 , m_nToRight = 0;
	}
	else if ( m_bKeyCross && ( m_OldKeyPoint.x>0 || m_OldKeyPoint.y>0 ))
	{	
		for(int i=0;i<m_nRegion;i++)
		{
			g_d.DrawLine(pDC, left+m_OldKeyPoint.x, top+m_aRegion[i].Area.top+((i==0)?2:1)*TEXTHIGH, 
				left+m_OldKeyPoint.x, top+m_aRegion[i].Area.bottom, VipColor.TXTCOLOR^VipColor.BACKCOLOR);
		}
		g_d.DrawLine(pDC,m_rcIn.left+left,m_OldKeyPoint.y+top,
			right-FXT_RWIDTH,m_OldKeyPoint.y+top,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
	}
	pDC->SetROP2(nMode);
}

void UFxt::ResetCross(CDC * pDC)
{
	DrawLastState(pDC);
	memset(&m_OldKeyPoint,0,sizeof(POINT));
	memset(&m_OldMousePoint,0,sizeof(POINT));
	m_bMouseCross = m_bKeyCross = 0;
	m_KLock = -1;
}

int UFxt::MouseMove(CDC * pDC,CPoint point)
{
	CPoint	XDPt = point;
	XDPt.x -= left;
	XDPt.y -= top;
	if ( point == m_RealMove )
		return 0;
	m_RealMove = point;

	CUnitView *pUnitView = NULL;
	if(m_pView->IsKindOf(RUNTIME_CLASS(CComView)) || m_pView->IsKindOf(RUNTIME_CLASS(CComView2)))
	pUnitView = (CUnitView*)m_pView;	

	if(m_bDrawLine)
	{
		m_ToolTip.PopToolTip();
		m_TipInfo.Empty();
		m_ToolTip.UpdateToolTip(m_pView,"");

		SetCursor(m_hDraw);
	}
	else if ( m_nMousePan  )
	{	SetCursor(m_hFxtMove);
		if ( m_nMousePan & 0xFFFF0000 )
		{
			int NowItem = GetItemNoFromX(point.x-left)+m_nLeftNo;
			double Delta = m_dNowVal - GetValFromY(m_nAtRegion,point.y-top);
			if ( m_bLArchor )
			{
				XPan(m_nAtItem-NowItem);
				if ( fabs(Delta) > COMPPREC )
				{
					double dOrg = Delta;
					if ( m_nAtRegion == 0 )
					{
						switch ( m_nMainAxisStatus )
						{
						case LOG10_AXIS:
						case R100_AXIS:
							break;
						default:
							m_aRegion[m_nAtRegion].dMaxVal += Delta;
							m_aRegion[m_nAtRegion].dMinVal += Delta;
							m_aRegion[m_nAtRegion].dMaxValEx += dOrg;
							m_aRegion[m_nAtRegion].dMinValEx += dOrg;
							break;
						}
					}
					else
					{	m_aRegion[m_nAtRegion].dMaxVal += Delta;
						m_aRegion[m_nAtRegion].dMinVal += Delta;
						m_aRegion[m_nAtRegion].dMaxValEx += dOrg;
						m_aRegion[m_nAtRegion].dMinValEx += dOrg;
					}
				}
			}
			else if ( m_bRArchor )
			{
				double dr = (NowItem-m_nAtItem)/(double)m_nWillNum+1.0;
				m_nWillNum *= dr;
				if ( m_nWillNum > m_nDataNum )
					m_nWillNum = m_nDataNum+3;
				else if ( m_nWillNum <=20 )		
					m_nWillNum = 20;
				CalcAllRatio(FALSE);
				if ( fabs(Delta/m_dNowVal) > 0.01 )
				{
					if ( Delta < -COMPPREC )
					{
						m_aRegion[m_nAtRegion].dPerUnit *= 1.03;
						m_aRegion[m_nAtRegion].dMaxVal  *= 0.97;
						m_aRegion[m_nAtRegion].dMaxValEx*= 0.97;
					}
					else if ( Delta > COMPPREC )
					{
						m_aRegion[m_nAtRegion].dPerUnit *= 0.97;
						m_aRegion[m_nAtRegion].dMaxVal  *= 1.03;
						m_aRegion[m_nAtRegion].dMaxValEx*= 1.03;
					}
					m_dNowVal = GetValFromY(m_nAtRegion,point.y-top);
				}
			}
			CalcRuleDrawAxis();		//重新计算X、Y轴的坐标
			ProcessMsg(WM_PAINT);
			return 0;	//不处理十字光标
		}
	}
	else if ( PtInRect(point) && point.y >= bottom-FXT_BHEIGHT && point.x < right-FXT_RWIDTH )
	{
		DrawLastState(pDC);
		memset(&m_OldMousePoint,0,sizeof(POINT));
		if ( m_nChgMouse == 0 )
			SetCursor(m_hMove);
		if ( m_nAtItem && m_bLArchor  )
		{
			int NowItem = GetItemNoFromX(point.x-left)+m_nLeftNo;
			double dr = (NowItem-m_nAtItem)/(double)m_nWillNum+1.0;
			if ( (point.x-m_nDownPoint.x)*(NowItem-m_nAtItem) > 0 )
			{
				m_nWillNum *= dr;
				if ( m_nWillNum <=20 )		
					m_nWillNum = 20;
				else if ( m_nWillNum > m_nDataNum+10 )
					m_nWillNum = m_nDataNum+10;

				CalcAllRatio();
				CalcRuleDrawAxis();		//重新计算X、Y轴的坐标
				ProcessMsg(WM_PAINT);
			}
			else if ( (point.x-m_nDownPoint.x)*(NowItem-m_nAtItem) < 0 
				&&  (point.x-m_OrgPoint.x)*(NowItem-m_nAtItem) <0 
				)
			{
				m_nDownPoint  = point;
			}
		}
		else if ( m_nAtItem && m_bRArchor )
		{
			int NowItem = GetItemNoFromX(point.x-left)+m_nLeftNo;
			XPan(m_nAtItem-NowItem,TRUE);
			CalcRuleDrawAxis();		//重新计算X、Y轴的坐标
			ProcessMsg(WM_PAINT);
		}
		return 0;
	}
	else if(m_SelectInfo.bExist && m_SelectInfo.IsMovable && m_SelectInfo.nSet==SELFLINE_OBJECT)
		SelfLineChg(pDC,point);
	else if ( m_bLArchor && !m_EageInfo.bAtEage && m_SelectInfo.bExist  )
	{	//如果是左键Down,并且有选中,可移动. 如果在移动轴，也不应当变鼠标.
		CRect r(&m_aRegion[m_SelectInfo.nInRegion].Area);
		if ( m_SelectInfo.IsMovable && !r.PtInRect(XDPt))
		{
			switch ( m_SelectInfo.nSet )
			{
			case ZB_OBJECT:
				if ( GetKeyState(VK_CONTROL) & 0xFF00 )
					SetCursor(m_hCanDrop_Overlap),
					m_SelectInfo.bAdd = TRUE;
				else
					SetCursor(m_hCanDrop);
				m_nChgMouse =1;			//鼠标现状变化
				break;
			case KL_OBJECT:
				SetCursor(m_hCannotDrop);	m_nChgMouse =1;
				break;
			case SELFLINE_OBJECT:	//已经处理完毕
				break;
			case HIS_DAT:
				SetCursor(m_hCannotDrop);	m_nChgMouse =1;
				break;
			}
		}
		else
			SetCursor(m_hCannotDrop);	m_nChgMouse =1;
	}//以上内容不画ToolTip，不画游标 但是他们也要处理十字光标等
	else if ( m_bBeginRectangle && abs(m_OrgPoint.x-point.x)>2*SELECT_ON )
	{  
		int tempMode = pDC->SetROP2 (R2_XORPEN);
		if ( point != m_ArchorPoint && m_ArchorPoint.y !=0 )
			g_d.DrawRect(pDC,m_OrgPoint.x,m_OrgPoint.y,m_ArchorPoint.x,m_ArchorPoint.y,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
		m_ArchorPoint = point;
		g_d.DrawRect(pDC,m_OrgPoint.x,m_OrgPoint.y,m_ArchorPoint.x,m_ArchorPoint.y,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
		pDC->SetROP2 (tempMode);
		//显示游标
		DrawXRule(pDC,point.x-left,point.y-top);
		DrawYRule(pDC,point.x-left,point.y-top);
	}
	else
	{
		CRect UnitRect(m_PeriodRc);
		UnitRect.left += left;
		UnitRect.right += left;
		UnitRect.top += top;
		UnitRect.bottom += top;
		if(pUnitView&&m_pStkInfo&&UnitRect.PtInRect(point))
		{	
			pUnitView->ShowPopPreviewTool(ZST_UNIT, ZST_MULTI, UnitRect, m_pStkInfo);
		}
		else if(pUnitView) 
			pUnitView->HidePopPreviewTool();

		if(PtInRect(point) && (point.y >= top+20 || (point.x>left+20 && point.x<right-20)))
		{
			SetCursor(LoadCursor(NULL,IDC_ARROW));
		}
		BOOL	bSelfOK = 0;
		FXTSELINFO		OldSelf = m_SelectInfo;
		memset(&m_SelectInfo,0,sizeof(m_SelectInfo));
		for ( int nR=0;nR<m_nRegion;++nR )
		{
			if ( SelectSelfLine(nR,point) >=0 )
			{
				if ( m_SelectInfo.nInOnePoint != 0  )
					SetCursor(AfxGetApp()->LoadCursor(IDC_DRAW_TURN));
				else
					SetCursor(AfxGetApp()->LoadCursor(IDC_MOVE2));
				m_nChgMouse =1;
				bSelfOK = 1;
				break;
			}
		}
		m_SelectInfo = OldSelf;
		/////////////////////////////
		if ( !bSelfOK )
		{
			//以下内容画ToolTip，游标
			if ( m_MainYRc.PtInRect(XDPt) ||
				(point.x>right-FXT_RWIDTH && point.y>bottom-FXT_BHEIGHT) )
			{	//如果落在主图的标尺中.
				m_TipInfo.Empty();
				//清楚十字光标
				if ( m_OldMousePoint.x > 0 )
					DrawLastState(pDC);
				memset(&m_OldMousePoint,0,sizeof(POINT));	//点不是有效的将不画，如x=0就不画
				return 0;
			}
			else if ( m_EageInfo.bAtEage || IsAtEage(XDPt) != -1 )
			{
				SetCursor(m_hHorSplit);	
				if ( m_EageInfo.bAtEage )
				{	if ( point != m_EageInfo.EagePoint && m_EageInfo.EagePoint.y>0 )
						g_d.ClipLine(pDC,left,m_EageInfo.EagePoint.y,right-FXT_RWIDTH,m_EageInfo.EagePoint.y,*this,VipColor.TXTCOLOR,R2_XORPEN);

					m_EageInfo.EagePoint = point;
					g_d.ClipLine(pDC,left,point.y,right-FXT_RWIDTH,point.y,*this,VipColor.TXTCOLOR,R2_XORPEN);
				}
				else
				{	//仅仅清楚十字光标，不清除状态!
					if ( m_OldMousePoint.x > 0 )
						DrawLastState(pDC);
					memset(&m_OldMousePoint,0,sizeof(POINT));
				}
				return 0;
			}
			else 
				SetCursor(LoadCursor(NULL,IDC_ARROW));
			////////////////////////////
			if ( point != m_TipArchor )
			{	
				//关闭ToolTip
				m_ToolTip.PopToolTip();
				m_TipArchor = point;
				m_TipInfo.Empty();
			}
			if ( !m_bOverStk && m_bExistZS )
			{	
				CRect zsrc;
				if ( m_KLZS.p )
				{	
					zsrc = m_KLZS.rect;
					if ( zsrc.PtInRect(point) )
					{
						m_TipInfo.Format("%s",m_KLZS.strInfo);
						m_ToolTip.UpdateToolTip(m_pView,_F(m_TipInfo));
					}
				}
				if ( m_ExpZS.p )
				{	
					zsrc = m_ExpZS.rect;
					if ( zsrc.PtInRect(point) )
					{
						m_TipInfo.Format("%s",m_ExpZS.strInfo);
						m_ToolTip.UpdateToolTip(m_pView,_F(m_TipInfo));
					}
				}
			}
			if(!m_bDrawLine)
			{
				if(GetTipInfo(point.x-left,point.y-top))
					m_ToolTip.UpdateToolTip(m_pView,_F(m_TipInfo));
				else
					m_ToolTip.UpdateToolTip(m_pView,"");
			}
			//////////////////////////////////////////////
			//显示游标
			DrawXRule(pDC,point.x-left,point.y-top);
			DrawYRule(pDC,point.x-left,point.y-top);
		}
	}
	if ( m_bKeyCross )
	{
		ResetCross(pDC);
		m_bMouseCross = 1;
	}
	if ( m_bMouseCross && point.x<right-FXT_RWIDTH && point.y<bottom-FXT_BHEIGHT)
	{	
		DrawLastState(pDC);
		m_OldMousePoint = point;
		FillDlgInfo(GetItemNoFromX(point.x-left)+m_nLeftNo);
		Disp(pDC,point.x,point.y);
		DrawLastState(pDC);
		PopupInfoDlg();
		Lock_K();
		Lock_KData();
	}
	else
	{	DrawLastState(pDC);
		Lock_K();
		Lock_KData();
		memset(&m_OldMousePoint,0,sizeof(POINT));
	}
	if(m_bDrawLine)
		DrawingMouseMove(pDC,point,NEWLINE_ID);
	if(g_CbInfo.m_pData)
	{
		FillCbInfo(point.y,m_nLeftNo + max(0,GetItemNoFromX(point.x-left)));
		m_pView->SendMessage(UM_REFLECT,Reflect_Cb_Date);
		m_pView->SendMessage(UM_REFLECT,Reflect_Tv_Date);
	}
	return 1;
}

#define ExpOK()	\
do{	\
	m_SelectInfo.nSet = EXP_OBJECT;	\
	m_SelectInfo.nGSIndex =m_ExpZS.p->nGSIndex;	\
	m_SelectInfo.IsMovable = 0;	\
	m_SelectInfo.bExist = 1; m_SelectInfo.nInRegion = nRegion;\
	m_SelectInfo.nInObject=nNo;	\
}while(0)

BOOL UFxt::SelecZS(int nRegion,int nNo,int nItem,CPoint point)
{
	CRect rc;
	int aimY = 0;
	if ( !m_bExistZS )
		return FALSE;
	point.x -= left;
	point.y -= top;
	if ( m_KLZS.p  && m_KLZS.p == m_aRegion[nRegion].aIndexInfo[nNo] )
	{	rc = m_KLZS.rect;
		if ( rc.PtInRect(point) )
		{
			m_SelectInfo.nSet = KL_OBJECT;
			m_SelectInfo.nGSIndex =m_KLZS.p->nGSIndex;
			m_SelectInfo.IsMovable = 0; //不能移动
			m_SelectInfo.bExist = 1;	m_SelectInfo.nInRegion = nRegion;
			m_SelectInfo.nInObject=nNo;	
			return TRUE;
		}
	}
	if ( m_ExpZS.p && m_ExpZS.p == m_aRegion[nRegion].aIndexInfo[nNo] )
	{	
		rc = m_ExpZS.rect;
		if ( rc.PtInRect(point) )
		{
			ExpOK();
			return TRUE;
		}
		if (  TBE_MEANLESS(m_ExpZS.p->pCalcData[ENTERLONG][nItem]) != 0 &&  
			( fabs(m_ExpZS.p->pCalcData[ENTERLONG][nItem]) < 0.1 
			|| fabs(m_ExpZS.p->pCalcData[ENTERSHORT][nItem]) > 0.1 )
		   )
		{	
			aimY = GetYPosOfRegion(nRegion,m_pObject->pHisDay[nItem].Low)+8;
			if ( abs(point.y-aimY) <= SELECT_ON )
			{	
				ExpOK();
				return TRUE;
			}
		}
		if ( TBE_MEANLESS(m_ExpZS.p->pCalcData[EXITLONG][nItem]) != 0 && 
			( fabs(m_ExpZS.p->pCalcData[EXITLONG][nItem]) > 0.1 
			|| fabs(m_ExpZS.p->pCalcData[EXITSHORT][nItem]) > 0.1 )
		   )
		{
			aimY = GetYPosOfRegion(nRegion,m_pObject->pHisDay[nItem].High)-8;
			if ( abs(point.y-aimY) <= SELECT_ON )
			{	
				ExpOK();
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL UFxt::QuickSelect(CPoint point)
{
	CRect tmpRc;
	CPoint	XDPt = point;
	XDPt.x -= left;
	XDPt.y -= top;
	////////////////////////
	memset(&m_SelectInfo,0,sizeof(FXTSELINFO));
	for ( int i=0;i<m_nRegion;i++ )
	{
		tmpRc = CRect(m_rcIn.left,m_aRegion[i].Area.top-TEXTHIGH,m_rcIn.right,m_aRegion[i].Area.bottom);
		if ( tmpRc.PtInRect(XDPt) )
		{
			m_nSelRegion = i;
			SetSwitchSel();
		}
		tmpRc = m_aRegion[i].Area;
		if ( tmpRc.PtInRect(XDPt) )	break;
	}
	if ( i == m_nRegion ) return FALSE;
	m_nSelfLineIn = i;
	//////////////////////////////////
	int j=0,k=0,No = GetItemNoFromX(XDPt.x);
	TOBJECT	* pObject = NULL;
	for ( j = 0;j<m_aRegion[i].nObject;j++ )
	{
		pObject = m_aRegion[i].aIndexInfo[j];
		if ( XDPt.y >= m_aRegion[i].DrawRc.top-20+7 && XDPt.y < m_aRegion[i].DrawRc.top-20+23 )
		{
			if ( pObject->nSet == THUN_OBJECT )
			{
				int Item = GetItemNoFromX(XDPt.x);
				if ( pObject->pCalcData[0][m_nLeftNo+Item] > COMPPREC )
				{
					m_SelectInfo.nSet = THUN_OBJECT;
					m_SelectInfo.nOutNo = 0;
					m_SelectInfo.nInObject=j;
					m_SelectInfo.nGSIndex=pObject->pCalcData[1][m_nLeftNo+Item];
					m_SelectInfo.IsMovable = 0;
					m_SelectInfo.bExist = 1;	m_SelectInfo.nInRegion = i;
					return TRUE;
				}
			}
		}
		if(SelectSelfLine(i,point)>=0)
		{	
			m_PtInArea = m_aRegion[i].DrawRc;
			m_AbsPtInArea = m_PtInArea;
			m_AbsPtInArea.OffsetRect(left,top);
			return TRUE;
		}
		if ( pObject->nSet != HIS_DAT )
		{
			if ( (pObject->nSet == KL_OBJECT ||	 pObject->nSet == EXP_OBJECT ) &&
				SelecZS(i,j,No+m_nLeftNo,point) )
				return TRUE;
			////////////////////////////
			if ( pObject->nSet == ZB_OBJECT )
			{
				for ( k = 0; k<pObject->nOutNum; k++ )
				{
					if ( abs( GetYPosOfRegion(i,pObject->pCalcData[k][No+m_nLeftNo],j)-XDPt.y) < SELECT_ON )
					{	
						m_SelectInfo.nSet = pObject->nSet;
						m_SelectInfo.IsMovable = 0;
						m_SelectInfo.bExist = 1;	
						m_SelectInfo.nInRegion = i;
						m_SelectInfo.nInObject = j;	
						m_SelectInfo.nOutNo = k;
						m_SelectInfo.nGSIndex = pObject->nGSIndex;
						return TRUE;
					}
				}
			}
		}	
		else  if ( pObject->nSet == HIS_DAT 
			&& GetYPosOfRegion(i,pObject->pHisDay[No+m_nLeftNo].High,j)-XDPt.y<0
			&& GetYPosOfRegion(i,pObject->pHisDay[No+m_nLeftNo].Low,j)-XDPt.y>0 )
		{
			if ( i==0 && m_bKColor )
			{
				for ( int g=1;g<m_aRegion[0].nObject;g++ )
					if ( m_aRegion[0].aIndexInfo[g]->nSet == KL_OBJECT )
					{
						m_SelectInfo.nSet = KL_OBJECT;
						m_SelectInfo.nOutNo = 0;
						m_SelectInfo.nInObject=g;
						m_SelectInfo.nGSIndex =m_aRegion[0].aIndexInfo[g]->nGSIndex;
						break;
					}
			}
			else
			{
				m_SelectInfo.nSet = pObject->nSet;
				m_SelectInfo.nOutNo = 0;
				m_SelectInfo.nInObject=j;
				m_SelectInfo.nGSIndex =pObject->nGSIndex;
			}
			m_SelectInfo.IsMovable = 0;
			m_SelectInfo.bExist = 1;	m_SelectInfo.nInRegion = i;
			return TRUE;
		}
		////////////////
		if ( XDPt.y>=m_aRegion[i].Area.top && XDPt.y <= m_aRegion[i].DrawRc.top )
		{
			for ( int k=0;k<pObject->nOutNum;k++ )
			{
				for ( int g=0;g<m_aRegion[i].nRect;g++ )
					if ( m_aRegion[i].aRct[g].thObj == j 
						&& m_aRegion[i].aRct[g].thOut == k )
					{
						CRect r(&m_aRegion[i].aRct[g].aRect);
						r.OffsetRect(m_aRegion[i].Area.left,m_aRegion[i].Area.top);
						if ( r.PtInRect(point) )
						{
							m_SelectInfo.nSet = m_aRegion[i].aIndexInfo[j]->nSet;
							m_SelectInfo.nOutNo = k;
							m_SelectInfo.nInObject=j;
							m_SelectInfo.nGSIndex = m_aRegion[i].aIndexInfo[j]->nGSIndex;
							m_SelectInfo.IsMovable = 0;
							m_SelectInfo.bExist = 1;	m_SelectInfo.nInRegion = i;
							return TRUE;
						}
					}
			}
		}
		////////////////////
		if ( XDPt.y >= m_aRegion[i].DrawRc.bottom ) 
		{
			if ( pObject->nSet == GBZS_OBJECT )
			{
				int Item = GetItemNoFromX(XDPt.x);
				if ( pObject->pCalcData[0][m_nLeftNo+Item] > COMPPREC )
				{
					m_SelectInfo.nSet = GBZS_OBJECT;
					m_SelectInfo.nOutNo = 0;
					m_SelectInfo.nInObject=j;
					m_SelectInfo.IsMovable = 0;
					m_SelectInfo.bExist = 1;	m_SelectInfo.nInRegion = i;
					return TRUE;
				}
			}
			if ( pObject->nSet == FC_OBJECT )
			{
				int Item = GetItemNoFromX(XDPt.x);
				if ( pObject->pCalcData[0][m_nLeftNo+Item] > COMPPREC )
				{
					m_SelectInfo.nSet = FC_OBJECT;
					m_SelectInfo.nOutNo = 0;
					m_SelectInfo.nInObject=j;
					m_SelectInfo.IsMovable = 0;
					m_SelectInfo.bExist = 1;	m_SelectInfo.nInRegion = i;
					return TRUE;
				}
			}
			if( pObject->nSet == GGBW_OBJECT )
			{
				int Item = GetItemNoFromX(XDPt.x);
				if ( pObject->pCalcData[0][m_nLeftNo+Item] > COMPPREC )
				{
					m_SelectInfo.nSet = GGBW_OBJECT;
					m_SelectInfo.nOutNo = 0;
					m_SelectInfo.nInObject=j;
					m_SelectInfo.nGSIndex = pObject->pCalcData[1][m_nLeftNo+Item];
					m_SelectInfo.IsMovable = 0;
					m_SelectInfo.bExist = 1;	m_SelectInfo.nInRegion = i;
					return TRUE;
				}
			}
		}
	}//其他
	return FALSE;
}

//不能随意WM_PAINT，尤其是自画线
//m_nAtItem 在左、右鼠标点下时为零,***让m_nAtItem 有效的条件时互斥的，因此MouseMove中可以依据
//m_nAtItem 信息调整位置
int  UFxt::LButtonDown(CDC * pDC,CPoint point)
{
	CPoint	XDPt = point;
	XDPt.x -= left;
	XDPt.y -= top;

	if(m_ViewZoomRect.PtInRect(XDPt))
		m_pView->SendMessage(WM_COMMAND, ID_HIDERIGHT);
	////////////////////////
	BOOL   bAbleRectangle = FALSE;
	m_bLArchor = TRUE;
	m_nAtItem = 0;
	////防止多次右键选中框出现
	if ( m_nRightRgn )
	{
		m_nRightRgn = 0;
		ProcessMsg(WM_PAINT);
	}
	////////////
	if(m_bEvenSelected) 
	{
		m_bEvenSelected = FALSE;
		ProcessMsg(WM_PAINT);
	}
	if ( m_nMousePan )
	{
		for ( int i=0;i<m_nRegion;i++ )
		{
			CRect r(m_aRegion[i].DrawRc);
			memset(&m_ArchorPoint,0,sizeof(POINT));
			memset(&m_OrgPoint ,0,sizeof(POINT));
			r.OffsetRect(left,top);
			if ( r.PtInRect(point) )
			{
				m_nAtRegion = i;
				m_nMousePan |= 0x10000;
				m_nAtItem = GetItemNoFromX(point.x-left)+m_nLeftNo;
				m_dNowVal = GetValFromY(i,point.y-top);
				m_pView->ClientToScreen(&r);
				ClipCursor(&r);
				break;
			}
		}
	}
	else if ( point.y > bottom- FXT_BHEIGHT && point.x < right-FXT_RWIDTH )
	{
		CRect r(left,bottom-FXT_BHEIGHT+1,right-FXT_RWIDTH,bottom);
		m_nAtItem = GetItemNoFromX(point.x-left)+m_nLeftNo;
		m_pView->ClientToScreen(&r);
		ClipCursor(&r);
		if ( !m_bMouseCross && !m_bKeyCross)
			m_KLock  = -1;
	}
	else if ( IsAtEage(XDPt) != -1 )
	{//移轴状态
		m_EageInfo.bAtEage = TRUE;
		memset(&(m_EageInfo.EagePoint),0,sizeof(POINT));
		SetCursor(m_hHorSplit);	
		m_nChgMouse =1;
	}
	else if ( QuickSelect(point) )
	{
		if(m_SelectInfo.nSet==SELFLINE_OBJECT)
		{
			CRect r(m_aRegion[m_SelectInfo.nInRegion].DrawRc);
			r.OffsetRect(left,top);
			//
			m_pView->ClientToScreen(&r);
			ClipCursor(&r);
		}
		else
		{
			CRect r(this);
			m_pView->ClientToScreen(&r);
			ClipCursor(&r);
		}
		ProcessMsg(WM_PAINT);
	}
	else
	{
		if(!m_bDrawLine)
			ProcessMsg(WM_PAINT);
		bAbleRectangle = TRUE;
	}
	//处于画线工具状态
	if(m_bDrawLine)
	{
		m_ToolTip.PopToolTip();
		m_TipInfo.Empty();
		m_ToolTip.UpdateToolTip(m_pView,"");
		//关闭鼠标拖拽时的框选功能
		bAbleRectangle = FALSE;
		//刚开始画线
		if(m_nPtNum == 0)
		{
			++m_nPtNum;
			if(!BeginDrawSelfLine(pDC,point))
				ResetSelfLine();			
		}
		else
		{
			TransSelfLine(point,NEWLINE_ID,m_nPtNum);
			++m_nPtNum;
			if(m_nPtNum>=g_DrawLineTool.GetMaxSetPtNum(m_nWay))	//画完了
			{
				g_DrawLineTool.AddLine(&m_NewDrawLine);
				//
				ResetSelfLine();
				ProcessMsg(WM_PAINT);
			}
		}
	}
	if ( bAbleRectangle )
	{
		for ( int i=0;i<m_nRegion;i++ )
		{
			CRect r(m_aRegion[i].DrawRc);
			memset(&m_ArchorPoint,0,sizeof(POINT));
			memset(&m_OrgPoint ,0,sizeof(POINT));
			r.OffsetRect(left,top);
			if ( r.PtInRect(point) )
			{
				m_pView->ClientToScreen(&r);
				ClipCursor(&r);
				memset(&m_ArchorPoint,0,sizeof(POINT));
				m_bBeginRectangle = TRUE;
				break;
			}
		}
	}
	DrawYRule(pDC,point.x-left,point.y-top);
	m_OrgPoint = point;
	m_nDownPoint = point;
	return 1;
}

int  UFxt::LButtonUp(CDC * pDC,CPoint point)
{
	CPoint	XDPt = point;
	XDPt.x -= left;
	XDPt.y -= top;
	////////////////////////
	if(!m_bDrawLine)
		ClipCursor(NULL);
	if ( m_nMousePan || m_nAtItem>0 )
	{
		m_nAtRegion = 0;
		if ( m_nMousePan )	//如果不是m_nMousePan状态，不可随意打开这个开关!
			m_nMousePan = 1;
		m_nAtItem = 0;
	}
	else if ( m_nChgMouse || m_EageInfo.bAtEage )
	{
		m_nChgMouse--;
		if ( m_EageInfo.bAtEage )
		{
			SetMyAxis();
			m_EageInfo.bAtEage = FALSE;
		}
		SetCursor(LoadCursor(NULL,IDC_ARROW));
	}
	if ( m_bBeginRectangle  )
	{
		m_bBeginRectangle = FALSE;
		////////////////////////
		if ( m_bLArchor && //如果是右键画框
			m_ArchorPoint.x+m_ArchorPoint.y>0 && abs(m_ArchorPoint.x - m_OrgPoint.x) > 2*SELECT_ON )
		{
			int nI = GetItemNoFromX(m_OrgPoint.x-left)-GetItemNoFromX(point.x-left);
			double dr = nI/(double)m_nWillNum+1.0;
			//精确放大,注意:与最后面保持的3个空白处有关
			if ( nI <0 && -nI >= 5 )
			{
				m_nLeftNo += GetItemNoFromX(m_OrgPoint.x-left);
				m_nLastNo = m_nLeftNo + (-nI);
				if(m_nLastNo==m_nDataNum-1)
				{
					m_nLeftNo-=4;
					nI-=4;
				}
				else
				{
					m_nLeftNo-=1;
					m_nLastNo-=1;
				}	
				m_nWillNum = -nI;
				m_KLock = -1;
			}
			CalcAllRatio();
			CalcRuleDrawAxis();		//重新计算X、Y轴的坐标
			//修改：如果在if外面，会导致Title显示的是最后一天的数据
			ProcessMsg(WM_PAINT);
		}
	}
	//////////////////////////////////////
	if(m_bDrawLine)
	{
		m_Point[1] = point;
		TransSelfLine(m_Point[1],NEWLINE_ID,1);
		++m_nPtNum;
		if(m_nWay == ID_TXTTOOLS)
		{
			ClipCursor(NULL);
			m_bDrawLine		= FALSE;
			CEditTxtDlg EditTxt;
			if(EditTxt.DoModal() == IDOK && EditTxt.m_strTxt.GetLength()>0)
			{
				strncpy(m_NewDrawLine.szBuffer,EditTxt.m_strTxt,200);
				m_NewDrawLine.szBuffer[200]=0;
				m_NewDrawLine.FontSize=g_nTxtFontSize;
				m_NewDrawLine.tversion = 1;
				m_NewDrawLine.tcrf = EditTxt.m_color;
				m_NewDrawLine.nUsePoint = (pDC->GetTextExtent(EditTxt.m_strTxt)).cx;
			}
		}
		if(m_nPtNum>=g_DrawLineTool.GetMaxSetPtNum(m_nWay))
		{
			g_DrawLineTool.AddLine(&m_NewDrawLine);
			//
			ResetSelfLine();
			ClipCursor(NULL);
			ProcessMsg(WM_PAINT);
		}
	}
	else if ( m_SelectInfo.IsMovable )
	{	//有可移动的选中对象
		m_SelectInfo.IsMovable = 0;
		CRect tmpRc;
		BOOL  bExist = 0;
		if ( m_SelectInfo.nSet == SELFLINE_OBJECT )
		{	//如果有自画线选中
			if ( m_SelectInfo.bMoved )	//被移动过
			{
				TransSelfLine(m_Point[0],m_SelectInfo.nSelfLineNo,0);
				TransSelfLine(m_Point[1],m_SelectInfo.nSelfLineNo,1);
				TransSelfLine(m_Point[2],m_SelectInfo.nSelfLineNo,2);
			}
			ResetSelfLine();	//基本变量复位
			ProcessMsg(WM_PAINT);
			m_bLArchor = FALSE;
			return 1;
		}
		TOBJECT * pObject = m_aRegion[m_SelectInfo.nInRegion].aIndexInfo[m_SelectInfo.nInObject];
		for ( int i=0;i<m_nRegion;i++ )
		{	tmpRc = m_aRegion[i].Area;
			if ( point.y > bottom-FXT_BHEIGHT && point.x < right 
				&& pObject->nSet != HIS_DAT )
			{	//落在时间轴上
				PushLastInfo(pObject);
				PopLastInfo(m_aRegion[m_nRegion].aIndexInfo[0]);
				m_aRegion[m_nRegion].nObject++;
				RejustOneObject(m_aRegion[m_nRegion].aIndexInfo[0],m_nDataNum);
				CalcOne(m_aRegion[m_nRegion].aIndexInfo[0]);
				CalcOneGSRatio(m_aRegion[m_nRegion].aIndexInfo[0],(m_nLastNo+1)-m_nDrawNum);
				CalcOneRegionRatio(m_nRegion);
				m_nRegion++;
				m_bChgRegion = TRUE;	//改变区域大小
				if ( !m_SelectInfo.bAdd )
					RegionDelOne(m_SelectInfo.nInRegion,pObject);
				ProcessMsg(WM_PAINT);
				break;
			}
			if ( !tmpRc.PtInRect(XDPt) )	
				continue;
			/////////////////////////////
			for ( int j=0;j< m_aRegion[i].nObject;j++ )
			{	if ( pObject == m_aRegion[i].aIndexInfo[j] )
				{	bExist = 1;		//如果已经有，不要
					break;		
				}
			}
			if ( !bExist  && m_SelectInfo.nSet != HIS_DAT  )
			{	//按理应当计算，但是移动过来的，一定是存在的 
				if(m_aRegion[i].nObject>=MAX_LINEOUT) break ;
				m_aRegion[i].aIndexInfo[m_aRegion[i].nObject] = AddObjectByIndex( pObject->nSet,pObject->nGSIndex);
				m_aRegion[i].nObject ++;
				if ( !m_SelectInfo.bAdd )
					RegionDelOne(m_SelectInfo.nInRegion,pObject);
				m_bChgRatio = TRUE;	//保险起见
				ProcessMsg(WM_PAINT);
				break;
			}
		}
	}
	/*
	else
	{
		CNewMenu menu;
		menu.LoadMenu(IDR_RIGHTMENU_COM);
		CNewMenu *pMenu = (CNewMenu *)menu.GetSubMenu(ID_PERIOD);
		m_pView->ClientToScreen(&point);
		pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
	}
	*/
	m_bLArchor = FALSE;
	return 1;
}

int  UFxt::LButtonDblClk(CDC * pDC,CPoint point)
{
	CPoint	XDPt = point;
	XDPt.x -= left;
	XDPt.y -= top;
	////////////////////////

	if ( m_MainYRc.PtInRect(XDPt) && !m_nInCom2) //双击主图轴进入走势图
	{
		g_pMainWin->PostMessage(WM_COMMAND,(g_bTickTStatus?ID_508:ID_501));	
		return 1;
	}
	for ( int i=0;i<m_nRegion;i++ )
	{
		CRect r(m_aRegion[i].Area);
		if ( r.PtInRect(XDPt) ) break;
	}
	//如果在主图中
	if(i==0)
	{
		if(QuickSelect(point) && m_SelectInfo.nSet==HIS_DAT && m_SelectInfo.nInObject==0)
		{
			m_bMouseCross = 1;
			memset(&m_OldMousePoint,0,sizeof(POINT));
			m_RealMove = m_OldMousePoint;
			m_KLock = -1;
			MouseMove(pDC,point);
			ProcessMsg(UM_HISDLG,0);
			return 1;
		}
		if(m_bMouseCross) ProcessMsg(WM_KEYDOWN,VK_ESCAPE);
		else
		{
			m_bMouseCross = 1;
			if( !m_pInfoDlg ) CreateInfoDlg();
			memset(&m_OldMousePoint,0,sizeof(POINT));
			m_RealMove = m_OldMousePoint;
			m_KLock = -1;
			MouseMove(pDC,point);
		}
	}
	else if(i<m_nRegion)
	{
		if(!m_bDbClickFT && m_nRegion>2)
		{
			m_bDbClickFT=TRUE;
			m_nOldSelRegion = m_nSelRegion;
			strcpy(m_strDbSaveZb,m_aRegion[i].aIndexInfo[0]->acCode);
			ProcessMsg(UM_SELNUM,2,1);
		}
		else if(m_bDbClickFT)
		{
			m_bDbClickFT=FALSE;
			strcpy(m_aRegion[i].aIndexInfo[0]->acCode,m_strDbSaveZb);
			ProcessMsg(UM_SELNUM,m_nOldRegion,1);
			m_nSelRegion=m_nOldSelRegion;
			ProcessMsg(WM_PAINT);
		}
	}
	return 1;
}

int  UFxt::RButtonDblClk(CDC * pDC,CPoint point)
{
	CPoint	XDPt = point;
	XDPt.x -= left;
	XDPt.y -= top;
	return 1;
}

int UFxt::RButtonDown(CDC *pDC, CPoint point)
{
	CPoint	XDPt = point;
	XDPt.x -= left;
	XDPt.y -= top;
	////////////////////////
	m_bRArchor  = TRUE;
	m_bRArchor = TRUE;
	m_nAtItem   = 0;
	if ( m_nMousePan )
	{
		for ( int i=0;i<m_nRegion;i++ )
		{
			CRect r(m_aRegion[i].DrawRc);
			memset(&m_ArchorPoint,0,sizeof(POINT));
			memset(&m_OrgPoint ,0,sizeof(POINT));
			r.OffsetRect(left,top);
			if ( r.PtInRect(point) )
			{
				m_nAtRegion = i;
				m_nMousePan |= 0x10000;
				m_nAtItem = GetItemNoFromX(point.x-left)+m_nLeftNo;
				m_dNowVal = GetValFromY(i,point.y);
				m_pView->ClientToScreen(&r);
				ClipCursor(&r);
				break;
			}
		}
	}
	else if ( point.x < right-FXT_RWIDTH && point.y > bottom- FXT_BHEIGHT )
	{
		CRect r(left,bottom-FXT_BHEIGHT,right-FXT_RWIDTH,bottom);
		m_nAtItem = GetItemNoFromX(point.x-left)+m_nLeftNo;
		m_pView->ClientToScreen(&r);
		ClipCursor(&r);
	}
	else
	{
		if ( m_nRightRgn )
		{
			m_nRightRgn = 0;
			ProcessMsg(WM_PAINT);
		}
		for ( int i=0;i<m_nRegion;i++ )
		{
			CRect r(m_aRegion[i].DrawRc);
			memset(&m_ArchorPoint,0,sizeof(POINT));
			memset(&m_OrgPoint ,0,sizeof(POINT));
			r.OffsetRect(left,top);
			if ( r.PtInRect(point) )
			{
				m_pView->ClientToScreen(&r);
				ClipCursor(&r);
				memset(&m_ArchorPoint,0,sizeof(POINT));
				m_bBeginRectangle = TRUE;
				m_nRightRgn = 1;
				break;
			}
		}
	}
	m_nDownPoint = point;
	m_OrgPoint = point;
	return 1;
}

int UFxt::RButtonUp(CDC *pDC, CPoint point)
{
	CPoint	XDPt = point;
	XDPt.x -= left;
	XDPt.y -= top;
	////////////////////////
	CNewMenu menu, *pMenu = NULL;
	int nType, nBakup;
	ClipCursor(NULL);
	//////////////////////////////
	if ( m_nMousePan || m_nAtItem>0 )
	{
		m_nAtRegion = 0;
		if ( m_nMousePan )	//如果不是m_nMousePan状态，不可满然打开这个开关!
			m_nMousePan = 1;
		m_nAtItem = 0;
		m_bRArchor = FALSE;
		return 0;
	}
	//////////////////////////////
	BOOL  bMoved = 0;
	menu.LoadMenu(IDR_RIGHTMENU_COM);
//	menu.LoadToolBar(IDR_MAINFRAME);
	if ( m_bBeginRectangle  )
	{
		m_bBeginRectangle = FALSE;
		if ( m_bRArchor && m_ArchorPoint.x+m_ArchorPoint.y>0 && abs(m_ArchorPoint.x - m_OrgPoint.x) > 2*SELECT_ON )
		{
			pMenu = (CNewMenu *)menu.GetSubMenu(ID_MOVE);
			bMoved = TRUE;
		}
	}
	if ( !bMoved )
	{
		nType = ChoiceType(point);
		if (nType > -1)
		{
			if (m_SelectInfo.nSet == SELFLINE_OBJECT && m_SelectInfo.IsMovable) 
				m_SelectInfo.IsMovable = FALSE;
			ProcessMsg(WM_PAINT);
			nBakup = nType;
			if (nType == ID_SELECTEXPZS || nType == ID_SELECTKZS)
				nType = ID_SELECTNORMAL;
			pMenu = (CNewMenu *)menu.GetSubMenu(nType);
			if (nBakup == ID_SELECTEXPZS)
			{
				pMenu->ModifyMenu(ID_DELZB,MF_BYCOMMAND,ID_DELZB,_F("删除交易系统指示"));
				pMenu->ModifyMenu(ID_MODIFY_FORMULA,MF_BYCOMMAND,ID_MODIFY_FORMULA,_F("修改交易系统公式"));
			}
			else if (nBakup == ID_SELECTKZS)
			{
				pMenu->ModifyMenu(ID_DELZB,MF_BYCOMMAND,ID_DELZB,_F("删除五彩K线"));
				pMenu->DeleteMenu(ID_MODIFY_FORMULA, MF_BYCOMMAND);
			}
			if(g_nFmlPack>0)
			{
				CNewMenu *pSubMenu = NULL;
				pSubMenu = (CNewMenu *)pMenu->GetSubMenu(4);
				if(pSubMenu)
				{
					pSubMenu->InsertMenu(-1,MF_BYPOSITION|MF_SEPARATOR);
					for(int i=0;i<g_nFmlPack;i++)
						pSubMenu->InsertMenu(-1,MF_BYPOSITION,ID_FMLPACK00+i,g_FmlPack[i].aPackName);
				}
			}
		}
	}
	if (pMenu == NULL) return -1;
	if ( THUN_OBJECT == m_SelectInfo.nSet )
		return 1;
	ProcessMsg(WM_PAINT);
	m_pView->ClientToScreen(&point);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
	m_bRArchor   = FALSE;
	if (nType == -1)	return -1;
	else	return 1;
}

int UFxt::ChoiceType(CPoint point)
{
	int i;
	CPoint	XDPt = point;
	XDPt.x -= left;
	XDPt.y -= top;
	////////////////////////
	CRect rect, rcAxis;
	rcAxis = m_aRegion[0].Area;
	rcAxis.left = m_aRegion[0].Area.right;
	rcAxis.right = m_rcIn.right;
	if (m_PeriodRc.PtInRect(XDPt))
		return ID_PERIOD;
	for (i = 0; i < m_nRegion; i++)
	{
		m_nSelRegionBak = i;
		rect = m_aRegion[i].Area;
		if (rcAxis.PtInRect(XDPt))
			return ID_AXIS;
		if (rect.PtInRect(XDPt))
		{
			BOOL bSel;
			bSel = QuickSelect(point);
			if (bSel)
			{
				if (m_SelectInfo.nSet == SELFLINE_OBJECT)
					return ID_DRAWLINE;
				if (m_SelectInfo.nSet == GBZS_OBJECT || m_SelectInfo.nSet == GGBW_OBJECT || m_SelectInfo.nSet == FC_OBJECT)
					return ID_MAIN;
				if (m_bExistZS)
				{
					if (m_SelectInfo.nSet == EXP_OBJECT)
						return ID_SELECTEXPZS;
					if (m_SelectInfo.nSet == KL_OBJECT)
						return ID_SELECTKZS;
				}
				if (i == 0 && m_SelectInfo.nSet == HIS_DAT)
				{
					if (strcmp(m_aRegion[0].aIndexInfo[m_SelectInfo.nInObject]->acCode, "OVERLAPK") == 0)
						return ID_DELOVERLAPK;
					else
						return ID_MAIN;
				}
				return ID_SELECTNORMAL;
			}
			if (i == 0)
				return ID_MAIN;
			else
				return ID_ANY;
		}
	}
	return -1;
}

#define UPDATE_COMPPREC	0.001

void UFxt::UpdateData(BOOL bForceCalcZB)
{
	DWORD	Saved_Volume = 0;
	int i=0;
	if ( m_nDataNum <=0 )	//加补丁，如果没有K线，则不维护
		return ;
	if ( m_Hq.Close < UPDATE_COMPPREC ) //如果昨收为0(今天无此股票),也不维护
		return ;
	//昨天的行情不用于K线维护
	if(g_pStockIO->BeDSStock(m_pStkInfo)&&m_HqEx.HqDate!=0&&g_iToday!=m_HqEx.HqDate) 
		return;
	TOBJECT * p = m_pObject;
	int Num = GetMinuteXh(m_nPeriod,TimeSec,g_nMinnNum);
	if(m_bDSFlag) 
	{
		Num = GetDSMinute(m_pTimer,FALSE);
		int tmpRatio = anFZRatio[m_nPeriod];
		if(m_nPeriod == PER_MINN)	tmpRatio = g_nMinnNum;
		Num =  ( Num + tmpRatio - 1 )/( tmpRatio );
	}
	int Total_Num = GetRealMinuteXh(m_nPeriod,TimeSec,g_nMinnNum);
	if (  (m_nPeriod == PER_MIN5 
		|| m_nPeriod == PER_MIN15 
		|| m_nPeriod == PER_MIN30 
		|| m_nPeriod == PER_HOUR 
		|| m_nPeriod == PER_MIN1 
		|| m_nPeriod == PER_MINN) && 
		Num <= Total_Num &&
		(g_nTQFlag != 2&&(g_nTQFlag!=3 || g_nPartTq!=1))
	   )
	{
		TransAnalyData(0);
		if(Num > m_nTodayNum)
		{
			m_nDataNum ++;

			int tmpfznum = 1;
			if(m_nPeriod==PER_MIN1)			tmpfznum = 1;
			else if(m_nPeriod==PER_MINN)	tmpfznum = g_nMinnNum;
			else							tmpfznum = fztb[m_nPeriod]*5;
			time_t mytimet=time(NULL);
			tm * mytm = localtime(&mytimet);
			m_pObject->pHisDay[m_nDataNum-1].Time.Daye.Minute = GetMinuteFromFZ(m_nTodayNum,tmpfznum,TimeSec);
			if(m_bDSFlag) 
				m_pObject->pHisDay[m_nDataNum-1].Time.Daye.Minute = GetCorrespondingMin(TRUE);
			m_pObject->pHisDay[m_nDataNum-1].Open = 
			m_pObject->pHisDay[m_nDataNum-1].High =
			m_pObject->pHisDay[m_nDataNum-1].Close=
			m_pObject->pHisDay[m_nDataNum-1].Low  = m_Hq.Now;
			m_pObject->pHisDay[m_nDataNum-1].Volume = 0;
			m_pObject->pHisDay[m_nDataNum-1].Amount = 0;
			m_pObject->pHisDay[m_nDataNum-1].Time.Daye.Day = (mytm->tm_mon+1)*100+mytm->tm_mday;
			m_nTodayNum++;
			//多加一个K线，才开始重新计算指标
			while(p)
			{	p->nArrayLen = m_nDataNum;
				p = p->next;
				if ( p &&  p->nSet == HIS_DAT )
				{//非第一个
				}
			}
			TransAnalyData(1);
			RawData2CalcData(m_pStkInfo->Code,m_pStkInfo->setcode,m_nPeriod,m_pObject->pHisDay,m_pHisData,m_nDataNum,g_NoTime);
			CalcAllGS();
			
			if ( m_nLeftNo + m_nDrawNum +1 >= m_nDataNum)
			{
				CalcAllRatio(TRUE,TRUE);
				XPan(1,TRUE); //
			}
			//只需要在这里加就行了,其他地方可以暂不管
			CalcRuleDrawAxis();
		}	
		else if(Num == m_nTodayNum)
		{
			if ( m_Hq.Now > UPDATE_COMPPREC )
			{
				m_pObject->pHisDay[m_nDataNum-1].High = max(m_pObject->pHisDay[m_nDataNum-1].High,m_Hq.Now);
				m_pObject->pHisDay[m_nDataNum-1].Low = min(m_pObject->pHisDay[m_nDataNum-1].Low,m_Hq.Now);
				m_pObject->pHisDay[m_nDataNum-1].Close= m_Hq.Now;
			}
			//维护成交量
			int ii;
			long tmpvolume = m_pObject->pHisDay[m_nDataNum-1].Volume;
			long oldvolume = 0;
			if(m_bDSFlag)
			{
				ii = 0;
				while(m_pObject->pHisDay[m_nDataNum-2-ii].Time.Daye.Day==m_pObject->pHisDay[m_nDataNum-1].Time.Daye.Day)
				{
					oldvolume+=m_pObject->pHisDay[max(0,m_nDataNum-2-ii)].Volume;
					++ii;
				}
			}
			else
			{
				for(ii=0;ii<m_nTodayNum-1;ii++)
					oldvolume+=m_pObject->pHisDay[max(0,m_nDataNum-2-ii)].Volume;
			}
			if(!testzs(m_pStkInfo))
			{	
				if(need_justcjl(m_pStkInfo->setcode,m_pStkInfo->Code))
					m_pObject->pHisDay[m_nDataNum-1].Volume = max(0,(long)(m_pStkInfo->Unit*m_Hq.Volume-oldvolume)); //两DWORD相差,结果也为DWORD,必须转为long
				else
					m_pObject->pHisDay[m_nDataNum-1].Volume = max(0,(long)(m_Hq.Volume-oldvolume)); //两DWORD相差,结果也为DWORD,必须转为long
			}
			else
				m_pObject->pHisDay[m_nDataNum-1].Volume = max(0,(long)(m_Hq.Amount/100-oldvolume));
			float tmpamount = m_pObject->pHisDay[m_nDataNum-1].Amount;			
			float oldamount = 0.0;
			if(m_bDSFlag)
			{
				ii = 0;
				while(m_pObject->pHisDay[m_nDataNum-2-ii].Time.Daye.Day==m_pObject->pHisDay[m_nDataNum-1].Time.Daye.Day)
				{
					oldamount+=m_pObject->pHisDay[max(0,m_nDataNum-2-ii)].Amount;
					++ii;
				}
			}
			else
			{
				for(ii=0;ii<m_nTodayNum-1;ii++)
					oldamount+=m_pObject->pHisDay[max(0,m_nDataNum-2-ii)].Amount;				
			}
			m_pObject->pHisDay[m_nDataNum-1].Amount = max(0,m_Hq.Amount-oldamount);
			if(m_bDSFlag&&m_pStkInfo->bigmarket==QH_TYPE)
				m_pObject->pHisDay[m_nDataNum-1].QHVolInStock = m_Hq.Yield_VolInStock.VolInStock;
			/////////////////////////////////////
			if(tmpvolume != m_pObject->pHisDay[m_nDataNum-1].Volume)
			{
				TransAnalyData(1);
				RawData2CalcData(m_pStkInfo->Code,m_pStkInfo->setcode,m_nPeriod,m_pObject->pHisDay,m_pHisData,m_nDataNum,g_NoTime);
				CalcAllGS();
				if ( m_nLeftNo + m_nDrawNum +1 >= m_nDataNum)
				{
					TOBJECT * p = m_pObject;
					while ( p )
					{	CalcOneGSRatio(p,m_nLeftNo);
						p = p->next;
					}
					//////////////////////////////////////////////
					for ( int i=0;i<m_nRegion;i++ )	//每个区域,考虑关系
						CalcOneRegionRatio(i);
				}
			}
			else TransAnalyData(1);
		}
	}
	else if ((g_nTQFlag != 2&&(g_nTQFlag!=3 || g_nPartTq!=1))
		 && (m_nPeriod == PER_DAYN
			|| m_nPeriod ==PER_DAY
			|| m_nPeriod ==PER_MONTH 
			|| m_nPeriod ==PER_WEEK 
			|| m_nPeriod ==PER_SEASON 
			|| m_nPeriod ==PER_YEAR )
		)
	{
		TransAnalyData(0);
		if(m_pLastStkInfo != m_pStkInfo || m_nPeriod == PER_DAY)
		{
			m_pLastStkInfo = m_pStkInfo;
			m_LastVolume = m_Hq.Volume;
			m_LastAmount = m_Hq.Amount;
		}
		Saved_Volume = m_pObject->pHisDay[m_nDataNum-1].Volume;
		//开市后由无到有的变化 因为开始将为0的全部过滤了
		DWORD dwNowDate = g_lOpenRq;
		if(g_pStockIO->BeDSStock(m_pStkInfo)&&m_HqEx.HqDate>0) dwNowDate = m_HqEx.HqDate; 
		if ( m_nPeriod == PER_DAY && dwNowDate != m_pObject->pHisDay[m_nDataNum-1].Time.Date )
		{
			++m_nDataNum;
			m_pObject->pHisDay[m_nDataNum-1].Time.Date = dwNowDate;
		}
		if( m_nPeriod == PER_DAY && (m_Hq.Now < UPDATE_COMPPREC || m_Hq.Open < UPDATE_COMPPREC || m_Hq.Max < UPDATE_COMPPREC || m_Hq.Min < UPDATE_COMPPREC) )
		{
			if(m_Hq.Open > UPDATE_COMPPREC) //如果四个价位中有一个为0,且开盘不为0,用开盘平滑,否则用昨收
			{
				m_pObject->pHisDay[m_nDataNum-1].Open   = m_Hq.Open;
				m_pObject->pHisDay[m_nDataNum-1].Close  = m_Hq.Now<UPDATE_COMPPREC?m_Hq.Open:m_Hq.Now;
				m_pObject->pHisDay[m_nDataNum-1].High   = m_Hq.Max<UPDATE_COMPPREC?m_Hq.Open:m_Hq.Max;				
				m_pObject->pHisDay[m_nDataNum-1].Low    = m_Hq.Min<UPDATE_COMPPREC?m_Hq.Open:m_Hq.Min;
			}
			else
			{
				float ftmp = m_Hq.Now;
				if(ftmp < UPDATE_COMPPREC) ftmp = m_Hq.Close;
				m_pObject->pHisDay[m_nDataNum-1].Open   = m_Hq.Open<UPDATE_COMPPREC?ftmp:m_Hq.Open;
				m_pObject->pHisDay[m_nDataNum-1].Close  = m_Hq.Now<UPDATE_COMPPREC?ftmp:m_Hq.Now;
				m_pObject->pHisDay[m_nDataNum-1].High   = m_Hq.Max<UPDATE_COMPPREC?ftmp:m_Hq.Max;
				m_pObject->pHisDay[m_nDataNum-1].Low    = m_Hq.Min<UPDATE_COMPPREC?ftmp:m_Hq.Min;
			}
			//对最高和最低进行容错
			m_pObject->pHisDay[m_nDataNum-1].High	= max(m_pObject->pHisDay[m_nDataNum-1].High,m_pObject->pHisDay[m_nDataNum-1].Open);
			m_pObject->pHisDay[m_nDataNum-1].High	= max(m_pObject->pHisDay[m_nDataNum-1].High,m_pObject->pHisDay[m_nDataNum-1].Close);
			m_pObject->pHisDay[m_nDataNum-1].Low    = min(m_pObject->pHisDay[m_nDataNum-1].Low,m_pObject->pHisDay[m_nDataNum-1].Open);
			m_pObject->pHisDay[m_nDataNum-1].Low    = min(m_pObject->pHisDay[m_nDataNum-1].Low,m_pObject->pHisDay[m_nDataNum-1].Close);
		}
		else if(m_Hq.Now > UPDATE_COMPPREC &&  m_Hq.Open > UPDATE_COMPPREC && m_Hq.Max > UPDATE_COMPPREC && m_Hq.Min > UPDATE_COMPPREC)
		{
			if(m_nPeriod == PER_DAY)
			{
				m_pObject->pHisDay[m_nDataNum-1].Open	= m_Hq.Open;
				m_pObject->pHisDay[m_nDataNum-1].High   = m_Hq.Max;
				m_pObject->pHisDay[m_nDataNum-1].Low    = m_Hq.Min;
			}
			else
			{
				m_pObject->pHisDay[m_nDataNum-1].High   = max(m_pObject->pHisDay[m_nDataNum-1].High,m_Hq.Max);
				m_pObject->pHisDay[m_nDataNum-1].Low    = min(m_pObject->pHisDay[m_nDataNum-1].Low,m_Hq.Min);
			}
			m_pObject->pHisDay[m_nDataNum-1].Close  = m_Hq.Now;
			//对最高和最低进行容错
			m_pObject->pHisDay[m_nDataNum-1].High	= max(m_pObject->pHisDay[m_nDataNum-1].High,m_pObject->pHisDay[m_nDataNum-1].Close);
			m_pObject->pHisDay[m_nDataNum-1].Low    = min(m_pObject->pHisDay[m_nDataNum-1].Low,m_pObject->pHisDay[m_nDataNum-1].Close);
		}

		if(m_nPeriod == PER_DAY)
		{
			if(!IsLocalStock(m_pStkInfo) && !testzs(m_pStkInfo) && need_justcjl(m_pStkInfo->setcode,m_pStkInfo->Code))
				m_pObject->pHisDay[m_nDataNum-1].Volume = m_pStkInfo->Unit*m_Hq.Volume;
			else
				m_pObject->pHisDay[m_nDataNum-1].Volume = m_Hq.Volume;
			if(m_pStkInfo->bigmarket!=QH_TYPE) 
				m_pObject->pHisDay[m_nDataNum-1].Amount = m_pStkInfo->AmoUnit*m_Hq.Amount;
			else m_pObject->pHisDay[m_nDataNum-1].QHVolInStock = m_Hq.Yield_VolInStock.VolInStock;
		}
		else		//周月线等周期还是要处理一下的
		{
			if(m_Hq.Volume > m_LastVolume)
			{
				int itmp = GetStockType(m_pStkInfo->setcode,m_pStkInfo->Code);
				BOOL bSpec = UseSpecVolumeUnit(itmp,m_nPeriod);
				if (bSpec)
				{
					if(m_bQz)
						m_pObject->pHisDay[m_nDataNum-1].Volume += (m_Hq.Volume-m_LastVolume)/100;
					else
						m_pObject->pHisDay[m_nDataNum-1].Volume += m_pStkInfo->Unit*(m_Hq.Volume-m_LastVolume)/100;
				}
				else
				{
					if(!testzs(m_pStkInfo) && need_justcjl(m_pStkInfo->setcode,m_pStkInfo->Code))
						m_pObject->pHisDay[m_nDataNum-1].Volume += m_pStkInfo->Unit*(m_Hq.Volume-m_LastVolume);
					else
						m_pObject->pHisDay[m_nDataNum-1].Volume += (m_Hq.Volume-m_LastVolume);
				}
				m_LastVolume = m_Hq.Volume;
			}
			m_pObject->pHisDay[m_nDataNum-1].Amount += m_Hq.Amount-m_LastAmount;
			if(m_pStkInfo->bigmarket==QH_TYPE)										//期货维护最新的持仓量
				m_pObject->pHisDay[m_nDataNum-1].QHVolInStock = m_Hq.Yield_VolInStock.VolInStock;
			m_LastAmount = m_Hq.Amount;
		}
		if ( bForceCalcZB || m_pObject->pHisDay[m_nDataNum-1].Volume != Saved_Volume )
		{
			TransAnalyData(1);
			RawData2CalcData(m_pStkInfo->Code,m_pStkInfo->setcode,m_nPeriod,m_pObject->pHisDay,m_pHisData,m_nDataNum,g_NoTime);
			CalcAllGS();
			if ( m_nLeftNo + m_nDrawNum +1 >= m_nDataNum)
			{
				//下面两行是为了防止延时行情最后一根K线不显示
				CalcAllRatio(TRUE,TRUE);
				XPan(1,TRUE); //

				TOBJECT * p = m_pObject;
				while ( p )
				{	CalcOneGSRatio(p,m_nLeftNo);
					p = p->next;
				}
				//////////////////////////////////////////////
				for ( int i=0;i<m_nRegion;i++ )	//每个区域,考虑关系
					CalcOneRegionRatio(i);
			}
		}
		else TransAnalyData(1);
	}
	if(m_nKeyNo < m_nDataNum)
	{
		if(m_bKeyCross) 
		{
			FillDlgInfo(m_nKeyNo);
			PopupInfoDlg();
		}
		if(m_bMouseCross) 
		{
			FillDlgInfo(GetItemNoFromX(m_RealMove.x-left)+m_nLeftNo);
			PopupInfoDlg();
		}
	}
}

void UFxt::SetPeriod(BYTE nPeriod,BOOL bReadData,BOOL bReDraw,BOOL bCommon)
{
	if(OnLine && m_bReqing)	
		return;
	if(!IsValidPeriod(nPeriod))
		return;
	if(bCommon) 
		m_nCommonPeriod = nPeriod;
	m_nPeriod		= nPeriod;
	m_nCalcPeriod	= TdxPeriodToCalcPeriod(m_nPeriod);
	m_PeriodKeyOrder= TdxPeriodToKeyOrder(m_nPeriod);
	//
	FillCbInfo(m_OldKeyPoint.y,m_nLeftNo);
	m_pView->SendMessage(UM_REFLECT,Reflect_Tv_Period);
	//
	if(m_pView->IsKindOf(RUNTIME_CLASS(CComView)))
		((CComView*)m_pView)->SetCurrPeriod(nPeriod);
	if(!bReadData) return;
	if(OnLine && !UseLocaldata())
	{
		ResetData();
		//请求时,说明是第一次 以便将一些变量复位
		if(!ReadData(FALSE))
			return;
	}
	else
	{
		ReadData();
		CalcAllRatio();
		m_bChgRatio = TRUE;			//改变比例
	}
	FillCbInfo(bottom,m_nLastNo);
	m_pView->SendMessage(UM_REFLECT,Reflect_Cb_FxtStart);
	if(bReDraw) ProcessMsg(WM_PAINT);
}

void UFxt::SetCom2Fml(LPFMLPACK pCom2Fml,BOOL bDrawMain)
{
	int i=0;
	if(pCom2Fml)
	{
		if(pCom2Fml->lRectNum<=0 || pCom2Fml->lRectNum>10) return;
		if(pCom2Fml->aFormular[0][0]!=0 && bDrawMain)
		{
			strncpy(m_strCom2ZbMain,pCom2Fml->aFormular[0],ZBNAME_LEN-1);
			m_strCom2ZbMain[ZBNAME_LEN-1] = 0;
			g_CF.TrimStr(m_strCom2ZbMain);
			m_Com2DrawMain = TRUE;
		}
		else m_Com2DrawMain = FALSE;
		for(i=1;i<pCom2Fml->lRectNum;i++)
		{
			strncpy(m_strCom2FavUseZb[i-1],pCom2Fml->aFormular[i],ZBNAME_LEN-1);
			m_strCom2FavUseZb[i-1][ZBNAME_LEN-1] = 0;
			g_CF.TrimStr(m_strCom2FavUseZb[i-1]);
		}
		m_Com2WinNum	= pCom2Fml->lRectNum;
		if(m_Com2WinNum<=0) m_Com2WinNum = 1;
		if(m_nInCom2)
		{
			m_bUseFmlPack = TRUE;
			m_NormalRegion= m_Com2WinNum;
			if(!m_bUseFmlPack || m_lBandPackNo<0 || m_lBandPackNo>=g_nFmlPack)
				SetPeriod(pCom2Fml->nPeriod,FALSE,FALSE,TRUE);
			ProcessMsg(UM_UNUSEFMLPACK);
		}
	}
}

void UFxt::GetCom2Fml(LPFMLPACK pCom2Fml)
{
	int i=0,j=0;
	int nRegion = 0;
	if(pCom2Fml)
	{
		memset(pCom2Fml,0,sizeof(FMLPACK));
		if(m_bDbClickFT) nRegion = m_nOldSelRegion;
		else nRegion = m_nRegion;
		for(i=0;i<nRegion;i++)
		{
			for(j=0;j<m_aRegion[i].nObject;j++)
			{
				if(m_aRegion[i].aIndexInfo[j]->nSet==ZB_TYPE) break;
			}
			if(j<m_aRegion[i].nObject)
				strncpy(pCom2Fml->aFormular[i],m_aRegion[i].aIndexInfo[j]->acCode,31);
		}
		pCom2Fml->lRectNum	= nRegion;
		pCom2Fml->nPeriod	= m_nPeriod;
	}
}

void UFxt::ProcessCommand(WPARAM wParam, LPARAM lParam)
{
	CNewMenu menu, *pMenu = NULL, *pMenu1 = NULL;
	if (HIWORD(wParam) == 0)	//菜单消息
	{
		switch (LOWORD(wParam))
		{
		case ID_DEL:
			if(g_DrawLineTool.GetShowFlag() && m_SelectInfo.nSet == SELFLINE_OBJECT
				&& m_SelectInfo.nSelfLineNo >= 0 && m_SelectInfo.nSelfLineNo < g_DrawLineTool.GetLineNum())
			{
				g_DrawLineTool.DeleteLine(m_SelectInfo.nSelfLineNo);
				g_DrawLineTool.CleanDel();
				memset(&m_SelectInfo,0,sizeof(FXTSELINFO));
				ProcessMsg(WM_PAINT);	//wParam=1要读数据 =2仅更新全局位图
			}
			break;
		case ID_INDEXHELP:
			{
				if (m_SelectInfo.bExist)
				{
					TOBJECT *tmp;
					tmp = m_aRegion[m_SelectInfo.nInRegion].aIndexInfo[m_SelectInfo.nInObject];
					CDescZBDlg dlg;
					dlg.Init(tmp->nSet, tmp->acCode);
					dlg.DoModal();
				}
				else ProcessZBInRegion(ID_INDEXHELP);
			}
			break;
		case ID_MODIFY_FORMULA:
			{
				if(IsBig5Version()) break;
				if (m_SelectInfo.bExist)
				{
					TINDEXINFO *pTmpIndex = NULL;
					TOBJECT *tmp;
					tmp = m_aRegion[m_SelectInfo.nInRegion].aIndexInfo[m_SelectInfo.nInObject];
					pTmpIndex = g_pCalc->_CallDTopViewCalc_8(tmp->nSet, tmp->nGSIndex);
					if (pTmpIndex != NULL)
					{
						BOOL bOK = TRUE;
						if (pTmpIndex->lpszPassword != NULL && strlen(pTmpIndex->lpszPassword) != 0)
						{
							CPasswordDlg PasswordDlg;
							PasswordDlg.m_strName = pTmpIndex->acCode;
							PasswordDlg.m_bFomulaCompareBin = TRUE;
							memcpy(PasswordDlg.m_lpszPassword,pTmpIndex->lpszPassword,PASSWORD_LEN);
							if (PasswordDlg.DoModal() != IDOK)
								bOK = FALSE;
						}
						if (bOK) 
						{
							if ( g_pCalc->_CallDTopViewCalc_16(tmp->nSet, tmp->nGSIndex,m_nCalcPeriod) )
							{
								//CalcOne
								m_bChgRatio = TRUE;
								ModifyProperty(tmp);
								//计算本部件当前屏幕的最值
								CalcOneGSRatio(tmp,(m_nLastNo+1)-m_nDrawNum);
								//调整本Region的最值，并且重新计算比例
								CalcOneRegionRatio(m_SelectInfo.nInRegion);
								ProcessMsg(WM_PAINT);
							}
						}
					}
				}
				else ProcessZBInRegion(ID_MODIFY_FORMULA);
			}
			break;
		}
	}
}

void UFxt::ChangeSelfColor()
{
	PeekMsg();
	memset(&m_SelectInfo,0,sizeof(FXTSELINFO));
	CColorDialog dlg(VipColor.DRAWLINETOOLCOLOR, 0, m_pView);
	if (dlg.DoModal() == IDOK)
	{
		VipColor.DRAWLINETOOLCOLOR = dlg.GetColor();
		ProcessMsg(WM_PAINT, 2);
	}
	ProcessMsg(WM_PAINT);	//wParam=1要读数据 =2仅更新全局位图
}

void UFxt::ChangeMenuZB(int nID,BOOL bFromMenu)
{
	BOOL bZs = FALSE;
	CString tmpStr;
	TINDEXINFO *pTmpIndex = NULL;
	if (nID != ID_MOREINDEX)
		pTmpIndex = g_pCalc->_CallDTopViewCalc_8(ZB_TYPE, g_strOftenUseZb[nID-ID_MENU_ZB1]);
	else
	{
		bZs = TRUE;
		CAllZSDlg dlg;
		tmpStr = (g_pCalc->_CallDTopViewCalc_8(m_SelectInfo.nSet, m_SelectInfo.nGSIndex))->acCode;
		dlg.Init(0, m_nPeriod, tmpStr, FALSE);
		if (dlg.DoModal() == IDOK)
		{
			if (dlg.m_bIsSpecialLine)
			{
				ChangGS(bFromMenu?m_nSelRegionBak:m_nSelRegion, dlg.m_lPos, SPECLINE_OBJECT);
				ProcessMsg(WM_PAINT);
			}
			else
			{
				pTmpIndex = g_pCalc->_CallDTopViewCalc_8(ZB_TYPE, dlg.m_lPos);
				for(int i=0;i<MAX_PARAMNUM;i++)
				{
					for(int j=0;j<MAX_PRDNUM;j++)
					{
						pTmpIndex->aPara[i].nValue[j]=dlg.m_curParams[i];
					}
				}
			}
		}
	}
	if (pTmpIndex != NULL)
	{
		ChangGS(bFromMenu?m_nSelRegionBak:m_nSelRegion, pTmpIndex->nIndexNo);
		ProcessMsg(WM_PAINT);
	}
	else if (!bZs)
	{
		char Buffer[100];
		_snprintf(Buffer,100,"找不到指标 : %s",tmpStr);
		TDX_MessageBox(m_pView->m_hWnd,Buffer,MB_ICONQUESTION);
	}
}

void UFxt::AdjustParam()
{
	int nSet, nIndexNo;
	nSet = m_SelectInfo.nSet;
	nIndexNo = m_SelectInfo.nGSIndex;
	CAdjustParamDlg dlg;
	dlg.Init(nSet, nIndexNo, m_nCalcPeriod, m_pView);
	dlg.DoModal();
}

BOOL UFxt::ExistZS()
{
	BOOL bOK = FALSE;
	for (int i=0;i<m_aRegion[0].nObject;i++ )
	{
		if ( m_aRegion[0].aIndexInfo[i]->nSet == KL_OBJECT || m_aRegion[0].aIndexInfo[i]->nSet == EXP_OBJECT)
		{
			bOK = TRUE;
			break;
		}
	}
	return bOK;
}

void UFxt::DelZs()
{
	BOOL bOK = FALSE;
	for (int i = 0; i < m_aRegion[0].nObject; i++)
	{
		if ( m_aRegion[0].aIndexInfo[i]->nSet == KL_OBJECT)
		{
			m_bKColor = FALSE;
			memset(&m_KLZS,0,sizeof(FXZSINFO));
			bOK = TRUE;
			m_pColorKIndex = NULL;
		}
		else if (m_aRegion[0].aIndexInfo[i]->nSet == EXP_OBJECT)
		{
			memset(&m_ExpZS,0,sizeof(FXZSINFO));
			bOK =TRUE;
			m_pExpertIndex = NULL;
		}
		if (bOK)
		{
			TOBJECT * pOb = m_aRegion[0].aIndexInfo[i];
			PushLastInfo(pOb);
			RegionDelOne(m_SelectInfo.nInRegion,pOb);
			m_bChgRatio = TRUE;
			ProcessMsg(WM_PAINT);
			--i;
		}
	}
}

void UFxt::SelectMany(UINT nID)
{
	int nType;
	LPCTSTR lpszText = NULL;
	TINDEXINFO *pTmpIndex = NULL;
	CAllZSDlg dlg;

	if (nID == ID_ZB_OVERLAP)
		nType = 0;
	else if (nID == ID_EXPSYSSHOW)
		nType = 2;	//交易系统指示
	else
		nType = 3;	//五彩K线指示
	if(nType==3&&m_nSpecProcDraw==2) 
		return;		//OX没有五彩K线
	for (int i=0;i<m_aRegion[0].nObject;i++ )
	{
		if ( m_aRegion[0].aIndexInfo[i]->nSet == KL_OBJECT || m_aRegion[0].aIndexInfo[i]->nSet == EXP_OBJECT)
		{
			lpszText = m_aRegion[0].aIndexInfo[i]->acCode;
			break;
		}
	}
	dlg.Init(nType, m_nCalcPeriod, lpszText);
	if (dlg.DoModal() == IDOK)
	{
		switch (nType)
		{
		case 3:		//K线模式指示
			pTmpIndex = g_pCalc->_CallDTopViewCalc_8(KL_TYPE, dlg.m_lPos);
			if (pTmpIndex != NULL)
			{
				if ( pTmpIndex->nDelStatus & ERR_INDEX )
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
				AppendRg0(pTmpIndex);
				m_pColorKIndex = pTmpIndex;
			}
			break;
		case 2:		//交易系统指示
			pTmpIndex = g_pCalc->_CallDTopViewCalc_8(EXP_TYPE, dlg.m_lPos);
			if (pTmpIndex != NULL)
			{
				if ( pTmpIndex->nDelStatus & ERR_INDEX )
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
				AppendRg0(pTmpIndex);
				m_pExpertIndex = pTmpIndex;
			}
			break;
		case 0:
			pTmpIndex = g_pCalc->_CallDTopViewCalc_8(ZB_TYPE, dlg.m_lPos);
			if (pTmpIndex != NULL)
			{
				if ( pTmpIndex->nDelStatus & ERR_INDEX )
				{
					TDX_MessageBox(NULL,"公式编译未通过!",MB_OK|MB_ICONEXCLAMATION);
					return;
				}
				ChangGS(m_nSelRegion, dlg.m_lPos);
				ProcessMsg(WM_PAINT);
			}
			break;
		}
	}
}

//统一把对应鼠标的位置变为相对位置,才能调用
int  UFxt::IsAtEage(CPoint point)
{
	for ( int i=0;i<m_nRegion-1;i++ )
	{
		if ( abs(point.y-m_aRegion[i].Area.bottom)<=SELECT_ON )
		{
			m_EageInfo.nWhitch = i;
			return i;
		}
	}
	return -1;
}

void UFxt::SetMyAxis()
{
	double  dRate = 0.0;
	int		Start = (m_nRegion-1)*m_nRegion/2 , nNo = m_EageInfo.nWhitch;
	dRate  = ((double)(m_EageInfo.EagePoint.y-top))/(bottom-FXT_BHEIGHT-top);
	if ( dRate < COMPPREC )
	{
		ProcessMsg(WM_PAINT);
		return ;
	}
	if ( (nNo<1 && dRate < m_aRatio[Start+nNo+1])
		|| (nNo == m_nRegion && dRate > m_aRatio[Start+nNo-1] )
		|| ( dRate < m_aRatio[Start+nNo+1] && dRate > m_aRatio[Start+nNo-1] && nNo > 0 && nNo <m_nRegion)
	  )
	{
		m_aRatio[Start+nNo] = dRate;
		m_bChgRegion = TRUE;
		SavePriv();
	}
	ProcessMsg(WM_PAINT);
}

void UFxt::SetXPanState(int nType)
{
	if ( nType )
		m_nMousePan = 1;
	else
	{
		m_nMousePan = 0;
		m_bChgRatio = TRUE;
		ProcessMsg(WM_PAINT);
	}
}

void  UFxt::TQdata(int flag)
{
	TransAnalyData(0);
	
	TOBJECT * p;
	
#define	OverStkTQ()	\
do{		p = m_pObject->next;		\
		while( p )						\
		{	if ( p->nSet == HIS_DAT )	\
				ReadSomeStk(p);			\
			p = p->next;		\
		}	\
}while(0)

	int nTodayDataNum = 0;
	m_pAnalyData = m_pObject->pHisDay;
	if (g_nTQFlag == -1)
	{
		memcpy(m_pObject->pHisDay,m_pOrignData,m_nDataNum*sizeof(AnalyData));
		TransAnalyData(1);
		RawData2CalcData(m_pStkInfo->Code,m_pStkInfo->setcode,m_nPeriod,m_pAnalyData,m_pHisData,m_nDataNum,g_NoTime);
		OverStkTQ();
		CalcAllGS();
	 	m_bChgRatio = TRUE;
		g_nTQFlag = 0;
	}
	switch ( flag )
	{
	case 0:
		if (g_nTQFlag == 0) break;
		memcpy(m_pObject->pHisDay,m_pOrignData,m_nDataNum*sizeof(AnalyData));
		TransAnalyData(1);
		RawData2CalcData(m_pStkInfo->Code,m_pStkInfo->setcode,m_nPeriod,m_pAnalyData,m_pHisData,m_nDataNum,g_NoTime);
	 	g_nTQFlag=0;
		OverStkTQ();
		CalcAllGS();
	 	m_bChgRatio = TRUE;
		break;
	case 1://前复权
		if (g_nTQFlag == 1) 
		{
			g_nTQFlag = 0;
			memcpy(m_pObject->pHisDay,m_pOrignData,m_nDataNum*sizeof(AnalyData));
			TransAnalyData(1);
			RawData2CalcData(m_pStkInfo->Code,m_pStkInfo->setcode,m_nPeriod,m_pAnalyData,m_pHisData,m_nDataNum,g_NoTime);
			OverStkTQ();
			CalcAllGS();
			m_bChgRatio = TRUE;
			break;
		}
		if (g_nTQFlag != 0)
			memcpy(m_pObject->pHisDay,m_pOrignData,m_nDataNum*sizeof(AnalyData));
		g_nTQFlag = 1;
		g_pStockIO->TQData(m_pStkInfo, m_pAnalyData,0,m_nLastNo,m_nDataNum,g_nTQFlag,m_nPeriod);
		TransAnalyData(1);
		RawData2CalcData(m_pStkInfo->Code,m_pStkInfo->setcode,m_nPeriod,m_pAnalyData,m_pHisData,m_nDataNum,g_NoTime);
		OverStkTQ();
		CalcAllGS();
		m_bChgRatio = TRUE;
		break;
	case 2://后复权
		if (g_nTQFlag == 2)
		{
			g_nTQFlag = 0;
			memcpy(m_pObject->pHisDay,m_pOrignData,m_nDataNum*sizeof(AnalyData));
			TransAnalyData(1);
			RawData2CalcData(m_pStkInfo->Code,m_pStkInfo->setcode,m_nPeriod,m_pAnalyData,m_pHisData,m_nDataNum,g_NoTime);
			OverStkTQ();
			CalcAllGS();
			m_bChgRatio = TRUE;
			break;
		}
		if (g_nTQFlag != 0)
			memcpy(m_pObject->pHisDay,m_pOrignData,m_nDataNum*sizeof(AnalyData));
		g_nTQFlag = 2;
		g_pStockIO->TQData(m_pStkInfo, m_pAnalyData,0,m_nLastNo,m_nDataNum,g_nTQFlag,m_nPeriod);//m_nLeftNo->0
		TransAnalyData(1);
		RawData2CalcData(m_pStkInfo->Code,m_pStkInfo->setcode,m_nPeriod,m_pAnalyData,m_pHisData,m_nDataNum,g_NoTime);
		OverStkTQ();
		CalcAllGS();
		m_bChgRatio = TRUE;
		break;
	case 3:
		if (g_nTQFlag == 3)
		{
			g_nTQFlag = 0;
			memcpy(m_pObject->pHisDay,m_pOrignData,m_nDataNum*sizeof(AnalyData));
			TransAnalyData(1);
			RawData2CalcData(m_pStkInfo->Code,m_pStkInfo->setcode,m_nPeriod,m_pAnalyData,m_pHisData,m_nDataNum,g_NoTime);
			OverStkTQ();
			CalcAllGS();
			m_bChgRatio = TRUE;
			break;
		}
		memcpy(m_pObject->pHisDay,m_pOrignData,m_nDataNum*sizeof(AnalyData));
		g_nTQFlag = 3;
		switch(g_nPartTq)
		{
		case 0:
			g_pStockIO->TQData(m_pStkInfo, m_pAnalyData,0,m_nLastNo,m_nDataNum,g_nTQFlag,m_nPeriod);
			TransAnalyData(1);
			RawData2CalcData(m_pStkInfo->Code,m_pStkInfo->setcode,m_nPeriod,m_pAnalyData,m_pHisData,m_nDataNum,g_NoTime);
			OverStkTQ();
			CalcAllGS();
			m_bChgRatio = TRUE;
			break;
		case 1:
			g_pStockIO->TQData(m_pStkInfo, m_pAnalyData,0,m_nLastNo,m_nDataNum,g_nTQFlag+g_nPartTq,m_nPeriod);//m_nLeftNo->0
			TransAnalyData(1);
			RawData2CalcData(m_pStkInfo->Code,m_pStkInfo->setcode,m_nPeriod,m_pAnalyData,m_pHisData,m_nDataNum,g_NoTime);
			OverStkTQ();
			CalcAllGS();
			m_bChgRatio = TRUE;
			break;
		}
		break;
	}
#undef OverStkTQ
}

void	UFxt::ProcessZBInRegion(UINT uiMsg)
{
	int nSet = 0, nGSIndex = -1, nInObj = -1;
	for ( int i = 0;i<m_aRegion[m_nSelRegionBak].nObject;i++ )
	{
		TOBJECT *pObject = m_aRegion[m_nSelRegionBak].aIndexInfo[i];
		if ( pObject->nSet == ZB_OBJECT )
		{
			nInObj = i;
			nSet = pObject->nSet;
			nGSIndex = pObject->nGSIndex;
		}
	}
	if(nGSIndex<0) return;
	switch(uiMsg)
	{
	case	ID_INDEXHELP:
		{
			TOBJECT *tmp;
			tmp = m_aRegion[m_nSelRegionBak].aIndexInfo[nInObj];
			CDescZBDlg dlg;
			dlg.Init(tmp->nSet, tmp->acCode);
			dlg.DoModal();
		}
		break;
	case	UM_ADJUSTPARAM:
		{
			CAdjustParamDlg dlg;
			dlg.Init(nSet, nGSIndex, m_nCalcPeriod, m_pView);
			dlg.DoModal();			
		}
		break;
	case	ID_DELZB:
		{
			TOBJECT * pOb = m_aRegion[m_nSelRegionBak].aIndexInfo[nInObj];
			PushLastInfo(pOb);
			RegionDelOne(m_nSelRegionBak,pOb);
			m_bChgRatio = TRUE;
			ProcessMsg(WM_PAINT);
		}
		break;
	case	ID_MODIFY_FORMULA:
		{
			TINDEXINFO *pTmpIndex = NULL;
			TOBJECT *tmp;
			tmp = m_aRegion[m_nSelRegionBak].aIndexInfo[nInObj];
			pTmpIndex = g_pCalc->_CallDTopViewCalc_8(tmp->nSet, tmp->nGSIndex);
			if (pTmpIndex != NULL)
			{
				BOOL bOK = TRUE;
				if (pTmpIndex->lpszPassword != NULL && strlen(pTmpIndex->lpszPassword) != 0)
				{
					CPasswordDlg PasswordDlg;
					PasswordDlg.m_strName = pTmpIndex->acCode;
					PasswordDlg.m_bFomulaCompareBin = TRUE;
					memcpy(PasswordDlg.m_lpszPassword,pTmpIndex->lpszPassword,PASSWORD_LEN);
					if (PasswordDlg.DoModal() != IDOK)
						bOK = FALSE;
				}
				if (bOK) 
				{
					if ( g_pCalc->_CallDTopViewCalc_16(tmp->nSet, tmp->nGSIndex,m_nCalcPeriod) )
					{
						//CalcOne
						m_bChgRatio = TRUE;
						ModifyProperty(tmp);
						//计算本部件当前屏幕的最值
						CalcOneGSRatio(tmp,(m_nLastNo+1)-m_nDrawNum);
						//调整本Region的最值，并且重新计算比例
						CalcOneRegionRatio(m_nSelRegionBak);
						ProcessMsg(WM_PAINT);
					}
				}
			}
		}
		break;
	}
}

//跳转到某个指标,先找指标再找五彩K线
void UFxt::JumpToZB(char *ZBCode,int nTarRegion)
{
	if(ZBCode[0]==0&&nTarRegion!=m_nRegion)
	{
		ProcessMsg(UM_SELNUM, nTarRegion, 1);
		return;
	}
	
	TINDEXINFO * pTmpIndex = g_pCalc->_CallDTopViewCalc_8(ZB_TYPE, ZBCode);
	if(pTmpIndex)
	{
		for (int i=0;i<m_nRegion;i++ )
		{
			for (int j=0;j<m_aRegion[i].nObject;j++ )
			{
				TOBJECT *p = m_aRegion[i].aIndexInfo[j];
				if ( p->nSet == ZB_OBJECT )
				{//不用本区域的比例等参数
					TINDEXINFO * pIndex = g_pCalc->_CallDTopViewCalc_8(ZB_OBJECT,p->nGSIndex);
					if(pIndex->nIndexNo==pTmpIndex->nIndexNo)
					{
						if(nTarRegion>=0&&nTarRegion!=i)
							continue;
						else return;
					}
				}
			}
		}
		
		ChangGS(nTarRegion>=0?nTarRegion:m_nSelRegion, pTmpIndex->nIndexNo);
		return;
	}
	
	pTmpIndex = g_pCalc->_CallDTopViewCalc_8(KL_TYPE, ZBCode);
	if (pTmpIndex)
	{
		for (int i=0;i<m_nRegion;i++ )
		{
			for (int j=0;j<m_aRegion[i].nObject;j++ )
			{
				TOBJECT *p = m_aRegion[i].aIndexInfo[j];
				if ( p->nSet == KL_OBJECT )
				{//不用本区域的比例等参数
					TINDEXINFO * pIndex = g_pCalc->_CallDTopViewCalc_8(KL_OBJECT,p->nGSIndex);
					if(pIndex->nIndexNo==pTmpIndex->nIndexNo)
					{
					//	DelZs();
						return;
					}
				}
			}
		}
		ProcessMsg(UM_CHANGEZS,2,pTmpIndex->nIndexNo);
		return;
	}
	
	pTmpIndex = g_pCalc->_CallDTopViewCalc_8(EXP_TYPE, ZBCode);
	if (pTmpIndex)
	{
		for (int i=0;i<m_nRegion;i++ )
		{
			for (int j=0;j<m_aRegion[i].nObject;j++ )
			{
				TOBJECT *p = m_aRegion[i].aIndexInfo[j];
				if ( p->nSet == EXP_OBJECT )
				{//不用本区域的比例等参数
					TINDEXINFO * pIndex = g_pCalc->_CallDTopViewCalc_8(EXP_OBJECT,p->nGSIndex);
					if(pIndex->nIndexNo==pTmpIndex->nIndexNo)
					{
					//	DelZs();
						return;
					}
				}
			}
		}
		ProcessMsg(UM_CHANGEZS,1,pTmpIndex->nIndexNo);
		return;
	}
	
//	if(!pTmpIndex)
//	{
//		ProcessMsg(UM_SELNUM, nTarRegion>=0?nTarRegion:m_nSelRegion, 1);
//		return;
//	}
}
