#include "stdafx.h"
#include <share.h>
#include "cwcl.h"

BOOL IsTdxChar(char tmpChar)
{
	if(tmpChar<0 || tmpChar>255) return FALSE;
	return isalnum(tmpChar);
}

Cwcl::Cwcl()
{
	All_CwData    = NULL;
	All_DataNum   = 0;
	m_CWBB = NULL;
	FileName[0] = '\0';
	m_CWBB = new DBH_Head;
	Fish = new(CBlowfish);

	ALL_RLTData = NULL; RLTSetInfo = NULL;
	ALL_FaceData = NULL;
	RLTDataSet = 0;
	All_RLTDataNum = ALL_FaceNum = 0;
}

Cwcl::~Cwcl()
{   
	TDEL(m_CWBB);
	TDEL(Fish);
	TDEL(All_CwData);
	ResetRLTData();
	ResetFaceData();
}

void Cwcl::Set_Password(BYTE * Password,short len)
{   
	Fish->InitializeBlowfish(Password,len);
}

unsigned long Cwcl::Get_RecordNum()
{
	unsigned long m_nItemNum;
	short nTag = OpenBase(m_CWBB,FileName,O_BINARY|O_RDONLY);
	if(nTag!=FALSE)
	{
		TDEL(m_CWBB);
		TDX_MessageBox(NULL,"不能打开股本变迁文件!",MB_OK|MB_ICONEXCLAMATION);
		return 0;
	}
	GetDbfRecNum   (m_CWBB);
	m_nItemNum     = m_CWBB -> RecNum;
	CloseBase(m_CWBB);
	return m_nItemNum;
}

void Cwcl::CWData_Encrypt(CWDATA *CWdata_obj,short len)
{	
	Fish->Encrypt((BYTE *)CWdata_obj, len);
}

void Cwcl::CWdata_Decrypt(CWDATA *CWdata_obj,short len)
{	
	Fish->Decrypt((BYTE *)CWdata_obj,len);
}

void Cwcl::ReadARecord(short RecordNum,CWDATA *CWdata_obj)
{
	char Code[7];
	char Date[9];
	char Type[2];
	char data[4][10];
	short nTag = OpenBase(m_CWBB,FileName,O_BINARY|O_RDONLY);
	if(nTag!=FALSE)
	{
		TDX_MessageBox(NULL,"不能打开股本变迁文件!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	ReadDbf(m_CWBB,RecordNum);
	GetFieldStr(m_CWBB,"Gpdm",Code);
	GetFieldStr(m_CWBB,"Date",Date);
	GetFieldStr(m_CWBB,"Type",Type);
	GetFieldStr(m_CWBB,"B01",data[0]);
	GetFieldStr(m_CWBB,"B02",data[1]);
	GetFieldStr(m_CWBB,"B03",data[2]);
	GetFieldStr(m_CWBB,"B04",data[3]);

	strcpy (CWdata_obj->Code,Code);
	CWdata_obj->Date = (long)atof(Date);
	CWdata_obj->Type = (char)atoi(Type);
	CWdata_obj->B01  = atof(data[0]);
	CWdata_obj->B02  = atof(data[1]);
	CWdata_obj->B03  = atof(data[2]);
	CWdata_obj->B04  = atof(data[3]);
	CloseBase(m_CWBB);
}

unsigned long Cwcl::Get_RecordNum(char *File)
{	
	unsigned long RecordNum;
	FILE * Pin = fopen(File,"rb");
	if (Pin == NULL)	return 0;
	fread(&RecordNum, sizeof (unsigned long),1, Pin);
	fclose(Pin);
	return ( RecordNum );
}

short Cwcl::Init_ZGB_LTGB()
{   
	if (All_DataNum != 0)		return (All_DataNum);
	unsigned long Record_Num;
	FILE *fp_in;
	fp_in = _fsopen(g_WSXHStr+"LData\\cache\\gbbq","rb",_SH_DENYNO);
	if (fp_in == NULL)  return -1;

	//判断股本变迁库的版本,根据文件长度
	long filelen = on_filelength(fp_in);
	fread(&Record_Num,sizeof(unsigned long),1,fp_in);
	if(filelen-sizeof(unsigned long) != Record_Num*sizeof(CWDATA))
		return -1;
	All_DataNum = Record_Num;
	TDEL(All_CwData);
	All_CwData = new CWDATA[Record_Num];
	if (All_CwData == NULL)	{ fclose(fp_in);TDX_MessageBox(NULL,"内存申请错",MB_OK);return -1; }

	int i;
	char *Password = "770303770222";
	CBlowfish  *p_fish;
	p_fish = new(CBlowfish);
	p_fish->InitializeBlowfish((BYTE *)Password,12);

	fread(All_CwData,sizeof(CWDATA)*Record_Num ,1, fp_in);
	for (i = 0;i<Record_Num;i++)
	{
		p_fish->Decrypt((BYTE *)(&All_CwData[i]),24);
		All_CwData[i].Code[6]=0;
		//判断是不是B股的分红,仅在客户端做此转换
		if(All_CwData[i].Type==CWCL_TYPE_DR && All_CwData[i].B01>COMPPREC)
		{
			int nGpKind = GetStockType(All_CwData[i].Setcode,All_CwData[i].Code);
			switch(nGpKind)
			{
			case CODE_SZBG:
				All_CwData[i].B01=All_CwData[i].B01/g_gy_rate;
				break;
			case CODE_SHBG:
				All_CwData[i].B01=All_CwData[i].B01/g_my_rate;
				break;
			}				
		}
	}

	fclose(fp_in);
	delete p_fish;
	return (Record_Num);
}

CWDATA * Cwcl::GetGpCwDataStart(char *GpCode,short setcode,short &DataNum) //找到某股票的财务缓冲区入口
{
	if(Init_ZGB_LTGB()==-1) return NULL;
	CWDATA *pStart = NULL;
	long start = 0;
	DataNum = 0;
	for(int i=0; i<All_DataNum; i++)
	{
		if(CompareCwcl(All_CwData+i,GpCode,setcode))
		{
			if(!pStart)
			{
				pStart = All_CwData+i;
				start = i;
			}
		}
		else if(pStart)
		{
			DataNum = i-start;
			break;
		}
	}
	if(pStart && i >= All_DataNum)
		DataNum = i-start;
	if(DataNum > GP_MAXGBBQNUM) //如果数目超过GP_MAXGBBQNUM,则取最新的GP_MAXGBBQNUM个
	{
		pStart+=DataNum-GP_MAXGBBQNUM;
		DataNum = GP_MAXGBBQNUM;
	}
	return pStart;
}

void Cwcl::GetGBBQ_Type(struct MemStkInfo * scode,struct AnalyData * AnalyDatap, char * fpBQtype,short ItemNum)
{
	if(Init_ZGB_LTGB()==-1) return;
	int   nFoundNum=0,nAllocNum=0,i,j;
	CWDATA *pCWData = NULL;
	//提取财务数据
	for(i=0; i<All_DataNum; i++)
	{   
		if(All_CwData[i].Date<10)   continue;
		if(CompareCwcl(All_CwData+i,scode->Code,scode->setcode))
		{   
			if(nFoundNum>=nAllocNum)
			{	
				nAllocNum+=20;
				pCWData = (CWDATA *)realloc(pCWData,nAllocNum*sizeof(CWDATA));
			}
			pCWData[nFoundNum]=All_CwData[i];
			nFoundNum++;
		}
	}
	for(i=0; i<ItemNum; i++) fpBQtype[i]=0;
	i=j=0;
	while(j<nFoundNum&&i<ItemNum)
	{   
		if(pCWData[j].Date==AnalyDatap[i].Time.Date)
		{	
			fpBQtype[i] = pCWData[j].Type;
			i++;
			j++;
		}
		else if(pCWData[j].Date>AnalyDatap[i].Time.Date)	i++;
		else	j++;
	}
	free(pCWData);
}

void Cwcl::MinTQData(struct MemStkInfo * scode,struct AnalyData * AnalyDatap,short nFrom,short nEnd,short nDataNum,short nTQFlag,short nYearFlag)
{	
	if(Init_ZGB_LTGB()==-1) return;

	int   nFoundNum=0,nAllocNum=0,i,j,m;
	CWDATA *pCWData=NULL;
	CTime t = CTime::GetCurrentTime();
	long lEndDate = 10000*t.GetYear()+100*t.GetMonth()+t.GetDay();
	float rate=1.0;
	//提取财务数据(价格复权)
	for(i=0,nFoundNum=0; i<All_DataNum; i++)
	{   if(All_CwData[i].Date<10)   continue;
		if(CompareCwcl(All_CwData+i,scode->Code,scode->setcode) && (All_CwData[i].Type==CWCL_TYPE_DR || All_CwData[i].Type==CWCL_TYPE_INPROCHG))
		{   
			if(lEndDate-All_CwData[i].Date>1130+nYearFlag*10000||lEndDate-All_CwData[i].Date<1130+(nYearFlag-1)*10000) continue;
			if(nFoundNum>=nAllocNum)
			{	nAllocNum+=20;
				pCWData = (CWDATA *)realloc(pCWData,nAllocNum*sizeof(CWDATA));
				if(!pCWData) return;
			}	
			pCWData[nFoundNum]=All_CwData[i];
			nFoundNum++;
		}
	}
	//初始化复权系数数组
	switch(nTQFlag)
	{
	case 1://前复权(变更填权缺口前的数据)
	case 3:
		for(i=0; i<nFoundNum; i++)
		{   //查找除权日
			if(g_nTQFlag==3)
			{
				if(pCWData[i].Date>g_nTqStart) continue;
			}
			for(j=nFrom; j<=nEnd; j++) 
			{
				if(t.GetYear()-nYearFlag!=pCWData[i].Date/10000) continue;
				if(j>0&&AnalyDatap[j-1].Time.Daye.Day<pCWData[i].Date%10000&&AnalyDatap[j].Time.Daye.Day>pCWData[i].Date%10000) break;
				if(AnalyDatap[j].Time.Daye.Day==pCWData[i].Date%10000) break;
			}
			//无法找到相应的除权除息日
			if(j>nEnd) continue;
			if(j!=0) rate = AnalyDatap[j-1].Close;
			else rate=1;
			//复权价=(原价-红利+配股金额)/(原股本+送股+配股)
			for(m=0; m<j; m++)
			{
				if(pCWData[i].Type==CWCL_TYPE_DR)
				{
					AnalyDatap[m].Open   = (10.0*AnalyDatap[m].Open  -pCWData[i].B01+pCWData[i].B04*pCWData[i].B02)/(10.0+pCWData[i].B03+pCWData[i].B04);
					AnalyDatap[m].High   = (10.0*AnalyDatap[m].High  -pCWData[i].B01+pCWData[i].B04*pCWData[i].B02)/(10.0+pCWData[i].B03+pCWData[i].B04);
					AnalyDatap[m].Low    = (10.0*AnalyDatap[m].Low   -pCWData[i].B01+pCWData[i].B04*pCWData[i].B02)/(10.0+pCWData[i].B03+pCWData[i].B04);
					AnalyDatap[m].Close  = (10.0*AnalyDatap[m].Close -pCWData[i].B01+pCWData[i].B04*pCWData[i].B02)/(10.0+pCWData[i].B03+pCWData[i].B04);
				}
				else if(pCWData[i].B03>1)
				{
					AnalyDatap[m].Open   = AnalyDatap[m].Open/pCWData[i].B03;
					AnalyDatap[m].High   = AnalyDatap[m].High/pCWData[i].B03;
					AnalyDatap[m].Low    = AnalyDatap[m].Low/pCWData[i].B03;
					AnalyDatap[m].Close  = AnalyDatap[m].Close/pCWData[i].B03;
				}
				if(g_bVolTQ)
				{
					AnalyDatap[m].Amount = AnalyDatap[m].Amount+pCWData[i].B02*pCWData[i].B04/10*AnalyDatap[m].Volume-AnalyDatap[m].Volume*pCWData[i].B01/10;
					AnalyDatap[m].Volume = AnalyDatap[m].Volume*(10+pCWData[i].B03+pCWData[i].B04)/10;
				}
			}
		}
		break;
	case 2://后复权(变更填权缺口后的数据)
	case 4:
		for(i=nFoundNum-1; i>=0; i--)
		{   //查找除权日
			if(g_nTQFlag==3)
			{
				if(pCWData[i].Date<g_nTqStart) continue;
			}
			for(j=nFrom; j<=nEnd; j++)
			{
				if(t.GetYear()-nYearFlag!=pCWData[i].Date/10000) continue;
				if(j>0&&AnalyDatap[j-1].Time.Daye.Day<pCWData[i].Date%10000&&AnalyDatap[j].Time.Daye.Day>pCWData[i].Date%10000) break;
				if(AnalyDatap[j].Time.Daye.Day==pCWData[i].Date%10000) break;
			}
			//无法找到相应的除权除息日
			if(j>nEnd) //j>=nEnd -> j>nEnd
				continue;
			//复权价=(原价-红利+配股金额)/(原股本+送股+配股)
			for(m=j; m<=nDataNum-1; m++)
			{	
				if(pCWData[i].Type==CWCL_TYPE_DR)
				{
					AnalyDatap[m].Open   = (AnalyDatap[m].Open  *(10.0+pCWData[i].B03+pCWData[i].B04)+pCWData[i].B01-pCWData[i].B04*pCWData[i].B02)/10.0;
					AnalyDatap[m].High   = (AnalyDatap[m].High  *(10.0+pCWData[i].B03+pCWData[i].B04)+pCWData[i].B01-pCWData[i].B04*pCWData[i].B02)/10.0;
					AnalyDatap[m].Low    = (AnalyDatap[m].Low   *(10.0+pCWData[i].B03+pCWData[i].B04)+pCWData[i].B01-pCWData[i].B04*pCWData[i].B02)/10.0;
					AnalyDatap[m].Close  = (AnalyDatap[m].Close *(10.0+pCWData[i].B03+pCWData[i].B04)+pCWData[i].B01-pCWData[i].B04*pCWData[i].B02)/10.0;
				}
				else if(pCWData[i].B03>1)
				{
					AnalyDatap[m].Open   = AnalyDatap[m].Open  *pCWData[i].B03;
					AnalyDatap[m].High   = AnalyDatap[m].High  *pCWData[i].B03;
					AnalyDatap[m].Low    = AnalyDatap[m].Low   *pCWData[i].B03;
					AnalyDatap[m].Close  = AnalyDatap[m].Close *pCWData[i].B03;
				}
				if(g_bVolTQ)
				{
					AnalyDatap[m].Volume = AnalyDatap[m].Volume*10.0/(10+pCWData[i].B03+pCWData[i].B04);
					AnalyDatap[m].Amount = AnalyDatap[m].Amount-pCWData[i].B02*pCWData[i].B04/10*AnalyDatap[m].Volume+AnalyDatap[m].Volume*pCWData[i].B01/10;
				}
			}
		}
		break;
	}
	free(pCWData);
}

void Cwcl::TQData(struct MemStkInfo * scode,struct AnalyData * AnalyDatap,short nFrom,short nEnd,short nDataNum,short nTQFlag,short nPeriod)
{
	if(Init_ZGB_LTGB()==-1) return;
	int   nFoundNum=0,nAllocNum=0,i,j,m;
	CWDATA *pCWData=NULL;
	//提取财务数据(价格复权)
	for(i=0,nFoundNum=0; i<All_DataNum; i++)
	{   
		if(All_CwData[i].Date<10)   continue;
		if(CompareCwcl(All_CwData+i,scode->Code,scode->setcode) && (All_CwData[i].Type==CWCL_TYPE_DR || All_CwData[i].Type==CWCL_TYPE_INPROCHG))
		{   
			if(nFoundNum>=nAllocNum)
			{	nAllocNum+=20;
				pCWData = (CWDATA *)realloc(pCWData,nAllocNum*sizeof(CWDATA));
				if(!pCWData) return;
			}
			pCWData[nFoundNum]=All_CwData[i];
			nFoundNum++;
		}
	}
	//初始化复权系数数组
	switch(nTQFlag)
	{
	case 1://前复权(变更填权缺口前的数据)
	case 3:
		for(i=0; i<nFoundNum; i++)
		{   //查找除权日			
			if(g_nTQFlag==3)
			{
				if(pCWData[i].Date>g_nTqStart) continue;
			}
			//
			for(j=nFrom; j<=nEnd; j++) 
				if((j>0&&AnalyDatap[j-1].Time.Date<pCWData[i].Date&&AnalyDatap[j].Time.Date>=pCWData[i].Date)||
					(j==0&&AnalyDatap[j].Time.Date==pCWData[i].Date)) 
					break;
			//无法找到相应的除权除息日
			if(j>nEnd) continue;
			//复权价=(原价-红利+配股金额)/(原股本+送股+配股)
			//B01:红利 B04:配股 B02:配股价 B03:送股
			for(m=0; m<j; m++)
			{
				
				float fOHigh = AnalyDatap[m].High, fOLow = AnalyDatap[m].Low;
				if(pCWData[i].Type==CWCL_TYPE_DR)
				{
					AnalyDatap[m].Open   = (10.0*AnalyDatap[m].Open  -pCWData[i].B01+pCWData[i].B04*pCWData[i].B02)/(10.0+pCWData[i].B03+pCWData[i].B04);
					AnalyDatap[m].High   = (10.0*AnalyDatap[m].High  -pCWData[i].B01+pCWData[i].B04*pCWData[i].B02)/(10.0+pCWData[i].B03+pCWData[i].B04);
					AnalyDatap[m].Low    = (10.0*AnalyDatap[m].Low   -pCWData[i].B01+pCWData[i].B04*pCWData[i].B02)/(10.0+pCWData[i].B03+pCWData[i].B04);
					AnalyDatap[m].Close  = (10.0*AnalyDatap[m].Close -pCWData[i].B01+pCWData[i].B04*pCWData[i].B02)/(10.0+pCWData[i].B03+pCWData[i].B04);
				}
				else if(pCWData[i].B03>1)
				{
					AnalyDatap[m].Open   = AnalyDatap[m].Open/pCWData[i].B03;
					AnalyDatap[m].High   = AnalyDatap[m].High/pCWData[i].B03;
					AnalyDatap[m].Low    = AnalyDatap[m].Low/pCWData[i].B03;
					AnalyDatap[m].Close  = AnalyDatap[m].Close/pCWData[i].B03;
				}				
				if(g_bVolTQ) 
				{
					AnalyDatap[m].Amount = AnalyDatap[m].Amount+pCWData[i].B02*pCWData[i].B04/10*AnalyDatap[m].Volume-AnalyDatap[m].Volume*pCWData[i].B01/10;
					AnalyDatap[m].Volume = AnalyDatap[m].Volume*(10+pCWData[i].B03+pCWData[i].B04)/10;
				}				
			}
			if(nPeriod==PER_WEEK||nPeriod==PER_MONTH||nPeriod==PER_DAYN||nPeriod==PER_SEASON||nPeriod==PER_YEAR)		//大于日线的周期除权
			{
				AnalyData tmpAnalyData = AnalyDatap[j];
				float fOHigh = tmpAnalyData.High, fOLow = tmpAnalyData.Low;
				if(j==nEnd)
				{
				}
				else
				{
					AnalyData AfterTQData  = AnalyDatap[j+1];
					AnalyDatap[j].Open   = 0.382f*AnalyDatap[j-1].Open+0.618f*AfterTQData.Open;
					AnalyDatap[j].High   = max(0.382f*AnalyDatap[j-1].High+0.618f*AfterTQData.High,AnalyDatap[j].Close);
					AnalyDatap[j].Low    = min(0.382f*AnalyDatap[j-1].Low+0.618f*AfterTQData.Low,AnalyDatap[j].Close);
				}				
				//最低价和收盘价不改
				if(j==nEnd && j>0)
				{
					if(pCWData[i].Type==CWCL_TYPE_DR)
					{
						AnalyDatap[j].Open   = (10.0*AnalyDatap[j].Open-pCWData[i].B01+pCWData[i].B04*pCWData[i].B02)/(10.0+pCWData[i].B03+pCWData[i].B04);
						AnalyDatap[j].High   = (10.0*AnalyDatap[j].High-pCWData[i].B01+pCWData[i].B04*pCWData[i].B02)/(10.0+pCWData[i].B03+pCWData[i].B04);
					}
					else if(pCWData[i].B03>1)
					{
						AnalyDatap[j].Open   = AnalyDatap[j].Open/pCWData[i].B03;
						AnalyDatap[j].High   = AnalyDatap[j].High/pCWData[i].B03;
					}
					//最高价不会低于上月收盘价的90%
					if(AnalyDatap[j].High<AnalyDatap[j-1].Close*0.9)
					{
						AnalyDatap[j].High=tmpAnalyData.High;
					}
					if(AnalyDatap[j].High<AnalyDatap[j].Close)
					{
						AnalyDatap[j].High=2*AnalyDatap[j].Close-AnalyDatap[j].High;
					}
					//处理开盘价
					if(AnalyDatap[j].Open<AnalyDatap[j-1].Close*0.9)
					{
						AnalyDatap[j].Open=tmpAnalyData.Open;
						AnalyDatap[j].High=tmpAnalyData.High;
					}
					if(AnalyDatap[j].Open<AnalyDatap[j].Low)
					{
						AnalyDatap[j].Low=2*AnalyDatap[j].Open-AnalyDatap[j].Low;
					}
				}
			}
		}
		break;
	case 2://后复权(变更填权缺口后的数据)
	case 4:
		for(i=nFoundNum-1; i>=0; i--)
		{
			//除开日期早得离谱的
			if(pCWData[i].Date<AnalyDatap[0].Time.Date) continue;
			if(g_nTQFlag==3)
			{
				if(pCWData[i].Date<g_nTqStart) continue;
			}
			//查找除权日
			for(j=nFrom;j<=nEnd&&pCWData[i].Date>AnalyDatap[j].Time.Date;j++);
			//无法找到相应的除权除息日
			if(j>nEnd) //j>=nEnd -> j>nEnd
				continue;
			//复权价=(原价-红利+配股金额)/(原股本+送股+配股)
			for(m=j+1; m<=nDataNum-1; m++)
			{	
				float fOHigh = AnalyDatap[m].High, fOLow = AnalyDatap[m].Low;
				if(pCWData[i].Type==CWCL_TYPE_DR)
				{
					AnalyDatap[m].Open   = (AnalyDatap[m].Open  *(10.0+pCWData[i].B03+pCWData[i].B04)+pCWData[i].B01-pCWData[i].B04*pCWData[i].B02)/10.0;
					AnalyDatap[m].High   = (AnalyDatap[m].High  *(10.0+pCWData[i].B03+pCWData[i].B04)+pCWData[i].B01-pCWData[i].B04*pCWData[i].B02)/10.0;
					AnalyDatap[m].Low    = (AnalyDatap[m].Low   *(10.0+pCWData[i].B03+pCWData[i].B04)+pCWData[i].B01-pCWData[i].B04*pCWData[i].B02)/10.0;
					AnalyDatap[m].Close  = (AnalyDatap[m].Close *(10.0+pCWData[i].B03+pCWData[i].B04)+pCWData[i].B01-pCWData[i].B04*pCWData[i].B02)/10.0;
				}
				else if(pCWData[i].B03>1)
				{
					AnalyDatap[m].Open   = AnalyDatap[m].Open  *pCWData[i].B03;
					AnalyDatap[m].High   = AnalyDatap[m].High  *pCWData[i].B03;
					AnalyDatap[m].Low    = AnalyDatap[m].Low  *pCWData[i].B03;
					AnalyDatap[m].Close  = AnalyDatap[m].Close  *pCWData[i].B03;
				}
				
				if(g_bVolTQ) 
				{
					AnalyDatap[m].Volume = AnalyDatap[m].Volume*10.0/(10+pCWData[i].B03+pCWData[i].B04);
					AnalyDatap[m].Amount = AnalyDatap[m].Amount-pCWData[i].B02*pCWData[i].B04/10*AnalyDatap[m].Volume+AnalyDatap[m].Volume*pCWData[i].B01/10;
				}
			}
			float fOHigh = AnalyDatap[j].High, fOLow = AnalyDatap[j].Low;
			if(nPeriod==PER_WEEK||nPeriod==PER_MONTH||nPeriod==PER_DAYN||nPeriod==PER_SEASON||nPeriod==PER_YEAR)
			{
				AnalyData AfterTQData  = AnalyDatap[j+1];
				if(j==nEnd)
				{
					AfterTQData  = AnalyDatap[j];
					if(pCWData[i].Type==CWCL_TYPE_DR)
					{
						AfterTQData.Open   = (AfterTQData.Open  *(10.0+pCWData[i].B03+pCWData[i].B04)+pCWData[i].B01-pCWData[i].B04*pCWData[i].B02)/10.0;
						AfterTQData.High   = (AfterTQData.High  *(10.0+pCWData[i].B03+pCWData[i].B04)+pCWData[i].B01-pCWData[i].B04*pCWData[i].B02)/10.0;
						AfterTQData.Low    = (AfterTQData.Low   *(10.0+pCWData[i].B03+pCWData[i].B04)+pCWData[i].B01-pCWData[i].B04*pCWData[i].B02)/10.0;
						AfterTQData.Close  = (AfterTQData.Close *(10.0+pCWData[i].B03+pCWData[i].B04)+pCWData[i].B01-pCWData[i].B04*pCWData[i].B02)/10.0;
					}
					else if(pCWData[i].B03>1)
					{
						AfterTQData.Open   = AfterTQData.Open  *pCWData[i].B03;
						AfterTQData.High   = AfterTQData.High  *pCWData[i].B03;
						AfterTQData.Low    = AfterTQData.Low  *pCWData[i].B03;
						AfterTQData.Close  = AfterTQData.Close  *pCWData[i].B03;
					}
					AnalyDatap[j] = AfterTQData;
				}
				else 
				{
					AnalyDatap[j].Open   = 0.382f*AnalyDatap[j-1].Open+0.618f*AfterTQData.Open;
					AnalyDatap[j].Close  = 0.382f*AnalyDatap[j-1].Close+0.618f*AfterTQData.Close;
					AnalyDatap[j].High   = max(0.382f*AnalyDatap[j-1].High+0.618f*AfterTQData.High, AnalyDatap[j].Close);
					AnalyDatap[j].Low    = min(0.382f*AnalyDatap[j-1].Low+0.618f*AfterTQData.Low, AnalyDatap[j].Close);
				}
			}
			else
			{
				if(pCWData[i].Type==CWCL_TYPE_DR)
				{
					AnalyDatap[j].Open   = (AnalyDatap[j].Open  *(10.0+pCWData[i].B03+pCWData[i].B04)+pCWData[i].B01-pCWData[i].B04*pCWData[i].B02)/10.0;
					AnalyDatap[j].High   = (AnalyDatap[j].High  *(10.0+pCWData[i].B03+pCWData[i].B04)+pCWData[i].B01-pCWData[i].B04*pCWData[i].B02)/10.0;
					AnalyDatap[j].Low    = (AnalyDatap[j].Low   *(10.0+pCWData[i].B03+pCWData[i].B04)+pCWData[i].B01-pCWData[i].B04*pCWData[i].B02)/10.0;
					AnalyDatap[j].Close  = (AnalyDatap[j].Close *(10.0+pCWData[i].B03+pCWData[i].B04)+pCWData[i].B01-pCWData[i].B04*pCWData[i].B02)/10.0;
				}
				else if(pCWData[i].B03>1)
				{
					AnalyDatap[j].Open   = AnalyDatap[j].Open  *pCWData[i].B03;
					AnalyDatap[j].High   = AnalyDatap[j].High  *pCWData[i].B03;
					AnalyDatap[j].Low    = AnalyDatap[j].Low  *pCWData[i].B03;
					AnalyDatap[j].Close  = AnalyDatap[j].Close  *pCWData[i].B03;
				}
			}
		}
		break;
	}
	free(pCWData);
}

short Cwcl::GetTQInfo(struct MemStkInfo * scode,CWDATA *pOutData,short nMaxNum,BOOL bOnlyQk)
{
	if(Init_ZGB_LTGB()==-1) return 0;
	int   nFoundNum=0,nAllocNum=0,i;
	CWDATA *pCWData=NULL;
	//提取财务数据
	for(i=0; i<All_DataNum; i++)
	{   
		if(All_CwData[i].Date<10)   continue;
		if(CompareCwcl(All_CwData+i,scode->Code,scode->setcode))
		{   
			if(!bOnlyQk || (bOnlyQk && (All_CwData[i].Type==CWCL_TYPE_DR||All_CwData[i].Type>=CWCL_TYPE_INPROCHG)))
			{
				if(nFoundNum>=nAllocNum)
				{	
					nAllocNum+=20;
					pCWData = (CWDATA *)realloc(pCWData,nAllocNum*sizeof(CWDATA));
				}
				pCWData[nFoundNum]=All_CwData[i];
				nFoundNum++;
			}
		}
	}
	if(pCWData)
	{
		memmove(pOutData,pCWData,MIN(nMaxNum,nFoundNum)*sizeof(CWDATA));
		free(pCWData);
	}
	return MIN(nMaxNum,nFoundNum);
}

short Cwcl::GetRecentGbbqInfo(CWDATA *pOutData,short nMaxNum)
{
	if(Init_ZGB_LTGB()==-1) return 0;
	int   nFoundNum=0,nAllocNum=0,i;
	CWDATA *pCWData=NULL;
	//提取财务数据
	for(i=0; i<All_DataNum; i++)
	{   
		if(All_CwData[i].Date>=g_lOpenRq)
		{   
			if(nFoundNum>=nAllocNum)
			{	
				nAllocNum+=20;
				pCWData = (CWDATA *)realloc(pCWData,nAllocNum*sizeof(CWDATA));
			}
			pCWData[nFoundNum]=All_CwData[i];
			nFoundNum++;
		}
	}
	if(pCWData)
	{
		memmove(pOutData,pCWData,MIN(nMaxNum,nFoundNum)*sizeof(CWDATA));
		free(pCWData);
	}
	return MIN(nMaxNum,nFoundNum);
}

void Cwcl::ReadLTGB_ZGB(struct MemStkInfo * scode,long * pDate, float * fpLTGB,float * fpZGB,short ItemNum,float fDefaultLTGB,float fDefaultZGB)
{	if(Init_ZGB_LTGB()==-1) return;
	int   nFoundNum=0,nAllocNum=0,i,j,jj,m;
	double  fThisLTGB,fThisZGB;
	CWDATA *pCWData=NULL;
	//提取财务数据
	for(i=0; i<All_DataNum; i++)
	{   if(All_CwData[i].Date<10)   continue;
		if(CompareCwcl(All_CwData+i,scode->Code,scode->setcode))
		{   if(nFoundNum>=nAllocNum)
			{	nAllocNum+=20;
				pCWData = (CWDATA *)realloc(pCWData,nAllocNum*sizeof(CWDATA));
			}
			if(All_CwData[i].Date >= pDate[0])
			{
				pCWData[nFoundNum] = All_CwData[i];
				nFoundNum++;
			}
		}
	}
	fThisLTGB = -1.0;
	fThisZGB = -1.0;
	//提取初始的流通股本信息
	for(i=0; i<nFoundNum; i++)
	{	if(pCWData[i].B01>1.000
		&&(pCWData[i].Type==CWCL_TYPE_SEND2OUT||pCWData[i].Type==CWCL_TYPE_IN2OUT||
		   pCWData[i].Type==CWCL_TYPE_OTHERCAPCHG||pCWData[i].Type==CWCL_TYPE_BUYBACK||pCWData[i].Type==CWCL_TYPE_NEW2OUT||
		   pCWData[i].Type==CWCL_TYPE_RESERVES2OUT||pCWData[i].Type==CWCL_TYPE_CONVERT2OUT))
		{	
			fThisLTGB=pCWData[i].B01;
			fThisZGB=pCWData[i].B02;
			break;
		}
	}
	if(fThisLTGB==-1 || fThisZGB==-1)
	{
		for(i=0; i<ItemNum; i++) 
		{
			fpLTGB[i]=fDefaultLTGB;
			fpZGB[i]=fDefaultZGB;
		}
	}
	else
	{
		for(i=0; i<ItemNum; i++) 
		{
			fpLTGB[i]=fThisLTGB;
			fpZGB[i]=fThisZGB;
		}
	}
	for(i=0; i<nFoundNum; i++)
	{	switch(pCWData[i].Type)
		{
		case CWCL_TYPE_SEND2OUT:
		case CWCL_TYPE_IN2OUT:
		case CWCL_TYPE_OTHERCAPCHG:
		case CWCL_TYPE_BUYBACK:
		case CWCL_TYPE_NEW2OUT:
		case CWCL_TYPE_RESERVES2OUT:
		case CWCL_TYPE_CONVERT2OUT:
			if(pCWData[i].B03<=COMPPREC) break;
			fThisLTGB = pCWData[i].B03;
			fThisZGB = pCWData[i].B04;
			break;
		}
		if(fThisLTGB<0.0) continue;
		if(fThisZGB<0.0) continue;
		for(j=ItemNum-1; j>=0; j--)
			if(pCWData[i].Date>=pDate[j])
				break;
		jj=j+1;
		if(pCWData[i].Date==pDate[j]) jj=j;
		if(pCWData[i].Date>pDate[ItemNum-1]) continue;		//added by iwld 20070227,尚未到期的复权bug
		for(m=MAX(0,jj); m<ItemNum; m++)
		{
			fpLTGB[m]=fThisLTGB;
			fpZGB[m]=fThisZGB;
		}
	}
	free(pCWData);
}

void Cwcl::ReadLTGB_ZGB_OneDate(struct MemStkInfo * scode,long Date,float *LTGB,float *ZGB,float fDefaultLTGB,float fDefaultZGB)
{
	*LTGB=fDefaultLTGB;
	*ZGB=fDefaultZGB;
	if(Init_ZGB_LTGB()==-1) return;
	unsigned long Record_Num = All_DataNum;
	int i; BOOL bFindOK = FALSE;
	short Have_Flag = 0;
	for (i = 0;i<Record_Num;i++)
	{	
		if(bFindOK) return;
		if(CompareCwcl(All_CwData+i,scode->Code,scode->setcode))
		{   
			Have_Flag = 1;
			if (All_CwData[i].Type == CWCL_TYPE_SEND2OUT || All_CwData[i].Type == CWCL_TYPE_IN2OUT
				|| All_CwData[i].Type == CWCL_TYPE_OTHERCAPCHG || All_CwData[i].Type == CWCL_TYPE_BUYBACK
				|| All_CwData[i].Type == CWCL_TYPE_NEW2OUT || All_CwData[i].Type == CWCL_TYPE_RESERVES2OUT
				|| All_CwData[i].Type == CWCL_TYPE_CONVERT2OUT
			   )
			{   
				if(All_CwData[i].Date <= Date)
				{
					*ZGB    =  All_CwData[i].B04;
					*LTGB   =  All_CwData[i].B03;
				}
				else 
				{
					*ZGB    =  All_CwData[i].B02;
					*LTGB   =  All_CwData[i].B01;
					bFindOK = TRUE;
				}
			}
		}
		else if (Have_Flag == 1)	break;
	}
}

int compare( const void *arg1, const void *arg2 )
{
	long lData1 = *(long*)arg1;
	long lData2 = *(long*)arg2;
	if(lData1>lData2) return 1;
	if(lData1==lData2) return 0;
	if(lData1<lData2) return -1;
	return 0; 
}

void	Cwcl::ResetRLTData()
{
	TDEL(ALL_RLTData); TDEL(RLTSetInfo);
	RLTDataSet = All_RLTDataNum = 0;
}

short	Cwcl::ReadRelationInfo(struct MemStkInfo * scode, STKRELATION *RLTData, short nMaxCount)
{
	short Ret = 0;
	if(!ALL_RLTData||RLTDataSet<1||!RLTSetInfo||All_RLTDataNum<1)
	{
		ResetRLTData();
		
		FILE *fp_in = _fsopen(g_WSXHStr+"LData\\cache\\relation.dat","rb",_SH_DENYNO);
		if (fp_in == NULL)  
		{
			ResetRLTData();
			return -1;
		}
		long filelen = on_filelength(fp_in);
		fread(&RLTDataSet,sizeof(short),1,fp_in);
		if(RLTDataSet<1||filelen-sizeof(short)<RLTDataSet*sizeof(short))
		{
			ResetRLTData();
			fclose(fp_in);
			return -1;
		}
		RLTSetInfo = new short[RLTDataSet];
		memset(RLTSetInfo, 0, sizeof(short)*RLTDataSet);
		fread(RLTSetInfo,sizeof(short)*RLTDataSet ,1, fp_in);
		All_RLTDataNum = 0;
		for(int i=0;i<RLTDataSet;i++)
			All_RLTDataNum += RLTSetInfo[i];
		if(All_RLTDataNum<1||filelen-sizeof(short)-RLTDataSet*sizeof(short)<All_RLTDataNum*sizeof(STKRELATION))
		{
			ResetRLTData();
			fclose(fp_in);
			return -1;
		}
		ALL_RLTData = new STKRELATION[All_RLTDataNum];
		memset(ALL_RLTData, 0, All_RLTDataNum*sizeof(STKRELATION));
		fread(ALL_RLTData,All_RLTDataNum*sizeof(STKRELATION),1, fp_in);
		for(i=0;i<All_RLTDataNum;i++)					//强制数字和字母	
		{
			for(int j=0;j<11;j++)
			{
				if(ALL_RLTData[i].Code[j]!=0&&!IsTdxChar(ALL_RLTData[i].Code[j]))
					ALL_RLTData[i].Code[j]=' ';
				ALL_RLTData[i].Code[10] = 0;
			}
			AllTrim(ALL_RLTData[i].Code);
		}
		fclose(fp_in);
	}

	short nSet = 0;
	for(int i=0,j=0;i<All_RLTDataNum;i++,j++)
	{
		if(j>=RLTSetInfo[nSet]) 
		{
			nSet++;
			j = 0;
		}
		if((scode->setcode==ALL_RLTData[i].SetCode || (ALL_RLTData[i].SetCode!=SZ && ALL_RLTData[i].SetCode!=SH)) && strcmp(scode->Code, ALL_RLTData[i].Code)==0)	//找到您了
		{
			//copy整组
			long nStartPos = 0;
			for(int k=0;k<nSet;k++) nStartPos += RLTSetInfo[k];
			if(Ret+RLTSetInfo[nSet]>=nMaxCount) break;
			memcpy(&RLTData[Ret], &ALL_RLTData[nStartPos], RLTSetInfo[nSet]*sizeof(STKRELATION));
			Ret += RLTSetInfo[nSet];

			i = nStartPos+RLTSetInfo[nSet]-1;
			nSet++;
			j = 0;
		}
		if(Ret>=nMaxCount) break;
	}
	return Ret;
}

void	Cwcl::ResetFaceData()
{
	TDEL(ALL_FaceData);
	ALL_FaceNum = 0;
}

long	Cwcl::ReadAllFaceInfo(TDXSTKOLDNAME* &FaceData)
{
	if(!ALL_FaceData||ALL_FaceNum<1)
	{
		ResetFaceData();
		
		FILE *fp_in = _fsopen(g_WSXHStr+"LData\\cache\\profile.dat","rb",_SH_DENYNO);
		if (fp_in == NULL)  
		{
			ResetFaceData();
			return -1;
		}
		long filelen = on_filelength(fp_in);
		if(filelen<=0 || filelen%sizeof(TDXSTKOLDNAME))
		{
			ResetFaceData();
			fclose(fp_in);
			return -1;
		}
		ALL_FaceNum = filelen/sizeof(TDXSTKOLDNAME);
		ALL_FaceData = new TDXSTKOLDNAME[ALL_FaceNum];
		memset(ALL_FaceData, 0, ALL_FaceNum*sizeof(TDXSTKOLDNAME));
		fread(ALL_FaceData,ALL_FaceNum*sizeof(TDXSTKOLDNAME),1, fp_in);
		for(int i=0;i<ALL_FaceNum;i++)					//强制数字和字母	
		{
			for(int j=0;j<7;j++)
			{
				if(ALL_FaceData[i].Code[j]!=0&&!IsTdxChar(ALL_FaceData[i].Code[j]))
					ALL_FaceData[i].Code[j]=' ';
			}
			ALL_FaceData[i].Code[6] = 0;
			ALL_FaceData[i].Name[8] = 0;
			ALL_FaceData[i].Reason[42] = 0;
			AllTrim(ALL_FaceData[i].Code);
		}
		fclose(fp_in);
	}
	FaceData = ALL_FaceData;
	return ALL_FaceNum;
}

short	Cwcl::ReadFaceInfo(struct MemStkInfo * scode, TDXSTKOLDNAME *FaceData, short nMaxCount)
{
	if(!scode) return 0;
	if(!ALL_FaceData||ALL_FaceNum<1)
	{
		ResetFaceData();
		
		FILE *fp_in = _fsopen(g_WSXHStr+"LData\\cache\\profile.dat","rb",_SH_DENYNO);
		if (fp_in == NULL)  
		{
			ResetFaceData();
			return -1;
		}
		long filelen = on_filelength(fp_in);
		if(filelen%sizeof(TDXSTKOLDNAME))
		{
			ResetFaceData();
			fclose(fp_in);
			return -1;
		}
		ALL_FaceNum = filelen/sizeof(TDXSTKOLDNAME);
		ALL_FaceData = new TDXSTKOLDNAME[ALL_FaceNum];
		memset(ALL_FaceData, 0, ALL_FaceNum*sizeof(TDXSTKOLDNAME));
		fread(ALL_FaceData,ALL_FaceNum*sizeof(TDXSTKOLDNAME),1, fp_in);
		for(int i=0;i<ALL_FaceNum;i++)					//强制数字和字母	
		{
			for(int j=0;j<7;j++)
			{
				if(ALL_FaceData[i].Code[j]!=0&&!IsTdxChar(ALL_FaceData[i].Code[j]))
					ALL_FaceData[i].Code[j]=' ';
				ALL_FaceData[i].Code[6] = 0;
			}
			AllTrim(ALL_FaceData[i].Code);
		}
		fclose(fp_in);
	}
	short Ret = 0;
	for(int i=0;i<ALL_FaceNum;i++)
	{
		if(scode->setcode==ALL_FaceData[i].SetCode&&strcmp(scode->Code, ALL_FaceData[i].Code)==0)	//找到您了
			memcpy(&FaceData[Ret++], &ALL_FaceData[i], sizeof(TDXSTKOLDNAME));
		if(Ret>=nMaxCount) break;
	}
	return Ret;
}
