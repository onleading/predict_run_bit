// WarnDtDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "WarnDtDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWarnDtDlg dialog


CWarnDtDlg::CWarnDtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWarnDtDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWarnDtDlg)
	m_CodeStr = _T("");
	//}}AFX_DATA_INIT
	m_nWarnFlag	= WARN_NONE;
	m_nShowNum	= 0;
}


void CWarnDtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWarnDtDlg)
	DDX_Control(pDX, IDC_DTWARN_CODEEDIT, m_Code);
	DDX_Control(pDX, IDC_DTWANR_STATIC, m_Static);
	DDX_Control(pDX, IDC_COMBO_WARNTYPE, m_WarnType);
	DDX_Control(pDX, IDC_BUTTON_ENTER, m_EnterBtn);
	DDX_Control(pDX, IDC_BUTTON_EXPORT, m_ExportBtn);
	DDX_Control(pDX, IDC_DTWARN_START, m_StartBtn);
	DDX_Control(pDX, IDC_DTWARN_LIST, m_List);
	DDX_Text(pDX, IDC_DTWARN_CODEEDIT, m_CodeStr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWarnDtDlg, CDialog)
	//{{AFX_MSG_MAP(CWarnDtDlg)
	ON_BN_CLICKED(IDC_DTWARN_START, OnDtwarnStart)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, OnButtonExport)
	ON_BN_CLICKED(IDC_BUTTON_ENTER, OnButtonEnter)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_CBN_SELCHANGE(IDC_COMBO_WARNTYPE, OnSelchangeComboWarntype)
	ON_NOTIFY(NM_DBLCLK, IDC_DTWARN_LIST, OnDblclkDtwarnList)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWarnDtDlg message handlers

void CWarnDtDlg::FillList()
{
	static char LineFlag[2][5] = {"委买","委卖"};
	static char OrderFlag[4][5]= {"挂买","撤买","挂卖","撤卖"};
	int i = 0;
	int ItemIndex = 0;
	CString tmpStr;
	m_List.SetRedraw(FALSE);
	m_List.DeleteAllItems();
	m_List.SetBkColor(VipColor.BACKCOLOR);
	m_List.SetTextBkColor(VipColor.BACKCOLOR);
	m_List.SetTextColor(VipColor.TXTCOLOR);
	for(i=0;i<g_DtWarnInf.nWarnNum;i++)
	{
		m_pStkInfo = g_pStockIO->Get_MemStkInfo(g_DtWarnInf.pWarnAtom[i].Code,g_DtWarnInf.pWarnAtom[i].SetCode);
		if(m_pStkInfo==NULL) continue;
		if(strstr(g_DtWarnInf.pWarnAtom[i].Code,LPCSTR(m_CodeStr)) == NULL) continue;
		switch(m_nWarnFlag)
		{
		case WARN_LINE:
			if(g_DtWarnInf.pWarnAtom[i].WarningNo != m_nWarnFlag) continue;
			ItemIndex = m_List.InsertItem(0,m_pStkInfo->Name);
			m_List.SetItemData(ItemIndex,i);
			tmpStr.Format("%02d:%02d:%02d",g_DtWarnInf.pWarnAtom[i].WarnTime/10000,g_DtWarnInf.pWarnAtom[i].WarnTime/100%100,g_DtWarnInf.pWarnAtom[i].WarnTime%100);
			m_List.SetItemText(ItemIndex,1,tmpStr);
			m_List.SetItemText(ItemIndex,2,LineFlag[g_DtWarnInf.pWarnAtom[i].uDetail.uTractor.WarnFlag]);
			tmpStr.Format("%.3f",g_DtWarnInf.pWarnAtom[i].uDetail.uTractor.Price);
			m_List.SetItemText(ItemIndex,3,tmpStr);
			tmpStr.Format("%d",g_DtWarnInf.pWarnAtom[i].uDetail.uTractor.FirstQty);
			m_List.SetItemText(ItemIndex,4,tmpStr);
			tmpStr.Format("%d",g_DtWarnInf.pWarnAtom[i].uDetail.uTractor.DiffQty);
			m_List.SetItemText(ItemIndex,5,tmpStr);
			tmpStr.Format("%d",g_DtWarnInf.pWarnAtom[i].uDetail.uTractor.OrderNum);
			m_List.SetItemText(ItemIndex,6,tmpStr);
			break;
		case WARN_ORDERSHOCK:
			if(g_DtWarnInf.pWarnAtom[i].WarningNo != m_nWarnFlag) continue;
			ItemIndex = m_List.InsertItem(0,m_pStkInfo->Name);
			m_List.SetItemData(ItemIndex,i);
			tmpStr.Format("%02d:%02d:%02d",g_DtWarnInf.pWarnAtom[i].WarnTime/10000,g_DtWarnInf.pWarnAtom[i].WarnTime/100%100,g_DtWarnInf.pWarnAtom[i].WarnTime%100);
			m_List.SetItemText(ItemIndex,1,tmpStr);
			m_List.SetItemText(ItemIndex,2,OrderFlag[g_DtWarnInf.pWarnAtom[i].uDetail.uOrderShock.WarnFlag]);
			tmpStr.Format("%.3f",g_DtWarnInf.pWarnAtom[i].uDetail.uOrderShock.Amount/g_DtWarnInf.pWarnAtom[i].uDetail.uOrderShock.Volume);
			m_List.SetItemText(ItemIndex,3,tmpStr);
			tmpStr.Format("%.0f",g_DtWarnInf.pWarnAtom[i].uDetail.uOrderShock.Volume/100);
			m_List.SetItemText(ItemIndex,4,tmpStr);
			break;
		default:
			ItemIndex = m_List.InsertItem(0,m_pStkInfo->Name);
			m_List.SetItemData(ItemIndex,i);
			tmpStr.Format("%02d:%02d:%02d",g_DtWarnInf.pWarnAtom[i].WarnTime/10000,g_DtWarnInf.pWarnAtom[i].WarnTime/100%100,g_DtWarnInf.pWarnAtom[i].WarnTime%100);
			m_List.SetItemText(ItemIndex,1,tmpStr);
			switch(g_DtWarnInf.pWarnAtom[i].WarningNo)
			{
			case WARN_LINE:
				m_List.SetItemText(ItemIndex,2,_F("拖拉机单"));
				tmpStr.Format(_F("%s,价格:%8.3f,首笔:%5d,增量:%4d,笔数:%2d"),LineFlag[g_DtWarnInf.pWarnAtom[i].uDetail.uTractor.WarnFlag],
					g_DtWarnInf.pWarnAtom[i].uDetail.uTractor.Price,g_DtWarnInf.pWarnAtom[i].uDetail.uTractor.FirstQty,
					g_DtWarnInf.pWarnAtom[i].uDetail.uTractor.DiffQty,g_DtWarnInf.pWarnAtom[i].uDetail.uTractor.OrderNum);
				m_List.SetItemText(ItemIndex,3,tmpStr);
				break;
			case WARN_ORDERSHOCK:
				m_List.SetItemText(ItemIndex,2,_F("委托异动"));
				tmpStr.Format(_F("%s,估计价格:%8.3f,估计数量:%.0f"),
					OrderFlag[g_DtWarnInf.pWarnAtom[i].uDetail.uOrderShock.WarnFlag],
					g_DtWarnInf.pWarnAtom[i].uDetail.uOrderShock.Amount/g_DtWarnInf.pWarnAtom[i].uDetail.uOrderShock.Volume,
					g_DtWarnInf.pWarnAtom[i].uDetail.uOrderShock.Volume/100);
				m_List.SetItemText(ItemIndex,3,tmpStr);
			default:
				break;
			}
			break;
		}
	}
	m_List.SetRedraw(TRUE);
}

void CWarnDtDlg::FillListHead()
{
	static char NormalHead[4][20]	= {"证券简称","时间","预警类型","预警描述"};
	static int	NormalWid[4]		= {60,60,60,300};
	static int  NormalFmt[4]		= {LVCFMT_CENTER,LVCFMT_CENTER,LVCFMT_CENTER,LVCFMT_LEFT};
	static char LineHead[7][20]		= {"证券简称","时间","委托方向","价格","首笔","增量","笔数"};
	static int	LineWid[7]			= {60,60,60,54,52,50,50};
	static int  LineFmt[7]			= {LVCFMT_CENTER,LVCFMT_CENTER,LVCFMT_CENTER,LVCFMT_RIGHT,LVCFMT_RIGHT,LVCFMT_RIGHT,LVCFMT_RIGHT};
	static char OrderHead[5][20]	= {"证券简称","时间","委托方向","估计价格","估计数量"};
	static int	OrderWid[5]			= {60,60,60,60,60};
	static int  OrderFmt[5]			= {LVCFMT_CENTER,LVCFMT_CENTER,LVCFMT_CENTER,LVCFMT_RIGHT,LVCFMT_RIGHT};
	int i = 0;
	CHeaderCtrl *pHeadCtrl = m_List.GetHeaderCtrl();
	if(pHeadCtrl)
	{
		int nColumnCount = pHeadCtrl->GetItemCount();
		for (i = 0;i < nColumnCount;i++) m_List.DeleteColumn(0);
	}
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;

	switch(m_nWarnFlag)
	{
	case WARN_LINE:
		for(i = 0; i < 7;i++)
		{
			lvColumn.pszText = _F(LineHead[i]);
			lvColumn.cx = LineWid[i];
			lvColumn.iSubItem = i;
			lvColumn.fmt = LineFmt[i];
			m_List.InsertColumn (i,&lvColumn);
		}
		break;
	case WARN_ORDERSHOCK:
		for(i = 0; i < 5;i++)
		{
			lvColumn.pszText = _F(OrderHead[i]);
			lvColumn.cx = OrderWid[i];
			lvColumn.iSubItem = i;
			lvColumn.fmt = OrderFmt[i];
			m_List.InsertColumn(i,&lvColumn);
		}
		break;
	default:
		for(i = 0; i < 4;i++)
		{
			lvColumn.pszText = _F(NormalHead[i]);
			lvColumn.cx = NormalWid[i];
			lvColumn.iSubItem = i;
			lvColumn.fmt = NormalFmt[i];
			m_List.InsertColumn(i,&lvColumn);
		}
		break;
	}
}

void CWarnDtDlg::OnCancel() 
{
	KillTimer(0x0101);
	GetParent()->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);
}

void CWarnDtDlg::OnDtwarnStart() 
{	
}

void CWarnDtDlg::OnButtonExport() 
{
	CMenu menu;
	menu.LoadMenu(IDR_OTHERMENU);
	CMenu *pMenu=(CMenu *)menu.GetSubMenu(1);
	
	CRect rect;
	GetDlgItem(ID_EXPORT)->GetWindowRect(&rect);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN,rect.left,rect.bottom+2,this);	
	
}

void CWarnDtDlg::OnButtonEnter() 
{
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	if(pos)
	{
		int nItem = m_List.GetNextSelectedItem(pos);
		int tmpWord = m_List.GetItemData(nItem);
		if(tmpWord<0 || tmpWord>=g_DtWarnInf.nWarnNum) return;
		MemStkInfo *tmpInfo = g_pStockIO->Get_MemStkInfo(g_DtWarnInf.pWarnAtom[tmpWord].Code,g_DtWarnInf.pWarnAtom[tmpWord].SetCode);
		if(tmpInfo)	ToZst(tmpInfo->Code,tmpInfo->setcode,FALSE);
	}
}

void CWarnDtDlg::OnOK() 
{
	OnButtonEnter();
}

BOOL CWarnDtDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//供调整使用
	CRect rectw,recti;
	GetWindowRect(rectw);
	m_List.GetWindowRect(&recti);
	m_nListLeft		= recti.left	-	rectw.left;
	m_nListTop		= recti.top		-	rectw.top;
	m_nListRight	= rectw.right	-	recti.right;
	m_nListBottom	= rectw.bottom	-	recti.bottom;
	m_StartBtn.GetWindowRect(&recti);
	m_nStartLeft	= recti.left	-	rectw.left;
	m_nBtnTop		= rectw.bottom	-	recti.top;
	m_nBtnHeight	= recti.Height();
	m_nBtnWidth		= recti.Width();
	m_WarnType.GetWindowRect(&recti);
	m_nComLeft		= rectw.right	-	recti.left;
	m_nComHeight	= recti.Height();
	m_nComWidth		= recti.Width();
	m_ExportBtn.GetWindowRect(&recti);
	m_nExportLeft	= rectw.right	-	recti.left;
	m_EnterBtn.GetWindowRect(&recti);
	m_nEnterLeft	= rectw.right	-	recti.left;
	m_Static.GetWindowRect(&recti);
	m_nStaticWidth	= recti.Width();
	m_nStaticLeft	= rectw.right	-	recti.left;
	m_Code.GetWindowRect(&recti);
	m_nEditWidth	= recti.Width();
	m_nEditLeft		= rectw.right	-	recti.left;

	m_WarnType.SetItemHeight(0,16);
	m_WarnType.SetItemHeight(-1,16);
	m_WarnType.SetCurSel(0);
	
	m_List.SetExtendedStyle(m_List.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	FillListHead();
	FillList();

	SetTimer(0x0101,1000,NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWarnDtDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	CRect rectw;
	GetWindowRect(rectw);
	ScreenToClient(&rectw);
	CWnd *pWnd1 = GetDlgItem(IDC_DTWARN_LIST);
	CWnd *pWnd2 = GetDlgItem(IDC_DTWARN_START);
	CWnd *pWnd3 = GetDlgItem(IDC_COMBO_WARNTYPE);
	CWnd *pWnd4 = GetDlgItem(IDC_BUTTON_EXPORT);
	CWnd *pWnd5 = GetDlgItem(IDC_BUTTON_ENTER);
	CWnd *pWnd6 = GetDlgItem(IDC_DTWANR_STATIC);
	CWnd *pWnd7 = GetDlgItem(IDC_DTWARN_CODEEDIT);
	if(pWnd1) pWnd1->MoveWindow(rectw.left+m_nListLeft,rectw.top+m_nListTop,rectw.Width()-m_nListLeft-m_nListRight,rectw.Height()-m_nListTop-m_nListBottom);
	if(pWnd2) pWnd2->MoveWindow(rectw.left+m_nStartLeft,rectw.bottom-m_nBtnTop,m_nBtnWidth,m_nBtnHeight);
	if(pWnd3) pWnd3->MoveWindow(rectw.right-m_nComLeft,rectw.bottom-m_nBtnTop,m_nComWidth,m_nComHeight);
	if(pWnd4) pWnd4->MoveWindow(rectw.right-m_nExportLeft,rectw.bottom-m_nBtnTop,m_nBtnWidth,m_nBtnHeight);
	if(pWnd5) pWnd5->MoveWindow(rectw.right-m_nEnterLeft,rectw.bottom-m_nBtnTop,m_nBtnWidth,m_nBtnHeight);
	if(pWnd6) pWnd6->MoveWindow(rectw.right-m_nStaticLeft,rectw.bottom-m_nBtnTop,m_nStaticWidth,m_nBtnHeight);
	if(pWnd7) pWnd7->MoveWindow(rectw.right-m_nEditLeft,rectw.bottom-m_nBtnTop,m_nEditWidth,m_nBtnHeight);
}

void CWarnDtDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	lpMMI->ptMinTrackSize.x = 428;
	lpMMI->ptMinTrackSize.y = 320;
	
	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CWarnDtDlg::OnSelchangeComboWarntype() 
{
	switch(m_WarnType.GetCurSel())
	{
	case 0:		m_nWarnFlag = WARN_NONE;		break;
	case 1:		m_nWarnFlag = WARN_LINE;		break;
	case 2:		m_nWarnFlag = WARN_ORDERSHOCK;	break;
	default:	m_nWarnFlag = WARN_NONE;		break;
	}
	FillListHead();
	FillList();
}

void CWarnDtDlg::OnDblclkDtwarnList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnButtonEnter();
	*pResult = 0;
}

void CWarnDtDlg::OnTimer(UINT nIDEvent) 
{
	switch(nIDEvent)
	{
	case 0x0101:
		if(m_nShowNum!=g_DtWarnInf.nWarnNum) FillList();
		m_nShowNum = g_DtWarnInf.nWarnNum;
		break;
	}
	CDialog::OnTimer(nIDEvent);
}

BOOL CWarnDtDlg::PreTranslateMessage(MSG* pMsg) 
{
	BOOL bRet = CDialog::PreTranslateMessage(pMsg);

	if(GetDlgItem(IDC_DTWARN_CODEEDIT)->m_hWnd == pMsg->hwnd && pMsg->message == WM_CHAR)
	{
		UpdateData(TRUE);
		FillList();
	}
	
	return bRet;
}
