// SeatQueryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "SeatQueryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSeatQueryDlg dialog


CSeatQueryDlg::CSeatQueryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSeatQueryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSeatQueryDlg)
	m_bSearchID = 0;
	m_strSeatID = _T("");
	m_bAutoRefresh = TRUE;
	//}}AFX_DATA_INIT
	m_nMainID = 0;
	m_nInitSeatID = -1;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_DS);
}


void CSeatQueryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSeatQueryDlg)
	DDX_Control(pDX, IDC_LIST_SEARCHRES, m_ListSeares);
	DDX_Control(pDX, IDC_LIST_BID, m_ListBid);
	DDX_Control(pDX, IDC_LIST_ASK, m_ListAsk);
	DDX_Control(pDX, IDC_COMBO_SEATNAME, m_SeatNameComb);
	DDX_Radio(pDX, IDC_RADIO_SEATID, m_bSearchID);
	DDX_Text(pDX, IDC_EDIT_SEATID, m_strSeatID);
	DDX_Control(pDX, IDC_STATIC_BID, m_Bid_Static);
	DDX_Control(pDX, IDC_STATIC_ASK, m_Ask_Static);
	DDX_Check(pDX, IDC_AUTOREFRESH_CHECK, m_bAutoRefresh);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSeatQueryDlg, CDialog)
	//{{AFX_MSG_MAP(CSeatQueryDlg)
	ON_BN_CLICKED(IDC_RADIO_SEATID, OnRadioSeatid)
	ON_BN_CLICKED(IDC_RADIO_SEATNAME, OnRadioSeatname)
	ON_BN_CLICKED(IDC_TOSEARCH, OnTosearch)
	ON_MESSAGE(UM_GETDATA_ACK,OnGetDataAck)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SEARCHRES, OnDblclkListSearchres)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_BID, OnDblclkListBid)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ASK, OnDblclkListAsk)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_AUTOREFRESH_CHECK, OnAutorefreshCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSeatQueryDlg message handlers

BOOL CSeatQueryDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon,FALSE);
	SetIcon(m_hIcon,TRUE);
	m_Bid_Static.SetTextColor(RGB(0,128,0));
	m_Ask_Static.SetTextColor(RGB(255,0,0));
	m_SeatNameComb.SetItemHeight(0,16);
	m_SeatNameComb.SetItemHeight(-1,16);

	OnRadioSeatid();

	LV_COLUMN lvColumn;
	static char *SeatSearchStr[2] = {"代码","席位简称"};
	static int SeatSearchWidth[2] = {40,100};
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	for(int i = 0; i < 2;i++)
	{
		lvColumn.pszText = _F(SeatSearchStr[i]);
		lvColumn.cx = SeatSearchWidth[i];
		lvColumn.iSubItem = i;
		m_ListSeares.InsertColumn (i,&lvColumn);
	}
	m_ListSeares.SetExtendedStyle(m_ListSeares.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	static char *SeatQueryStr[3] = {"代码","证券名称","挂盘价格"};
	static int SeatQueryWidth[3] = {40,110,60};
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	for(i = 0; i < 3;i++)
	{
		lvColumn.pszText = _F(SeatQueryStr[i]);
		lvColumn.cx = SeatQueryWidth[i];
		lvColumn.iSubItem = i;
		m_ListBid.InsertColumn (i,&lvColumn);
		m_ListAsk.InsertColumn (i,&lvColumn);
	}
	m_ListBid.SetExtendedStyle(m_ListSeares.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_ListAsk.SetExtendedStyle(m_ListSeares.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	if(m_nInitSeatID>0)			//有预设的，则直接打开
		DoSearchForInitSeatID();

	SetTimer(1, 8000, NULL);
	
	return TRUE;
}

void CSeatQueryDlg::OnOK() 
{
}

void CSeatQueryDlg::OnCancel() 
{
	if(g_pMainWin)
		g_pMainWin->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);
}

void CSeatQueryDlg::OnRadioSeatid() 
{
	m_bSearchID = 0;
	GetDlgItem(IDC_EDIT_SEATID)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_SEATNAME)->EnableWindow(FALSE);
}

void CSeatQueryDlg::OnRadioSeatname() 
{
	m_bSearchID = 1;
	GetDlgItem(IDC_EDIT_SEATID)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_SEATNAME)->EnableWindow(TRUE);
	FillCompDropList();
}

void CSeatQueryDlg::OnTosearch() 
{
	UpdateData(TRUE);

	if(m_bSearchID==0)				//搜寻席位编号
	{
		vector<Seat_Code2Com_Ex> vSc2c;
		
		FILE *fp_brkseat = _fsopen(g_WSXHStr+"LData\\cache\\brkseat.dat","rb",_SH_DENYNO);
		if(!fp_brkseat) return;
		int i=0,argc=0;
		char	lpString[256]={0},*argv[4]={0};
		for(rewind(fp_brkseat),i=0;!feof(fp_brkseat);++i)
		{
			memset(lpString,0,256);
			fgets(lpString,255,fp_brkseat);
			GetCmdLine(lpString,argv,4, ",|");
			
			Seat_Code2Com_Ex scn={0};
			long SeatID = -1;
			if(argv[0]) scn.ComID = atoi(argv[0]);
			if(argv[1]) scn.setCode = atoi(argv[1]);
			if(argv[2]&&strstr(argv[2], m_strSeatID)) 
			{
				scn.SeatID = atoi(argv[2]);
				vSc2c.push_back(scn);
			}
		}
		fclose(fp_brkseat);
		
		m_ListSeares.DeleteAllItems();
		//填list
		LV_ITEM lvItem;
		lvItem.mask = LVIF_TEXT|LVIF_STATE;
		lvItem.state = 0;
		lvItem.stateMask = 0;
		lvItem.iSubItem = 0;
		int nItem = 0;
		for(i=0;i<vSc2c.size();i++)
		{
			lvItem.iItem = nItem;
			char strSeatID[10]={0};
			sprintf(strSeatID, "%04d", vSc2c[i].SeatID);
			lvItem.pszText = strSeatID;
			m_ListSeares.InsertItem (&lvItem);

			char	ComNameS[21]={0},ComNameL[61]={0};
			GetSeatNameFromSID(vSc2c[i].SeatID, ComNameS, ComNameL);
			m_ListSeares.SetItemText(nItem,1,_F(ComNameS));
			m_ListSeares.SetItemData(nItem,vSc2c[i].SeatID);

			nItem++;
		}
	}
	else if(m_bSearchID==1)			//搜寻席位名称
	{
		vector<Seat_Com2Name_Ex> vSc2n;

		FILE *fp_brkcomp = _fsopen(g_WSXHStr+"LData\\cache\\brkcomp.dat","rb",_SH_DENYNO);
		if(!fp_brkcomp) return;
		CString snb;
		m_SeatNameComb.GetWindowText(snb);
		if(snb.GetLength()<1) return;

		int i=0,argc=0;
		char	lpString[256]={0},*argv[4]={0};
		for(rewind(fp_brkcomp),i=0;!feof(fp_brkcomp);++i)
		{
			memset(lpString,0,256);
			fgets(lpString,255,fp_brkcomp);
			GetCmdLine(lpString,argv,4, ",|");
			
			Seat_Com2Name_Ex scn2={0};
			long CompID = -1;
			if(argv[0]) scn2.ComID = atoi(argv[0]);
			if(argv[1]) strncpy(scn2.ComNameS, argv[1], 20);
			if(argv[2]) strncpy(scn2.ComNameL, argv[2], 60);
			if(strstr(argv[1], snb)||strstr(argv[2], snb))
				vSc2n.push_back(scn2);
		}
		fclose(fp_brkcomp);

		m_ListSeares.DeleteAllItems();
		//填list
		LV_ITEM lvItem;
		lvItem.mask = LVIF_TEXT|LVIF_STATE;
		lvItem.state = 0;
		lvItem.stateMask = 0;
		lvItem.iSubItem = 0;
		int nItem = 0;
		for(i=0;i<vSc2n.size();i++)
		{
			for(int ii=0;ii<1;ii++)
			{
				FILE *fp_brkseat = _fsopen(g_WSXHStr+"LData\\cache\\brkseat.dat","rb",_SH_DENYNO);
				if(!fp_brkseat) return;
				int j=0;
				char	lpString0[256]={0},*argv0[4]={0};
				for(rewind(fp_brkseat),j=0;!feof(fp_brkseat);++j)
				{
					memset(lpString0,0,256);
					fgets(lpString0,255,fp_brkseat);
					GetCmdLine(lpString0,argv0,4, ",|");

					if(argv0[0]&&atoi(argv0[0])==vSc2n[i].ComID)
					{
						lvItem.iItem = nItem;
						char strSeatID[10]={0};
						int nSeatID = atoi(argv0[2]);
						sprintf(strSeatID, "%04d", nSeatID);
						lvItem.pszText = strSeatID;
						m_ListSeares.InsertItem (&lvItem);
						
						char	ComNameS[21]={0},ComNameL[61]={0};
						GetSeatNameFromSID(nSeatID, ComNameS, ComNameL);						
						m_ListSeares.SetItemText(nItem,1,_F(ComNameS));
						m_ListSeares.SetItemData(nItem, nSeatID);

						nItem++;
					}
				}
				fclose(fp_brkseat);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CSeatQueryDlg::DoSearchForInitSeatID()
{
	if(m_nInitSeatID<=0)
		return;
	m_strSeatID.Format("%04d", m_nInitSeatID);
	UpdateData(FALSE);

	OnTosearch();
	int nSeatID = m_ListSeares.GetItemData(0);
	if(nSeatID>=0) GetQueryData(nSeatID);
}

void CSeatQueryDlg::FillCompDropList()
{
	FILE *fp_brkcomp = _fsopen(g_WSXHStr+"LData\\cache\\brkcomp.dat","rb",_SH_DENYNO);
	if(!fp_brkcomp) return;

	m_SeatNameComb.ResetContent();
	
	int i=0,argc=0;
	char	lpString[256]={0},*argv[4]={0};
	vector<Seat_Com2Name_Ex> vsce;
	for(rewind(fp_brkcomp),i=0;!feof(fp_brkcomp);++i)
	{
		memset(lpString,0,256);
		fgets(lpString,255,fp_brkcomp);
		GetCmdLine(lpString,argv,4, ",|");
		
		Seat_Com2Name_Ex scn2={0};
		if(argv[1]) strncpy(scn2.ComNameS, argv[1], 20);
		for(i=0;i<vsce.size();i++)
		{
			if(strcmp(argv[1], vsce[i].ComNameS)==0)
				break;
		}
		if(i==vsce.size())
		{
			vsce.push_back(scn2);
			m_SeatNameComb.AddString(_F(argv[1]));
		}
	}
	fclose(fp_brkcomp);
	m_SeatNameComb.Clear();
}

void CSeatQueryDlg::FillQueryRes(SEATQUERYINFO *pInfo, int nGetNum)
{
	map<long, SEATQUERYINFO*, less<long> >SortInfo_Bid;
	map<long, SEATQUERYINFO*, greater<long> >SortInfo_Ask;
	for(int i=0;i<nGetNum;i++)
	{
		SEATQUERYINFO *pCurInfo = &pInfo[i];
		MemStkInfo *pStock = g_pStockIO->GetDSStkInfo(pCurInfo->Market, pCurInfo->Code);
		if(!pStock) continue;

		if(pCurInfo->nSeatType>0)
			SortInfo_Bid[100000*pCurInfo->nSeatType+pStock->Index] = pCurInfo;
		else if(pCurInfo->nSeatType<0)
			SortInfo_Ask[100000*pCurInfo->nSeatType-pStock->Index] = pCurInfo;
	}
	
	m_ListBid.SetRedraw(FALSE);
	m_ListBid.DeleteAllItems();
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	int nItem = 0;
	map<long, SEATQUERYINFO*, less<long> >::iterator iterB = SortInfo_Bid.begin();
	short nCurType = -1;
	for(;iterB!=SortInfo_Bid.end();iterB++)
	{
		long xxx = (*iterB).first;
		short nType_Bid = ((*iterB).first)/100000;
		if(nType_Bid!=nCurType)
		{
			lvItem.iItem = nItem;
			char strItem[10]={0};
			sprintf(strItem, "%dS", nType_Bid-1);
			lvItem.pszText = strItem;
			if(nType_Bid-1) 
			{
				m_ListBid.InsertItem (&lvItem);

				m_ListBid.SetItemData(nItem, -1);
				nItem++;
			}
			
			nCurType = nType_Bid;

			//添加第一个
			lvItem.iItem = nItem;
			SEATQUERYINFO *pInfo = (*iterB).second;
			MemStkInfo *pStock = g_pStockIO->GetDSStkInfo(pInfo->Market, pInfo->Code);
			if(!pStock) continue;

			char strPrice[10]={0};
			sprintf(strItem, "%s", pStock->Code);
			lvItem.pszText = strItem;
			m_ListBid.InsertItem (&lvItem);
			m_ListBid.SetItemText(nItem,1,pStock->Name);
			sprintf(strPrice, "%.3f", pInfo->fPrice);
			if(pInfo->fPrice) 
				m_ListBid.SetItemText(nItem,2,strPrice);
			else m_ListBid.SetItemText(nItem,2,"--");

			m_ListBid.SetItemData(nItem, pStock->Index);
			nItem++;
		}
		else
		{
			lvItem.iItem = nItem;
			SEATQUERYINFO *pInfo = (*iterB).second;
			MemStkInfo *pStock = g_pStockIO->GetDSStkInfo(pInfo->Market, pInfo->Code);
			if(!pStock) continue;

			char strItem[10]={0}, strPrice[10]={0};
			sprintf(strItem, "%s", pStock->Code);
			lvItem.pszText = strItem;
			m_ListBid.InsertItem (&lvItem);
			m_ListBid.SetItemText(nItem,1,pStock->Name);
			sprintf(strPrice, "%.3f", pInfo->fPrice);
			if(pInfo->fPrice>COMPPREC) 
				m_ListBid.SetItemText(nItem,2,strPrice);
			else m_ListBid.SetItemText(nItem,2,"--");

			m_ListBid.SetItemData(nItem, pStock->Index);
			nItem++;
		}
	}
	m_ListBid.SetRedraw(TRUE);

	m_ListAsk.SetRedraw(TRUE);
	m_ListAsk.DeleteAllItems();;
	nItem = 0;
	map<long, SEATQUERYINFO*, greater<long> >::iterator iterA = SortInfo_Ask.begin();
	nCurType = 1;
	for(;iterA!=SortInfo_Ask.end();iterA++)
	{
		long xxx = (*iterA).first;
		short nType_Ask = ((*iterA).first)/100000;
		if(nType_Ask!=nCurType)
		{
			lvItem.iItem = nItem;
			char strItem[10]={0};
			sprintf(strItem, "%dS", nType_Ask+1);
			lvItem.pszText = strItem;
			if(nType_Ask+1) 
			{
				m_ListAsk.InsertItem (&lvItem);

				m_ListAsk.SetItemData(nItem, -1);
				nItem++;
			}
			
			nCurType = nType_Ask;

			//添加第一个
			lvItem.iItem = nItem;
			SEATQUERYINFO *pInfo = (*iterA).second;
			MemStkInfo *pStock = g_pStockIO->GetDSStkInfo(pInfo->Market, pInfo->Code);
			if(!pStock) continue;

			char strPrice[10]={0};
			sprintf(strItem, "%s", pStock->Code);
			lvItem.pszText = strItem;
			m_ListAsk.InsertItem (&lvItem);
			m_ListAsk.SetItemText(nItem,1,pStock->Name);
			sprintf(strPrice, "%.3f", pInfo->fPrice);
			if(pInfo->fPrice>COMPPREC) 
				m_ListAsk.SetItemText(nItem,2,strPrice);
			else m_ListAsk.SetItemText(nItem,2,"--");

			m_ListAsk.SetItemData(nItem, pStock->Index);
			nItem++;
		}
		else
		{
			lvItem.iItem = nItem;
			SEATQUERYINFO *pInfo = (*iterA).second;
			MemStkInfo *pStock = g_pStockIO->GetDSStkInfo(pInfo->Market, pInfo->Code);
			if(!pStock) continue;

			char strItem[10]={0}, strPrice[10]={0};
			sprintf(strItem, "%s", pStock->Code);
			lvItem.pszText = strItem;
			m_ListAsk.InsertItem (&lvItem);
			m_ListAsk.SetItemText(nItem,1,pStock->Name);
			sprintf(strPrice, "%.3f", pInfo->fPrice);
			if(pInfo->fPrice>COMPPREC) 
				m_ListAsk.SetItemText(nItem,2,strPrice);
			else m_ListAsk.SetItemText(nItem,2,"--");

			m_ListAsk.SetItemData(nItem, pStock->Index);
			nItem++;
		}
	}
	m_ListAsk.SetRedraw(TRUE);
}

void CSeatQueryDlg::GetQueryData(int nSeatID) 
{
	g_nLastQuerySeatID = nSeatID;
	g_nMainID = MainIDPlus(m_nMainID,HKSEATQUERY_WIN);
	g_nAssisID = HKSEATQ_ASSISD;
	g_pGetData->GetOneSeatQuery(nSeatID);
}

void CSeatQueryDlg::OnGetDataAck(WPARAM wParam,LPARAM lParam)
{
	if(m_nMainID!=g_AnsHeader3.MainID) return;

	SEATQUERYINFO sqi[200]={0};
	int nGetNum = g_pGetData->GetSeatQueryACK(sqi, 200);

	FillQueryRes(sqi, nGetNum);
}

void CSeatQueryDlg::OnDblclkListSearchres(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_ListSeares.GetFirstSelectedItemPosition();
	if(pos)
	{
		int nItem = m_ListSeares.GetNextSelectedItem (pos);
		if(nItem >=0 && nItem < m_ListSeares.GetItemCount())
		{
			int nSeatID = m_ListSeares.GetItemData(nItem);
			GetQueryData(nSeatID);
		}
	}
	
	*pResult = 0;
}

void CSeatQueryDlg::OnDblclkListBid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_ListBid.GetFirstSelectedItemPosition();
	if(pos)
	{
		int nItem = m_ListBid.GetNextSelectedItem (pos);
		if(nItem >=0 && nItem < m_ListBid.GetItemCount())
		{
			long nIDIndex = m_ListBid.GetItemData(nItem);
			MemStkInfo *pStock = g_pStockIO->GetDSStkInfo(nIDIndex);
			if(pStock)
				ToZst(pStock->Code,pStock->setcode,FALSE);
		}
	}

	*pResult = 0;
}

void CSeatQueryDlg::OnDblclkListAsk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_ListAsk.GetFirstSelectedItemPosition();
	if(pos)
	{
		int nItem = m_ListAsk.GetNextSelectedItem (pos);
		if(nItem >=0 && nItem < m_ListAsk.GetItemCount())
		{
			long nIDIndex = m_ListAsk.GetItemData(nItem);
			MemStkInfo *pStock = g_pStockIO->GetDSStkInfo(nIDIndex);
			if(pStock) 
				ToZst(pStock->Code,pStock->setcode,FALSE);
		}
	}
	
	*pResult = 0;
}

void CSeatQueryDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==1 && m_bAutoRefresh && g_nLastQuerySeatID>0)
		GetQueryData(g_nLastQuerySeatID);
	
	CDialog::OnTimer(nIDEvent);
}

void CSeatQueryDlg::OnAutorefreshCheck() 
{
	UpdateData(TRUE);	
}
