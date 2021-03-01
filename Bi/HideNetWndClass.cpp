// HideNetWndClass.cpp : implementation file
//

#include "stdafx.h"
#include "IDefine.h"
#include "HostAHq.h"
#include "HideNetWnd.h"
#include "HideNetWndClass.h"
#include "CoopCache.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHideNetWndClass

CHideNetWndClass::CHideNetWndClass(CHideNetWnd *pOwner)
{
	m_pOwnerThread = pOwner;
}

CHideNetWndClass::~CHideNetWndClass()
{
	
}

BEGIN_MESSAGE_MAP(CHideNetWndClass, CWnd)
	//{{AFX_MSG_MAP(CHideNetWndClass)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
extern CCoopCache	g_iReqQueue;
extern void		LoadCalcQueue(), LoadTestQueue();
void CHideNetWndClass::OnTimer(UINT nIDEvent)
{
	char strError[256]={0};
	switch(nIDEvent)
	{
	case DATA_HOSTA:
		{
			if(!m_pOwnerThread||!m_pOwnerThread->m_pHq||m_pOwnerThread->m_pHq->m_nSourceType!=DATA_HOSTA) 
				break;
			CHostAHq *pHq = (CHostAHq*)m_pOwnerThread->m_pHq;

			ReqItem ri={0};
			if(g_iReqQueue.PeekCache(ri))
			{
				//¼ÇÂ¼Ê±¼ä
				time_t tt = time(NULL);
				char strRefreshTimet[20]={0};
				sprintf(strRefreshTimet, "%d", tt);

				if(ri.Currency[0]&&ri.nAction==ACTION_CALC)
					pHq->TradeCalc(ri.Currency);
				else if(ri.Currency[0]&&ri.nAction==ACTION_TEST)
				{
					WritePrivateProfileString("POOL","LASTTEST",strRefreshTimet,g_WSXHStr+"PoolInfo_Bit.ini");
					pHq->TradeFunc_bitc("FAOOVOL_NOPOS",ri.Currency);
					LoadTestQueue();
				}
				else if(ri.Currency[0]==0&&ri.nAction==ACTION_TEST)
				{
					Sleep(1000);
					WritePrivateProfileString("POOL","LASTTEST",strRefreshTimet,g_WSXHStr+"PoolInfo_Bit.ini");
					exit(-1);
				//	LoadCalcQueue();
				}
				else if(ri.Currency[0]==0&&ri.nAction==ACTION_CALC)
				{
					Sleep(1000);
					long nLastTest = GetPrivateProfileInt("POOL", "LASTTEST", 0, g_WSXHStr+"PoolInfo_Bit.ini");
					if(time(NULL)-nLastTest>12*60*60)
						LoadTestQueue();
					else 
						LoadCalcQueue();
				}
			}
		}
		break;
	default:
		break;
	}
	
	CWnd::OnTimer(nIDEvent);
}
