#include "stdafx.h"
#include "Registry.h"
/////////////////////////////////////////////////////////////

#define		MAX_REG_ITEM		1024*4
/////////////////////////////////////////////////////////////
CRegistry::CRegistry(char * strVersion)
{	m_bRegSafe	= FAIL;
	strcpy(m_strVersion,strVersion);
	TMALLOC(m_pRegIndex,MAX_REG_ITEM*sizeof(REGINDEX),REGINDEX);
	for(short i=0;i<MAX_REG_ITEM;i++)
	{	m_pRegIndex->lNextIndex = EMPTY;
		m_pRegIndex->lChildIndex= EMPTY;
	}
}

CRegistry::~CRegistry()
{	long    lDataPtr=0,lTargetIndex=0,lpRegIndex[32],lStackTop=1;
	REGINDEX*pReg=NULL;
	memset(lpRegIndex,0,sizeof(long)*32);
	while(lStackTop)
	{	lTargetIndex = lpRegIndex[--lStackTop];
		pReg=&m_pRegIndex[lTargetIndex];
		if(pReg)
		{
			TDEL(pReg->pData);
			if(pReg->lNextIndex!=EMPTY)
			{	lpRegIndex[lStackTop++] = pReg->lNextIndex;
			}
			if(pReg->lChildIndex!=EMPTY)
			{	lpRegIndex[lStackTop++] = pReg->lChildIndex;
			}
		}
	}
	TDEL(m_pRegIndex)
}

long CRegistry::OpenReg(char* lpzRegDBPath,short bCreate)
{	FILE*   fpRegDB=NULL;
	char*   lpzDBBuf=NULL;
	long    lRegDBLen=0,lTargetIndex,lpRegIndex[32],lStackTop=1;
	if(m_pRegIndex==NULL)	return(m_bRegSafe=FAIL);
	REGINDEX		*pReg=NULL;
	if(!bCreate)	fpRegDB = _fsopen(lpzRegDBPath,"rb",SH_DENYNO);
	if(bCreate||NULL==fpRegDB)
	{	memset(&m_RegHead,0,sizeof(REGHEAD));
		strcpy(m_RegHead.strVersion,m_strVersion);
		NewIndex("Root",RT_KEY,NULL,NULL,EMPTY);
		return(m_bRegSafe=SUCCESS);
	}
	lRegDBLen = on_filelength(fpRegDB);
	lpzDBBuf  = new char[lRegDBLen];
	if(lpzDBBuf==NULL)
	{	fclose(fpRegDB);
		return(m_bRegSafe=FAIL);
	}
	if(fread(lpzDBBuf,lRegDBLen,1,fpRegDB)!=1)
	{	fclose(fpRegDB);
		TDEL(lpzDBBuf);
		return(m_bRegSafe=FAIL);
	}
	fclose(fpRegDB);
	memcpy(&m_RegHead,lpzDBBuf,sizeof(REGHEAD));
	if(strcmp(m_RegHead.strVersion,m_strVersion))
	{	TDEL(lpzDBBuf);
		return(m_bRegSafe=FAIL);
	}
	if(m_RegHead.lIndexOffset+sizeof(REGINDEX)*m_RegHead.lIndexSize>lRegDBLen)
	{	TDEL(lpzDBBuf);
		return(m_bRegSafe=FAIL);
	}
	memcpy(m_pRegIndex,lpzDBBuf+
		m_RegHead.lIndexOffset,sizeof(REGINDEX)*m_RegHead.lIndexSize);
	memset(lpRegIndex,0,sizeof(long)*32);
	BOOL bFindError=FALSE;
	while(lStackTop)
	{	lTargetIndex = lpRegIndex[--lStackTop];
		pReg=&m_pRegIndex[lTargetIndex];
		m_pRegIndex[lTargetIndex].pData = NULL;
		if(pReg->lType!=RT_KEY&&pReg->lType!=RT_VALUE)
		{	
			TMALLOC(m_pRegIndex[lTargetIndex].pData,pReg->lLength,char);
			if(m_RegHead.lDataOffset+pReg->lOffset+pReg->lLength<=lRegDBLen)
			{
				memcpy(m_pRegIndex[lTargetIndex].pData,lpzDBBuf+
					m_RegHead.lDataOffset+pReg->lOffset,pReg->lLength);
			}
			else bFindError=TRUE;
		}
		if(pReg->lNextIndex!=EMPTY)
		{	lpRegIndex[lStackTop++] = pReg->lNextIndex;
		}
		if(pReg->lChildIndex!=EMPTY)
		{	lpRegIndex[lStackTop++] = pReg->lChildIndex;
		}
	}
	TDEL(lpzDBBuf);
	if(bFindError) return(m_bRegSafe=FAIL); 
	return(m_bRegSafe=SUCCESS);
}

long CRegistry::SaveReg(char* lpzRegDBPath)
{//	if(!g_TdxCfg.bWriteAccess) return FAIL;
	if(m_bRegSafe==FAIL) return FAIL;
	FILE*fpRegDB = _fsopen(lpzRegDBPath,"wb",SH_DENYNO);
	if(NULL==fpRegDB) return FAIL;
	REGINDEX*pReg=NULL;
	long    lDataPtr=0,lTargetIndex=0,lpRegIndex[32],lStackTop=1;
	m_RegHead.lIndexOffset = sizeof(REGHEAD);
	m_RegHead.lDataOffset  = sizeof(REGHEAD)+sizeof(REGINDEX)*(m_RegHead.lIndexSize);
	fwrite(&m_RegHead,sizeof(REGHEAD),1,fpRegDB);
	fwrite(m_pRegIndex,sizeof(REGINDEX)*(m_RegHead.lIndexSize),1,fpRegDB);
	memset(lpRegIndex,0,sizeof(long)*32);
	while(lStackTop)
	{	lTargetIndex = lpRegIndex[--lStackTop];
		pReg=&m_pRegIndex[lTargetIndex];
		if(pReg->pData!=NULL)
		{   fwrite(pReg->pData,pReg->lLength,1,fpRegDB);
			pReg->lOffset = lDataPtr;
			lDataPtr += pReg->lLength;
		}
		if(pReg->lNextIndex!=EMPTY)
		{	lpRegIndex[lStackTop++] = pReg->lNextIndex;
		}
		if(pReg->lChildIndex!=EMPTY)
		{	lpRegIndex[lStackTop++] = pReg->lChildIndex;
		}
	}
	fseek(fpRegDB,0L,SEEK_SET);
	fwrite(&m_RegHead,sizeof(REGHEAD),1,fpRegDB);
	fwrite(m_pRegIndex,sizeof(REGINDEX)*m_RegHead.lIndexSize,1,fpRegDB);
	fclose(fpRegDB);
	return SUCCESS;
}

long	CRegistry::RegWrite(char*lpzBranchPath,long lType,void*pData,long lLength)
{	long	lpBeginStack[6],lpEndStack[6];
	long    lStackTop=0,lWordPtr=0,lWordTag=0;
	long    lKeyNum=0,lTypeTag=0;
	long    i=0,lParentIndex=0,lTargetIndex=0;
	char    strTmpName[64];
	if(m_bRegSafe==FAIL) return FAIL;
	while(lWordPtr<=strlen(lpzBranchPath))
	{	if(lpzBranchPath[lWordPtr]==0||
			 lpzBranchPath[lWordPtr]=='\\')
		{	if(lWordTag)
			{	lpEndStack[lStackTop++] = lWordPtr-1;
				lWordPtr++;
				lWordTag = 0;
			}
			else	lWordPtr++;
		}
		else 
		{	if(lWordTag)		lWordPtr++;
			else
			{	lpBeginStack[lStackTop] = lWordPtr;
				lWordPtr ++;
				lWordTag = 1;
			}
		}
	}
	if(lStackTop==0)	return FAIL;
	lWordPtr = strlen(lpzBranchPath)-1;
	if(lWordPtr>=0)	lTypeTag = lpzBranchPath[lWordPtr]=='\\';
	lKeyNum = lStackTop-1;
	for(i=0;i<lKeyNum;i++)
	{	memset(strTmpName,0,64);
		strncpy(strTmpName,lpzBranchPath+lpBeginStack[i],lpEndStack[i]-lpBeginStack[i]+1);
		lTargetIndex = LocateChild(strTmpName,lParentIndex,RT_KEY);
		if(lTargetIndex==EMPTY)
			lTargetIndex = NewIndex(strTmpName,RT_KEY,NULL,NULL,lParentIndex);
		lParentIndex = lTargetIndex;
	}
	memset(strTmpName,0,64);
	strncpy(strTmpName,lpzBranchPath+lpBeginStack[lKeyNum],lpEndStack[lKeyNum]-lpBeginStack[lKeyNum]+1);
	if(lTypeTag)
	{	lTargetIndex = LocateChild(strTmpName,lParentIndex,RT_KEY);
		if(lTargetIndex==EMPTY)
			lTargetIndex = NewIndex(strTmpName,lType+RT_KEY,pData,lLength,lParentIndex);
		else
			lTargetIndex = SetIndexData(strTmpName,lType+RT_KEY,pData,lLength,lTargetIndex);
	}
	else
	{	lTargetIndex = LocateChild(strTmpName,lParentIndex,RT_VALUE);
		if(lTargetIndex==EMPTY)
			lTargetIndex = NewIndex(strTmpName,lType+RT_VALUE,pData,lLength,lParentIndex);
		else
			lTargetIndex = SetIndexData(strTmpName,lType+RT_VALUE,pData,lLength,lTargetIndex);
	}
	return(lTargetIndex);
}

long CRegistry::RegRead(char* lpzBranchPath,void* pData,long nMaxBytes)
{	if(m_bRegSafe==FAIL) return FAIL;
	long  lTargetIndex = FindBranch(lpzBranchPath);
	if(lTargetIndex==EMPTY) return FAIL;
	REGINDEX	*pReg = &m_pRegIndex[lTargetIndex];
	if(pReg->lType==RT_KEY||pReg->lType==RT_VALUE) return FAIL;
	memcpy(pData,pReg->pData,MAX(0,MIN(pReg->lLength,nMaxBytes)));
	if(pReg->lType==RT_KEY+RT_CHARSTRING||pReg->lType==RT_VALUE+RT_CHARSTRING)
		((char*)pData)[MAX(0,MIN(pReg->lLength,nMaxBytes-1))] = 0;
	return SUCCESS;
}

long CRegistry::FindBranch(char* lpzBranchPath)
{	if(lpzBranchPath==NULL||strlen(lpzBranchPath)==0) return FAIL;
	if(m_RegHead.lIndexRecNum==0) return FAIL;
	long	lpBeginStack[6],lpEndStack[6];
	long    lStackTop=0,lWordPtr=0,lWordTag=0;
	long    lTargetIndex=0,lParentIndex=0;
	long    i=0,lTypeTag=0,lKeyNum=0;
	char    strTmpName[64];
	while(lWordPtr<=strlen(lpzBranchPath))
	{	if(lpzBranchPath[lWordPtr]==0||
			lpzBranchPath[lWordPtr]=='\\')
		{	if(lWordTag)
			{	lpEndStack[lStackTop++] = lWordPtr-1;
				lWordPtr++;
				lWordTag = 0;
			}
			else		lWordPtr++;
		}
		else 
		{	if(lWordTag)		lWordPtr++;
			else
			{	lpBeginStack[lStackTop] = lWordPtr;
				lWordPtr ++;
				lWordTag = 1;
			}
		}
	}
	if(lStackTop==0) return FAIL;
	lWordPtr = strlen(lpzBranchPath)-1;
	if(lWordPtr>=0)	lTypeTag = lpzBranchPath[lWordPtr]=='\\';
	lKeyNum = lStackTop-1;
	for(i=0;i<lKeyNum;i++)
	{	memset(strTmpName,0,64);
		strncpy(strTmpName,lpzBranchPath+lpBeginStack[i],lpEndStack[i]-lpBeginStack[i]+1);
		lTargetIndex = LocateChild(strTmpName,lParentIndex,RT_KEY);
		if(lTargetIndex==EMPTY)	return FAIL;
		lParentIndex = lTargetIndex;
	}
	memset(strTmpName,0,64);
	strncpy(strTmpName,lpzBranchPath+lpBeginStack[lKeyNum],lpEndStack[lKeyNum]-lpBeginStack[lKeyNum]+1);
	lTargetIndex = LocateChild(strTmpName,lParentIndex,lTypeTag?RT_KEY:RT_VALUE);
	return(lTargetIndex==EMPTY?FAIL:lTargetIndex);
}

char* CRegistry::Peekstr(char* lpzBranchPath)
{	if(m_bRegSafe==FAIL) return NULL;
	long  lTargetIndex = FindBranch(lpzBranchPath);
	if(lTargetIndex==EMPTY) return NULL;
	if(m_pRegIndex[lTargetIndex].pData==NULL) return NULL;
	return(m_pRegIndex[lTargetIndex].pData);
}

long CRegistry::LocateChild(char* lpzChildName,long lParentIndex,long lType)
{	long	lTargetIndex=0,lTypeMin=lType,lTypeMax=lType;
	if(lParentIndex<0)									return EMPTY;
	if(lParentIndex>=m_RegHead.lIndexRecNum) 			return EMPTY;
	if(EMPTY==m_pRegIndex[lParentIndex].lChildIndex)	return EMPTY;
	switch(lType)
	{	case	RT_KEY:
			lTypeMin=RT_KEY;	
			lTypeMax=RT_VALUE-1; 
			break;
		case	RT_VALUE:
			lTypeMin=RT_VALUE;  
			lTypeMax=RT_LIMIT-1;
			break;
		case	RT_ANY:	
			lTypeMin=RT_KEY;    
			lTypeMax=RT_LIMIT-1;
	}
	lTargetIndex = m_pRegIndex[lParentIndex].lChildIndex;
	REGINDEX	*pReg = &m_pRegIndex[lTargetIndex];
	while(lTargetIndex!=EMPTY&&
			!(0==strcmp(pReg->strName,lpzChildName) &&
			  pReg->lType>=lTypeMin&&pReg->lType<=lTypeMax))
	{	lTargetIndex = pReg->lNextIndex;
		if(lTargetIndex!=EMPTY)	pReg = &m_pRegIndex[lTargetIndex];
	}
	return lTargetIndex;
}


long CRegistry::NewIndex(char*lpzName,long lType,void*pData,long lLength,long lParentIndex)
{	long	lTargetIndex=0,lNextIndex=0;
	if(m_RegHead.lIndexRecNum<=m_RegHead.lIndexSize)
	{	lTargetIndex = m_RegHead.lIndexSize;
		m_RegHead.lIndexSize++;
	}
	else
	{	while(m_pRegIndex[lTargetIndex].bUsed&&
			lTargetIndex<=m_RegHead.lIndexSize)	lTargetIndex++;
	}
	if(lTargetIndex==EMPTY) return FAIL;
	REGINDEX	*pReg = &m_pRegIndex[lTargetIndex];
	pReg->lPrevIndex	= EMPTY;
	pReg->lChildIndex	= EMPTY;
	if(lParentIndex==EMPTY)
	{	pReg->lNextIndex       = EMPTY;
		pReg->lParentIndex     = EMPTY;
	}
	else if(m_pRegIndex[lParentIndex].lChildIndex==EMPTY)
	{	pReg->lNextIndex       = EMPTY;
		pReg->lParentIndex     = lParentIndex;
		m_pRegIndex[lParentIndex].lChildIndex	= lTargetIndex;
	}
	else
	{	lNextIndex = m_pRegIndex[lParentIndex].lChildIndex;
		pReg->lNextIndex       = lNextIndex;
		pReg->lParentIndex     = lParentIndex;
		m_pRegIndex[lParentIndex].lChildIndex	= lTargetIndex;
		m_pRegIndex[lNextIndex  ].lPrevIndex	= lTargetIndex;
	}
	strcpy(pReg->strName,lpzName);
	pReg->lType   = lType;
	pReg->bUsed   = TRUE;
	pReg->lOffset = NULL;
	pReg->pData   = NULL;
	pReg->lLength = 0;
	if(pData!=NULL)
	{	pReg->lLength = lLength;
		TMALLOC(m_pRegIndex[lTargetIndex].pData,lLength,char);
		if(lLength>0)	memcpy(m_pRegIndex[lTargetIndex].pData,pData,lLength);
	}
	m_RegHead.lIndexRecNum ++;
	return lTargetIndex;
}

long	CRegistry::SetIndexData(char*lpzName,long lType,void*pData,long lLength,long lTargetIndex)
{	if(lTargetIndex==EMPTY) return FAIL;
	REGINDEX	*pReg = &m_pRegIndex[lTargetIndex];
	strcpy(pReg->strName,lpzName);
	pReg->lType   = lType;
	pReg->bUsed   = TRUE;
	pReg->lOffset = NULL;
	pReg->lLength = 0;
	TDEL(m_pRegIndex[lTargetIndex].pData)
	if(pData!=NULL)
	{	
		TMALLOC(m_pRegIndex[lTargetIndex].pData,lLength,char);
		if(lLength>0) memcpy(m_pRegIndex[lTargetIndex].pData,pData,lLength);
		pReg->lLength = lLength;
	}
	return lTargetIndex;
}
