#include	"stdafx.h"
#include	"CoopCache.h"
/////////////////////////////////////////////////////////////////////////////
CCoopCache::CCoopCache(long nCacheSize)
{
	m_nCacheSize = nCacheSize;
	m_pReqQueue = new ReqItem[m_nCacheSize];
	memset(m_pReqQueue,0,sizeof(ReqItem)*m_nCacheSize);
	m_nHeadID = m_nEndID = 0;
}
CCoopCache::~CCoopCache()
{
	TDELA(m_pReqQueue);
}
/////////////////////////////////////////////////////////////////////////////
void	CCoopCache::SendCache(ReqItem *pReq)
{
	m_SecLock.Lock();
	if(m_nHeadID+1==m_nEndID||m_nEndID==0&&m_nHeadID+1==m_nCacheSize) 
	{
		m_SecLock.Unlock();
		return;
	}
	memcpy(&m_pReqQueue[m_nHeadID],pReq,sizeof(ReqItem));
	InterlockedExchange(&m_nHeadID,((m_nHeadID+1)>=m_nCacheSize)?0:m_nHeadID+1);
	m_SecLock.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
BOOL	CCoopCache::PeekCache(ReqItem &iReq)
{
	m_SecLock.Lock();
	if(m_nHeadID!=m_nEndID)
	{
		memcpy(&iReq,&m_pReqQueue[m_nEndID],sizeof(ReqItem));
		InterlockedExchange(&m_nEndID,((m_nEndID+1)>=m_nCacheSize)?0:m_nEndID+1);
		m_SecLock.Unlock();
		return(TRUE);
	}
	m_SecLock.Unlock();
	return(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
