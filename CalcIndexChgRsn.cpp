#include "stdafx.h"
#include "CalcIndexChgRsn.h"

TCalcIndexChgReason::TCalcIndexChgReason()
{
	m_bInited		= FALSE;
	m_bWeightCaled	= FALSE;
	memset(m_IndexName,0,26*sizeof(char));
	m_IndexClose	= 0;
	m_pStkWeight	= NULL;
	m_nStkWeight	= 0;
	m_nMaxStkNum	= 0;
	m_nStkSpace		= 0;
}

TCalcIndexChgReason::~TCalcIndexChgReason()
{
	TDELA(m_pStkWeight);
}

BOOL TCalcIndexChgReason::SetInitData(char *szIndexName,float fIndexC,int nMaxGpNum)
{
	m_bInited		= FALSE;
	m_bWeightCaled	= FALSE;
	if(szIndexName[0]=='\0' || fIndexC<0.001 || nMaxGpNum<=0) return FALSE;
	if(m_nStkSpace < nMaxGpNum)
	{
		StkWeight *pStkWeight = new StkWeight[nMaxGpNum];
		if(pStkWeight == NULL) return FALSE;
		TDELA(m_pStkWeight);
		m_pStkWeight	= pStkWeight;
		m_nStkSpace		= nMaxGpNum;
	}
	m_nStkWeight		= 0;
	m_nMaxStkNum		= nMaxGpNum;
	memset(m_pStkWeight,0,nMaxGpNum*sizeof(StkWeight));
	strncpy(m_IndexName,szIndexName,25);
	m_IndexName[24]		= '\0';
	m_IndexClose		= fIndexC;
	m_bInited			= TRUE;
	return m_bInited;
}

BOOL TCalcIndexChgReason::AddOne(short SetCode,char *StkCode,float fNow,float fClose,float fCapital)
{
	if(!m_bInited) return FALSE;
	if(m_nStkWeight >= m_nMaxStkNum) return FALSE;
	if(StkCode[0]=='\0' || fClose<0.0001 || fCapital<0.0001) return FALSE;
	m_bWeightCaled	= FALSE;
	m_pStkWeight[m_nStkWeight].SetCode		= SetCode;
	strncpy(m_pStkWeight[m_nStkWeight].StkCode,StkCode,13);
	m_pStkWeight[m_nStkWeight].StkCode[12]	= '\0';
	m_pStkWeight[m_nStkWeight].fNow			= fNow;
	m_pStkWeight[m_nStkWeight].fClose		= fClose;
	m_pStkWeight[m_nStkWeight].fCapital		= fCapital;
	m_pStkWeight[m_nStkWeight].dWeight		= 0;
	m_pStkWeight[m_nStkWeight].fResult		= 0;
	m_nStkWeight++;
	return TRUE;
}

BOOL TCalcIndexChgReason::ReCalcWeight()
{
	m_bWeightCaled	= FALSE;
	if(!m_bInited) return FALSE;
	if(m_nStkWeight<=0) return FALSE;
	double dAllWeight = 0;
	int i=0;
	for(i=0;i<m_nStkWeight;i++) dAllWeight += m_pStkWeight[i].fClose*m_pStkWeight[i].fCapital;
	if(dAllWeight < 0.0001) return FALSE;
	for(i=0;i<m_nStkWeight;i++)
		m_pStkWeight[i].dWeight = m_pStkWeight[i].fCapital/dAllWeight;
	m_bWeightCaled = TRUE;
	return TRUE;
}

BOOL TCalcIndexChgReason::UpdateResult(short SetCode,char *StkCode,float fNow)
{
	if(!m_bInited || !m_bWeightCaled) return FALSE;
	if(StkCode == NULL) return FALSE;
	int i=0;
	for(i=0;i<m_nStkWeight;i++)
		if(m_pStkWeight[i].SetCode==SetCode && stricmp(m_pStkWeight[i].StkCode,StkCode)==0) break;
	if(i>=m_nStkWeight) return FALSE;
	if(fNow < 0.0001) m_pStkWeight[i].fResult = 0;
	else m_pStkWeight[i].fResult = float((fNow-m_pStkWeight[i].fClose)*m_pStkWeight[i].dWeight*m_IndexClose);
	return TRUE;
}

BOOL TCalcIndexChgReason::GetOneResult(short SetCode,char *StkCode,float &fResult)
{
	if(!m_bInited || !m_bWeightCaled) return FALSE;
	int i=0;
	for(i=0;i<m_nStkWeight;i++)
		if(m_pStkWeight[i].SetCode==SetCode && stricmp(m_pStkWeight[i].StkCode,StkCode)==0) break;
	if(i>=m_nStkWeight) return FALSE;
	fResult = m_pStkWeight[i].fResult;
	return TRUE;
}

int TCalcIndexChgReason::GetAllResult(LPSTKWEIGHT &pStkWeight)
{
	if(!m_bInited || !m_bWeightCaled) return 0;
	pStkWeight = m_pStkWeight;
	return m_nStkWeight;
}
