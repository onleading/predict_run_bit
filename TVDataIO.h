#ifndef __TDX__TOPVIEW__DATA__ENGINE__H__
#define __TDX__TOPVIEW__DATA__ENGINE__H__

#include "TDXTVData.h"

#define TDX_TV_BSVOL			TDT_0301
#define TDX_TV_CAPITALABDFSTAT	TDT_1013
#define TDX_TV_CAPITALVOLSTAT	TDT_1001
#define TDX_TV_TOP10			TDT_0401
#define TDX_TV_DEPARTSTAT		TDT_0801
#define TDX_TV_VOL				TDT_0101
#define TDX_TV_TRADEABDFSTAT	TDT_0501
#define TDX_TV_TRADEABDFMKT		TDT_0502

#define TDX_TV_GRAPH_ABDFSTAT	0
#define TDX_TV_GRAPH_CAPSTAT4	1
#define TDX_TV_GRAPH_CAPSTAT9	2
#define TDX_TV_GRAPH_TOP10		3
//
#define TDX_TV_GRAPH_ACC		4			//ABDFSTAT+CAPSTAT4+CAPSTAT9
#define TDX_TV_GRAPH_ACCT		5			//ABDFSTAT+CAPSTAT4+CAPSTAT9+TOP10

class TVDataIO
{
private:
	BOOL		m_bInited;
	BOOL		m_bReleased;
	BOOL		m_bDataLoaded;

	IZxwDataManager	*m_pDataCore;

	HINSTANCE	m_hTopViewDll;
	char		m_HomePath[MAX_PATH];
	DWORD		m_dwUserID;
	char		m_szPassword[MAX_PATH];
	char		m_szServerIP[32];
	DWORD		m_dwServerPort;
	DWORD		m_dwProvinceID;
private:
	void	LoadTopviewDll();			//����TopViewģ��
	void	FreeTopviewDll();			//�ͷ�TopViewģ��
public:
	DllInit					m_FuncInit;
	DllGetZXWDataInterface	m_FuncGetInterface;
	DllReceiveData			m_FuncReceiveData;
	DllRelease				m_FuncRelease;
public:
	TVDataIO(DWORD dwUserId,char *szPassword,char *szServerIp,DWORD dwServerPort,DWORD dwProvinceId);
	~TVDataIO();
	BOOL	CheckInit();
	BOOL	GetTopviewData();			//����TopView����
	//�������󲿷�	pszMarketCodeΪNULLʱ����ȫ�г�����,dwDate=0ʱ����ָ����Ʊ��������,���߶���0(NULL)ʱ,����NULL
	void *	GetData(DWORD dwDataType,DWORD dwCycleType,const char *pszMarketCode,DWORD dwDate,DWORD *pdwCount,BOOL IsGetLast = FALSE);
	//ϯλ������Ϣ
	BOOL	GetSeat(const char *pszSeatId,char* pszSeatName,int nMaxSeatNameLen,BYTE *cSeatType);
};

extern class TVDataIO *g_pTVDataIO;

#endif