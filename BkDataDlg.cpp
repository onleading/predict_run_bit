// BkDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "BkDataDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBkDataDlg dialog

CBkDataDlg::CBkDataDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBkDataDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBkDataDlg)
	//}}AFX_DATA_INIT
	m_bOneProcessing = FALSE;
	m_pBkData = NULL;

	memset(&m_CurReq,0,sizeof(struct BkData_Req));

	m_nMainID = 0;
	m_bGetDSData = FALSE;
	m_nCurGPNo  = 0;

	data_filelen = 0;
	data_pos = 0;
	FileBuffer = NULL;
	m_strSyncFile[0]=0;
	m_strSyncSaveFile[0]=0;
}


void CBkDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBkDataDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBkDataDlg, CDialog)
	//{{AFX_MSG_MAP(CBkDataDlg)
	ON_WM_TIMER()
	ON_MESSAGE(UM_GETDATA_ACK,OnGetDataAck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBkDataDlg message handlers

void CBkDataDlg::SetBkDataObject(CBkData *pBkData)
{
	m_pBkData = pBkData;
}

BOOL CBkDataDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
		
	SetTimer(0x7,100,NULL);	//0.1秒检查一下缓冲区

	return TRUE;
}

void CBkDataDlg::OnCancel() 
{	
	StopTimeOutTimer();
}

void CBkDataDlg::OnOK() 
{
}

void CBkDataDlg::PostNcDestroy() 
{	
	TDEL(m_CurReq.lpnUsedStkIndex);
	TDEL(FileBuffer);
	delete this;
	g_pBKDataDlg = NULL;
}

void CBkDataDlg::ProcessOneSubscrible()
{
	BOOL bRet = TRUE;
	m_bOneProcessing = TRUE;
	m_nCurGPNo  = 0;
	m_bGetDSData = FALSE;
	ContinueWant();
	if(!bRet)
		m_bOneProcessing = FALSE;
}

void CBkDataDlg::AfterGetData()
{
	::PostMessage(m_CurReq.hWnd,m_CurReq.nMsgID,m_CurReq.wParam,m_CurReq.lParam);
	m_bOneProcessing = FALSE;
}

void CBkDataDlg::SetTimeOutTimer()
{
	KillTimer(0x8);
	SetTimer(0x8,BK_TIMEOUT_SECONDS*1000,NULL);
}

void CBkDataDlg::StopTimeOutTimer()
{
	KillTimer(0x8);
}

//不断请求数据
void CBkDataDlg::ContinueWant()
{
	if(!m_bOneProcessing) 
		return;
	MemStkInfo *pStkInfo;
	char *		pTranBuff = new char[REQBUFFER_LEN];
	int			gpnum = 0;

	if(m_CurReq.nType==WIDEDATA_HQ)													//发完全行情数据请求
	{
		MULIDX	MulIdx[GRIDHQ_MAXNUM]={0};
		int		nDSNum = 0, nABNum = 0;
		g_nMainID = MainIDPlus(m_nMainID,BKDATA_WIN);
		g_nAssisID = BKDATA_ASSISID;

		if(m_nCurGPNo>=m_CurReq.nStkNum && !m_bGetDSData)
		{
			m_nCurGPNo = 0;
			m_bGetDSData = TRUE;
		}
		else if(m_nCurGPNo>=m_CurReq.nStkNum && m_bGetDSData)
		{
			m_nCurGPNo = 0;
			m_bGetDSData = FALSE;
		}

		while(!m_bGetDSData && m_nCurGPNo < m_CurReq.nStkNum && nABNum < GRIDHQ_MAXNUM)
		{
			pStkInfo = (*g_pStockIO)[m_CurReq.lpnUsedStkIndex[m_nCurGPNo]];
			if(!pStkInfo) continue;
			if(IsLocalStock(pStkInfo))
			{
				m_nCurGPNo++;
				continue;
			}
			if(g_pStockIO->BeDSStock(pStkInfo))
			{
			}
			else
			{
				memcpy(pTranBuff+nABNum*(1+SH_CODE_LEN),&(pStkInfo->setcode),1);
				memcpy(pTranBuff+nABNum*(1+SH_CODE_LEN)+1,pStkInfo->Code,SH_CODE_LEN);
				nABNum++;
			}
			m_nCurGPNo++;
		}
		if(nABNum>0 && OnLine)
		{
			g_pGetData->GetMultiCurrStockDataEx(pTranBuff,nABNum);
			SetTimeOutTimer();
		}
		else if(!m_bGetDSData)
		{
			m_nCurGPNo = 0;
			m_bGetDSData = TRUE;
		}
		while(m_bGetDSData && m_nCurGPNo < m_CurReq.nStkNum && nDSNum < GRIDHQ_MAXNUM)
		{
			pStkInfo = (*g_pStockIO)[m_CurReq.lpnUsedStkIndex[m_nCurGPNo]];
			if(!pStkInfo) continue;
			if(g_pStockIO->BeDSStock(pStkInfo))
			{
				MulIdx[nDSNum].Market = pStkInfo->setcode;
				memcpy(MulIdx[nDSNum].Code,pStkInfo->Code,CODE_SIZE);
				nDSNum++;
			}
			else 
			{				
			}
			m_nCurGPNo++;
		}
		if(nDSNum>0&&DSOnLine)
		{
			g_pGetData->DSGetMulCurrData(MulIdx,nDSNum);
			SetTimeOutTimer();
		}
		if(nABNum<1&&nDSNum<1)
		{
			AfterGetData();
			delete pTranBuff;
			return;
		}
	}
	delete pTranBuff;
}

BOOL CBkDataDlg::TryFileLenReq()
{
	g_nMainID = MainIDPlus(m_nMainID,BKDATA_WIN);
	g_nAssisID = SYNCDATALEN_ASSISID;
	BOOL bRet = g_pGetData->GetFileLen(m_strSyncFile);
	if(bRet)
		SetTimeOutTimer();
	return bRet;
}

void CBkDataDlg::OnGetDataAck(WPARAM wParam,LPARAM lParam)
{
	static struct CurrStockDataEx BkData_StockDataExBuf[GRIDHQ_MAXNUM];
	if(m_nMainID!=g_AnsHeader.MainID&&m_nMainID!=g_AnsHeader3.MainID) 
		return;
	if( g_AnsHeader.AssisID!=BKDATA_ASSISID && g_AnsHeader3.AssisID!=BKDATA_ASSISID && 
		g_AnsHeader.AssisID!=SYNCDATALEN_ASSISID && g_AnsHeader.AssisID!=SYNCDATA_ASSISID)
		return;
	StopTimeOutTimer();
	if(m_CurReq.nType==WIDEDATA_HQ)													//完全行情数据
	{
		if(m_CurReq.nStkNum <= 0) 
		{
			AfterGetData();
			return;
		}
		if(wParam==1&&g_AnsHeader3.AssisID==BKDATA_ASSISID)
		{
			MULIDX MulHQData[GRIDHQ_MAXNUM]={0};
			g_pGetData->DSGetMulCurrDataACK(MulHQData, GRIDHQ_MAXNUM);
		}
		else if(g_AnsHeader.AssisID==BKDATA_ASSISID)
		{
			g_pGetData->GetMultiCurrStockDataExACK(BkData_StockDataExBuf,GRIDHQ_MAXNUM);
		}
		if(m_nCurGPNo >= m_CurReq.nStkNum && m_bGetDSData)
		{
			AfterGetData();
			return;
		}
		Sleep(5); //加入延时,以免有时发数据太急造成网络卡包
		ContinueWant();
	}
}

void CBkDataDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==0x7 && m_pBkData)
	{
		if(OnLine && !m_bOneProcessing && m_pBkData->GetOneSubscrible(&m_CurReq))
			ProcessOneSubscrible();
		if(m_bOneProcessing && !OnLine)
			m_bOneProcessing=FALSE;
	}
	if(nIDEvent==0x8)
	{
		StopTimeOutTimer();	
		m_bOneProcessing=FALSE;
	}
	CDialog::OnTimer(nIDEvent);
}
