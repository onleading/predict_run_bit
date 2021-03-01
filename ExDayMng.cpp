#include "StdAfx.h"
#include "ExDayMng.h"

ExdayMng::ExdayMng()
{
	m_bInited = FALSE;

	m_pExdayData = NULL;
	m_ExdayHead = NULL;
	long i = 0;
	for(i=0;i<2;i++)
	{
		m_pExdayIndex[i]= NULL;
		memset(m_ExdayPath[i],0,MAX_PATH*sizeof(char));
		m_ExdayFile[i]	= NULL;
	}
}

ExdayMng::~ExdayMng()
{
	long i = 0;
	for(i=0;i<2;i++)
	{
		if(m_ExdayFile[i])
			fclose(m_ExdayFile[i]);
		TDELA(m_pExdayIndex[i]);
	}
	TDELA(m_pExdayData);
	TDELA(m_ExdayHead);
}

BOOL ExdayMng::Init()
{
	if(m_bInited) return TRUE;
	memset(m_ExdayPath,0,2*MAX_PATH*sizeof(char));
	long i = 0;
	if(m_ExdayHead==NULL) m_ExdayHead = new EXDAYHEAD[2];
	if(m_ExdayHead==NULL) return FALSE;
	for(i=0;i<2;i++)
	{
		if(m_pExdayIndex[i]==NULL) m_pExdayIndex[i] = new EXDAYINDEX[MAXGPNUM];
		if(m_pExdayIndex[i]==NULL) return FALSE;
		sprintf(m_ExdayPath[i],"%sLData\\%s\\eday\\%spkday.ned",HomePath,GetSetStr(i),GetSetStr(i));
		m_ExdayFile[i]	= _fsopen(m_ExdayPath[i],"r+b",SH_DENYNO);
		if(!m_ExdayFile[i]) m_ExdayFile[i] = _fsopen(m_ExdayPath[i],"w+b",SH_DENYNO);
		if(!m_ExdayFile[i]) return FALSE;
		if(m_ExdayFile[i])
		{
			long lFileLen = on_filelength(m_ExdayFile[i]);
			if(lFileLen<=sizeof(EXDAYHEAD)+MAXGPNUM*sizeof(EXDAYINDEX) || lFileLen%EXDAYBLOCK_LEN!=0)
			{
				memset(&(m_ExdayHead[i]),0,sizeof(EXDAYHEAD));
				m_ExdayHead[i].lIndexSpc = MAXGPNUM;
				m_ExdayHead[i].lBlockNum = MAXGPNUM+1;
				memset(m_pExdayIndex[i],0,MAXGPNUM*sizeof(EXDAYINDEX));
				//
				fseek(m_ExdayFile[i],0,SEEK_SET);
				fwrite(&(m_ExdayHead[i]),sizeof(EXDAYHEAD),1,m_ExdayFile[i]);
				fwrite(m_pExdayIndex[i],MAXGPNUM*sizeof(EXDAYINDEX),1,m_ExdayFile[i]);
				fseek(m_ExdayFile[i],0,SEEK_END);
			}
			else
			{
				fseek(m_ExdayFile[i],0,SEEK_SET);
				fread(&(m_ExdayHead[i]),sizeof(EXDAYHEAD),1,m_ExdayFile[i]);
				//文件头异常
				if(m_ExdayHead[i].lIndexSpc!=MAXGPNUM || m_ExdayHead[i].lStockNum>=MAXGPNUM || m_ExdayHead[i].lBlockNum != lFileLen/EXDAYBLOCK_LEN)
				{
					//这种情况应该是不存在的.简单处理,清空文件,初始化掉
					chsize(fileno(m_ExdayFile[i]),0L);
					//
					memset(&(m_ExdayHead[i]),0,sizeof(EXDAYHEAD));
					m_ExdayHead[i].lIndexSpc = MAXGPNUM;
					m_ExdayHead[i].lBlockNum = MAXGPNUM+1;
					memset(m_pExdayIndex[i],0,MAXGPNUM*sizeof(EXDAYINDEX));
					//
					fseek(m_ExdayFile[i],0,SEEK_SET);
					fwrite(&(m_ExdayHead[i]),sizeof(EXDAYHEAD),1,m_ExdayFile[i]);
					fwrite(m_pExdayIndex[i],MAXGPNUM*sizeof(EXDAYINDEX),1,m_ExdayFile[i]);
					fseek(m_ExdayFile[i],0,SEEK_END);
				}
				else									//检查文件索引
				{
					fread(m_pExdayIndex[i],MAXGPNUM*sizeof(EXDAYINDEX),1,m_ExdayFile[i]);
				}
			}
		}
	}
	m_pExdayData	= NULL;
	m_lExdayDataSpc	= 0;
	m_lExdayDataNum	= 0;
	
	memset(m_BlankBlock,0,EXDAYBLOCK_LEN*sizeof(char));
	m_bInited = TRUE;
	return TRUE;
}

long ExdayMng::ReadStkExday(LPEXDAYDATA pExday,short setcode,char *Code,long lReqNum)
{
	if(setcode!=SZ && setcode!=SH) 
		return 0;
	m_CSLock[setcode].Lock();
	if(!Init()) return 0;
	m_CSLock[setcode].Unlock();
	if(Code == NULL) return 0;
	if(m_ExdayFile[setcode] == NULL) return 0;
	long i = 0;
	long j = 0;
	long lDataNum = 0;
	for(i=0;i<m_ExdayHead[setcode].lStockNum;i++)
	{
		if(strncmp(Code,m_pExdayIndex[setcode][i].Code,SH_CODE_LEN)==0)
			break;
	}
	if(i>=m_ExdayHead[setcode].lStockNum) return 0;								//找不到股票
	lDataNum = m_pExdayIndex[setcode][i].lDataNum;
	if(pExday==NULL) return lDataNum;	//未提供缓冲区,返回数据个数
	if(lReqNum>lDataNum) lReqNum = lDataNum;
	if(lReqNum<=0) return 0;
	m_CSLock[setcode].Lock();
	//读取
	for(j=0;j<m_pExdayIndex[setcode][i].lDataBlockNum;j++)
	{
		if(EXDAYBLOCK_LEN/sizeof(EXDAYDATA)*(j+1)>lReqNum)						//数据读取结束
		{
			fseek(m_ExdayFile[setcode],m_pExdayIndex[setcode][i].lDataBlockNo[j]*EXDAYBLOCK_LEN,SEEK_SET);
			fread(&(pExday[EXDAYBLOCK_LEN/sizeof(EXDAYDATA)*j]),(lReqNum-EXDAYBLOCK_LEN/sizeof(EXDAYDATA)*j)*sizeof(EXDAYDATA),1,m_ExdayFile[setcode]);
			break;
		}
		else
		{
			fseek(m_ExdayFile[setcode],m_pExdayIndex[setcode][i].lDataBlockNo[j]*EXDAYBLOCK_LEN,SEEK_SET);
			fread(&(pExday[EXDAYBLOCK_LEN/sizeof(EXDAYDATA)*j]),EXDAYBLOCK_LEN/sizeof(EXDAYDATA)*sizeof(EXDAYDATA),1,m_ExdayFile[setcode]);
		}
	}
	m_CSLock[setcode].Unlock();
	return lReqNum;
}

BOOL ExdayMng::WriteStkExday(short setcode,long lIndex,LPEXDAYDATA pExday,long lDataNum)
{
	if(setcode!=SZ && setcode!=SH) 
		return FALSE;
	m_CSLock[setcode].Lock();
	if(!Init()) return FALSE;
	m_CSLock[setcode].Unlock();
	if(m_ExdayFile[setcode] == NULL) return FALSE;
	long i = 0;
	long j = 0;
	long k = 0;
	//先使用已经分配的数据块
	m_CSLock[setcode].Lock();
	for(i=0;i<m_pExdayIndex[setcode][lIndex].lDataBlockNum;i++)
	{
		//数据足够
		if(EXDAYBLOCK_LEN/sizeof(EXDAYDATA)*(i+1)<=lDataNum)
		{
			fseek(m_ExdayFile[setcode],m_pExdayIndex[setcode][lIndex].lDataBlockNo[i]*EXDAYBLOCK_LEN,SEEK_SET);
			fwrite(&(pExday[EXDAYBLOCK_LEN/sizeof(EXDAYDATA)*i]),EXDAYBLOCK_LEN/sizeof(EXDAYDATA)*sizeof(EXDAYDATA),1,m_ExdayFile[setcode]);
		}
		else	//空间足够
		{
			fseek(m_ExdayFile[setcode],m_pExdayIndex[setcode][lIndex].lDataBlockNo[i]*EXDAYBLOCK_LEN,SEEK_SET);
			fwrite(&(pExday[EXDAYBLOCK_LEN/sizeof(EXDAYDATA)*i]),(lDataNum-EXDAYBLOCK_LEN/sizeof(EXDAYDATA)*i)*sizeof(EXDAYDATA),1,m_ExdayFile[setcode]);
			break;
		}
	}
	if(i>=m_pExdayIndex[setcode][lIndex].lDataBlockNum)	//空间不够
	{
		while(EXDAYBLOCK_LEN/sizeof(EXDAYDATA)*m_pExdayIndex[setcode][lIndex].lDataBlockNum<lDataNum)
		{
			m_pExdayIndex[setcode][lIndex].lDataBlockNo[m_pExdayIndex[setcode][lIndex].lDataBlockNum] = m_ExdayHead[setcode].lBlockNum;
			m_ExdayHead[setcode].lBlockNum++;
			if(EXDAYBLOCK_LEN/sizeof(EXDAYDATA)*(m_pExdayIndex[setcode][lIndex].lDataBlockNum+1)<=lDataNum)
			{
				fseek(m_ExdayFile[setcode],m_pExdayIndex[setcode][lIndex].lDataBlockNo[m_pExdayIndex[setcode][lIndex].lDataBlockNum]*EXDAYBLOCK_LEN,SEEK_SET);
				fwrite(&(pExday[EXDAYBLOCK_LEN/sizeof(EXDAYDATA)*i]),EXDAYBLOCK_LEN/sizeof(EXDAYDATA)*sizeof(EXDAYDATA),1,m_ExdayFile[setcode]);
			}
			else	//空间足够
			{
				fseek(m_ExdayFile[setcode],m_pExdayIndex[setcode][lIndex].lDataBlockNo[m_pExdayIndex[setcode][lIndex].lDataBlockNum]*EXDAYBLOCK_LEN,SEEK_SET);
				fwrite(m_BlankBlock,EXDAYBLOCK_LEN,1,m_ExdayFile[setcode]);
				fseek(m_ExdayFile[setcode],0,SEEK_END);
				fseek(m_ExdayFile[setcode],m_pExdayIndex[setcode][lIndex].lDataBlockNo[m_pExdayIndex[setcode][lIndex].lDataBlockNum]*EXDAYBLOCK_LEN,SEEK_SET);
				fwrite(&(pExday[EXDAYBLOCK_LEN/sizeof(EXDAYDATA)*m_pExdayIndex[setcode][lIndex].lDataBlockNum]),
					(lDataNum-EXDAYBLOCK_LEN/sizeof(EXDAYDATA)*m_pExdayIndex[setcode][lIndex].lDataBlockNum)*sizeof(EXDAYDATA),1,m_ExdayFile[setcode]);
			}
			m_pExdayIndex[setcode][lIndex].lDataBlockNum++;
		}
		fseek(m_ExdayFile[setcode],0,SEEK_SET);
		fwrite(&(m_ExdayHead[setcode]),sizeof(EXDAYHEAD),1,m_ExdayFile[setcode]);
	}
	m_pExdayIndex[setcode][lIndex].lDataNum = lDataNum;
	fseek(m_ExdayFile[setcode],EXDAYBLOCK_LEN*(lIndex+1),SEEK_SET);
	fwrite(&(m_pExdayIndex[setcode][lIndex]),sizeof(EXDAYINDEX),1,m_ExdayFile[setcode]);
	m_CSLock[setcode].Unlock();
	return TRUE;
}

BOOL ExdayMng::WriteExdayFile(short setcode,char *pfDateDay[],long plDateDayLen[],long plDate[],long lDayNum)
{
	if(setcode!=SZ && setcode!=SH) 
		return FALSE;
	m_CSLock[setcode].Lock();
	if(!Init()) return FALSE;
	m_CSLock[setcode].Unlock();
	if(m_ExdayFile[setcode] == NULL) return 0;
	if(lDayNum == 0) return TRUE;
	long i = 0;
	long j = 0;
	long k = 0;
	long l = 0;
	long m = 0;
	BOOL bRet = TRUE;
	LPEXDAYDATA pTmpExDay = NULL;
	long lDataNum = 0;		//单个文件包含的数据个数
	BOOL bHasChange = FALSE;
	//先将已经有的代码转入
	for(i=0;i<m_ExdayHead[setcode].lStockNum;i++)
	{
		m_lExdayDataNum = ReadStkExday(NULL,setcode,m_pExdayIndex[setcode][i].Code,0);
		//多要30天的空间
		CHECKEXDAYSTEPSPC(bRet,m_pExdayData,pTmpExDay,m_lExdayDataSpc,m_lExdayDataNum+30,30,EXDAYDATA);
		if(!bRet) return FALSE;
		m_lExdayDataNum = ReadStkExday(m_pExdayData,setcode,m_pExdayIndex[setcode][i].Code,m_lExdayDataNum);
		bHasChange = FALSE;
		for(j=0;j<lDayNum;j++)
		{
			if(pfDateDay[j]==NULL || plDateDayLen[j]<=0)
				continue;
			lDataNum	= plDateDayLen[j]/sizeof(EXDAYDATA);
			pTmpExDay	= LPEXDAYDATA(pfDateDay[j]);
			for(k=0;k<lDataNum;k++)
			{
				if(pTmpExDay[k].Code[0]=='\0')
				{
					pTmpExDay[k].bTransed = char(TRUE);
					continue;
				}
				if(strncmp(pTmpExDay[k].Code,m_pExdayIndex[setcode][i].Code,SH_CODE_LEN)==0)
				{
					pTmpExDay[k].bTransed = char(TRUE);
					for(l=0;l<m_lExdayDataNum;l++)
					{
						if(m_pExdayData[l].Time>pTmpExDay[k].Time)			//插入数据
						{
							bHasChange = TRUE;
							memmove(&(m_pExdayData[l+1]),&(m_pExdayData[l]),(m_lExdayDataNum-l)*sizeof(EXDAYDATA));
							memmove(&(m_pExdayData[l]),&(pTmpExDay[k]),sizeof(EXDAYDATA));
							m_lExdayDataNum++;
							break;
						}
						else if(m_pExdayData[l].Time==pTmpExDay[k].Time)	//覆盖
						{
							bHasChange = TRUE;
							memmove(&(m_pExdayData[l]),&(pTmpExDay[k]),sizeof(EXDAYDATA));
							break;
						}
					}
					if(l>=m_lExdayDataNum)									//加到末尾
					{
						bHasChange = TRUE;
						memmove(&(m_pExdayData[l]),&(pTmpExDay[k]),sizeof(EXDAYDATA));
						m_lExdayDataNum++;
					}
					break;
				}
			}
		}
		//有修改,写盘
		if(bHasChange)
			WriteStkExday(setcode,i,m_pExdayData,m_lExdayDataNum);
	}
	//遍历,添加还没有的股票
	CHECKEXDAYSTEPSPC(bRet,m_pExdayData,pTmpExDay,m_lExdayDataSpc,30,30,EXDAYDATA);
	for(i=0;i<lDayNum;i++)
	{
		if(pfDateDay[i]==NULL || plDateDayLen[i]<=0)
			continue;
		lDataNum	= plDateDayLen[i]/sizeof(EXDAYDATA);
		pTmpExDay	= LPEXDAYDATA(pfDateDay[i]);
		for(j=0;j<lDataNum;j++)
		{
			if(pTmpExDay[j].Code[0]=='\0')
			{
				pTmpExDay[j].bTransed = char(TRUE);
				continue;
			}
			if(pTmpExDay[j].bTransed) continue;
			pTmpExDay[j].bTransed = TRUE;
			memmove(&(m_pExdayData[0]),&(pTmpExDay[j]),sizeof(EXDAYDATA));
			m_lExdayDataNum = 1;
			//
			strncpy(m_pExdayIndex[setcode][m_ExdayHead[setcode].lStockNum].Code,pTmpExDay[j].Code,SH_CODE_LEN);
			//
			for(k=i+1;k<lDayNum;k++)
			{
				if(pfDateDay[k]==NULL || plDateDayLen[k]<=0)
					continue;
				long lDataNum2	= plDateDayLen[k]/sizeof(EXDAYDATA);
				LPEXDAYDATA pTmpExDay2	= LPEXDAYDATA(pfDateDay[k]);
				for(l=0;l<lDataNum2;l++)
				{
					if(pTmpExDay2[l].bTransed) continue;
					if(strncmp(pTmpExDay2[l].Code,pTmpExDay[j].Code,SH_CODE_LEN)==0)
					{
						pTmpExDay2[l].bTransed = char(TRUE);
						for(m=0;m<m_lExdayDataNum;m++)
						{
							if(m_pExdayData[m].Time>pTmpExDay2[l].Time)			//插入数据
							{
								memmove(&(m_pExdayData[m+1]),&(m_pExdayData[m]),(m_lExdayDataNum-m)*sizeof(EXDAYDATA));
								memmove(&(m_pExdayData[m]),&(pTmpExDay2[l]),sizeof(EXDAYDATA));
								m_lExdayDataNum++;
								break;
							}
							else if(m_pExdayData[m].Time==pTmpExDay2[l].Time)	//覆盖
							{
								memmove(&(m_pExdayData[m]),&(pTmpExDay2[l]),sizeof(EXDAYDATA));
								break;
							}
						}
						if(m>=m_lExdayDataNum)									//加到末尾
						{
							memmove(&(m_pExdayData[m]),&(pTmpExDay2[l]),sizeof(EXDAYDATA));
							m_lExdayDataNum++;
						}
						break;
					}
				}
			}
			//
			m_ExdayHead[setcode].lStockNum++;
			WriteStkExday(setcode,m_ExdayHead[setcode].lStockNum-1,m_pExdayData,m_lExdayDataNum);
		}
	}
	return TRUE;
}