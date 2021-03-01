#include "stdafx.h"
#include "dataio.h"

#define FOR_GPQH(para) for(para = 0; para < 2; para++)

StockDataIo::StockDataIo()
{
	for (int i = 0; i < 2; i++)
	{   
		hqhead[i]     = NULL;
		hqhead_l2[i]  = NULL;
		hqqueue[i]   = NULL;
		hqmark[i]	  = NULL;
		hqImgStat[i]  = NULL;
		hqTckStat[i]  = NULL;
		hqDDEReport[i]= NULL;
		IndexNum[i]   = 0;
		StockNum[i]   = 0;
		HostStkNum[i] = 0;
	}
	m_pCwcl = NULL;
	m_aICList = NULL;
	m_aICListEx = NULL;
	
	m_aDSStkInfo.reserve(MAX_DS_STKNUM);
	m_aDSStkInfo.clear();
	m_code2IndexMap.clear();

	memset(m_aTimerSet, 0, sizeof(m_aTimerSet));
	m_nDSNum = 0;
}

StockDataIo::~StockDataIo()
{
	int  i;
	char filename[2][256];
	FOR_GPQH(i)
	{
		sprintf(filename[i], "%sLData\\cache\\%s.tcu",g_WSXHStr,GetSetStr(i));			
		CFile theFile;
		if(theFile.Open(filename[i],CFile::modeRead|CFile::shareDenyNone))
		{
			theFile.Close();
			if(theFile.Open(filename[i],CFile::modeCreate|CFile::modeWrite))
			{
				theFile.Write(hqhead[i],StockNum[i]*sizeof(CurrStockData));
				theFile.Close();
			}
		}
		sprintf(filename[i], "%sLData\\cache\\%s.th2",g_WSXHStr,GetSetStr(i));			
		if(theFile.Open(filename[i],CFile::modeRead|CFile::shareDenyNone))
		{
			theFile.Close();
			if(theFile.Open(filename[i],CFile::modeCreate|CFile::modeWrite))
			{
				theFile.Write(hqhead_l2[i],StockNum[i]*sizeof(CurrStockData_L2));
				theFile.Write(hqqueue[i],StockNum[i]*sizeof(CurrQueue));
				theFile.Close();
			}
		}
	}
	if(g_bHasDS) SaveDSCurrData();
	FOR_GPQH(i)
	{
		head[i].clear();

		TDEL(hqhead[i]);
		TDEL(hqhead_l2[i]);
		TDEL(hqqueue[i]);
		TDEL(hqmark[i]);
		TDEL(hqImgStat[i]);
		TDEL(hqTckStat[i]);
		TDEL(hqDDEReport[i]);
	}
	TDEL(m_pCwcl);
	TDEL(m_aICList);
	TDEL(m_aICListEx);
}

BOOL StockDataIo::TcpipInit(BOOL bOffLineMode)
{
	char	filename[2][256];
	FILE	*fpt;
	int		i,j,headlen = sizeof(unsigned short)+40+2*sizeof(long);
	FOR_GPQH(i)
	{
		//读TNF
		sprintf(filename[i], "%sLData\\cache\\%sex.tnf",g_WSXHStr,GetSetStr(i));
		fpt = fopen(filename[i],"rb");
		if(!fpt) 
		{ 
			if(bOffLineMode)
			{
				if(g_nEmbedTabSel==LOGIN_ONLYWT)
					TDX_MessageBox(NULL,"初始化文件不存在,必须先连接一次行情后才能使用独立交易方式!\r\n\r\n首次登录的客户请采用行情+交易方式登录.",MB_OK|MB_ICONEXCLAMATION);
				else
					TDX_MessageBox(NULL,"初始化文件不存在,不能脱机运行,必须先连接一次行情后才能脱机运行!",MB_OK|MB_ICONEXCLAMATION);
			}
			else
				TDX_MessageBox(NULL,"初始化数据没有取得!",MB_OK|MB_ICONEXCLAMATION);
			return FALSE;
		}
		StockNum[i] = (on_filelength(fpt)-headlen)/sizeof(struct MemStkInfo);  //股票数

		long nSpaceNum = StockNum[i]+3*LOCALINDEX_MAXSPACE;
		head[i].reserve(nSpaceNum);
		head[i].clear();
		for(int k=0;k<nSpaceNum;k++)
		{
			MemStkInfo iInfo={0};
			head[i].push_back(iInfo);
		}
		fseek(fpt,sizeof(unsigned short)+40,SEEK_SET);

		//得到lOpenRq
		fread(&g_lOpenRq,1,sizeof(long),fpt);
		g_bLastRqDown=FALSE;	//更换了数据以后,g_bLastRqDown赋为FALSE,表示要重新下载数据
		fseek(fpt, headlen, SEEK_SET);
		fread (&head[i][0],1,StockNum[i]*sizeof (struct MemStkInfo),fpt);
		fclose(fpt);
		//判断是否是深圳新数据
		if(i==0 && StockNum[0]>0 && strlen(head[i][0].Code)!=SH_CODE_LEN)
		{
			TDX_MessageBox(NULL,"错误：数据结构格式不正确!",MB_OK|MB_ICONERROR);
			return FALSE;
		}
		IndexNum[i] = 0;
		for(k = 0;k < StockNum[i];k++)
			if(testzs( &head[i][k] )) IndexNum[i]++;
		//读THQ
		hqhead[i] = new CurrStockData[StockNum[i]+3*LOCALINDEX_MAXSPACE];
		if (!hqhead[i]) return FALSE;
		memset(hqhead[i],0,(StockNum[i]+3*LOCALINDEX_MAXSPACE)*sizeof (CurrStockData));
		sprintf(filename[i], "%sLData\\cache\\%s.tcu",g_WSXHStr,GetSetStr(i));
		CFile theFile;
		BOOL bRet = theFile.Open(filename[i],CFile::modeRead|CFile::shareDenyNone);
		if(!bRet)
		{
			if(!theFile.Open(filename[i],CFile::modeCreate|CFile::modeWrite))
			{
				TDX_MessageBox(NULL,"文件创建错误!",MB_OK|MB_ICONEXCLAMATION);
				return FALSE;
			}
			theFile.Write(hqhead[i],StockNum[i]*sizeof(CurrStockData));
			theFile.Close();
		}
		else 
		{
			theFile.Read(hqhead[i],StockNum[i]*sizeof(CurrStockData));
			theFile.Close();
		}
		//读TH2
		hqhead_l2[i] = new CurrStockData_L2[StockNum[i]+3*LOCALINDEX_MAXSPACE];
		hqqueue[i] = new CurrQueue[StockNum[i]+3*LOCALINDEX_MAXSPACE];
		if (!hqhead_l2[i] || !hqqueue[i]) return FALSE;
		memset(hqhead_l2[i],0,(StockNum[i]+3*LOCALINDEX_MAXSPACE)*sizeof (CurrStockData_L2));
		memset(hqqueue[i],0,(StockNum[i]+3*LOCALINDEX_MAXSPACE)*sizeof (CurrQueue));
		sprintf(filename[i], "%sLData\\cache\\%s.th2",g_WSXHStr,GetSetStr(i));
		bRet = theFile.Open(filename[i],CFile::modeRead|CFile::shareDenyNone);
		if(!bRet)
		{
			if(!theFile.Open(filename[i],CFile::modeCreate|CFile::modeWrite))
			{
				TDX_MessageBox(NULL,"文件创建错误!",MB_OK|MB_ICONEXCLAMATION);
				return FALSE;
			}
			theFile.Write(hqhead_l2[i],StockNum[i]*sizeof(CurrStockData_L2));
			theFile.Write(hqqueue[i],StockNum[i]*sizeof(CurrQueue));
			theFile.Close();
		}
		else 
		{
			theFile.Read(hqhead_l2[i],StockNum[i]*sizeof(CurrStockData_L2));
			theFile.Read(hqqueue[i],StockNum[i]*sizeof(CurrQueue));			
			theFile.Close();
		}
		//处理hqmark
		hqmark[i] = new MarkInfo[StockNum[i]+3*LOCALINDEX_MAXSPACE];
		if (!hqmark[i]) return FALSE;
		memset(hqmark[i],0,(StockNum[i]+3*LOCALINDEX_MAXSPACE)*sizeof (MarkInfo));
		//处理hqImgStat
		hqImgStat[i] = new SumImgStat[StockNum[i]+3*LOCALINDEX_MAXSPACE];
		if (!hqImgStat[i]) return FALSE;
		memset(hqImgStat[i],0,(StockNum[i]+3*LOCALINDEX_MAXSPACE)*sizeof (SumImgStat));
		//处理hqTckStat
		hqTckStat[i] = new SumTckStat[StockNum[i]+3*LOCALINDEX_MAXSPACE];
		if (!hqTckStat[i]) return FALSE;
		memset(hqTckStat[i],0,(StockNum[i]+3*LOCALINDEX_MAXSPACE)*sizeof (SumTckStat));
		//处理hqDDEReport
		hqDDEReport[i] = new DDEREPORT[StockNum[i]+3*LOCALINDEX_MAXSPACE];
		if (!hqDDEReport[i]) return FALSE;
		memset(hqDDEReport[i],0,(StockNum[i]+3*LOCALINDEX_MAXSPACE)*sizeof (DDEREPORT));
		//在这里增加自定指数
		HostStkNum[i] = StockNum[i];
	//	ReAddLc(i);
		//
		for (j=0;j<StockNum[i];j++)
		{
			_FL(head[i][j].Name);
			head[i][j].Index = j;
			(hqhead[i]+j)->Close = head[i][j].Close;
		}
	}
    if(g_iToday!=g_lOpenRq)		g_bYesterdayFlag=1;
    else						g_bYesterdayFlag=0;

	ResetMemStkOtherInfo();	//填充MemStkInfo的初始值
	GetJBZL();				//在股票信息中加入财务数据
//	GetConceptBlock();		//取概念板块
//	GetZHBBlockNum();
	m_pCwcl = new Cwcl;		//构造股本变迁类
	m_pCwcl->Init_ZGB_LTGB();
	LoadIndentInfo();
	return TRUE;
}

void StockDataIo::ReAddLc(short setcode)
{
	if(setcode==SZ && g_pLcIdxMng)
	{
		StockNum[setcode]	= HostStkNum[setcode];
		LPLCSECIDX pLcSecIdx = NULL;
		long lLcSecNum = g_pLcIdxMng->GetSecIdx(pLcSecIdx);
		long i = 0;
		if(lLcSecNum>0 && pLcSecIdx!=NULL)
		{
			for(i=0;i<lLcSecNum;i++)
			{
				strncpy(head[setcode][StockNum[setcode]].Code,pLcSecIdx[i].Code,CODE_SIZE-1);
				head[setcode][StockNum[setcode]].Code[CODE_SIZE-1] = '\0';
				strncpy(head[setcode][StockNum[setcode]].Name,pLcSecIdx[i].Name,NAME_SIZE-1);
				head[setcode][StockNum[setcode]].Name[NAME_SIZE-1] = '\0';
				head[setcode][StockNum[setcode]].Index = StockNum[setcode];
				hqhead[setcode][StockNum[setcode]].Close = head[setcode][StockNum[setcode]].Close;
				head[setcode][StockNum[setcode]].Unit = 1;
				head[setcode][StockNum[setcode]].AmoUnit = 1.0f;
				StockNum[setcode]++;
			}
		}
	}
	g_bBlockModified = TRUE;
}

void StockDataIo::SetCurrStockData( short tmpsetcode, short Index, char * the_hq, char * the_hq_l2 )
{
	if(!the_hq) return;
	if(tmpsetcode>1||Index<0||Index>=StockNum[tmpsetcode]) 
	{
		if(g_bHasDS&&Index>=DS_FROM&&tmpsetcode<g_nDSMarketNum)
		{
			MemStkInfo *pStkInfo = (*this)[Index];
			if(pStkInfo) DSSetCurrData(pStkInfo, (PCURRDATA)the_hq, NULL);
		}
		return;
	}
	MemStkInfo *pStkInfo = Get_MemStkInfo(tmpsetcode,Index);
//	if(IsLocalStock(pStkInfo)) 
//		return;
	memcpy( (char *)(hqhead[tmpsetcode]+Index), the_hq, sizeof(struct CurrStockData));
	if(the_hq_l2)
		memcpy( (char *)(hqhead_l2[tmpsetcode]+Index), the_hq_l2, sizeof(struct CurrStockData_L2));
}

void StockDataIo::SetCurrQueue( short tmpsetcode, short Index, char * the_queue)
{
	if(!the_queue) return;
	if(tmpsetcode>1||Index<0||Index>=StockNum[tmpsetcode]) 
		return;
	memcpy( (char *)(hqqueue[tmpsetcode]+Index), the_queue, sizeof(struct CurrQueue));
}

void StockDataIo::SetCurrMarkInfo( short tmpsetcode, short Index, char * the_mrk )
{    
	if(Index < 0 || Index >= StockNum[tmpsetcode]) return;
	struct MarkInfo *pMrk = (struct MarkInfo *)the_mrk;
	memcpy( (char *)(hqmark[tmpsetcode]+Index), the_mrk, sizeof(struct MarkInfo));
}

void StockDataIo::SetStatData( short tmpsetcode, short Index, char * the_imgstat, char * the_tckstat)
{    
	if(Index < 0 || Index >= StockNum[tmpsetcode]) return;
	SUMIMGSTAT *pImg = (SUMIMGSTAT *)the_imgstat;
	SUMTCKSTAT *pTck = (SUMTCKSTAT *)the_tckstat;
	if(pImg)
		memcpy( (char *)(hqImgStat[tmpsetcode]+Index), pImg, sizeof(SUMIMGSTAT));
	if(pTck)
		memcpy( (char *)(hqTckStat[tmpsetcode]+Index), pTck, sizeof(SUMTCKSTAT));
}

#include "ProgressWnd.h"
void StockDataIo::CalcDDEReport()
{
	int i=0,j=0,k=0;
	float tmpDDX	= 0;
	float tmpDDY	= 0;
	long tmpDayCount= 0;
	BOOL bRed		= TRUE;
	EXDAYINFO tmpExday[60];
	long lDataNum	= 0;
	CFile FxFile;
	int	tmpInt;
	if(g_DDEDataNeedLoad)
	{
		CProgressWnd wndProgress(NULL,_F("正在计算，请稍等..."),TRUE);
		wndProgress.GoModal();
		wndProgress.SetRange(0,StockNum[SZ]+StockNum[SH]);
		wndProgress.SetStep(1);
		for(i=0;i<2;i++)
		{
			for(j=0;j<StockNum[i];j++)
			{
				wndProgress.SetText(head[i][j].Name);
				wndProgress.StepIt();
				if(j%10==0)
				{
					wndProgress.PeekAndPump();
					if(wndProgress.Cancelled())
					{
						TDX_MessageBox(NULL,"计算被取消,报表数据不完整！",MB_OK|MB_ICONINFORMATION);
						i=100;
						break;
					}
				}
				tmpInt = GetStockType(i,head[i][j].Code);
				if(tmpInt!=CODE_SHAG && tmpInt!=CODE_SZAG && tmpInt!=CODE_SHBG && tmpInt!=CODE_SZBG && tmpInt!=CODE_ZXB && tmpInt!=CODE_CYB && tmpInt!=CODE_SHQZ && tmpInt!=CODE_SZQZ)
					continue;
				
				hqDDEReport[i][j].fDDX59	= 0;
				hqDDEReport[i][j].fDDY59	= 0;
				hqDDEReport[i][j].DDXRed59	= 0;
				hqDDEReport[i][j].DDXRed9	= 0;
				
				lDataNum = ReadExDayData(&(head[i][j]),tmpExday,60);
				if(lDataNum<=0) continue;
				tmpDDX = 0;
				tmpDDY = 0;
				tmpDayCount	= 0;
				bRed = TRUE;
				for(k=lDataNum-1;k>=0;k--)
				{
					if(tmpExday[k].Date>=g_lOpenRq) continue;
					if(head[i][j].ActiveCapital>COMPPREC)
					{
						tmpDDX = (tmpExday[k].BigTickBVol-tmpExday[k].BigTickSVol)/head[i][j].ActiveCapital;
						if(tmpDDX>0)
						{
							if(bRed) hqDDEReport[i][j].DDXRed59++;
							if(tmpDayCount<9) hqDDEReport[i][j].DDXRed9++;
						}
						else bRed = FALSE;
					}
					if(tmpExday[k].Cjbs>COMPPREC)
						tmpDDY = (tmpExday[k].SellCjbs-tmpExday[k].BuyCjbs)/tmpExday[k].Cjbs;
					else tmpDDY = 0;
					hqDDEReport[i][j].fDDX59 += tmpDDX;
					hqDDEReport[i][j].fDDY59 += tmpDDY;
					tmpDayCount++;
					if(tmpDayCount>=59) break;
				}
			}
		}
		if(i==2) g_DDEDataNeedLoad = FALSE;
	}
	for(i=0;i<2;i++)
	{
		for(j=0;j<StockNum[i];j++)
		{
			tmpInt = GetStockType(i,head[i][j].Code);
			if(tmpInt!=CODE_SHAG && tmpInt!=CODE_SZAG && tmpInt!=CODE_SHBG && tmpInt!=CODE_SZBG && tmpInt!=CODE_ZXB && tmpInt!=CODE_CYB && tmpInt!=CODE_SHQZ && tmpInt!=CODE_SZQZ)
				continue;
			
			if(head[i][j].ActiveCapital>COMPPREC)
			{
				hqDDEReport[i][j].fDDX			= (hqTckStat[i][j].fVol[1][0]+hqTckStat[i][j].fVol[1][2]-hqTckStat[i][j].fVol[1][1]-hqTckStat[i][j].fVol[1][3])/head[i][j].ActiveCapital;
				hqDDEReport[i][j].fBigIOWeight	= (hqTckStat[i][j].fVol[1][0]+hqTckStat[i][j].fVol[1][2]+hqTckStat[i][j].fVol[1][4]+hqTckStat[i][j].fVol[1][1]+hqTckStat[i][j].fVol[1][3]+hqTckStat[i][j].fVol[1][4])/(2*head[i][j].ActiveCapital);
			}
			else
			{
				hqDDEReport[i][j].fDDX			= 0;
				hqDDEReport[i][j].fBigIOWeight	= 0;
			}
			float SMBCjbs = hqTckStat[i][j].VolNum[0][1]*1.0+hqTckStat[i][j].VolNum[0][3]*1.0-hqTckStat[i][j].VolNum[0][0]*1.0-hqTckStat[i][j].VolNum[0][2]*1.0;
			float SPBCjbs = hqTckStat[i][j].VolNum[0][1]*1.0+hqTckStat[i][j].VolNum[0][3]*1.0+hqTckStat[i][j].VolNum[0][0]*1.0+hqTckStat[i][j].VolNum[0][2]*1.0;
			if(hqTckStat[i][j].CJBS>COMPPREC) hqDDEReport[i][j].fDDY = SMBCjbs/hqTckStat[i][j].CJBS*100;
			else hqDDEReport[i][j].fDDY = 0;
			if(SPBCjbs>COMPPREC) hqDDEReport[i][j].fDDZ = SMBCjbs/SPBCjbs*100;
			else hqDDEReport[i][j].fDDZ = 0;
			hqDDEReport[i][j].fDDX60	= hqDDEReport[i][j].fDDX+hqDDEReport[i][j].fDDX59;
			hqDDEReport[i][j].fDDY60	= hqDDEReport[i][j].fDDY+hqDDEReport[i][j].fDDY59;
			if(hqDDEReport[i][j].fDDX>0)
			{
				hqDDEReport[i][j].DDXRed	= hqDDEReport[i][j].DDXRed59+1;
				hqDDEReport[i][j].DDXRed10	= hqDDEReport[i][j].DDXRed9+1;
			}
			else
			{
				hqDDEReport[i][j].DDXRed	= 0;
				hqDDEReport[i][j].DDXRed10	= hqDDEReport[i][j].DDXRed9;
			}
			if(hqhead[i][j].Volume>=1)
			{
				hqDDEReport[i][j].fBigBuyRate	= 100*(hqTckStat[i][j].fVol[1][0]+hqTckStat[i][j].fVol[1][2]+hqTckStat[i][j].fVol[1][4])/hqhead[i][j].Volume;
				hqDDEReport[i][j].fBigSellRate	= 100*(hqTckStat[i][j].fVol[1][1]+hqTckStat[i][j].fVol[1][3]+hqTckStat[i][j].fVol[1][4])/hqhead[i][j].Volume;
				float AvePrice = hqhead[i][j].Amount/hqhead[i][j].Volume;
				hqDDEReport[i][j].fBigIOAmount	= AvePrice*(hqTckStat[i][j].fVol[1][0]+hqTckStat[i][j].fVol[1][2]-hqTckStat[i][j].fVol[1][1]-hqTckStat[i][j].fVol[1][3]);
			}
			else
			{
				hqDDEReport[i][j].fBigBuyRate	= 0;
				hqDDEReport[i][j].fBigSellRate	= 0;
				hqDDEReport[i][j].fBigIOAmount	= 0;
			}
			float fBuyOrder	= max(hqImgStat[i][j].fBOrder-hqImgStat[i][j].fBCancel-hqhead[i][j].Volume,0);
			float fSellOrder= max(hqImgStat[i][j].fSOrder-hqImgStat[i][j].fSCancel-hqhead[i][j].Volume,0);
			if(fBuyOrder+fSellOrder>1)
				hqDDEReport[i][j].fBOrderRate = 100*(fBuyOrder-fSellOrder)/(fBuyOrder+fSellOrder);
			else hqDDEReport[i][j].fBOrderRate = 0;
			if(hqTckStat[i][j].CJBS>=1) hqDDEReport[i][j].fAveTradeVol	= hqhead[i][j].Volume*1.0/hqTckStat[i][j].CJBS;
			else hqDDEReport[i][j].fAveTradeVol	= 0;
		}
	}
}

LPDDEREPORT StockDataIo::ReadDDEReport(short set_code,short Index)
{
	if(set_code != SZ && set_code != SH) return NULL;
	if(Index < 0 || Index >= StockNum[set_code]) return NULL;
    return hqDDEReport[set_code] + Index;
}

struct MemStkInfo * StockDataIo::operator [] (short i)
{
	if(i < 0) return NULL;		
    if(i< StockNum[0])
		return &head[0][i];
    else if(i< StockNum[0]+StockNum[1])
		return &head[1][i-StockNum[0]];
	else if(g_bHasDS&&i>=DS_FROM)
		return GetDSStkInfo(i-DS_FROM);
	return NULL;
};

MemStkInfo * StockDataIo::Get_MemStkInfo( short set_code, short Index )
{
	if(set_code != SZ && set_code != SH) 
	{
		if(g_bHasDS&&set_code<g_nDSMarketNum)
			return GetDSStkInfo(Index);
		return NULL;
	}
	if(Index < 0 || Index >= StockNum[set_code]) return NULL;
    return &head[set_code][Index];
}

MemStkInfo * StockDataIo::Get_MemStkInfoFromHqxuhao( short set_code, short Hqxuhao )
{
	if(set_code != SZ && set_code != SH)
	{
		if(g_bHasDS&&set_code>=0&&set_code<g_nDSMarketNum)
			return GetDSStkInfo(Hqxuhao);
		return NULL;
	}
	
	for(int i = 0;i < StockNum[set_code];i++)
		if(head[set_code][i].Index == Hqxuhao) break;
	if(i >= StockNum[set_code])	return NULL;
	return &head[set_code][i];
}

MemStkInfo * StockDataIo::Get_MemStkInfo(const char * Code,short setcode)
{
	if(setcode!=SZ && setcode!=SH)	
	{
		if(g_bHasDS&&setcode>=0&&setcode<MAX_MKT_NUM)
			return GetDSStkInfo(setcode,Code);
		return Get_MemStkInfo(Code);
	}
    char string[CODE_SIZE]={0};
    strncpy(string,Code,CODE_SIZE-1);
    string[CODE_SIZE-1] = 0;
	for (int i = 0; i < StockNum[setcode]; i++)
		if (!strcmpi(head[setcode][i].Code, string)) return &head[setcode][i];
	return NULL;
}

MemStkInfo * StockDataIo::Get_MemStkInfo(const char * Code )		//只针对大陆市场
{
    char string[10]={0};
    memcpy(string,Code,CODE_SIZE-1);
    string[CODE_SIZE-1] = 0;
    for (int i = 0; i < StockNum[0]; i++)
		if (!strcmpi(head[0][i].Code, string)) return &head[0][i];
    for (i = 0; i < StockNum[1]; i++)
		if (!strcmpi(head[1][i].Code, string)) return &head[1][i];
	if(g_bHasDS)
	{
		for(i=0;i<m_nDSNum;i++)
			if(stricmp(m_aDSStkInfo[i].Code,Code)==0) break;
		if(i<m_nDSNum)
			return &m_aDSStkInfo[i];
	}
	return NULL;
}

MemStkInfo * StockDataIo::Get_MemStkInfo_FromMultiBuf(const char * pBuff)
{
	short	setcode;
	char	gpcode[CODE_SIZE]={0};
	CString tempLine,tempCode;

	tempLine = pBuff;
	int tmpb = tempLine.Find("#",0);
	int tmp = (tmpb==-1?0:tmpb);
	tempCode = tempLine.Mid(tmp+1);
	setcode = (tmpb>=0)?atoi(tempLine):(tempLine[0]-'0');
	strncpy(gpcode,tempCode,CODE_SIZE-1);
	gpcode[CODE_SIZE-1] = 0;
	return g_pStockIO->Get_MemStkInfo(gpcode,setcode);
}

short StockDataIo::GetIndex_FromMultiBuf(const char * pBuff)
{
	short	setcode;
	char	gpcode[CODE_SIZE]={0};
	CString tempLine,tempCode;

	tempLine = pBuff;
	int tmpb = tempLine.Find("#",0);
	int tmp = (tmpb==-1?0:tmpb);
	tempCode = tempLine.Mid(tmp+1);
	setcode = tmpb>=0?atoi(tempLine):tempLine[0]-'0';
	strncpy(gpcode,tempCode,CODE_SIZE-1);
	gpcode[CODE_SIZE-1] = 0;
	return g_pStockIO->GetIndex(gpcode,setcode);
}

short StockDataIo::GetIndex( short setcode, short Index )
{
	if (setcode == 0)		return Index;
	else if (setcode == 1)  return Index + StockNum[0];
	else if(Index>=DS_FROM)	return Index;
	else					return Index + StockNum[0] + StockNum[1];
}

short StockDataIo::GetIndex(const char * Code )
{
	char   string[CODE_SIZE]={0};
	strncpy ( string, Code, CODE_SIZE-1 );
	string[CODE_SIZE-1] = 0;
	for(int i = 0; i < StockNum[0]; i++)
		if(!strcmp(head[0][i].Code, string)) return i;
	for(i = 0; i < StockNum[1]; i++)
		if(!strcmp(head[1][i].Code, string)) return i+StockNum[0];
	if(g_bHasDS)
	{
		for(i=0;i<MAX_MKT_NUM;i++)
		{
			MemStkInfo *pInfo = GetDSStkInfo(i,Code);
			if(pInfo)	return(pInfo->Index);
		}
	}
	return -1;
}

short StockDataIo::GetIndex(const char * Code,short setcode)
{
	if(setcode!=SZ && setcode!=SH)	
	{
		if(g_bHasDS&&setcode<MAX_MKT_NUM)
		{
			MemStkInfo *pInfo = GetDSStkInfo(setcode,Code);
			if(pInfo)	return(pInfo->Index);
		}
		return GetIndex(Code);
	}
	char   string[10];
	strncpy ( string, Code, CODE_SIZE-1 );
	string[CODE_SIZE-1] = 0;
	for(int i = 0; i < StockNum[setcode]; i++)
	{
		if(!strcmp(head[setcode][i].Code, string)) 
		{
			if(setcode==SH) return StockNum[0]+i;
			return i;
		}
	}
	return -1;
}

char* StockDataIo::GetNameFromIndex(short index)
{
	if(index < 0) return NULL;
	MemStkInfo *tmpInfo = (*this)[index];
	if(!tmpInfo)  return NULL;
	return tmpInfo->Name;
}

char*	StockDataIo::GetMarketInfo(struct MemStkInfo *tmpscode)
{
	static char s_MarketInfo[100];
	if(!tmpscode || tmpscode->bigmarket<0 || tmpscode->bigmarket>15) 
		return "";
	for(int i=0;tmpscode && i < g_nDSMarketNum;i++)
	{
		if(g_pDSMarket[i].MktType==tmpscode->setcode) 
		{
			sprintf(s_MarketInfo,"%s - %s",g_strBigMarket[tmpscode->bigmarket],DSGetMarketNameFromXH(i));
			return s_MarketInfo;
		}
	}		
	return "";
}

BOOL testzs_nolc(struct MemStkInfo *pStkInfo)
{
	if(!pStkInfo) return FALSE;
	return testzs(pStkInfo) && !IsLocalStock(pStkInfo) && !IsLCPt(pStkInfo);
}

//返回符合条件ftype和setcode的股票数,股票索引在p数组中
short StockDataIo::FiltCode( short setcode, union flunion ftype, short * p)		//仅适用于大陆市场
{
  short  i,j,k,stocktype;
  j = 0;
  if ((setcode == SZ) || (setcode == SZ_SH))
  {
	  for (i=0;i<StockNum[SZ];i++)
	  {
		  if(ftype.fshort == 0x00)
		  {
			  if(!testzs_nolc(&head[SZ][i]))
				  p[j++] = i;
			  continue;
		  }
		  if(ftype.fshort == 0x99)
		  {
			  if(testzs_nolc(&head[SZ][i]))
				  p[j++] = i;
			  continue;
		  }
		  if(testzs_nolc(&head[SZ][i])) continue;
		  stocktype=GetStockType(SZ,head[SZ][i].Code);
		  if(stocktype==CODE_KFJJ && !g_bNo003InAG)
			stocktype=CODE_SZOTHER;
		  switch (stocktype)
		  {
			case CODE_SZAG:
				if (ftype.ftype.astk==1) p[j++]=i;
				break;
			case CODE_SZSPEC:
				if (ftype.ftype.addcode==1) p[j++]=i;
				break;
			case CODE_SZQZ:
				if (ftype.ftype.qz==1) p[j++]=i;
				break;
			case CODE_SZZQ:
				if (ftype.ftype.qiz==1) p[j++]=i;
				break;
			case CODE_SZGZ:
				if (ftype.ftype.gz==1) p[j++]=i;
				break;
			case CODE_KFJJ:
				if (ftype.ftype.jj==1) p[j++]=i;
				break;
			case CODE_SZJJ:
				if (ftype.ftype.jj==1) p[j++]=i;
				break;
			case CODE_SZBG:
				if (ftype.ftype.bstk==1) p[j++]=i;
				break;
			case CODE_ZXB:
				if (ftype.ftype.zxbstk==1) p[j++]=i;
				break;
			case CODE_CYB:
				if (ftype.ftype.cybstk==1) p[j++]=i;
				break;
			case CODE_SB:
				if (ftype.ftype.sb==1) p[j++]=i;
				break;
			default:      
				if (ftype.ftype.qtstk==1) p[j++]=i; //其它
				break;
			}
		  if(j>MAXGPNUM-1) return j;
		}
	}
	if ((setcode==SH) || (setcode == SZ_SH))
	{
		k=StockNum[SZ];
		for (i=0;i<StockNum[SH];i++)
		{   
			if(ftype.fshort == 0x00)
			{
				if(!testzs_nolc(&head[SH][i]))
					p[j++] = i+k;
				continue;
			}
			if(ftype.fshort == 0x99)
			{
				if(testzs_nolc(&head[SH][i]))
					p[j++] = i+k;
				continue;
			}
			if(testzs_nolc(&head[SH][i])) continue;
		    stocktype=GetStockType(SH,head[SH][i].Code);
   		    switch (stocktype)
			{
			case CODE_SHGZ:
				if (ftype.ftype.gz==1) p[j++]=i+k;
				break;
			case CODE_SHZQ:
				if (ftype.ftype.qiz==1) p[j++]=i+k;
				break;
			case CODE_SHJJ:
				if (ftype.ftype.jj==1) p[j++]=i+k;
				break;
			case CODE_SHAG:
				if (ftype.ftype.astk==1) p[j++]=i+k;
				break;
			case CODE_SHSPEC:
				if (ftype.ftype.addcode==1) p[j++]=i+k;
				break;
			case CODE_SHQZ:
				if (ftype.ftype.qz==1) p[j++]=i+k;
				break;				
			case CODE_SHBG:
				if (ftype.ftype.bstk==1) p[j++]=i+k;
				break;
			default:  
				if (ftype.ftype.qtstk==1) p[j++]=i+k; 
				break;
			}
			if(j>MAXGPNUM-1) return j;
		}
	}
	return j;
}

#include "FastIni.h"
void StockDataIo::LoadIndentInfo()
{
	CFastINI iniFile(1);
	CString strFile = g_WSXHStr+"mark.dat", strCode;
	if(iniFile.LoadIniFile(strFile))
	{
		for(int i=0;i<GetStockNum(ALL_ALL);i++)
		{
			MemStkInfo *pInfo = (*this)[i];
			if(!pInfo) continue;
			strCode.Format("%02d%s", pInfo->setcode, pInfo->Code);
			pInfo->fl_flag = iniFile.GetPrivateProfileInt("MARK", strCode, 0, strFile);
		}
	}
}

short StockDataIo::GetDomain( MemStkInfo * tmpscode )
{	
	if(tmpscode == NULL) return 0;//如果这空，返回为0(即SZ)
	return tmpscode->setcode;
}

//得到某类股票的总数,仅国内AB股
short   StockDataIo::GetStockNum(int flag)
{
	switch (flag)
	{
	case ALL_SZ:  return StockNum[SZ];
	case ALL_SH:  return StockNum[SH];
	case SZ_GP:	  return StockNum[SZ]-IndexNum[SZ];
	case SH_GP:	  return StockNum[SH]-IndexNum[SH];
	case ALL_ALL: return StockNum[SZ]+StockNum[SH];
	case DS_ALL: return m_nDSNum;
	}
	return 0;
}

float StockDataIo::GetStockClose(struct MemStkInfo *tmpscode)
{
	if ( !tmpscode ) return 0.0f;
	short setcode = GetDomain ( tmpscode );
	if(tmpscode->Index<DS_FROM)
		return hqhead[setcode][tmpscode->Index].Close;
	return m_aICList[tmpscode->Index-DS_FROM].Close;
}

float StockDataIo::GetStockNow(struct MemStkInfo *tmpscode)
{
	if ( !tmpscode ) return 0.0f;	
	CurrStockData iHq = {0};
	g_pStockIO->ReadCurrStockData(tmpscode, &iHq);
	return iHq.Now;
}

//处理财务和除权数据
void StockDataIo::GetGBBQ_Type(struct MemStkInfo * scode,struct AnalyData * AnalyDatap, char * fpBQtype,short ItemNum)
{
	if(scode == NULL) return;
	m_pCwcl->GetGBBQ_Type(scode,AnalyDatap,fpBQtype,ItemNum);
}

CWDATA * StockDataIo::GetGpCwDataStart(char *GpCode,short setcode,short &DataNum) //找到某股票的财务缓冲区入口
{
	if(!m_pCwcl) return NULL;
	return m_pCwcl->GetGpCwDataStart(GpCode,setcode,DataNum);
}

void StockDataIo::ReadLTGB(struct MemStkInfo * scode,struct AnalyData * AnalyDatap, float * fpLTGB,short ItemNum,float fDefaultLTGB)
{
	if(scode == NULL) return;

	float fDefaultZGB = 0;
	float *fpZGB = new float[ItemNum];
	long *pDate = new long[ItemNum];
	for(int i=0;i < ItemNum;i++)
		pDate[i] = AnalyDatap[i].Time.Date;
	m_pCwcl->ReadLTGB_ZGB(scode,pDate,fpLTGB,fpZGB,ItemNum,fDefaultLTGB,fDefaultZGB);
	delete pDate;
	delete fpZGB;
}

void StockDataIo::ReadLTGB_ZGB(struct MemStkInfo * scode,long *pDate, float * fpLTGB, float *fpZGB,short ItemNum,float fDefaultLTGB,float fDefaultZGB)
{
	if(scode == NULL) return;
	m_pCwcl->ReadLTGB_ZGB(scode,pDate,fpLTGB,fpZGB,ItemNum,fDefaultLTGB,fDefaultZGB);
}

void StockDataIo::ReadLTGB_ZGB_OneDate(struct MemStkInfo * scode,long Date, float * fpLTGB, float *fpZGB,float fDefaultLTGB,float fDefaultZGB)
{
	if(scode == NULL) return;
	m_pCwcl->ReadLTGB_ZGB_OneDate(scode,Date,fpLTGB,fpZGB,fDefaultLTGB,fDefaultZGB);
}

void StockDataIo::ReadRealTGB_ZGB(struct MemStkInfo * scode,long Date, long *pDate, float * fpLTGB, float *fpZGB,short ItemNum, float &fRealLTGB, float &fRealZGB)
{
	if(scode->J_start>19000000&&Date<scode->J_start)
	{
		fRealLTGB = 0;
		fRealZGB = 0;
		return;
	}
	for(int i=ItemNum-1;i>=0&&pDate[i]>Date;i--);
	if(i>=0) 
	{
		fRealLTGB = fpLTGB[i];
		fRealZGB = fpZGB[i];
	}
	else
	{
		fRealLTGB = scode->ActiveCapital;
		fRealZGB = scode->J_zgb; 
	}
}

short StockDataIo::GetTQInfo(struct MemStkInfo * scode,CWDATA *pOutData,short nMaxNum,BOOL bOnlyQk)
{
	if(scode == NULL) return 0;
	return m_pCwcl->GetTQInfo(scode,pOutData,nMaxNum,bOnlyQk);
}

short StockDataIo::GetRecentGbbqInfo(CWDATA *pOutData,short nMaxNum)
{
	return m_pCwcl->GetRecentGbbqInfo(pOutData,nMaxNum);
}

short StockDataIo::GetRLTData(struct MemStkInfo * scode, STKRELATION *pOutData, short nMaxNum)
{
	if(scode == NULL) return 0;
	return m_pCwcl->ReadRelationInfo(scode,pOutData,nMaxNum);
}

long StockDataIo::GetAllFaceData(TDXSTKOLDNAME* &pOutData)
{
	return m_pCwcl->ReadAllFaceInfo(pOutData);
}

short StockDataIo::GetFaceData(struct MemStkInfo * scode, TDXSTKOLDNAME *pOutData, short nMaxNum)
{
	if(scode == NULL) return 0;
	return m_pCwcl->ReadFaceInfo(scode,pOutData,nMaxNum);
}

void StockDataIo::FixTQDataFromDay(AnalyData * AnalyDatap,short nDataNum,AnalyData * pTempData,short nReadNum,short nPeriod,BOOL bDataValid,int nStkType)
{
	int i,j,k;
	BOOL bSpec = UseSpecVolumeUnit(nStkType,nPeriod);;	
	DWORD nUnit = 1;
	if(bSpec) nUnit = 100;
	for(i=0,j=-1,k=0;i<nReadNum && k<nDataNum;)
	{
		if(j==-1)
		{
			if(pTempData[i].Time.Date>AnalyDatap[k].Time.Date)
			{
				if(i==0)
				{
					k++;
					continue;
				}
				else
				{
					if(pTempData[i-1].Time.Date<=AnalyDatap[k].Time.Date)	//开始
					{
						j++;
						k++;
						pTempData[j].Time.Date	= pTempData[i].Time.Date;
						pTempData[j].Open       = pTempData[i].Open;
						pTempData[j].High       = pTempData[i].High;
						pTempData[j].Low        = pTempData[i].Low;
						pTempData[j].Close      = pTempData[i].Close;
						pTempData[j].Amount     = pTempData[i].Amount;
						pTempData[j].Volume     = pTempData[i].Volume/nUnit;
						pTempData[j].YClose     = pTempData[i].YClose;
						pTempData[j].zd.up      = pTempData[i].zd.up;
						pTempData[j].zd.down    = pTempData[i].zd.down;
						i++;
						continue;
					}
					else
					{
						k++;
						continue;
					}
				}
			}
			else
			{
				i++;
				continue;
			}
		}
		else
		{
			if(pTempData[i].Time.Date > AnalyDatap[k].Time.Date)
			{
				if(i>0 && pTempData[i-1].Time.Date <= AnalyDatap[k].Time.Date)
				{
					k++;
					j++;
					pTempData[j].Time.Date	= pTempData[i].Time.Date;
					pTempData[j].Open       = pTempData[i].Open;
					pTempData[j].High       = pTempData[i].High;
					pTempData[j].Low        = pTempData[i].Low;
					pTempData[j].Close      = pTempData[i].Close;
					pTempData[j].Amount     = pTempData[i].Amount;
					pTempData[j].Volume     = pTempData[i].Volume/nUnit;
					pTempData[j].YClose     = pTempData[i].YClose;
					pTempData[j].zd.up      = pTempData[i].zd.up;
					pTempData[j].zd.down    = pTempData[i].zd.down;
					i++;
				}
				else
				{
					k++;
					continue;
				}
			}
			else
			{
				pTempData[j].Time.Date	= pTempData[i].Time.Date;
				pTempData[j].High       = MAX(pTempData[j].High,pTempData[i].High);
				pTempData[j].Low        = MIN(pTempData[j].Low ,pTempData[i].Low );
				pTempData[j].Close      = pTempData[i].Close;
				pTempData[j].Amount    += pTempData[i].Amount;
				pTempData[j].Volume    += pTempData[i].Volume/nUnit;
				pTempData[j].YClose    += pTempData[i].YClose;
				pTempData[j].zd.up     += pTempData[i].zd.up;
				pTempData[j].zd.down   += pTempData[i].zd.down;
				i++;
			}
		}
	}
	for(i=0,k=1;i<nDataNum && k<=j;i++)
	{
		if(!bDataValid && k>=j) break;
		if(pTempData[k].Time.Date > AnalyDatap[i].Time.Date) continue;
		if(pTempData[k].Time.Date == AnalyDatap[i].Time.Date)
		{
			memcpy(&(AnalyDatap[i]),&(pTempData[k]),sizeof(ANALYDATA));
			k++;
		}
		if(pTempData[k].Time.Date < AnalyDatap[i].Time.Date)
		{
			k++;
			i--;
		}
	}
}

short StockDataIo::TQData(struct MemStkInfo * scode,struct AnalyData * AnalyDatap,short nFrom,short nEnd,short nDataNum,short nTQFlag,short nPeriod)
{
	//复权调整
    if (scode == NULL) return 0;
	if (testzs(scode)) return nDataNum;
	if (nDataNum <= 0) return 0;
	long i=0,j=0,k=0;
	int nStkType = GetStockType(scode->setcode,scode->Code);
	if(nPeriod==PER_MIN1||nPeriod==PER_MIN5||nPeriod==PER_MIN15||
		nPeriod==PER_MIN30||nPeriod==PER_MINN||nPeriod==PER_HOUR)
	{
		//最多两年
		int nYearFlag = 0;
		int nEnd_1 = nEnd, nFrom_1 = nFrom;
		for(int i=nEnd_1;i>nFrom_1;i--)
		{
			if(AnalyDatap[i].Time.Daye.Day<AnalyDatap[i-1].Time.Daye.Day) ++nYearFlag;
			if(nYearFlag>0&&AnalyDatap[i].Time.Daye.Day<=AnalyDatap[nEnd_1].Time.Daye.Day)
			{
				nFrom_1 = i;
				break;
			}
		}
		m_pCwcl->MinTQData(scode,AnalyDatap,nFrom_1,nEnd_1,nDataNum,nTQFlag,0);

		nYearFlag = 0;
		int nEnd_2 = nFrom_1-1, nFrom_2 = nFrom;
		if(nFrom_2<=nEnd_2)
		{
			for(int i=nEnd_2;i>nFrom_2;i--)
			{
				if(AnalyDatap[i].Time.Daye.Day<AnalyDatap[i-1].Time.Daye.Day) ++nYearFlag;
				if(nYearFlag>0&&AnalyDatap[i].Time.Daye.Day<=AnalyDatap[nEnd_2].Time.Daye.Day)
				{
					nFrom_2 = i;
					break;
				}
			}

			m_pCwcl->MinTQData(scode,AnalyDatap,nFrom_2,nEnd_2,nDataNum,nTQFlag,1);
		}
		return nDataNum;
	}
	else if(nPeriod == PER_DAY)
	{
		m_pCwcl->TQData(scode,AnalyDatap,nFrom,nEnd,nDataNum,nTQFlag,nPeriod);
		return nDataNum;
	}
	else
	{
		if(OnLine)
		{
			//在线除权
			m_pCwcl->TQData(scode,AnalyDatap,nFrom,nEnd,nDataNum,nTQFlag,nPeriod);
			//本地数据的完整性检查
			BOOL bDataValid = TRUE;
			long VipDocLastDate = GetVipDocLastDate();
			if( VipDocLastDate<g_lOpenRq && GetForwardDate(VipDocLastDate)<g_lOpenRq && !g_bLastRqDown) 
				bDataValid = FALSE;
			//申请本地数据
			long nReadNum = GetAnalyDataNum(scode,PER_DAY);
			if(nReadNum<=0) return nDataNum;
			struct CurrStockData TmpHQ;
			int tmpTodayNum;
			ANALYDATA *pTempData = new ANALYDATA[nReadNum+10];
			if(pTempData == NULL) return nDataNum;
			nReadNum  = ReadAnalyDataAndCurrHQ(scode,pTempData,nReadNum,PER_DAY,&TmpHQ,tmpTodayNum);
			BOOL bReadCache = FALSE;
			//本地数据整理
			switch(nTQFlag)
			{
			case 2:		//后复权,从已有历史数据范围内的第一个到最近一次除权标记,第一个数据需要对齐
				for(i=0;i<nReadNum;i++)
				{
					if(pTempData[i].Time.Date>=AnalyDatap[0].Time.Date) break;
				}
				if(i==0 && pTempData[i].Time.Date>AnalyDatap[0].Time.Date)	//前面数据不足,不考虑
				{
					TDELA(pTempData);
					return nDataNum;
				}
				if(i>0) memmove(pTempData,&(pTempData[i]),(nReadNum-i)*sizeof(ANALYDATA));
				nReadNum -= i;
				break;
			case 4:		//后复权,从指定时间到最近一次除权标记,第一个数据之前的要截掉,若指定时间在第一数据之后,指定时间要包含
				for(i=0;i<nReadNum;i++)
				{
					if(pTempData[i].Time.Date>=AnalyDatap[0].Time.Date) break;
				}
				if(i>0) memmove(pTempData,&(pTempData[i]),(nReadNum-i)*sizeof(ANALYDATA));
				nReadNum -= i;
				if(AnalyDatap[0].Time.Date<=g_nTqStart && pTempData[0].Time.Date>g_nTqStart)
				{
					TDELA(pTempData);
					return nDataNum;
				}
				break;
			}
			//本地除权
			m_pCwcl->TQData(scode,pTempData,0,nReadNum-1,nReadNum,nTQFlag,PER_DAY);
			FixTQDataFromDay(AnalyDatap,nDataNum,pTempData,nReadNum,nPeriod,bDataValid,nStkType);
			switch(nTQFlag)
			{
			case 1:
			case 3:
				if(!bDataValid)
				{
					ANALYDATA *pCacheData = new ANALYDATA[RECENTNUM+10];
					long nCacheNum = 0;
					if(pCacheData != NULL)
					{
						nCacheNum  = ReadCacheData(scode,scode->setcode,pCacheData,RECENTNUM,PER_DAY,1);
						if(nCacheNum>0)
						{
							m_pCwcl->TQData(scode,pCacheData,0,nCacheNum-1,nCacheNum,nTQFlag,PER_DAY);
							FixTQDataFromDay(AnalyDatap,nDataNum,pCacheData,nCacheNum,nPeriod,TRUE,nStkType);
						}
						TDELA(pCacheData);
					}
				}
				break;
			}
			TDELA(pTempData);
			return nDataNum;
		}
		else
		{
			long nReadNum = GetAnalyDataNum(scode,PER_DAY),nFlagDate;
			struct CurrStockData TmpHQ;
			int tmpTodayNum;
			ANALYDATA *pTempData = new ANALYDATA[nReadNum];
			nReadNum  = ReadAnalyDataAndCurrHQ(scode,pTempData,nReadNum,PER_DAY,&TmpHQ,tmpTodayNum);
			m_pCwcl->TQData(scode,pTempData,0,nReadNum-1,nReadNum,nTQFlag,PER_DAY);
			nFlagDate = -1;
			switch(nPeriod)
			{
			case PER_WEEK:		//周线除权
				for(i=0,j=-1; i<nReadNum; i++)
				{   if(GetFriday(pTempData[i].Time.Date)!=nFlagDate)
					{	j++;
						pTempData[j].Time.Date	= pTempData[i].Time.Date;
						pTempData[j].Open       = pTempData[i].Open;
						pTempData[j].High       = pTempData[i].High;
						pTempData[j].Low        = pTempData[i].Low;
						pTempData[j].Close      = pTempData[i].Close;
						pTempData[j].Amount     = pTempData[i].Amount;
						pTempData[j].Volume     = pTempData[i].Volume;
						pTempData[j].YClose     = pTempData[i].YClose;
						pTempData[j].zd.up      = pTempData[i].zd.up;
						pTempData[j].zd.down    = pTempData[i].zd.down;
						nFlagDate = GetFriday(pTempData[i].Time.Date);
					}
					else
					{	pTempData[j].Time.Date	= pTempData[i].Time.Date;
						pTempData[j].High       = MAX(pTempData[j].High,pTempData[i].High);
						pTempData[j].Low        = MIN(pTempData[j].Low ,pTempData[i].Low );
						pTempData[j].Close      = pTempData[i].Close;
						pTempData[j].Amount    += pTempData[i].Amount;
						pTempData[j].Volume    += pTempData[i].Volume;
						pTempData[j].YClose	   += pTempData[i].YClose;
						pTempData[j].zd.up     += pTempData[i].zd.up;
						pTempData[j].zd.down   += pTempData[i].zd.down;
					}
				}
				break;
			case PER_MONTH:		//月线除权
				for(i=0,j=-1; i<nReadNum; i++)
				{   if(pTempData[i].Time.Date/100!=nFlagDate)
					{	j++;
						pTempData[j].Time.Date	= pTempData[i].Time.Date;
						pTempData[j].Open       = pTempData[i].Open;
						pTempData[j].High       = pTempData[i].High;
						pTempData[j].Low        = pTempData[i].Low;
						pTempData[j].Close      = pTempData[i].Close;
						pTempData[j].Amount     = pTempData[i].Amount;
						pTempData[j].Volume     = pTempData[i].Volume;
						pTempData[j].YClose     = pTempData[i].YClose;
						pTempData[j].zd.up      = pTempData[i].zd.up;
						pTempData[j].zd.down    = pTempData[i].zd.down;
						nFlagDate = pTempData[i].Time.Date/100;
					}
					else
					{	pTempData[j].Time.Date	= pTempData[i].Time.Date;
						pTempData[j].High       = MAX(pTempData[j].High,pTempData[i].High);
						pTempData[j].Low        = MIN(pTempData[j].Low ,pTempData[i].Low );
						pTempData[j].Close      = pTempData[i].Close;
						pTempData[j].Amount    += pTempData[i].Amount;
						pTempData[j].Volume    += pTempData[i].Volume;
						pTempData[j].YClose    += pTempData[i].YClose;
						pTempData[j].zd.up     += pTempData[i].zd.up;
						pTempData[j].zd.down   += pTempData[i].zd.down;
					}
				}
				break;
			case PER_SEASON:
				for(i=0,j=-1; i<nReadNum; i++)
				{
					if((pTempData[i].Time.Date-100)%10000/300!=nFlagDate)
					{
						j++;
						pTempData[j].Time.Date	= pTempData[i].Time.Date;
						pTempData[j].Open       = pTempData[i].Open;
						pTempData[j].High       = pTempData[i].High;
						pTempData[j].Low        = pTempData[i].Low;
						pTempData[j].Close      = pTempData[i].Close;
						pTempData[j].Amount     = pTempData[i].Amount;
						pTempData[j].Volume     = pTempData[i].Volume;
						pTempData[j].YClose     = pTempData[i].YClose;
						pTempData[j].zd.up      = pTempData[i].zd.up;
						pTempData[j].zd.down    = pTempData[i].zd.down;
						nFlagDate = (pTempData[i].Time.Date-100)%10000/300;
					}
					else
					{
						pTempData[j].Time.Date	= pTempData[i].Time.Date;
						pTempData[j].High       = MAX(pTempData[j].High,pTempData[i].High);
						pTempData[j].Low        = MIN(pTempData[j].Low ,pTempData[i].Low );
						pTempData[j].Close      = pTempData[i].Close;
						pTempData[j].Amount    += pTempData[i].Amount;
						pTempData[j].Volume    += pTempData[i].Volume;
						pTempData[j].YClose    += pTempData[i].YClose;
						pTempData[j].zd.up     += pTempData[i].zd.up;
						pTempData[j].zd.down   += pTempData[i].zd.down;
					}
				}
				break;
			case PER_YEAR:
				for(i=0,j=-1; i<nReadNum; i++)
				{
					if(pTempData[i].Time.Date/10000!=nFlagDate)
					{
						j++;
						pTempData[j].Time.Date	= pTempData[i].Time.Date;
						pTempData[j].Open       = pTempData[i].Open;
						pTempData[j].High       = pTempData[i].High;
						pTempData[j].Low        = pTempData[i].Low;
						pTempData[j].Close      = pTempData[i].Close;
						pTempData[j].Amount     = pTempData[i].Amount;
						pTempData[j].Volume     = pTempData[i].Volume;
						pTempData[j].YClose     = pTempData[i].YClose;
						pTempData[j].zd.up      = pTempData[i].zd.up;
						pTempData[j].zd.down    = pTempData[i].zd.down;
						nFlagDate = pTempData[i].Time.Date/10000;
					}
					else
					{
						pTempData[j].Time.Date	= pTempData[i].Time.Date;
						pTempData[j].High       = MAX(pTempData[j].High,pTempData[i].High);
						pTempData[j].Low        = MIN(pTempData[j].Low ,pTempData[i].Low );
						pTempData[j].Close      = pTempData[i].Close;
						pTempData[j].Amount    += pTempData[i].Amount;
						pTempData[j].Volume    += pTempData[i].Volume;
						pTempData[j].YClose    += pTempData[i].YClose;
						pTempData[j].zd.up     += pTempData[i].zd.up;
						pTempData[j].zd.down   += pTempData[i].zd.down;
					}
				}
				break;
			}
			memmove(AnalyDatap,&pTempData[MAX(j+1-nDataNum,0)],MIN(nDataNum,j+1)*sizeof(ANALYDATA));
			delete []pTempData;pTempData=NULL;		
			return MIN(nDataNum,j+1);
		}
	}
	return nDataNum;
}

//读某股票的行情
BOOL StockDataIo::ReadCurrStockData(struct MemStkInfo *tmpscode,
									 struct CurrStockData * hqp,struct CurrStockData_L2 * hqp_l2,struct ExCurrData *hqexp)
{   
	if(!hqp) return FALSE;
	memset(hqp,0,sizeof(struct CurrStockData));
	if(hqp_l2) 
		memset(hqp_l2,0,sizeof(struct CurrStockData_L2));
	if(hqexp) 
		memset(hqexp,0,sizeof(EXCURRDATA));
    if (tmpscode == NULL) return FALSE;
    short tmpsetcode = GetDomain(tmpscode);
	if(tmpscode->Index>=DS_FROM) 
		DSGetCurrData(tmpscode,hqp,hqexp);
	else	
	{
		if(tmpsetcode!=SZ && tmpsetcode!=SH) return FALSE;
		if(tmpscode->Index>=StockNum[tmpsetcode]) return FALSE;
		memcpy( hqp, &(hqhead[tmpsetcode][tmpscode->Index]), sizeof(struct CurrStockData));
		if(hqp_l2)
			memcpy( hqp_l2, &(hqhead_l2[tmpsetcode][tmpscode->Index]), sizeof(struct CurrStockData_L2));
	}
	if(tmpscode->Close<COMPPREC && tmpscode->bigmarket==FE_TYPE)
		tmpscode->Close=hqp->Close;
    return TRUE;
};

BOOL StockDataIo::ReadCurrStockDataForCode(struct CurrStockData * hq_tmp,const char * Code,short setcode,struct CurrStockData_L2 * hqp_l2,struct ExCurrData *hqexp)
{   
	if(!hq_tmp) return FALSE;
	memset(hq_tmp,0,sizeof(struct CurrStockData));
	if(hqp_l2) 
		memset(hqp_l2,0,sizeof(struct CurrStockData_L2));
	if(hqexp) 
		memset(hqexp,0,sizeof(EXCURRDATA));
	struct MemStkInfo * tmpscode = Get_MemStkInfo(Code,setcode);
    if (tmpscode == NULL) return FALSE;
    ReadCurrStockData(tmpscode,hq_tmp,hqp_l2,hqexp);
    return TRUE;
}

BOOL StockDataIo::ReadCurrQueue(struct MemStkInfo *tmpscode,struct CurrQueue * pQueue)
{   
	if(!pQueue)		return FALSE;
	memset(pQueue,0,sizeof(struct CurrQueue));
	if(!tmpscode)	return FALSE;
    short tmpsetcode = GetDomain(tmpscode);
	if(tmpsetcode!=SZ && tmpsetcode!=SH) 
		return FALSE;
	if(tmpscode->Index>=StockNum[tmpsetcode]) 
		return FALSE;
	memcpy( pQueue, &(hqqueue[tmpsetcode][tmpscode->Index]), sizeof(struct CurrQueue));
    return TRUE;
};

BOOL StockDataIo::ReadCurrMarkInfo( struct MemStkInfo *tmpscode, struct MarkInfo * mrk_tmp )
{    
    short tmpsetcode = GetDomain(tmpscode);
	if(tmpsetcode!=SZ && tmpsetcode!=SH) return FALSE;
	if(tmpscode->Index>=StockNum[tmpsetcode]) return FALSE;
	memcpy( mrk_tmp, &(hqmark[tmpsetcode][tmpscode->Index]), sizeof(struct MarkInfo));
	return TRUE;
}

BOOL StockDataIo::ReadCurrStatData( struct MemStkInfo *tmpscode, struct  SumImgStat * Img_tmp,struct SumTckStat * Tck_tmp)
{    
    short tmpsetcode = GetDomain(tmpscode);
	if(tmpsetcode!=SZ && tmpsetcode!=SH) return FALSE;
	if(tmpscode->Index>=StockNum[tmpsetcode]) return FALSE;
	if(Img_tmp)
		memcpy( Img_tmp, &(hqImgStat[tmpsetcode][tmpscode->Index]), sizeof(struct SumImgStat));
	if(Tck_tmp)
		memcpy( Tck_tmp, &(hqTckStat[tmpsetcode][tmpscode->Index]), sizeof(struct SumTckStat));
	return TRUE;
}

short StockDataIo::DayTranWeekOrMonOrDayN(struct AnalyData * TempAnalyDatap,
						struct AnalyData * AnalyDatap,
						short ItemNum,short ReadedDayNum,short type,char * code,short setcode,short mulnum)
{
	MemStkInfo *pStock=Get_MemStkInfo(code,setcode);
	BOOL bSpec=FALSE;
	if(pStock)
		bSpec = UseSpecVolumeUnit(GetStockType(setcode,code),type);;
	short i,ret = 0;
	long Start_Day = 0,now_date;
	memset(AnalyDatap,0,sizeof(struct AnalyData) * ItemNum);
	if(type == PER_DAYN) //对于N天日线情况
	{
		int tmpCount = mulnum-ReadedDayNum%mulnum;
		for(i = 0;i<ReadedDayNum;i++)
		{
			if(ret==0 || tmpCount==mulnum)
			{
				tmpCount = 0;
				ret ++;
				if(ret>ItemNum) break;
				AnalyDatap[ret -1].l_Open = TempAnalyDatap[i].l_Open;
				AnalyDatap[ret -1].l_High = TempAnalyDatap[i].l_High;
				AnalyDatap[ret -1].l_Low  = TempAnalyDatap[i].l_Low;
			}
			AnalyDatap[ret-1].Time.Date = TempAnalyDatap[i].Time.Date;
			if (AnalyDatap[ret-1].l_High < TempAnalyDatap[i].l_High)
				AnalyDatap[ret-1].l_High = TempAnalyDatap[i].l_High;
			if (AnalyDatap[ret-1].l_Low  > TempAnalyDatap[i].l_Low)
				AnalyDatap[ret-1].l_Low  = TempAnalyDatap[i].l_Low;
			AnalyDatap[ret-1].l_Close    = TempAnalyDatap[i].l_Close;
			AnalyDatap[ret-1].Amount    += TempAnalyDatap[i].Amount;			
			if(pStock && bSpec)
				AnalyDatap[ret-1].Volume += TempAnalyDatap[i].Volume/100;
			else
				AnalyDatap[ret-1].Volume += TempAnalyDatap[i].Volume;
			if (testzs(code,setcode))
			{	AnalyDatap[ret-1].zd.up   += TempAnalyDatap[i].zd.up;
				AnalyDatap[ret-1].zd.down += TempAnalyDatap[i].zd.down;
			}
			else AnalyDatap[ret-1].YClose  = TempAnalyDatap[i].YClose;

			tmpCount++;
		}
		return ret;
	}
	for (i = 0;i<ReadedDayNum;i++)
	{
		now_date = TempAnalyDatap[i].Time.Date;
		switch(type)
		{
		case PER_WEEK:
			if ( GetFriday(now_date) != Start_Day)
			{   Start_Day = GetFriday(now_date);
				ret ++;
				if(ret>ItemNum) break;				
				AnalyDatap[ret -1].l_Open = TempAnalyDatap[i].l_Open;
				AnalyDatap[ret -1].l_High = TempAnalyDatap[i].l_High;
				AnalyDatap[ret -1].l_Low  = TempAnalyDatap[i].l_Low;
			}
			break;
		case PER_MONTH:
			if ( GetMonth1st(now_date) != Start_Day)
			{   Start_Day = GetMonth1st(now_date);
				ret ++;
				if(ret>ItemNum) break;
				AnalyDatap[ret -1].l_Open = TempAnalyDatap[i].l_Open;
				AnalyDatap[ret -1].l_High = TempAnalyDatap[i].l_High;
				AnalyDatap[ret -1].l_Low  = TempAnalyDatap[i].l_Low;
			}
			break;
		case PER_SEASON:
			if ( GetSeason1st(now_date) != Start_Day)
			{   Start_Day = GetSeason1st(now_date);
				ret ++;
				if(ret>ItemNum) break;
				AnalyDatap[ret -1].l_Open = TempAnalyDatap[i].l_Open;
				AnalyDatap[ret -1].l_High = TempAnalyDatap[i].l_High;
				AnalyDatap[ret -1].l_Low  = TempAnalyDatap[i].l_Low;
			}
			break;
		case PER_YEAR:
			if ( GetYear1st(now_date) != Start_Day)
			{   Start_Day = GetYear1st(now_date);
				ret ++;
				if(ret>ItemNum) break;
				AnalyDatap[ret -1].l_Open = TempAnalyDatap[i].l_Open;
				AnalyDatap[ret -1].l_High = TempAnalyDatap[i].l_High;
				AnalyDatap[ret -1].l_Low  = TempAnalyDatap[i].l_Low;
			}
			break;
		}
		if(ret>0 && ret<=ItemNum)
		{
			AnalyDatap[ret-1].Time.Date = TempAnalyDatap[i].Time.Date;
			if (AnalyDatap[ret-1].l_High < TempAnalyDatap[i].l_High)
				AnalyDatap[ret-1].l_High = TempAnalyDatap[i].l_High;
			if (AnalyDatap[ret-1].l_Low  > TempAnalyDatap[i].l_Low)
				AnalyDatap[ret-1].l_Low  = TempAnalyDatap[i].l_Low;
			AnalyDatap[ret-1].l_Close    = TempAnalyDatap[i].l_Close;
			AnalyDatap[ret-1].Amount    += TempAnalyDatap[i].Amount;
			if(pStock && bSpec)
				AnalyDatap[ret-1].Volume	+= TempAnalyDatap[i].Volume/100;
			else
				AnalyDatap[ret-1].Volume    += TempAnalyDatap[i].Volume;
			if (testzs(code,setcode))
			{	AnalyDatap[ret-1].zd.up   += TempAnalyDatap[i].zd.up;
				AnalyDatap[ret-1].zd.down += TempAnalyDatap[i].zd.down;
			}
			else AnalyDatap[ret-1].YClose  = TempAnalyDatap[i].YClose;
		}
	}
	return ret;
}

//得到分析数据的大致数目
long StockDataIo::GetAnalyDataNum(struct MemStkInfo * scode,short Type,short mulnum)
{
	/////////////////
	char string[256];
	int	 RetNum = 0;
    if(scode == NULL) return 0;
    int  setcode = GetDomain ( scode );
	BOOL bDSStk = BeDSStock(scode);
	switch(Type)
	{
	case PER_MIN1:
	case PER_MINN:
		return 0;
	case PER_MIN5:
	case PER_MIN15:
	case PER_MIN30:
	case PER_HOUR:
		{
			sprintf( string, "%s%s\\fzline\\%s%s.lc5", g_TdxCfg.DocPath, GetSetStr(setcode),GetSetStr(setcode),scode->Code);
			CFileStatus FileStatus;
			if( CFile::GetStatus(string,FileStatus) )
			{
				short ratio = anFZRatio[Type]/5;
				long size = FileStatus.m_size;
				RetNum = size/sizeof(struct AnalyData);
				RetNum /= ratio;
			}
		}
		break;
	case PER_WEEK:
	case PER_MONTH:
	case PER_SEASON:
	case PER_YEAR:
		{
			if(bDSStk) sprintf(string,"%sds\\day\\%d#%s.day",g_TdxCfg.DocPath,(scode->setcode!=32)?scode->setcode:31,scode->Code);
			else sprintf(string, "%s%s\\Day\\%s%s.day", g_TdxCfg.DocPath, GetSetStr(setcode),GetSetStr(setcode),scode->Code);
			CFileStatus FileStatus;
			if( CFile::GetStatus(string,FileStatus) )
			{
				long size = FileStatus.m_size;
				RetNum = size/sizeof(struct AnalyData);
			}
		}
		break;
	case PER_DAY:
	case PER_DAYN:
		{
			int Multiple=(Type==PER_DAY)?1:mulnum;
			if(bDSStk) sprintf(string,"%sds\\day\\%d#%s.day",g_TdxCfg.DocPath,(scode->setcode!=32)?scode->setcode:31,scode->Code);
			else sprintf(string, "%s%s\\Day\\%s%s.day", g_TdxCfg.DocPath, GetSetStr(setcode),GetSetStr(setcode),scode->Code);
			CFileStatus FileStatus;
			if( CFile::GetStatus(string,FileStatus) )
			{
				long size = FileStatus.m_size;
				RetNum = size/(sizeof(struct AnalyData)*Multiple);
			}
		}
		break;
	}
	RetNum+=1;	//不管是多日线还是日线，都加1，否则算多日线时容易出错
	return RetNum;
}

//得到扩展数据的大致数目
long StockDataIo::GetExDayDataNum(struct MemStkInfo * scode)
{
	if(g_pExdayMng && scode)
		return g_pExdayMng->ReadStkExday(NULL,scode->setcode,scode->Code,0);
	return 0;
}

//根据偏移来读历史数据
short StockDataIo::ReadAnalyDataOffset(struct MemStkInfo * scode,struct AnalyData * AnalyDatap, short ItemNum ,short Type, long offset,short mulnum)
{
    if(scode == NULL) return 0;

	long TotalNum = GetAnalyDataNum(scode,Type,mulnum);
	struct AnalyData * pAnalyDataTmp = new AnalyData[TotalNum+1];

	struct CurrStockData hqtmp;
	short  nReadDataNum,start,datanum;
	int	nTmp;
	datanum = 0;
	nReadDataNum = ReadAnalyDataAndCurrHQ(scode,pAnalyDataTmp,TotalNum,Type,&hqtmp,nTmp,mulnum);
	if( nReadDataNum > offset )
	{
		start   = MAX(0,nReadDataNum - (offset+ItemNum));
		datanum = MIN(ItemNum,nReadDataNum-offset);
		memmove ( AnalyDatap, pAnalyDataTmp + start, (long)datanum * sizeof ( struct AnalyData ) );
	}

	delete pAnalyDataTmp;
	return datanum;
}

//根据两个时间来读历史数据(只针对日线等),AnalyDatap应该足够大
short StockDataIo::ReadAnalyDataFromTime(struct MemStkInfo * scode,struct AnalyData * AnalyDatap,short ItemNum,long StartDate,long EndDate,short Type,short mulnum)
{
	if(Type!=PER_DAY && Type!=PER_WEEK && Type!=PER_MONTH && Type!=PER_DAYN && Type!=PER_SEASON && Type!=PER_YEAR) return 0;
    if(scode == NULL) return 0;

	long TotalNum;
	short dataflag = 1000;
	TotalNum = GetAnalyDataNum(scode,Type,mulnum);
	struct AnalyData * pAnalyDataTmp = new AnalyData[TotalNum];

	struct CurrStockData hqtmp;
	short  nReadDataNum;
	int	nTmp;
	nReadDataNum = ReadAnalyDataAndCurrHQ(scode,pAnalyDataTmp,TotalNum,Type,&hqtmp,nTmp,mulnum,dataflag);
	int	startpos=-1,endpos=-1;
	for(int i = 0;i < nReadDataNum;i++)
	{
		if(startpos == -1 && pAnalyDataTmp[i].Time.Date >= StartDate)
			startpos = i;
		if(pAnalyDataTmp[i].Time.Date  >= EndDate) 
		{
			endpos = i;	break;
		}
	}
	if(nReadDataNum > 0 && (pAnalyDataTmp[0].Time.Date > EndDate || pAnalyDataTmp[nReadDataNum-1].Time.Date < StartDate))
	{
		delete pAnalyDataTmp;
		return 0;
	}
	if(startpos == -1)
	{
		delete pAnalyDataTmp;
		return 0;
	}
	if(endpos>1 && pAnalyDataTmp[endpos].Time.Date>EndDate && pAnalyDataTmp[endpos-1].Time.Date<StartDate)
	{
		delete pAnalyDataTmp;
		return 0;
	}
	if(endpos == -1) endpos = nReadDataNum - 1;

	if( endpos-startpos+1 > ItemNum )	//暂时判断
		startpos = endpos+1 - ItemNum;

	memmove( AnalyDatap,pAnalyDataTmp+startpos,(endpos-startpos+1)*sizeof(struct AnalyData) );

	delete pAnalyDataTmp;
	return endpos-startpos+1;
}

short StockDataIo::ReadExDayData(struct MemStkInfo * scode,LPEXDAYINFO pExdayInfo,short ItemNum)
{
    if(scode == NULL || pExdayInfo==NULL || ItemNum<=0) return 0;
	//得到扩展数据的个数
	long TotalNum = GetExDayDataNum(scode);
	LPEXDAYDATA pExdayDataTmp = new EXDAYDATA[TotalNum+10];
	if(pExdayDataTmp == NULL) return 0;
	//读取所有扩展数据
	int nReadDataNum = 0;
	int i=0;
	char string[256];
    int  setcode = GetDomain(scode);
	
	if(g_pExdayMng)
		nReadDataNum=g_pExdayMng->ReadStkExday(pExdayDataTmp,setcode,scode->Code,TotalNum);
	if(nReadDataNum<=0) nReadDataNum = 0;
	if(nReadDataNum>ItemNum)
	{
		memmove(pExdayDataTmp,&(pExdayDataTmp[nReadDataNum-ItemNum]),ItemNum*sizeof(EXDAYDATA));
		nReadDataNum = ItemNum;
	}
	for(i=0;i<nReadDataNum;i++)
	{
		pExdayInfo[i].Date			= pExdayDataTmp[i].Time;
		//
		pExdayInfo[i].Cjbs			= pExdayDataTmp[i].Cjbs;
		pExdayInfo[i].BuyCjbs		= long(max(0,min((1.0*pExdayDataTmp[i].Cjbs)*pExdayDataTmp[i].BuyCjbsRatio/50000.0,pExdayDataTmp[i].Cjbs)));
		pExdayInfo[i].SellCjbs		= long(max(0,min((1.0*pExdayDataTmp[i].Cjbs)*pExdayDataTmp[i].SellCjbsRatio/50000.0,pExdayDataTmp[i].Cjbs)));
		pExdayInfo[i].BigBCjbs		= long(max(0,min((1.0*pExdayInfo[i].BuyCjbs)*pExdayDataTmp[i].BigBuyCjbsRatio/50000.0,pExdayInfo[i].BuyCjbs)));
		pExdayInfo[i].BigSCjbs		= long(max(0,min((1.0*pExdayInfo[i].SellCjbs)*pExdayDataTmp[i].BigSellCjbsRatio/50000.0,pExdayInfo[i].SellCjbs)));
		//
		pExdayInfo[i].ItemNum		= pExdayDataTmp[i].ItemNum;
		pExdayInfo[i].BuyItem		= long(max(0,min((1.0*pExdayDataTmp[i].ItemNum)*pExdayDataTmp[i].BuyItemRatio/50000.0,pExdayDataTmp[i].ItemNum)));
		pExdayInfo[i].SellItem		= max(0,min(pExdayDataTmp[i].ItemNum,pExdayDataTmp[i].ItemNum-pExdayInfo[i].BuyItem));
		pExdayInfo[i].BigItem		= long(max(0,min((1.0*pExdayDataTmp[i].ItemNum)*pExdayDataTmp[i].BigItemRatio/50000.0,pExdayDataTmp[i].ItemNum)));
		pExdayInfo[i].BigBItem		= long(max(0,min((1.0*pExdayInfo[i].BigItem)*pExdayDataTmp[i].BigBuyItemRatio/50000.0,pExdayInfo[i].BigItem)));
		pExdayInfo[i].BigSItem		= max(0,min(pExdayInfo[i].BigItem,pExdayInfo[i].BigItem-pExdayInfo[i].BigBItem));
		//
		pExdayInfo[i].BuyVolume		= long(max(0,min((1.0*pExdayDataTmp[i].Volume)*pExdayDataTmp[i].OutSideRatio/50000.0,pExdayDataTmp[i].Volume)));
		pExdayInfo[i].SellVolume	= max(0,min(pExdayDataTmp[i].Volume,pExdayDataTmp[i].Volume-pExdayInfo[i].BuyVolume));
		pExdayInfo[i].BigVolume		= long(max(0,min((1.0*pExdayDataTmp[i].Volume)*pExdayDataTmp[i].BigVolRatio/50000.0,pExdayDataTmp[i].Volume)));
		pExdayInfo[i].BigBVolume	= long(max(0,min((1.0*pExdayInfo[i].BigVolume)*pExdayDataTmp[i].BigBuyVolRatio/50000.0,pExdayInfo[i].BigVolume)));
		pExdayInfo[i].BigSVolume	= max(0,min(pExdayInfo[i].BigVolume,pExdayInfo[i].BigVolume-pExdayInfo[i].BigBVolume));
		//
		pExdayInfo[i].BigTickBVol	= long(max(0,min((1.0*pExdayDataTmp[i].Volume)*pExdayDataTmp[i].BigBuyTckRatio/50000.0,pExdayDataTmp[i].Volume)));
		pExdayInfo[i].BigTickSVol	= long(max(0,min((1.0*pExdayDataTmp[i].Volume)*pExdayDataTmp[i].BigSellTckRatio/50000.0,pExdayDataTmp[i].Volume)));
		//
		pExdayInfo[i].fBOrder		= pExdayDataTmp[i].fBOrder;
		pExdayInfo[i].fBCancel		= pExdayDataTmp[i].fBCancel;
		pExdayInfo[i].fSOrder		= pExdayDataTmp[i].fSOrder;
		pExdayInfo[i].fSCancel		= pExdayDataTmp[i].fSCancel;
		pExdayInfo[i].fTotalBOrder	= max(0,pExdayDataTmp[i].fBOrder-pExdayDataTmp[i].fBCancel-pExdayDataTmp[i].Volume);
		pExdayInfo[i].fTotalSOrder	= max(0,pExdayDataTmp[i].fSOrder-pExdayDataTmp[i].fSCancel-pExdayDataTmp[i].Volume);
		pExdayInfo[i].fBuyAvp		= pExdayDataTmp[i].fBuyAvp;
		pExdayInfo[i].fSellAvp		= pExdayDataTmp[i].fSellAvp;
	}
	if(nReadDataNum > 0 && g_lOpenRq > pExdayInfo[nReadDataNum-1].Date)
	{
		if(nReadDataNum>=ItemNum)
		{
			memmove(&(pExdayInfo[0]),&(pExdayInfo[1]),(nReadDataNum-1)*sizeof(EXDAYINFO));
			nReadDataNum--;
		}
		CurrStockData		hq;
		CurrStockData_L2	hq2;
		EXDAYINFO			curexday;
		SUMIMGSTAT			ImgStat;
		SUMTCKSTAT			TckStat;
		memset(&curexday,0,sizeof(EXDAYINFO));
		if(ReadCurrStockData(scode,&hq,&hq2))
		{
			curexday.Date			= g_lOpenRq;
			curexday.Cjbs			= hq.CJBS;
			curexday.ItemNum		= hq.ItemNum;
			curexday.BuyVolume		= hq.Outside;
			curexday.SellVolume		= hq.Inside;
			curexday.fTotalBOrder	= hq2.TotalBuyv;
			curexday.fTotalSOrder	= hq2.TotalSellv;
			curexday.fBuyAvp		= hq2.BuyAvp;
			curexday.fSellAvp		= hq2.SellAvp;
		}
		if(ReadCurrStatData(scode,&ImgStat,&TckStat))
		{
			curexday.BuyCjbs		= TckStat.VolNum[0][0]+TckStat.VolNum[0][2]+TckStat.VolNum[0][4];
			curexday.SellCjbs		= TckStat.VolNum[0][1]+TckStat.VolNum[0][3]+TckStat.VolNum[0][4];
			curexday.BigBCjbs		= TckStat.VolNum[1][0]+TckStat.VolNum[1][2]+TckStat.VolNum[1][4];
			curexday.BigSCjbs		= TckStat.VolNum[1][1]+TckStat.VolNum[1][3]+TckStat.VolNum[1][4];
			//
			curexday.BuyItem		= max(0,min((ImgStat.InItemNum+curexday.ItemNum-ImgStat.OutItemNum)/2,curexday.ItemNum));
			curexday.SellItem		= max(0,curexday.ItemNum-curexday.BuyItem);
			curexday.BigItem		= max(0,min(ImgStat.LargeItemNum,curexday.ItemNum));
			curexday.BigBItem		= max(0,min((ImgStat.LargeInItemNum+ImgStat.LargeItemNum-ImgStat.LargeOutItemNum)/2,curexday.BigItem));
			curexday.BigSItem		= max(0,min(curexday.BigItem-curexday.BigBItem,curexday.BigItem));
			//
			curexday.BigVolume		= max(0,min(ImgStat.LargeVol,hq.Volume));
			curexday.BigBVolume		= max(0,min((ImgStat.LargeInVol+ImgStat.LargeVol-ImgStat.LargeOutVol)/2,curexday.BigVolume));
			curexday.BigSVolume		= max(0,min(curexday.BigVolume-curexday.BigBVolume,curexday.BigVolume));
			curexday.BigTickBVol	= TckStat.fVol[1][0]+TckStat.fVol[1][2]+TckStat.fVol[1][4];
			curexday.BigTickSVol	= TckStat.fVol[1][1]+TckStat.fVol[1][3]+TckStat.fVol[1][4];
			//
			curexday.fBOrder		= ImgStat.fBOrder;
			curexday.fBCancel		= ImgStat.fBCancel;
			curexday.fSOrder		= ImgStat.fSOrder;
			curexday.fSCancel		= ImgStat.fSCancel;
		}
		memcpy(&(pExdayInfo[nReadDataNum]),&curexday,sizeof(EXDAYINFO));
		nReadDataNum++;
	}

	delete[] pExdayDataTmp;
	return nReadDataNum;
}

short StockDataIo::ReadVipDocData(struct MemStkInfo * scode, struct AnalyData * AnalyDatap, short ItemNum ,short Type,short mulnum,short dataflag)
{
	short  i,j;
	int    nReadDataNum;
	char   string[256];
	long   filelen;
	short  ratio;
    struct AnalyData * pAnalyDataTmp;

    if(scode == NULL) return 0;
    int  setcode = GetDomain ( scode );
	BOOL bDSStk = BeDSStock( scode );
	short stockkind = GetStockType(scode->setcode,scode->Code);
    switch (Type)
	{
	case PER_DAY:
		if(bDSStk)
		{
			sprintf(string,"%sds\\day\\%d#%s.day",g_TdxCfg.DocPath,(scode->setcode!=32)?scode->setcode:31,scode->Code);
			g_HisDataReader.Lock();
			FILE * fpt = _fsopen(string, "rb",_SH_DENYNO);
			if(fpt)
			{
				filelen = on_filelength(fpt);
				nReadDataNum = filelen/sizeof(struct AnalyData);
				fread(AnalyDatap,sizeof(struct AnalyData),(long)nReadDataNum,fpt);
				fclose(fpt);
			}
			else nReadDataNum = 0;
			g_HisDataReader.Unlock();
			return nReadDataNum;
		}
		else		
			sprintf( string, "%s%s\\day\\%s%s.day", g_TdxCfg.DocPath, GetSetStr(setcode),GetSetStr(setcode),scode->Code);
		break;
	case PER_WEEK:
	case PER_MONTH:
	case PER_SEASON:
	case PER_YEAR:
		{
			short  MulTiple=300;
			if(Type==PER_WEEK)
				MulTiple=5;
			else if(Type==PER_MONTH)
				MulTiple=24;
			long   MulWantNum = min(30000,ItemNum * MulTiple);
			struct AnalyData * TempAnalyDatap = new AnalyData[MulWantNum];
			short ReadedDayNum = ReadVipDocData(scode, TempAnalyDatap,MulWantNum,PER_DAY,mulnum,0);
			short ReadedMonNum = DayTranWeekOrMonOrDayN(TempAnalyDatap,AnalyDatap,ItemNum,ReadedDayNum,Type,scode->Code,scode->setcode,mulnum);
			delete TempAnalyDatap;
			return ReadedMonNum;
		}
		break;
	case PER_DAYN:
		{
			long   MulWantNum = min(30000,ItemNum * mulnum);
			struct AnalyData * TempAnalyDatap = new AnalyData[MulWantNum];
			short ReadedDayNum = ReadVipDocData(scode, TempAnalyDatap,MulWantNum,PER_DAY,mulnum,0);
			short ReadedDayNNum = DayTranWeekOrMonOrDayN(TempAnalyDatap,AnalyDatap,ItemNum,ReadedDayNum,Type,scode->Code,scode->setcode,mulnum);
			delete TempAnalyDatap;
			return ReadedDayNNum;
		}
		break;
	case PER_MIN5:
	case PER_MIN15:
	case PER_MIN30:
	case PER_HOUR:
		sprintf( string, "%s%s\\fzline\\%s%s.lc5", g_TdxCfg.DocPath, GetSetStr(setcode),GetSetStr(setcode),scode->Code);
		break;
	default :
		return 0;
   }
	g_HisDataReader.Lock();
	FILE * fpt = _fsopen(string, "rb",_SH_DENYNO);
	if ( fpt==NULL ) 
	{
		g_HisDataReader.Unlock();
		return 0;
	}
	filelen = on_filelength(fpt);
	ratio   = anFZRatio[Type];
	if(ratio != 1) ratio /= 5; //修正ratio
	nReadDataNum = MIN(ItemNum * ratio,filelen/sizeof(struct AnalyData));
	if (nReadDataNum == 0) {fclose(fpt);g_HisDataReader.Unlock();return 0;}
	fseek( fpt, -(long)sizeof(struct AnalyData) * nReadDataNum, SEEK_END );
	if (ratio != 1)
	{     
		pAnalyDataTmp = new AnalyData[nReadDataNum];
		if (pAnalyDataTmp==NULL) {fclose(fpt);g_HisDataReader.Unlock();return 0;}
		fread ( pAnalyDataTmp, sizeof(AnalyData),(long)nReadDataNum,fpt);
		nReadDataNum/=ratio;
		for ( i=0; i < nReadDataNum; i++ )
		{   
			memmove( AnalyDatap + i, pAnalyDataTmp+ i * ratio, sizeof(struct AnalyData) );
			for ( j=1; j<ratio; j++ )
			{   
				AnalyDatap[i].l_High = MAX( AnalyDatap[i].l_High, pAnalyDataTmp[i*ratio+j].l_High );
				AnalyDatap[i].l_Low = MIN( AnalyDatap[i].l_Low, pAnalyDataTmp[i*ratio+j].l_Low );
				AnalyDatap[i].Volume += pAnalyDataTmp[ i*ratio + j ].Volume;
				AnalyDatap[i].Amount += pAnalyDataTmp[ i*ratio + j ].Amount;
			}
			AnalyDatap[i].l_Close= pAnalyDataTmp[ i*ratio + ratio - 1 ].l_Close;
			AnalyDatap[i].Time.Daye.Day = pAnalyDataTmp[ i*ratio+ratio-1 ].Time.Daye.Day;
			AnalyDatap[i].Time.Daye.Minute = pAnalyDataTmp[ i*ratio+ratio-1 ].Time.Daye.Minute;
		}
		delete pAnalyDataTmp;
	}
	else
		fread (  AnalyDatap,sizeof(struct AnalyData),(long)nReadDataNum,fpt);
	fclose(fpt);
	g_HisDataReader.Unlock();

	if(Type>=PER_MIN5&&Type<=PER_HOUR) return nReadDataNum;	//本地的lc5文件存放的本来就是float结构,这点与日线不一样

	float  funit=(GetXSFlag(scode)==XS3)?0.001:0.01;
	if(stockkind==CODE_SZGZHG ||stockkind==CODE_SHGZHG)
		funit = 0.01f;
	for (i=0; i<nReadDataNum; i++)
	{  //转为小数
	   AnalyDatap[i].Open  = funit * (float)(AnalyDatap[i].l_Open);
	   AnalyDatap[i].Close = funit * (float)(AnalyDatap[i].l_Close);
	   AnalyDatap[i].High  = funit * (float)(AnalyDatap[i].l_High);
	   AnalyDatap[i].Low   = funit * (float)(AnalyDatap[i].l_Low);
	}
	if(!testzs(scode))
	{
		for (i=0; i<nReadDataNum; i++)
			AnalyDatap[i].YClose = 0.0;
	}
	////
	if(Type==PER_DAY)
	{
		BOOL bZS = testzs(scode);
		if(stockkind==CODE_SZJJ || stockkind==CODE_SHJJ)
		{
			for(i=0;i<nReadDataNum&&AnalyDatap[i].Time.Date<20030303;++i)
			{
				AnalyDatap[i].Open  *= 10;
				AnalyDatap[i].Close *= 10;
				AnalyDatap[i].High  *= 10;
				AnalyDatap[i].Low   *= 10;
				AnalyDatap[i].YClose = 0.0;
			}
		}
	}
	////
	return nReadDataNum;
}

short StockDataIo::ReadAnalyDataAndCurrHQ ( struct MemStkInfo * scode,
					struct AnalyData * AnalyDatap, short ItemNum ,short Type,struct CurrStockData * pTmp,int & nTodayDataNum,short mulnum,short dataflag)
{
	short  setcode;
	short  nReadDataNum=0;

	if ( scode == NULL) return 0;
	setcode = GetDomain ( scode );

	int itmp = GetStockType(scode->setcode,scode->Code);
	BOOL bSpec = UseSpecVolumeUnit(itmp,Type);
	BOOL bQZ = (itmp==CODE_SHQZ || itmp==CODE_SZQZ);

	ReadCurrStockData(scode,pTmp);
	BOOL bMakeNew=FALSE;
	switch (Type)
	{
	case PER_WEEK:
	case PER_MONTH:
	case PER_SEASON:
	case PER_YEAR:
		nTodayDataNum = 0;
		nReadDataNum = ReadVipDocData(scode, AnalyDatap, ItemNum, Type,mulnum,dataflag);
		if(g_bYesterdayFlag) return nReadDataNum;
		if( nReadDataNum > 0 && AnalyDatap[nReadDataNum-1].Time.Date >= g_iToday ) return nReadDataNum;
		if( nReadDataNum == 0 )
			bMakeNew=TRUE;
		else if(Type==PER_WEEK && (nReadDataNum > 0 && GetFriday(g_iToday)!=GetFriday(AnalyDatap[nReadDataNum-1].Time.Date)))
			bMakeNew=TRUE;
		else if(Type==PER_MONTH && (nReadDataNum > 0 && GetMonth1st(g_iToday)!=GetMonth1st(AnalyDatap[nReadDataNum-1].Time.Date)))
			bMakeNew=TRUE;
		else if(Type==PER_SEASON && (nReadDataNum > 0 && GetSeason1st(g_iToday)!=GetSeason1st(AnalyDatap[nReadDataNum-1].Time.Date)))
			bMakeNew=TRUE;
		else if(Type==PER_YEAR && (nReadDataNum > 0 && GetYear1st(g_iToday)!=GetYear1st(AnalyDatap[nReadDataNum-1].Time.Date)))
			bMakeNew=TRUE;
		if(bMakeNew) //如果不是同一周或同一月,则生成一新线 Modify 2004.1.30
		{
			if ( nReadDataNum > 0 && nReadDataNum+1 > ItemNum)
			{
				memmove(AnalyDatap,AnalyDatap+1,sizeof(struct AnalyData)*(nReadDataNum-1));
				nReadDataNum-=1;
			}
			memset(&AnalyDatap[nReadDataNum],0,sizeof(struct AnalyData));
			if( pTmp->Now < COMPPREC || pTmp->Open < COMPPREC || pTmp->Max < COMPPREC || pTmp->Min < COMPPREC)
			{
				AnalyDatap[nReadDataNum].Open   = pTmp->Close;
				AnalyDatap[nReadDataNum].High   = pTmp->Close;
				AnalyDatap[nReadDataNum].Low    = pTmp->Close;
				AnalyDatap[nReadDataNum].Close  = pTmp->Close;
			}
			else
			{
				AnalyDatap[nReadDataNum].Open   = pTmp->Open;
				AnalyDatap[nReadDataNum].High   = pTmp->Max ;
				AnalyDatap[nReadDataNum].Low    = pTmp->Min ;
				AnalyDatap[nReadDataNum].Close  = pTmp->Now ;
			}
			if (bSpec)
			{
				if(bQZ)
					AnalyDatap[nReadDataNum].Volume = pTmp->Volume/100;
				else
					AnalyDatap[nReadDataNum].Volume = scode->Unit*pTmp->Volume/100;
			}
			else
			{
				if( !testzs(scode) && need_justcjl(setcode,scode->Code))
					AnalyDatap[nReadDataNum].Volume = scode->Unit*pTmp->Volume;
				else
					AnalyDatap[nReadDataNum].Volume = pTmp->Volume;
			}
			AnalyDatap[nReadDataNum].Amount = scode->AmoUnit*pTmp->Amount;
			if( testzs(scode) )
			{
				AnalyDatap[nReadDataNum].zd.up   = pTmp->Buyv[0];
				AnalyDatap[nReadDataNum].zd.down = pTmp->Sellv[0];
			}
			else
				AnalyDatap[nReadDataNum].YClose = 0.0;
			nReadDataNum+=1;		
		}
		else if(nReadDataNum>0)
		{
			AnalyDatap[nReadDataNum-1].High   = max(AnalyDatap[nReadDataNum-1].High,pTmp->Max);
			if(pTmp->Min>COMPPREC)
				AnalyDatap[nReadDataNum-1].Low    = min(AnalyDatap[nReadDataNum-1].Low,pTmp->Min);
			if(pTmp->Now>COMPPREC)
				AnalyDatap[nReadDataNum-1].Close  = pTmp->Now;
			if (bSpec)
			{
				if(bQZ)
					AnalyDatap[nReadDataNum-1].Volume += pTmp->Volume/100;
				else
					AnalyDatap[nReadDataNum-1].Volume += scode->Unit*pTmp->Volume/100;
			}
			else
			{
				if(!testzs(scode) &&  need_justcjl(setcode,scode->Code))
					AnalyDatap[nReadDataNum-1].Volume += scode->Unit*pTmp->Volume;
				else
					AnalyDatap[nReadDataNum-1].Volume += pTmp->Volume;
			}
			AnalyDatap[nReadDataNum-1].Amount += scode->AmoUnit*pTmp->Amount;			
		}
		if(nReadDataNum>0)		
			AnalyDatap[nReadDataNum-1].Time.Date = g_iToday;
		return nReadDataNum;
	case PER_DAYN:
		nTodayDataNum = 0;
        nReadDataNum = ReadVipDocData(scode, AnalyDatap, ItemNum, Type,mulnum,dataflag);
		if(g_bYesterdayFlag) return nReadDataNum;
		if( nReadDataNum > 0 && AnalyDatap[nReadDataNum-1].Time.Date >= g_iToday ) return nReadDataNum;
		if(nReadDataNum>0)
		{
			AnalyDatap[nReadDataNum-1].High   = max(AnalyDatap[nReadDataNum-1].High,pTmp->Max);
			if(pTmp->Min>COMPPREC)
				AnalyDatap[nReadDataNum-1].Low    = min(AnalyDatap[nReadDataNum-1].Low,pTmp->Min);
			if(pTmp->Now>COMPPREC)
				AnalyDatap[nReadDataNum-1].Close  = pTmp->Now;
			if (bSpec)
			{
				if(bQZ)
					AnalyDatap[nReadDataNum-1].Volume += pTmp->Volume/100;
				else
					AnalyDatap[nReadDataNum-1].Volume += scode->Unit*pTmp->Volume/100;
			}
			else
			{
				if(!testzs(scode) &&  need_justcjl(setcode,scode->Code))
					AnalyDatap[nReadDataNum-1].Volume += scode->Unit*pTmp->Volume;
				else
					AnalyDatap[nReadDataNum-1].Volume += pTmp->Volume;
			}
			AnalyDatap[nReadDataNum-1].Amount += scode->AmoUnit*pTmp->Amount;	
			AnalyDatap[nReadDataNum-1].Time.Date = g_iToday;
		}
		return nReadDataNum;
	case PER_DAY:
		nTodayDataNum = 0;
        nReadDataNum=ReadVipDocData(scode, AnalyDatap, ItemNum, Type,mulnum,dataflag);
        if (g_bYesterdayFlag) break;	//如果是昨天的，则不动态维护
 	    if ( nReadDataNum > 0 && AnalyDatap[nReadDataNum-1].Time.Date >= g_iToday ) break;
		if ( nReadDataNum > 0 && nReadDataNum+1 > ItemNum)
		{
			memmove(AnalyDatap,AnalyDatap+1,sizeof(struct AnalyData)*(nReadDataNum-1));
			nReadDataNum-=1;
		}
		memset(&AnalyDatap[nReadDataNum],0,sizeof(struct AnalyData));
		if( pTmp->Now < COMPPREC || pTmp->Open < COMPPREC || pTmp->Max < COMPPREC || pTmp->Min < COMPPREC)
		{
			AnalyDatap[nReadDataNum].Open   = pTmp->Close;
			AnalyDatap[nReadDataNum].High   = pTmp->Close;
			AnalyDatap[nReadDataNum].Low    = pTmp->Close;
			AnalyDatap[nReadDataNum].Close  = pTmp->Close;
		}
		else
		{
			AnalyDatap[nReadDataNum].Open   = pTmp->Open;
			AnalyDatap[nReadDataNum].High   = pTmp->Max ;
			AnalyDatap[nReadDataNum].Low    = pTmp->Min ;
			AnalyDatap[nReadDataNum].Close  = pTmp->Now ;
		}
		//对最高和最低进行容错
		AnalyDatap[nReadDataNum].High = max(AnalyDatap[nReadDataNum].High,AnalyDatap[nReadDataNum].Close);
		AnalyDatap[nReadDataNum].Low  = min(AnalyDatap[nReadDataNum].Low,AnalyDatap[nReadDataNum].Close);

		if(!testzs(scode) &&  need_justcjl(setcode,scode->Code))
			AnalyDatap[nReadDataNum].Volume = scode->Unit*pTmp->Volume;
		else
			AnalyDatap[nReadDataNum].Volume = pTmp->Volume;
        AnalyDatap[nReadDataNum].Amount = scode->AmoUnit*pTmp->Amount;
		if( testzs(scode) )
	    {
			AnalyDatap[nReadDataNum].zd.up   = pTmp->Buyv[0];
			AnalyDatap[nReadDataNum].zd.down = pTmp->Sellv[0];
		}
		else
			AnalyDatap[nReadDataNum].YClose = 0.0;
	    AnalyDatap[nReadDataNum].Time.Date = g_iToday;
		nReadDataNum+=1;
	    break;
	case PER_MIN5:
	case PER_MIN15:
	case PER_MIN30:
	case PER_HOUR:
		nReadDataNum=ReadVipDocData(scode, AnalyDatap, ItemNum, Type,mulnum,dataflag);
		break;
	default:
		return 0;
   }
   return nReadDataNum;
}

/////////////////////////////////////////////////////////////////////////////
//					支持第三方市场的函数
/////////////////////////////////////////////////////////////////////////////

BOOL	StockDataIo::DSInit()
{
	int i=0;
	FILE *fp=NULL;

	//装入服务器信息
	memset(&g_iDsSrvInfo,0,sizeof(SRVINFOANS));
	fp=_fsopen(g_WSXHStr+"LData\\cache\\ds_code.dat","rb",_SH_DENYNO);
	if(fp==NULL) return FALSE;
	fread(&g_iDsSrvInfo,sizeof(SRVINFOANS),1,fp);
	fclose(fp);

	//得到过滤后的的市场信息
	DSLoadMarketInfo();
	m_nDSNum = g_iDsSrvInfo.IndexNum;
	TDEL(m_aICList);
	TDEL(m_aICListEx);

	//装入时间信息
	if(g_bDSNeedTimer) memset(m_aTimerSet, 0, sizeof(m_aTimerSet));
	short nTimerMap[MAX_DS_TIMERNUM]={0};
	if(!DSLoadTimer(g_iDsSrvInfo.TimerNum,m_aTimerSet,nTimerMap))
		return FALSE;
	//读入品种信息
	g_bLongDSName = FALSE;
	fp=_fsopen(g_WSXHStr+"LData\\cache\\ds_code.dat","rb",_SH_DENYNO);
	if(fp)
	{
		//读出过滤后股票信息
		MemStkInfo tmpStkInfo={0};
		int tmpStkNum=0;
		fseek(fp,sizeof(SRVINFOANS),SEEK_SET);
		for(i=0;i < m_nDSNum;i++)
		{
			memset(&tmpStkInfo,0,sizeof(MemStkInfo));
			//读出品种信息
			STKINFO tmpInfo = {0};
			fread(&tmpInfo,sizeof(STKINFO),1,fp);
			tmpInfo.Code[CODE_SIZE-1]=0;
			tmpInfo.Name[NAME_SIZE-1]=0;
			tmpInfo.SpellCode[SPELLCODE_SIZE-1]=0;
			if(tmpInfo.setcode!=47&&tmpInfo.setcode!=28&&tmpInfo.setcode!=29&&tmpInfo.setcode!=30&&tmpInfo.setcode!=50) continue;
			for(int j=0;j<g_nDSMarketNum;j++)
			{
				if(tmpInfo.setcode==g_pDSMarket[j].MktType) 
					break;
			}
			if(j < g_nDSMarketNum)
			{
				tmpStkInfo.bigmarket=tmpInfo.BigMarket;
				tmpStkInfo.DocNum=tmpInfo.DocNum;
				if(g_pDSMarket[j].DocNum>0) 
					tmpStkInfo.DocNum=g_pDSMarket[j].DocNum;
			 	tmpStkInfo.setcode = g_pDSMarket[j].MktType;
				strcpy(tmpStkInfo.Code,tmpInfo.Code);
				strcpy(tmpStkInfo.Name,tmpInfo.Name);
				if(tmpStkInfo.Name[0]==0)
					strcpy(tmpStkInfo.Name,tmpStkInfo.Code);
				if(strlen(tmpStkInfo.Name)>8)
					g_bLongDSName = TRUE;
				strcpy(tmpInfo.SpellCode,GetPYStr(tmpInfo.Name,5));
				strcpy(tmpStkInfo.SpellCode,tmpInfo.SpellCode);
				_FL(tmpStkInfo.Name);								//得到拼音以后,再转成繁体
				tmpStkInfo.TimerID = nTimerMap[tmpInfo.TimerID];
				tmpStkInfo.Unit = tmpInfo.Unit;
				tmpStkInfo.AmoUnit = tmpInfo.AmoUnit;
					
				//建索引
				MemStkInfo* pStock = GetDSStkInfo(tmpStkInfo.setcode, tmpStkInfo.Code);
				if(!pStock)
				{
 					CODEMAPINFO codeInfo;
					memset(&codeInfo, 0, sizeof(CODEMAPINFO));
					codeInfo.Market = tmpStkInfo.setcode;
					strcpy(codeInfo.Code, tmpStkInfo.Code);
					tmpStkInfo.Index = DS_FROM + m_aDSStkInfo.size();
					m_code2IndexMap[codeInfo] = m_aDSStkInfo.size();
					m_aDSStkInfo.push_back(tmpStkInfo);

					if(m_aDSStkInfo.size()>=m_aDSStkInfo.capacity()-1)	//保证内存不会重新分配
						break;
				}
				else pStock->TimerID = tmpStkInfo.TimerID;
			}
		}
		fclose(fp);
		m_nDSNum = m_aDSStkInfo.size();
		//得到行情缓冲区
		m_aICListEx = new EXCURRDATA[m_nDSNum];
		memset(m_aICListEx,0,sizeof(EXCURRDATA)*m_nDSNum);
		m_aICList = new CURRDATA[m_nDSNum];
		memset(m_aICList,0,sizeof(CURRDATA)*m_nDSNum);
		for(long j=0;j<m_nDSNum;j++)
			m_aICList[j].Reserved = (DWORD)(&m_aICListEx[j]);
		return TRUE;
	}
    return FALSE;
}

void StockDataIo::AfterDSLoginSuccess()
{	
	if(g_GC.bIsSSDS)
	{
		if(g_GC.bOnlyHK)
			GetDSMarketCfg(HomePath+"hkmarket.dat");
		else
			GetDSMarketCfg(HomePath+"l2market.dat");
	}
	else
		GetDSMarketCfg(HomePath+"dsmarket.dat");		
	//登录
	if(g_mSeat2Com.size()<1)
		LoadSeatData();
	if(!g_pStockIO->DSInit())
	{
		DeleteFile(g_WSXHStr+"LData\\cache\\ds_code.dat");
		DeleteFile(g_WSXHStr+"LData\\cache\\ds_time.dat");
		DeleteFile(g_WSXHStr+"LData\\cache\\dshq.dat");
		DeleteFile(g_WSXHStr+"LData\\cache\\dshqex.dat");
	}
	//先得到状态栏
	if(g_pMainWin)
		g_pMainWin->SendMessage(UM_STATUS_REQ,0); 
	//需要重新装载板块股设置
	LoadColorfulBlockGP();
}

MemStkInfo*	StockDataIo::GetDSStkInfo(long nIDIndex)
{
	if(nIDIndex>=DS_FROM)	
		nIDIndex -= DS_FROM;
	return((nIDIndex>=0&&nIDIndex<m_nDSNum)?&m_aDSStkInfo[nIDIndex]:NULL);
}

MemStkInfo*	StockDataIo::GetDSStkInfo(BYTE Market,const char Code[CODE_SIZE])
{
	CODEMAPINFO codeInfo; 
	memset(&codeInfo, 0, sizeof(CODEMAPINFO));
	codeInfo.Market = Market; 
	strncpy(codeInfo.Code, Code,CODE_SIZE-1);
	codeInfo.Code[CODE_SIZE-1]=0;
	map<CODEMAPINFO, long, CodeStrCmp>::iterator iter = m_code2IndexMap.find(codeInfo);
	if(iter == m_code2IndexMap.end()) return NULL;
	long i = (*iter).second;
	return(i<m_aDSStkInfo.size()?&m_aDSStkInfo[i]:NULL);
}

BOOL	StockDataIo::BeDSStock(MemStkInfo *pStkInfo)
{
	if(!g_bHasDS||!pStkInfo) return FALSE;
	int  setcode = GetDomain ( pStkInfo );
	return setcode>1 && setcode<MAX_MKT_NUM && pStkInfo->Index>=DS_FROM && pStkInfo->Index<DS_FROM+m_nDSNum;
}

BOOL	StockDataIo::BeDSStock(BYTE Market,const char Code[CODE_SIZE])
{
	if(!g_bHasDS) return FALSE;
	MemStkInfo *pStkInfo = GetDSStkInfo(Market, Code);
	if(!pStkInfo) return FALSE;
	int  setcode = GetDomain ( pStkInfo );
	return setcode>1 && setcode<MAX_MKT_NUM && pStkInfo->Index>=DS_FROM && pStkInfo->Index<DS_FROM+m_nDSNum;	
}

BOOL	StockDataIo::BeDSStock(long nIDIndex)
{
	if(!g_bHasDS) return FALSE;
	MemStkInfo *pStkInfo = (*this)[nIDIndex];
	if(!pStkInfo) return FALSE;
	int  setcode = GetDomain ( pStkInfo );
	return setcode>1 && setcode<MAX_MKT_NUM && pStkInfo->Index>=DS_FROM && pStkInfo->Index<DS_FROM+m_nDSNum;
}

BOOL	StockDataIo::DSSetCurrData(MemStkInfo *pStkInfo, PCURRDATA pData,PEXCURRDATA pDataEx)
{
	if(!pStkInfo) return FALSE;
	long nXH = pStkInfo->Index-DS_FROM;
	pStkInfo->Close=pData->Close; //首先将MemStkInfo中的Close赋值
	if(pData)
	{
		memcpy(&m_aICList[nXH],pData,sizeof(CURRDATA));
		m_aICList[nXH].Reserved = (DWORD)(PEXCURRDATA)(&m_aICListEx[nXH]);
	}
	if(pDataEx)
		memcpy(&m_aICListEx[nXH],pDataEx,sizeof(EXCURRDATA));
	return TRUE;
}

void	StockDataIo::DSGetCurrData(MemStkInfo *pStkInfo,PCURRDATA pData,PEXCURRDATA pDataEx)
{
	long nXH = pStkInfo->Index-DS_FROM;
	if(pData)
		memcpy(pData,&m_aICList[nXH],sizeof(CURRDATA));
	if(pDataEx)
		memcpy(pDataEx,&m_aICListEx[nXH],sizeof(EXCURRDATA));
}

short	StockDataIo::DSFiltCode(int nWhichXh,short*pIdx,short IdxSize)
{
	short j=0;
	if(nWhichXh>=0 && nWhichXh<g_nDSMarketNum) 
	{
		for(int i=0;i < m_nDSNum && j<IdxSize;i++)
		{
			if(m_aDSStkInfo[i].setcode==g_pDSMarket[nWhichXh].MktType)
				pIdx[j++]=m_aDSStkInfo[i].Index;
		}
	}
	return j;
}

PTIMEINF	StockDataIo::DSGetTimeInf(short TimeID)
{
	return(&m_aTimerSet[TimeID]);
}

void	StockDataIo::SaveDSCurrData()
{
	if(m_aICList && m_aICListEx && m_nDSNum>0)
	{
		FILE *fp=_fsopen(g_WSXHStr+"LData\\cache\\dshq.dat","wb",_SH_DENYNO);
		if(fp)
		{
			fwrite(m_aICList,sizeof(CURRDATA),m_nDSNum,fp);
			fclose(fp);
			fp=_fsopen(g_WSXHStr+"LData\\cache\\dshqex.dat","wb",_SH_DENYNO);
			if(fp)
			{
				fwrite(m_aICListEx,sizeof(EXCURRDATA),m_nDSNum,fp);
				fclose(fp);
			}
		}	
	}
}

void	StockDataIo::LoadDSCurrData()
{
	if(m_aICList && m_aICListEx && m_nDSNum>0)
	{
		FILE *fp=_fsopen(g_WSXHStr+"LData\\cache\\dshq.dat","rb",_SH_DENYNO);
		if(fp)
		{
			fread(m_aICList,sizeof(CURRDATA),m_nDSNum,fp);
			fclose(fp);
			fp=_fsopen(g_WSXHStr+"LData\\cache\\dshqex.dat","rb",_SH_DENYNO);
			if(fp)
			{
				fread(m_aICListEx,sizeof(EXCURRDATA),m_nDSNum,fp);
				fclose(fp);
			}
		}	
	}
}

//对于二代转码机,前提是有价格,如果量有变化,则生成Tick,对于只有现价变化而生成Tick的只有上海股票和三板
BOOL StockDataIo::CanMakeOneTick(MemStkInfo *pStkInfo,CurrStockData & Hq,BOOL bDs,BOOL bZs,float fLastPrice,long lLastVolume)
{
	if(!pStkInfo) 
		return FALSE;
	if(g_bG2DT)
	{
		if(Hq.Now<COMPPREC)
			return FALSE;
		if( (bZs && (long)(Hq.Amount/10000)>lLastVolume) ||
			(!bZs && Hq.Volume>lLastVolume) ||
			((pStkInfo->setcode==SH || (pStkInfo->setcode==SZ && pStkInfo->Code[0]=='4')) && fabs(Hq.Now-fLastPrice)>COMPPREC )
		)
			return TRUE;
		return FALSE;		
	}
	if( ( bZs && ( fabs(Hq.Now-fLastPrice)>=COMPPREC || (long)(Hq.Amount/10000)>lLastVolume ) )
		||(!bZs && (Hq.Volume>lLastVolume || (!bDs && fabs(Hq.Now-fLastPrice)>=COMPPREC && Hq.Volume>=lLastVolume && lLastVolume>0 && fLastPrice>COMPPREC)))
		)
		return TRUE;
	return FALSE;
}