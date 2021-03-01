#include "StdAfx.h"
#include "BkData.h"
#include "BkDataDlg.h"

CWnd *				g_pBKDataDlg = NULL;
CCriticalSection	l_BkDataCs;

CBkData::CBkData()
{
	memset(m_Req,0,sizeof(struct BkData_Req)*MAX_BKDATA_REQNUM);
	m_nReqNum = 0;
}

CBkData::~CBkData()
{
	for(int i=0;i<MAX_BKDATA_REQNUM;i++)
	{
		TDEL(m_Req[i].lpnUsedStkIndex);
	}
}

BOOL CBkData::Create(CWnd *pMainWnd)
{
	g_pBKDataDlg = new CBkDataDlg(pMainWnd);
	((CBkDataDlg *)g_pBKDataDlg)->Create(IDD_BKDATA_DIALOG);
	((CBkDataDlg *)g_pBKDataDlg)->SetBkDataObject(this);
	g_pBKDataDlg->ShowWindow(SW_HIDE);
	return TRUE;
}

//nCancelFlag 0:常规 1:能被后来者取消 2:取消以前的同类nType
BOOL CBkData::Subscrible(HWND hWnd,UINT nMsgID,UINT wParam,UINT lParam,int nType,short *lpnUsedStkIndex,short nStkNum,int nCancelFlag,char *strCode,struct IW_Req *pIWReq)
{
	//必须行情在线才能订阅
	if(nStkNum<=0 || m_nReqNum>=MAX_BKDATA_REQNUM || !OnLine) 
		return FALSE;
	l_BkDataCs.Lock();
	//取消可以取消的队列
	for(int i=0;i<m_nReqNum;i++)
	{
		if(m_Req[i].nCancelFlag==1 || (nCancelFlag==2 && m_Req[i].nType==nType))
		{
			TDEL(m_Req[i].lpnUsedStkIndex);
			if(i<MAX_BKDATA_REQNUM-1)
			{
				memmove(m_Req+i,m_Req+i+1,sizeof(struct BkData_Req)*(MAX_BKDATA_REQNUM-i-1));
				m_Req[MAX_BKDATA_REQNUM-1].lpnUsedStkIndex = NULL;
			}
			m_nReqNum--;
		}
	}
	//加入到队列
	struct BkData_Req *pOne = m_Req+m_nReqNum;
	TDEL(pOne->lpnUsedStkIndex);

	pOne->hWnd		= hWnd;
	pOne->nMsgID	= nMsgID;
	pOne->wParam	= wParam;
	pOne->lParam	= lParam;
	pOne->nType		= nType;
	if(strCode)
	{
		strncpy(pOne->strCode,strCode,10);
		pOne->strCode[10]=0;
	}
	else
		pOne->strCode[0]=0;
	if(pIWReq)
		memcpy(&pOne->IWReq,pIWReq,sizeof(IW_Req));
	else
		memset(&pOne->IWReq,0,sizeof(IW_Req));
	pOne->nCancelFlag= nCancelFlag;
	pOne->nStkNum	= nStkNum;
	pOne->lpnUsedStkIndex = new short[nStkNum];
	memcpy(pOne->lpnUsedStkIndex,lpnUsedStkIndex,sizeof(short)*nStkNum);	
	m_nReqNum++;

	l_BkDataCs.Unlock();
	return TRUE;
}

BOOL CBkData::GetOneSubscrible(struct BkData_Req *pReq)
{
	if(!pReq || m_nReqNum<=0) 
		return FALSE;

	l_BkDataCs.Lock();

	TDEL(pReq->lpnUsedStkIndex);
	pReq->hWnd		= m_Req[0].hWnd;
	pReq->nMsgID	= m_Req[0].nMsgID;
	pReq->wParam	= m_Req[0].wParam;
	pReq->lParam	= m_Req[0].lParam;
	pReq->nType		= m_Req[0].nType;
	strncpy(pReq->strCode,m_Req[0].strCode,10);
	pReq->strCode[10]=0;
	pReq->IWReq		= m_Req[0].IWReq;
	pReq->nCancelFlag= m_Req[0].nCancelFlag;
	pReq->nStkNum	= m_Req[0].nStkNum;
	pReq->lpnUsedStkIndex = new short[m_Req[0].nStkNum];
	memcpy(pReq->lpnUsedStkIndex,m_Req[0].lpnUsedStkIndex,sizeof(short)*m_Req[0].nStkNum);	
	
	TDEL(m_Req[0].lpnUsedStkIndex);
	memmove(m_Req,m_Req+1,sizeof(struct BkData_Req)*(MAX_BKDATA_REQNUM-1));
	m_Req[MAX_BKDATA_REQNUM-1].lpnUsedStkIndex = NULL;
	m_nReqNum--;

	l_BkDataCs.Unlock();
	return TRUE;
}

