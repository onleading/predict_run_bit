// UrgentListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TdxW.h"
#include "UrgentListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUrgentListDlg dialog
char *l_List_Info[2] = {"发表时间","公告标题"};
int   l_List_Width[2] = {80,270};

CUrgentListDlg::CUrgentListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUrgentListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUrgentListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CUrgentListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUrgentListDlg)
	DDX_Control(pDX, IDC_TITLELIST, m_TitleList);
	DDX_Control(pDX, IDC_INFO_EDIT, m_RichEditEx);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUrgentListDlg, CDialog)
	//{{AFX_MSG_MAP(CUrgentListDlg)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TITLELIST, OnItemchangedTitlelist)
	ON_BN_CLICKED(IDC_GODOWN, OnGodown)
	ON_BN_CLICKED(IDC_GOUP, OnGoup)
	ON_BN_CLICKED(IDC_BUTTON_MORE, OnMoreList)
	ON_BN_CLICKED(IDC_SAVE, OnSaveit)
	ON_NOTIFY(NM_DBLCLK, IDC_TITLELIST, OnDblclkTitlelist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUrgentListDlg message handlers

BOOL CUrgentListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_INDEX));
	m_ImageList.Add(hIcon);
	hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_INDEX2));
	m_ImageList.Add(hIcon);
	m_TitleList.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_TitleList.SetExtendedStyle (LVS_EX_FULLROWSELECT);

	//插入列
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	for(int i = 0; i < 2;i++)
	{
		lvColumn.pszText = l_List_Info[i];
		lvColumn.cx = l_List_Width[i];
		lvColumn.iSubItem = i;
		m_TitleList.InsertColumn (i,&lvColumn);
	}

	FillList();

	unsigned mask = ::SendMessage(m_RichEditEx.m_hWnd, EM_GETEVENTMASK, 0, 0);
	::SendMessage(m_RichEditEx.m_hWnd, EM_SETEVENTMASK, 0, mask | ENM_LINK);
	::SendMessage(m_RichEditEx.m_hWnd, EM_AUTOURLDETECT, true, 0);  //自动检测URL

	//从ini读出相应的配置
	int x,y,nCx,nCy, nCxw, nCyw;
	CRect rect, rcw;
	GetClientRect(&rect);
	nCx=rect.Width();
	nCy=rect.Height();
	GetWindowRect(&rcw);
	nCxw=rcw.Width();
	nCyw=rcw.Height();
	x=ScreenCx-nCx-16;
	y=ScreenCy-nCy-81;
	SetWindowPos(&wndTop,x,y,nCxw,nCyw,SWP_SHOWWINDOW);

	m_bShowContent = FALSE;
	OnMoreList(); 
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUrgentListDlg::FillList(int nSetSel)
{
	m_TitleList.DeleteAllItems();	
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	for(int i=0;i<g_aUrgentInfo.size();i++)
	{
		CTime TimeTag(g_aUrgentInfo[i].TimeTag);
		char strTimeTag[64]={0};
		sprintf(strTimeTag, "%02d:%02d:%02d", TimeTag.GetHour(), TimeTag.GetMinute(), TimeTag.GetSecond());

		lvItem.iItem = i;
		lvItem.iImage = 0;
		lvItem.pszText = strTimeTag;
		m_TitleList.InsertItem(&lvItem);
		m_TitleList.SetItemText(i,1,g_aUrgentInfo[i].Info);
		m_TitleList.SetItemData(i,i);
	}

	if(nSetSel>=0) m_nLastSel = nSetSel;
	else if(nSetSel==-1) m_nLastSel = g_aUrgentInfo.size()-1;

	if(m_nLastSel>=0 && m_nLastSel<g_aUrgentInfo.size())
	{
		m_TitleList.SetItemState(m_nLastSel,LVIS_FOCUSED,LVIS_FOCUSED);
		m_TitleList.SetItemState(m_nLastSel,LVIS_SELECTED,LVIS_SELECTED);
		short nContentPos = g_aUrgentInfo[m_nLastSel].TitleLen+1;
		m_RichEditEx.SetWindowText("");
		m_RichEditEx.SetWindowText(&g_aUrgentInfo[m_nLastSel].Info[nContentPos]);
		m_TitleList.EnsureVisible(m_nLastSel,FALSE);
	}
	else
		m_RichEditEx.SetWindowText("");
}

void CUrgentListDlg::OnItemchangedTitlelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	POSITION pos = m_TitleList.GetFirstSelectedItemPosition ();
	if(pos)
	{
		int nItem = m_TitleList.GetNextSelectedItem (pos);
		int TitleIndex = m_TitleList.GetItemData(nItem);
		if(TitleIndex < 0) return;
		m_nLastSel = TitleIndex;
		short nContentPos = g_aUrgentInfo[m_nLastSel].TitleLen+1;
		m_RichEditEx.SetWindowText("");
		m_RichEditEx.SetWindowText(&g_aUrgentInfo[m_nLastSel].Info[nContentPos]);
		SetWindowText(g_aUrgentInfo[m_nLastSel].Info);
	}
	
	*pResult = 0;
}

void CUrgentListDlg::OnGodown() 
{
	m_nLastSel++;
	m_nLastSel = min(m_nLastSel, g_aUrgentInfo.size()-1);
	m_nLastSel = max(m_nLastSel, 0);
	FillList(m_nLastSel);
}

void CUrgentListDlg::OnGoup() 
{
	m_nLastSel--;
	m_nLastSel = max(m_nLastSel, 0);
	m_nLastSel = min(m_nLastSel, g_aUrgentInfo.size()-1);
	FillList(m_nLastSel);
}

void CUrgentListDlg::OnMoreList()
{
	if(g_aUrgentInfo.empty()) 
	{
		SendMessage(WM_COMMAND, IDCANCEL);
		return;
	}
	
	if(m_bShowContent)
	{
		m_TitleList.ShowWindow(SW_SHOWNORMAL);
		m_RichEditEx.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SAVE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_MORE)->SetWindowText(_F("阅读公告"));
		SetWindowText(_F("金牛公告"));
		m_bShowContent = FALSE;
	}
	else 
	{
		m_TitleList.ShowWindow(SW_HIDE);
		m_RichEditEx.ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_SAVE)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_BUTTON_MORE)->SetWindowText(_F("更多>>"));
		SetWindowText(g_aUrgentInfo[m_nLastSel].Info);
		m_bShowContent = TRUE;
	}
	UpdateData();
}

void CUrgentListDlg::OnSaveit() 
{
	CString tmpFileTitle;
	tmpFileTitle.Format("%s",g_aUrgentInfo[m_nLastSel].Info);
	CString tmpFileName;
	CFileDialog FileDialog(FALSE,NULL,tmpFileTitle+".txt",
							OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXTENSIONDIFFERENT,
							_F("文本文件(*.txt)|*.txt||"));
	if (FileDialog.DoModal() == IDOK)
	{		
		tmpFileName=FileDialog.GetPathName();
		if(tmpFileName.Find(".",0)<0)
			tmpFileName+=".txt";
		CFile theFile;
		if(theFile.Open(tmpFileName,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone))
		{
			short nContentPos = g_aUrgentInfo[m_nLastSel].TitleLen+1;
			theFile.Write(&g_aUrgentInfo[m_nLastSel].Info[nContentPos],strlen(&g_aUrgentInfo[m_nLastSel].Info[nContentPos]));
			theFile.Close();
		}
		else
			TDX_MessageBox(m_hWnd,"存盘文件失败!",MB_OK|MB_ICONERROR);
	}
}

void CUrgentListDlg::OnOK() //不允许用户按OK
{

}

void CUrgentListDlg::OnCancel() 
{
	GetParent() ->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);
}

void CUrgentListDlg::OnDblclkTitlelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_TitleList.GetFirstSelectedItemPosition ();
	if(pos)
	{
		int nItem = m_TitleList.GetNextSelectedItem (pos);
		int TitleIndex = m_TitleList.GetItemData(nItem);
		if(TitleIndex < 0) return;
		m_nLastSel = TitleIndex;
		short nContentPos = g_aUrgentInfo[m_nLastSel].TitleLen+1;
		m_RichEditEx.SetWindowText("");
		m_RichEditEx.SetWindowText(&g_aUrgentInfo[m_nLastSel].Info[nContentPos]);
	}
	OnMoreList();
	
	*pResult = 0;
}
