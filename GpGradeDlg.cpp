// GpGradeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "GpGradeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL	IsValidGradeDate(long ldate);

/////////////////////////////////////////////////////////////////////////////
// CGpGradeDlg dialog


CGpGradeDlg::CGpGradeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGpGradeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGpGradeDlg)
	//}}AFX_DATA_INIT
	m_pCurStkInfo = NULL;
	m_nTabSel = 2;

	m_pAllGradeInfo = NULL;
	m_nAllGradeNum = 0;
	
	m_pOrgCodeName = NULL;
	m_nOrgNum = 0;
}


void CGpGradeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGpGradeDlg)
	DDX_Control(pDX, IDC_TAB, m_Tab);
	DDX_Control(pDX, IDC_HINT_STATIC, m_Hint_Static);
	DDX_Control(pDX, IDC_LIST1, m_GPList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGpGradeDlg, CDialog)
	//{{AFX_MSG_MAP(CGpGradeDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, OnColumnclickList1)
	ON_MESSAGE(UM_SEARCH_GPGRADE,SearchGpGrade)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGpGradeDlg message handlers

char *GradeListStr[] = {"证券名","评级日期","评级","本年EPS","下年EPS","后年EPS","目标价","机构名称"};
int   GradeListWidth[] = {70,60,54,55,55,55,80,85};
char *g_GradeStr[] = {"卖出","减持","持有","增持","买入"};

char * GetDaysStr(long lDate)
{
	static char s_tmpDaysStr[100];
	long ltmp=SubLongTime(g_iToday,lDate);
	if(ltmp==0)
		strcpy(s_tmpDaysStr,"今天");
	else if(ltmp==1)
		strcpy(s_tmpDaysStr,"昨天");
	else if(ltmp==2)
		strcpy(s_tmpDaysStr,"前天");
	else
		sprintf(s_tmpDaysStr,"%d天前",ltmp);
	return s_tmpDaysStr;
}

BOOL CGpGradeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Hint_Static.SetTextColor(RGB(255,0,0));

	CString tmpStr;
	m_Tab.InsertItem(0,_F("最新评测"));
	m_Tab.InsertItem(1,_F("所有评测"));
	m_Tab.InsertItem(2,_F("个股评测"));

	m_ImageList.Create(16,16,ILC_COLOR4|ILC_MASK,1,0);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_EXP5));
	m_ImageList.Add(hIcon);
	hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_EXP4));
	m_ImageList.Add(hIcon);
	hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_EXP3));
	m_ImageList.Add(hIcon);
	hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_EXP2));
	m_ImageList.Add(hIcon);
	hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_EXP1));
	m_ImageList.Add(hIcon);

	m_GPList.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_GPList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_GPList.m_nSortedCol=3;

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_CENTER;
	for(int i = 0; i < 8;i++)
	{
		lvColumn.pszText = _F(GradeListStr[i]);
		lvColumn.cx = GradeListWidth[i];
		lvColumn.iSubItem = i;
		m_GPList.InsertColumn (i,&lvColumn);
	}

	//取数据
	m_nAllGradeNum = g_pStockIO->GetAllGradeData(m_pAllGradeInfo);
	m_nOrgNum = g_pStockIO->GetAllOrgData(m_pOrgCodeName);

	SetTimer(0x100,500,NULL);
	return TRUE;
}

//wParam为股票Index,lParam为1表示要切换至当前股票的预测页
void CGpGradeDlg::SearchGpGrade(WPARAM wParam,LPARAM lParam)
{
	m_pCurStkInfo = (*g_pStockIO)[wParam];
	CString tmpStr;
	if(m_pCurStkInfo)
		tmpStr.Format("[%s]%s",m_pCurStkInfo->Name,_F("评测"));
	else
		tmpStr.Format(_F("个股评测"));
	m_Tab.DeleteItem(2);
	m_Tab.InsertItem(2,tmpStr);

	if(lParam==1)
	{
		m_nTabSel=2;
		m_Tab.SetCurSel(m_nTabSel);
		FillList();
	}
	else if(2==m_nTabSel)	
		FillList();
}

void CGpGradeDlg::FillList()
{	
	m_GPList.SetRedraw(FALSE);
	m_GPList.DeleteAllItems();
	char tmpstr[100];
	int nStockNo;
	MemStkInfo *tmpInfo=NULL;
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	int i,nAddedItem = 0,ltmp=0;
	for(i=0;i < m_nAllGradeNum;i++)
	{
		if(!IsValidGradeDate(m_pAllGradeInfo[i].lDate)) continue;
		if(m_nTabSel==2 && m_pCurStkInfo)
		{
			if(strcmp(m_pCurStkInfo->Code,m_pAllGradeInfo[i].Code)) 
				continue;
		}
		else if(m_nTabSel==0)
		{
			ltmp=SubLongTime(g_iToday,m_pAllGradeInfo[i].lDate);
			if(ltmp>7) continue;
		}
		nStockNo = g_pStockIO->GetIndex(m_pAllGradeInfo[i].Code);
		tmpInfo = (*g_pStockIO)[nStockNo];
		if(!tmpInfo) continue;

		if(m_pAllGradeInfo[i].lGrade<1 || m_pAllGradeInfo[i].lGrade>5)
		{
			lvItem.iImage = 2;	
			strcpy(tmpstr,"无");
		}
		else
		{
			lvItem.iImage = m_pAllGradeInfo[i].lGrade-1;
			strcpy(tmpstr,g_GradeStr[m_pAllGradeInfo[i].lGrade-1]);
		}
		lvItem.iItem = nAddedItem;
		lvItem.pszText = tmpInfo->Name;
		m_GPList.InsertItem(&lvItem);
		//
		m_GPList.SetItemText(nAddedItem,1,_F(IntToStr(m_pAllGradeInfo[i].lDate)));
		m_GPList.SetItemText(nAddedItem,2,_F(tmpstr));
		//
		if(m_pAllGradeInfo[i].fEps0>COMPPREC)
			m_GPList.SetItemText(nAddedItem,3,FloatToStr(m_pAllGradeInfo[i].fEps0));
		if(m_pAllGradeInfo[i].fEps1>COMPPREC)
			m_GPList.SetItemText(nAddedItem,4,FloatToStr(m_pAllGradeInfo[i].fEps1));
		if(m_pAllGradeInfo[i].fEps2>COMPPREC)
			m_GPList.SetItemText(nAddedItem,5,FloatToStr(m_pAllGradeInfo[i].fEps2));
		m_GPList.SetItemText(nAddedItem,6,m_pAllGradeInfo[i].szTargetPrice);
		//
		if(m_pOrgCodeName[m_pAllGradeInfo[i].lOrgCode].lOrgCode != m_pAllGradeInfo[i].lOrgCode)
		{
			for(int j=0;j<m_nOrgNum;j++)
			{
				if(m_pOrgCodeName[j].lOrgCode == m_pAllGradeInfo[i].lOrgCode)
				{
					m_GPList.SetItemText(nAddedItem,7,m_pOrgCodeName[j].OrgName);
					break;
				}
			}
		}
		else
			m_GPList.SetItemText(nAddedItem,7,m_pOrgCodeName[m_pAllGradeInfo[i].lOrgCode].OrgName);
		
		//
		m_GPList.SetItemData(nAddedItem,nStockNo);
		++nAddedItem;
	}
	m_GPList.SetRedraw(TRUE);
}

void CGpGradeDlg::OnCancel()
{
	GetParent()->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);	
}

void CGpGradeDlg::OnOK() 
{
	POSITION pos = m_GPList.GetFirstSelectedItemPosition ();
	if(pos)
	{
		int nItem = m_GPList.GetNextSelectedItem (pos);
		int GPIndex = m_GPList.GetItemData(nItem);
		if(GPIndex < 0) return;
		ToZst((*g_pStockIO)[GPIndex]->Code,(*g_pStockIO)[GPIndex]->setcode,FALSE);
	}
}

void CGpGradeDlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnOK();
	*pResult = 0;
}

void CGpGradeDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_nTabSel=m_Tab.GetCurSel();
	FillList();
	
	*pResult = 0;
}

int CALLBACK  MyListCompare_GpGrade(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	//通过传递的参数来得到CSortList对象指针，从而得到排序方式
	CSortListCtrl * pV=(CSortListCtrl *)lParamSort;
	//通过ItemData来确定数据
	CString szComp1,szComp2;
	int iCompRes;
	szComp1=pV->GetItemText(lParam1,pV->m_nSortedCol);
	szComp2=pV->GetItemText(lParam2,pV->m_nSortedCol);
	switch(pV->m_nSortedCol)
	{
	case 1:
	case 3:
	case 4:
	case 5:
	case 6:
		iCompRes=atof(szComp1)<=atof(szComp2)?-1:1;
		break;
	default:
		iCompRes=szComp1.Compare(szComp2);
		break;
	}
	//根据当前的排序方式进行调整
	if(pV->m_fAsc)
		return iCompRes;
	else
		return -iCompRes;
}

void CGpGradeDlg::OnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	int nCnt=m_GPList.GetItemCount(),i,j;
	int *nTmpStockNo = new int[nCnt];
	for (i = 0; i < nCnt; i++)
	{
		nTmpStockNo[i]=m_GPList.GetItemData(i);
		m_GPList.SetItemData(i, i);//供排序使用的item编号
	}

	//设置排序方式
	if( pNMListView->iSubItem==m_GPList.m_nSortedCol )
		m_GPList.m_fAsc = !m_GPList.m_fAsc;
	else
	{
		m_GPList.m_fAsc = TRUE;
		m_GPList.m_nSortedCol = pNMListView->iSubItem;
	}
	//调用排序函数,此函数为CListCtrl定义好的,但是需要调用我们定义的函数才比较任意两个项目的值
	m_GPList.SortItems(MyListCompare_GpGrade, (LPARAM)&m_GPList); 

	//还原ItemData
	for ( i = 0; i < nCnt; ++i)
	{
		j=m_GPList.GetItemData(i);
		m_GPList.SetItemData(i, nTmpStockNo[j]);
	}
	delete nTmpStockNo;
	*pResult = 0;
}

void CGpGradeDlg::OnTimer(UINT nIDEvent)
{
	if(nIDEvent==0x100)	
	{
		int nTmpCurGP=GetSYSCurrentGPIndex();
		if(nTmpCurGP!=-1 && (*g_pStockIO)[nTmpCurGP]!=m_pCurStkInfo)
			SendMessage(UM_SEARCH_GPGRADE,nTmpCurGP);
	}	
	CDialog::OnTimer(nIDEvent);
}
