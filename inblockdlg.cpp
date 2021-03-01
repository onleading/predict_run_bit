// inblockdlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "inblockdlg.h"
#include "GridView.h"
#include "MyFileFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInBlockDlg dialog

static BOOL l_bWithGP=TRUE;
CInBlockDlg::CInBlockDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInBlockDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInBlockDlg)
	m_bWithGP = FALSE;
	//}}AFX_DATA_INIT
	m_nCurGP = -1;
	m_nCurSelBlock = -1;
	m_nBlockNum = 0;
	m_nCurSelFy=-1;
	m_nFyNum=0;
	m_bFindInFY=FALSE;
}


void CInBlockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInBlockDlg)
	DDX_Control(pDX, IDC_INFO_STATIC, m_Info_Static);
	DDX_Control(pDX, IDC_BLOCK_LIST2, m_Block_List2);
	DDX_Control(pDX, IDC_BLOCK_LIST, m_Block_List);
	DDX_Check(pDX, IDC_WITHGP_CHECK, m_bWithGP);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInBlockDlg, CDialog)
	//{{AFX_MSG_MAP(CInBlockDlg)
	ON_MESSAGE(UM_SEARCH_BLOCK,SearchBlock)
	ON_NOTIFY(NM_DBLCLK, IDC_BLOCK_LIST, OnDblclkBlockList)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_WITHGP_CHECK, OnWithgpCheck)
	ON_NOTIFY(NM_DBLCLK, IDC_BLOCK_LIST2,OnDblclkBlockList)
	ON_BN_CLICKED(IDC_FINDINFY, OnFindinfy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInBlockDlg message handlers

BOOL CInBlockDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Info_Static.SetTextColor(RGB(255,0,0));

	//股票列表框
	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,2,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_BLOCK));
	m_ImageList.Add(hIcon);
	hIcon=::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_BLOCK3));
	m_ImageList.Add(hIcon);
	m_Block_List.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_Block_List2.SetImageList(&m_ImageList,LVSIL_SMALL);

	InitList();

	m_bWithGP = l_bWithGP;
	if(m_nCurGP >= 0)
		SendMessage(UM_SEARCH_BLOCK,m_nCurGP);
	SetTimer(0x100,500,NULL);
	//如果没有趋势导航，则不显示趋势导航查找功能
	if(g_vfyGroup.size()==0)
		GetDlgItem(IDC_FINDINFY)->ShowWindow(SW_HIDE);

	UpdateData(FALSE);
	return TRUE;
}

char *InBlockStr[3] = {"所属板块","板块类型","证券数"};
int   InBlockWidth[3] = {130,75,50};
char *InBlockStr2[2] = {"趋势导航信息","所在行"};
int   InBlockWidth2[2] = {130,50};

void CInBlockDlg::InitList()
{
	//重新插入列
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_CENTER;
	int i;
	for(i = 0; i < 3;i++)
	{
		lvColumn.pszText = _F(InBlockStr[i]);
		lvColumn.cx = InBlockWidth[i];
		lvColumn.iSubItem = i;
		m_Block_List.InsertColumn (i,&lvColumn);
	}
	for(i=0;i<2;i++)
	{
		lvColumn.pszText=_F(InBlockStr2[i]);
		lvColumn.cx = InBlockWidth2[i];
		lvColumn.iSubItem = i;
		m_Block_List2.InsertColumn (i,&lvColumn);
	}
}

BOOL CInBlockDlg::ParseHeadInfo(ColHeadInfo &colinfo,char *colstr)
{
	colinfo.calcType = CALC_STATIC;
	colinfo.dynaID = -1;
	strcpy(colinfo.strTitle, "NULL");
	colinfo.width = DEF_ITEM_WIDTH;
	colinfo.dataType = TYPE_STRING;
	colinfo.strFormula="NULL";
	
	char	*argv[4];
	int argc = GetCmdLine(colstr,argv,4,"&");
	if(argc!=4)		//错误格式
	{
		return FALSE;
	}
	//解析名称
	switch(argv[0][0])  
	{
	case '$':
		{
			colinfo.calcType = CALC_DYNA_TXT;
			if(strcmp(argv[0],"$ZQDM")==0)
				colinfo.dynaID=ZQDM;
		}
		break;
	case '^':
		{
			colinfo.calcType = CALC_DYNA_NUM;
			colinfo.dynaID=-2;
		}
		break;
	default:
		{
			colinfo.calcType = CALC_STATIC;
			colinfo.dynaID = -1;
		}
		break;
	}
	return TRUE;
}

//*
const char strSubSep[] = {'#','$','#'};
void CInBlockDlg::FindInFyGroup(int nGroup)
{
	if(g_vfyGroup[nGroup].subGroup.size()>0) return;
	int m_nGPCode=-1;	//股票代码字段所在列号
	if(g_vfyGroup[nGroup].nType==1)
		return;
	char srrMsg[128]={0};
	char strFileName[MAX_PATH]={0};
	sprintf(strFileName, "%s%02d.dat",g_strTendName,g_vfyGroup[nGroup].nIndex_file);
	CString	Ini = g_WSXHStr+"LData\\cache\\"+(CString)strFileName;	
	CFile theFile;
	if(!theFile.Open(Ini,CFile::modeRead|CFile::shareDenyWrite))
		return;
	long filesize=theFile.GetLength();
	byte *fbuf=new byte[filesize];
	if(!fbuf)
	{
		theFile.Close();
		return;
	}
	long nRead=theFile.ReadHuge(fbuf,filesize);
	theFile.Close();
	if(nRead==0)
	{
		TDEL(fbuf);
		return;
	}
	byte *buf=NULL;
	long bufsize=0;
	if(g_vfyGroup[nGroup].bEncrypted)
	{//如果是加密文件,则解密
		BYTE key[]=TEA_KEY;
		if(!g_FF.mTEA_decipher(fbuf,filesize,buf,bufsize,key,TEA_KEY_LEN))
		{
			TDEL(fbuf);
			return;
		}
		TDEL(fbuf);
	}
	else
	{
		buf=fbuf;
		bufsize=filesize;
	}

	char	lpString[MAX_FYONE_LEN],*argv[MAX_FYCOL];
	int nStarPos = -1, nEndPos = -1;
	int nSubGrid = -1;

	istrstream istr((const char*)buf,bufsize);
	for(int i=0,nStk=0;istr;++i)
	{
		memset(lpString,0,MAX_FYONE_LEN);
		istr.getline(lpString,MAX_FYONE_LEN-1);

		if(strncmp(lpString, strSubSep, sizeof(strSubSep))==0) 
		{
			++nSubGrid;		//发现一个功能
			nStarPos = i+1;
			nStk = 0;
			continue;
		}
		if(nStarPos>0&&nSubGrid>g_vfyGroup[nGroup].strItemName.size()-1) break;
		if(i==nStarPos)		//读取功能配置
		{
			int argc = GetCmdLine(lpString,argv,MAX_FYCOL,"|");
			for(int j=0;j<argc;j++)
			{
				char strTmp[128]={0};
				strncpy(strTmp, argv[j], 127);
				ColHeadInfo chi;
				if(!ParseHeadInfo(chi, strTmp))
					break;
				if(chi.dynaID==ZQDM)
					m_nGPCode=j;
				chi.nHeadID = j;
				if(j>=TOTAL_COL-1)
					break;
			}
		}
		if(nStarPos>0&&i>nStarPos)		//读取行数据
		{
			CString strRow(lpString);
			TrimString(strRow);
			if(strRow.GetLength()>0)
			{
				if(m_nGPCode>=0)
				{
					CString tmpstr=strRow;
					char *argv[MAX_FYCOL];
					int argc = GetCmdLine(tmpstr.GetBuffer(MAX_FYONE_LEN),argv,MAX_FYCOL,"|");
					int ncode=argv[m_nGPCode]-argv[0];
				
					MemStkInfo *pstkinfo=g_pStockIO->Get_MemStkInfo(argv[m_nGPCode]);
					if(pstkinfo)
					{
						nStk++;
						if(g_pStockIO->GetIndex(argv[m_nGPCode], pstkinfo->setcode)==m_nCurGP)
						{
							m_FyInf[m_nFyNum].nGroup=nGroup;
							m_FyInf[m_nFyNum].nItem=nSubGrid;
							strcpy(m_FyInf[m_nFyNum].strGroupName,g_vfyGroup[nGroup].strGroupName);
							strcpy(m_FyInf[m_nFyNum].strItemName,g_vfyGroup[nGroup].strItemName[nSubGrid]);
							m_FyInf[m_nFyNum].nRow=nStk;
							m_nFyNum++;
						}
					}
					tmpstr.ReleaseBuffer();
				}
			}
		}
	}
	TDEL(buf);
}
//*/

void CInBlockDlg::StartSearch()
{
	int i,j,gpnum;
	short indexbuff[MAXGPNUM];
	m_nBlockNum = 0;
	for(i = 0;i < g_nBlockNum;i++)
	{
		gpnum = FiltBlockCode(i+2,indexbuff);//i+2除掉自选股和股票池
		for(j=0;j<gpnum;j++)
			if(indexbuff[j]==m_nCurGP) break;
		if(j < gpnum)
		{
			strcpy(m_BlockInf[m_nBlockNum].strBlockName,g_BlockInfo[i].Name);
			m_BlockInf[m_nBlockNum].gpnum = gpnum;
			m_BlockInf[m_nBlockNum].nBlockNo = i;
			m_nBlockNum++;
		}
	}
	for(i = 0;i < TOTAL_DY_NUM;i++)
	{
		gpnum = FiltDYHYCode(0,i,indexbuff);
		for(j=0;j<gpnum;j++)
			if(indexbuff[j]==m_nCurGP) break;
		if(j < gpnum)
		{
			strcpy(m_BlockInf[m_nBlockNum].strBlockName,_F(g_cwAddrTopic[i]));
			m_BlockInf[m_nBlockNum].gpnum = gpnum;
			m_BlockInf[m_nBlockNum].nBlockNo = i+DY_START;
			m_nBlockNum++;
		}
	}
	for(i = 0;i < TOTAL_HY_NUM;i++)
	{
		gpnum = FiltDYHYCode(1,i,indexbuff);
		for(j=0;j<gpnum;j++)
			if(indexbuff[j]==m_nCurGP) break;
		if(j < gpnum)
		{
			strcpy(m_BlockInf[m_nBlockNum].strBlockName,_F(g_cwHYstr[i]));
			m_BlockInf[m_nBlockNum].gpnum = gpnum;
			m_BlockInf[m_nBlockNum].nBlockNo = i+HY_START;
			m_nBlockNum++;
		}
	}
	//在证监会行业中查找
	ZJHHY_Cfg *	pzjhhy = NULL;
	MemStkInfo *tmpInfo = (*g_pStockIO)[m_nCurGP];
	if(tmpInfo) pzjhhy = g_pConstMng->GetHyFromHYID(tmpInfo->J_zjhhy);
	if(pzjhhy)
	{
		strcpy(m_BlockInf[m_nBlockNum].strBlockName,_F(pzjhhy->HYName));
		m_BlockInf[m_nBlockNum].nBlockNo = g_pConstMng->GetHyXHFromHYID(tmpInfo->J_zjhhy)+ZJHHY_START;		
		m_BlockInf[m_nBlockNum].gpnum = 0;
		MemStkInfo *tmpInfo1;
		for (i=0;i<g_pStockIO->GetStockNum(ALL_ALL);i++)
		{
			tmpInfo1=(*g_pStockIO)[i];
			if(tmpInfo1 && g_pConstMng->GetHyXHFromHYID(tmpInfo1->J_zjhhy)==m_BlockInf[m_nBlockNum].nBlockNo-ZJHHY_START)
				m_BlockInf[m_nBlockNum].gpnum++;
		}
		m_nBlockNum++;
	}

	for(i = 0;i < g_ConceptBlockNum;i++)
	{
		gpnum = FiltConceptCode(i,indexbuff);
		for(j=0;j<gpnum;j++)
			if(indexbuff[j]==m_nCurGP) break;
		if(j < gpnum)
		{
			strcpy(m_BlockInf[m_nBlockNum].strBlockName,_F(g_ConceptBlock[i].strBlockName));
			m_BlockInf[m_nBlockNum].gpnum = gpnum;
			m_BlockInf[m_nBlockNum].nBlockNo = i+CON_START;
			m_nBlockNum++;
		}
	}
	for(i = 0;i < g_ZHBBlockNum;i++)
	{
		gpnum = FiltZHCode(i,indexbuff);
		for(j=0;j<gpnum;j++)
			if(indexbuff[j]==m_nCurGP) break;
		if(j < gpnum)
		{
			strcpy(m_BlockInf[m_nBlockNum].strBlockName,_F(g_ZHBBlock[i].zhmc));
			m_BlockInf[m_nBlockNum].gpnum = gpnum;
			m_BlockInf[m_nBlockNum].nBlockNo = i+ZHB_START;
			m_nBlockNum++;
		}
	}

	m_nFyNum=0;
	if(m_bFindInFY)
	{
		if(g_vfyGroup.size()==0)
			return;
		if(!g_bFYDataAlwaysSync && !g_bDebugMode)
		{
			g_nDataSyncType=DATASYNC_TEND;
			AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_DATASYNC);
		}
		int n=g_vfyGroup.size();
		for(i=0;i<n;i++)
			FindInFyGroup(i);
	}
}

void CInBlockDlg::SearchBlock(WPARAM wParam,LPARAM lParam)
{
	m_nCurGP = wParam;
	if(m_nCurGP < 0) return;
	MemStkInfo *tmpInfo = (*g_pStockIO)[m_nCurGP];
	CString tmpStr;
	tmpStr.Format("%s(%s)"+(CString)(_F("所属板块")),tmpInfo->Name,tmpInfo->Code);
	SetWindowText(tmpStr);

	StartSearch();

	m_Block_List.DeleteAllItems();
	m_Block_List2.DeleteAllItems();
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	for(int i = 0;i < m_nBlockNum;i++)
	{
		lvItem.iItem = i;
		lvItem.iImage = 0;
		lvItem.pszText = m_BlockInf[i].strBlockName;
		m_Block_List.InsertItem(&lvItem);
		if(m_BlockInf[i].nBlockNo<DY_START)
			tmpStr="自定板块";
		else if(m_BlockInf[i].nBlockNo<HY_START)
			tmpStr="地区板块";
		else if(m_BlockInf[i].nBlockNo<CON_START)
			tmpStr="行业板块";
		else if(m_BlockInf[i].nBlockNo<ZHB_START)
			tmpStr="概念板块";
		else if(m_BlockInf[i].nBlockNo<ZJHHY_START)
			tmpStr="组合板块";
		else
			tmpStr="证监会行业";
		m_Block_List.SetItemText(i,1,_F(tmpStr));
		m_Block_List.SetItemText(i,2,IntToStr(m_BlockInf[i].gpnum));
	}
	for(i=0;i<m_nFyNum;i++)
	{
		lvItem.iItem=i;
		lvItem.iImage=1;
		lvItem.pszText=m_FyInf[i].strItemName;
		m_Block_List2.InsertItem(&lvItem);
		m_Block_List2.SetItemText(i,1,IntToStr(m_FyInf[i].nRow));
	}
}

void CInBlockDlg::OnOK() 
{
	CWnd * SubWnd = GetFocus();
	POSITION pos=0;
	if(SubWnd==&m_Block_List)
	{
		pos= m_Block_List.GetFirstSelectedItemPosition ();
		if(pos)
		{
			int nItem = m_Block_List.GetNextSelectedItem (pos);
			if(nItem >=0 && nItem < m_nBlockNum)
			{
				m_nCurSelBlock = nItem;
				
				int blockno = m_BlockInf[nItem].nBlockNo;
				//向行情窗口发消息,让选股结果显示在行情图中
				g_bOldGridDomain=FALSE;
				CGridView *tempWnd = (CGridView *)SwitchToView("Grid");
				if(tempWnd)
					tempWnd->SendMessage(WM_CHANGEBLOCK,blockno,m_nCurGP);
				g_bOldGridDomain=TRUE;
				AfxGetMainWnd()->SetFocus();
			}
		}
	}
	if(SubWnd==&m_Block_List2)
	{
		pos=m_Block_List2.GetFirstSelectedItemPosition();
		if(pos)
		{
			int nItem=m_Block_List2.GetNextSelectedItem(pos);
			if(nItem>=0 && nItem<m_nFyNum)
			{
				m_nCurSelFy=nItem;
				g_bOldGridDomain=FALSE;
				CGridView *tempWnd = (CGridView *)SwitchToView("Grid");
				if(tempWnd)
				{
					g_FyLocation.nGroup=m_FyInf[m_nCurSelFy].nGroup;
					g_FyLocation.nItem=m_FyInf[m_nCurSelFy].nItem;
					g_FyLocation.nRow=m_FyInf[m_nCurSelFy].nRow;
					tempWnd->SendMessage(WM_CHANGEBLOCK,-3,0);
				}
				g_bOldGridDomain=TRUE;
				AfxGetMainWnd()->SetFocus();
			}
		}
	}
}

void CInBlockDlg::OnCancel() 
{
	GetParent()->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);	
	l_bWithGP=m_bWithGP;
}

void CInBlockDlg::OnDblclkBlockList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnOK();
	*pResult = 0;
}

void CInBlockDlg::OnTimer(UINT nIDEvent) 
{
	if(m_bWithGP && nIDEvent==0x100)	
	{
		int nTmpCurGP=GetSYSCurrentGPIndex();
		if(nTmpCurGP!=-1 && nTmpCurGP!=m_nCurGP)
		{
			m_nCurGP=nTmpCurGP;
			SendMessage(UM_SEARCH_BLOCK,m_nCurGP);
		}
	}	
	CDialog::OnTimer(nIDEvent);
}

void CInBlockDlg::OnWithgpCheck() 
{
	UpdateData(TRUE);	
}

void CInBlockDlg::OnFindinfy() 
{
	m_Info_Static.SetWindowText(_F("搜索中..."));
	m_bFindInFY=TRUE;
	if(m_nCurGP >= 0)
		SendMessage(UM_SEARCH_BLOCK,m_nCurGP);
	m_bFindInFY=FALSE;
	m_Info_Static.SetWindowText("");
}
