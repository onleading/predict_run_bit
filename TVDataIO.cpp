#include "stdafx.h"
#include "TVDataIO.h"

TVDataIO::TVDataIO(DWORD dwUserId,char *szPassword,char *szServerIp,DWORD dwServerPort,DWORD dwProvinceId)
{
	m_bInited		= FALSE;
	m_bReleased		= FALSE;
	m_bDataLoaded	= FALSE;
	m_pDataCore		= NULL;
	m_hTopViewDll	= ::LoadLibrary("TDXTVData.dll");
	if(m_hTopViewDll)
	{
		m_FuncInit			= (DllInit)GetProcAddress(m_hTopViewDll,"DllInit");
		m_FuncGetInterface	= (DllGetZXWDataInterface)GetProcAddress(m_hTopViewDll,"DllGetZXWDataInterface");
		m_FuncReceiveData	= (DllReceiveData)GetProcAddress(m_hTopViewDll,"DllReceiveData");
		m_FuncRelease		= (DllRelease)GetProcAddress(m_hTopViewDll,"DllRelease");
		if(m_FuncInit==NULL || m_FuncGetInterface==NULL || m_FuncReceiveData==NULL || m_FuncRelease==NULL)
		{
			m_FuncInit			= NULL;
			m_FuncGetInterface	= NULL;
			m_FuncReceiveData	= NULL;
			m_FuncRelease		= NULL;
			FreeTopviewDll();
			m_bInited			= FALSE;
			m_bReleased			= TRUE;
		}
		else
		{
			m_bInited		= TRUE;
			memset(m_HomePath,0,MAX_PATH*sizeof(char));
			strncpy(m_HomePath,LPCSTR(HomePath),MAX_PATH-30);
			m_dwUserID		= dwUserId;
			memset(m_szPassword,0,MAX_PATH*sizeof(char));
			strncpy(m_szPassword,szPassword,MAX_PATH-30);
			memset(m_szServerIP,0,32*sizeof(char));
			strncpy(m_szServerIP,szServerIp,30);
			m_dwServerPort	= dwServerPort;
			m_dwProvinceID	= dwProvinceId;
			if(!m_FuncInit(m_HomePath,m_dwUserID,m_szPassword,m_szServerIP,m_dwServerPort,m_dwProvinceID))
			{
				m_FuncRelease();
				m_FuncInit			= NULL;
				m_FuncGetInterface	= NULL;
				m_FuncReceiveData	= NULL;
				m_FuncRelease		= NULL;
				FreeTopviewDll();
				m_bInited			= FALSE;
				m_bReleased			= TRUE;
			}
			else
			{
				m_pDataCore = m_FuncGetInterface();
				if(m_pDataCore) 
					m_bDataLoaded = TRUE;
				else
				{
					m_FuncRelease();
					m_FuncInit			= NULL;
					m_FuncGetInterface	= NULL;
					m_FuncReceiveData	= NULL;
					m_FuncRelease		= NULL;
					FreeTopviewDll();
					m_bInited			= FALSE;
					m_bReleased			= TRUE;
				}
			}
		}
	}
	else m_bReleased = TRUE;
}

TVDataIO::~TVDataIO()
{
	FreeTopviewDll();
}

BOOL TVDataIO::CheckInit()
{
	return m_bInited;
}

void TVDataIO::LoadTopviewDll()
{
	m_hTopViewDll	= ::LoadLibrary("TDXTVData.dll");
}

void TVDataIO::FreeTopviewDll()
{
	if(!m_bReleased)	
		m_FuncRelease();
	m_bReleased			= TRUE;
	m_FuncInit			= NULL;
	m_FuncGetInterface	= NULL;
	m_FuncReceiveData	= NULL;
	m_FuncRelease		= NULL;
	if(m_hTopViewDll)	
		::FreeLibrary(m_hTopViewDll);
	m_hTopViewDll		= NULL;
	m_bInited			= FALSE;
	m_bDataLoaded		= FALSE;
	m_pDataCore			= NULL;
}

BOOL TVDataIO::GetTopviewData()
{
	if(!m_bInited) return FALSE;
	m_FuncReceiveData();
	return TRUE;
}

void *TVDataIO::GetData(DWORD dwDataType,DWORD dwCycleType,const char *pszMarketCode,DWORD dwDate,DWORD *pdwCount,BOOL IsGetLast)
{
	if(pdwCount != NULL) (*pdwCount) = 0;
	if(!m_bInited || !m_bDataLoaded) 
		return NULL;
	switch(dwDataType)
	{
	case TDX_TV_BSVOL:
	case TDX_TV_CAPITALABDFSTAT:
	case TDX_TV_CAPITALVOLSTAT:
	case TDX_TV_TOP10:
	case TDX_TV_DEPARTSTAT:
	case TDX_TV_VOL:
	case TDX_TV_TRADEABDFSTAT:
	case TDX_TV_TRADEABDFMKT:
		switch(dwCycleType)
		{
		case TDP_DAY:
		case TDP_WEEK:
		case TDP_MONTH:
			break;
		default:
			return NULL;
		}
		break;
	default:
		return NULL;
	}
	if(pszMarketCode==NULL && dwDate==0) 
		return NULL;
	else if(pszMarketCode==NULL) 
		return m_pDataCore->GetData(dwDataType,dwCycleType,dwDate,pdwCount,IsGetLast);
	else if(dwDate==0) 
		return m_pDataCore->GetData(dwDataType,dwCycleType,pszMarketCode,pdwCount);
	return m_pDataCore->GetData(dwDataType,dwCycleType,pszMarketCode,dwDate,IsGetLast);
}

BOOL TVDataIO::GetSeat(const char *pszSeatId,char* pszSeatName,int nMaxSeatNameLen,BYTE *cSeatType)
{
	if(!m_bInited || !m_bDataLoaded) 
		return NULL;
	(*cSeatType) = m_pDataCore->GetSeatType(pszSeatId);
	if(pszSeatName==NULL || nMaxSeatNameLen<=0) 
		return TRUE;
	return m_pDataCore->GetSeatName(pszSeatId,pszSeatName,nMaxSeatNameLen);
}
