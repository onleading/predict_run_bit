// GpGxdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "GpGxdDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGpGxdDlg dialog


CGpGxdDlg::CGpGxdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGpGxdDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGpGxdDlg)
	//}}AFX_DATA_INIT
	m_nTabSel = 0;
}


void CGpGxdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGpGxdDlg)
	DDX_Control(pDX, IDC_TAB, m_Tab);
	DDX_Control(pDX, IDC_LIST1, m_GPList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGpGxdDlg, CDialog)
	//{{AFX_MSG_MAP(CGpGxdDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, OnColumnclickList1)
	ON_MESSAGE(UM_SEARCH_GPGXD,SearchGpGxd)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGpGxdDlg message handlers

char *GxdListStr[] = {"股票代码","股票名称","贡献点数","涨幅%","昨收盘","参与计算股本(万)","权重","名次"};
int   GxdListWidth[] = {60,60,60,50,55,110,45,40};

void CGpGxdDlg::CalcWeight()
{
	//////////////////////////////////////////////
	int i = 0,j = 0;
	double rate = 0;
#define ReCalcRate(r) if(r<=0.1);\
	else if(r<=0.2) r=0.2;\
	else if(r<=0.3) r=0.3;\
	else if(r<=0.4) r=0.4;\
	else if(r<=0.5) r=0.5;\
	else if(r<=0.6) r=0.6;\
	else if(r<=0.7) r=0.7;\
	else rate=1;
	//////////////////////////////////////////////
	m_pStk = g_pStockIO->Get_MemStkInfo("000001",1);
	g_pStockIO->ReadCurrStockData(m_pStk,&m_hq);
	m_SH000001.SetInitData("上证指数",m_hq.Close);
	for(i=0;i<g_pStockIO->GetStockNum(ALL_SH);i++)
	{
		m_pStk = g_pStockIO->Get_MemStkInfo(1,i);
		if(m_pStk==NULL) continue;
		if(m_pStk->Code[0]!='6' && m_pStk->Code[0]!='9') continue;
		//新股于上市第11个交易日开始计入
		if(m_pStk->Name[0]=='N') continue;
		if(m_pStk->J_start>20000101 && SubLongTime(g_lOpenRq,m_pStk->J_start)<14) 
			continue;
		g_pStockIO->ReadCurrStockData(m_pStk,&m_hq);
		switch(m_pStk->Code[0])
		{
		case '6':
			m_SH000001.AddOne(m_pStk->setcode,m_pStk->Code,m_hq.Now,m_hq.Close,m_pStk->J_zgb-m_pStk->J_hg-m_pStk->J_bg);
			break;
		case '9':
			if(m_pStk->Code[1]=='0') m_SH000001.AddOne(m_pStk->setcode,m_pStk->Code,m_hq.Now,m_hq.Close,m_pStk->J_bg);
			break;
		}
	}
	//////////////////////////////////////////////
	m_pStk = g_pStockIO->Get_MemStkInfo(SZ_DPCODE,SZ);
	g_pStockIO->ReadCurrStockData(m_pStk,&m_hq);
	m_SZ399001.SetInitData("深证成指",m_hq.Close);
	for(i=0;i<g_ConceptBlockNum;i++)
	{
		if(strcmp(g_ConceptBlock[i].strBlockName,"深证成指")==0)
		{
			for(j=0;j<g_ConceptBlock[i].nStockNum;j++)
			{
				m_pStk = (*g_pStockIO)[g_ConceptBlock[i].nContent[j]];
				if(m_pStk==NULL) continue;
				if(m_pStk->Name[0]=='N') continue;
				g_pStockIO->ReadCurrStockData(m_pStk,&m_hq);
				rate = m_pStk->ActiveCapital/(m_pStk->J_zgb-m_pStk->J_hg-m_pStk->J_bg);
				m_SZ399001.AddOne(m_pStk->setcode,m_pStk->Code,m_hq.Now,m_hq.Close,(m_pStk->J_zgb-m_pStk->J_hg-m_pStk->J_bg)*rate);
			}
		}
	}
	//////////////////////////////////////////////
	m_pStk = g_pStockIO->Get_MemStkInfo(HS300_SHZS,SH);
	g_pStockIO->ReadCurrStockData(m_pStk,&m_hq);
	m_HS300.SetInitData("沪深300",m_hq.Close);
	for(i=0;i<g_ConceptBlockNum;i++)
	{
		if(strcmp(g_ConceptBlock[i].strBlockName,"沪深300")==0)
		{
			for(j=0;j<g_ConceptBlock[i].nStockNum;j++)
			{
				m_pStk = (*g_pStockIO)[g_ConceptBlock[i].nContent[j]];
				if(m_pStk==NULL) continue;
				if(m_pStk->Name[0]=='N') continue;
				g_pStockIO->ReadCurrStockData(m_pStk,&m_hq);
				rate = m_pStk->ActiveCapital/(m_pStk->J_zgb-m_pStk->J_hg-m_pStk->J_bg);
				ReCalcRate(rate);
				m_HS300.AddOne(m_pStk->setcode,m_pStk->Code,m_hq.Now,m_hq.Close,(m_pStk->J_zgb-m_pStk->J_hg-m_pStk->J_bg)*rate);
			}
			break;
		}
	}
	//////////////////////////////////////////////
	m_pStk = g_pStockIO->Get_MemStkInfo(ZXB_DPCODE,SZ);
	g_pStockIO->ReadCurrStockData(m_pStk,&m_hq);
	m_SZ399005.SetInitData("中小板指",m_hq.Close);
	for(i=0;i<g_ConceptBlockNum;i++)
	{
		if(strcmp(g_ConceptBlock[i].strBlockName,"中小板指")==0)
		{
			for(j=0;j<g_ConceptBlock[i].nStockNum;j++)
			{
				m_pStk = (*g_pStockIO)[g_ConceptBlock[i].nContent[j]];
				if(m_pStk==NULL) continue;
				if(m_pStk->Name[0]=='N') continue;
				g_pStockIO->ReadCurrStockData(m_pStk,&m_hq);
				rate = m_pStk->ActiveCapital/(m_pStk->J_zgb-m_pStk->J_hg-m_pStk->J_bg);
				m_SZ399005.AddOne(m_pStk->setcode,m_pStk->Code,m_hq.Now,m_hq.Close,(m_pStk->J_zgb-m_pStk->J_hg-m_pStk->J_bg)*rate);
			}
			break;
		}
	}
	//////////////////////////////////////////////
	m_SH000001.ReCalcWeight();
	m_SZ399001.ReCalcWeight();
	m_HS300.ReCalcWeight();
	m_SZ399005.ReCalcWeight();
}

BOOL CGpGxdDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString tmpStr;
	m_Tab.InsertItem(0,_F("上证指数"));
	m_Tab.InsertItem(1,_F("深证成指"));
	m_Tab.InsertItem(2,_F("沪深300指数"));
	m_Tab.InsertItem(3,_F("中小板指"));
	m_Tab.SetCurSel(0);
	m_nTabSel = 0;

	m_ImageList.Create(16,16,ILC_COLOR4|ILC_MASK,1,0);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_GP));
	m_ImageList.Add(hIcon);

	m_GPList.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_GPList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_CENTER;
	for(int i = 0; i < 8;i++)
	{
		lvColumn.pszText = _F(GxdListStr[i]);
		lvColumn.cx = GxdListWidth[i];
		lvColumn.iSubItem = i;
		m_GPList.InsertColumn (i,&lvColumn);
	}

	CalcWeight();

	SetTimer(0x100,90*1000,NULL);
	return TRUE;
}

void CGpGxdDlg::SearchGpGxd(WPARAM wParam,LPARAM lParam)
{
	if(wParam<0 || wParam>3) return;

	m_Tab.SetCurSel(wParam);
	m_nTabSel = wParam;
	FillList(m_nTabSel);
}

//用于比较贡献度的回调函数
int GxdSort(const void * a,const void *b)
{
	STKWEIGHT *sorta,*sortb;
	sorta = (STKWEIGHT *)a;
	sortb = (STKWEIGHT *)b;
	if(sorta->fResult < sortb->fResult) return 1;
	if(sorta->fResult > sortb->fResult) return -1;
	return 0;
}

void CGpGxdDlg::FillList(int nSel)
{	
	if(OnLine)	RefreshAllHq(120);

	int i = 0,j = 0;
	LPSTKWEIGHT pStkWeight = NULL;
	int nStkWeight = 0;
	switch(nSel)
	{
	case 0:
		for(i=0;i<g_pStockIO->GetStockNum(ALL_SH);i++)
		{
			m_pStk = g_pStockIO->Get_MemStkInfo(1,i);
			if(m_pStk==NULL) continue;
			if(m_pStk->Code[0]!='6' && m_pStk->Code[0]!='9') continue;
			g_pStockIO->ReadCurrStockData(m_pStk,&m_hq);
			m_SH000001.UpdateResult(m_pStk->setcode,m_pStk->Code,m_hq.Now);
		}
		nStkWeight = m_SH000001.GetAllResult(pStkWeight);
		break;
	case 1:
		for(i=0;i<g_ConceptBlockNum;i++)
		{
			if(strcmp(g_ConceptBlock[i].strBlockName,"深证成指")==0)
			{
				for(j=0;j<g_ConceptBlock[i].nStockNum;j++)
				{
					m_pStk = (*g_pStockIO)[g_ConceptBlock[i].nContent[j]];
					if(m_pStk==NULL) continue;
					g_pStockIO->ReadCurrStockData(m_pStk,&m_hq);
					m_SZ399001.UpdateResult(m_pStk->setcode,m_pStk->Code,m_hq.Now);
				}
			}
		}
		nStkWeight = m_SZ399001.GetAllResult(pStkWeight);
		break;
	case 2:
		for(i=0;i<g_ConceptBlockNum;i++)
		{
			if(strcmp(g_ConceptBlock[i].strBlockName,"沪深300")==0)
			{
				for(j=0;j<g_ConceptBlock[i].nStockNum;j++)
				{
					m_pStk = (*g_pStockIO)[g_ConceptBlock[i].nContent[j]];
					if(m_pStk==NULL) continue;
					g_pStockIO->ReadCurrStockData(m_pStk,&m_hq);
					m_HS300.UpdateResult(m_pStk->setcode,m_pStk->Code,m_hq.Now);
				}
			}
		}
		nStkWeight = m_HS300.GetAllResult(pStkWeight);
		break;
	case 3:
		for(i=0;i<g_pStockIO->GetStockNum(ALL_SZ);i++)
		{
			m_pStk = g_pStockIO->Get_MemStkInfo(short(0),i);
			if(m_pStk==NULL) continue;
			if(m_pStk->Code[0]!='0' || m_pStk->Code[1]!='0' || m_pStk->Code[2]!='2') continue;
			g_pStockIO->ReadCurrStockData(m_pStk,&m_hq);
			m_SZ399005.UpdateResult(m_pStk->setcode,m_pStk->Code,m_hq.Now);
		}
		nStkWeight = m_SZ399005.GetAllResult(pStkWeight);
		break;
	}
	if(nStkWeight>0)
		qsort((void *)pStkWeight,nStkWeight,sizeof(STKWEIGHT),GxdSort);

	m_GPList.SetRedraw(FALSE);
	m_GPList.DeleteAllItems();
	int nCnt=0;
	char tmpstr[100];
	int nStockNo;
	MemStkInfo *tmpInfo=NULL;
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;

	for(i=0;i<nStkWeight;i++)
	{
		nStockNo = g_pStockIO->GetIndex(pStkWeight[i].StkCode,pStkWeight[i].SetCode);
		if(nStockNo<0) continue;
		tmpInfo = (*g_pStockIO)[nStockNo];
		if(!tmpInfo) continue;
		lvItem.iItem = nCnt;
		lvItem.iImage = 0;
		lvItem.pszText = tmpInfo->Code;
		m_GPList.InsertItem(&lvItem);

		m_GPList.SetItemText(nCnt,1,tmpInfo->Name);

		sprintf(tmpstr,"%.2f",pStkWeight[i].fResult);
		m_GPList.SetItemText(nCnt,2,tmpstr);
		
		if(pStkWeight[i].fClose>COMPPREC && pStkWeight[i].fNow>COMPPREC)
		{
			sprintf(tmpstr,"%.2f",(pStkWeight[i].fNow-pStkWeight[i].fClose)*100.0/pStkWeight[i].fClose);
			m_GPList.SetItemText(nCnt,3,tmpstr);
		}

		sprintf(tmpstr,"%.3f",pStkWeight[i].fClose);
		m_GPList.SetItemText(nCnt,4,tmpstr);
		
		sprintf(tmpstr,"%12.2f",pStkWeight[i].fCapital);
		m_GPList.SetItemText(nCnt,5,tmpstr);

		sprintf(tmpstr,"%2.2f",pStkWeight[i].fClose*pStkWeight[i].dWeight*100);
		m_GPList.SetItemText(nCnt,6,tmpstr);

		sprintf(tmpstr,"%d",nCnt+1);
		m_GPList.SetItemText(nCnt,7,tmpstr);

		m_GPList.SetItemData(nCnt,nStockNo);
		nCnt++;
	}

	m_GPList.SetRedraw(TRUE);
	m_GPList.m_nSortedCol=2;
	m_GPList.m_fAsc=FALSE;
}

void CGpGxdDlg::OnCancel()
{
	GetParent()->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);	
}

void CGpGxdDlg::OnOK() 
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

void CGpGxdDlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnOK();
	*pResult = 0;
}

void CGpGxdDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_nTabSel=m_Tab.GetCurSel();
	FillList(m_nTabSel);
	
	*pResult = 0;
}

int CALLBACK  MyListCompare_GpGxd(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
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
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
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

void CGpGxdDlg::OnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult) 
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
	m_GPList.SortItems(MyListCompare_GpGxd, (LPARAM)&m_GPList); 

	//还原ItemData
	for ( i = 0; i < nCnt; ++i)
	{
		j=m_GPList.GetItemData(i);
		m_GPList.SetItemData(i, nTmpStockNo[j]);
	}
	delete nTmpStockNo;
	*pResult = 0;
}

void CGpGxdDlg::OnTimer(UINT nIDEvent)
{
	if(nIDEvent==0x100)	
		FillList(m_nTabSel);
	CDialog::OnTimer(nIDEvent);
}

void CGpGxdDlg::OnExport() 
{
	CString tmpFilename;
	CFileDialog file(FALSE,"*.txt",_F("贡献度.txt"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_F("文本文件(*.txt)|*.txt|所有文件(*.*)|*.*||"));
	if(file.DoModal()==IDOK)
	{
		tmpFilename=file.GetPathName();
		BOOL bRet = ListCtrl2Txt(&m_GPList,tmpFilename);
		if(bRet && TDX_MessageBox(m_hWnd,"导出成功,是否打开此文件?",MB_ICONQUESTION|MB_OKCANCEL)==IDOK)
		{
			CString tmpStr;
			tmpStr.Format("\"%s\"",tmpFilename);
			::ShellExecute(NULL,NULL,tmpStr,NULL,NULL,SW_SHOW);
		}		
	}
}
