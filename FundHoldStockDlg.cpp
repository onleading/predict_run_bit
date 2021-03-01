// FundHoldStockDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "FundHoldStockDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFundHoldStockDlg dialog

CFundHoldStockDlg::CFundHoldStockDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFundHoldStockDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFundHoldStockDlg)
	//}}AFX_DATA_INIT
	m_pFundHoldStock = NULL;
	m_pFundBase = NULL;
	m_strTitle = "基金分析平台";
	memset(m_lpszSymbol,0,7);
	m_nType = HOLDSTOCK;
	m_nFlag = 0;
	vtPoiName.clear();
}


void CFundHoldStockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFundHoldStockDlg)
	DDX_Control(pDX, IDC_LIST1, m_list1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFundHoldStockDlg, CDialog)
	//{{AFX_MSG_MAP(CFundHoldStockDlg)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, OnColumnclickList1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnItemchangedList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFundHoldStockDlg message handlers

BOOL CFundHoldStockDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_pFundHoldStock = g_pFundHoldStock;
	m_pFundBase = g_pFundBase;
	m_nFund = g_nFundNum;
	m_nFHSNum = g_nHoldNum;

	SetWindowText(_F(m_strTitle));
	m_Pie.SubclassDlgItem(IDC_HOLDSTOCK, this); 
	m_Pie.SetBackColor(RGB(255,255,255));
	m_Pie.SetBackDefaut(RGB(255,255,255));
	if(HOLDSTOCK == m_nType)
		m_Pie.SetTitle(_F("重仓持股基金情况\r\n(以列表中列出的持股总数为基准)"));
	else
		m_Pie.SetTitle(_F("基金十大持股情况"));
	m_Pie.Reset();

	//股票列表框
	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_GP));
	HICON hIcon2 = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_JJ));
	if(HOLDSTOCK == m_nType)
		m_ImageList.Add(hIcon2);
	else
		m_ImageList.Add(hIcon);
	m_list1.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_list1.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	Initlist();
	FillCtl(m_lpszSymbol);
	if(HOLDSTOCK == m_nType)
	{
		m_list1.m_nSortedCol = 3;
		m_list1.m_fAsc = FALSE;
	}
	return TRUE;
}

void CFundHoldStockDlg::Initlist()
{
	if(HOLDSTOCK == m_nType)
	{
		const short nNum = 6;
		LPCSTR lpszHeadName[nNum] = {"序号","基金名称","本期持股(万)","占上市公司%","占基金净值%","上期持股(万)"};
		int lpnColWidth[nNum] = {40,110,90,80,80,85};
		for(int i=0;i<nNum;i++)
		{
			m_list1.InsertColumn(i,_F(lpszHeadName[i]),LVCFMT_LEFT,lpnColWidth[i]);
		}
	}
	else
	{
		const short nNum = 6;
		LPCSTR lpszHeadName[nNum] = {"序号","股票代码","股票名称","本期数量","占基金%","上期数量"};
		int lpnColWidth[nNum] = {40,70,80,90,80,90};
		for(int i=0;i<nNum;i++)
		{
			m_list1.InsertColumn(i,_F(lpszHeadName[i]),LVCFMT_LEFT,lpnColWidth[i]);
		}
	}
}

int ZGBRateSort(const void* a,const void * b)
{
	HOLD_FUNDINFO * sorta,*sortb;
	sortb = (HOLD_FUNDINFO*)b;
	sorta = (HOLD_FUNDINFO*)a;
	if(sorta->fZGBRate < sortb->fZGBRate) return 1;
	if(sorta->fZGBRate > sortb->fZGBRate) return -1;
	return 0;
}

COLORREF lprgb[11] = {RGB(230,20,20),RGB(232,175,20),RGB(43,61,6),RGB(54,205,39),RGB(39,205,164),
					RGB(39,148,205),RGB(27,69,228),RGB(136,27,228),RGB(202,27,228),RGB(228,27,93),RGB(0,0,255)};

void CFundHoldStockDlg::FillCtl(const char* lpszSymbol)
{
	GetFundBaseData();
	m_pFundHoldStock = g_pFundHoldStock;
	m_pFundBase = g_pFundBase;
	m_nFund = g_nFundNum;
	m_nFHSNum = g_nHoldNum;	
	if(!m_pFundHoldStock || !m_pFundBase) 
		return;
	CString strtmp;
	int tmpvalue;
	MemStkInfo *tempStkInfo = NULL;
	vtPoiName.clear();
	if(HOLDSTOCK == m_nType)
	{
		int nIndex = 0;
		int nFundIndex = 0;
		int nSubItem;

		vttmp.clear();
		HOLD_FUNDINFO sttmp;
		memset(&sttmp,0,sizeof(HOLD_FUNDINFO));

		tempStkInfo = g_pStockIO->Get_MemStkInfo(lpszSymbol);
		if(NULL == tempStkInfo) return;
		for(int i=0;i<m_nFHSNum;i++)
		{
			for(int j = 0;j<m_pFundHoldStock[i].nNum;j++)
			{
				if(strcmp(m_pFundHoldStock[i].stockinfo[j].szSymbol,lpszSymbol) != 0)
					continue;
				for(nFundIndex=0;nFundIndex<m_nFund;nFundIndex++)
				{
					if(0 != strcmp(m_pFundHoldStock[i].szSymbol,m_pFundBase[nFundIndex].szSymbol))
						continue;
					strcpy(sttmp.szName,m_pFundBase[nFundIndex].szName);
					break;
				}
				if(nFundIndex >= m_nFund)	strcpy(sttmp.szName,m_pFundHoldStock[i].szSymbol);

				sttmp.fAmount = m_pFundHoldStock[i].stockinfo[j].fAcount;
										
				sttmp.fZGBRate = m_pFundHoldStock[i].stockinfo[j].fAcount*100/(tempStkInfo->J_zgb*10000);
				
				if(nFundIndex >= m_nFund || m_pFundBase[nFundIndex].fTotalNet<COMPPREC) 
					sttmp.fNetRate = MEANLESS_DATA;
				else
					sttmp.fNetRate = m_pFundHoldStock[i].stockinfo[j].fMarketValue*100/m_pFundBase[nFundIndex].fTotalNet;
				
				sttmp.fLastAmount = m_pFundHoldStock[i].stockinfo[j].fLastAcount;
				vttmp.push_back(sttmp);
				m_list1.SetItemData(nIndex,nIndex);
				nIndex++;
			}
		}
		qsort((void*)vttmp.begin(),vttmp.size(),sizeof(HOLD_FUNDINFO),ZGBRateSort);
		for(i=0;i<vttmp.size();i++)
		{
			nSubItem = 1;
			strtmp.Format("%d",i+1);
			m_list1.InsertItem(i,strtmp);
			
			m_list1.SetItemText(i,nSubItem++,_F(vttmp[i].szName));

			strtmp.Format("%.2f",vttmp[i].fAmount/10000);
			m_list1.SetItemText(i,nSubItem++,strtmp);

			if(MEANLESS_DATA == vttmp[i].fZGBRate)
				m_list1.SetItemText(i,nSubItem++,"--");
			else
			{
				strtmp.Format("%.2f",vttmp[i].fZGBRate);
				m_list1.SetItemText(i,nSubItem++,strtmp);
			}
			
			if(MEANLESS_DATA == vttmp[i].fNetRate)
				m_list1.SetItemText(i,nSubItem++,"--");
			else
			{
				strtmp.Format("%.2f",vttmp[i].fNetRate);
				m_list1.SetItemText(i,nSubItem++,strtmp);
			}
			
			strtmp.Format("%.2f",vttmp[i].fLastAmount/10000);
			m_list1.SetItemText(i,nSubItem++,strtmp);
		}

		float fTop9 = 0.0;
		float fTotal = 0.0;
		int nSize = vttmp.size();
		int tmpvalueTotal = 0;
		for(i=0;i<nSize;i++)
			fTotal += vttmp[i].fAmount;
		
		if(nSize <= 9)
		{
			for(i=0;i<nSize;i++)
			{
				strtmp.Format("%s %.2f%%",vttmp[i].szName,vttmp[i].fAmount*100/fTotal);
				tmpvalue = (int)(vttmp[i].fAmount*360/fTotal+0.5);
				m_Pie.AddPiece(lprgb[i],RGB(255,255,255),tmpvalue,_F(strtmp));
				vtPoiName.push_back(_F(vttmp[i].szName));
			}
		}
		else
		{
			for(i=0;i<9;i++)
			{
				fTop9 += vttmp[i].fAmount;
				strtmp.Format("%s %.2f%%",vttmp[i].szName,vttmp[i].fAmount*100/fTotal);
				tmpvalue = (int)(vttmp[i].fAmount*360/fTotal+0.5);
				tmpvalueTotal += tmpvalue;
				m_Pie.AddPiece(lprgb[i],RGB(255,255,255),tmpvalue,_F(strtmp));
				vtPoiName.push_back(_F(vttmp[i].szName));
			}
			
			tmpvalue = 360-tmpvalueTotal;
			strtmp.Format("其他 %.2f%%",100-fTop9*100/fTotal);
			m_Pie.AddPiece(lprgb[10],RGB(255,255,255),tmpvalue,_F(strtmp));
		}	
	}
	else
	{
		for(int i=0;i<m_nFHSNum;i++)
		{
			if(strcmp(m_pFundHoldStock[i].szSymbol,lpszSymbol) != 0)
				continue;
			if(m_fTotalNet<COMPPREC)
				continue;
			CString strCode;		
			float tmptotal = 0.0;
			int ntmpvaluetotal = 0;
			m_Pie.Reset();
			for(int j=0;j<m_pFundHoldStock[i].nNum;j++)
			{
				strtmp.Format("%d",j+1);
				m_list1.InsertItem(j,strtmp);
				
				m_list1.SetItemText(j,1,m_pFundHoldStock[i].stockinfo[j].szSymbol);
				
				tempStkInfo = g_pStockIO->Get_MemStkInfo((const char*)m_pFundHoldStock[i].stockinfo[j].szSymbol);
				if(!tempStkInfo)	strCode = m_pFundHoldStock[i].stockinfo[j].szSymbol;
				else				strCode = tempStkInfo->Name;
				m_list1.SetItemText(j,2,strCode);
				
				strtmp.Format("%.1f",m_pFundHoldStock[i].stockinfo[j].fAcount);
				m_list1.SetItemText(j,3,strtmp);
				
				strtmp.Format("%.2f",m_pFundHoldStock[i].stockinfo[j].fMarketValue*100/m_fTotalNet);
				m_list1.SetItemText(j,4,strtmp);
				
				strtmp.Format("%.1f",m_pFundHoldStock[i].stockinfo[j].fLastAcount);
				m_list1.SetItemText(j,5,strtmp);
				
				tmptotal += m_pFundHoldStock[i].stockinfo[j].fMarketValue;
				tmpvalue = (int)(m_pFundHoldStock[i].stockinfo[j].fMarketValue*360/m_fTotalNet+0.5);
				ntmpvaluetotal += tmpvalue;
				strtmp.Format("%s  %.2f%%",strCode,m_pFundHoldStock[i].stockinfo[j].fMarketValue*100/m_fTotalNet);
				m_Pie.AddPiece(lprgb[j],RGB(255,255,255),tmpvalue,strtmp);
				m_list1.SetItemData(j,j);
				vtPoiName.push_back(m_pFundHoldStock[i].stockinfo[j].szSymbol);
			}
			tmpvalue = 360-ntmpvaluetotal;
			strtmp.Format("其他  %.2f%%",(m_fTotalNet-tmptotal)*100/m_fTotalNet);
			m_Pie.AddPiece(lprgb[10],RGB(255,255,255),tmpvalue,_F(strtmp));
		}
	}	
}

int nFundHoldType=0;
int CALLBACK HoldStockCmp(LPARAM lparam1,LPARAM lparam2,LPARAM	lparamsort)
{
	CSortListCtrl * plist = (CSortListCtrl*)lparamsort;
	CString strcomp1,strcomp2;
	int ncmpRes;
	strcomp1 = plist->GetItemText(lparam1,plist->m_nSortedCol);
	strcomp2 = plist->GetItemText(lparam2,plist->m_nSortedCol);
	switch(plist->m_nSortedCol)
	{
	case 1:
		ncmpRes = strcomp1.Compare(strcomp2);
		break;
	case 2:
		if(HOLDSTOCK == nFundHoldType)	ncmpRes = atof(strcomp1)<atof(strcomp2)?-1:1;
		else	ncmpRes = strcomp1.Compare(strcomp2);
		break;
	default:
		ncmpRes = atof(strcomp1)<atof(strcomp2)?-1:1;
	}
	if(plist->m_fAsc)
		return ncmpRes;
	else
		return -ncmpRes;
}

void CFundHoldStockDlg::OnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	int nCount = m_list1.GetItemCount();
	if( pNMListView->iSubItem==m_list1.m_nSortedCol )
		m_list1.m_fAsc = !m_list1.m_fAsc;
	else
	{
		m_list1.m_fAsc = TRUE;
		m_list1.m_nSortedCol = pNMListView->iSubItem;
	}
	nFundHoldType = m_nType;
	m_list1.SortItems(HoldStockCmp,(LPARAM)&m_list1);
	for(int i=0;i<nCount;i++)
	{
		m_list1.SetItemData(i,i);
	}
	*pResult = 0;
}

void CFundHoldStockDlg::OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nSel = pNMListView->iItem;
	CString strtmp = m_list1.GetItemText(nSel,1);
	for(int i=0;i<vtPoiName.size();i++)
	{
		if(strcmp(strtmp,vtPoiName[i]) == 0)
		{
			m_Pie.SetSelItem(i);
			return;
		}
	}
	m_Pie.SetSelItem(-1);
	*pResult = 0;
}
