// ChangeDomainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "ChangeDomainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangeDomainDlg dialog


CChangeDomainDlg::CChangeDomainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChangeDomainDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangeDomainDlg)
	//}}AFX_DATA_INIT
	m_nTotalNum = 0;
	m_nSelDomain = 0;
	memset(m_GpDomain,0,MAX_DOMAIN_SELNUM*sizeof(struct GpDomain_Struct));
	memset(m_bChecked,0,MAX_DOMAIN_SELNUM*sizeof(short));

	m_bSelectOne = FALSE;
}


void CChangeDomainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangeDomainDlg)
	DDX_Control(pDX, IDC_DOMAIN_LIST, m_Domain_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangeDomainDlg, CDialog)
	//{{AFX_MSG_MAP(CChangeDomainDlg)
	ON_NOTIFY(NM_CLICK, IDC_DOMAIN_LIST, OnClickDomainList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_DOMAIN_LIST, OnKeydownDomainList)
	ON_MESSAGE(UM_FILLDOMAIN,FillDomain)
	ON_NOTIFY(NM_DBLCLK, IDC_DOMAIN_LIST, OnDblclkDomainList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeDomainDlg message handlers

BOOL CChangeDomainDlg::TestSelected(int flag,int which)
{
	for(int i=0;i < m_nSelDomain;i++)
		if(m_GpDomain[i].flag==flag && m_GpDomain[i].which==which) break;
	if(i < m_nSelDomain) return TRUE;
	return FALSE;
}

BOOL CChangeDomainDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_bSelectOne)
		SetWindowText(_F("选择分类或板块"));

	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_BLOCK));
	m_ImageList.Add(hIcon);
	hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_BLOCK_SELECTED));
	m_ImageList.Add(hIcon);
	m_Domain_List.SetImageList(&m_ImageList,LVSIL_SMALL);

	m_Domain_List.SetExtendedStyle (LVS_EX_FULLROWSELECT);

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 100;
	lvColumn.iSubItem = 0;
	lvColumn.pszText  = "F0514M0824";
	m_Domain_List.InsertColumn (0,&lvColumn);

	PostMessage(UM_FILLDOMAIN);
	return TRUE;
}

void CChangeDomainDlg::FillDomain(WPARAM wParam,LPARAM lParam)
{
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	m_nTotalNum = 0;
	for(int i=0;i < TOTAL_TYPENUM;i++)
	{
		if(TestSelected(DOMAIN_TYPE,i))
		{
			m_bChecked[m_nTotalNum] = 1;
			lvItem.iImage = 1;
		}
		else
			lvItem.iImage = 0;
		lvItem.iItem = m_nTotalNum++;
		lvItem.pszText = _F(GPType[i]);
		m_Domain_List.InsertItem (&lvItem);
	}

	for(i=0;i < TOTAL_DY_NUM;i++)
	{
		if(TestSelected(DOMAIN_DY,i)) 
		{
			m_bChecked[m_nTotalNum] = 1;
			lvItem.iImage = 1;
		}
		else
			lvItem.iImage = 0;
		lvItem.iItem = m_nTotalNum++;
		lvItem.pszText = _F(g_cwAddrTopic[i]);
		m_Domain_List.InsertItem (&lvItem);
	}		
	for(i=0;i < TOTAL_HY_NUM;i++)
	{
		if(TestSelected(DOMAIN_HY,i)) 
		{
			m_bChecked[m_nTotalNum] = 1;
			lvItem.iImage = 1;
		}
		else
			lvItem.iImage = 0;
		lvItem.iItem = m_nTotalNum++;
		lvItem.pszText = _F(g_cwHYstr[i]);
		m_Domain_List.InsertItem (&lvItem);
	}
	for(i=0;i < g_ConceptBlockNum;i++)
	{
		if(TestSelected(DOMAIN_CON,i)) 
		{
			m_bChecked[m_nTotalNum] = 1;
			lvItem.iImage = 1;
		}
		else
			lvItem.iImage = 0;
		lvItem.iItem = m_nTotalNum++;
		lvItem.pszText = _F(g_ConceptBlock[i].strBlockName);
		m_Domain_List.InsertItem (&lvItem);
	}
	for(i=0;i < g_ZHBBlockNum;i++)
	{
		if(TestSelected(DOMAIN_ZHB,i)) 
		{
			m_bChecked[m_nTotalNum] = 1;
			lvItem.iImage = 1;
		}
		else
			lvItem.iImage = 0;
		lvItem.iItem = m_nTotalNum++;
		lvItem.pszText = _F(g_ZHBBlock[i].zhmc);
		m_Domain_List.InsertItem (&lvItem);
	}
	for(i=0;i < g_nBlockNum;i++)
	{
		if(TestSelected(DOMAIN_USERBLOCK,i)) 
		{
			m_bChecked[m_nTotalNum] = 1;
			lvItem.iImage = 1;
		}
		else
			lvItem.iImage = 0;
		lvItem.iItem = m_nTotalNum++;
		lvItem.pszText = g_BlockInfo[i].Name;
		m_Domain_List.InsertItem (&lvItem);
	}
}

void CChangeDomainDlg::FillGPDomain(int i)
{
	if(m_nSelDomain>=MAX_DOMAIN_SELNUM) 
		return;
	if(i < TOTAL_TYPENUM)
	{
		m_GpDomain[m_nSelDomain].flag = DOMAIN_TYPE;
		strcpy(m_GpDomain[m_nSelDomain].Name,GPType[i]);
		m_GpDomain[m_nSelDomain].which = i;
	}
	else if(i < TOTAL_TYPENUM+TOTAL_DY_NUM)
	{
		m_GpDomain[m_nSelDomain].flag = DOMAIN_DY;
		strcpy(m_GpDomain[m_nSelDomain].Name,g_cwAddrTopic[i-TOTAL_TYPENUM]);
		m_GpDomain[m_nSelDomain].which = i-TOTAL_TYPENUM;
	}
	else if(i < TOTAL_TYPENUM+TOTAL_DY_NUM+TOTAL_HY_NUM)
	{
		m_GpDomain[m_nSelDomain].flag = DOMAIN_HY;
		strcpy(m_GpDomain[m_nSelDomain].Name,g_cwHYstr[i-TOTAL_TYPENUM-TOTAL_DY_NUM]);
		m_GpDomain[m_nSelDomain].which = i-TOTAL_TYPENUM-TOTAL_DY_NUM;
	}
	else if(i < TOTAL_TYPENUM+TOTAL_DY_NUM+TOTAL_HY_NUM+g_ConceptBlockNum)
	{
		m_GpDomain[m_nSelDomain].flag = DOMAIN_CON;
		strcpy(m_GpDomain[m_nSelDomain].Name,g_ConceptBlock[i-TOTAL_TYPENUM-TOTAL_DY_NUM-TOTAL_HY_NUM].strBlockName);
		m_GpDomain[m_nSelDomain].which = i-TOTAL_TYPENUM-TOTAL_DY_NUM-TOTAL_HY_NUM;
	}
	else if(i < TOTAL_TYPENUM+TOTAL_DY_NUM+TOTAL_HY_NUM+g_ConceptBlockNum+g_ZHBBlockNum)
	{
		m_GpDomain[m_nSelDomain].flag = DOMAIN_ZHB;
		strcpy(m_GpDomain[m_nSelDomain].Name,g_ZHBBlock[i-TOTAL_TYPENUM-TOTAL_DY_NUM-TOTAL_HY_NUM-g_ConceptBlockNum].zhmc);
		m_GpDomain[m_nSelDomain].which = i-TOTAL_TYPENUM-TOTAL_DY_NUM-TOTAL_HY_NUM-g_ConceptBlockNum;
	}
	else
	{
		m_GpDomain[m_nSelDomain].flag = DOMAIN_USERBLOCK;
		strcpy(m_GpDomain[m_nSelDomain].Name,g_BlockInfo[i-TOTAL_TYPENUM-TOTAL_DY_NUM-TOTAL_HY_NUM-g_ConceptBlockNum-g_ZHBBlockNum].Name);
		m_GpDomain[m_nSelDomain].which = i-TOTAL_TYPENUM-TOTAL_DY_NUM-TOTAL_HY_NUM-g_ConceptBlockNum-g_ZHBBlockNum;
	}
	m_nSelDomain++;
}

void CChangeDomainDlg::OnOK() 
{
	m_nSelDomain = 0;
	if(m_bSelectOne) //如果只是选择一个板块，并不是选择范围
	{
		POSITION pos = m_Domain_List.GetFirstSelectedItemPosition ();
		if(pos)
		{
			int i = m_Domain_List.GetNextSelectedItem (pos);
			FillGPDomain(i);
			CDialog::OnOK();
		}
		return;
	}
	for(int i=0;i < m_nTotalNum;i++)
	{
		if(m_bChecked[i])
			FillGPDomain(i);
	}
	CDialog::OnOK();
}

void CChangeDomainDlg::OnClickDomainList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(!m_bSelectOne)
	{
		POSITION pos;
		pos = m_Domain_List.GetFirstSelectedItemPosition();
		if(pos != NULL)
		{
			int m_index = m_Domain_List.GetNextSelectedItem(pos);
			LV_ITEM lvi;
			lvi.iItem = m_index;
			lvi.iSubItem = 0;
			lvi.mask=LVIF_IMAGE;
			m_Domain_List.GetItem(&lvi);
			if( m_bChecked[m_index] == 0 )
			{
				lvi.iImage=1;
				m_bChecked[m_index] = 1;
			}
			else
			{
				lvi.iImage=0;
				m_bChecked[m_index] = 0;
			}
			m_Domain_List.SetItem(&lvi);
		}
	}	
	*pResult = 0;
}

void CChangeDomainDlg::OnKeydownDomainList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(!m_bSelectOne)
	{
		LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
		if(pLVKeyDow->wVKey == VK_SPACE)
		{
			POSITION pos;
			pos = m_Domain_List.GetFirstSelectedItemPosition();
			if(pos != NULL)
			{
				int m_index = m_Domain_List.GetNextSelectedItem(pos);
				LV_ITEM lvi;
				lvi.iItem = m_index;
				lvi.iSubItem = 0;
				lvi.mask=LVIF_IMAGE;
				m_Domain_List.GetItem(&lvi);
				if( m_bChecked[m_index] == 0 )
				{
					lvi.iImage=1;
					m_bChecked[m_index] = 1;
				}
				else
				{
					lvi.iImage=0;
					m_bChecked[m_index] = 0;
				}
				m_Domain_List.SetItem(&lvi);
			}
		}	
	}
	*pResult = 0;
}

void CChangeDomainDlg::OnDblclkDomainList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(m_bSelectOne) OnOK();	
	*pResult = 0;
}
