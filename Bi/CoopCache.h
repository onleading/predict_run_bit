#ifndef	_COOP_CACHE_CORE_
#define	_COOP_CACHE_CORE_
/////////////////////////////////////////////////////////////////////////////
enum	ItemAction
{
	ACTION_TEST,
	ACTION_CALC
};
struct	ReqItem
{
	char	Currency[20];
	int		nAction;
};
#define MAX_ITEM	10000
/////////////////////////////////////////////////////////////////////////////
class	CCoopCache
{
public:
	void	SendCache(ReqItem *pReq);
	BOOL	PeekCache(ReqItem &iReq);
public:
	CCoopCache(long nCacheSize=MAX_ITEM);
	~CCoopCache();
private:
	CCriticalSection	m_SecLock;
	long	m_nCacheSize;
	long	m_nHeadID,m_nEndID;
	ReqItem	*m_pReqQueue;
};
/////////////////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////////////////
