#ifndef __EXDAYMNG__H__
#define	__EXDAYMNG__H__


#define CHECKEXDAYSTEPSPC(bRet,pt,tmppt,spc,req,step,type) {\
	if((req)<=0) (bRet) = FALSE;\
	if((bRet) && ((pt)==NULL || (spc)<(req)))\
	{\
		(tmppt) = new type[(req)+(step)];\
		if((tmppt)==NULL) (bRet) = FALSE;\
		else\
		{\
			memset((tmppt),0,((req)+(step))*sizeof(type));\
			if((spc)>0 && (pt)!=NULL) memcpy((tmppt),(pt),(spc)*sizeof(type));\
			TDELA(pt);\
			(pt) = (tmppt);\
			(spc) = (req)+(step);\
		}\
	}\
}

class ExdayMng
{
public:
	ExdayMng();
	~ExdayMng();
	BOOL Init();

	long ReadStkExday(LPEXDAYDATA pExday,short setcode,char *Code,long lReqNum);
	BOOL WriteStkExday(short setcode,long lIndex,LPEXDAYDATA pExday,long lDataNum);
	BOOL WriteExdayFile(short setcode,char *pfDateDay[],long plDateDayLen[],long plDate[],long lDayNum);
	
protected:
	BOOL	m_bInited;
	
	CCriticalSection m_CSLock[2];

	char	m_ExdayPath[2][MAX_PATH];
	FILE	*m_ExdayFile[2];
	LPEXDAYHEAD		m_ExdayHead;		//文件头
	LPEXDAYINDEX	m_pExdayIndex[2];	//文件索引缓存
	char	m_BlankBlock[EXDAYBLOCK_LEN];

	//写入时的缓存
	LPEXDAYDATA		m_pExdayData;
	long	m_lExdayDataSpc;
	long	m_lExdayDataNum;
};

#endif