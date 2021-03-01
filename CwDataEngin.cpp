// CwDataEngin.cpp: implementation of the CCwDataEngin class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CwDataEngin.h"
#include "VCwData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
long CCwDataEngin::m_nCount = 0;
MemSecurityInfo CCwDataEngin::m_SecurityInfo[CW_SECURITY_NUM]={0};
map<long, MemConstInfo, greater<long> > CCwDataEngin::m_mHyConstMap;
map<long, MemSecurityInfo*, greater<long> > CCwDataEngin::m_mInnerCodeMap, CCwDataEngin::m_mComCodeMap;

CCwDataEngin::CCwDataEngin()
{
	m_pMsgWnd = NULL;

	m_nReqID = 0;
	m_pAckBuf = NULL;
	m_nRecNum = 0;
	m_nReqPassedTime = 0;

	InterlockedIncrement(&m_nCount);
}

CCwDataEngin::~CCwDataEngin()
{
	TDELA(m_pAckBuf)
	InterlockedDecrement(&m_nCount);

//	if(m_nCount<1) 
//		TD_Uninit_();
}

//////////////////////////////////////////////////////////////////////////
void	CCwDataEngin::InitHyInfo()
{
	if(!m_mHyConstMap.empty()) return;

	MemConstInfo iConst1[CW_CONSTVALUE_NUM]={0};
	int nConstNum1 = TD_GetConstList_(C_CW_TJJHY, iConst1);
	for(int i=0;i<nConstNum1;i++)
		m_mHyConstMap[iConst1[i].m_lCID] = iConst1[i];

	MemConstInfo iConst2[CW_CONSTVALUE_NUM]={0};
	int nConstNum2 = TD_GetConstList_(C_CW_ZJHHY, iConst2);
	for(i=0;i<nConstNum2;i++)
		m_mHyConstMap[iConst2[i].m_lCID] = iConst2[i];

	MemConstInfo iConst3[CW_CONSTVALUE_NUM]={0};
	int nConstNum3 = TD_GetConstList_(C_CW_TDXHY, iConst3);
	for(i=0;i<nConstNum3;i++)
		m_mHyConstMap[iConst3[i].m_lCID] = iConst3[i];
}

void	CCwDataEngin::InitSecurityInfo()
{
	if(!m_mInnerCodeMap.empty()&&!m_mComCodeMap.empty()) return;
	m_mInnerCodeMap.clear();
	m_mComCodeMap.clear();

	memset(m_SecurityInfo, 0, sizeof(m_SecurityInfo));
	long nSecNum = TD_GetAllSecurity_(m_SecurityInfo);
	for(int i=nSecNum-1;i>=0;i--)
	{
		m_mInnerCodeMap[m_SecurityInfo[i].m_nInnerCode] = &m_SecurityInfo[i];
		m_mComCodeMap[m_SecurityInfo[i].m_nComCode] = &m_SecurityInfo[i];
	}
}

BOOL	CCwDataEngin::GetHyConstInfo(long ConstID, MemConstInfo &iConstInfo)
{
	if(ConstID==C_CW_TJJHY)
	{
		iConstInfo.m_lCID = C_CW_TJJHY;
		strcpy(iConstInfo.m_szCName, "统计局行业");
		return TRUE;
	}
	if(ConstID==C_CW_ZJHHY)
	{
		iConstInfo.m_lCID = C_CW_ZJHHY;
		strcpy(iConstInfo.m_szCName, "证监会行业");
		return TRUE;
	}
	if(ConstID==C_CW_TDXHY)
	{
		iConstInfo.m_lCID = C_CW_TDXHY;
		strcpy(iConstInfo.m_szCName, "行业");
		return TRUE;
	}

	map<long, MemConstInfo, greater<long> >::iterator iter = m_mHyConstMap.find(ConstID);
	if(iter==m_mHyConstMap.end()) return FALSE;

	iConstInfo = (*iter).second;
	return iConstInfo.m_lCID==ConstID;
}
//////////////////////////////////////////////////////////////////////////
BOOL	CCwDataEngin::InitFastVSQL(CWnd *pDataWnd)						//快速数据服务
{
	if(!pDataWnd) return FALSE;
	m_pMsgWnd = pDataWnd;

	char path[256];
	GetCurrentDirectory(255,path);
	CString HomePath = path;
	if(HomePath.Right(1) != "\\")
		HomePath += "\\";
	
	if(!TD_Init_Environment_("zszq",HomePath,g_WSXHStr,"",g_WSXHStr+g_strYxhjCfg,0))
		return FALSE;
	TD_SetFeedBackMsg_(m_pMsgWnd->GetSafeHwnd(),UM_FEEDBACK_MSG);
	if(!TD_RefreshData_(0,0))
		return FALSE;

	return TRUE;
}

long dwStart = 0;
void	CCwDataEngin::GetData(const char * ReqCommand,long ReqType,long ReqOption,long reserved)
{
	TDELA(m_pAckBuf)
	m_nRecNum = 0;
	m_nFieldLen = 0;

	m_FieldMap.clear();
	memset(pFieldIndex, 0, sizeof(pFieldIndex));
	memset(m_FieldInfo, 0, sizeof(m_FieldInfo));

	m_nInnerCodeID = -1;
	m_nComCodeID = -1;
	m_nStockCodeID = -1;

	m_nReqPassedTime = 0;
	dwStart = GetTickCount();

	m_nReqID = TD_GetData_(ReqCommand,ReqType,ReqOption,m_pMsgWnd->GetSafeHwnd(),UM_ASYN_FST_MSG);
}

long	CCwDataEngin::GetDataACK(WPARAM wParam,LPARAM lParam)
{
	if(wParam<=0) return 0;
	m_pAckBuf = new char[wParam+1024];
	memset(m_pAckBuf, 0, wParam+1024);

	for(int i=0;i<C_EnumEnd+1;i++)
		m_mConstData[i].clear();

	long nFieldNum = 0;
	m_nRecNum = TD_GetDataACK_(m_nReqID,m_pAckBuf,wParam+1024,m_FieldInfo,nFieldNum);

	m_nReqPassedTime = GetTickCount()-dwStart;

	m_nInnerCodeID = -1;
	m_nComCodeID = -1;
	m_nStockCodeID = -1;
	for(i=0;i<nFieldNum;i++)
	{
		if(m_FieldInfo[i].nId<CW_FIELD_NUM)
			pFieldIndex[m_FieldInfo[i].nId] = &m_FieldInfo[i];
		else 
			m_FieldMap[m_FieldInfo[i].nId] = &m_FieldInfo[i];
		m_nFieldLen+=m_FieldInfo[i].nWid;

		if(strcmp(m_FieldInfo[i].szName, "证券内部编码")==0)
			m_nInnerCodeID = i;
		if(strcmp(m_FieldInfo[i].szName, "公司代码")==0)
			m_nComCodeID = i;
		if(strcmp(m_FieldInfo[i].szName, "股票代码")==0)
			m_nStockCodeID = i;
		if(strcmp(m_FieldInfo[i].szName, "证券代码")==0)
			m_nStockCodeID = i;
	}

	return m_nRecNum;
}

short	CCwDataEngin::GetCodeIDType(char *FieldID)
{
	if((FieldID[0]!='T'&&FieldID[0]!='t')||!isdigit(FieldID[1])) return -1;

	long nFieldId = atol(&FieldID[1]);
	MemFieldInfo *pmfi = NULL;

	if(nFieldId<CW_FIELD_NUM) 
		pmfi = pFieldIndex[nFieldId];
	else
	{
		map<long, MemFieldInfo*, greater<long> >::iterator iter = m_FieldMap.find(nFieldId);
		if(iter==m_FieldMap.end()) return -1;
		
		pmfi = (*iter).second;
	}
	if(!pmfi) return -1;

	if(strcmp(pmfi->szName, "证券内部编码")==0)
		return INNER_CODE;
	if(strcmp(pmfi->szName, "公司代码")==0)
		return COM_CODE;
	if(strcmp(pmfi->szName, "股票代码")==0)
		return STOCK_CODE;
	if(strcmp(pmfi->szName, "证券代码")==0)
		return STOCK_CODE;
	return -1;
}

long	CCwDataEngin::GetTypeIDFromCode(short nTypeID, char *Code)
{
	for(int i=0;i<CW_SECURITY_NUM;i++)
	{
		if(strcmp(Code, m_SecurityInfo[i].m_szSymbol)==0&&m_SecurityInfo[i].m_nSecuCategory==1)
		{
			switch(nTypeID)
			{
			case INNER_CODE:
				return m_SecurityInfo[i].m_nInnerCode;
				break;
			case COM_CODE:
				return m_SecurityInfo[i].m_nComCode;
			default:
				return -1;
			}
		}
	}
	return -1;
}
//////////////////////////////////////////////////////////////////////////
BOOL	CCwDataEngin::GetMemFieldInfo(char *FieldID, MemFieldInfo* &pFieldInfo)
{
	if((FieldID[0]!='T'&&FieldID[0]!='t')||!isdigit(FieldID[1])) return FALSE;

	long nFieldId = atol(&FieldID[1]);
	pFieldInfo = NULL;

	if(nFieldId<CW_FIELD_NUM) 
		pFieldInfo = pFieldIndex[nFieldId];
	else
	{
		map<long, MemFieldInfo*, greater<long> >::iterator iter = m_FieldMap.find(nFieldId);
		if(iter==m_FieldMap.end()) return FALSE;
		
		pFieldInfo = (*iter).second;
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
BOOL	CCwDataEngin::GetIntData(int nRec,MemFieldInfo *pFieldInfo,int &nRet)
{
	switch(pFieldInfo->nType)
	{
	case C_Short:
		{
			short iTmp=0;
			memcpy(&iTmp,m_pAckBuf+nRec*m_nFieldLen+pFieldInfo->nPos,pFieldInfo->nWid);
			nRet = iTmp;
		}
		break;
	case C_Int:
		{
			int iTmp=0;
			memcpy(&iTmp,m_pAckBuf+nRec*m_nFieldLen+pFieldInfo->nPos,pFieldInfo->nWid);
			nRet = iTmp;
		}
		break;
	case C_Char:
		{
			char iTmp=0;
			memcpy(&iTmp,m_pAckBuf+nRec*m_nFieldLen+pFieldInfo->nPos,pFieldInfo->nWid);
			nRet = iTmp;
		}
		break;
	case C_Date:
		{
			long iTmp=0;
			memcpy(&iTmp,m_pAckBuf+nRec*m_nFieldLen+pFieldInfo->nPos,pFieldInfo->nWid);
			nRet = iTmp;
		}
		break;
	case C_DateTime:
		{
			LONGLONG llDate = 0;
			memcpy(&llDate,m_pAckBuf+nRec*m_nFieldLen+pFieldInfo->nPos,pFieldInfo->nWid);
			nRet = (long)llDate;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL	CCwDataEngin::GetIntData(int nRec,char *FieldID,int &nRet)
{
	MemFieldInfo *pmfi = NULL;
	GetMemFieldInfo(FieldID, pmfi);
	return pmfi&&GetIntData(nRec, pmfi, nRet);
}
//////////////////////////////////////////////////////////////////////////
BOOL	CCwDataEngin::GetFloatData(int nRec,MemFieldInfo *pFieldInfo,double &fRet)
{
	switch(pFieldInfo->nType)
	{
	case C_Float:
		{
			float fTmp=0.0f;
			memcpy(&fTmp,m_pAckBuf+nRec*m_nFieldLen+pFieldInfo->nPos,pFieldInfo->nWid);
			fRet = fTmp;
		}
		break;
	case C_Double:
		{
			double fTmp=0.0;
			memcpy(&fTmp,m_pAckBuf+nRec*m_nFieldLen+pFieldInfo->nPos,pFieldInfo->nWid);
			fRet = fTmp;
		}
		break;
	default:
		return FALSE;
	}
	
	return TRUE;
}

BOOL	CCwDataEngin::GetFloatData(int nRec,char *FieldID,double &fRet)
{
	MemFieldInfo *pmfi = NULL;
	GetMemFieldInfo(FieldID, pmfi);
	return pmfi&&GetFloatData(nRec, pmfi, fRet);
}
//////////////////////////////////////////////////////////////////////////
#define MAX_LISTSTR_LEN 1024*5 

BOOL	CCwDataEngin::GetStringData(int nRec,MemFieldInfo *pFieldInfo,char *buf,int wantlen)
{
	if(pFieldInfo->nType>=C_StrFirst && pFieldInfo->nType<=C_StrEnd)
	{
		memcpy(buf,m_pAckBuf+nRec*m_nFieldLen+pFieldInfo->nPos,min(pFieldInfo->nWid, MAX_LISTSTR_LEN-1));
		buf[pFieldInfo->nWid]=0;

		return TRUE;
	}
	else if(pFieldInfo->nType>=C_EnumFirst&&pFieldInfo->nType<=C_EnumEnd)		//枚举类型转换为字符
	{
		if(m_mConstData[pFieldInfo->nType].empty())						//空的
		{
			MemConstInfo iConst[CW_CONSTVALUE_NUM]={0};
			int nEnumVal = TD_GetConstList_(pFieldInfo->nType, iConst);
			for(int i=0;i<nEnumVal;i++)
				m_mConstData[pFieldInfo->nType][iConst[i].m_lCID] = iConst[i];
		}
		if(m_mConstData[pFieldInfo->nType].empty()) return FALSE;

		long EnumID = -1;
		memcpy(&EnumID,m_pAckBuf+nRec*m_nFieldLen+pFieldInfo->nPos,pFieldInfo->nWid);
		map<long, MemConstInfo, greater<long> >::iterator iter = m_mConstData[pFieldInfo->nType].find(EnumID);
		if(iter == m_mConstData[pFieldInfo->nType].end()) return FALSE;
		MemConstInfo mci =  (*iter).second;
		strncpy(buf, mci.m_szCName, CW_NAME_LEN);

		return TRUE;
	}

	switch(pFieldInfo->nType)
	{
	case C_DateTime:
		{
			LONGLONG llDate = 0;
			memcpy(&llDate,m_pAckBuf+nRec*m_nFieldLen+pFieldInfo->nPos,pFieldInfo->nWid);
			sprintf(buf, "%d", llDate);
		}
		break;
	case C_Short:
		{
			short iTmp=0;
			memcpy(&iTmp,m_pAckBuf+nRec*m_nFieldLen+pFieldInfo->nPos,pFieldInfo->nWid);
			sprintf(buf, "%d", iTmp);
		}
		break;
	case C_Int:
		{
			int iTmp=0;
			memcpy(&iTmp,m_pAckBuf+nRec*m_nFieldLen+pFieldInfo->nPos,pFieldInfo->nWid);
			sprintf(buf, "%d", iTmp);
		}
		break;
	case C_Char:
		{
			char iTmp=0;
			memcpy(&iTmp,m_pAckBuf+nRec*m_nFieldLen+pFieldInfo->nPos,pFieldInfo->nWid);
			sprintf(buf, "%d", iTmp);
		}
		break;
	case C_Date:
		{
			LONGLONG iTmp=0;
			memcpy(&iTmp,m_pAckBuf+nRec*m_nFieldLen+pFieldInfo->nPos,pFieldInfo->nWid);
			sprintf(buf, "%lld", iTmp);
		}
		break;
	case C_LongLong:
		{
			LONGLONG iTmp=0;
			memcpy(&iTmp,m_pAckBuf+nRec*m_nFieldLen+pFieldInfo->nPos,pFieldInfo->nWid);
			sprintf(buf, "%lld", iTmp);
		}
		break;
	case C_Float:
		{
			float fTmp=0.0f;
			memcpy(&fTmp,m_pAckBuf+nRec*m_nFieldLen+pFieldInfo->nPos,pFieldInfo->nWid);
			sprintf(buf, "%.3f", fTmp);
		}
		break;
	case C_Double:
		{
			double fTmp=0.0;
			memcpy(&fTmp,m_pAckBuf+nRec*m_nFieldLen+pFieldInfo->nPos,pFieldInfo->nWid);
			sprintf(buf, "%.3f", fTmp);
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL	CCwDataEngin::GetStringData(int nRec,char *FieldID,char *buf,int wantlen)
{
	MemFieldInfo *pmfi = NULL;
	GetMemFieldInfo(FieldID, pmfi);
	return pmfi&&GetStringData(nRec, pmfi, buf, wantlen);
}

BOOL	CCwDataEngin::GetStockCode(int nRec, char *FieldID, short nCodeIDType, char *CodeBuf, int wantlen)
{
	if((FieldID[0]!='T'&&FieldID[0]!='t')||!isdigit(FieldID[1])) return FALSE;
	CodeBuf[0] = 0;
	switch(nCodeIDType)
	{
	case INNER_CODE:
		{
			int nInnerCodeID = -1;
			BOOL bSuccess = GetIntData(nRec, &m_FieldInfo[m_nInnerCodeID], nInnerCodeID);	
			if(!bSuccess) return FALSE;
			map<long, MemSecurityInfo*, greater<long> >::iterator iter = m_mInnerCodeMap.find(nInnerCodeID);
			if(iter == m_mInnerCodeMap.end()) return FALSE;
			MemSecurityInfo *pmsi = (*iter).second;
			if(pmsi) strncpy(CodeBuf, pmsi->m_szSymbol, 10);
		}
		break;
	case COM_CODE:
		{
			int nComCodeID = -1;
			BOOL bSuccess = GetIntData(nRec, &m_FieldInfo[m_nComCodeID], nComCodeID);
			if(!bSuccess) return FALSE;
			map<long, MemSecurityInfo*, greater<long> >::iterator iter = m_mComCodeMap.find(nComCodeID);
			if(iter == m_mComCodeMap.end()) return FALSE;
			MemSecurityInfo *pmsi = (*iter).second;
			if(pmsi) strncpy(CodeBuf, pmsi->m_szSymbol, 10);
		}
		break;
	case STOCK_CODE:
		{
			BOOL bSuccess = GetStringData(nRec, &m_FieldInfo[m_nStockCodeID], CodeBuf, 10);
			if(!bSuccess) return FALSE;
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL	CCwDataEngin::GetStockName(int nRec, char *FieldID, short nCodeIDType, char *NameBuf, int wantlen) 
{
	if((FieldID[0]!='T'&&FieldID[0]!='t')||!isdigit(FieldID[1])) return FALSE;
	NameBuf[0] = 0;
	switch(nCodeIDType)
	{
	case INNER_CODE:
		{
			int nInnerCodeID = -1;
			BOOL bSuccess = GetIntData(nRec, &m_FieldInfo[m_nInnerCodeID], nInnerCodeID);
			if(!bSuccess) return FALSE;
			map<long, MemSecurityInfo*, greater<long> >::iterator iter = m_mInnerCodeMap.find(nInnerCodeID);
			if(iter == m_mInnerCodeMap.end()) return FALSE;
			MemSecurityInfo *pmsi = (*iter).second;
			if(pmsi) strncpy(NameBuf, pmsi->m_szSecuAbbr, 10);
		}
		break;
	case COM_CODE:
		{
			int nComCodeID = -1;
			BOOL bSuccess = GetIntData(nRec, &m_FieldInfo[m_nComCodeID], nComCodeID);
			if(!bSuccess) return FALSE;
			map<long, MemSecurityInfo*, greater<long> >::iterator iter = m_mComCodeMap.find(nComCodeID);
			if(iter == m_mComCodeMap.end()) return FALSE;
			MemSecurityInfo *pmsi = (*iter).second;
			if(pmsi) strncpy(NameBuf, pmsi->m_szSecuAbbr, 10);
		}
		break;
	case STOCK_CODE:
		{
			char CodeBuf[CODE_SIZE]={0};
			BOOL bSuccess = GetStringData(nRec, &m_FieldInfo[m_nStockCodeID], CodeBuf, 8);
			if(!bSuccess) return FALSE;
			for(int i=0;i<CW_SECURITY_NUM;i++)
			{
				if(strcmp(m_SecurityInfo[i].m_szSymbol, CodeBuf)==0)
				{
					strncpy(NameBuf, m_SecurityInfo[i].m_szSecuAbbr, 20);
					break;
				}
			}
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}