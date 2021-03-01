// indexsortdlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "indexsortdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIndexSortDlg dialog

CIndexSortDlg::CIndexSortDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIndexSortDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIndexSortDlg)
	//}}AFX_DATA_INIT
	m_pIndex = NULL;
	m_parPos = NULL;
}

CIndexSortDlg::~CIndexSortDlg()
{
	TDEL(m_parPos);
}

void CIndexSortDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIndexSortDlg)
	DDX_Control(pDX, IDC_STATICINFO, m_OutStatic);
	DDX_Control(pDX, IDC_OUTLINE_LISTBOX, m_OutLineListBox);
	DDX_Control(pDX, IDC_INDEX_LIST, m_IndexList);
	DDX_Control(pDX, IDC_STATUS, m_CStatus);
	DDX_Control(pDX, IDC_CARRY_PROGRESS, m_Carry_Progress);
	DDX_Control(pDX, IDC_CALCCYCLE_COMBO, m_CalcCycle_Combo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIndexSortDlg, CDialog)
	//{{AFX_MSG_MAP(CIndexSortDlg)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_INDEX_LIST, OnItemchangedIndexList)
	ON_CBN_SELCHANGE(IDC_CALCCYCLE_COMBO, OnSelchangeCalccycleCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIndexSortDlg message handlers

BOOL CIndexSortDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_CStatus.SetTextColor(RGB(0,0,255));
	int i, nPos = 0, nCount;
	TINDEXINFO *pTmp;

	CRect rc;

	m_OutStatic.GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.DeflateRect(2,2);
	m_Form.Create(NULL, "", WS_CHILD|WS_VISIBLE, rc, this, ID_CONTROL_VIEW , NULL);
	m_Form.Init(rc);
	m_Form.HideSelf();

	for(i = 0; g_CalcPeriodStr[i] != NULL ;i++)
		m_CalcCycle_Combo.AddString (_F(g_CalcPeriodStr[i]));
	m_CalcCycle_Combo.SetCurSel(DAY_ST);	//默认是日线
	m_Form.SetStatus(ZB_TYPE, 0, DAY_ST);

	//为列表设置图标
	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_INDEX));
	m_ImageList.Add(hIcon);
	m_IndexList.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_IndexList.SetExtendedStyle (LVS_EX_FULLROWSELECT);

	m_pIndex = g_pCalc->_CallDTopViewCalc_8(ZB_TYPE, 0L);
	if (m_pIndex == NULL) return FALSE;
	pTmp = m_pIndex;
	//填充指标列表
	char *ColStr[2] = {"指标名称","中文名称"}; //先构造栏目
	int ColWidth[2] = {80,90};
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	for(i = 0; i < 2;i++)
	{
		lvColumn.pszText = _F(ColStr[i]);
		lvColumn.cx = ColWidth[i];
		lvColumn.iSubItem = i;
		m_IndexList.InsertColumn (i,&lvColumn);
	}
	LV_ITEM lvItem; //再填充列表
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	CString tempStr;
	nCount = g_pCalc->_CallDTopViewCalc_7(ZB_TYPE);
	m_parPos = new int[nCount];
	memset(m_parPos, 0, nCount);
	for(i = 0;i < nCount;i++)
	{
		m_parPos[nPos] = i;
		lvItem.iItem = nPos;
		lvItem.iImage = 0;
		tempStr.Format("%s",pTmp->acCode);
		lvItem.pszText = tempStr.GetBuffer(0);
		m_IndexList.InsertItem (&lvItem);
		m_IndexList.SetItemText (nPos,1,_F(pTmp->acName));
		nPos++;
		pTmp++;
	}

	if (m_IndexList.GetItemCount() > 0)
	{
		m_IndexList.SetItemState(0,LVIS_FOCUSED,LVIS_FOCUSED);
		m_IndexList.SetItemState(0,LVIS_SELECTED,LVIS_SELECTED);
	}

	SetDlgItemText(IDOK,_F("确定"));
	GetDlgItem(IDC_CARRY_PROGRESS)->ShowWindow(SW_HIDE);
	m_OutLineListBox.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_OUTLINE_STATIC)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATUS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_PERIOD_STATIC)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_CALCCYCLE_COMBO)->ShowWindow(SW_HIDE);

	m_IndexList.GetWindowRect(&rc);
	ScreenToClient(&rc);
	//向下变长102象素
	m_IndexList.MoveWindow(rc.left,rc.top,rc.Width(),rc.Height()+102);

	m_OutLineListBox.SetItemHeight (0,16);

	return TRUE;
}

void CIndexSortDlg::FillOutLine(long index)
{
	m_OutLineListBox.ResetContent();
	for(int i = 0;i < m_pIndex->nLineNum;i++)
		m_OutLineListBox.AddString(m_pIndex->aLine[i].acLineName);
	m_OutLineListBox.SetCurSel(0);
}

//处理IndexList的选中事件
void CIndexSortDlg::OnItemchangedIndexList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	POSITION pos;

	pos = m_IndexList.GetFirstSelectedItemPosition();
	if(pos != NULL)
	{
		m_lCurrentIndex = m_IndexList.GetNextSelectedItem(pos);
		if (m_lCurrentIndex >= 0)
		{
			m_pIndex = g_pCalc->_CallDTopViewCalc_8(0, m_lCurrentIndex);
			if (m_pIndex == NULL)
				GetDlgItem(IDOK)->EnableWindow(FALSE);
			else
			{
				FillOutLine(m_lCurrentIndex);
				//先将所有的edit和spin隐藏,然后在m_Form.SetStatus的处理中做相应调整
				m_Form.HideSelf();
				m_Form.SetStatus(ZB_TYPE, m_lCurrentIndex, m_CalcCycle_Combo.GetCurSel());
			}
		}
	}
	*pResult = 0;
}

void CIndexSortDlg::OnCancel()
{
	m_Form.SendMessage(UM_RESTORE, 0, 0);
	CDialog::OnCancel();
}

extern UINT	Edit_ID[];
extern UINT	SPIN_ID[];

void CIndexSortDlg::OnOK()
{
	UpdateData(TRUE);
	CString tempStr;
	int CurCycle = m_CalcCycle_Combo.GetCurSel();

	if (m_lCurrentIndex < 0)
		m_lCurrentIndex = 0;
	IndexSortInfo[CurIndexSort].Index = m_parPos[m_lCurrentIndex];
	IndexSortInfo[CurIndexSort].Cycle = CalcPeriodToTdxPeriod(CurCycle);

	for(int i=0;Edit_ID[i]!=NULL;i++)
	{
		if(m_pIndex->nParaNum<=i) break;
		m_Form.GetDlgItem(Edit_ID[i])->GetWindowText(tempStr);
		IndexSortInfo[CurIndexSort].Param[i] = atoi(tempStr);
		m_pIndex->aPara[i].nValue[DAY_ST] = IndexSortInfo[CurIndexSort].Param[i];
	}

	CDialog::OnOK();
	return;
}

void CIndexSortDlg::OnSelchangeCalccycleCombo() 
{
	int nSel = m_CalcCycle_Combo.GetCurSel();
	m_Form.HideSelf();
	m_Form.SetStatus(ZB_TYPE, m_lCurrentIndex, nSel);	
}
