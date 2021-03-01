#include "stdafx.h"
#include "Znxg.h"
#include "ProgressWnd.h"

CZnxg::CZnxg(CWnd *pWnd)
{
	m_pParent = pWnd;
	m_nABGNum = 0;
	//各分界值，可设置
	g_pXGSign.Xz_sign = 4500;		//小盘,中盘分界  4500万股
	g_pXGSign.Dz_sign = 15000;		//大盘,中盘分界  15000万股

	g_pXGSign.Yp_sign = 0.60f;		//绩优,绩平分界  每股税后利润（每股收益）>= 0.60元
	g_pXGSign.Pc_sign = 0.10f;		//绩平,绩差分界  每股税后利润（每股收益）<  0.60元 并且
									//每股税后利润（每股收益）>= 0.10元
	g_pXGSign.Ks_sign = 0.0;		//亏损标志

	g_pXGSign.Cxg_Day_L = 120;		//次新股上市时间次新标志

	g_pXGSign.Jzc_G_sign = 2.50f;	//净资产高标志
	g_pXGSign.Jzc_D_sign = 1.00f;	//净资产低标志

	g_pXGSign.Gjx_G_sign = 1.50f;	//公积金高标志
	g_pXGSign.Gclr_G_sign = 0.50f;	//滚存利润高标志
	g_pXGSign.DMI_Day = 14;			//DMI： 14日DMI
	g_pXGSign.RSI_Day = 12;			//长周期RSI：12日RSI
	g_pXGSign.WVAD_Day = 24;		//24日WVAD
}

CZnxg::~CZnxg()
{
}

//支持断点续算 CalcFlag 0:本地计算或宽带计算,可续算 1:远程请求文件
BOOL CZnxg::CalcAllStockStyle()
{
	BOOL bRet = FALSE;
	int	 nStartNo = 0,i,k;
	memset(m_pGPCode, 0, sizeof(m_pGPCode));
	//得到深沪AB股的集合
	union   flunion ftype;
	ftype.fshort = 0x81;//包括中小企业
	m_nABGNum = g_pStockIO->FiltCode(SZ_SH,ftype,m_pABGXh);
	ftype.fshort = 0x02;
	m_nABGNum += g_pStockIO->FiltCode(SZ_SH,ftype,m_pABGXh+m_nABGNum);

	BOOL bReadSuccess = FALSE;
	//此处有待商榷,Cache文件在几个版本中倒底怎么读
	CFile RCachFile;
	if(RCachFile.Open(g_WSXHStr+(CString)"tmp\\clecache.tmp",CFile::modeRead|CFile::shareDenyNone))
	{
		CTime theTime,NowTime = CTime::GetCurrentTime();
		RCachFile.Read(&theTime,sizeof(theTime));
		long Lastdate;
		RCachFile.Read(&Lastdate,sizeof(long));
		char version;
		RCachFile.Read(&version,sizeof(char));
		if(version == ZNXG_VERSION && Lastdate == g_lOpenRq) 	//智能选股版本不一致等，则直接返回
		{
			DWORD FileLen = RCachFile.GetLength();
			if(!g_bZnxg_Down && (NowTime-theTime).GetHours() < 1)
				bReadSuccess = TRUE;
			if(bReadSuccess)
			{
				nStartNo = (FileLen-sizeof(theTime)-sizeof(long)-sizeof(char))/(SH_CODE_LEN+sizeof(long)+sizeof(short));
				nStartNo = min(m_nABGNum,nStartNo);	//nStartNo不能大于m_nABGNum
				for(k = 0;k < nStartNo;k++)
				{
					RCachFile.Read(m_pGPCode+k,SH_CODE_LEN);
					m_pGPCode[k][SH_CODE_LEN] = 0;
					RCachFile.Read(m_pStkAttrib+k,sizeof(long));
					RCachFile.Read(m_pStkXh+k,sizeof(short));
				}
			}
		}
		RCachFile.Close();
	}
	if(bReadSuccess && nStartNo == m_nABGNum)	return TRUE;

	if(!CanProcesLocalData(m_pParent))
		return FALSE;	

	RefreshAllHq();

	g_bZnxg_Down = FALSE;

	int		nTodayDataNum;
	short   nDataNum;
	MemStkInfo *tmpInfo;
	struct AnalyData *pAnalyData = new AnalyData[200+10];
	memset(pAnalyData, 0, (200+10)*sizeof(AnalyData));
	struct CurrStockData  CurrHQ={0};
	//设定模态进度条对话框
	CProgressWnd wndProgress(m_pParent,_F("正在计算，请稍等..."),TRUE);
	wndProgress.GoModal();
	wndProgress.SetRange(0,m_nABGNum-nStartNo);
	wndProgress.SetStep(3);
	g_bDonotClose = TRUE;
	for(i = 0; i < m_nABGNum;i++)
	{
		tmpInfo = (*g_pStockIO)[m_pABGXh[i]];
		if(!tmpInfo || tmpInfo->Close<COMPPREC) 
			continue;
		for(k=0;k < nStartNo;k++)
			if(strcmp(tmpInfo->Code,m_pGPCode[k])==0) break;
		if(k >= nStartNo)	//不在其中，则重算
		{
			if(i%3 == 0)
			{
				wndProgress.SetText(tmpInfo->Name);
				wndProgress.StepIt();
			}
			if(i%10 == 0)
			{
				wndProgress.PeekAndPump();
				if(wndProgress.Cancelled())
				{
					if(i > 0)	bRet = TRUE; //如果中途取消，则也进入
					else		bRet = FALSE;
					m_nABGNum = i;
					g_bDonotClose = FALSE;
					goto writecache;
				}
			}
			nDataNum = g_pStockIO->ReadAnalyDataAndCurrHQ(tmpInfo,pAnalyData,200,PER_DAY,&CurrHQ,nTodayDataNum);
			//随后填权处理
			g_pStockIO->TQData(tmpInfo,pAnalyData,0,nDataNum-1,nDataNum,1,PER_DAY);
			
			if(nDataNum <= 20||GetKLineStyleSet(tmpInfo,pAnalyData,nDataNum,nDataNum-1,&m_pStkXh[i])<=0)
				m_pStkXh[i] = 0;
			m_pStkAttrib[i] = GetGP_Tj_Attrib(m_pABGXh[i],pAnalyData,nDataNum);

			strncpy(m_pGPCode[i],tmpInfo->Code,SH_CODE_LEN);
		}
	}
	m_nABGNum = i;
	bRet = TRUE;
	g_bDonotClose = FALSE;
writecache:;
	delete []pAnalyData;
	CFile WCachFile;
	if(nStartNo == 0)
	{
		if(WCachFile.Open(g_WSXHStr+(CString)"tmp\\clecache.tmp",CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive))
		{
			CTime theTime = CTime::GetCurrentTime();
			WCachFile.Write(&theTime,sizeof(theTime));
			long NowDate = g_lOpenRq;
			WCachFile.Write(&NowDate,sizeof(long));
			char version = ZNXG_VERSION;
			WCachFile.Write(&version,sizeof(char));
			for(k = 0;k < m_nABGNum;k++)
			{
				WCachFile.Write(m_pGPCode+k,SH_CODE_LEN);
				WCachFile.Write(m_pStkAttrib+k,sizeof(long));
				WCachFile.Write(m_pStkXh+k,sizeof(short));
			}
			WCachFile.Close();
		}
	}
	else
	{
		if(WCachFile.Open(g_WSXHStr+(CString)"tmp\\clecache.tmp",CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite|CFile::shareExclusive))
		{
			WCachFile.SeekToEnd();
			for(k = 0;k < m_nABGNum-nStartNo;k++)
			{
				WCachFile.Write(m_pGPCode+nStartNo+k,SH_CODE_LEN);
				WCachFile.Write(m_pStkAttrib+nStartNo+k,sizeof(long));
				WCachFile.Write(m_pStkXh+nStartNo+k,sizeof(short));
			}
			WCachFile.Close();
		}
	}
	return bRet;
}

short CZnxg::GetKLineStyleGP(short Type,short *lpnGPIndex)
{
	int nCount = 0;
	for(short i = 0;i < m_nABGNum;i++)
	{
		if(m_pStkXh[i] == Type)
			lpnGPIndex[nCount++] = m_pABGXh[i];
	}
	return nCount;
}

short CZnxg::GetKLineStyleSet(MemStkInfo *tmpInfo,struct AnalyData *pAnalyData,short nDataNum,short nNo,short * npoutStyle,short nMaxNum)
{
	short	nStyleKind=0;
	short   nTrendSet[128];
	short	npStyle[256];
	long    npLoc[256];
	short   nSpan=5;
	short	bStyle;
	short   nFirstLoc;
	short   nSecendLoc;
	short   nThirdLoc;
	short   i,j;
	if(nDataNum<3*nSpan||nNo<3*nSpan-1) return nStyleKind;
	for(i=0; i<34; i++)
	{	if(isTrendK_Style(tmpInfo,pAnalyData,nDataNum,nSpan,nNo,TrendCodeTable[i])!=0) nTrendSet[i] = TRUE;
		else                                                                   nTrendSet[i] = FALSE;
	}
	CLE_CONTABLE *tmpTable;
	int tmpInt;
	for(i=0; i<121; i++)
	{
		if(i <= 31)
		{
			tmpTable = g_pCleKLine1;
			tmpInt = i;
		}
		else if(i <= 58)
		{
			tmpTable = g_pCleKLine2;
			tmpInt = i-32;
		}
		else if(i <= 93)
		{
			tmpTable = g_pCleKLine3;
			tmpInt = i-59;
		}
		else 
		{
			tmpTable = g_pCleKLine4;
			tmpInt = i-94;
		}
		if(nTrendSet[tmpTable[tmpInt].Status-1]==TRUE)
		{	bStyle = FALSE;
			switch(tmpTable[tmpInt].MsgNo)
			{
			case KLINE_1001://低档多条并列阳线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDUP,2,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1002://低档多条并列阴线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDDOWN,2,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1003://低档横盘中剑形线
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,0,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1004://低档横盘中弓形线
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,0,KLINEO_SMALL,KLINEO_UVLSHADOW,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1005://低档横盘中下跳大阳线
				nSecendLoc = nSpan;
				nFirstLoc  = nNo;
				do
				{	nFirstLoc = E_OddK_Style(pAnalyData,nSecendLoc,nFirstLoc,KLINEO_UP,KLINEO_BIG,0,1,1);
					if(nFirstLoc>0&&nFirstLoc<nNo&&
					   OddK_Size  (pAnalyData,nFirstLoc-1)==KLINEO_SMALL&&
					   OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_DOWN &&
					   pAnalyData[nFirstLoc].Close<pAnalyData[nFirstLoc-1].Open )
						{ bStyle=TRUE; break; }
					nSecendLoc--;
					nFirstLoc --;
				}
				while(nFirstLoc!=-1&&nSecendLoc>0);
				break;
			case KLINE_1006://低档横盘中上跳大阴线
				nSecendLoc = nSpan;
				nFirstLoc  = nNo;
				do
				{	nFirstLoc = E_OddK_Style(pAnalyData,nSecendLoc,nFirstLoc,KLINEO_DOWN,KLINEO_BIG,0,1,1);
					if(nFirstLoc>0&&nFirstLoc<nNo&&
					   OddK_Size  (pAnalyData,nFirstLoc-1)==KLINEO_SMALL&&
					   OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP   &&
					   pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-1].Open )
						{ bStyle=TRUE; break; }
					nSecendLoc--;
					nFirstLoc --;
				}
				while(nFirstLoc!=-1&&nSecendLoc>0);
				break;
			case KLINE_1007://低档横盘末期弓形线
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,0,KLINEO_SMALL,KLINEO_UVLSHADOW,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1008://低档横盘末期剑形线
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,0,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1009://低档横盘末期大阳突破
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_BIG,0,1,1);
				if(nFirstLoc==nNo)  { bStyle=TRUE; break; }
				break;
			case KLINE_1010://小幅盘落后大阳向上
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_BIG,0,1,1);
				if(nFirstLoc==nNo&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,3,0)==nNo-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1011://低档横盘末期红三兵
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_SMALL,0,3,1);
				if(nFirstLoc==nNo)  { bStyle=TRUE; break; }
				break;
			case KLINE_1012://低档急跌三大阴
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,3,1);
				if(nFirstLoc>1&&OddK_Size(pAnalyData,nFirstLoc-2)!=KLINEO_SMALL&&
				   OddK_Size(pAnalyData,nFirstLoc-1)!=KLINEO_SMALL&&
				   OddK_Size(pAnalyData,nFirstLoc)!=KLINEO_SMALL)
					 { bStyle=TRUE; break; }
				break;
			case KLINE_1013://三空阴线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,2,1);
				if(nFirstLoc>0&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG&&
				   OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_BIG&&
				   OddK_Size(pAnalyData,nFirstLoc-2)==KLINEO_BIG)
					 { bStyle=TRUE; break; }
				break;
			case KLINE_1014://低档急跌后下跳十字星
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&OddK_UpDown(pAnalyData,nFirstLoc)==KLINEO_LEVEL&&
				   OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOHEADFOOT&&
				   OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOHEAD    &&
				   OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOFOOT    )
					 { bStyle=TRUE; break; }
				break;
			case KLINE_1015://低档急跌后下跳剑形线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL&&
				   OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_DVLSHADOW)
					 { bStyle=TRUE; break; }
				break;
			case KLINE_1016://低档急跌后下跳弓形线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL&&
				   OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_UVLSHADOW)
					 { bStyle=TRUE; break; }
				break;
			case KLINE_1017://低档急跌后尽头阴线(有下长影)
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-1].Low)  { bStyle=TRUE; break; }
				break;
			case KLINE_1018://低档急跌后独阳线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FAKECUTIN,1,1);
				if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG)  { bStyle=TRUE; break; }
				break;
			case KLINE_1019://低档急跌后独阴线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
				if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG)  { bStyle=TRUE; break; }
				break;
			case KLINE_1020://低档急跌后阳包阴
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUD,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1021://低档急跌后最后包容阴线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDU,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1022://低档急跌后阴孕阳
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDU,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1023://低档急跌后阴孕阴
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDD,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1024://低档急跌后阴孕十字星
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDL,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1025://低档急跌后阴孕弓形线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDD,1,1);
				while(nFirstLoc>=0)
				{   if(OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL&&
					   OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_UVLSHADOW)
					{	bStyle=TRUE;	break;	}
					else	nFirstLoc = E_TwinK_Style(pAnalyData,nSpan-(nNo-nFirstLoc),nFirstLoc-1,KLINET_GDD,1,1);
				}
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDU,1,1);
				while(nFirstLoc>=0)
				{   if(OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL&&
					   OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_UVLSHADOW)
					{	bStyle=TRUE;	break;	}
					else	nFirstLoc = E_TwinK_Style(pAnalyData,nSpan-(nNo-nFirstLoc),nFirstLoc-1,KLINET_GDU,1,1);
				}
				break;
			case KLINE_1026://低档急跌后阴孕剑形线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDD,1,1);
				while(nFirstLoc>=0)
				{   if(OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL&&
					   OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_DVLSHADOW)
					{	bStyle=TRUE;	break;	}
					else	nFirstLoc = E_TwinK_Style(pAnalyData,nSpan-(nNo-nFirstLoc),nFirstLoc-1,KLINET_GDD,1,1);
				}
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDU,1,1);
				while(nFirstLoc>=0)
				{   if(OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL&&
					   OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_DVLSHADOW)
					{	bStyle=TRUE;	break;	}
					else	nFirstLoc = E_TwinK_Style(pAnalyData,nSpan-(nNo-nFirstLoc),nFirstLoc-1,KLINET_GDU,1,1);
				}
				break;
			case KLINE_1027://低档急跌后切入线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUTIN,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1028://低档急跌后阴夹阳
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_NDU,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1029://低档急跌后上拉线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGUP,1,1);
				if(nFirstLoc>0&&OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_BIG)  { bStyle=TRUE; break; }
				break;
			case KLINE_1030://低档急跌后连拉剑形线
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_SMALL,KLINEO_DVLSHADOW,2,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1031://低档二次狙击性阳线
				nSecendLoc = E_TwinK_Style(pAnalyData,3*nSpan,nNo,KLINET_ASCENT,2,1);
				if(nSecendLoc!=-1)
				{	nFirstLoc = E_TwinK_Style(pAnalyData,3*nSpan,nSecendLoc+2,KLINET_SLIDE,1,0);
					if(nFirstLoc!=-1&&nFirstLoc==nSecendLoc+2)
					{	nThirdLoc = E_TwinK_Style(pAnalyData,3*nSpan+nSecendLoc-nNo,nSecendLoc-3,KLINET_SLIDE,1,0);
						if(nThirdLoc!=-1&&nThirdLoc<nSecendLoc)
						{ bStyle=TRUE; break; }
					}
				}
				break;
			case KLINE_1032://上档盘整后大阳突破
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
				if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG)  { bStyle=TRUE; break; }
				break;
			case KLINE_1033://跳高一字线后不补缺
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
				if(nFirstLoc!=-1&&E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_LEVEL,KLINEO_SMALL,KLINEO_NOHEADFOOT,1,1)==nFirstLoc)  { bStyle=TRUE; break; }
				break;
			case KLINE_1034://二次高开阳线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,2,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1035://跳空攀援线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
				if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,1,1)==nFirstLoc+1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1036://渐大三连阳
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,0,0,3,1);
				if(nFirstLoc>2&&
				   (pAnalyData[nFirstLoc].Close-pAnalyData[nFirstLoc].Open)>(pAnalyData[nFirstLoc-1].Close-pAnalyData[nFirstLoc-1].Open+COMPPREC) &&
				   (pAnalyData[nFirstLoc-1].Close-pAnalyData[nFirstLoc-1].Open)>(pAnalyData[nFirstLoc-2].Close-pAnalyData[nFirstLoc-2].Open+COMPPREC))
					 { bStyle=TRUE; break; }
				break;
			case KLINE_1037://上升中途跳高十字星
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
				if(nFirstLoc!=-1&&OddK_UpDown(pAnalyData,nFirstLoc)==KLINEO_LEVEL&&
				   OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOHEADFOOT&&
				   OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOHEAD    &&
				   OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOFOOT    )
					 { bStyle=TRUE; break; }
				break;
			case KLINE_1038://上升中途跳高剑形线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
				if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_DVLSHADOW&&
				   OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL)
					 { bStyle=TRUE; break; }
				break;
			case KLINE_1039://上升中途跳高弓形线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
				if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_UVLSHADOW&&
				   OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL)
					 { bStyle=TRUE; break; }
				break;
			case KLINE_1040://上升中途小压迫线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CRACK,1,1);
				if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL)  { bStyle=TRUE; break; }
				break;
			case KLINE_1041://上升中途伪阴线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNDOWN,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1042://上升中途阳包阴
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUD,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1043://上升中途归顺线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SURRENDER,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1044://上升中途上拉线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGUP,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1045://上升中途阳孕阴
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUD,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1046://上升中途切入线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUTIN,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1047://上升中途阳孕阳
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUU,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1048://上升中途并阳线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDUP,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1049://上升中途滑行线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1050://上升三连击
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,2,1);
				if(nFirstLoc>0&&nFirstLoc<nNo&&
				   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_DOWN&&
				   pAnalyData[nFirstLoc-2].Open>pAnalyData[nFirstLoc+1].Close)
					{ nFirstLoc++; bStyle=TRUE; break; }
				break;
			case KLINE_1051://上升三法
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1);
				if(nFirstLoc>2&&nFirstLoc<nNo&&
				   OddK_UpDown(pAnalyData,nFirstLoc-3)==KLINEO_UP&&
				   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_UP&&
				   pAnalyData[nFirstLoc-2].Open<pAnalyData[nFirstLoc+1].Close)
				{ nFirstLoc++; bStyle=TRUE; break; }
				break;
			case KLINE_1052://上升跳空三法
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,2,1);
				if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGDOWN,1,1)==nFirstLoc+1)
				{ nFirstLoc++; bStyle=TRUE; break; }
				break;
			case KLINE_1053://超越覆盖线或压迫线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,1,1);
				while(nFirstLoc>3)
				{	if(E_TwinK_Style(pAnalyData,nSpan-nNo+nFirstLoc,nFirstLoc-1,KLINET_SLIDE,2,1)==nFirstLoc-2&&
					   pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-4].Open) { bStyle=TRUE; break; }
					nFirstLoc = E_TwinK_Style(pAnalyData,nSpan-nNo+nFirstLoc,nFirstLoc-1,KLINET_ASCENT,1,1);
				}
				break;
			case KLINE_1054://上升中途 受阻红三兵
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,0,0,3,1);
				if(nFirstLoc!=-1&&
				  (pAnalyData[nFirstLoc].Close-pAnalyData[nFirstLoc].Open)<(pAnalyData[nFirstLoc-1].Close-pAnalyData[nFirstLoc-1].Open) &&
				  (pAnalyData[nFirstLoc-1].Close-pAnalyData[nFirstLoc-1].Open)<(pAnalyData[nFirstLoc-2].Close-pAnalyData[nFirstLoc-2].Open))
					{ bStyle=TRUE; break; }
				break;
			case KLINE_1055://上升中途 思考红三兵
				nFirstLoc = nNo;
				do
				{   if(nFirstLoc>1&&nFirstLoc>nNo-nSpan)
					{	if(OddK_UpDown(pAnalyData,nFirstLoc  )==KLINEO_UP &&
						   OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP &&
						   OddK_UpDown(pAnalyData,nFirstLoc-2)==KLINEO_UP &&
						   OddK_Size(pAnalyData,nFirstLoc  )==KLINEO_SMALL&&
						   OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_BIG  &&
						   OddK_Size(pAnalyData,nFirstLoc-2)==KLINEO_SMALL)
						{	bStyle=TRUE; break;
						}
					}
					nFirstLoc--;
				}
				while(nFirstLoc>1&&nFirstLoc>nNo-nSpan);
				break;
			case KLINE_1056://上升中途阴夹阳
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_NDU,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1057://上升中途阳包阳
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUU,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1058://上升中途伪阳线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNUP,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1059://高档多条并列阳线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDUP,2,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1060://高档多根并列阴线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDDOWN,2,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1061://反击滑行线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
				if(nFirstLoc>=0&&nFirstLoc+1==E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_BIG,0,1,1) &&
				   pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-2].Open) { bStyle=TRUE; break; }
				break;
			case KLINE_1062://高档横盘中上跳大阴线
				nSecendLoc = nSpan;
				nFirstLoc  = nNo;
				do
				{	nFirstLoc = E_OddK_Style(pAnalyData,nSecendLoc,nFirstLoc,KLINEO_DOWN,KLINEO_BIG,0,1,1);
					if(nFirstLoc>0&&nFirstLoc<nNo&&
					   OddK_Size  (pAnalyData,nFirstLoc-1)==KLINEO_SMALL&&
					   OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP   &&
					   pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-1].Open )
						{ bStyle=TRUE; break; }
					nSecendLoc--;
					nFirstLoc --;
				}
				while(nFirstLoc!=-1&&nSecendLoc>0);
				break;
			case KLINE_1063://高档横盘中弓形线
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,0,KLINEO_SMALL,KLINEO_UVLSHADOW,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1064://高档横盘中剑形线
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,0,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1065://滑行线接滑行线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
				if(nFirstLoc>3&&E_TwinK_Style(pAnalyData,nSpan,nFirstLoc-2,KLINET_SLIDE,1,1)==nFirstLoc-3&&
				   OddK_UpDown(pAnalyData,nFirstLoc-2)==KLINEO_UP)
					{ bStyle=TRUE; break; }
				break;
			case KLINE_1066://见顶三鸦
				nSecendLoc = nSpan;
				nFirstLoc  = nNo;
				do
				{	nFirstLoc = E_TwinK_Style(pAnalyData,nSecendLoc,nFirstLoc,KLINET_FEIGNDOWN,1,1);
					if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSecendLoc,nFirstLoc+1,KLINET_SLIDE,1,1)==nFirstLoc+1)
					{ bStyle=TRUE; break; }
					nSecendLoc--;
					nFirstLoc --;
				}
				while(nFirstLoc!=-1&&nSecendLoc>0);
				break;
			case KLINE_1067://高档盘整末期剑形线
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,0,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1068://高档盘整末期大阴向下
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1069://高档盘整末期黑三兵
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1070://小幅盘升后大阴向下
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,1,1);
				if(nFirstLoc>5&&pAnalyData[nFirstLoc].Close<MIN(pAnalyData[nFirstLoc-6].Close,pAnalyData[nFirstLoc-6].Open)) { bStyle=TRUE; break; }
				break;
			case KLINE_1071://暴跌三鸦
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1072://炮楼顶
				nFirstLoc  = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1);
				nSecendLoc = E_TwinK_Style(pAnalyData,nSpan,nFirstLoc,KLINET_ASCENT,2,1);
				if(nFirstLoc>10&&nSecendLoc<nFirstLoc&&nSecendLoc>nFirstLoc-10) { bStyle=TRUE; break; }
				break;
			case KLINE_1073://高档急升三大阳
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,3,1);
				if(nFirstLoc>1&&OddK_Size(pAnalyData,nFirstLoc-2)!=KLINEO_SMALL&&
				   OddK_Size(pAnalyData,nFirstLoc-1)!=KLINEO_SMALL&&
				   OddK_Size(pAnalyData,nFirstLoc)!=KLINEO_SMALL)
					{ bStyle=TRUE; break; }
				break;
			case KLINE_1074://三空阳线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,3,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1075://高档弃十字星
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
				if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1)==nFirstLoc+2&&
				   OddK_Size(pAnalyData,nFirstLoc+1)==KLINEO_SMALL&&
				   OddK_Shadow(pAnalyData,nFirstLoc+1)!=KLINEO_NOHEAD&&
				   OddK_Shadow(pAnalyData,nFirstLoc+1)!=KLINEO_NOFOOT)
					{ bStyle=TRUE; break; }
				break;
			case KLINE_1076://高档弃弓形线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ABANDON,1,1);
				if(nFirstLoc!=-1&&nFirstLoc<nNo)
				{	if(OddK_UpDown(pAnalyData,nFirstLoc)==KLINEO_UP       &&
					   OddK_Size  (pAnalyData,nFirstLoc)==KLINEO_SMALL    &&
					   OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_UVLSHADOW&&
					   pAnalyData[nFirstLoc+1].Close<pAnalyData[nFirstLoc].Close)
						{ bStyle=TRUE; break; }
				}
				break;
			case KLINE_1077://高档弃吊劲线
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_SMALL,KLINEO_DVLSHADOW,1,0);
				if(nFirstLoc>0&&nFirstLoc<nNo)
				{	if(OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP  &&
					   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_DOWN&&
					   pAnalyData[nFirstLoc-1].Close<pAnalyData[nFirstLoc].Close&&
					   pAnalyData[nFirstLoc+1].Open <pAnalyData[nFirstLoc].Close)
						{ bStyle=TRUE; break; }
				}
				break;
			case KLINE_1078://高档尽头阳线
				nSecendLoc = nSpan;
				nFirstLoc  = nNo;
				do
				{	nFirstLoc = E_OddK_Style(pAnalyData,nSecendLoc,nFirstLoc,KLINEO_UP,0,0,1,1);
					if(nFirstLoc!=-1&&nFirstLoc<nNo&&
					   OddK_Size  (pAnalyData,nFirstLoc  )!=KLINEO_SMALL&&
					   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_UP   &&
					   pAnalyData[nFirstLoc+1].Open >pAnalyData[nFirstLoc].Close&&
					   pAnalyData[nFirstLoc+1].Close<pAnalyData[nFirstLoc].High )
					{ nFirstLoc++; bStyle=TRUE; break; }
					nSecendLoc--;
					nFirstLoc --;
				}
				while(nFirstLoc!=-1&&nSecendLoc>0);
				break;
			case KLINE_1079://绝顶独阴线
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,1,1);
				while(nFirstLoc>0)
				{   if(nFirstLoc<nNo&&OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP&&
					   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_UP&&
					   pAnalyData[nFirstLoc].Open>pAnalyData[nFirstLoc-1].Close&&
					   pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-1].Close-0.2*(pAnalyData[nFirstLoc-1].Close-pAnalyData[nFirstLoc-1].Open)&&
					   pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc+1].Close)
					{	nFirstLoc++;
						bStyle=TRUE;
						break;
					}
					else
						nFirstLoc = E_OddK_Style(pAnalyData,nSpan-(nNo-nFirstLoc),nFirstLoc-1,KLINEO_DOWN,KLINEO_BIG,0,1,1);
				}
				break;
			case KLINE_1080://绝顶独阳线
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_BIG,0,1,1);
				while(nFirstLoc>0)
				{   if(nFirstLoc<nNo&&OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP&&
					   pAnalyData[nFirstLoc].Open>MAX(pAnalyData[nFirstLoc-1].Open,pAnalyData[nFirstLoc-1].Close)&&
					   pAnalyData[nFirstLoc].Open>MAX(pAnalyData[nFirstLoc+1].Open,pAnalyData[nFirstLoc+1].Close))
					{	nFirstLoc++;
						bStyle=TRUE;
						break;
					}
					nFirstLoc = E_OddK_Style(pAnalyData,nSpan-(nNo-nFirstLoc),nFirstLoc-1,KLINEO_UP,KLINEO_BIG,0,1,1);
				}
				break;
			case KLINE_1081://高档跳空滑行线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
				while(nFirstLoc>1)
				{   if(pAnalyData[nFirstLoc-1].Close>MAX(pAnalyData[nFirstLoc-2].Open,pAnalyData[nFirstLoc-2].Close))
					{	bStyle=TRUE;	break;	}
					nFirstLoc = E_TwinK_Style(pAnalyData,nSpan-(nNo-nFirstLoc),nFirstLoc-1,KLINET_SLIDE,1,1);
				}
				break;
			case KLINE_1082://高档阴包阳
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDU,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1083://高档最后包容阳线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUD,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1084://高档阴包阴
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDD,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1085://高档阴包剑形线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDD,1,1);
				if(nFirstLoc>0&&OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_SMALL&&OddK_Shadow(pAnalyData,nFirstLoc-1)==KLINEO_DVLSHADOW) { bStyle=TRUE; break; }
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDU,1,1);
				if(nFirstLoc>0&&OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_SMALL&&OddK_Shadow(pAnalyData,nFirstLoc-1)==KLINEO_DVLSHADOW) { bStyle=TRUE; break; }
				break;
			case KLINE_1086://高档阳孕阴
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUD,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1087://高档阳孕阳
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUU,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1088://高档阳孕十字星
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUL,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1089://高档阳孕吊颈线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUD,1,1);
				if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_DVLSHADOW)
					{ bStyle=TRUE; break; }
				break;
			case KLINE_1090://高档覆盖线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COVER,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1091://高档阳夹阴
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_NDU,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1092://高档下拖线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGDOWN,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1093://高档二次狙击性阴线
				nSecendLoc = E_TwinK_Style(pAnalyData,3*nSpan,nNo,KLINET_SLIDE,2,1);
				if(nSecendLoc!=-1)
				{	nFirstLoc = E_TwinK_Style(pAnalyData,3*nSpan,nSecendLoc+2,KLINET_ASCENT,1,0);
					if(nFirstLoc!=-1&&nFirstLoc==nSecendLoc+2)
					{	nThirdLoc = E_TwinK_Style(pAnalyData,3*nSpan+nSecendLoc-nNo,nSecendLoc-3,KLINET_ASCENT,1,0);
						if(nThirdLoc!=-1&&nThirdLoc<nSecendLoc)
						{ bStyle=TRUE; break; }
					}
				}
				break;
			case KLINE_1094://下档盘整后大阴突破
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc>0&&E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,1,0)<nFirstLoc-1)
					{ bStyle=TRUE; break; }
				break;
			case KLINE_1095://跳水一字线后不补缺
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&
				   OddK_UpDown(pAnalyData,nFirstLoc)==KLINEO_LEVEL&&
				   OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_NOHEADFOOT)
					{ bStyle=TRUE; break; }
				break;
			case KLINE_1096://二次跳空阴线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,2,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1097://跳空滑行线
			nSpan = nSpan;
				nSecendLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				while(nSecendLoc>1&&nSecendLoc<nNo)
				{   if(pAnalyData[nSecendLoc-1].Open <pAnalyData[nSecendLoc-2].Open &&
					   pAnalyData[nSecendLoc-1].Open >pAnalyData[nSecendLoc-2].Close&&
					   pAnalyData[nSecendLoc-1].Close<pAnalyData[nSecendLoc-2].Close&&
					   pAnalyData[nSecendLoc+1].Open <pAnalyData[nSecendLoc  ].Open &&
					   pAnalyData[nSecendLoc+1].Open >pAnalyData[nSecendLoc  ].Close&&
					   pAnalyData[nSecendLoc+1].Close<pAnalyData[nSecendLoc  ].Close)
					{	nFirstLoc=nSecendLoc+1; bStyle=TRUE; break;	}
					nSecendLoc = E_TwinK_Style(pAnalyData,nSpan-(nNo-nSecendLoc),nSecendLoc-1,KLINET_JMPDOWN,1,1);
				}
				break;
			case KLINE_1098://渐大三连阴
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,0,0,3,1);
				if(nFirstLoc!=-1&&nFirstLoc>1)
				{   if(pAnalyData[nFirstLoc].Open-pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-1].Open-pAnalyData[nFirstLoc-1].Close&&
					   pAnalyData[nFirstLoc-1].Open-pAnalyData[nFirstLoc-1].Close>pAnalyData[nFirstLoc-2].Open-pAnalyData[nFirstLoc-2].Close)
						{ bStyle=TRUE; break; }
				}
				break;
			case KLINE_1099://下跌中途跳水十字星
				nFirstLoc = nNo;
				do
				{	if(nFirstLoc>1&&
					   MIN(pAnalyData[nFirstLoc-2].Close,pAnalyData[nFirstLoc-2].Open)>MAX(pAnalyData[nFirstLoc-1].Close,pAnalyData[nFirstLoc-1].Open) &&
					   MIN(pAnalyData[nFirstLoc-1].Close,pAnalyData[nFirstLoc-1].Open)>MAX(pAnalyData[nFirstLoc  ].Close,pAnalyData[nFirstLoc  ].Open) &&
					   OddK_Shadow(pAnalyData,nFirstLoc  )!=KLINEO_NOHEADFOOT&&
					   OddK_Shadow(pAnalyData,nFirstLoc  )!=KLINEO_NOHEAD    &&
					   OddK_Shadow(pAnalyData,nFirstLoc  )!=KLINEO_NOFOOT    &&
					   OddK_Shadow(pAnalyData,nFirstLoc-1)!=KLINEO_NOHEADFOOT&&
					   OddK_Shadow(pAnalyData,nFirstLoc-1)!=KLINEO_NOHEAD    &&
					   OddK_Shadow(pAnalyData,nFirstLoc-1)!=KLINEO_NOFOOT    &&
					   OddK_Size  (pAnalyData,nFirstLoc  )==KLINEO_SMALL	 &&
					   OddK_Size  (pAnalyData,nFirstLoc-1)==KLINEO_SMALL	 )
					{	bStyle=TRUE;
						break;
					}
					nFirstLoc--;
				}
				while(nFirstLoc>1&&nFirstLoc>nNo-nSpan);
				break;
			case KLINE_1100://下跌中途跳水弓形线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_ULSHADOW)
					{ bStyle=TRUE; break; }
				break;
			case KLINE_1101://下跌中途跳水剑形线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_DVLSHADOW)
					{ bStyle=TRUE; break; }
				break;
			case KLINE_1102://下跌中途小奉承线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FLATTER,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1103://下跌中途伪阳线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNUP,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1104://下跌中途阴包阳
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDU,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1105://下跌中途反叛线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_REBELL,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1106://下跌中途下拖线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGDOWN,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1107://下跌中途阴包阳
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDU,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1108://下跌中途覆盖线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COVER,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1109://下跌中途阴孕阴
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDD,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1110://下跌中途并阳线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDUP,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1111://下跌中途并阴线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDDOWN,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1112://下跌中途攀援线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,1,1);
				if(nFirstLoc==nNo)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1113://下降三连击
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1);
				if(nFirstLoc!=-1&&nFirstLoc<nNo&&
				   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_UP&&
				   pAnalyData[nFirstLoc-2].Open<pAnalyData[nFirstLoc+1].Close)
				{ nFirstLoc++; bStyle=TRUE; break; }
				break;
			case KLINE_1114://下降三法
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,2,1);
				if(nFirstLoc>2&&nFirstLoc<nNo&&
				   OddK_UpDown(pAnalyData,nFirstLoc-3)==KLINEO_DOWN&&
				   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_DOWN&&
				   pAnalyData[nFirstLoc-2].Open>pAnalyData[nFirstLoc+1].Close)
				{ nFirstLoc++; bStyle=TRUE; break; }
				break;
			case KLINE_1115://下降跳空三法
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&nFirstLoc<nNo&&
				   E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGUP,1,1)==nFirstLoc+1)
					{ bStyle=TRUE; break; }
				break;
			case KLINE_1116://超越切入线或奉承线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUTIN,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FLATTER,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1117://下跌中途受阻黑三兵
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,0,0,3,1);
				if(nFirstLoc!=-1)
				{   if(pAnalyData[nFirstLoc].Open-pAnalyData[nFirstLoc].Close<pAnalyData[nFirstLoc-1].Open-pAnalyData[nFirstLoc-1].Close&&
					   pAnalyData[nFirstLoc-1].Open-pAnalyData[nFirstLoc-1].Close<pAnalyData[nFirstLoc-2].Open-pAnalyData[nFirstLoc-2].Close)
						{ bStyle=TRUE; break; }
				}
				break;
			case KLINE_1118://下跌中途思考黑三兵
				nFirstLoc = nNo;
				do
				{   if(nFirstLoc>1&&nFirstLoc>nNo-nSpan)
					{	if(OddK_UpDown(pAnalyData,nFirstLoc  )==KLINEO_DOWN &&
						   OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_DOWN &&
						   OddK_UpDown(pAnalyData,nFirstLoc-2)==KLINEO_DOWN &&
						   OddK_Size(pAnalyData,nFirstLoc  )==KLINEO_SMALL  &&
						   OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_BIG    &&
						   OddK_Size(pAnalyData,nFirstLoc-2)==KLINEO_SMALL  )
						{	bStyle=TRUE; break;
						}
					}
					nFirstLoc--;
				}
				while(nFirstLoc>1&&nFirstLoc>nNo-nSpan);
				break;
			case KLINE_1119://下跌中途阳夹阴
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_NUD,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1120://下跌中途阴包阴
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDD,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1121://下跌中途伪阴线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNDOWN,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			default        :
				break;
			}
			if(bStyle)
			{   j=0;
				while(j<nStyleKind&&nFirstLoc<=npLoc[j]) j++;
				if(j<nStyleKind)
				{	memmove(&npStyle[j],&npStyle[j+1],sizeof(short)*(nStyleKind-j));
					memmove(&npLoc  [j],&npLoc  [j+1],sizeof(long )*(nStyleKind-j));
				}
				npStyle[j] = tmpTable[tmpInt].MsgNo;
				npLoc  [j] = nFirstLoc;
				nStyleKind++;
			}
		}
	}
	memmove(npoutStyle,npStyle,sizeof(short)*nMaxNum);
	if(nStyleKind!=0&&npLoc[0]!=nNo) return 0;
	return nStyleKind;
}

//线性回归分析(返回斜率,fDiff返回相关系数,fBase返回截距)
//作回归分析但不作相关系数校正
//符合线性程度必须依靠fDiff判断
float CZnxg::LineRegress(float *fpOrigin,short nDataNum,float &fDiff,float &fBase)
{   
	float   fAverX;
	float	fAverY;
	float   fLxx;
	float   fLxy;
	float   fLyy;
	short   i;
	fAverX = (nDataNum-1.0)/2.0;
	fAverY = 0.0;
	for(i=0; i<nDataNum; i++)  fAverY+=fpOrigin[i];
	fAverY = fAverY/nDataNum;
	fLxx = 0.0;
	fLxy = 0.0;
	fLyy = 0.0;
	for(i=0; i<nDataNum; i++) fLxx += (i-fAverX)*(i-fAverX);
	for(i=0; i<nDataNum; i++) fLxy += (i-fAverX)*(fpOrigin[i]-fAverY);
	for(i=0; i<nDataNum; i++) fLyy += (fpOrigin[i]-fAverY)*(fpOrigin[i]-fAverY);
	fBase = fAverY-fAverX*fLxy/fLxx;
	fDiff = fLxy/sqrt(fLxx*fLyy);
	return fLxy/fLxx;
}

//取最大最小值
void CZnxg::MaxMinVal(float *fpOrigin,short nDataNum,float &fMax,float &fMin)
{
	fMax = fpOrigin[0];
	fMin = fpOrigin[0];
	for(short i=0; i<nDataNum; i++)
	{   if(fpOrigin[i]>fMax) fMax=fpOrigin[i];
		if(fpOrigin[i]<fMin) fMin=fpOrigin[i];
	}
}

//判断一段K线中是否存在指定的单根K线形态
//时间段为pAnalyData[nNo-nSpan+1<=(...)<=nNo]
//nMode==1为连续统计,nMode==0为不判断连续统计
//若存在nKLineStyle形态的K线形态返回序号
//否则返回-1
short CZnxg::E_OddK_Style(struct AnalyData *pAnalyData,short nSpan,short nNo,short nUpDown,short nSize,short nShadow,short nWeight,short nMode)
{	
	short   cCount=0;
	short   nFirstLoc;
	if(nSpan<0||nNo<0) return -1;
	for(short i=nNo; i>MAX(nNo-nSpan,0); i--)
	{	if((nUpDown==0||OddK_UpDown(pAnalyData,i)==nUpDown) &&
		   (nSize  ==0||OddK_Size  (pAnalyData,i)==nSize  ) &&
		   (nShadow==0||OddK_Shadow(pAnalyData,i)==nShadow) )
		{	cCount++;
			if(cCount==1) 		nFirstLoc=i;    //确定第一个符合的形态的位置
			if(cCount>=nWeight) break;			//已完成查找
		}
		else
		{	if(nMode==1)  cCount=0;		//连续统计
		}
	}
	if(cCount>=nWeight) return nFirstLoc;
	return -1;
}

//判断一段K线中是否存在指定的nWeight根K线形态
//时间段为pAnalyData[nNo-nSpan+1<=(...)<=nNo]
//nMode==1为连续统计,nMode==0为不判断连续统计
//若存在nKStyle形态的K线形态返回序号
//否则返回-1
short CZnxg::E_TwinK_Style(struct AnalyData *pAnalyData,short nSpan,short nNo,short nKStyle,short nWeight,short nMode)
{	
	short   cCount=0;
	short   nFirstLoc;
	for(short i=nNo; i>MAX(nNo-nSpan,0); i--)
	{	if(isTwinK_Style(pAnalyData,i,nKStyle)==nKStyle)
		{
			cCount++;
			if(cCount==1) 		nFirstLoc=i;    //确定第一个符合的形态的位置
			if(cCount>=nWeight) break;			//已完成查找
		}
		else
		{	if(nMode==1)  cCount=0;		//连续统计
		}
	}
	if(cCount>=nWeight) return nFirstLoc;
	return -1;
}

//参考依据:
//1.长短		长度比
//2.涨幅		幅度比
//3.涨速		速度比,水平度
//4.价位		价位比
//5.符号		符号比
//6.密度		密度

//判断pAnalyData[0,...,nDataNum-1]中序号为nNo,跨度为nSpan的K线区段的形态
//其中移动平均高低档运算以6*nSpan计算,
short CZnxg::isTrendK_Style(MemStkInfo *tmpInfo,struct AnalyData *pAnalyData,short nDataNum,short nSpan,short nNo,short nKStyle)
{	
	short    naLoc[60*2];     //段末位置
	short	 naLen[60*2];     //段长
	float    faSlope[60*2];	//斜率(速度)
	float    faWeight[60*2];  //段权重(涨幅和,相对本段)
	float    faDiff[60*2];  	//段涨幅
	float    fPriceLv;		//价位比
	short    nNodeNum;
	short    nNearNode;
	short    nLastNear;
	short    i,j;
	if(nDataNum<6*nSpan) return 0;

	HISDAT *tmpHisData = new HISDAT[nDataNum];
	memset(tmpHisData, 0, nDataNum*sizeof(HISDAT));
	RawData2CalcData(tmpInfo->Code,tmpInfo->setcode,PER_DAY,pAnalyData,tmpHisData,nDataNum,g_NoTime); //转成TCALC所认识的数据
	int 	nRealNum = g_pCalc->_CallDTopViewCalc_3(tmpInfo->Code,tmpInfo->setcode,PER_DAY,tmpHisData,nDataNum);
	delete []tmpHisData;
	if(nRealNum < 0)  return 0;
	float *pOutData[MAX_LINEOUT];
	for(j=0; j<MAX_LINEOUT;j++) 
		pOutData[j] = g_pCalc->_CallDTopViewCalc_13(nRealNum);
	int IndexNo = g_pCalc->_CallDTopViewCalc_9(ZB_TYPE,"ZXNH");
	if(IndexNo < 0)
	{
		for (j=0; j<MAX_LINEOUT; j++) g_pCalc->_CallDTopViewCalc_14(pOutData[j]);
		return 0;
	}
	g_pCalc->_CallDTopViewCalc_5(ZB_TYPE, IndexNo, pOutData);

	nLastNear	= nRealNum-1;
	nNearNode	= nRealNum-2;
	nNodeNum	= 0;
	while(nNearNode>=0&&nNearNode<nRealNum)
	{	while(nNearNode>=0&&nNearNode<nRealNum)
		{	if(nNearNode==0) break;
			if(fabs(pOutData[0][nNearNode])<0.05) { nNearNode--; continue; }
			if(nLastNear-nNearNode<2) { nNearNode--; continue; }
			break;
		}
		if(nNearNode>=0&&nNearNode<nRealNum)
		{
			naLoc[nNodeNum]     = nNearNode;
			naLen[nNodeNum]		= nLastNear-nNearNode+1;
			faDiff[nNodeNum]	= pAnalyData[nLastNear].Close-pAnalyData[nNearNode].Close;
			if(naLen[nNodeNum]*pAnalyData[nNearNode].Close>COMPPREC)
				faSlope[nNodeNum]	= faDiff[nNodeNum]/(naLen[nNodeNum]*pAnalyData[nNearNode].Close);
			faWeight[nNodeNum]  = 0.0;
			for(i=nNearNode; i<=nLastNear; i++)
			{
				if(pAnalyData[nLastNear].Close>COMPPREC)
					faWeight[nNodeNum] += (pAnalyData[i].Close-pAnalyData[nNearNode].Close)/pAnalyData[nLastNear].Close;
			}
			nNodeNum++;
			nLastNear = nNearNode;
			nNearNode --;
		}
	}
	for (j=0; j<MAX_LINEOUT; j++) g_pCalc->_CallDTopViewCalc_14(pOutData[j]);

	float	fMax;
	float	fMin;
	fMax = pAnalyData[0].Close;
	fMin = pAnalyData[0].Close;
	for(i=1; i<nDataNum; i++)
	{   if(fMax<pAnalyData[i].Close) fMax = pAnalyData[i].Close;
		if(fMin>pAnalyData[i].Close) fMin = pAnalyData[i].Close;
	}
	fPriceLv = 0.0;
	for(i=nDataNum-5; i<nDataNum; i++)	fPriceLv += pAnalyData[i].Close;
	fPriceLv = fPriceLv/5.0;
	if(fabs(fMax-fMin)>COMPPREC) fPriceLv = (fPriceLv-fMin)/(fMax-fMin);

	float HIGHPRICERATE	=	 0.80f;			//高档
	float LOWPRICERATE	=	 0.30f;			//低档
	float UPLIMIT		=	 COMPPREC;		//上升途中(幅度)
	float DOWNLIMIT		=	-COMPPREC;		//下跌途中(幅度)
	float UPLIMITW		=	 COMPPREC;		//上升途中(权重)
	float DOWNLIMITW	=	-COMPPREC;		//下跌途中(权重)
	float DULLRATE		=	 0.15f*nDataNum;//横盘比数
	float DULLLEN		=    0.90f*nSpan;	//横盘跨距
	float FASTUPLIMIT	=	 0.03f;			//急升(幅度)
	float FASTDOWNLIMIT	=	-0.03f;			//急跌(幅度)
	float SLOWUPLIMIT	=	 0.025f;		//缓升(幅度)
	float SLOWDOWNLIMIT	=	-0.025f;		//缓跌(幅度)
	float FASTUPLIMITW	=	 0.03f;			//急升(权重)
	float FASTDOWNLIMITW=	-0.03f;			//急跌(权重)
	float SLOWUPLIMITW	=	 0.025f;		//缓升(权重)
	float SLOWDOWNLIMITW=	-0.025f;		//缓跌(权重)
	switch(nKStyle)
	{
	case KLINETL_HIGH:
	//高档(*),短期均价相对长期最高最低中心价而言,
		if(fPriceLv>HIGHPRICERATE)	return nKStyle;
		break;
	case KLINETL_LOW:
	//低档(*),短期均价相对长期最高最低中心价而言,
		if(fPriceLv<LOWPRICERATE)	return nKStyle;
		break;
	case KLINETL_UP:
	//上升途中(*),短期均价相对长期最高最低中心价而言,
		if(fPriceLv>HIGHPRICERATE)	return 0;
		if(fPriceLv<LOWPRICERATE )	return 0;
		if((faSlope[0]>UPLIMIT||faWeight[0]>UPLIMITW)&&(naLen[0]>DULLLEN||nNodeNum<DULLRATE))
			return nKStyle;
		break;
	case KLINETL_DOWN:
	//下跌途中(*),短期均价相对长期最高最低中心价而言
		if(fPriceLv>HIGHPRICERATE)	return 0;
		if(fPriceLv<LOWPRICERATE )	return 0;
		if((faSlope[0]<DOWNLIMIT||faWeight[0]<DOWNLIMITW)&&(naLen[0]>DULLLEN||nNodeNum<DULLRATE))
			return nKStyle;
		break;
	case KLINETL_TOSS:
	case KLINETL_DULL:
	//横盘(*),对大区段而言
		if(nNodeNum>DULLRATE)	return nKStyle;
		if(faWeight[0]<UPLIMITW&&faWeight[0]>DOWNLIMITW) return nKStyle;
		break;
	case KLINETL_HIGHTOSS:
	//高档震荡(*)
	case KLINETL_HIGHDULL:
	//高档横盘(*)
		if(fPriceLv<HIGHPRICERATE)	return 0;
		if(nNodeNum>DULLRATE)	return nKStyle;
		if(faWeight[0]<UPLIMITW&&faWeight[0]>DOWNLIMITW) return nKStyle;
		break;
	case KLINETL_LOWTOSS:
	//低档震荡(*)
	case KLINETL_LOWDULL:
	//低档横盘(*)
		if(fPriceLv>LOWPRICERATE)	return 0;
		if(nNodeNum>DULLRATE)	return nKStyle;
		if(faWeight[0]<UPLIMITW&&faWeight[0]>DOWNLIMITW) return nKStyle;
		break;
	case KLINETL_FASTUP:
	//急升(*)
		if(faSlope[0]>FASTUPLIMIT)   return nKStyle;
		break;
	case KLINETL_FASTDOWN:
	//急跌(*)
		if(faSlope[0]<FASTDOWNLIMIT) return nKStyle;
		break;
	case KLINETL_SLOWUP:
	//缓升(*)
		if(faSlope[0]>SLOWUPLIMIT&&faWeight[0]>SLOWUPLIMITW) return 0;
		if(faSlope [0]>UPLIMIT)		return nKStyle;
		if(faWeight[0]>UPLIMITW)	return nKStyle;
		break;
	case KLINETL_SLOWDOWN:
	//缓跌(*)
		if(faSlope[0]<SLOWDOWNLIMIT&&faWeight[0]<SLOWDOWNLIMITW) return 0;
		if(faSlope [0]<DOWNLIMIT)		return nKStyle;
		if(faWeight[0]<DOWNLIMITW)		return nKStyle;
		break;
	case KLINETL_HIGHFASTUP:
	//高档急升
		if(fPriceLv<HIGHPRICERATE)		return 0;
		if(faSlope[0]>FASTUPLIMIT)   	return nKStyle;
		break;
	case KLINETL_HIGHFASTDOWN:
	//高档急跌
		if(fPriceLv<HIGHPRICERATE)		return 0;
		if(faSlope[0]<FASTDOWNLIMIT) 	return nKStyle;
		break;
	case KLINETL_HIGHSLOWUP:
	//高档缓升
		if(fPriceLv<HIGHPRICERATE) return 0;
		if(faSlope[0]<SLOWUPLIMIT&&faSlope[0]>UPLIMIT)			return nKStyle;
		if(faWeight[0]<SLOWUPLIMITW&&faWeight[0]>UPLIMITW)		return nKStyle;
		break;
	case KLINETL_HIGHSLOWDOWN:
	//高档缓跌
		if(fPriceLv<HIGHPRICERATE)		return 0;
		if(faSlope[0]>SLOWDOWNLIMIT&&faSlope[0]<DOWNLIMIT)		return nKStyle;
		if(faWeight[0]>SLOWDOWNLIMITW&&faWeight[0]<DOWNLIMITW)	return nKStyle;
		break;
	case KLINETL_LOWFASTUP:
	//低档急升
		if(fPriceLv>LOWPRICERATE)		return 0;
		if(faSlope[0]>FASTUPLIMIT)   	return nKStyle;
		break;
	case KLINETL_LOWFASTDOWN:
	//低档急跌
		if(fPriceLv>LOWPRICERATE)		return 0;
		if(faSlope[0]<FASTDOWNLIMIT) 	return nKStyle;
		break;
	case KLINETL_LOWSLOWUP:
	//低档缓升
		if(fPriceLv>LOWPRICERATE)		return 0;
		if(faSlope[0]<SLOWUPLIMIT&&faSlope[0]>UPLIMIT)			return nKStyle;
		if(faWeight[0]<SLOWUPLIMITW&&faWeight[0]>UPLIMITW)		return nKStyle;
		break;
	case KLINETL_LOWSLOWDOWN:
	//低档缓跌
		if(fPriceLv>LOWPRICERATE)		return 0;
		if(faSlope[0]>SLOWDOWNLIMIT&&faSlope[0]<DOWNLIMIT)		return nKStyle;
		if(faWeight[0]>SLOWDOWNLIMITW&&faWeight[0]<DOWNLIMITW)	return nKStyle;
		break;

	case KLINETL_HIGHTOSS1:
	//高档震荡初期
		break;
	case KLINETL_HIGHTOSS2:
	//高档震荡中期
		break;
	case KLINETL_HIGHTOSS3:
	//高档震荡末期
		break;
	case KLINETL_LOWTOSS1:
	//低档震荡初期
		break;
	case KLINETL_LOWTOSS2:
	//低档震荡中期
		break;
	case KLINETL_LOWTOSS3:
	//低档震荡末期
		break;
	case KLINETL_HIGHDULL1:
	//高档横盘初期
		break;
	case KLINETL_HIGHDULL2:
	//高档横盘中期
		break;
	case KLINETL_HIGHDULL3:
	//高档横盘末期
		break;
	case KLINETL_LOWDULL1:
	//低档横盘初期
		break;
	case KLINETL_LOWDULL2:
	//低档横盘中期
		break;
	case KLINETL_LOWDULL3:
	//低档横盘末期
		break;
	}
	return 0;
}

short CZnxg::GetKLineStyleInside(struct AnalyData *pAnalyData,short nDataNum,short nSpan,short nNo,short nTrendKStyle)
{
	short   nFirstLoc;
	short   nSecendLoc;
	short   nThirdLoc;
	switch(nTrendKStyle)
	{
	case KLINETL_HIGH:
	//高档(*)
		//上档盘整后大阳突破
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
		if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG) return KLINE_1032;
		//跳高一字线后不补缺
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
		if(nFirstLoc!=-1&&E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_LEVEL,KLINEO_SMALL,KLINEO_NOHEADFOOT,1,1)==nFirstLoc) return KLINE_1033;
		//二次高开阳线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,2,1)!=-1) return KLINE_1034;
		//跳空攀援线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
		if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,1,1)==nFirstLoc+1) return KLINE_1035;
		//高档多条并列阳线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDUP,4,1)!=-1) return KLINE_1059;
		//高档多根并列阴线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDDOWN,4,1)!=-1) return KLINE_1060;
		//反击滑行线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
		if(nFirstLoc>=0&&nFirstLoc+1==E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_BIG,0,1,1) &&
		   pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-2].Open) return KLINE_1061;
		//高档横盘中上跳大阴线
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,1,1);
		if(nFirstLoc>0&&nFirstLoc<nNo&&
		   OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_SMALL&&OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP&&
		   OddK_Size(pAnalyData,nFirstLoc+1)==KLINEO_SMALL&&OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_UP&&
		   pAnalyData[nFirstLoc].Open>pAnalyData[nFirstLoc-1].Open&& pAnalyData[nFirstLoc].Open>pAnalyData[nFirstLoc+1].Open &&
		   pAnalyData[nFirstLoc].Close<pAnalyData[nFirstLoc-1].Close&&pAnalyData[nFirstLoc].Close<pAnalyData[nFirstLoc+1].Close)
			return KLINE_1062;
		//高档横盘中弓形线
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_SMALL,KLINEO_UVLSHADOW,1,1)!=-1) return KLINE_1063;
		//高档横盘中剑形线
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1)!=-1) return KLINE_1064;
		//滑行线接滑行线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
		if(nFirstLoc>3&&E_TwinK_Style(pAnalyData,nSpan,nFirstLoc-2,KLINET_SLIDE,1,1)==nFirstLoc-3&&
		   OddK_UpDown(pAnalyData,nFirstLoc-2)==KLINEO_UP)
			return KLINE_1065;
		//见顶三鸦
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNDOWN,2,1);
		if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1)==nFirstLoc+1) return KLINE_1066;
		//小幅盘升后大阴向下
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,1,1)!=-1) return KLINE_1070;
		//暴跌三鸦
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1)!=-1) return KLINE_1071;
		//炮楼顶
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,2,1);
		if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1)>nFirstLoc) return KLINE_1072;
		//高档弃十字星
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
		if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1)==nFirstLoc+2&&
		   OddK_Size(pAnalyData,nFirstLoc+1)==KLINEO_SMALL&&
		   OddK_Shadow(pAnalyData,nFirstLoc+1)!=KLINEO_NOHEAD&&
		   OddK_Shadow(pAnalyData,nFirstLoc+1)!=KLINEO_NOFOOT)
			return KLINE_1075;
		//高档弃弓形线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ABANDON,1,1);
		if(nFirstLoc!=-1&&nFirstLoc<nNo)
		{	if(OddK_UpDown(pAnalyData,nFirstLoc)==KLINEO_UP       &&
			   OddK_Size  (pAnalyData,nFirstLoc)==KLINEO_SMALL    &&
			   OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_UVLSHADOW&&
			   pAnalyData[nFirstLoc+1].Close<pAnalyData[nFirstLoc].Close)
				return KLINE_1076;
		}
		//高档尽头阳线
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_BIG,KLINEO_ULSHADOW,1,1);
		if(nFirstLoc!=-1&&nFirstLoc<nNo)
		{   if(OddK_UpDown(pAnalyData,nFirstLoc)==KLINEO_UP&&
			   pAnalyData[nFirstLoc+1].Open >pAnalyData[nFirstLoc].Close&&
			   pAnalyData[nFirstLoc+1].Close<pAnalyData[nFirstLoc].High)
				return KLINE_1078;
		}
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_NORMAL,KLINEO_ULSHADOW,1,1);
		if(nFirstLoc!=-1&&nFirstLoc<nNo)
		{   if(OddK_UpDown(pAnalyData,nFirstLoc)==KLINEO_UP&&
			   pAnalyData[nFirstLoc+1].Open >pAnalyData[nFirstLoc].Close&&
			   pAnalyData[nFirstLoc+1].Close<pAnalyData[nFirstLoc].High)
				return KLINE_1078;
		}
		//绝顶独阴线
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,1,1);
		while(nFirstLoc>0)
		{   if(nFirstLoc<nNo&&OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP&&
			   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_UP&&
			   pAnalyData[nFirstLoc].Open>pAnalyData[nFirstLoc-1].Close&&
			   pAnalyData[nFirstLoc].Open>pAnalyData[nFirstLoc+1].Close)
				return KLINE_1079;
			nFirstLoc = E_OddK_Style(pAnalyData,nSpan-(nNo-nFirstLoc),nFirstLoc,KLINEO_DOWN,KLINEO_BIG,0,1,1);
		}
		//绝顶独阳线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
		if(nFirstLoc!=-1)
		{	if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNUP,1,1)==nFirstLoc+1||
			   E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_REBELL ,1,1)==nFirstLoc+1)
				return KLINE_1080;
		}
		//高档跳空滑行线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
		if(nFirstLoc>0)
		{	if(OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP&&
			   pAnalyData[nFirstLoc-1].Close<pAnalyData[nFirstLoc-1].Close)
				return KLINE_1081;
		}
		//高档阴包阳
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDU,1,1)!=-1) return KLINE_1082;
		//高档最后包容阳线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUD,1,1)!=-1) return KLINE_1083;
		//高档阴包剑形线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDD,1,1);
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_SMALL,KLINEO_DLSHADOW,1,1)==nFirstLoc&&
		   nFirstLoc!=-1)
			return KLINE_1085;
		//高档阴包阴
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDD,1,1)!=-1) return KLINE_1084;
		//高档阳孕阳
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUU,1,1)!=-1) return KLINE_1087;
		//高档阳孕十字星
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUL,1,1);
		if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOHEAD&&
		   OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOFOOT)
			return KLINE_1088;
		//高档阳孕吊颈线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUD,1,1);
		if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_DVLSHADOW)
			return KLINE_1089;
		//高档阳孕阴
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUD,1,1)!=-1) return KLINE_1086;
		//高档覆盖线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COVER,1,1)!=-1) return KLINE_1090;
		//高档阳夹阴
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_NDU,1,1)!=-1) return KLINE_1091;
		//高档下拖线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGDOWN,1,1)!=-1) return KLINE_1092;
		//高档二次狙击性阴线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1);
		if(nFirstLoc!=-1)
		{	nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,1,0);
			if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,1,0)!=nFirstLoc)
			{   if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,0,0,1,0)>nFirstLoc) return KLINE_1093;
			}
		}
		break;
	case KLINETL_LOW:
	//低档(*)
		//低档多条并列阳线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDUP,4,1)!=-1) return KLINE_1001;
		//低档多条并列阴线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDDOWN,4,1)!=-1) return KLINE_1002;
		//低档二次狙击性阳线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,2,1);
		if(nFirstLoc!=-1)
		{	nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,0);
			if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,0)!=nFirstLoc)
			{   if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,0,0,1,0)>nFirstLoc) return KLINE_1031;
			}
		}
		break;
	case KLINETL_TOSS:
	//震荡(*)
		break;
	case KLINETL_DULL:
	//横盘(*)
		break;
	case KLINETL_UP:
	//上升途中(*)
		//渐大三连阳
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,0,0,3,1);
		if(nFirstLoc>2&&
		   (pAnalyData[nFirstLoc].Close-pAnalyData[nFirstLoc].Open)>(pAnalyData[nFirstLoc-1].Close-pAnalyData[nFirstLoc-1].Open+COMPPREC) &&
		   (pAnalyData[nFirstLoc-1].Close-pAnalyData[nFirstLoc-1].Open)>(pAnalyData[nFirstLoc-2].Close-pAnalyData[nFirstLoc-2].Open+COMPPREC))
			return KLINE_1036;
		//上升中途跳高十字星
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,2,1);
		if(nFirstLoc!=-1&&E_OddK_Style(pAnalyData,nSpan,nNo,0,KLINEO_LEVEL,0,2,1)==nFirstLoc) return KLINE_1037;
		//上升中途跳高剑形线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
		if(nFirstLoc!=-1&&E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1)==nFirstLoc) return KLINE_1038;
		//上升中途跳高弓形线
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_SMALL,KLINEO_UVLSHADOW,1,1);
		if(nFirstLoc>0&&OddK_UpDown(pAnalyData,nFirstLoc)==KLINEO_UP&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG) return KLINE_1039;
		//上升中途小压迫线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CRACK,1,1);
		if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL) return KLINE_1040;
		//上升中途伪阴线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNDOWN,1,1)!=-1) return KLINE_1041;
		//上升中途阳包阴
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUD,1,1)!=-1) return KLINE_1042;
		//上升中途归顺线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SURRENDER,1,1)!=-1) return KLINE_1043;
		//上升中途上拉线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGUP,1,1)!=-1) return KLINE_1044;
		//上升中途阳孕阴
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUD,1,1)!=-1) return KLINE_1045;
		//上升中途切入线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUTIN,1,1)!=-1) return KLINE_1046;
		//上升中途阳孕阳
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUU,1,1)!=-1) return KLINE_1047;
		//上升中途并阳线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDUP,3,1)!=-1) return KLINE_1048;
		//上升中途滑行线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1)!=-1) return KLINE_1049;
		//上升三连击
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,2,1);
		if(nFirstLoc>0&&nFirstLoc<nNo&&
		   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_DOWN&&
		   OddK_Size  (pAnalyData,nFirstLoc+1)==KLINEO_BIG&&
		   pAnalyData[nFirstLoc-2].Open>pAnalyData[nFirstLoc+1].Close)
			return KLINE_1050;
		//上升三法
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1);
		if(nFirstLoc!=-1&&nFirstLoc<nNo&&pAnalyData[nFirstLoc-3].Open<pAnalyData[nFirstLoc+1].Close) return KLINE_1051;
		//上升跳空三法
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,2,1);
		if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGDOWN,1,1)==nFirstLoc+2) return KLINE_1052;
		//超越覆盖线或压迫线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,1,1);
		if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,3,1)==nFirstLoc-5&&
		   pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-5].Open) return KLINE_1053;
		//上升中途 受阻红三兵
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,0,0,3,1);
		if(nFirstLoc!=-1&&
		  (pAnalyData[nFirstLoc].Close-pAnalyData[nFirstLoc].Open)<(pAnalyData[nFirstLoc-1].Close-pAnalyData[nFirstLoc-1].Open) &&
		  (pAnalyData[nFirstLoc-1].Close-pAnalyData[nFirstLoc-1].Open)<(pAnalyData[nFirstLoc-2].Close-pAnalyData[nFirstLoc-2].Open))
			return KLINE_1054;
		//上升中途 思考红三兵
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,0,0,3,1);
		if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc-2)==KLINEO_SMALL&&
		  OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_BIG &&
		  OddK_Size(pAnalyData,nFirstLoc  )==KLINEO_SMALL)
			return KLINE_1055;
		//上升中途阴夹阳
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_NDU,1,1)!=-1) return KLINE_1056;
		//上升中途阳包阳
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUU,1,1)!=-1) return KLINE_1057;
		//上升中途伪阳线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNUP,1,1)!=-1) return KLINE_1058;
		break;
	case KLINETL_DOWN:
	//下跌途中(*)
		//二次跳空阴线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,2,1)!=-1)
			return KLINE_1096;
		//跳空滑行线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
		if(nFirstLoc>1)
		{	nSecendLoc = E_TwinK_Style(pAnalyData,nSpan,nFirstLoc-1,KLINET_JMPDOWN,1,1);
			if(nSecendLoc==nFirstLoc-1)
			{   nThirdLoc = E_TwinK_Style(pAnalyData,nSpan,nSecendLoc-1,KLINET_JMPDOWN,1,1);
				if(nThirdLoc>0&&nThirdLoc==nFirstLoc-1) return KLINE_1097;
			}
		}
		//渐大三连阴
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,0,0,3,1);
		if(nFirstLoc!=-1)
		{   if(pAnalyData[nFirstLoc].Open-pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-1].Open-pAnalyData[nFirstLoc-1].Close&&
			   pAnalyData[nFirstLoc-1].Open-pAnalyData[nFirstLoc-1].Close<pAnalyData[nFirstLoc-2].Open-pAnalyData[nFirstLoc-2].Close)
				return KLINE_1098;
		}
		//下跌中途跳水十字星
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_LEVEL,0,0,3,1);
		if(nFirstLoc!=-1)
		{   if(OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOHEADFOOT&&
			   OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOHEAD    &&
			   OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOFOOT    &&
			   OddK_Shadow(pAnalyData,nFirstLoc-1)!=KLINEO_NOHEADFOOT&&
			   OddK_Shadow(pAnalyData,nFirstLoc-1)!=KLINEO_NOHEAD    &&
			   OddK_Shadow(pAnalyData,nFirstLoc-1)!=KLINEO_NOFOOT    &&
			   OddK_Shadow(pAnalyData,nFirstLoc-2)!=KLINEO_NOHEADFOOT&&
			   OddK_Shadow(pAnalyData,nFirstLoc-2)!=KLINEO_NOHEAD    &&
			   OddK_Shadow(pAnalyData,nFirstLoc-2)!=KLINEO_NOFOOT    )
				return KLINE_1099;
		}
		//下跌中途跳水弓形线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
		if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_ULSHADOW)
			return KLINE_1100;
		//下跌中途跳水剑形线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
		if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_DLSHADOW)
			return KLINE_1101;
		//下跌中途小奉承线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FLATTER,1,1)!=-1)
			return KLINE_1102;
		//下跌中途伪阳线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNUP,1,1)!=-1)
			return KLINE_1103;
		//下跌中途阴包阳
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDU,1,1)!=-1)
			return KLINE_1104;
		//下跌中途反叛线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_REBELL,1,1)!=-1)
			return KLINE_1105;
		//下跌中途下拖线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGDOWN,1,1)!=-1)
			return KLINE_1106;
		//下跌中途阴包阳
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDU,1,1)!=-1)
			return KLINE_1107;
		//下跌中途覆盖线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COVER,1,1)!=-1)
			return KLINE_1108;
		//下跌中途阴孕阴
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDD,1,1)!=-1)
			return KLINE_1109;
		//下跌中途并阳线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDUP,1,1)==nNo)
			return KLINE_1110;
		//下跌中途并阴线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDDOWN,1,1)==nNo)
			return KLINE_1111;
		//下跌中途攀援线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,1,1)==nNo)
			return KLINE_1112;
		//下降三连击
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1);
		if(nFirstLoc>0&&nFirstLoc<nNo&&
		   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_UP&&
		   OddK_Size  (pAnalyData,nFirstLoc+1)==KLINEO_BIG&&
		   pAnalyData[nFirstLoc-2].Open<pAnalyData[nFirstLoc+1].Close)
			return KLINE_1113;
		//下降三法
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,2,1);
		if(nFirstLoc>0&&nFirstLoc<nNo&&
		   OddK_Size  (pAnalyData,nFirstLoc-3)==KLINEO_BIG&&
		   OddK_Size  (pAnalyData,nFirstLoc+1)==KLINEO_BIG&&
		   OddK_UpDown(pAnalyData,nFirstLoc-3)==KLINEO_DOWN&&
		   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_DOWN)
			return KLINE_1114;
		//下降跳空三法
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
		if(nFirstLoc!=-1&&nFirstLoc<nNo&&
		   E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGUP,1,1)==nFirstLoc+1)
			return KLINE_1115;
		//超越切入线或奉承线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUTIN,1,1)!=-1)
			return KLINE_1116;
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FLATTER,1,1)!=-1)
			return KLINE_1116;
		//下跌中途受阻黑三兵
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,0,0,3,1);
		if(nFirstLoc!=-1)
		{   if(pAnalyData[nFirstLoc].Open-pAnalyData[nFirstLoc].Close<pAnalyData[nFirstLoc-1].Open-pAnalyData[nFirstLoc-1].Close&&
			   pAnalyData[nFirstLoc-1].Open-pAnalyData[nFirstLoc-1].Close<pAnalyData[nFirstLoc-2].Open-pAnalyData[nFirstLoc-2].Close)
				return KLINE_1117;
		}
		//下跌中途思考黑三兵
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,0,0,5,1);
		if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL&&
		   OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_BIG)
			return KLINE_1118;
		//下跌中途阳夹阴
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_NUD,1,1)==nNo)
			return KLINE_1119;
		//下跌中途阴包阴
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDD,1,1)==nNo)
			return KLINE_1120;
		//下跌中途伪阴线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNDOWN,1,1)==nNo)
			return KLINE_1121;
		break;
	case KLINETL_HIGHTOSS:
	//高档震荡(*)
		break;
	case KLINETL_LOWTOSS:
	//低档震荡(*)
		break;
	case KLINETL_HIGHDULL:
	//高档横盘(*)
		break;
	case KLINETL_LOWDULL:
	//低档横盘(*)
		//低档横盘中剑形线
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1)!=-1) return KLINE_1003;
		//低档横盘中弓形线
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_SMALL,KLINEO_UVLSHADOW,1,1)!=-1) return KLINE_1004;
		//低档横盘中下跳大阳线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FAKECUTIN,1,1);
		if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG) return KLINE_1005;
		//低档横盘中上跳大阴线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FAKECOVER,1,1);
		if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG) return KLINE_1006;
		//低档横盘末期红三兵
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_SMALL,0,3,1)==nNo) return KLINE_1011;
		//下档盘整后大阴突破
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
		if(nFirstLoc>0&&E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,1,0)<nFirstLoc-1)
			return KLINE_1094;
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
		if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_NOHEADFOOT)
			return KLINE_1095;
		break;
	case KLINETL_FASTUP:
	//急升(*)
		break;
	case KLINETL_FASTDOWN:
	//急跌(*)
		break;
	case KLINETL_SLOWUP:
	//缓升(*)
		break;
	case KLINETL_SLOWDOWN:
	//缓跌(*)
		break;
	case KLINETL_HIGHTOSS1:
	//高档震荡初期
		break;
	case KLINETL_LOWTOSS1:
	//低档震荡初期
		break;
	case KLINETL_HIGHTOSS2:
	//高档震荡中期
		break;
	case KLINETL_LOWTOSS2:
	//低档震荡中期
		break;
	case KLINETL_HIGHTOSS3:
	//高档震荡末期
		break;
	case KLINETL_LOWTOSS3:
	//低档震荡末期
		break;
	case KLINETL_HIGHDULL1:
	//高档横盘初期
		break;
	case KLINETL_LOWDULL1:
	//低档横盘初期
		break;
	case KLINETL_HIGHDULL2:
	//高档横盘中期
		break;
	case KLINETL_LOWDULL2:
	//低档横盘中期
		break;
	case KLINETL_HIGHDULL3:
	//高档横盘末期
		//高档盘整末期剑形线
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1)!=-1) return KLINE_1067;
		//高档盘整末期大阴向下
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,1,1)!=-1) return KLINE_1068;
		//高档盘整末期黑三兵
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,3,1)!=-1) return KLINE_1069;
		break;
	case KLINETL_LOWDULL3:
	//低档横盘末期
		//低档横盘末期弓形线
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_SMALL,KLINEO_UVLSHADOW,1,1)!=-1) return KLINE_1007;
		//低档横盘末期剑形线
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1)!=-1) return KLINE_1008;
		//低档横盘末期大阳突破
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_BIG,0,1,1)==nNo) return KLINE_1009;
		break;
	case KLINETL_HIGHFASTUP:
	//高档急升
		//高档急升三大阳
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,3,1)!=-1) return KLINE_1073;
		//三空阳线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,3,1)!=-1) return KLINE_1074;
		//高档弃吊劲线
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_SMALL,KLINEO_DLSHADOW,1,0);
		if(nFirstLoc>0&&nFirstLoc<nNo)
		{	if(OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP  &&
			   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_DOWN&&
			   pAnalyData[nFirstLoc-1].Close<pAnalyData[nFirstLoc].Close&&
			   pAnalyData[nFirstLoc+1].Open <pAnalyData[nFirstLoc].Close)
				return KLINE_1077;
		}
		break;
	case KLINETL_HIGHFASTDOWN:
	//高档急跌
		break;
	case KLINETL_HIGHSLOWUP:
	//高档缓升
		break;
	case KLINETL_HIGHSLOWDOWN:
	//高档缓跌
		break;
	case KLINETL_LOWFASTUP:
	//低档急升
		break;
	case KLINETL_LOWFASTDOWN:
	//低档急跌
		//低档急跌三大阴
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,3,1);
		if(nFirstLoc!=-1&&nFirstLoc==E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,3,1)) return KLINE_1012;
		//三空阴线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,3,1)!=-1) return KLINE_1013;
		//低档急跌后下跳十字星
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1);
		if( nFirstLoc<nNo &&
			E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1)==nFirstLoc+1 &&
			OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_LEVEL  &&
			OddK_Size  (pAnalyData,nFirstLoc+1)==KLINEO_SMALL  &&
			OddK_Shadow(pAnalyData,nFirstLoc+1)!=KLINEO_NOHEAD &&
			OddK_Shadow(pAnalyData,nFirstLoc+1)!=KLINEO_NOFOOT ) return KLINE_1014;
		//低档急跌后下跳剑形线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
		if(nFirstLoc!=-1&&E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_SMALL,KLINEO_DVLSHADOW,1,0)==nFirstLoc+1&&
			pAnalyData[nFirstLoc+1].Close<pAnalyData[nFirstLoc].Open) return KLINE_1015;
		//低档急跌后下跳弓形线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
		if(nFirstLoc!=-1&&E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_SMALL,KLINEO_UVLSHADOW,1,0)==nFirstLoc+1&&
			pAnalyData[nFirstLoc+1].Close<pAnalyData[nFirstLoc].Open) return KLINE_1016;
		//低档急跌后尽头阴线(有下长影)
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
		if(nFirstLoc!=-1&&pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-1].Low) return KLINE_1017;
		//低档急跌后独阳线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FAKECUTIN,1,1);
		if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG) return KLINE_1018;
		//低档急跌后独阴线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
		if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG) return KLINE_1019;
		//低档急跌后阴孕十字星
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDL,1,1);
		if(nFirstLoc!=-1&&nFirstLoc<nNo&&
			OddK_Shadow(pAnalyData,nFirstLoc+1)!=KLINEO_NOHEAD &&
			OddK_Shadow(pAnalyData,nFirstLoc+1)!=KLINEO_NOFOOT ) return KLINE_1024;
		//低档急跌后阴孕弓形线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDD,1,1);
		if(nFirstLoc!=-1&&E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_SMALL,KLINEO_UVLSHADOW,1,1)==nFirstLoc) return KLINE_1025;
		//低档急跌后阴孕剑形线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDU,1,1);
		if(nFirstLoc!=-1&&E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1)==nFirstLoc) return KLINE_1026;
		//低档急跌后切入线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUTIN,1,1)!=-1) return KLINE_1027;
		//低档急跌后阴夹阳
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_NDU,1,1)!=-1) return KLINE_1028;
		//低档急跌后上拉线
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGUP,1,1);
		if(nFirstLoc>0&&OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_BIG) return KLINE_1029;
		//低档急跌后连拉剑形线
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_SMALL,KLINEO_DVLSHADOW,2,1)!=-1) return KLINE_1030;
		//低档急跌后阳包阴
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUD,1,1)!=-1) return KLINE_1020;
		//低档急跌后最后包容阴线
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDU,1,1)!=-1) return KLINE_1021;
		//低档急跌后阴孕阳
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDU,1,1)!=-1) return KLINE_1022;
		//低档急跌后阴孕阴
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDD,1,1)!=-1) return KLINE_1023;
		break;
	case KLINETL_LOWSLOWUP:
	//低档缓升
		break;
	case KLINETL_LOWSLOWDOWN:
	//低档缓跌
		//小幅盘落后大阳向上
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,4,0);
		if(nFirstLoc!=-1&&nFirstLoc==nNo-1&&E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_BIG,0,1,1)==nNo) return KLINE_1010;
		break;
	}
	return 0;
}

//取单K线阳线,阴线,平盘线
//平盘线为绝对平盘
short CZnxg::OddK_UpDown(struct AnalyData *pAnalyData,short nNo)
{
	if(fabs(pAnalyData[nNo].Close-pAnalyData[nNo].Open)<COMPPREC) return KLINEO_LEVEL;
	if(pAnalyData[nNo].Close>pAnalyData[nNo].Open) return KLINEO_UP;
	return KLINEO_DOWN;
}

//取单K线大小形态(大,中,小)
//以1/5为界
short CZnxg::OddK_Size(struct AnalyData *pAnalyData,short nNo)
{
	float   fRate;
	fRate = fabs(pAnalyData[nNo].Close-pAnalyData[nNo].Open)/pAnalyData[nNo].Close;
	if(fRate>0.08 ) return KLINEO_BIG;
	if(fRate<0.02 ) return KLINEO_SMALL;
	return KLINEO_NORMAL;
}

//取单K线影线形态(秃头,光脚,秃头光脚...)
//秃头光脚  无上下影线
//光头      无上影线
//光脚      无下影线
short CZnxg::OddK_Shadow(struct AnalyData *pAnalyData,short nNo)
{
	float fHeadRate;
	float fFootRate;
	fHeadRate = (pAnalyData[nNo].High-MAX(pAnalyData[nNo].Close,pAnalyData[nNo].Open))/fabs(pAnalyData[nNo].Close-pAnalyData[nNo].Open);
	fFootRate = (MIN(pAnalyData[nNo].Close,pAnalyData[nNo].Open)-pAnalyData[nNo].Low)/fabs(pAnalyData[nNo].Close-pAnalyData[nNo].Open);
	if(fHeadRate<COMPPREC&&fFootRate<COMPPREC) return KLINEO_NOHEADFOOT;
	if(fHeadRate<COMPPREC&&fFootRate>COMPPREC) return KLINEO_NOHEAD;
	if(fHeadRate>COMPPREC&&fFootRate<COMPPREC) return KLINEO_NOFOOT;
	if(fabs(fHeadRate-fFootRate)<COMPPREC  ) return KLINEO_ELSHADOW;
	if(fHeadRate>3.0&&fHeadRate/fFootRate>3.0) return KLINEO_UVLSHADOW;
	if(fFootRate>3.0&&fFootRate/fHeadRate>3.0) return KLINEO_DVLSHADOW;
	if(fHeadRate/fFootRate>1.0&&fFootRate>0.1) return KLINEO_ULSHADOW;
	return KLINEO_DLSHADOW;
}

//判断双根K线形态
//是否pAnalyData中序号为nNo的K线片段和其前一K线片段具有nKLineStyle形态
//不具有时返回0,否则返回nKStyle
short CZnxg::isTwinK_Style(struct AnalyData *pAnalyData,short nNo,short nKStyle)
{
	short   nPrevUpDown,nThisUpDown;
	short   nPrevSize ,nThisSize ;
	float   fPrevOpen ,fThisOpen ;
	float   fPrevClose,fThisClose;
	if(nNo<1)  return 0;
	nPrevUpDown = OddK_UpDown(pAnalyData,nNo-1);
	nThisUpDown = OddK_UpDown(pAnalyData,nNo  );
	nPrevSize   = OddK_Size (pAnalyData,nNo-1);
	nThisSize   = OddK_Size (pAnalyData,nNo  );
	fPrevOpen   = pAnalyData[nNo-1].Open;
	fThisOpen   = pAnalyData[nNo].Open;
	fPrevClose  = pAnalyData[nNo-1].Close;
	fThisClose  = pAnalyData[nNo].Close;
	switch(nKStyle)
	{
	case KLINET_ASCENT:
	//攀援线
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_UP&&FloatComp(fThisOpen,fPrevClose)<0&&
		   FloatComp(fThisOpen,fPrevOpen)>0&&FloatComp(fThisClose,fPrevClose)>0)
			return nKStyle;
		break;
	case KLINET_SLIDE:
	//滑行线
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_DOWN&&FloatComp(fThisOpen,fPrevClose)>0&&
		   FloatComp(fThisOpen,fPrevOpen)<0&&FloatComp(fThisClose,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_JMPUP:
	//跳高线
		if(nPrevUpDown!=KLINEO_DOWN&&nThisUpDown!=KLINEO_DOWN&&
		   FloatComp(fThisOpen,fPrevClose)>0)
			return nKStyle;
		break;
	case KLINET_JMPDOWN:
	//跳水线
		if(nPrevUpDown!=KLINEO_UP&&nThisUpDown!=KLINEO_UP&&
		   FloatComp(fThisOpen,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_CRACK:
	//压迫线
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_DOWN&&
		   FloatComp(fThisOpen,fPrevOpen)>0)
			return nKStyle;
		break;
	case KLINET_FLATTER:
	//奉承线
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_UP&&
		   nPrevSize==KLINEO_BIG&&nThisUpDown!=KLINEO_SMALL&&
		   FloatComp(fThisClose,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_ABANDON:
	//弃子线
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_UP&&
		   FloatComp(fThisOpen,fPrevClose)>0&&nThisSize==KLINEO_SMALL)
			return nKStyle;
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_DOWN&&
		   FloatComp(fThisOpen,fPrevClose)<0&&nThisSize==KLINEO_SMALL)
			return nKStyle;
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_DOWN&&
		   nPrevSize==KLINEO_BIG&&nThisUpDown==KLINEO_SMALL&&
		   FloatComp(fThisOpen,fPrevClose)>0)
			return nKStyle;
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_UP&&
		   nPrevSize==KLINEO_BIG&&nThisUpDown==KLINEO_SMALL&&
		   FloatComp(fThisOpen,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_COVER:
	//覆盖线
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_DOWN&&
		   nPrevSize==KLINEO_BIG&&nThisUpDown!=KLINEO_SMALL&&
		   FloatComp(fThisOpen,fPrevClose)>0&&FloatComp(fThisClose,fPrevOpen)>0&&FloatComp(fThisClose,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_CUTIN:
	//切入线
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_UP&&
		   nPrevSize==KLINEO_BIG&&nThisUpDown!=KLINEO_SMALL&&
		   FloatComp(fThisClose,fPrevClose)>0&&FloatComp(fThisClose,fPrevOpen)<0&&FloatComp(fThisOpen,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_FAKECOVER:
	//类覆盖线
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_DOWN&&
		   FloatComp(fThisOpen,fPrevClose)>0&&FloatComp(fThisOpen,fPrevOpen)<0)
			return nKStyle;
		break;
	case KLINET_FAKECUTIN:
	//类切入线
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_UP&&
		   FloatComp(fThisClose,fPrevClose)>0&&FloatComp(fThisClose,fPrevOpen)<0&&FloatComp(fThisOpen,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_COORD:
	//并列线(*)
		if(isTwinK_Style(pAnalyData,nNo,KLINET_COORDUP  )==KLINET_COORDUP  ) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_COORDDOWN)==KLINET_COORDDOWN) return nKStyle;
		break;
	case KLINET_COORDUP:
	//并列阳线
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_UP     &&
		   fabs(fPrevOpen -fThisOpen )<MIN(fPrevClose-fPrevOpen,fThisClose-fThisOpen)*0.6 &&
		   fabs(fPrevClose-fThisClose)<MIN(fPrevClose-fPrevOpen,fThisClose-fThisOpen)*0.6 &&
		   (fPrevClose-fPrevOpen )/(fThisClose-fThisOpen)<1.3 &&
		   (fPrevClose-fPrevOpen )/(fThisClose-fThisOpen)>0.7 )
				return nKStyle;
		break;
	case KLINET_COORDDOWN:
	//并列阴线
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_DOWN&&
		   fabs(fPrevOpen -fThisOpen )<MIN(fPrevOpen-fPrevClose,fThisOpen-fThisClose)*0.6 &&
		   fabs(fPrevClose-fThisClose)<MIN(fPrevOpen-fPrevClose,fThisOpen-fThisClose)*0.6 &&
		   (fPrevOpen-fPrevClose)/(fThisOpen-fThisClose)<1.3 &&
		   (fPrevOpen-fPrevClose)/(fThisOpen-fThisClose)>0.7 )
			return nKStyle;
		break;
	case KLINET_CONTAIN:
	//包容线(*)
		if(isTwinK_Style(pAnalyData,nNo,KLINET_CUU)==KLINET_CUU) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_CUD)==KLINET_CUD) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_CUL)==KLINET_CUL) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_CDU)==KLINET_CDU) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_CDD)==KLINET_CDD) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_CDL)==KLINET_CDL) return nKStyle;
		break;
	case KLINET_CUU:
	//阳包阳
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)>-COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)< COMPPREC) break;
		if(nThisUpDown!=KLINEO_UP) break;
		if(nPrevUpDown!=KLINEO_UP) break;
		return nKStyle;
	case KLINET_CUD:
	//阳包阴
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)>-COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)< COMPPREC) break;
		if(nThisUpDown!=KLINEO_UP  ) break;
		if(nPrevUpDown!=KLINEO_DOWN) break;
		return nKStyle;
	case KLINET_CUL:
	//阳包星
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)>-COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)< COMPPREC) break;
		if(nThisUpDown!=KLINEO_UP   ) break;
		if(nPrevUpDown!=KLINEO_LEVEL) break;
		if(OddK_Shadow(pAnalyData,nNo-1)==KLINEO_NOHEADFOOT)	break;
		if(OddK_Shadow(pAnalyData,nNo-1)==KLINEO_NOHEAD    )	break;
		if(OddK_Shadow(pAnalyData,nNo-1)==KLINEO_NOFOOT    )	break;
		return nKStyle;
	case KLINET_CDU:
	//阴包阳
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)>-COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)< COMPPREC) break;
		if(nThisUpDown!=KLINEO_DOWN) break;
		if(nPrevUpDown!=KLINEO_UP  ) break;
		return nKStyle;
	case KLINET_CDD:
	//阴包阴
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)>-COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)< COMPPREC) break;
		if(nThisUpDown!=KLINEO_DOWN) break;
		if(nPrevUpDown!=KLINEO_DOWN) break;
		return nKStyle;
	case KLINET_CDL:
	//阴包星
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)>-COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)< COMPPREC) break;
		if(nThisUpDown!=KLINEO_DOWN ) break;
		if(nPrevUpDown!=KLINEO_LEVEL) break;
		if(OddK_Shadow(pAnalyData,nNo-1)==KLINEO_NOHEADFOOT)	break;
		if(OddK_Shadow(pAnalyData,nNo-1)==KLINEO_NOHEAD    )	break;
		if(OddK_Shadow(pAnalyData,nNo-1)==KLINEO_NOFOOT    )	break;
		return nKStyle;
	case KLINET_GRAVID:
	//孕妊线(*)
		if(isTwinK_Style(pAnalyData,nNo,KLINET_GUU)==KLINET_GUU) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_GUD)==KLINET_GUD) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_GUL)==KLINET_GUL) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_GDU)==KLINET_GDU) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_GDD)==KLINET_GDD) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_GDL)==KLINET_GDL) return nKStyle;
		break;
	case KLINET_GUU:
	//阳孕阳
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)< COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)>-COMPPREC) break;
		if(nPrevUpDown!=KLINEO_UP) break;
		if(nThisUpDown!=KLINEO_UP) break;
		return nKStyle;
	case KLINET_GUD:
	//阳孕阴
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)< COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)>-COMPPREC) break;
		if(nPrevUpDown!=KLINEO_UP  ) break;
		if(nThisUpDown!=KLINEO_DOWN) break;
		return nKStyle;
	case KLINET_GUL:
	//阳孕星
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)< COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)>-COMPPREC) break;
		if(nPrevUpDown!=KLINEO_UP   ) break;
		if(nThisUpDown!=KLINEO_LEVEL) break;
		if(OddK_Shadow(pAnalyData,nNo)==KLINEO_NOHEADFOOT)	break;
		if(OddK_Shadow(pAnalyData,nNo)==KLINEO_NOHEAD    )	break;
		if(OddK_Shadow(pAnalyData,nNo)==KLINEO_NOFOOT    )	break;
		return nKStyle;
	case KLINET_GDU:
	//阴孕阳
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)< COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)>-COMPPREC) break;
		if(nPrevUpDown!=KLINEO_DOWN) break;
		if(nThisUpDown!=KLINEO_UP  ) break;
		return nKStyle;
	case KLINET_GDD:
	//阴孕阴
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)< COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)>-COMPPREC) break;
		if(nPrevUpDown!=KLINEO_DOWN) break;
		if(nThisUpDown!=KLINEO_DOWN) break;
		return nKStyle;
	case KLINET_GDL:
	//阴孕星
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)< COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)>-COMPPREC) break;
		if(nPrevUpDown!=KLINEO_DOWN ) break;
		if(nThisUpDown!=KLINEO_LEVEL) break;
		if(OddK_Shadow(pAnalyData,nNo)==KLINEO_NOHEADFOOT)  break;
		if(OddK_Shadow(pAnalyData,nNo)==KLINEO_NOHEAD    )	break;
		if(OddK_Shadow(pAnalyData,nNo)==KLINEO_NOFOOT    )	break;
		return nKStyle;
	case KLINET_DRAGUP:
	//上拉线
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_UP&&FloatComp(fThisClose,fPrevOpen)>0&&
		   FloatComp(fThisOpen,fPrevClose)>0&&FloatComp(fThisOpen,fPrevOpen)<0)
			return nKStyle;
		break;
	case KLINET_DRAGDOWN:
	//下拖线
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_DOWN&&
		   FloatComp(fThisOpen,fPrevOpen)>0&&FloatComp(fThisOpen,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_SURRENDER:
	//归顺线
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_UP&&
		   FloatComp(fThisOpen,fPrevOpen)==0)
			return nKStyle;
		break;
	case KLINET_REBELL:
	//反叛线
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_DOWN&&
		   FloatComp(fThisOpen,fPrevOpen)==0)
			return nKStyle;
		break;
	case KLINET_FEIGN:
	//伪K线(*)
		if(isTwinK_Style(pAnalyData,nNo,KLINET_FEIGNUP  )==KLINET_FEIGNUP  ) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_FEIGNDOWN)==KLINET_FEIGNDOWN) return nKStyle;
		break;
	case KLINET_FEIGNUP:
	//伪阳线
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_UP&&
		   FloatComp(fThisClose,fPrevClose)<0&&FloatComp(fThisOpen,fPrevOpen)<0)
			return nKStyle;
		break;
	case KLINET_FEIGNDOWN:
	//伪阴线
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_DOWN&&
		   FloatComp(fThisClose,fPrevClose)>0&&FloatComp(fThisOpen,fPrevOpen)>0)
			return nKStyle;
		break;
	case KLINET_NIP:
	//中夹线(*)
		if(isTwinK_Style(pAnalyData,nNo,KLINET_NDU)==KLINET_NDU) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_NUD)==KLINET_NUD) return nKStyle;
		break;
	case KLINET_NDU:
	//阴夹阳
		if(nNo<2) return 0;
		if(nPrevUpDown!=KLINEO_UP  ) return 0;
		if(nThisUpDown!=KLINEO_DOWN) return 0;
		if(OddK_UpDown(pAnalyData,nNo-2)!=KLINEO_DOWN) 	 return 0;
		if(OddK_Size  (pAnalyData,nNo-2)==KLINEO_SMALL)  return 0;
		if(nPrevSize==KLINEO_SMALL)  					 return 0;
		if(nThisSize==KLINEO_SMALL)  					 return 0;
		if(pAnalyData[nNo-2].Close>fPrevClose) return 0;
		if(pAnalyData[nNo-2].Close>fPrevOpen)  return nKStyle;
		break;
	case KLINET_NUD:
	//阳夹阴
		if(nNo<2) return 0;
		if(nPrevUpDown!=KLINEO_DOWN) return 0;
		if(nThisUpDown!=KLINEO_UP  ) return 0;
		if(OddK_UpDown(pAnalyData,nNo-2)!=KLINEO_UP) 	 return 0;
		if(OddK_Size  (pAnalyData,nNo-2)==KLINEO_SMALL)  return 0;
		if(nPrevSize==KLINEO_SMALL)  					 return 0;
		if(nThisSize==KLINEO_SMALL)  					 return 0;
		if(pAnalyData[nNo-2].Close>fPrevOpen) return 0;
		if(pAnalyData[nNo-2].Close>fPrevClose)  return nKStyle;
		break;
	}
	return 0;
}

//基本面及技术面选股
short CZnxg::GetOtherStyleGP(short Type,short *lpnGPIndex)
{
	int nCount = 0;
	for(int i = 0;i < m_nABGNum;i++)
	{
		if(Is_X_Type(Type, m_pStkAttrib[i]) == 1)
			lpnGPIndex[nCount++] = m_pABGXh[i];
	}
	return nCount;
}

short CZnxg::Is_X_Type(short Type, long Stk_Attrib)
{
	if (Stk_Attrib == -1)	return 0;
	long Att = Stk_Attrib;
	CLE_OTHERXG_RES	p_TJ;
	memcpy(&p_TJ,&Att,sizeof(long));
	switch (Type)
	{
	case JBMXG_1001://  小盘次新股
		if (p_TJ.P_flag == 2 &&
			p_TJ.Cxg_flag == 1)
			return 1;
		break;
	case JBMXG_1002://  中盘次新股
		if (p_TJ.P_flag == 1 &&
			p_TJ.Cxg_flag == 1)
			return 1;
		break;
	case JBMXG_1003://  大盘次新股
		if (p_TJ.P_flag == 0 &&
			p_TJ.Cxg_flag == 1)
			return 1;
		break;
	case JBMXG_1004://	小盘绩优股
		if( p_TJ.P_flag == 2 &&
			p_TJ.J_flag == 0)
			return 1;
		break;
	case JBMXG_1005://	小盘绩平股
		if( p_TJ.P_flag == 2 &&
			p_TJ.J_flag == 1)
			return 1;
		break;
	case JBMXG_1006://	小盘绩差股
		if( p_TJ.P_flag == 2 &&
			p_TJ.J_flag == 2)
			return 1;
		break;
	case JBMXG_1007://	中盘绩优股
		if( p_TJ.P_flag == 1 &&
			p_TJ.J_flag == 0)
			return 1;
		break;
	case JBMXG_1008://	中盘绩平股
		if( p_TJ.P_flag == 1 &&
			p_TJ.J_flag == 1)
			return 1;
		break;
	case JBMXG_1009://	中盘绩差股
		if( p_TJ.P_flag == 1 &&
			p_TJ.J_flag == 2)
			return 1;
		break;
	case JBMXG_1010://	大盘绩优股
		if( p_TJ.P_flag == 0 &&
			p_TJ.J_flag == 0)
			return 1;
		break;
	case JBMXG_1011://	大盘绩平股
		if( p_TJ.P_flag == 0 &&
			p_TJ.J_flag == 1)
			return 1;
		break;
	case JBMXG_1012://	大盘绩差股
		if( p_TJ.P_flag == 0 &&
			p_TJ.J_flag == 2)
			return 1;
		break;
	case JBMXG_1013://   亏损股
		if ( p_TJ.J_flag == 3 )
			return 1;
		break;
	case JBMXG_1014://   具股本扩张能力股	净资产高并且公积金高
		if (p_TJ.Jzc_GD_flag == 1 &&
			p_TJ.Gjj_G_flag  == 1)
			return 1;
		break;
	case JBMXG_1015://   具高送配能力股	净资产高并且公积金高并且小盘
		if (p_TJ.Jzc_GD_flag == 1 &&
			p_TJ.Gjj_G_flag  == 1 &&
			p_TJ.P_flag == 2)
			return 1;
		break;
	case JBMXG_1016://   具丰厚滚存利润股
		if (p_TJ.Gclr_G_flag == 1)
			return 1;
		break;

	case JSMXG_1001://   BIAS买入指示标志
		if (p_TJ.BIAS_Buy_flag == 1)
			return 1;
		break;
	case JSMXG_1002://   SAR买入指示
		if (p_TJ.SAR_Buy_flag == 1)
			return 1;
		break;
	case JSMXG_1003://   KDJ买入指示
		if (p_TJ.KDJ_Buy_flag == 1)
			return 1;
		break;
	case JSMXG_1004://   RSI买入指示
		if (p_TJ.RSI_Buy_flag == 1)
			return 1;
		break;
	case JSMXG_1005://   MACD买入指示
		if (p_TJ.MACD_Buy_flag == 1)
			return 1;
		break;
	case JSMXG_1006://   MTM买入指示
		if (p_TJ.MTM_Buy_flag == 1)
			return 1;
		break;
	}
	return 0;
}

//计算股票的条件属性字
long CZnxg::GetGP_Tj_Attrib(short Index,struct AnalyData  * pTJ_AnalyData,short ItNum)
{
	short Data_Num;
	long Res = 0;
	CLE_OTHERXG_RES * p_Res = (CLE_OTHERXG_RES *)(&Res);
	MemStkInfo * p_MemStkInfo = (*g_pStockIO)[Index];
	Data_Num = ItNum;
	TJ_AnalyData = pTJ_AnalyData;
///////////////////////////////////////////////////////////
	if (Get_SS_Day(Index) <= g_pXGSign.Cxg_Day_L)
		p_Res -> Cxg_flag = 1;
	else p_Res -> Cxg_flag = 0;
///////////////////////////////////////////////////////////
	if (p_MemStkInfo -> ActiveCapital > g_pXGSign.Dz_sign)
		p_Res -> P_flag = DPG;
	else if (p_MemStkInfo -> ActiveCapital <= g_pXGSign.Xz_sign)
		p_Res -> P_flag = XPG;
	else 
		p_Res -> P_flag = ZPG;
///////////////////////////////////////////////////////////
	if (p_MemStkInfo -> J_zgb < 1)
		p_Res -> J_flag = JPG;
	else
	{
		if ((p_MemStkInfo->J_jly*12/p_MemStkInfo->J_HalfYearFlag)/(p_MemStkInfo -> J_zgb) >= g_pXGSign.Yp_sign)
			p_Res -> J_flag = JYG;
		else if ((p_MemStkInfo->J_jly*12/p_MemStkInfo->J_HalfYearFlag)/(p_MemStkInfo -> J_zgb) >= g_pXGSign.Pc_sign)
			p_Res -> J_flag = JPG;
		else if ((p_MemStkInfo->J_jly*12/p_MemStkInfo->J_HalfYearFlag)/(p_MemStkInfo -> J_zgb) >= g_pXGSign.Ks_sign)
			p_Res -> J_flag = JCG;
		else 
			p_Res -> J_flag = KCG;
	}
///////////////////////////////////////////////////////////
	if (p_MemStkInfo -> J_zgb < 1)		
		p_Res -> Jzc_GD_flag = 0;
	else
	{
		if (p_MemStkInfo -> J_jzc/10/p_MemStkInfo -> J_zgb >= g_pXGSign.Jzc_G_sign)
			p_Res -> Jzc_GD_flag = 1;
		else 	
			p_Res -> Jzc_GD_flag = 0;
	}
///////////////////////////////////////////////////////////
	if (p_MemStkInfo -> J_zgb < 1)	
		p_Res -> Gjj_G_flag = 0;
	else
	{
		if (p_MemStkInfo -> J_zbgjj/10/p_MemStkInfo -> J_zgb >= g_pXGSign.Gjx_G_sign)
			p_Res -> Gjj_G_flag = 1;
		else 
			p_Res -> Gjj_G_flag = 0;
	}
//////////////////////////////////////////////////////////
	if (p_MemStkInfo -> J_zgb < 1) 	
		p_Res -> Gclr_G_flag = 0;
	else
	{
		if(p_MemStkInfo -> J_wfply/10/p_MemStkInfo -> J_zgb >= g_pXGSign.Gclr_G_sign)
			p_Res -> Gclr_G_flag = 1;
		else 
			p_Res -> Gclr_G_flag = 0;
	}
	if (Data_Num <= 3)	return Res;
//////////////////////////////////////////////////////////
	if (Is_BIAS_Buy(Index,pTJ_AnalyData,Data_Num) == TRUE)	   p_Res -> BIAS_Buy_flag = TRUE;
	if (Is_SAR_Buy(Index,pTJ_AnalyData,Data_Num)  == TRUE)     p_Res -> SAR_Buy_flag  = TRUE;
	if (Is_KDJ_Buy(Index,pTJ_AnalyData,Data_Num)  == TRUE)     p_Res -> KDJ_Buy_flag  = TRUE;
	if (Is_RSI_Buy(Index,pTJ_AnalyData,Data_Num)  == TRUE)     p_Res -> RSI_Buy_flag  = TRUE;
	if (Is_MACD_Buy(Index,pTJ_AnalyData,Data_Num) == TRUE)     p_Res -> MACD_Buy_flag = TRUE;
	if (Is_MTM_Buy(Index,pTJ_AnalyData,Data_Num) == TRUE)      p_Res -> MTM_Buy_flag = TRUE;
	return Res;
}

BOOL CZnxg::Is_BIAS_Buy(short Index,struct AnalyData  * pTJ_AnalyData,short ItNum)
{
	MemStkInfo * p_MemStkInfo;
	p_MemStkInfo = (*g_pStockIO)[Index];
	HISDAT *tmpHisData = new HISDAT[ItNum];
	RawData2CalcData(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,pTJ_AnalyData,tmpHisData,ItNum,g_NoTime); //转成TCALC所认识的数据
	int nRealNum = g_pCalc->_CallDTopViewCalc_3(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,tmpHisData,ItNum);
	delete tmpHisData;
	if(nRealNum < 1) return FALSE;
	int IndexNo = g_pCalc->_CallDTopViewCalc_9(TJ_TYPE,"BIAS买入");
	if(IndexNo < 0) return FALSE;
	float *pOutData[MAX_LINEOUT];
	for(int j=0; j<MAX_LINEOUT;j++) 
		pOutData[j] = g_pCalc->_CallDTopViewCalc_13(nRealNum);
	g_pCalc->_CallDTopViewCalc_5(TJ_TYPE,IndexNo, pOutData);
	float fRet = pOutData[0][nRealNum-1];
	for (j=0; j<MAX_LINEOUT; j++) g_pCalc->_CallDTopViewCalc_14(pOutData[j]);
	return ((TBE_MEANLESS(fRet) != 0) && fabs(fRet) > 0.1 );
}

BOOL CZnxg::Is_SAR_Buy(short  Index,struct AnalyData  * pTJ_AnalyData,short ItNum)
{
	MemStkInfo * p_MemStkInfo;
	p_MemStkInfo = (*g_pStockIO)[Index];
	HISDAT *tmpHisData = new HISDAT[ItNum];
	RawData2CalcData(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,pTJ_AnalyData,tmpHisData,ItNum,g_NoTime); //转成TCALC所认识的数据
	int nRealNum = g_pCalc->_CallDTopViewCalc_3(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,tmpHisData,ItNum);
	delete tmpHisData;
	if(nRealNum < 1) return FALSE;
	int IndexNo = g_pCalc->_CallDTopViewCalc_9(TJ_TYPE,"SAR买入");
	if(IndexNo < 0) return FALSE;
	float *pOutData[MAX_LINEOUT];
	for(int j=0; j<MAX_LINEOUT;j++) 
		pOutData[j] = g_pCalc->_CallDTopViewCalc_13(nRealNum);
	g_pCalc->_CallDTopViewCalc_5(TJ_TYPE,IndexNo, pOutData);
	float fRet = pOutData[0][nRealNum-1];
	for (j=0; j<MAX_LINEOUT; j++) g_pCalc->_CallDTopViewCalc_14(pOutData[j]);
	return ((TBE_MEANLESS(fRet) != 0) && fabs(fRet) > 0.1 );
}

BOOL CZnxg::Is_KDJ_Buy(short Index,struct AnalyData  * pTJ_AnalyData,short ItNum)
{
	MemStkInfo * p_MemStkInfo;
	p_MemStkInfo = (*g_pStockIO)[Index];
	HISDAT *tmpHisData = new HISDAT[ItNum];
	RawData2CalcData(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,pTJ_AnalyData,tmpHisData,ItNum,g_NoTime); //转成TCALC所认识的数据
	int nRealNum = g_pCalc->_CallDTopViewCalc_3(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,tmpHisData,ItNum);
	delete tmpHisData;
	if(nRealNum < 1) return FALSE;
	int IndexNo = g_pCalc->_CallDTopViewCalc_9(TJ_TYPE,"KDJ买入");
	if(IndexNo < 0) return FALSE;
	float *pOutData[MAX_LINEOUT];
	for(int j=0; j<MAX_LINEOUT;j++) 
		pOutData[j] = g_pCalc->_CallDTopViewCalc_13(nRealNum);
	g_pCalc->_CallDTopViewCalc_5(TJ_TYPE,IndexNo, pOutData);
	float fRet = pOutData[0][nRealNum-1];
	for (j=0; j<MAX_LINEOUT; j++) g_pCalc->_CallDTopViewCalc_14(pOutData[j]);
	return ((TBE_MEANLESS(fRet) != 0) && fabs(fRet) > 0.1 );
}

BOOL CZnxg::Is_RSI_Buy(short  Index,struct AnalyData  * pTJ_AnalyData,short ItNum)
{
	MemStkInfo * p_MemStkInfo;
	p_MemStkInfo = (*g_pStockIO)[Index];
	HISDAT *tmpHisData = new HISDAT[ItNum];
	RawData2CalcData(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,pTJ_AnalyData,tmpHisData,ItNum,g_NoTime); //转成TCALC所认识的数据
	int nRealNum = g_pCalc->_CallDTopViewCalc_3(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,tmpHisData,ItNum);
	delete tmpHisData;
	if(nRealNum < 1) return FALSE;
	int IndexNo = g_pCalc->_CallDTopViewCalc_9(TJ_TYPE,"RSI买入");
	if(IndexNo < 0) return FALSE;
	float *pOutData[MAX_LINEOUT];
	for(int j=0; j<MAX_LINEOUT;j++) 
		pOutData[j] = g_pCalc->_CallDTopViewCalc_13(nRealNum);
	g_pCalc->_CallDTopViewCalc_5(TJ_TYPE,IndexNo, pOutData);
	float fRet = pOutData[0][nRealNum-1];
	for (j=0; j<MAX_LINEOUT; j++) g_pCalc->_CallDTopViewCalc_14(pOutData[j]);
	return ((TBE_MEANLESS(fRet) != 0) && fabs(fRet) > 0.1 );
}

BOOL CZnxg::Is_MACD_Buy(short Index,struct AnalyData  * pTJ_AnalyData,short ItNum)
{
	MemStkInfo * p_MemStkInfo;
	p_MemStkInfo = (*g_pStockIO)[Index];
	HISDAT *tmpHisData = new HISDAT[ItNum];
	RawData2CalcData(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,pTJ_AnalyData,tmpHisData,ItNum,g_NoTime); //转成TCALC所认识的数据
	int nRealNum = g_pCalc->_CallDTopViewCalc_3(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,tmpHisData,ItNum);
	delete tmpHisData;
	if(nRealNum < 1) return FALSE;
	int IndexNo = g_pCalc->_CallDTopViewCalc_9(TJ_TYPE,"MACD买入");
	if(IndexNo < 0) return FALSE;
	float *pOutData[MAX_LINEOUT];
	for(int j=0; j<MAX_LINEOUT;j++) 
		pOutData[j] = g_pCalc->_CallDTopViewCalc_13(nRealNum);
	g_pCalc->_CallDTopViewCalc_5(TJ_TYPE,IndexNo, pOutData);
	float fRet = pOutData[0][nRealNum-1];
	for (j=0; j<MAX_LINEOUT; j++) g_pCalc->_CallDTopViewCalc_14(pOutData[j]);
	return ((TBE_MEANLESS(fRet) != 0) && fabs(fRet) > 0.1 );
}

BOOL CZnxg::Is_MTM_Buy(short Index,struct AnalyData  * pTJ_AnalyData,short ItNum)
{
	MemStkInfo * p_MemStkInfo;
	p_MemStkInfo = (*g_pStockIO)[Index];
	HISDAT *tmpHisData = new HISDAT[ItNum];
	RawData2CalcData(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,pTJ_AnalyData,tmpHisData,ItNum,g_NoTime); //转成TCALC所认识的数据
	int nRealNum = g_pCalc->_CallDTopViewCalc_3(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,tmpHisData,ItNum);
	delete tmpHisData;
	if(nRealNum < 1) return FALSE;
	int IndexNo = g_pCalc->_CallDTopViewCalc_9(TJ_TYPE,"MTM买入");
	if(IndexNo < 0) return FALSE;
	float *pOutData[MAX_LINEOUT];
	for(int j=0; j<MAX_LINEOUT;j++) 
		pOutData[j] = g_pCalc->_CallDTopViewCalc_13(nRealNum);
	g_pCalc->_CallDTopViewCalc_5(TJ_TYPE,IndexNo, pOutData);
	float fRet = pOutData[0][nRealNum-1];
	for (j=0; j<MAX_LINEOUT; j++) g_pCalc->_CallDTopViewCalc_14(pOutData[j]);
	return ((TBE_MEANLESS(fRet) != 0) && fabs(fRet) > 0.1 );
}
