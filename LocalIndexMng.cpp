#include "stdafx.h"
#include "tdxw.h"
#include "LocalIndexMng.h"


LcIdxMng::LcIdxMng()
{
	m_pLcSecIdx			= new LCSECIDX[3*LOCALINDEX_MAXSPACE];
	m_LcSecIdxNum		= 0;
	//
	m_pLcPtSet			= NULL;
	m_LcPtSetNum		= 0;
	m_LcPtSetSpc		= 0;
	//
	m_pLcExtSet			= NULL;
	m_LcExtSetNum		= 0;
	m_LcExtSetSpc		= 0;
	//
	m_pSrcText			= NULL;
	m_lSrcTextLen		= 0;
	m_lSrcTextSpc		= 0;
	//
	m_pLcIdxSet			= NULL;
	m_LcIdxSetNum		= 0;
	m_LcIdxSetSpc		= 0;
	//
	m_pLcIdxStk			= NULL;
	m_LcIdxStkNum		= 0;
	m_LcIdxStkSpc		= 0;
	//
	m_pLcIdxDay			= NULL;
	m_LcIdxStkNum		= 0;
	m_LcIdxStkSpc		= 0;
	//
	m_pLcStkDay			= NULL;
	m_LcIdxStkNum		= 0;
	m_LcIdxStkSpc		= 0;
	//
	m_pCurStock			= NULL;
	//
	m_pIndexAnalyData	= NULL;
	m_IndexAnalyNum		= 0;
	m_IndexAnalySpc		= 0;
	//
	m_pStockAnalyData	= NULL;
	m_StockAnalyNum		= 0;
	m_StockAnalySpc		= 0;
	//
	m_pCwcl				= NULL;
	m_pCWData			= NULL;
	m_lCWDataNum		= 0;
	m_lCWDataSpc		= 0;
	//
	m_MaxDays			= 0;
}

LcIdxMng::~LcIdxMng()
{
	TDELA(m_pLcSecIdx);
	TDELA(m_pLcPtSet);
	TDELA(m_pLcExtSet);
	TDELA(m_pSrcText);
	TDELA(m_pLcIdxSet);
	TDELA(m_pLcIdxStk);
	TDELA(m_pLcIdxDay);
	TDELA(m_pLcStkDay);
	TDELA(m_pIndexAnalyData);
	TDELA(m_pStockAnalyData);
	TDELA(m_pCWData);
	TDEL(m_pCwcl);
}

BOOL LcIdxMng::LoadAll(BOOL bInit)
{
	if(m_pLcSecIdx==NULL)	
		m_pLcSecIdx	= new LCSECIDX[3*LOCALINDEX_MAXSPACE];
	if(m_pLcSecIdx==NULL)	
		return FALSE;
	
	LoadExtSet(bInit);	//加载外部品种
	LoadIdxSet(bInit);	//加载组合品种
	LoadPtSet(bInit);	//加载退市品种

	long i = 0;
	
	memset(m_pLcSecIdx,0,3*LOCALINDEX_MAXSPACE*sizeof(LCSECIDX));
	m_LcSecIdxNum	= 0;
	for(i=0;i<m_LcIdxSetNum;i++)
	{
		m_pLcSecIdx[m_LcSecIdxNum].lType		= 1;
		memcpy(m_pLcSecIdx[m_LcSecIdxNum].Code,m_pLcIdxSet[i].Code,(CODE_SIZE-1)*sizeof(char));
		m_pLcSecIdx[m_LcSecIdxNum].Code[CODE_SIZE-1] = '\0';
		memcpy(m_pLcSecIdx[m_LcSecIdxNum].Name,m_pLcIdxSet[i].Name,(NAME_SIZE-1)*sizeof(char));
		m_pLcSecIdx[m_LcSecIdxNum].Name[NAME_SIZE-1] = '\0';
		m_pLcSecIdx[m_LcSecIdxNum].lMemStkIdx	= m_LcSecIdxNum;
		m_pLcSecIdx[m_LcSecIdxNum].lSubIndex	= i;
		m_LcSecIdxNum++;
	}
	for(i=0;i<m_LcExtSetNum;i++)
	{
		m_pLcSecIdx[m_LcSecIdxNum].lType		= 2;
		memcpy(m_pLcSecIdx[m_LcSecIdxNum].Code,m_pLcExtSet[i].Code,(CODE_SIZE-1)*sizeof(char));
		m_pLcSecIdx[m_LcSecIdxNum].Code[CODE_SIZE-1] = '\0';
		memcpy(m_pLcSecIdx[m_LcSecIdxNum].Name,m_pLcExtSet[i].Name,(NAME_SIZE-1)*sizeof(char));
		m_pLcSecIdx[m_LcSecIdxNum].Name[NAME_SIZE-1] = '\0';
		m_pLcSecIdx[m_LcSecIdxNum].lMemStkIdx	= m_LcSecIdxNum;
		m_pLcSecIdx[m_LcSecIdxNum].lSubIndex	= i;
		m_LcSecIdxNum++;
	}
	for(i=0;i<m_LcPtSetNum;i++)
	{
		m_pLcSecIdx[m_LcSecIdxNum].lType	= 3;
		memcpy(m_pLcSecIdx[m_LcSecIdxNum].Code,m_pLcPtSet[i].Code,(CODE_SIZE-1)*sizeof(char));
		m_pLcSecIdx[m_LcSecIdxNum].Code[CODE_SIZE-1] = '\0';
		memcpy(m_pLcSecIdx[m_LcSecIdxNum].Name,m_pLcPtSet[i].Name,(NAME_SIZE-1)*sizeof(char));
		m_pLcSecIdx[m_LcSecIdxNum].Name[NAME_SIZE-1] = '\0';
		m_pLcSecIdx[m_LcSecIdxNum].lMemStkIdx	= m_LcSecIdxNum;
		m_pLcSecIdx[m_LcSecIdxNum].lSubIndex	= i;
		m_LcSecIdxNum++;
	}

	return TRUE;
}

BOOL LcIdxMng::SaveAll()
{
	SaveExtSet();
	SaveIdxSet();
	SavePtSet();
	return TRUE;
}

long LcIdxMng::GetSecNum()
{
	return m_LcSecIdxNum;
}

long LcIdxMng::GetSecIdx(LPLCSECIDX &pLcSecIdx)
{
	pLcSecIdx = m_pLcSecIdx;
	return m_LcSecIdxNum;
}

BOOL LcIdxMng::LoadPtSet(BOOL bInit)
{
	BOOL bRet = TRUE;
	long i = 0;
	CString FilePath = g_WSXHStr+"lc\\";
	FilePath += "lcpts.lpi";
	FILE *fpt = fopen(LPCSTR(FilePath),"rb");
	if(fpt==NULL) return FALSE;

	m_LcPtSetNum = on_filelength(fpt);
	if(m_LcPtSetNum<=0 || m_LcPtSetNum%sizeof(LCPTSET)!=0)
	{
		fclose(fpt);
		m_LcPtSetNum = 0;
		return FALSE;
	}
	m_LcPtSetNum /= sizeof(LCPTSET);
	if(m_LcPtSetNum>LOCALINDEX_MAXSPACE) m_LcPtSetNum = LOCALINDEX_MAXSPACE;
	NEWLCIDXSPACE(bRet,m_pLcPtSet,m_LcPtSetSpc,m_LcPtSetNum,INDEXSETSPACE_STEP,LCPTSET);
	if(!bRet)
	{
		fclose(fpt);
		m_pLcPtSet		= NULL;
		m_LcPtSetNum	= 0;
		m_LcPtSetSpc	= 0;
		return FALSE;
	}
	fseek(fpt,0,SEEK_SET);
	m_LcPtSetNum = fread(m_pLcPtSet,sizeof(LCPTSET),m_LcPtSetNum,fpt);
	fclose(fpt);
	return TRUE;
}

BOOL LcIdxMng::SavePtSet()
{
	if(m_pLcPtSet==NULL || m_LcPtSetNum<0) return FALSE;
	CString FilePath = g_WSXHStr+"lc\\";
	FilePath += "lcpts.lpi";
	FILE *fpt = fopen(LPCSTR(FilePath),"wb");
	if(fpt==NULL) return FALSE;
	if(m_LcPtSetNum>0)
		fwrite(m_pLcPtSet,m_LcPtSetNum*sizeof(LCPTSET),1,fpt);
	fclose(fpt);
	return TRUE;
}

BOOL LcIdxMng::AddLcPt(LPLCPTSET pLcPtSet)
{
	BOOL bRet = TRUE;
	LPLCPTSET pTmpLcPtSet = NULL;
	CHECKLCIDXSPACE(bRet,m_pLcPtSet,pTmpLcPtSet,m_LcPtSetSpc,(m_LcPtSetNum+1),INDEXSETSPACE_STEP,LCPTSET);
	if(!bRet) return FALSE;
	memcpy(&(m_pLcPtSet[m_LcPtSetNum]),pLcPtSet,sizeof(LCPTSET));
	m_LcPtSetNum++;
	return SavePtSet();
}

BOOL LcIdxMng::ChgLcPt(LPLCPTSET pLcPtSet,long lPtNo)
{
	memcpy(&(m_pLcPtSet[lPtNo]),pLcPtSet,sizeof(LCPTSET));
	return SavePtSet();
}

BOOL LcIdxMng::DelLcPt(long lPtNo)
{
	if(lPtNo<0 || lPtNo>=m_LcPtSetNum || m_pLcPtSet==NULL) return FALSE;
	if(lPtNo<m_LcPtSetNum-1)
		memmove(&(m_pLcPtSet[lPtNo]),&(m_pLcPtSet[lPtNo+1]),(m_LcPtSetNum-1-lPtNo)*sizeof(LCPTSET));
	m_LcPtSetNum--;
	memset(&(m_pLcPtSet[m_LcPtSetNum]),0,sizeof(LCPTSET));
	return SavePtSet();
}

long LcIdxMng::GetPtNum()
{
	return m_LcPtSetNum;
}

long LcIdxMng::GetPtSet(LPLCPTSET &pLcPtSet)
{
	pLcPtSet = m_pLcPtSet;
	return m_LcPtSetNum;
}

BOOL LcIdxMng::TestPtExist(char *pPtCode,long lPtNo)
{
	if(pPtCode==NULL) return FALSE;
	if(m_pLcPtSet==NULL) return FALSE;
	long i = 0;
	for(i=0;i<m_LcPtSetNum;i++)
	{
		if(i==lPtNo) continue;		//不检查自身
		if(strncmp(pPtCode,m_pLcPtSet[i].Code,NAME_SIZE-1)==0)
			return TRUE;
	}
	return FALSE;
}

LPLCPTSET LcIdxMng::GetLcPTSet(char *pPtCode)
{
	if(pPtCode==NULL) return NULL;
	if(m_pLcPtSet==NULL) return NULL;
	long i = 0;
	for(i=0;i<m_LcPtSetNum;i++)
	{
		if(strncmp(pPtCode,m_pLcPtSet[i].Code,NAME_SIZE-1)==0)
			return m_pLcPtSet+i;
	}
	return NULL;
}

BOOL LcIdxMng::LoadExtSet(BOOL bInit)
{
	BOOL bRet = TRUE;
	long i = 0;
	CString FilePath = g_WSXHStr+"lc\\";
	FilePath += "lcext.lei";
	FILE *fpt = fopen(LPCSTR(FilePath),"rb");
	if(fpt==NULL) return FALSE;

	m_LcExtSetNum = on_filelength(fpt);
	if(m_LcExtSetNum<=0 || m_LcExtSetNum%sizeof(LCEXTSET)!=0)
	{
		fclose(fpt);
		m_LcExtSetNum = 0;
		return FALSE;
	}
	m_LcExtSetNum /= sizeof(LCEXTSET);
	if(m_LcExtSetNum>LOCALINDEX_MAXSPACE) m_LcExtSetNum = LOCALINDEX_MAXSPACE;
	NEWLCIDXSPACE(bRet,m_pLcExtSet,m_LcExtSetSpc,m_LcExtSetNum,INDEXSETSPACE_STEP,LCEXTSET);
	if(!bRet)
	{
		fclose(fpt);
		m_pLcExtSet		= NULL;
		m_LcExtSetNum	= 0;
		m_LcExtSetSpc	= 0;
		return FALSE;
	}
	fseek(fpt,0,SEEK_SET);
	m_LcExtSetNum = fread(m_pLcExtSet,sizeof(LCEXTSET),m_LcExtSetNum,fpt);
	fclose(fpt);
	return TRUE;
}

BOOL LcIdxMng::SaveExtSet()
{
	if(m_pLcExtSet==NULL || m_LcExtSetNum<0) return FALSE;
	CString FilePath = g_WSXHStr+"lc\\";
	FilePath += "lcext.lei";
	FILE *fpt = fopen(LPCSTR(FilePath),"wb");
	if(fpt==NULL) return FALSE;
	if(m_LcExtSetNum>0)
		fwrite(m_pLcExtSet,m_LcExtSetNum*sizeof(LCEXTSET),1,fpt);
	fclose(fpt);
	return LoadAll();
}

BOOL LcIdxMng::ReLoad(long lExtNo)
{
	if(lExtNo>=0 && lExtNo<m_LcExtSetNum)
	{
		if(!ReLoadExtern(lExtNo))
		{
			TDX_MessageBox(NULL,"数据转换失败,数据源或者目标文件访问失败,或者数据内容和格式不合要求!",MB_OK|MB_ICONERROR);
			return FALSE;
		}
		else
		{
			TDX_MessageBox(NULL,"导入成功!",MB_OK|MB_ICONINFORMATION);
			return TRUE;
		}
	}

	long i=0;
	for(i=0;i<m_LcExtSetNum;i++)
		ReLoadExtern(i);
	return TRUE;
}

long LcIdxMng::GetDate(char *buf,long lDateFormat)
{
	if(buf==NULL) return 0;
	AllTrim(buf);
	char cDateMark = '\0';
	long i = 0;
	long lSpace1 = 0,lSpace2 = 0;
	long lRet = 0;
	switch(lDateFormat)
	{
	case LCEXTDATE__M__D__Y:
	case LCEXTDATE__D__M__Y:
	case LCEXTDATE__Y__M__D:
		cDateMark = '/';
	case LCEXTDATE_M_D_Y:
	case LCEXTDATE_D_M_Y:
	case LCEXTDATE_Y_M_D:
		if(cDateMark=='\0')
			cDateMark = '-';
		break;
	case LCEXTDATEYMD:
		if(strlen(buf)!=8) return lRet;
		break;
	default: return lRet;
	}
	//
	switch(lDateFormat)
	{
	case LCEXTDATE__M__D__Y:
	case LCEXTDATE_M_D_Y:
	case LCEXTDATE__D__M__Y:
	case LCEXTDATE_D_M_Y:
		lSpace1 = 2;
		lSpace2 = 5;
	case LCEXTDATE__Y__M__D:
	case LCEXTDATE_Y_M_D:
		if(lSpace1==0) lSpace1 = 4;
		if(lSpace2==0) lSpace2 = 7;
		for(i=0;i<10;i++)
		{
			if(i==lSpace1 || i==lSpace2)
			{
				if(buf[i]!=cDateMark) return lRet;
			}
			else
			{
				if(buf[i]<'0' || buf[i]>'9') return lRet;
			}
		}
		break;
	case LCEXTDATEYMD:
		for(i=0;i<8;i++)
			if(buf[i]<'0' || buf[i]>'9') return lRet;
		break;
	}
	//
	switch(lDateFormat)
	{
	case LCEXTDATE__M__D__Y:
	case LCEXTDATE_M_D_Y:
		lRet =	(buf[6]-'0')*10000000l
			+(buf[7]-'0')*1000000l
			+(buf[8]-'0')*100000l
			+(buf[9]-'0')*10000l
			+(buf[0]-'0')*1000l
			+(buf[1]-'0')*100l
			+(buf[3]-'0')*10l
			+(buf[4]-'0');
		break;
	case LCEXTDATE__D__M__Y:
	case LCEXTDATE_D_M_Y:
		lRet =	(buf[6]-'0')*10000000l
			+(buf[7]-'0')*1000000l
			+(buf[8]-'0')*100000l
			+(buf[9]-'0')*10000l
			+(buf[3]-'0')*1000l
			+(buf[4]-'0')*100l
			+(buf[0]-'0')*10l
			+(buf[1]-'0');
		break;
	case LCEXTDATE__Y__M__D:
	case LCEXTDATE_Y_M_D:
		lRet =	(buf[0]-'0')*10000000l
			+(buf[1]-'0')*1000000l
			+(buf[2]-'0')*100000l
			+(buf[3]-'0')*10000l
			+(buf[5]-'0')*1000l
			+(buf[6]-'0')*100l
			+(buf[8]-'0')*10l
			+(buf[9]-'0');
		break;
	case LCEXTDATEYMD:
		lRet =	(buf[0]-'0')*10000000l
			+(buf[1]-'0')*1000000l
			+(buf[2]-'0')*100000l
			+(buf[3]-'0')*10000l
			+(buf[4]-'0')*1000l
			+(buf[5]-'0')*100l
			+(buf[6]-'0')*10l
			+(buf[7]-'0');
		break;
	}
	return lRet;
}

BOOL LcIdxMng::ReLoadExtern(long lExtNo)
{
	if(lExtNo<0 || lExtNo>=m_LcExtSetNum || m_pLcExtSet==NULL) return FALSE;
	FILE *fsrc=NULL;
	FILE *fdesc=NULL;
	fsrc = fopen(m_pLcExtSet[lExtNo].SrcFilePath,"rb");
	if(fsrc==NULL) return FALSE;
	long m_lSrcTextLen = on_filelength(fsrc);
	BOOL bRet = TRUE;
	if(m_lSrcTextLen==0)
	{
		fclose(fsrc);
		return FALSE;
	}
	bRet = TRUE;
	NEWLCIDXSPACE(bRet,m_pSrcText,m_lSrcTextSpc,m_lSrcTextLen,1024,char);
	if(!bRet)	return FALSE;
	memset(m_pSrcText,0,m_lSrcTextSpc*sizeof(char));
	fseek(fsrc,0,SEEK_SET);
	if(fread(m_pSrcText,m_lSrcTextLen,1,fsrc)!=1)
	{
		fclose(fsrc);
		return FALSE;
	}
	fclose(fsrc);
	//
	int i=0;
	int j=0;
	int k=0;
	char buf[256];
	char *tmp = NULL;
	int nCount= 0;
	int nLine = 0;
	BOOL bGetNextLine = FALSE;
	BOOL bEnd = FALSE;
	long lVolRatio = 1;

	m_StockAnalyNum = 0;
	for(i=0;i<m_lSrcTextLen;i++)
	{
		if(m_pSrcText[i] == '\n')
			m_StockAnalyNum++;
	}
	bRet = TRUE;
	NEWLCIDXSPACE(bRet,m_pStockAnalyData,m_StockAnalySpc,m_StockAnalyNum,INDEXDAYSPACE_STEP,ANALYDATA);
	if(!bRet)	return FALSE;
	memset(m_pStockAnalyData,0,m_StockAnalySpc*sizeof(ANALYDATA));
	//
	switch(m_pLcExtSet[lExtNo].nVolRatio)
	{
	case LCEXTVOL_1P1:		lVolRatio = 1;		break;
	case LCEXTVOL_1P10:		lVolRatio = 10;		break;
	case LCEXTVOL_1P100:	lVolRatio = 100;	break;
	case LCEXTVOL_1P1000:	lVolRatio = 1000;	break;
	case LCEXTVOL_1P10000:	lVolRatio = 10000;	break;
	default:				lVolRatio = 1;		break;
	}
	//
	for(i=0,tmp = &(m_pSrcText[0]);i<m_lSrcTextLen && nLine<m_StockAnalyNum;i++)
	{
		if(nCount >= m_StockAnalyNum) break;
		for(j=i;j<m_lSrcTextLen;j++)
			if(m_pSrcText[j]=='\n') break;
		if(j<m_lSrcTextLen)
			m_pSrcText[j] = '\0';
		bGetNextLine = FALSE;
		for(k=0;k<10;k++)
		{
			switch(m_pLcExtSet[lExtNo].nSpaceType)
			{
			case LCEXTSPACE_COMMA:
				ParseMessageStr(tmp,buf,",",k+1);
				break;
			case LCEXTSPACE_SEM:
				ParseMessageStr(tmp,buf,";",k+1);
				break;
			case LCEXTSPACE_TAB:
				ParseMessageStr(tmp,buf,"\t",k+1);
				break;
			case LCEXTSPACE_SPACE:
				ParseMessageStr(tmp,buf," ",k+1);
				break;
			default:
				ParseMessageStr(tmp,buf,"\t",k+1);
				break;
			}
			switch(m_pLcExtSet[lExtNo].nFieldFormat[k])
			{
			case LCEXTFORMAT_DATE:
				m_pStockAnalyData[nCount].Time.Date	= GetDate(buf,m_pLcExtSet[lExtNo].nDateFormat);
				if(m_pStockAnalyData[nCount].Time.Date==0) bGetNextLine = TRUE;
				break;
			case LCEXTFORMAT_OPEN:
				m_pStockAnalyData[nCount].l_Open	= LONG(atof(buf)*100+0.5);
				break;
			case LCEXTFORMAT_HIGH:
				m_pStockAnalyData[nCount].l_High	= LONG(atof(buf)*100+0.5);
				break;
			case LCEXTFORMAT_LOW:
				m_pStockAnalyData[nCount].l_Low		= LONG(atof(buf)*100+0.5);
				break;
			case LCEXTFORMAT_CLOSE:
				m_pStockAnalyData[nCount].l_Close	= LONG(atof(buf)*100+0.5);
				break;
			case LCEXTFORMAT_VOLUME:
				m_pStockAnalyData[nCount].Volume	= DWORD(atof(buf)/lVolRatio+0.5);
				break;
			case LCEXTFORMAT_AMOUNT:
				m_pStockAnalyData[nCount].Amount	= float(atof(buf));
				break;
			case LCEXTFORMAT_SKIP:
			default:
				break;
			}
			if(bGetNextLine) break;
		}
		i=j+1;
		tmp = &(m_pSrcText[i]);
		nLine++;
		if(nLine<=m_pLcExtSet[lExtNo].lSkipLines) continue;
		if(bGetNextLine) continue;
		nCount++;
		if(i>=m_lSrcTextLen) break;
	}
	m_StockAnalyNum = nCount;
	//此处进行价位容错
	BOOL	bHasOpen	= FALSE;
	BOOL	bHasLow		= FALSE;
	BOOL	bHasHigh	= FALSE;
	BOOL	bHasClose	= FALSE;
	for(i=0;i<10;i++)
	{
		switch(m_pLcExtSet[lExtNo].nFieldFormat[i])
		{
			case LCEXTFORMAT_OPEN:
				bHasOpen	= TRUE;
				break;
			case LCEXTFORMAT_HIGH:
				bHasHigh	= TRUE;
				break;
			case LCEXTFORMAT_LOW:
				bHasLow		= TRUE;
				break;
			case LCEXTFORMAT_CLOSE:
				bHasClose	= TRUE;
				break;
		}
	}
	for(i=0;i<m_StockAnalyNum;i++)
	{
		if(!bHasClose)
		{
			if(!bHasOpen)
			{
				if(!bHasHigh)
				{
					if(!bHasLow) m_pStockAnalyData[i].l_Close = 0;
					else m_pStockAnalyData[i].l_Close = m_pStockAnalyData[i].l_Low;
				}
				else m_pStockAnalyData[i].l_Close = m_pStockAnalyData[i].l_High;
			}
			else m_pStockAnalyData[i].l_Close = m_pStockAnalyData[i].l_Open;
		}
		if(!bHasOpen)	m_pStockAnalyData[i].l_Open = m_pStockAnalyData[i].l_Close;
		if(!bHasLow)	m_pStockAnalyData[i].l_Low = min(m_pStockAnalyData[i].l_Open,m_pStockAnalyData[i].l_Close);
		m_pStockAnalyData[i].l_Low = min(m_pStockAnalyData[i].l_Open,m_pStockAnalyData[i].l_Low);
		m_pStockAnalyData[i].l_Low = min(m_pStockAnalyData[i].l_Close,m_pStockAnalyData[i].l_Low);
		if(!bHasHigh)	m_pStockAnalyData[i].l_High = max(m_pStockAnalyData[i].l_Open,m_pStockAnalyData[i].l_Close);
		m_pStockAnalyData[i].l_High = max(m_pStockAnalyData[i].l_Open,m_pStockAnalyData[i].l_High);
		m_pStockAnalyData[i].l_High = max(m_pStockAnalyData[i].l_Close,m_pStockAnalyData[i].l_High);
	}

	CString	FilePath;
	FilePath.Format("%s%s\\day\\%s%s.day",g_TdxCfg.DocPath,GetSetStr(SZ),GetSetStr(SZ),m_pLcExtSet[lExtNo].Code);
	g_HisDataReader.Lock();
	fdesc = fopen(LPCSTR(FilePath),"wb");
	if(fdesc!=NULL)
	{
		fseek(fdesc,0,SEEK_SET);
		if(m_StockAnalyNum>0) fwrite(m_pStockAnalyData,m_StockAnalyNum*sizeof(ANALYDATA),1,fdesc);
		fclose(fdesc);
	}
	else 
	{
		g_HisDataReader.Unlock();
		return FALSE;
	}
	g_HisDataReader.Unlock();
	if(m_StockAnalyNum<=0) return FALSE;
	return TRUE;
}

BOOL LcIdxMng::AddLcExt(LPLCEXTSET pLcExtSet)
{
	BOOL bRet = TRUE;
	LPLCEXTSET pTmpLcExtSet = NULL;
	CHECKLCIDXSPACE(bRet,m_pLcExtSet,pTmpLcExtSet,m_LcExtSetSpc,(m_LcExtSetNum+1),INDEXSETSPACE_STEP,LCEXTSET);
	if(!bRet) return FALSE;
	memcpy(&(m_pLcExtSet[m_LcExtSetNum]),pLcExtSet,sizeof(LCEXTSET));
	m_LcExtSetNum++;
	return SaveExtSet();
}

BOOL LcIdxMng::ChgLcExt(LPLCEXTSET pLcExtSet,long lExtNo)
{
	memcpy(&(m_pLcExtSet[lExtNo]),pLcExtSet,sizeof(LCEXTSET));
	return SaveExtSet();
}

BOOL LcIdxMng::DelLcExt(long lExtNo)
{
	if(lExtNo<0 || lExtNo>=m_LcExtSetNum || m_pLcExtSet==NULL) return FALSE;
	if(lExtNo<m_LcExtSetNum-1)
		memmove(&(m_pLcExtSet[lExtNo]),&(m_pLcExtSet[lExtNo+1]),(m_LcExtSetNum-1-lExtNo)*sizeof(LCEXTSET));
	m_LcExtSetNum--;
	memset(&(m_pLcExtSet[m_LcExtSetNum]),0,sizeof(LCEXTSET));
	return SaveExtSet();
}

long LcIdxMng::GetExtNum()
{
	return m_LcExtSetNum;
}

long LcIdxMng::GetExtSet(LPLCEXTSET &pLcExtSet)
{
	pLcExtSet = m_pLcExtSet;
	return m_LcExtSetNum;
}

BOOL LcIdxMng::TestExtExist(char *pExtCode,long lExtNo)
{
	if(pExtCode==NULL) return FALSE;
	if(m_pLcExtSet==NULL) return FALSE;
	long i = 0;
	for(i=0;i<m_LcExtSetNum;i++)
	{
		if(i==lExtNo) continue;		//不检查自身
		if(strncmp(pExtCode,m_pLcExtSet[i].Code,CODE_SIZE-1)==0)
			return TRUE;
	}
	return FALSE;
}

/*
BOOL LcIdxMng::LoadIdxSet(BOOL bInit)
{
	if(m_pCwcl == NULL) m_pCwcl = new Cwcl;
	BOOL bRet = TRUE;
	long i = 0;

	if(!g_pConstMng) return FALSE;
	m_LcIdxSetNum = g_pConstMng->GetZJHHYBlockNum();
	if(m_LcIdxSetNum>LOCALINDEX_MAXSPACE) m_LcIdxSetNum = LOCALINDEX_MAXSPACE;
	NEWLCIDXSPACE(bRet,m_pLcIdxSet,m_LcIdxSetSpc,m_LcIdxSetNum,INDEXSETSPACE_STEP,LCIDXSET);
	if(!bRet)
	{
		m_pLcIdxSet		= NULL;
		m_LcIdxSetNum	= 0;
		m_LcIdxSetSpc	= 0;
		return FALSE;
	}
	for(i=0;i<m_LcIdxSetNum;i++)
	{
		ZJHHY_Cfg *pHyCfg = g_pConstMng->GetHyFromHYXH(i);
		if(!pHyCfg) continue;

		memset(&m_pLcIdxSet[i], 0, sizeof(m_pLcIdxSet[i]));

		m_pLcIdxSet[i].nRefreshedTime = 0;
		sprintf(m_pLcIdxSet[i].Code, "%s%03d", LCIDX_FIX, pHyCfg->HYID);
		m_pLcIdxSet[i].Code[CODE_SIZE-1]=0;
		strncpy(m_pLcIdxSet[i].Name, pHyCfg->HYName, NAME_SIZE-1);
		m_pLcIdxSet[i].nHYID = pHyCfg->HYID;
		m_pLcIdxSet[i].nUnitNum = g_pConstMng->GetStockNumInHY(pHyCfg->HYID);
		m_pLcIdxSet[i].QZType = 0;
		m_pLcIdxSet[i].BaseDate = GetVipDocFirstDate();
		m_pLcIdxSet[i].BaseValue = 1000.0f;
	}
	return m_LcIdxSetNum>0;
}
*/

BOOL LcIdxMng::LoadIdxSet(BOOL bInit)
{
	if(m_pCwcl == NULL) m_pCwcl = new Cwcl;
	BOOL bRet = TRUE;
	long i = 0;
	
	if(!g_pConstMng) return FALSE;
	m_LcIdxSetNum = g_pConstMng->GetZJHHYBlockNum();
	if(m_LcIdxSetNum>LOCALINDEX_MAXSPACE) m_LcIdxSetNum = LOCALINDEX_MAXSPACE;
	NEWLCIDXSPACE(bRet,m_pLcIdxSet,m_LcIdxSetSpc,m_LcIdxSetNum,INDEXSETSPACE_STEP,LCIDXSET);
	if(!bRet)
	{
		m_pLcIdxSet		= NULL;
		m_LcIdxSetNum	= 0;
		m_LcIdxSetSpc	= 0;
		return FALSE;
	}
	for(i=0;i<m_LcIdxSetNum;i++)
	{
		ZJHHY_Cfg *pHyCfg = g_pConstMng->GetHyFromHYXH(i);
		if(!pHyCfg) continue;
		
		memset(&m_pLcIdxSet[i], 0, sizeof(m_pLcIdxSet[i]));
		
		m_pLcIdxSet[i].nRefreshedTime = 0;
		sprintf(m_pLcIdxSet[i].Code, "%s", pHyCfg->HYCode);
		m_pLcIdxSet[i].Code[CODE_SIZE-1]=0;
		strncpy(m_pLcIdxSet[i].Name, pHyCfg->HYName, NAME_SIZE-1);
		m_pLcIdxSet[i].nHYID = pHyCfg->HYID;
		m_pLcIdxSet[i].nUnitNum = g_pConstMng->GetStockNumInHY(pHyCfg->HYID);
	//	m_pLcIdxSet[i].QZType = 0;
	//	m_pLcIdxSet[i].BaseDate = GetVipDocFirstDate();
	//	m_pLcIdxSet[i].BaseValue = 1000.0f;
	}
	return m_LcIdxSetNum>0;
}

BOOL LcIdxMng::SaveIdxSet()
{
	return TRUE;

	if(m_pLcIdxSet==NULL || m_LcIdxSetNum<0) return FALSE;
	CString FilePath = g_WSXHStr+"lc\\";
	FilePath += "lcidx.lii";
	FILE *fpt = fopen(LPCSTR(FilePath),"wb");
	if(fpt==NULL) return FALSE;
	if(m_LcIdxSetNum>0)
		fwrite(m_pLcIdxSet,m_LcIdxSetNum*sizeof(LCIDXSET),1,fpt);
	fclose(fpt);
	return LoadAll();
}

BOOL LcIdxMng::ReCalc(long lIndexNo)
{
	BOOL bRet = TRUE;
	if(lIndexNo>=0 && lIndexNo<m_LcIdxSetNum)
	{
		time_t ct = time(NULL);
		if(ct-m_pLcIdxSet[lIndexNo].nRefreshedTime<60*10)
			return TRUE;
		InterlockedExchange(&m_pLcIdxSet[lIndexNo].nRefreshedTime, ct);
		m_CalcSec.Lock();
		if(!ReCalcIndex(lIndexNo))
		{
		//	TDX_MessageBox(NULL,"计算失败,指定的股票样本错误,或者计算时间范围不合理!",MB_OK|MB_ICONERROR);
			bRet = FALSE;
		}
		m_CalcSec.Unlock();
	}
	else
	{
		long i=0;
		m_CalcSec.Lock();
		for(i=0;i<m_LcIdxSetNum;i++)
			ReCalcIndex(i);
		m_CalcSec.Unlock();
	}
	SaveIdxSet();
	return bRet;
}

BOOL LcIdxMng::GetIdxStk(LPLCIDXSTK &pLcIdxStk,long &LcIdxStkNum,long &LcIdxStkSpc,long lIndexNo)
{
	if(lIndexNo<0 || lIndexNo>=m_LcIdxSetNum || m_pLcIdxSet==NULL) return FALSE;
	BOOL bRet = TRUE;

	/*
	CString FilePath;
	FilePath.Format("%slc\\%s.cis",LPCSTR(g_WSXHStr),m_pLcIdxSet[lIndexNo].Code);
	FILE *fpt = fopen(LPCSTR(FilePath),"rb");
	if(fpt==NULL) return FALSE;

	LcIdxStkNum = on_filelength(fpt);
	if(LcIdxStkNum<=0 || LcIdxStkNum%sizeof(LCIDXSTK)!=0)
	{
		fclose(fpt);
		LcIdxStkNum = 0;
		return FALSE;
	}
	LcIdxStkNum /= sizeof(LCIDXSTK);
	NEWLCIDXSPACE(bRet,pLcIdxStk,LcIdxStkSpc,LcIdxStkNum,INDEXSTKSPACE_STEP,LCIDXSTK);
	if(!bRet)
	{
		fclose(fpt);
		pLcIdxStk	= NULL;
		LcIdxStkNum	= 0;
		LcIdxStkSpc	= 0;
		return FALSE;
	}
	fseek(fpt,0,SEEK_SET);
	LcIdxStkNum = fread(pLcIdxStk,sizeof(LCIDXSTK),LcIdxStkNum,fpt);
	fclose(fpt);
	return TRUE;
	*/
	short lpnUsedStkIndex[MAXGPNUM] = {-1};
	long nStockNum = FiltZJHHYCode(lIndexNo, lpnUsedStkIndex);
//	long nStockNum = FiltLCCode(lIndexNo, lpnUsedStkIndex);
	if(nStockNum<1) return FALSE;
	NEWLCIDXSPACE(bRet,pLcIdxStk,LcIdxStkSpc,nStockNum,INDEXSTKSPACE_STEP,LCIDXSTK);
	if(!bRet)
	{
		pLcIdxStk	= NULL;
		LcIdxStkNum	= 0;
		LcIdxStkSpc	= 0;
		return FALSE;
	}
	LcIdxStkNum = 0;
	for(int i=0;i<nStockNum;i++)
	{
		MemStkInfo *pInfo = (*g_pStockIO)[lpnUsedStkIndex[i]];
		if(!pInfo) continue;

		memset(&pLcIdxStk[LcIdxStkNum], 0, sizeof(pLcIdxStk[LcIdxStkNum]));
		pLcIdxStk[LcIdxStkNum].nSetCode = pInfo->setcode;
		strncpy(pLcIdxStk[LcIdxStkNum].Code, pInfo->Code, CODE_SIZE-1);
		++LcIdxStkNum;
	}
	return LcIdxStkNum>0;
}

BOOL LcIdxMng::SaveIdxStk(LPLCIDXSTK pLcIdxStk,long LcIdxStkNum,LPLCIDXSET pLcIdxSet,LPLCIDXSET pOldLcIdxSet)
{
	if(pLcIdxSet==NULL) return FALSE;
	if(pLcIdxStk==NULL || LcIdxStkNum<=0) return FALSE;
	CString FilePath;
	FilePath.Format("%slc\\%s.cis",LPCSTR(g_WSXHStr),pLcIdxSet->Code);
	FILE *fpt = fopen(LPCSTR(FilePath),"wb");
	if(fpt==NULL) return FALSE;
	fwrite(pLcIdxStk,LcIdxStkNum*sizeof(LCIDXSTK),1,fpt);
	fclose(fpt);
	if(pOldLcIdxSet && strcmp(pLcIdxSet->Code,pOldLcIdxSet->Code)!=0)	//需要删除老的
	{
		FilePath.Format("%slc\\%s.cis",LPCSTR(g_WSXHStr),pOldLcIdxSet->Code);
		unlink(FilePath);
	}
	return TRUE;
}

void LcIdxMng::DelIdxStk(long lIndexNo)
{
	if(lIndexNo<0 || lIndexNo>=m_LcIdxSetNum || m_pLcIdxSet==NULL) return;
	CString FilePath;
	FilePath.Format("%slc\\%s.cis",LPCSTR(g_WSXHStr),m_pLcIdxSet[lIndexNo].Code);
	unlink(FilePath);
}

long LcIdxMng::GetIdxNum()
{
	return m_LcIdxSetNum;
}

long LcIdxMng::GetIdxSet(LPLCIDXSET &pLcIdxSet)
{
	pLcIdxSet = m_pLcIdxSet;
	return m_LcIdxSetNum;
}

BOOL LcIdxMng::AddLcIndex(LPLCIDXSET pLcIdxSet,LPLCIDXSTK pLcIdxStk,long LcIdxStkNum)
{
	BOOL bRet = TRUE;
	LPLCIDXSET pTmpLcIdxSet = NULL;
	CHECKLCIDXSPACE(bRet,m_pLcIdxSet,pTmpLcIdxSet,m_LcIdxSetSpc,(m_LcIdxSetNum+1),INDEXSETSPACE_STEP,LCIDXSET);
	if(!bRet) return FALSE;
	memcpy(&(m_pLcIdxSet[m_LcIdxSetNum]),pLcIdxSet,sizeof(LCIDXSET));
	m_pLcIdxSet[m_LcIdxSetNum].nUnitNum = LcIdxStkNum;
	if(SaveIdxStk(pLcIdxStk,LcIdxStkNum,&(m_pLcIdxSet[m_LcIdxSetNum])))
		m_LcIdxSetNum++;
	return SaveIdxSet();
}

BOOL LcIdxMng::ChgLcIndex(LPLCIDXSET pLcIdxSet,long lIndexNo,LPLCIDXSTK pLcIdxStk,long LcIdxStkNum)
{
	if(!SaveIdxStk(pLcIdxStk,LcIdxStkNum,pLcIdxSet,&(m_pLcIdxSet[m_LcIdxSetNum])))
		return FALSE;
	memcpy(&(m_pLcIdxSet[lIndexNo]),pLcIdxSet,sizeof(LCIDXSET));
	m_pLcIdxSet[lIndexNo].nUnitNum = LcIdxStkNum;
	return SaveIdxSet();
}

BOOL LcIdxMng::DelLcIndex(long lIndexNo)
{
	if(lIndexNo<0 || lIndexNo>=m_LcIdxSetNum || m_pLcIdxSet==NULL) return FALSE;
	DelIdxStk(lIndexNo);
	if(lIndexNo<m_LcIdxSetNum-1)
		memmove(&(m_pLcIdxSet[lIndexNo]),&(m_pLcIdxSet[lIndexNo+1]),(m_LcIdxSetNum-1-lIndexNo)*sizeof(LCIDXSET));
	m_LcIdxSetNum--;
	memset(&(m_pLcIdxSet[m_LcIdxSetNum]),0,sizeof(LCIDXSET));
	return SaveIdxSet();
}

BOOL LcIdxMng::TestIndexExist(char *pIdxCode,long lIdxNo)
{
	if(pIdxCode==NULL) return FALSE;
	if(m_pLcIdxSet==NULL) return FALSE;
	long i = 0;
	for(i=0;i<m_LcIdxSetNum;i++)
	{
		if(i==lIdxNo) continue;		//不检查自身
		if(strncmp(pIdxCode,m_pLcIdxSet[i].Code,CODE_SIZE-1)==0)
			return TRUE;
	}
	return FALSE;
}

BOOL LcIdxMng::TestLCIndex(char *pIdxCode,long &lIdxNo)
{
	if(pIdxCode==NULL) return FALSE;
	if(m_pLcIdxSet==NULL) return FALSE;
	long i = 0;
	for(i=0;i<m_LcIdxSetNum;i++)
	{
		if(strncmp(pIdxCode,m_pLcIdxSet[i].Code,CODE_SIZE-1)==0)
		{
			lIdxNo = i;
			return TRUE;
		}
	}
	lIdxNo = -1;
	return FALSE;
}

void LcIdxMng::CheckReCalc(CWnd *pView,char *pIdxCode)
{
	if(m_pLcIdxSet==NULL || pIdxCode==NULL) return;
	long i = 0;
	for(i=0;i<m_LcIdxSetNum;i++)
	{
		if(strncmp(pIdxCode,m_pLcIdxSet[i].Code,CODE_SIZE-1)==0)
		{
			ReCalc(i);
			return;
		}
	}
}

void LcIdxMng::SetNeedReCalc()
{
	if(m_pLcIdxSet==NULL) return;
	long i = 0;
	for(i=0;i<m_LcIdxSetNum;i++)
		m_pLcIdxSet[i].nRefreshedTime = 0;
}


long LcIdxMng::GetLcNum()
{
	return m_LcIdxSetNum+m_LcExtSetNum+m_LcPtSetNum;
}

#include "ProgressWnd.h"
BOOL LcIdxMng::ReCalcIndex(long lIndexNo)
{
	if(lIndexNo<0 || lIndexNo>=m_LcIdxSetNum || m_pLcIdxSet==NULL) return FALSE;
	TDELA(m_pLcIdxStk);
	if(!GetIdxStk(m_pLcIdxStk,m_LcIdxStkNum,m_LcIdxStkSpc,lIndexNo))
		return FALSE;
	//最大天数,只可能多不可能少
	m_MaxDays = g_CF.DateToDay(g_iToday)-g_CF.DateToDay(19901219)+1;
	if(m_MaxDays<=0 || m_MaxDays>=100000) return FALSE;
	BOOL bRet = TRUE;
	long i=0,j=0,k=0;
	long tmpDate = 19901219;
	CString FilePath;
	FILE *fpt = NULL;
	long xsflag = XS2;
	double xsrate = 0.01;
	double fBaseCap = 1.0;
	NEWLCIDXSPACE(bRet,m_pLcIdxDay,m_LcIdxDaySpc,m_MaxDays,INDEXDAYSPACE_STEP,LCIDXDAY);
	NEWLCIDXSPACE(bRet,m_pIndexAnalyData,m_IndexAnalySpc,m_MaxDays,INDEXDAYSPACE_STEP,ANALYDATA);
	NEWLCIDXSPACE(bRet,m_pLcStkDay,m_LcStkDaySpc,m_MaxDays,INDEXDAYSPACE_STEP,LCIDXDAY);
	NEWLCIDXSPACE(bRet,m_pStockAnalyData,m_StockAnalySpc,m_MaxDays,INDEXDAYSPACE_STEP,ANALYDATA);
	if(!bRet) return FALSE;
	//初始化指数,完全按照自然天处理,如果有差异,就是数据问题
	if(m_pLcIdxSet[lIndexNo].BaseDate<19901219)
		m_pLcIdxSet[lIndexNo].BaseDate = 19901219;
	memset(m_pLcIdxDay,0,m_LcIdxDaySpc*sizeof(LCIDXDAY));
	for(i=0;i<m_MaxDays;i++)
	{
		m_pLcIdxDay[i].lDate	= tmpDate;
		tmpDate = g_CF.DateOffset(tmpDate,1,FALSE);
	}
	for(i=0;i<m_MaxDays;i++)
	{
		if(m_pLcIdxDay[i].lDate+366>=m_pLcIdxSet[lIndexNo].BaseDate)
			break;
	}
	if(i>0)
	{
		memmove(&(m_pLcIdxDay[0]),&(m_pLcIdxDay[i]),(m_MaxDays-i)*sizeof(LCIDXDAY));
		m_MaxDays -= i;
	}
	if(m_MaxDays<=0) return FALSE;
	
	/*
	CString tmpStr;
	tmpStr.Format(_F("正在计算%s，请稍等..."),m_pLcIdxSet[lIndexNo].Code);
	CProgressWnd wndProgress(NULL,tmpStr,TRUE);
	wndProgress.GoModal();
	wndProgress.SetRange(0,m_LcIdxStkNum);
	wndProgress.SetStep(1);
	*/
	
	//修改策略为:一切数据按照自然日来排列,无成交量的剔除
	for(i=0;i<m_LcIdxStkNum;i++)
	{
		/*
		wndProgress.SetText(m_pLcIdxStk[i].Code);
		wndProgress.StepIt();
		if(i%10==0)
		{
			wndProgress.PeekAndPump();
			if(wndProgress.Cancelled())
			{
				TDX_MessageBox(NULL,"计算被取消！",MB_OK|MB_ICONINFORMATION);
				break;
			}
		}
		*/
		
		PeekMsg();
		//读磁盘文件
		m_pCurStock = g_pStockIO->Get_MemStkInfo(m_pLcIdxStk[i].Code,m_pLcIdxStk[i].nSetCode);
		if(m_pCurStock == NULL) continue;
		FilePath.Format("%s%s\\day\\%s%s.day",g_TdxCfg.DocPath,GetSetStr(m_pLcIdxStk[i].nSetCode),GetSetStr(m_pLcIdxStk[i].nSetCode),m_pLcIdxStk[i].Code);
		g_HisDataReader.Lock();
		fpt = fopen(LPCSTR(FilePath),"rb");
		if(fpt==NULL) 
		{
			g_HisDataReader.Unlock();
			continue;
		}
		m_StockAnalyNum = on_filelength(fpt);
		if(m_StockAnalyNum<=0 || m_StockAnalyNum%sizeof(ANALYDATA)!=0)
		{
			fclose(fpt);
			g_HisDataReader.Unlock();
			m_StockAnalyNum = 0;
			continue;
		}
		m_StockAnalyNum /= sizeof(ANALYDATA);
		fseek(fpt,0,SEEK_SET);
		m_StockAnalyNum = fread(m_pStockAnalyData,sizeof(ANALYDATA),m_StockAnalyNum,fpt);
		fclose(fpt);
		g_HisDataReader.Unlock();
		PeekMsg();
		//数据作一次转移
		memset(m_pLcStkDay,0,m_MaxDays*sizeof(LCIDXDAY));
		for(j=0;j<m_MaxDays;j++)
		{
			m_pLcStkDay[j].lDate = m_pLcIdxDay[j].lDate;
		}
		xsflag = GetXSFlag(m_pCurStock);
		switch(xsflag)
		{
		case 0:	xsrate = 1;		break;
		case 1:	xsrate = 0.1;	break;
		case 2:	xsrate = 0.01;	break;
		case 3:	xsrate = 0.001;	break;
		default:xsrate = 0.01;	break;
		}
		for(j=0,k=0;j<m_MaxDays && k<m_StockAnalyNum;)
		{
			if(m_pLcStkDay[j].lDate > m_pStockAnalyData[k].Time.Date)
			{
				if(k<=m_pLcIdxSet[lIndexNo].lGiveupDays)
				{
					m_pLcStkDay[j].fOpen	= 0;
					m_pLcStkDay[j].fMax		= 0;
					m_pLcStkDay[j].fMin		= 0;
					m_pLcStkDay[j].fClose	= 0;
					m_pLcStkDay[j].fYClose	= 0;
					if(k<m_StockAnalyNum-1) k++;
					else j++;
				}
				else
				{
					m_pLcStkDay[j].fOpen	= m_pStockAnalyData[k].l_Close*xsrate;
					m_pLcStkDay[j].fMax		= m_pStockAnalyData[k].l_Close*xsrate;
					m_pLcStkDay[j].fMin		= m_pStockAnalyData[k].l_Close*xsrate;
					m_pLcStkDay[j].fClose	= m_pStockAnalyData[k].l_Close*xsrate;
					m_pLcStkDay[j].fYClose	= m_pStockAnalyData[k].l_Close*xsrate;
					if(k<m_StockAnalyNum-1) k++;
					else j++;
				}
			}
			else if(m_pLcStkDay[j].lDate < m_pStockAnalyData[k].Time.Date)
			{
				if(k<=m_pLcIdxSet[lIndexNo].lGiveupDays)
				{
					m_pLcStkDay[j].fOpen	= 0;
					m_pLcStkDay[j].fMax		= 0;
					m_pLcStkDay[j].fMin		= 0;
					m_pLcStkDay[j].fClose	= 0;
					m_pLcStkDay[j].fYClose	= 0;
				}
				else
				{
					if(j==0)
					{
						m_pLcStkDay[j].fOpen	= m_pStockAnalyData[k-1].l_Close*xsrate;
						m_pLcStkDay[j].fMax		= m_pStockAnalyData[k-1].l_Close*xsrate;
						m_pLcStkDay[j].fMin		= m_pStockAnalyData[k-1].l_Close*xsrate;
						m_pLcStkDay[j].fClose	= m_pStockAnalyData[k-1].l_Close*xsrate;
						m_pLcStkDay[j].fYClose	= m_pStockAnalyData[k-1].l_Close*xsrate;
					}
					else
					{
						m_pLcStkDay[j].fOpen	= m_pLcStkDay[j-1].fClose;
						m_pLcStkDay[j].fMax		= m_pLcStkDay[j-1].fClose;
						m_pLcStkDay[j].fMin		= m_pLcStkDay[j-1].fClose;
						m_pLcStkDay[j].fClose	= m_pLcStkDay[j-1].fClose;
						m_pLcStkDay[j].fYClose	= m_pLcStkDay[j-1].fClose;
					}
				}
				j++;
			}
			else
			{
				if(k<=m_pLcIdxSet[lIndexNo].lGiveupDays)
				{
					m_pLcStkDay[j].fOpen	= 0;
					m_pLcStkDay[j].fMax		= 0;
					m_pLcStkDay[j].fMin		= 0;
					m_pLcStkDay[j].fClose	= 0;
					m_pLcStkDay[j].fYClose	= 0;
				}
				else
				{
					m_pLcStkDay[j].fOpen	= m_pStockAnalyData[k].l_Open*xsrate;
					m_pLcStkDay[j].fMax		= m_pStockAnalyData[k].l_High*xsrate;
					m_pLcStkDay[j].fMin		= m_pStockAnalyData[k].l_Low*xsrate;
					m_pLcStkDay[j].fClose	= m_pStockAnalyData[k].l_Close*xsrate;
					m_pLcStkDay[j].fYClose	= m_pStockAnalyData[k-1].l_Close*xsrate;
					m_pLcStkDay[j].fVolume	= m_pStockAnalyData[k].Volume;
					m_pLcStkDay[j].fAmount	= m_pStockAnalyData[k].Amount;
				}
				j++;
				if(k<m_StockAnalyNum-1) k++;
			}
		}
		m_LcStkDayNum = m_MaxDays;

		PeekMsg();
		//除权除息,填写昨收调整值,填写流通股本总股本,权重
		if(m_pCwcl)
		{
			m_lCWDataNum = m_pCwcl->Init_ZGB_LTGB();
			if(m_lCWDataNum>0)
			{
				NEWLCIDXSPACE(bRet,m_pCWData,m_lCWDataSpc,m_lCWDataNum,CWDATASPACE_STEP,CWDATA);
				m_lCWDataNum = m_pCwcl->GetTQInfo(m_pCurStock,m_pCWData,m_lCWDataSpc);
				//对股价造成影响:现金分红,送红股,公积金转增股本,配股,流通股扩股
				//对股本造成影响:直接读取变动前后的数值
				//
				//对于按照主要指数编制方法计算的,一律使用等比除权,前复权和后复权相同(结果为市值,不是股价),为方便起见,使用等比前复权
				//对于同等权重,需要逐步处理(最近比较烦)
				//这就是core......
				if(!TQData(&(m_pLcIdxSet[lIndexNo]),m_pCurStock,m_pLcStkDay,m_LcStkDayNum,m_pCWData,m_lCWDataNum))
					continue;
			}
		}
		PeekMsg();
		for(j=0;j<m_MaxDays;j++)
		{
			m_pLcIdxDay[j].fOpen	+= m_pLcStkDay[j].fOpen;
			m_pLcIdxDay[j].fMax		+= m_pLcStkDay[j].fMax;
			m_pLcIdxDay[j].fMin		+= m_pLcStkDay[j].fMin;
			m_pLcIdxDay[j].fClose	+= m_pLcStkDay[j].fClose;
			m_pLcIdxDay[j].fYClose	+= m_pLcStkDay[j].fYClose;
			m_pLcIdxDay[j].fVolume	+= m_pLcStkDay[j].fVolume;
			m_pLcIdxDay[j].fAmount	+= m_pLcStkDay[j].fAmount;
			m_pLcIdxDay[j].fFlowCap	+= m_pLcStkDay[j].fFlowCap;
			m_pLcIdxDay[j].fTotalCap+= m_pLcStkDay[j].fTotalCap;
			if(j>0)
			{
				if(m_pLcStkDay[j].fClose > m_pLcStkDay[j-1].fClose+COMPPREC)
					m_pLcIdxDay[k].wAdvance++;
				else if(m_pLcStkDay[j].fClose < m_pLcStkDay[j-1].fClose-COMPPREC)
					m_pLcIdxDay[k].wDecline++;
			}
		}
	}
	//日期处理
	for(i=0,j=0;i<m_MaxDays;i++)
	{
		if(m_pLcIdxDay[i].fVolume<COMPPREC) continue;
		//在基准日之前,只允许有一个交易日
		if(m_pLcIdxDay[i].lDate<=m_pLcIdxSet[lIndexNo].BaseDate)
		{
			if(i>0) memcpy(&(m_pLcIdxDay[0]),&(m_pLcIdxDay[i]),sizeof(LCIDXDAY));
			j = 1;
			continue;
		}
		if(i>j)
		{
			memcpy(&(m_pLcIdxDay[j]),&(m_pLcIdxDay[i]),sizeof(LCIDXDAY));
			j++;
		}
	}
	m_LcIdxDayNum = j;
	//
	fBaseCap = 1.0;
	for(j=1;j<m_LcIdxDayNum;j++)
	{
		if(m_pLcIdxDay[j-1].fClose>COMPPREC && m_pLcIdxDay[j].fYClose>COMPPREC)
			fBaseCap = m_pLcIdxDay[j-1].fClose/m_pLcIdxDay[j].fYClose;
		m_pLcIdxDay[j].fOpen	*= fBaseCap;
		m_pLcIdxDay[j].fMax		*= fBaseCap;
		m_pLcIdxDay[j].fMin		*= fBaseCap;
		m_pLcIdxDay[j].fClose	*= fBaseCap;
	}
	//转换,存盘
	if(m_LcIdxDayNum>0)
	{
		//基准值
		if(m_pLcIdxDay[0].lDate<=m_pLcIdxSet[lIndexNo].BaseDate)
			fBaseCap = m_pLcIdxDay[0].fClose;
		else fBaseCap = m_pLcIdxDay[0].fOpen;
		for(i=0,j=0;i<m_LcIdxDayNum;i++)
		{
			if(m_pLcIdxDay[i].lDate<=m_pLcIdxSet[lIndexNo].BaseDate)
				continue;
			m_pIndexAnalyData[j].Time.Date	= m_pLcIdxDay[i].lDate;
			m_pIndexAnalyData[j].l_Open	= DWORD(m_pLcIdxDay[i].fOpen/fBaseCap*(double)m_pLcIdxSet[lIndexNo].BaseValue*100.0+0.5);
			m_pIndexAnalyData[j].l_High	= DWORD(m_pLcIdxDay[i].fMax/fBaseCap*(double)m_pLcIdxSet[lIndexNo].BaseValue*100.0+0.5);
			m_pIndexAnalyData[j].l_Low	= DWORD(m_pLcIdxDay[i].fMin/fBaseCap*(double)m_pLcIdxSet[lIndexNo].BaseValue*100.0+0.5);
			m_pIndexAnalyData[j].l_Close= DWORD(m_pLcIdxDay[i].fClose/fBaseCap*(double)m_pLcIdxSet[lIndexNo].BaseValue*100.0+0.5);
			m_pIndexAnalyData[j].Amount	= m_pLcIdxDay[i].fAmount;
			m_pIndexAnalyData[j].Volume	= DWORD(m_pLcIdxDay[i].fVolume/100);
			m_pIndexAnalyData[j].zd.up	= WORD(m_pLcIdxDay[k].wAdvance);
			m_pIndexAnalyData[j].zd.down= WORD(m_pLcIdxDay[k].wDecline);
			j++;
		}
		m_LcIdxDayNum = j;
		FilePath.Format("%s%s\\day\\%s%s.day",g_TdxCfg.DocPath,GetSetStr(SZ),GetSetStr(SZ),m_pLcIdxSet[lIndexNo].Code);
		g_HisDataReader.Lock();
		fpt = fopen(LPCSTR(FilePath),"wb");
		if(fpt!=NULL)
		{
			fwrite(m_pIndexAnalyData,sizeof(ANALYDATA),m_LcIdxDayNum,fpt);
			fclose(fpt);
		}
		g_HisDataReader.Unlock();
		PeekMsg();
		//存盘行情
		CURRDATA iData = {0};
		MemStkInfo *pIdxInfo = g_pStockIO->Get_MemStkInfo(m_pLcIdxSet[lIndexNo].Code, SZ);
		if(pIdxInfo)
		{
			g_pStockIO->ReadCurrStockData(pIdxInfo, &iData);
			iData.Open = m_pIndexAnalyData[m_LcIdxDayNum-1].l_Open/100.0f;
			iData.Max = m_pIndexAnalyData[m_LcIdxDayNum-1].l_High/100.0f;
			iData.Min = m_pIndexAnalyData[m_LcIdxDayNum-1].l_Low/100.0f;
			iData.Now = m_pIndexAnalyData[m_LcIdxDayNum-1].l_Close/100.0f;
			if(m_LcIdxDayNum>1) 
				iData.Close = m_pIndexAnalyData[m_LcIdxDayNum-2].l_Close/100.0f;
			else iData.Close = iData.Open;
			iData.Amount = m_pIndexAnalyData[m_LcIdxDayNum-1].Amount;
			iData.Volume = m_pIndexAnalyData[m_LcIdxDayNum-1].Volume;
			iData.Buyv[0] = m_pIndexAnalyData[m_LcIdxDayNum-1].zd.up;
			iData.Sellv[0] = m_pIndexAnalyData[m_LcIdxDayNum-1].zd.down;
			g_pStockIO->SetCurrStockData(pIdxInfo->setcode, pIdxInfo->Index, (char *)(&(iData)));
		}
	}
	
	return TRUE;
}

BOOL LcIdxMng::TQData(LPLCIDXSET pLcIdxSet,MemStkInfo *pCurStock,LPLCIDXDAY pLcStkDay,long lLcStkDayNum,LPCWDATA pCWData,long lCWDataNum)
{
	if(pCurStock==NULL) return FALSE;
	long i=0,j=0,k=0;
	float fFlowCap	= 0;
	float fTotalCap	= 0;
	long lTQDate	= 0;
	long lTQStartNo	= 0;
	double fTotalRate= 0;
	double fCloseBak	= 0;
	float fTQClose1	= 0;
	float fTQClose2	= 0;
	//
	for(i=0;i<lCWDataNum;i++)
	{
		switch(pCWData[i].Type)
		{
		case CWCL_TYPE_SEND2OUT:
		case CWCL_TYPE_IN2OUT:
		case CWCL_TYPE_OTHERCAPCHG:
		case CWCL_TYPE_BUYBACK:
		case CWCL_TYPE_NEW2OUT:
		case CWCL_TYPE_RESERVES2OUT:
		case CWCL_TYPE_CONVERT2OUT:
			if(fFlowCap<COMPPREC && pCWData[i].B01>1.000) fFlowCap = pCWData[i].B01;
			if(fTotalCap<COMPPREC && pCWData[i].B02>1.000) fTotalCap = pCWData[i].B02;
			break;
		default:	break;
		}
		if(fFlowCap>COMPPREC && fTotalCap>COMPPREC)
			break;
	}
	if(fFlowCap<COMPPREC) fFlowCap = pCurStock->ActiveCapital;
	if(fTotalCap<COMPPREC) fTotalCap = pCurStock->J_zgb;
	//填写流通股本总股本
	if(lCWDataNum<=0)
	{
		for(j=0;j<lLcStkDayNum;j++)
		{
			pLcStkDay[j].fFlowCap	= fFlowCap;
			pLcStkDay[j].fTotalCap	= fTotalCap;
		}
	}
	else
	{
		for(i=0,j=0;i<lCWDataNum && j<lLcStkDayNum;)
		{
			switch(pCWData[i].Type)
			{
			case CWCL_TYPE_SEND2OUT:
			case CWCL_TYPE_IN2OUT:
			case CWCL_TYPE_OTHERCAPCHG:
			case CWCL_TYPE_BUYBACK:
			case CWCL_TYPE_NEW2OUT:
			case CWCL_TYPE_RESERVES2OUT:
			case CWCL_TYPE_CONVERT2OUT:
				if(pLcStkDay[j].lDate>pCWData[i].Date)
				{
					if(i<lCWDataNum-1)
					{
						if(pCWData[i].B03>1.000) fFlowCap = pCWData[i].B03;
						if(pCWData[i].B04>1.000) fTotalCap = pCWData[i].B04;
						i++;
						continue;
					}
				}
				else if(pLcStkDay[j].lDate==pCWData[i].Date)
				{
					if(pCWData[i].B03>1.000) fFlowCap = pCWData[i].B03;
					if(pCWData[i].B04>1.000) fTotalCap = pCWData[i].B04;
					if(i<lCWDataNum-1) i++;
				}
				break;
			default:
				if(i<lCWDataNum-1)
				{
					i++;
					continue;
				}
				break;
			}
			pLcStkDay[j].fFlowCap	= fFlowCap;
			pLcStkDay[j].fTotalCap	= fTotalCap;
			j++;
		}
	}
	//进行昨收价格除权,非等比复权的,直接计算最后结果
	switch(pLcIdxSet->QZType)
	{
	case LCIDXWEIGHT_FLOWCAP:
	case LCIDXWEIGHT_ADJUSTCAP:
	case LCIDXWEIGHT_TOTALCAP:
		//处理:配股,送红股,公积金转增,扩股
		for(i=0,j=0;i<lCWDataNum && j<lLcStkDayNum;)
		{
			switch(pCWData[i].Type)
			{
			case CWCL_TYPE_DR:
				if(pLcStkDay[j].lDate>pCWData[i].Date)
				{
					if(j==0 || pLcStkDay[j-1].lDate<pCWData[i].Date)
					{
						pLcStkDay[j].fYClose = (10.0*pLcStkDay[j].fYClose+pCWData[i].B04*pCWData[i].B02)/(10.0+pCWData[i].B03+pCWData[i].B04);
						i++;
					}
					else j++;
				}
				else if(pLcStkDay[j].lDate==pCWData[i].Date)
				{
					pLcStkDay[j].fYClose = (10.0*pLcStkDay[j].fYClose+pCWData[i].B04*pCWData[i].B02)/(10.0+pCWData[i].B03+pCWData[i].B04);
					i++;
				}
				else j++;
				break;
			case CWCL_TYPE_INPROCHG:
				if(pLcStkDay[j].lDate>pCWData[i].Date)
				{
					if(j==0 || pLcStkDay[j-1].lDate<pCWData[i].Date)
					{
						if(pCWData[i].B03>1) pLcStkDay[j].fYClose /= pCWData[i].B03;
						i++;
					}
					else j++;
				}
				else if(pLcStkDay[j].lDate==pCWData[i].Date)
				{
					if(pCWData[i].B03>1) pLcStkDay[j].fYClose /= pCWData[i].B03;
					i++;
				}
				else j++;
				break;
			default:
				i++;
				break;
			}
		}
		break;
	case LCIDXWEIGHT_PRICE_NOTQ:
		break;
	case LCIDXWEIGHT_PRICE_EFTQ:
	case LCIDXWEIGHT_PRICE_EBTQ:
	case LCIDXWEIGHT_SAME:
		//处理:现金分红,配股,送红股,公积金转增,扩股
		for(i=0,j=0;i<lCWDataNum && j<lLcStkDayNum;)
		{
			switch(pCWData[i].Type)
			{
			case CWCL_TYPE_DR:
				if(pLcStkDay[j].lDate>pCWData[i].Date)
				{
					if(j==0 || pLcStkDay[j-1].lDate<pCWData[i].Date)
					{
						pLcStkDay[j].fYClose = (10.0*pLcStkDay[j].fYClose-pCWData[i].B01+pCWData[i].B04*pCWData[i].B02)/(10.0+pCWData[i].B03+pCWData[i].B04);
						i++;
					}
					else j++;
				}
				else if(pLcStkDay[j].lDate==pCWData[i].Date)
				{
					pLcStkDay[j].fYClose = (10.0*pLcStkDay[j].fYClose-pCWData[i].B01+pCWData[i].B04*pCWData[i].B02)/(10.0+pCWData[i].B03+pCWData[i].B04);
					i++;
				}
				else j++;
				break;
			case CWCL_TYPE_INPROCHG:
				if(pLcStkDay[j].lDate>pCWData[i].Date)
				{
					if(j==0 || pLcStkDay[j-1].lDate<pCWData[i].Date)
					{
						if(pCWData[i].B03>1) pLcStkDay[j].fYClose /= pCWData[i].B03;
						i++;
					}
					else j++;
				}
				else if(pLcStkDay[j].lDate==pCWData[i].Date)
				{
					if(pCWData[i].B03>1) pLcStkDay[j].fYClose /= pCWData[i].B03;
					i++;
				}
				else j++;
				break;
			default:
				i++;
				break;
			}
		}
		break;
	}
	//计算最后加总的数值
	fTotalRate	= 1.0;
	fCloseBak	= 0.0;
	switch(pLcIdxSet->QZType)
	{
	case LCIDXWEIGHT_FLOWCAP:
		for(j=0;j<lLcStkDayNum;j++)
		{
			fTotalRate = pLcStkDay[j].fFlowCap;
			pLcStkDay[j].fOpen	*= fTotalRate;
			pLcStkDay[j].fMax	*= fTotalRate;
			pLcStkDay[j].fMin	*= fTotalRate;
			pLcStkDay[j].fClose	*= fTotalRate;
			pLcStkDay[j].fYClose*= fTotalRate;
		}
		break;
	case LCIDXWEIGHT_ADJUSTCAP:
		for(j=0;j<lLcStkDayNum;j++)
		{
			if(pLcStkDay[j].fTotalCap>COMPPREC) fTotalRate = pLcStkDay[j].fFlowCap/pLcStkDay[j].fTotalCap;
			else fTotalRate = 1;
			if(fTotalRate>0.8)		fTotalRate = 1;
			else if(fTotalRate>0.7)	fTotalRate = 0.8;
			else if(fTotalRate>0.6)	fTotalRate = 0.7;
			else if(fTotalRate>0.5)	fTotalRate = 0.6;
			else if(fTotalRate>0.4)	fTotalRate = 0.5;
			else if(fTotalRate>0.3)	fTotalRate = 0.4;
			else if(fTotalRate>0.2)	fTotalRate = 0.3;
			else if(fTotalRate>0.1)	fTotalRate = 0.2;
			else if(fTotalRate<0)	fTotalRate = 0;
			fTotalRate *= pLcStkDay[j].fTotalCap;
			pLcStkDay[j].fOpen	*= fTotalRate;
			pLcStkDay[j].fMax	*= fTotalRate;
			pLcStkDay[j].fMin	*= fTotalRate;
			pLcStkDay[j].fClose	*= fTotalRate;
			pLcStkDay[j].fYClose*= fTotalRate;
		}
		break;
	case LCIDXWEIGHT_TOTALCAP:
		for(j=0;j<lLcStkDayNum;j++)
		{
			fTotalRate = pLcStkDay[j].fTotalCap;
			pLcStkDay[j].fOpen	*= fTotalRate;
			pLcStkDay[j].fMax	*= fTotalRate;
			pLcStkDay[j].fMin	*= fTotalRate;
			pLcStkDay[j].fClose	*= fTotalRate;
			pLcStkDay[j].fYClose*= fTotalRate;
		}
		break;
	case LCIDXWEIGHT_PRICE_NOTQ:
		break;
	case LCIDXWEIGHT_PRICE_EFTQ:
		if(lLcStkDayNum>0) fCloseBak = pLcStkDay[0].fClose;
		for(j=1;j<lLcStkDayNum;j++)
		{
			if(pLcStkDay[j].fYClose>COMPPREC && fCloseBak>COMPPREC)
				fTotalRate *= (fCloseBak/pLcStkDay[j].fYClose);
			fCloseBak = pLcStkDay[j].fClose;
			pLcStkDay[j].fOpen	*= fTotalRate;
			pLcStkDay[j].fMax	*= fTotalRate;
			pLcStkDay[j].fMin	*= fTotalRate;
			pLcStkDay[j].fClose	*= fTotalRate;
			pLcStkDay[j].fYClose = pLcStkDay[j-1].fClose;
		}
		break;
	case LCIDXWEIGHT_PRICE_EBTQ:
		for(j=lLcStkDayNum-2;j>=0;j--)
		{
			if(pLcStkDay[j+1].fYClose>COMPPREC && pLcStkDay[j].fClose>COMPPREC)
				fTotalRate *= (pLcStkDay[j+1].fYClose/pLcStkDay[j].fClose);
			pLcStkDay[j].fOpen	*= fTotalRate;
			pLcStkDay[j].fMax	*= fTotalRate;
			pLcStkDay[j].fMin	*= fTotalRate;
			pLcStkDay[j].fClose	*= fTotalRate;
			pLcStkDay[j+1].fYClose = pLcStkDay[j].fClose;
		}
		break;
	case LCIDXWEIGHT_SAME:
		if(lLcStkDayNum>0)
		{
			fCloseBak = pLcStkDay[0].fClose;
			fTQClose1 = pLcStkDay[0].fClose;
			fTQClose2 = pLcStkDay[0].fClose;
			if(pLcStkDay[0].fYClose>COMPPREC)
			{
				pLcStkDay[j].fOpen		/= pLcStkDay[j].fYClose;
				pLcStkDay[j].fMax		/= pLcStkDay[j].fYClose;
				pLcStkDay[j].fMin		/= pLcStkDay[j].fYClose;
				pLcStkDay[j].fClose		/= pLcStkDay[j].fYClose;
				pLcStkDay[j].fYClose	= 1;
			}
			else
			{
				pLcStkDay[0].fOpen		= 1;
				pLcStkDay[0].fMax		= 1;
				pLcStkDay[0].fMin		= 1;
				pLcStkDay[0].fClose		= 1;
				pLcStkDay[0].fYClose	= 1;
			}
		}
		for(j=1;j<lLcStkDayNum;j++)
		{
			//
			if(pLcStkDay[j].fYClose>COMPPREC)
			{
				pLcStkDay[j].fOpen		/= pLcStkDay[j].fYClose;
				pLcStkDay[j].fMax		/= pLcStkDay[j].fYClose;
				pLcStkDay[j].fMin		/= pLcStkDay[j].fYClose;
				pLcStkDay[j].fClose		/= pLcStkDay[j].fYClose;
				pLcStkDay[j].fYClose	= 1;
			}
			else
			{
				pLcStkDay[j].fOpen		=
				pLcStkDay[j].fMax		=
				pLcStkDay[j].fMin		=
				pLcStkDay[j].fClose		=
				pLcStkDay[j].fYClose	= 1;
			}
		}
	default:
		break;
	}
	return TRUE;
}
