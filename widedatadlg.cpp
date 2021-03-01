// WideDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "WideDataDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWideDataDlg dialog


CWideDataDlg::CWideDataDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWideDataDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWideDataDlg)
	//}}AFX_DATA_INIT
	m_nType = WIDEDATA_HQ;
	m_nMainID = g_nWideDataMainID;
	m_strDesc = "";

	m_nTotalRow = 0;
	m_bGetDSData = FALSE;
	
	m_pLSetInfo = NULL;
	m_bMsgAfterDeath = FALSE;
}


void CWideDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWideDataDlg)
	DDX_Control(pDX, IDC_PROGRESS, m_ctlProgress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWideDataDlg, CDialog)
	//{{AFX_MSG_MAP(CWideDataDlg)
	ON_MESSAGE(UM_GETDATA_ACK,OnGetDataAck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWideDataDlg message handlers

BOOL CWideDataDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	

	if(m_nType==WIDEDATA_HQ)
	{
		m_strDesc = _F("正在刷新当前行情...");
		SetWindowText(_F("刷新数据"));
	}
	else if(m_nType==WIDEDATA_SORT)
	{
		m_strDesc = _F("正在刷新当前行情...");
		SetWindowText(_F("刷新数据"));
	}
	else if(m_nType==WIDEDATA_IMG_STAT || m_nType==WIDEDATA_TCK_STAT)
	{
		m_strDesc = _F("正在刷新统计数据...");
		SetWindowText(_F("刷新数据"));
	}
	m_nStockNum = m_nTotalRow;
	m_ctlProgress.SetRange32(0,m_nStockNum);
	GetDlgItem(IDC_STATIC1)->SetWindowText(m_strDesc);

	m_nCurGPNo  = 0;
	ContinueWant();

	return TRUE;
}

void CWideDataDlg::OnOK() 
{
}

void CWideDataDlg::OnCancel()
{
	g_nWideDataMainID = m_nMainID; //退出前，改变g_nWideDataMainID
	CDialog::OnCancel();
}

void CWideDataDlg::ContinueWant() //不断请求数据
{
	MemStkInfo *pStkInfo;
	char *		pTranBuff = new char[REQBUFFER_LEN];
	int			gpnum = 0;

	if(m_nType==WIDEDATA_HQ)			//发完全行情数据请求
	{
		MULIDX	MulIdx[GRIDHQ_MAXNUM]={0};
		int		nDSNum = 0, nABNum = 0;
		g_nMainID = MainIDPlus(m_nMainID,WIDEDATA_WIN);
		g_nAssisID = WIDEHQ_ASSISID;

		if(m_nCurGPNo>=m_nStockNum && !m_bGetDSData)				//请求DS
		{
			m_nCurGPNo = 0;
			m_bGetDSData = TRUE;
		}
		else if(m_nCurGPNo>=m_nStockNum && m_bGetDSData)
		{
			m_nCurGPNo = 0;
			m_bGetDSData = FALSE;
		}

		while(!m_bGetDSData && m_nCurGPNo < m_nStockNum && nABNum < GRIDHQ_MAXNUM)
		{
			pStkInfo = (*g_pStockIO)[lpnUsedStkIndex[m_nCurGPNo]];
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
			g_pGetData->GetMultiCurrStockDataEx(pTranBuff,nABNum);
		else if(!m_bGetDSData)
		{
			m_nCurGPNo = 0;
			m_bGetDSData = TRUE;
		}
		while(m_bGetDSData && m_nCurGPNo < m_nStockNum && nDSNum < GRIDHQ_MAXNUM)
		{
			pStkInfo = (*g_pStockIO)[lpnUsedStkIndex[m_nCurGPNo]];
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
			g_pGetData->DSGetMulCurrData(MulIdx,nDSNum);
		if(nABNum<1&&nDSNum<1)
		{
			PostMessage(WM_COMMAND,IDCANCEL);
			delete pTranBuff;
			return;
		}
	}
	else if(m_nType==WIDEDATA_SORT&&m_pLSetInfo&&m_pLSetInfo->bUsed)
	{
		g_nMainID = MainIDPlus(m_nMainID,WIDEDATA_WIN);
		g_nAssisID = WIDEHQ_ASSISID;
		if(m_pLSetInfo->WhichGpMarket<DS_START)
		{
			g_pGetData->GetMultiCurrStockDataEx(m_pLSetInfo->WhichGpMarket,m_pLSetInfo->ColType,
				m_pLSetInfo->nStartRow,min(GRIDHQ_MAXNUM,m_pLSetInfo->nValidRow),m_pLSetInfo->SortType);
		}
		else if(m_pLSetInfo->WhichGpMarket<FY_START)
		{
			int MktType=g_pDSMarket[m_pLSetInfo->WhichGpMarket-DS_START].MktType;
			g_pGetData->DSGetMulData(MktType,m_pLSetInfo->ColType,
				m_pLSetInfo->nStartRow,m_pLSetInfo->nValidRow,m_pLSetInfo->SortType);
		}
	}
	else if(m_nType==WIDEDATA_IMG_STAT || m_nType==WIDEDATA_TCK_STAT)
	{
		g_nMainID = MainIDPlus(m_nMainID,WIDEDATA_WIN);
		g_nAssisID = WIDEHQ_ASSISID;
		SUMIMGSTAT	ImgStat;
		SUMTCKSTAT	TckStat;
		int			tmpLen=1+SH_CODE_LEN+sizeof(DWORD);
		while(m_nCurGPNo < m_nStockNum && gpnum < MAXSTAT_MAXNUM)
		{
			pStkInfo = (*g_pStockIO)[lpnUsedStkIndex[m_nCurGPNo]];
			if(pStkInfo && !(g_pStockIO->BeDSStock(pStkInfo)))
			{
				if( (m_nType==WIDEDATA_IMG_STAT && time(NULL)-pStkInfo->nImgStatLastSeconds>30) || 
					(m_nType==WIDEDATA_TCK_STAT && time(NULL)-pStkInfo->nTckStatLastSeconds>30)
				  )
				{
					memcpy(pTranBuff+gpnum*tmpLen,&(pStkInfo->setcode),1);
					memcpy(pTranBuff+gpnum*tmpLen+1,pStkInfo->Code,SH_CODE_LEN);
					g_pStockIO->ReadCurrStatData(pStkInfo,&ImgStat,&TckStat);
					if(m_nType==WIDEDATA_IMG_STAT)
						memcpy(pTranBuff+gpnum*tmpLen+1+SH_CODE_LEN,&ImgStat.RefreshNum,sizeof(DWORD));
					else
						memcpy(pTranBuff+gpnum*tmpLen+1+SH_CODE_LEN,&TckStat.CJBS,sizeof(DWORD));
					gpnum++;
				}
			}
			m_nCurGPNo++;
		}
		if(gpnum>0 && OnLine)
			g_pGetData->GetStatData(pTranBuff,gpnum,(m_nType==WIDEDATA_IMG_STAT));
		else
			PostMessage(WM_COMMAND,IDCANCEL);
	}
	delete pTranBuff;
}

void CWideDataDlg::OnGetDataAck(WPARAM wParam,LPARAM lParam)
{
	static struct CurrStockDataEx WideData_StockDataExBuf[GRIDHQ_MAXNUM];
	if(m_nMainID!=g_AnsHeader.MainID&&m_nMainID!=g_AnsHeader3.MainID) return;
	if(m_nStockNum <= 0) 
	{
		PostMessage(WM_COMMAND,IDCANCEL);
		return;
	}

	//显示信息
	CString tmpStr;
	tmpStr.Format("%s[%d%%]",m_strDesc,m_nCurGPNo*100/m_nStockNum);
	GetDlgItem(IDC_STATIC1)->SetWindowText(tmpStr);
	m_ctlProgress.SetPos(m_nCurGPNo);

	if(g_AnsHeader.AssisID!=WIDEHQ_ASSISID&&g_AnsHeader3.AssisID!=WIDEHQ_ASSISID)
		return;

	if(m_nType==WIDEDATA_HQ)										//完全行情数据
	{
		if(wParam==1&&g_AnsHeader3.AssisID==WIDEHQ_ASSISID)
		{
			MULIDX MulHQData[GRIDHQ_MAXNUM]={0};
			g_pGetData->DSGetMulCurrDataACK(MulHQData, GRIDHQ_MAXNUM);
		}
		else if(g_AnsHeader.AssisID==WIDEHQ_ASSISID)
		{
			g_pGetData->GetMultiCurrStockDataExACK(WideData_StockDataExBuf,GRIDHQ_MAXNUM);
		}
		if(m_nCurGPNo >= m_nStockNum && m_bGetDSData)
		{
			PostMessage(WM_COMMAND,IDCANCEL);
			return;
		}
		Sleep(5); //加入延时,以免有时发数据太急造成网络卡包
		ContinueWant();
	}
	if(m_nType==WIDEDATA_SORT)										//完全行情数据(排序用)
	{
		if(m_pLSetInfo->WhichGpMarket<DS_START)
		{
			CurrStockDataEx StockDataExBuf[GRIDHQ_MAXNUM]={0};
			g_pGetData->GetMultiCurrStockDataExACK(StockDataExBuf,GRIDHQ_MAXNUM);
			for(int j = 0,tempIndex=0;j < min(GRIDHQ_MAXNUM,m_pLSetInfo->nValidRow);j++)
			{
				tempIndex = g_pStockIO->GetIndex(StockDataExBuf[j].Code,StockDataExBuf[j].SetCode);
				m_pLSetInfo->lpnUsedStkIndex[(m_pLSetInfo->nStartRow+j+m_pLSetInfo->nTolRow)%m_pLSetInfo->nTolRow] = tempIndex;
			}
		}
		else if(m_pLSetInfo->WhichGpMarket<FY_START)
		{
			MULIDX  MulIdx[GRIDHQ_MAXNUM]={0};
			g_pGetData->DSGetMulCurrDataACK(MulIdx,GRIDHQ_MAXNUM);
			MemStkInfo *pInfo = NULL;
			for(int j = 0;j < min(GRIDHQ_MAXNUM,m_pLSetInfo->nValidRow);j++)
			{
				pInfo = g_pStockIO->GetDSStkInfo(MulIdx[j].Market,MulIdx[j].Code);
				if(pInfo==NULL) continue;
				m_pLSetInfo->lpnUsedStkIndex[(m_pLSetInfo->nStartRow+j+m_pLSetInfo->nTolRow)%m_pLSetInfo->nTolRow] = pInfo->Index;
			}
		}
		PostMessage(WM_COMMAND,IDCANCEL);
		if(m_bMsgAfterDeath == TRUE)
			m_pLSetInfo->pMsgRtnWnd->PostMessage(m_LastMsg.message, m_LastMsg.wParam, m_LastMsg.lParam);
	}
	if(m_nType==WIDEDATA_IMG_STAT || m_nType==WIDEDATA_TCK_STAT)	//完全统计行情数据
	{
		g_pGetData->GetStatDataAck(m_nType==WIDEDATA_IMG_STAT);
		if(m_nCurGPNo >= m_nStockNum)
		{
			PostMessage(WM_COMMAND,IDCANCEL);
			return;
		}
		Sleep(5); //加入延时,以免有时发数据太急造成网络卡包
		ContinueWant();
	}
}

BOOL CWideDataDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(m_nType==WIDEDATA_SORT&&pMsg->message==WM_KEYUP)
	{
		m_bMsgAfterDeath = TRUE;
		memcpy(&m_LastMsg, pMsg, sizeof(MSG));
	}
	return CDialog::PreTranslateMessage(pMsg);
}
