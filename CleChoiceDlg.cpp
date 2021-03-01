// CleChoiceDlg.cpp : implementation file

#include "stdafx.h"
#include "tdxw.h"
#include "CleChoiceDlg.h"
#include "Znxg.h"
#include "ComView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCleChoiceDlg dialog


CCleChoiceDlg::CCleChoiceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCleChoiceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCleChoiceDlg)
	//}}AFX_DATA_INIT
	m_nSelNum = 0;
	CurGPSel = -1;
}


void CCleChoiceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCleChoiceDlg)
	DDX_Control(pDX, IDC_GPNUM_STATIC, m_GPNum_Static);
	DDX_Control(pDX, IDC_CONINF_EDIT, m_ConInfoEdit);
	DDX_Control(pDX, IDC_GPLIST, m_GPList);
	DDX_Control(pDX, IDC_CONTAB, m_ConTab);
	DDX_Control(pDX, IDC_CONLIST, m_ConList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCleChoiceDlg, CDialog)
	//{{AFX_MSG_MAP(CCleChoiceDlg)
	ON_BN_CLICKED(IDC_SAVETOBLOCK, OnSavetoblock)
	ON_NOTIFY(TCN_SELCHANGE, IDC_CONTAB, OnSelchangeContab)
	ON_BN_CLICKED(IDC_CONINFO, OnConinfo)
	ON_WM_CTLCOLOR()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CONLIST, OnItemchangedConlist)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_TOVIP, OnTovip)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_GPLIST, OnItemchangedGplist)
	ON_NOTIFY(NM_DBLCLK, IDC_GPLIST, OnDblclkGplist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCleChoiceDlg message handlers

BOOL CCleChoiceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_GPNum_Static.SetTextColor(RGB(0,0,255));

	//对话框位置从盘中得到
	CString	Ini = g_WSXHStr+g_strUserCfg;
	int wintop  = GetPrivateProfileInt("OTHER","ZNTOP",0,Ini);
	int winleft = GetPrivateProfileInt("OTHER","ZNLEFT",0,Ini);
	if(wintop>10 && wintop<ScreenCx-10 && winleft>-20 && winleft<ScreenCy+20)
		SetWindowPos(NULL,winleft,wintop,0,0,SWP_NOSIZE);

	static char *CleConTabStr[6] = {"低进定式","高进定式","高出定式","低出定式","基本指标","技术指标"};
	for(int i = 0;i < 6;i++)
		m_ConTab.InsertItem(i,_F(CleConTabStr[i]));

	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_GP));
	m_ImageList.Add(hIcon);
	hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_CLEICON));
	m_ImageList.Add(hIcon);
	hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_CLENONE));
	m_ImageList.Add(hIcon);

	//定制条件列表
	m_ConList.SetImageList(&m_ImageList,LVSIL_SMALL);

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 136;
	lvColumn.iSubItem = 0;
	lvColumn.pszText  = "F0514M0824";
	m_ConList.InsertColumn (0,&lvColumn);

	//选中股票列表
	m_GPList.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_GPList.SetExtendedStyle (LVS_EX_FULLROWSELECT);
	static char *CleGPStr[6] = {" 代码"," 名称"," 现价","涨幅%","换手%","市盈(动)"};
	static int   CleGPInt[6] = {60,60,55,55,55,60};
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	for(i = 0; i < 6;i++)
	{
		lvColumn.pszText = _F(CleGPStr[i]);
		lvColumn.cx = CleGPInt[i];
		lvColumn.iSubItem = i;
		m_GPList.InsertColumn (i,&lvColumn);
	}

	m_ConInfoEdit.ShowWindow(SW_HIDE);
	m_GPList.ShowWindow(SW_SHOW);
	bConInfo = FALSE;

	CurTabSel  = 0;
	m_ConTab.SetCurSel(CurTabSel);

	CurListSel = -1;
	FillList();

	SetTimer(0x011,GetListRefreshTime(),NULL);

	if(!FindFile("cleinfo.dat"))
		GetDlgItem(IDC_CONINFO)->ShowWindow(SW_HIDE);

	UpdateData(FALSE);
	return TRUE;
}

void CCleChoiceDlg::FillList()
{
	m_ConList.SetRedraw(FALSE);
	m_ConList.DeleteAllItems();
	CLE_CONTABLE *tmpTable;
	switch(CurTabSel)
	{
	case 0:	tmpTable = g_pCleKLine1;break;
	case 1:	tmpTable = g_pCleKLine2;break;
	case 2:	tmpTable = g_pCleKLine3;break;
	case 3:	tmpTable = g_pCleKLine4;break;
	case 4:	tmpTable = g_pCleJBM;	break;
	case 5:	tmpTable = g_pCleJSM;	break;
	default:tmpTable = NULL;		break;
	}
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	int ntmp;
	for(int i = 0;;i++)
	{
		lvItem.iItem = i;
		if(!tmpTable || tmpTable[i].topic[0] == 0)
			break;
		if(CurTabSel < 4)	ntmp = g_pZnxg->GetKLineStyleGP(tmpTable[i].MsgNo,m_nSelGPIndex);
		else				ntmp = g_pZnxg->GetOtherStyleGP(tmpTable[i].MsgNo,m_nSelGPIndex);
		if(ntmp == 0)
			lvItem.iImage = 2;
		else
			lvItem.iImage = 1;
		lvItem.pszText = _F(tmpTable[i].topic);
		m_ConList.InsertItem (&lvItem);
	}
	if(i > 0)
	{
		m_ConList.SetItemState(0,LVIS_FOCUSED,LVIS_FOCUSED);
		m_ConList.SetItemState(0,LVIS_SELECTED,LVIS_SELECTED);
		CurListSel = 0;
		GetConGP(CurTabSel,CurListSel,bConInfo);
	}
	m_ConList.SetRedraw(TRUE);
}

CString CCleChoiceDlg::GetColStr(int colflag,MemStkInfo *tmpInfo)
{
	struct CurrStockData CurrHQ;
	g_pStockIO->ReadCurrStockData(tmpInfo,&CurrHQ);
	short xsflag = GetXSFlag(tmpInfo);
	CString tmpStr;
	float ftmp;
	switch(colflag)
	{
	case ZJCJ:
		ftmp=CurrHQ.Now;
		if(ftmp<COMPPREC)
			ftmp=CurrHQ.Close;
		tmpStr.Format(xsflag==XS3?"%.3f":"%.2f",ftmp);
		break;
	case ZAF:
		if (CurrHQ.Close>COMPPREC)
		{	
			if (CurrHQ.Now < COMPPREC)	
				ftmp = 0.0;
			else
				ftmp = CurrHQ.Now - CurrHQ.Close;
			tmpStr.Format("%7.2f",ftmp/CurrHQ.Close*(float)100.0);
		}
		break;
	case J_HSL:
		if( !testzs(tmpInfo) && tmpInfo->ActiveCapital > 1 )
			tmpStr.Format("%7.2f",1.0*CurrHQ.Volume*tmpInfo->Unit/(tmpInfo->ActiveCapital*100.0));
		break;
	case SYL:
		if( !testzs(tmpInfo) && tmpInfo->J_zgb > 1)
		{	
			ftmp = (tmpInfo->J_jly*12/tmpInfo->J_HalfYearFlag)/(tmpInfo->J_zgb);
			if(ftmp > COMPPREC)
			{
				if(CurrHQ.Now > COMPPREC)
					tmpStr.Format("%7.2f",CurrHQ.Now/ftmp);
				else if(CurrHQ.Close > COMPPREC)
					tmpStr.Format("%7.2f",CurrHQ.Close/ftmp);
			}
		}
		break;
	default:
		break;
	}
	return tmpStr;
}

void CCleChoiceDlg::FillHQ(BOOL bFirst)
{
	m_GPList.SetRedraw(FALSE);
	if(bFirst)
		m_GPList.DeleteAllItems();

	MemStkInfo *tmpInfo;
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	for(int i = 0;i < m_nSelNum;i++)
	{
		tmpInfo = (*g_pStockIO)[m_nSelGPIndex[i]];
		if(bFirst)
		{
			lvItem.iItem = i;
			lvItem.iImage = 0;
			lvItem.pszText = tmpInfo->Code;
			m_GPList.InsertItem(&lvItem);
			m_GPList.SetItemData(i,m_nSelGPIndex[i]);
		}
		m_GPList.SetItemText(i,1,tmpInfo->Name);
		m_GPList.SetItemText(i,2,GetColStr(ZJCJ,tmpInfo));
		m_GPList.SetItemText(i,3,GetColStr(ZAF,tmpInfo));
		m_GPList.SetItemText(i,4,GetColStr(J_HSL,tmpInfo));
		m_GPList.SetItemText(i,5,GetColStr(SYL,tmpInfo));
	}
	if(bFirst)	//如果是第一次
		CurGPSel = -1;
	else
	{
		m_GPList.SetItemState(CurGPSel,LVIS_FOCUSED,LVIS_FOCUSED);
		m_GPList.SetItemState(CurGPSel,LVIS_SELECTED,LVIS_SELECTED);
	}
	m_GPList.SetRedraw(TRUE);
}

void CCleChoiceDlg::GetConGP(int TabSel,int ListSel,BOOL bInfo)
{
	CLE_CONTABLE *tmpTable;
	switch(TabSel)
	{
	case 0:	tmpTable = g_pCleKLine1;break;
	case 1:	tmpTable = g_pCleKLine2;break;
	case 2:	tmpTable = g_pCleKLine3;break;
	case 3:	tmpTable = g_pCleKLine4;break;
	case 4:	tmpTable = g_pCleJBM;	break;
	case 5:	tmpTable = g_pCleJSM;	break;
	default:tmpTable = NULL;		break;
	}
	if(bInfo)
	{
		m_ConInfoEdit.SetRedraw(FALSE);
		m_ConInfoEdit.SetWindowText("");
		CString tmpStr;
		if(TabSel >= 0 && TabSel < 4)
			tmpStr.Format("XG%04d",tmpTable[ListSel].MsgNo);
		else if(TabSel == 4)
			tmpStr.Format("JB%04d",tmpTable[ListSel].MsgNo-JBMXG_1001);
		else 
			tmpStr.Format("JS%04d",tmpTable[ListSel].MsgNo-JSMXG_1001);
		CString tmpLine,tmpidStr,TotalStr;
		CStdioFile theFile;
		if(theFile.Open("cleinfo.dat",CFile::modeRead|CFile::shareDenyNone))
		{
			while(theFile.ReadString(tmpLine))
			{
				if(tmpLine.IsEmpty()) continue;
				if(tmpLine[0] == '?')
				{
					tmpidStr = tmpLine.Mid(1,6);
					if(tmpidStr == tmpStr)
					{
						while(theFile.ReadString(tmpLine))
						{
							if(tmpLine[0] == '?') break;
							TotalStr+=tmpLine+"\r\n";
						}
						break;
					}
				}
			}
		}
		m_ConInfoEdit.SetWindowText(TotalStr);
		m_ConInfoEdit.SetRedraw(TRUE);
	}
	else
	{
		m_nSelNum = 0;
		if(TabSel < 4)
			m_nSelNum = g_pZnxg->GetKLineStyleGP(tmpTable[ListSel].MsgNo,m_nSelGPIndex);
		else
			m_nSelNum = g_pZnxg->GetOtherStyleGP(tmpTable[ListSel].MsgNo,m_nSelGPIndex);
		FillHQ();
		CString tmpStr;
		tmpStr.Format("选出%d支",m_nSelNum);
		SetDlgItemText(IDC_GPNUM_STATIC,_F(tmpStr));
	}
}

void CCleChoiceDlg::OnOK() 
{
}

void CCleChoiceDlg::OnCancel() 
{
	//将对话框位置写盘
	CString	Ini = g_WSXHStr+g_strUserCfg;
	CRect rect;
	GetWindowRect(&rect);
	WritePrivateProfileString("OTHER","ZNTOP",IntToStr(rect.top),Ini);
	WritePrivateProfileString("OTHER","ZNLEFT",IntToStr(rect.left),Ini);
	GetParent()->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);
	TDEL(g_pZnxg);
}

#include "GetBlockDlg.h"
void CCleChoiceDlg::OnSavetoblock() 
{
	CGetBlockDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		if(HasGPInBlockAndContinue(dlg.nSelBlock))
			MakeBlock(m_nSelGPIndex,m_nSelNum,dlg.nSelBlock,TRUE);//生成板块并显示板块
	}
}

void CCleChoiceDlg::OnSelchangeContab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int index= m_ConTab.GetCurSel();
	if(index != CurTabSel)
	{
		CurTabSel = index;
		FillList();
	}
	*pResult = 0;
}

void CCleChoiceDlg::OnConinfo()
{
	bConInfo = !bConInfo;
	m_ConInfoEdit.ShowWindow(bConInfo?SW_SHOW:SW_HIDE);
	m_GPList.ShowWindow(bConInfo?SW_HIDE:SW_SHOW);
	GetConGP(CurTabSel,CurListSel,bConInfo);
	SetDlgItemText(IDC_CONINFO,bConInfo?_F("选出股票"):_F("条件注释"));
	GetDlgItem(IDC_TOVIP)->EnableWindow(bConInfo?FALSE:TRUE);
	GetDlgItem(IDC_SAVETOBLOCK)->EnableWindow(bConInfo?FALSE:TRUE);
	GetDlgItem(IDC_GPNUM_STATIC)->ShowWindow(bConInfo?FALSE:TRUE);
}

HBRUSH CCleChoiceDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if (pWnd->GetDlgCtrlID() == IDC_CONINF_EDIT)
		return (HBRUSH)GetStockObject(WHITE_BRUSH);
	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

void CCleChoiceDlg::OnItemchangedConlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		
	POSITION pos;
	pos = m_ConList.GetFirstSelectedItemPosition();
	if(pos != NULL)
	{
		int m_index = m_ConList.GetNextSelectedItem(pos);
		if(CurListSel != m_index)
		{
			CurListSel = m_index;
			GetConGP(CurTabSel,CurListSel,bConInfo);
		}
	}
	*pResult = 0;
}

void CCleChoiceDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 0x011 && !bConInfo)
		FillHQ(FALSE);
	CDialog::OnTimer(nIDEvent);
}

//打开组合图
void CCleChoiceDlg::OnTovip() 
{
	if(CurGPSel < 0) return;
	int iNo = m_GPList.GetItemData(CurGPSel);
	MemStkInfo *ps = (*g_pStockIO)[iNo];
	if(!ps)	return;

	ShowGpInCom(ps->Code,ps->setcode);
	GetDlgItem(IDC_GPLIST)->SetFocus();
}

void CCleChoiceDlg::OnItemchangedGplist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	POSITION pos;
	pos = m_GPList.GetFirstSelectedItemPosition();
	if(pos != NULL)
	{
		int m_index = m_GPList.GetNextSelectedItem(pos);
		if(CurGPSel != m_index)
			CurGPSel = m_index;
	}	
	*pResult = 0;
}

void CCleChoiceDlg::OnDblclkGplist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnTovip();	
	*pResult = 0;
}
