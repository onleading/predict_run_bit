// warninfopage.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "warninfopage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWarnInfoPage property page

IMPLEMENT_DYNCREATE(CWarnInfoPage, CPropertyPage)

CWarnInfoPage::CWarnInfoPage() : CPropertyPage(CWarnInfoPage::IDD)
{
	//{{AFX_DATA_INIT(CWarnInfoPage)
	m_nWarnFlag = 2;
	//}}AFX_DATA_INIT
	m_TempWg.clear();
}

CWarnInfoPage::~CWarnInfoPage()
{
}

void CWarnInfoPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWarnInfoPage)
	DDX_Control(pDX, IDC_GPLIST, m_GP_List);
	DDX_Radio(pDX, IDC_SC1, m_nWarnFlag);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWarnInfoPage, CPropertyPage)
	//{{AFX_MSG_MAP(CWarnInfoPage)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_BN_CLICKED(IDC_SC1, OnSc1)
	ON_BN_CLICKED(IDC_SC2, OnSc2)
	ON_BN_CLICKED(IDC_SC3, OnSc3)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
void CWarnInfoPage::FillGpList()
{
	int nGpNum = 0;
	m_GP_List.SetRedraw(FALSE);
	m_GP_List.DeleteAllItems();
	for(int i=0;i<m_TempWg.size();i++)
	{
		MemStkInfo *pStkInfo = g_pStockIO->Get_MemStkInfo(m_TempWg[i].code, m_TempWg[i].setcode);
		if(!pStkInfo) continue;

		m_GP_List.InsertItem(nGpNum,pStkInfo->Name,0);
		m_GP_List.SetItemData(nGpNum,(DWORD)pStkInfo);
		++nGpNum;
	}
	m_GP_List.SetRedraw(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
// CWarnInfoPage message handlers

BOOL CWarnInfoPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_nWarnFlag = g_WarnParam2.nWarnFlag;

	GetDlgItem(IDC_CONTENT_EDIT)->SetWindowText(_F("市场雷达优先级:\r\n\r\n 1.连续大量\r\n 2.停板预警\r\n 3.涨幅突破(跌幅跌破)\r\n 4.量比突破\r\n 5.巨额成交\r\n 6.大量成交\r\n 7.单笔异涨(单笔异跌)\r\n 8.低开长阳(高开长阴)\r\n 9.出现大买挂盘(出现大卖挂盘)\r\n10.创今日新高(创今日新低)\r\n"));

	for(int i=0;i<g_aSelfWarnGp.size();i++)
		m_TempWg.push_back(g_aSelfWarnGp[i]);
	
	m_ImageList2.Create(16,16,ILC_COLOR8|ILC_MASK,1,0);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_GP));
	m_ImageList2.Add(hIcon);
	m_GP_List.SetImageList(&m_ImageList2,LVSIL_SMALL);

	FillGpList();
	UpdateData(FALSE);

	if(m_nWarnFlag!=2)
	{
		GetDlgItem(IDC_GPLIST)->EnableWindow(FALSE);
		GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
		GetDlgItem(IDC_DEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_RESET)->EnableWindow(FALSE);
	}

	return TRUE;
}

void CWarnInfoPage::OnOK() 
{
	UpdateData(TRUE);
	g_WarnParam2.nWarnFlag = m_nWarnFlag;
	g_aSelfWarnGp.clear();
	for(int i=0;i<m_TempWg.size();i++)
	{
		MemStkInfo *pStkInfo = g_pStockIO->Get_MemStkInfo(m_TempWg[i].code,m_TempWg[i].setcode);
		if(pStkInfo)
		{
			PWARNINGCND wg={0};
			wg.setcode = pStkInfo->setcode;
			strcpy(wg.code, pStkInfo->Code);
			g_aSelfWarnGp.push_back(wg);
		}
	}
	
	CPropertyPage::OnOK();
}

#include "SelectGPDlg.h"
void CWarnInfoPage::OnAdd() 
{
	UpdateData(TRUE);
	CSelectGPDlg dlg;
	dlg.bSingleSelect = FALSE;

	int i, j;
	if(dlg.DoModal() == IDOK)
	{
		if(dlg.SelectedGPNum > 0)
		{
			int nLastNum = m_TempWg.size();
			MemStkInfo *pMemStkInfo=NULL;
			for(i = 0;i < dlg.SelectedGPNum;i++)
			{
				pMemStkInfo = (*g_pStockIO)[dlg.SelectedGPIndex[i]];
				if(!pMemStkInfo) continue;

				BOOL bFind = FALSE;
				for(j=0;j<nLastNum;j++)
				{
					MemStkInfo *pStkInfo = g_pStockIO->Get_MemStkInfo(m_TempWg[j].code,m_TempWg[j].setcode);
					if(!pStkInfo) continue;
					if(pStkInfo==pMemStkInfo)
					{
						bFind = TRUE;
						break;
					}
				}
				if(!bFind)
				{
				//	m_GP_List.InsertItem(m_GP_List.GetItemCount(),pMemStkInfo->Name,0);
				//	m_GP_List.SetItemData(m_GP_List.GetItemCount(),(DWORD)pMemStkInfo);

					PWARNINGCND wg={0};
					wg.setcode = pMemStkInfo->setcode;
					strcpy(wg.code, pMemStkInfo->Code);
					m_TempWg.push_back(wg);
				}
			}
			FillGpList();
		}
	}
	UpdateData(FALSE);
}

void CWarnInfoPage::OnDel() 
{
	UpdateData(TRUE);

	POSITION pos = m_GP_List.GetFirstSelectedItemPosition();
	if( pos != NULL )
	{
		int nItem;
		while (pos)
		{
			nItem = m_GP_List.GetNextSelectedItem(pos);
			MemStkInfo *pMemStkInfo = (MemStkInfo*)m_GP_List.GetItemData(nItem);
			if(pMemStkInfo)
			{
				for(int i=0;i<m_TempWg.size();i++)
				{
					if(m_TempWg[i].setcode==pMemStkInfo->setcode&&strcmp(m_TempWg[i].code, pMemStkInfo->Code)==0)
					{
						m_TempWg[i].setcode = -1;
						m_TempWg[i].code[0] = 0; 
					}
				}
			}
			m_GP_List.DeleteItem(nItem);
			pos = m_GP_List.GetFirstSelectedItemPosition();
		}
	}
	
	UpdateData(FALSE);
}

void CWarnInfoPage::OnSc1() 
{
	GetDlgItem(IDC_GPLIST)->EnableWindow(FALSE);
	GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
	GetDlgItem(IDC_DEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_RESET)->EnableWindow(FALSE);
}

void CWarnInfoPage::OnSc2() 
{
	GetDlgItem(IDC_GPLIST)->EnableWindow(FALSE);
	GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
	GetDlgItem(IDC_DEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_RESET)->EnableWindow(FALSE);
}

void CWarnInfoPage::OnSc3() 
{
	GetDlgItem(IDC_GPLIST)->EnableWindow(TRUE);
	GetDlgItem(IDC_ADD)->EnableWindow(TRUE);
	GetDlgItem(IDC_DEL)->EnableWindow(TRUE);
	GetDlgItem(IDC_RESET)->EnableWindow(TRUE);
}

void CWarnInfoPage::OnReset() 
{
	m_TempWg.clear();

	FillGpList();
}
