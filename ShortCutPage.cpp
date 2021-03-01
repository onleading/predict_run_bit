// ShortCutPage.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "ShortCutPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShortCutPage property page

IMPLEMENT_DYNCREATE(CShortCutPage, CPropertyPage)

CShortCutPage::CShortCutPage() : CPropertyPage(CShortCutPage::IDD)
{
	//{{AFX_DATA_INIT(CShortCutPage)
	m_bChanged = FALSE;
	//}}AFX_DATA_INIT
}

CShortCutPage::~CShortCutPage()
{
}

void CShortCutPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShortCutPage)
	DDX_Control(pDX, IDC_LIST, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShortCutPage, CPropertyPage)
	//{{AFX_MSG_MAP(CShortCutPage)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShortCutPage message handlers

BOOL CShortCutPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 60;
	lvColumn.iSubItem = 0;
	lvColumn.pszText  = "www";
	m_List.InsertColumn (0,&lvColumn);	//插入一列
	lvColumn.cx = 80;
	lvColumn.pszText  = "allfresh";
	m_List.InsertColumn (1,&lvColumn);	//插入一列
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_GP));
	m_ImageList.Add(hIcon);
	m_List.SetImageList(&m_ImageList,LVSIL_SMALL);

	FillList();

	return TRUE;
}

void CShortCutPage::FillList()
{
	m_List.SetRedraw(FALSE);
	m_List.DeleteAllItems ();
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;

	int nCnt=0;
	map<CString, ShortCut_Cfg, cmpstr >::iterator iter = g_mShortCut.begin();	
	for(; iter!=g_mShortCut.end(); ++iter)
	{
		CString tmpShortCut = (*iter).first;
		ShortCut_Cfg scn = (*iter).second;
		lvItem.iItem = nCnt;
		lvItem.iImage = 0;
		lvItem.pszText = tmpShortCut.GetBuffer(0);
		m_List.InsertItem (&lvItem);
		m_List.SetItemText(nCnt,1,scn.RawCode);
		nCnt++;
	}	
	m_List.SetRedraw(TRUE);
}

#include "SelectGPDlg.h"
#include "InputName.h"
void CShortCutPage::OnAdd() 
{
	CSelectGPDlg dlg;
	dlg.bSingleSelect = TRUE;
	if(dlg.DoModal() == IDOK)
	{
		if(dlg.SelectedGPNum > 0)
		{
			MemStkInfo *tmpInfo = (*g_pStockIO)[dlg.SelectedGPIndex[0]];
			if(tmpInfo)
			{
				CInputName dlg;
				dlg.HintInfo = "请输入您的快捷字串:";
				dlg.Caption = "输入的快捷字串";
				CString tmpStr=tmpInfo->Code;
				dlg.m_NewName = tmpStr.Right(2);
				if(dlg.DoModal()==IDOK)
				{
					TrimString(dlg.m_NewName);
					if(strlen(dlg.m_NewName)>0)
					{
						ShortCut_Cfg scn={0};
						scn.setcode = tmpInfo->setcode;
						strncpy(scn.RawCode,tmpInfo->Code,SH_CODE_LEN);
						g_mShortCut[dlg.m_NewName] = scn;
						//
						FillList();
						m_bChanged=TRUE;
					}
				}
			}
		}
	}
}

void CShortCutPage::OnDel() 
{
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	if(pos != NULL)
	{
		int nItem = m_List.GetNextSelectedItem(pos);
		CString tmpShortCut = m_List.GetItemText(nItem,0);
		map<CString, ShortCut_Cfg, cmpstr >::iterator iter = g_mShortCut.find(tmpShortCut);
		if(iter!=g_mShortCut.end())
		{
			g_mShortCut.erase(iter);
			//
			FillList();
			m_bChanged=TRUE;
		}		
	}
}

void CShortCutPage::OnOK() 
{
	if(m_bChanged)
		SaveShortCutCfg();

	CPropertyPage::OnOK();
}
