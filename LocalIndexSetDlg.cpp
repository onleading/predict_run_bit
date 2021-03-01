// LocalIndexSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "LocalIndexSetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLocalIndexSetDlg dialog

#include "SelectGPDlg.h"

CLocalIndexSetDlg::CLocalIndexSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLocalIndexSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLocalIndexSetDlg)
	m_Code = _T("");
	m_Name = _T("");
	m_BaseDate = 0;
	m_BaseVol = 0.0;
	m_Author = _T("");
	m_Description = _T("");
	m_Error = _T("");
	m_lGiveupDays = 0;
	//}}AFX_DATA_INIT
	memset(&m_LcIdxSet,0,sizeof(LCIDXSET));
	m_LcIdxNo	= -1;
	m_LcIdxSet.BaseDate		= 20080808;
	m_LcIdxSet.BaseValue	= 1000;
	m_LcIdxSet.QZType		= 0;
	//
	m_pLcIdxStk		= NULL;
	m_LcIdxStkNum	= 0;
	m_LcIdxStkSpc	= 0;
}

CLocalIndexSetDlg::~CLocalIndexSetDlg()
{
	TDELA(m_pLcIdxStk);
}

void CLocalIndexSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLocalIndexSetDlg)
	DDX_Control(pDX, IDC_LCWEIGHTMODE_COMBO, m_TQTypeCombo);
	DDX_Control(pDX, IDC_LCSTOCK_LIST, m_StockList);
	DDX_Text(pDX, IDC_LCINDEXCODE_EDIT, m_Code);
	DDX_Text(pDX, IDC_LCINDEXNAME_EDIT, m_Name);
	DDX_DateTimeCtrl(pDX, IDC_LCBASEDATE_DATETIMEPICKER, m_BaseDate);
	DDX_Text(pDX, IDC_LCBASEVALUE_EDIT, m_BaseVol);
	DDX_Text(pDX, IDC_LCAUTHOR_EDIT, m_Author);
	DDX_Text(pDX, IDC_LCDESC_EDIT, m_Description);
	DDX_Text(pDX, IDC_LCINDEXERROR_STATIC, m_Error);
	DDX_Text(pDX, IDC_GIVEUPDAYS_EDIT, m_lGiveupDays);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLocalIndexSetDlg, CDialog)
	//{{AFX_MSG_MAP(CLocalIndexSetDlg)
	ON_BN_CLICKED(ID_LCADDSTOCK_BUTTON, OnLcaddstockButton)
	ON_BN_CLICKED(ID_LCDELSTOCK_BUTTON, OnLcdelstockButton)
	ON_BN_CLICKED(ID_LCCLNSTOCK_BUTTON, OnLcclnstockButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLocalIndexSetDlg message handlers
void CLocalIndexSetDlg::SetIdxSet(LPLCIDXSET pLcIdxSet,long lIndexNo)
{
	if(pLcIdxSet)
	{
		memcpy(&m_LcIdxSet,pLcIdxSet,sizeof(LCIDXSET));
		if(g_pLcIdxMng)
		{
			m_LcIdxNo	= lIndexNo;
			g_pLcIdxMng->GetIdxStk(m_pLcIdxStk,m_LcIdxStkNum,m_LcIdxStkSpc,lIndexNo);
		}
	}
}

char *l_strTQTypeStr[20]={"流通股本加权","流通股分级靠档","总股本加权","价格权重(不复权)","价格权重(等比前复权)","价格权重(等比后复权)","同等权重"};

BOOL CLocalIndexSetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	GetDlgItem(IDC_LCFIX)->SetWindowText(LCIDX_FIX);

	char StockListHeadStr[][10] = {"证券代码","证券简称"};
	int StockListHeadWid[] = {80,80};

	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_GP));
	m_ImageList.Add(hIcon);

	m_StockList.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_StockList.SetExtendedStyle(m_StockList.GetExtendedStyle()|LVS_EX_FULLROWSELECT);

	int i=0;
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;

	for(i=0; i<2;i++)
	{
		lvColumn.pszText	= _F(StockListHeadStr[i]);
		lvColumn.cx			= StockListHeadWid[i];
		lvColumn.iSubItem	= i;
		m_StockList.InsertColumn (i,&lvColumn);
	}

	m_TQTypeCombo.ResetContent();
	for(i=0;i<7;i++)
		m_TQTypeCombo.AddString(_F(l_strTQTypeStr[i]));	
	m_TQTypeCombo.SetCurSel(m_LcIdxSet.QZType);

	m_Code			= m_LcIdxSet.Code+3;
	m_Name			= m_LcIdxSet.Name;
	m_BaseDate		= CTime(m_LcIdxSet.BaseDate/10000,m_LcIdxSet.BaseDate/100%100,m_LcIdxSet.BaseDate%100,0,0,0);
	m_BaseVol		= m_LcIdxSet.BaseValue;
	m_Author		= m_LcIdxSet.Author;
	m_lGiveupDays	= m_LcIdxSet.lGiveupDays;
	m_Description	= m_LcIdxSet.Descript;

	FillStockList();

	GetDlgItem(IDC_LCAUTHOR_EDIT)->EnableWindow(m_LcIdxNo<0);
	UpdateData(FALSE);
	
	return TRUE;
}

void CLocalIndexSetDlg::FillStockList()
{
	long i=0;
	int nItem=0;
	MemStkInfo	*pCurStock = NULL;
	
	m_StockList.SetRedraw(FALSE);
	m_StockList.DeleteAllItems();

	for(i=0;i<m_LcIdxStkNum;i++)
	{
		nItem = m_StockList.InsertItem(i,m_pLcIdxStk[i].Code);
		m_StockList.SetItemData(nItem,i);
		pCurStock = g_pStockIO->Get_MemStkInfo(m_pLcIdxStk[i].Code,m_pLcIdxStk[i].nSetCode);
		if(pCurStock) m_StockList.SetItemText(nItem,1,pCurStock->Name);
		else m_StockList.SetItemText(nItem,1,m_pLcIdxStk[i].Code);
	}
	m_StockList.SetRedraw(TRUE);
}

void CLocalIndexSetDlg::OnLcaddstockButton() 
{
	UpdateData(TRUE);

	CSelectGPDlg dlg;
	dlg.bSingleSelect = FALSE;

	int	i=0,j=0;
	BOOL bFind=FALSE;

	if(dlg.DoModal() == IDOK)
	{
		if(dlg.SelectedGPNum > 0)
		{
			MemStkInfo *pMemStkInfo;
			short typeflag = CODE_SZOTHER;
			for(i = 0;i < dlg.SelectedGPNum;i++)
			{
				pMemStkInfo = (*g_pStockIO)[dlg.SelectedGPIndex[i]];
				if(!pMemStkInfo) continue;
				if(testzs(pMemStkInfo) || g_pLcIdxMng->TestIndexExist(pMemStkInfo->Code)) continue;
				bFind = FALSE;
				for(j = 0;j < m_LcIdxStkNum;j++)
				{
					if(pMemStkInfo->setcode==m_pLcIdxStk[j].nSetCode
						&& strncmp(pMemStkInfo->Code,m_pLcIdxStk[j].Code,SH_CODE_LEN)==0)
					{
						bFind = TRUE;
						break;
					}
				}
				if(!bFind)
				{
					BOOL bRet = TRUE;
					LPLCIDXSTK	pTmpLcIdxStk = NULL;
					CHECKLCIDXSPACE(bRet,m_pLcIdxStk,pTmpLcIdxStk,m_LcIdxStkSpc,(m_LcIdxStkNum+1),INDEXSTKSPACE_STEP,LCIDXSTK);
					if(bRet)
					{
						m_pLcIdxStk[m_LcIdxStkNum].nSetCode = pMemStkInfo->setcode;
						strncpy(m_pLcIdxStk[m_LcIdxStkNum].Code,pMemStkInfo->Code,SH_CODE_LEN);
						m_LcIdxStkNum++;
					}
				}
			}
			FillStockList();
		}
	}
	UpdateData(FALSE);
}

void CLocalIndexSetDlg::OnLcdelstockButton() 
{
	UpdateData(TRUE);
	POSITION pos = m_StockList.GetFirstSelectedItemPosition();
	if(pos == NULL)	return;
	int nItem = m_StockList.GetNextSelectedItem(pos);
	if(nItem<0 || nItem>=m_StockList.GetItemCount()) return;
	DWORD nIndex = m_StockList.GetItemData(nItem);
	if(nIndex<0 || nIndex>=m_LcIdxStkNum)
	{
		FillStockList();
		return;
	}
	if(nIndex!=m_LcIdxStkNum-1)
		memmove(&(m_pLcIdxStk[nIndex]),&(m_pLcIdxStk[nIndex+1]),(m_LcIdxStkNum-1-nIndex)*sizeof(LCIDXSTK));
	m_LcIdxStkNum--;
	memset(&(m_pLcIdxStk[m_LcIdxStkNum]),0,sizeof(LCIDXSTK));
	FillStockList();
	if(nItem>=m_StockList.GetItemCount()) m_StockList.SetItemState(m_StockList.GetItemCount()-1,LVIS_SELECTED,LVIS_SELECTED);
	else m_StockList.SetItemState(nItem,LVIS_SELECTED,LVIS_SELECTED);
	UpdateData(FALSE);
}

void CLocalIndexSetDlg::OnLcclnstockButton() 
{
	if(m_pLcIdxStk!=NULL && m_LcIdxStkSpc>0)
		memset(m_pLcIdxStk,0,m_LcIdxStkSpc*sizeof(LCIDXSTK));
	m_LcIdxStkNum	= 0;
	FillStockList();
}

void CLocalIndexSetDlg::OnOK() 
{
	UpdateData(TRUE);
	BOOL bNoError = TRUE;
	if(bNoError)
	{
		m_Code.TrimLeft();
		m_Code.TrimRight();
		if(m_Code.GetLength()!=SH_CODE_LEN-3)
		{
			m_Error = _F("请填写完整品种代码后3位.");
			bNoError = FALSE;
		}
	}
	if(bNoError)
	{
		m_Name.TrimLeft();
		m_Name.TrimRight();
		if(m_Name.GetLength()<=0 || m_Name.GetLength()>NAME_LEN)
		{
			m_Error = _F("请填写品种简称,要求不多于8个字符或4个汉字.");
			bNoError = FALSE;
		}
	}
	if(bNoError)
	{
		CTime tmptm = CTime::GetCurrentTime();
		if(m_BaseDate<CTime(1990,12,19,0,0,0))
		{
			m_Error = _F("基准时间太早,请作调节.");
			bNoError = FALSE;
		}
		else if(m_BaseDate>=tmptm)
		{
			m_Error = _F("基准时间不得为未来时间,请作调节.");
			bNoError = FALSE;
		}
	}
	if(bNoError)
	{
		if(m_BaseVol<0.8)
		{
			m_Error = _F("基准数值至少应为1,请作调节.");
			bNoError = FALSE;
		}
		if(m_BaseVol>100000+COMPPREC)
		{
			m_Error = _F("基准数值不得大于100000,请作调节.");
			bNoError = FALSE;
		}
	}
	if(bNoError)
	{
		if(m_lGiveupDays<0)
		{
			m_Error = _F("新股剔除交易日数不得为负,请作调节.");
			bNoError = FALSE;
		}
		if(m_lGiveupDays>100)
		{
			m_Error = _F("新股剔除交易日数不得大于100,请作调节.");
			bNoError = FALSE;
		}
	}
	if(bNoError)
	{
		if(m_LcIdxStkNum<=0)
		{
			m_Error = _F("无样本股,请作调节.");
			bNoError = FALSE;
		}
	}
	CString tmpstr = LCIDX_FIX+m_Code;
	strncpy(m_LcIdxSet.Code,tmpstr,SH_CODE_LEN);
	strncpy(m_LcIdxSet.Name,m_Name,NAME_LEN);
	if(bNoError)
	{
		if(g_pLcIdxMng->TestIndexExist(m_LcIdxSet.Code,m_LcIdxNo))
		{
			m_Error = _F("已经有相同代码的品种,请作修改.");
			bNoError = FALSE;
		}
	}
	if(!bNoError)
	{
		UpdateData(FALSE);
		return;
	}
	strncpy(m_LcIdxSet.Author,m_Author,30);
	m_LcIdxSet.nUnitNum		= m_LcIdxStkNum;
	m_LcIdxSet.QZType		= m_TQTypeCombo.GetCurSel();
	m_LcIdxSet.BaseDate		= m_BaseDate.GetYear()*10000+m_BaseDate.GetMonth()*100+m_BaseDate.GetDay();
	m_LcIdxSet.BaseValue	= m_BaseVol;
	m_LcIdxSet.lGiveupDays	= m_lGiveupDays;
	strncpy(m_LcIdxSet.Descript,m_Description,200);
	CDialog::OnOK();
}

void CLocalIndexSetDlg::OnCancel() 
{
	CDialog::OnCancel();
}
