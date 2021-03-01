// AllZSDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "AllZSDlg.h"
#include "interface.h"
#include "DescZBDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAllZSDlg dialog


CAllZSDlg::CAllZSDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAllZSDlg::IDD, pParent)
{
	m_lPos = -1;
	m_nSet = 0;
	m_nPeriod = DAY_ST;
	//{{AFX_DATA_INIT(CAllZSDlg)
	//}}AFX_DATA_INIT
}

void CAllZSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAllZSDlg)
	DDX_Control(pDX, IDC_DESCBTN, m_desbt);
	DDX_Control(pDX, IDC_TREE1, m_tree);
	DDX_Control(pDX, IDC_STATICPOS, m_OutStatic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAllZSDlg, CDialog)
	//{{AFX_MSG_MAP(CAllZSDlg)
	ON_BN_CLICKED(IDC_DESCBTN, OnDescbtn)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, OnDblclkTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnSelchangedTree1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAllZSDlg message handlers

BOOL CAllZSDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString tmpStr;
	
	CRect rc;

	m_bIsSpecialLine = FALSE;
	m_OutStatic.GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.DeflateRect(2,2);
	m_Form.Create(NULL, "", WS_CHILD|WS_VISIBLE, rc, this, ID_CONTROL_VIEW , NULL);
	m_Form.Init(rc);
	m_Form.HideSelf();

	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_INDEX));
	m_ImageList.Add(hIcon);
	hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_INDEX2));
	m_ImageList.Add(hIcon);

	m_tree.SetImageList(&m_ImageList,LVSIL_NORMAL);
	switch (m_nSet)
	{
	case 0:
		if(!m_bOverlap)
			tmpStr.Format("%s", "选择指标");
		else
			tmpStr.Format("%s", "选择主图指标");
		break;
	case 2:
		tmpStr.Format("%s", "交易系统指示");
		break;
	case 3:
		tmpStr.Format("%s", "五彩K线指示");
		break;
	}
	SetWindowText(_F(tmpStr));
	FillTree();
	m_desbt.EnableWindow(FALSE);
	return TRUE;
}

void CAllZSDlg::Init(int nSet, int nPeriod, LPCTSTR text, BOOL bIsOverlap)
{
	m_nSet = nSet;
	m_nPeriod = nPeriod;
	m_lpszCurrActive = text;
	m_bOverlap = bIsOverlap;
}

void CAllZSDlg::OnOK() 
{
	HTREEITEM pos=m_tree.GetSelectedItem();
	if(pos==NULL) return;
	if(m_tree.ItemHasChildren(pos)) return;
	DWORD dwPos=m_tree.GetItemData(pos);
	m_bIsSpecialLine = (HIWORD(dwPos)==4);
	m_lPos = LOWORD(dwPos);
	m_Form.GetCurrData(m_curParams);
	CDialog::OnOK();
}

void CAllZSDlg::OnDescbtn() 
{
	long lIndex;

	HTREEITEM pos = m_tree.GetSelectedItem();
	if (pos != NULL && !m_tree.ItemHasChildren(pos))
	{
		DWORD dwPos = m_tree.GetItemData(pos);
		m_bIsSpecialLine = (HIWORD(dwPos)==4);
		lIndex = LOWORD(dwPos);
		if (lIndex >= 0)
		{
			CDescZBDlg dlg;
			dlg.Init(m_nSet, lIndex, m_bIsSpecialLine);
			dlg.DoModal();
		}
	}
}

void CAllZSDlg::FillTree()
{
	int i,j,nTotalNum,nTotalType;
	HTREEITEM hRoot = TVI_ROOT,hChild = TVI_ROOT;
	char tempItemStr[125];
	
	if(m_bOverlap)
	{
		m_tree.SetRedraw(FALSE);
		m_tree.DeleteAllItems(); //首先清空原来的树内容
		if(m_nSet == EXP_TYPE)
			hRoot = m_tree.InsertItem(_F("交易系统指示"),0,0,TVI_ROOT,TVI_LAST);
		else if(m_nSet == KL_TYPE)
			hRoot = m_tree.InsertItem(_F("五彩K线指示"),0,0,TVI_ROOT,TVI_LAST);
		else
			hRoot = m_tree.InsertItem(_F("主图指标"),0,0,TVI_ROOT,TVI_LAST);
		m_tree.SetItemData(hRoot,0);
		
		long lCount = g_pCalc->_CallDTopViewCalc_7(m_nSet);
		TINDEXINFO *pTmpIndex=NULL;
		for (i = 0; i < lCount; i++)
		{
			pTmpIndex = g_pCalc->_CallDTopViewCalc_8(m_nSet, i);
			if(!pTmpIndex) continue;
			//当需要只显示主图叠加的指标时,如果此指标不拥有OnlyMain标志,跳过;否则统统显示
			if (m_nSet == ZB_TYPE && !(pTmpIndex->nDrawMode & OnlyMain)) continue;
			sprintf(tempItemStr,"%s %s",pTmpIndex->acCode,pTmpIndex->acName);
			hChild = m_tree.InsertItem(_F(tempItemStr),1,1,hRoot,TVI_LAST);

			if(m_nSet == ZB_TYPE)
				m_tree.SetItemData(hChild,3*65536+i);
			else if(m_nSet == EXP_TYPE)
				m_tree.SetItemData(hChild,65536+i);
			else if(m_nSet == KL_TYPE)
				m_tree.SetItemData(hChild,2*65536+i);
			else if(m_nSet == UF_TYPE)
				m_tree.SetItemData(hChild,5*65536+i);
		}
		m_tree.Expand(hRoot,TVE_EXPAND);
		m_tree.SetRedraw(TRUE);
		return;
	}
	
	m_tree.SetRedraw(FALSE);
	m_tree.DeleteAllItems(); //首先清空原来的树内容
	if(m_nSet == ZB_TYPE && !m_bOverlap)
	{
		hRoot = m_tree.InsertItem(_F("常用指标"),0,0,TVI_ROOT,TVI_LAST);
		m_tree.SetItemData(hRoot,0);
		TINDEXINFO *pTmpIndex = NULL;
		for(i = 0;i < g_nOftenUseZb;i++)
		{
			pTmpIndex=g_pCalc->_CallDTopViewCalc_8(ZB_TYPE, g_strOftenUseZb[i]);
			if(!pTmpIndex||(pTmpIndex->nDrawMode&OnlyMain)) continue;
			sprintf(tempItemStr,"%s %s",pTmpIndex->acCode,pTmpIndex->acName);
			hChild = m_tree.InsertItem(_F(tempItemStr),1,1,hRoot,TVI_LAST);
			m_tree.SetItemData(hChild,3*65536+pTmpIndex->nIndexNo);
		}
		m_tree.Expand(hRoot,TVE_EXPAND);
	}

	TINDEXINFO *tmpIndexInfo=NULL;
	nTotalNum = g_pCalc->_CallDTopViewCalc_7(ZB_TYPE);
	nTotalType = g_pCalc->_CallDTopViewCalc_10(ZB_TYPE);
	for(i = 0;i < nTotalType;i++)
	{
		TYPEX te={0};
		g_pCalc->_CallDTopViewCalc_12(&te, ZB_TYPE, i);
		hRoot = m_tree.InsertItem(_F(te.lpszName),0,0,TVI_ROOT,TVI_LAST);
		m_tree.SetItemData(hRoot,0);
		for(j = 0;j < nTotalNum;j++)
		{
			tmpIndexInfo = g_pCalc->_CallDTopViewCalc_8(ZB_TYPE,j);
			if(tmpIndexInfo && tmpIndexInfo->nType == te.nTypeID && !(tmpIndexInfo->nDrawMode&OnlyMain))
			{
				sprintf(tempItemStr,"%s %s",tmpIndexInfo->acCode,tmpIndexInfo->acName);
				hChild = m_tree.InsertItem(_F(tempItemStr),1,1,hRoot,TVI_LAST);

				if(m_nSet == ZB_TYPE)
					m_tree.SetItemData(hChild,3*65536+j);
				else if(m_nSet == EXP_TYPE)
					m_tree.SetItemData(hChild,65536+j);
				else if(m_nSet == KL_TYPE)
					m_tree.SetItemData(hChild,2*65536+j);
				else if(m_nSet == UF_TYPE)
					m_tree.SetItemData(hChild,5*65536+j);
			}
		}
	}
	hRoot = m_tree.InsertItem(_F("特殊画线指标"),0,0,TVI_ROOT,TVI_LAST);
	m_tree.SetItemData(hRoot,0);
	for(j=0;j<g_nSpeclIndexNum;j++)
	{
		tmpIndexInfo = g_SpeclIndex+j;
		sprintf(tempItemStr,"%s %s",tmpIndexInfo->acCode,tmpIndexInfo->acName);
		hChild = m_tree.InsertItem(_F(tempItemStr),1,1,hRoot,TVI_LAST);
		m_tree.SetItemData(hChild,4*65536+j);
	}
	nTotalNum = g_pCalc->_CallDTopViewCalc_7(EXP_TYPE);
	m_tree.SetRedraw(TRUE);
}

void CAllZSDlg::OnDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	HTREEITEM pos = m_tree.GetSelectedItem();
	if(pos==NULL) return;
	DWORD dwPos=m_tree.GetItemData(pos);
	if (dwPos>0&&!m_tree.ItemHasChildren(pos))
		OnOK();	
}

void CAllZSDlg::OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;	
	
	long lIndex;
	
	HTREEITEM pos = m_tree.GetSelectedItem();
	if(pos==NULL) return;
	DWORD dwPos=m_tree.GetItemData(pos);
	if (dwPos>0&&!m_tree.ItemHasChildren(pos))
	{
		DWORD dwPos=m_tree.GetItemData(pos);
		m_bIsSpecialLine = (HIWORD(dwPos)==4);
		lIndex = LOWORD(dwPos);
		if (lIndex >= 0)
		{
			//先将所有的edit和spin隐藏,然后在m_Form.SetStatus的处理中做相应调整
			m_Form.HideSelf();
			m_Form.SetStatus(m_nSet, lIndex, m_nPeriod, m_bIsSpecialLine);
		}
		m_desbt.EnableWindow(TRUE);
	}
	else
	{
		m_desbt.EnableWindow(FALSE);
		m_Form.HideSelf();
		m_Form.SetStatus(m_nSet, 0, m_nPeriod, TRUE);
	}
}
