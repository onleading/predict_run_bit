// Other2Page.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "Other2Page.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COther2Page property page

IMPLEMENT_DYNCREATE(COther2Page, CPropertyPage)

COther2Page::COther2Page() : CPropertyPage(COther2Page::IDD)
{
	//{{AFX_DATA_INIT(COther2Page)
	m_nMode = -1;
	m_nNetCondition = g_nNetCondition;
	m_bChangeZB = FALSE;
	//}}AFX_DATA_INIT
}

COther2Page::~COther2Page()
{
}

void COther2Page::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COther2Page)
	DDX_Control(pDX, IDC_SUB_BUTTON, m_SubButtonCtrl);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_AddButtonCtrl);
	DDX_Control(pDX, IDC_ALLZB_LIST, m_AllzbList);
	DDX_Control(pDX, IDC_OFTENUSE_LIST, m_OftenList);
	DDX_Radio(pDX, IDC_MODE1, m_nMode);
	DDX_Radio(pDX, IDC_RADIO1, m_nNetCondition);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COther2Page, CPropertyPage)
	//{{AFX_MSG_MAP(COther2Page)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_UP_BUTTON, OnUpButton)
	ON_BN_CLICKED(IDC_DOWN_BUTTON, OnDownButton)
	ON_BN_CLICKED(IDC_SUB_BUTTON, OnSubButton)
	ON_NOTIFY(NM_DBLCLK, IDC_ALLZB_LIST, OnDblclkAllzbList)
	ON_NOTIFY(NM_DBLCLK, IDC_OFTENUSE_LIST, OnDblclkOftenuseList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COther2Page message handlers

BOOL COther2Page::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_nOftenUseZb=g_nOftenUseZb;
	memcpy(m_strOftenUseZb,g_strOftenUseZb,sizeof(m_strOftenUseZb));

	CString	Ini = g_WSXHStr+g_strUserCfg;
	BOOL bTmpMode = GetPrivateProfileInt("Other","SimpleMode",1,Ini);
	if(bTmpMode)
		m_nMode = 0;
	else
		m_nMode = 1;
	
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 100;
	lvColumn.iSubItem = 0;
	lvColumn.pszText  = "www";
	m_AllzbList.InsertColumn (0,&lvColumn);	//插入一列
	m_OftenList.InsertColumn (1,&lvColumn);	//插入一列
	lvColumn.cx = 100;
	lvColumn.pszText  = "allfresh";
	m_AllzbList.InsertColumn (1,&lvColumn);	//插入一列
	m_AllzbList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_OftenList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_INDEX));
	m_ImageList.Add(hIcon);
	m_AllzbList.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_OftenList.SetImageList(&m_ImageList,LVSIL_SMALL);

	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_STATE|LVIF_IMAGE;
	lvItem.state = 0;
	lvItem.iImage = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	char *pStr,*pStr2;
	int nTotalZbNum = g_pCalc->_CallDTopViewCalc_7(ZB_TYPE);
	for(int i = 0;i < nTotalZbNum;i++)	//填充条件选股指标列表框
	{
		lvItem.iItem = i;
		pStr = (*g_pCalc)[ZB_TYPE][i].acCode;
		lvItem.pszText = pStr;
		m_AllzbList.InsertItem (&lvItem);
		pStr2 = (*g_pCalc)[ZB_TYPE][i].acName;
		m_AllzbList.SetItemText(i,1,_F(pStr2));
		m_AllzbList.SetItemData(i,0);//画线
	}
	m_AllzbList.SetItemState(nTotalZbNum-1,LVIS_FOCUSED,LVIS_FOCUSED);
	m_AllzbList.SetItemState(nTotalZbNum-1,LVIS_SELECTED,LVIS_SELECTED);
	m_AllzbList.EnsureVisible(nTotalZbNum-1,TRUE);
	for(i = 0;i < g_nSpeclIndexNum;i++)
	{
		lvItem.iItem = nTotalZbNum+i;
		pStr = g_SpeclIndex[i].acCode;
		lvItem.pszText = pStr;
		m_AllzbList.InsertItem (&lvItem);
		pStr2 = g_SpeclIndex[i].acName;
		m_AllzbList.SetItemText(nTotalZbNum+i,1,_F(pStr2));
		m_AllzbList.SetItemData(nTotalZbNum+i,i+1); //特殊画线
	}
	for(int j = 0;j < m_nOftenUseZb; j++)	//填充条件选股指标列表框
	{
		lvItem.iItem = j;
		lvItem.pszText = m_strOftenUseZb[j];
		m_OftenList.InsertItem (&lvItem);
	}
	m_OftenList.SetItemState(m_nOftenUseZb-1,LVIS_FOCUSED,LVIS_FOCUSED);
	m_OftenList.SetItemState(m_nOftenUseZb-1,LVIS_SELECTED,LVIS_SELECTED);
	m_OftenList.EnsureVisible(m_nOftenUseZb-1,TRUE);

	GetDlgItem(IDC_NET_STATIC)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_RADIO1)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_RADIO2)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_RADIO3)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_RADIO4)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_NETHINT_STATIC)->ShowWindow(SW_SHOW);

	GetDlgItem(IDC_STYLE_STATIC)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_MODE1)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_MODE3)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STYLEHINT_STATIC)->ShowWindow(SW_SHOW);
	
#if defined(OEM_STAR) || defined(OEM_STAR_HM) || defined(LEVEL2) || defined(PERSONAL) || defined(OEM_NEWJY)
	GetDlgItem(IDC_MODE1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_MODE3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STYLE_STATIC)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STYLEHINT_STATIC)->ShowWindow(SW_HIDE);
#endif
	
	UpdateData(FALSE);

	return TRUE;
}

void COther2Page::OnOK()
{
	UpdateData(TRUE);
	if(m_nNetCondition != g_nNetCondition)
	{
		g_nNetCondition = m_nNetCondition;
		TDX_MessageBox(m_hWnd,"上网环境的设置需要重新进入系统才能生效!",MB_OK|MB_ICONINFORMATION);
	}

	//立即将常见指标存盘
	g_nOftenUseZb=m_nOftenUseZb;
	memcpy(g_strOftenUseZb,m_strOftenUseZb,sizeof(m_strOftenUseZb));
	
	CString	Ini = g_WSXHStr+g_strUserCfg;
	char tempStr[256];
	sprintf(tempStr,"%d",g_nOftenUseZb);
	WritePrivateProfileString("Other","nFavoZb",tempStr,Ini);
	for(int i=0;i<g_nOftenUseZb;i++)
	{
		sprintf(tempStr,"MyFavoZb%02d",i);
		WritePrivateProfileString("Other",tempStr,g_strOftenUseZb[i],Ini);
	}
	BOOL bTmpMode = FALSE;
	if(m_nMode==0)	bTmpMode = TRUE;
	else			bTmpMode = FALSE;
//	WritePrivateProfileString("Other","SimpleMode",bTmpMode?"1":"0",Ini);

	CPropertyPage::OnOK();
}

void COther2Page::OnAddButton() 
{
	POSITION pos = m_AllzbList.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int iNo = m_AllzbList.GetNextSelectedItem(pos);
		int ItemData = m_AllzbList.GetItemData(iNo);
		char *pStr;
		if(ItemData==0) //一般画线
			pStr = (*g_pCalc)[ZB_TYPE][iNo].acCode;
		else			//特殊画线
			pStr = g_SpeclIndex[ItemData-1].acCode;
		if(!pStr) return;
		if(m_nOftenUseZb>=22)
		{
			TDX_MessageBox(m_hWnd,"常用指标数已经达到上限22个,请先删除掉一部分再添加!",MB_ICONERROR|MB_OK);
		}
		else
		{
			BOOL bIn=FALSE;
			for(int i=0;i<m_nOftenUseZb;i++)
			{
				if(m_OftenList.GetItemText(i,0)==pStr)
				{
					bIn=TRUE;
					break;
				}
			}
			if(bIn)
			{
				TDX_MessageBox(m_hWnd,"此指标在常用指标中已经存在!",MB_ICONERROR|MB_OK);
			}
			else
			{
				m_OftenList.InsertItem(m_nOftenUseZb,pStr);
				strcpy(m_strOftenUseZb[m_nOftenUseZb],pStr);
				m_OftenList.SetItemState(m_nOftenUseZb,LVIS_FOCUSED,LVIS_FOCUSED);
				m_OftenList.SetItemState(m_nOftenUseZb,LVIS_SELECTED,LVIS_SELECTED);
				m_OftenList.EnsureVisible(m_nOftenUseZb,FALSE);

				m_nOftenUseZb++;
				m_bChangeZB=TRUE;
			}
		}
	}
}

void COther2Page::OnUpButton() 
{
	POSITION pos2 = m_OftenList.GetFirstSelectedItemPosition();	
	if(pos2!=NULL)
	{
		int iNo2 = m_OftenList.GetNextSelectedItem(pos2);
		CString tmpStr=m_strOftenUseZb[iNo2];
		int upNo=iNo2-1;
		if(upNo>=0)
		{
			strcpy(m_strOftenUseZb[iNo2],m_strOftenUseZb[upNo]);
			strcpy(m_strOftenUseZb[upNo],tmpStr);
			m_OftenList.SetItemText(iNo2,0,m_strOftenUseZb[iNo2]);
			m_OftenList.SetItemText(upNo,0,m_strOftenUseZb[upNo]);

			m_OftenList.SetItemState(upNo,LVIS_FOCUSED,LVIS_FOCUSED);
			m_OftenList.SetItemState(upNo,LVIS_SELECTED,LVIS_SELECTED);
			m_OftenList.EnsureVisible(upNo,FALSE);
			m_bChangeZB=TRUE;
		}
	}
}

void COther2Page::OnDownButton() 
{
	POSITION pos2 = m_OftenList.GetFirstSelectedItemPosition();	
	if(pos2!=NULL)
	{
		int iNo2 = m_OftenList.GetNextSelectedItem(pos2);
		CString tmpStr=m_strOftenUseZb[iNo2];
		int downNo=iNo2+1;
		if(downNo<m_nOftenUseZb)
		{
			strcpy(m_strOftenUseZb[iNo2],m_strOftenUseZb[downNo]);
			strcpy(m_strOftenUseZb[downNo],tmpStr);
			m_OftenList.SetItemText(iNo2,0,m_strOftenUseZb[iNo2]);
			m_OftenList.SetItemText(downNo,0,m_strOftenUseZb[downNo]);

			m_OftenList.SetItemState(downNo,LVIS_FOCUSED,LVIS_FOCUSED);
			m_OftenList.SetItemState(downNo,LVIS_SELECTED,LVIS_SELECTED);
			m_OftenList.EnsureVisible(downNo,FALSE);
			m_bChangeZB=TRUE;
		}
	}	
}

void COther2Page::OnSubButton() 
{
	POSITION pos2 = m_OftenList.GetFirstSelectedItemPosition();
	if (pos2 != NULL)
	{
		int iNo2 = m_OftenList.GetNextSelectedItem(pos2);
		CString str;
		str=m_OftenList.GetItemText(iNo2,0);
		if(m_nOftenUseZb>1)
		{
			if(strcmp(str,m_strOftenUseZb[iNo2])==0)
			{
				for(int i=iNo2;i<m_nOftenUseZb-1;i++)
				{
					strcpy(m_strOftenUseZb[i],m_strOftenUseZb[i+1]);
				}
				m_OftenList.DeleteItem(iNo2);
				m_nOftenUseZb--;
				m_bChangeZB=TRUE;
			}
		}
		else
		{
			TDX_MessageBox(m_hWnd,"常用指标至少要保留一个!",MB_ICONERROR|MB_OK);
		}
	}
}

void COther2Page::OnDblclkAllzbList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnAddButton();	
	*pResult = 0;
}

void COther2Page::OnDblclkOftenuseList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnSubButton();	
	*pResult = 0;
}
