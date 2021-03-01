// PadListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "PadListDlg.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPadListDlg dialog


CPadListDlg::CPadListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPadListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPadListDlg)
	//}}AFX_DATA_INIT
	m_nLastSel		= -1;
	m_bModified		= FALSE;
	m_bSwitchShow	= FALSE;
}


void CPadListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPadListDlg)
	DDX_Control(pDX, IDC_PADLIST, m_PadList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPadListDlg, CDialog)
	//{{AFX_MSG_MAP(CPadListDlg)
	ON_BN_CLICKED(IDC_IMPORTPAD, OnImportpad)
	ON_BN_CLICKED(IDC_EXPORTPAD, OnExportpad)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_NOTIFY(NM_DBLCLK, IDC_PADLIST, OnDblclkPadList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PADLIST, OnItemchangedPadlist)
	ON_BN_CLICKED(IDC_SHOWSHORTCUT, OnShowshortcut)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPadListDlg message handlers

char *l_PadList_Info[4] = {"������","��������","��ݼ�","�л���"};
int   l_PadList_Width[4] = {60,95,48,60};

BOOL CPadListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_PAD));	
	m_ImageList.Add(hIcon);
	hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_PAD2));
	m_ImageList.Add(hIcon);
	m_PadList.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_PadList.SetExtendedStyle (LVS_EX_FULLROWSELECT);
	//������
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	for(int i = 0; i < 4;i++)
	{
		lvColumn.pszText = l_PadList_Info[i];
		lvColumn.cx = l_PadList_Width[i];
		lvColumn.iSubItem = i;
		m_PadList.InsertColumn (i,&lvColumn);
	}

	FillList();

	return TRUE;
}

void CPadListDlg::FillList()
{
	GetDynaCusPadInfo();

	m_PadList.DeleteAllItems();	
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	for(int i=0;i<g_vDynaCmdTab.size();i++)
	{
		lvItem.iItem = i;
		lvItem.iImage = 0;
		lvItem.pszText = g_vDynaCmdTab[i].EnName;
		m_PadList.InsertItem(&lvItem);
		m_PadList.SetItemText(i,1,g_vDynaCmdTab[i].ChName);
		m_PadList.SetItemText(i,2,g_vDynaCmdTab[i].CmdInfo.Code);
		if(g_vDynaCmdTab[i].ShowShortCut)
			m_PadList.SetItemText(i,3,_F("��ʾ"));
		else 
			m_PadList.SetItemText(i,3,_F("  ����"));
		m_PadList.SetItemData(i,i);
	}
	if(m_nLastSel>=0 && m_nLastSel<g_vDynaCmdTab.size())
	{
		m_PadList.SetItemState(m_nLastSel,LVIS_FOCUSED,LVIS_FOCUSED);
		m_PadList.SetItemState(m_nLastSel,LVIS_SELECTED,LVIS_SELECTED);
		m_PadList.EnsureVisible(m_nLastSel,FALSE);
	}
}

void CPadListDlg::OnOK() 
{
	POSITION pos = m_PadList.GetFirstSelectedItemPosition ();
	if(pos)
	{
		int nItem = m_PadList.GetNextSelectedItem (pos);
		int PadIndex = m_PadList.GetItemData(nItem);
		if(PadIndex < 0) return;
		((CMainFrame*)AfxGetMainWnd())->PostMessage(WM_COMMAND,IDC_CUSTOMPAD1+PadIndex);
		CDialog::OnOK();
	}
}

void CPadListDlg::OnImportpad() 
{
	CFileDialog dlg(TRUE,"*.sp", NULL, NULL, _F("���涨���ļ�(*.sp)|*.sp||"), this);	
	if (dlg.DoModal() == IDOK)
	{
		CString strFilePath = dlg.GetPathName();
		CString tmpPadIni = g_WSXHStr+(CString)"sheet\\"+dlg.GetFileName();
		if(CopyFile(strFilePath, tmpPadIni, FALSE))
		{
			FillList();
			m_bModified = TRUE;
		}
	}
}

void CPadListDlg::OnExportpad() 
{
	POSITION pos = m_PadList.GetFirstSelectedItemPosition ();
	if(pos)
	{
		int nItem = m_PadList.GetNextSelectedItem (pos);
		int PadIndex = m_PadList.GetItemData(nItem);
		if(PadIndex < 0) return;
		m_nLastSel = PadIndex;
		CString tmpPadCode = g_vDynaCmdTab[PadIndex].EnName;
		BROWSEINFO BrInfo ;		
		ZeroMemory( &BrInfo, sizeof(BrInfo)) ;
		BrInfo.hwndOwner = this->m_hWnd ;
		BrInfo.lpszTitle = "ѡ���ư��浼��Ŀ¼";
		BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;
		//����ļ���
		LPITEMIDLIST ppidlDestination= SHBrowseForFolder(&BrInfo);
		//�û�ѡ����ȡ����ť
		if(NULL == ppidlDestination)
			return;		
		char szPath[ MAX_PATH] ;
		SHGetPathFromIDList( ppidlDestination, szPath);
		CString destFile = (CString)(LPCSTR)szPath + "\\" + tmpPadCode + ".sp";
		CString tmpPadIni = g_WSXHStr + "sheet\\"+tmpPadCode + ".sp";
		CopyFile(tmpPadIni, destFile, FALSE);		
	}
}

void CPadListDlg::OnDelete() 
{
	POSITION pos = m_PadList.GetFirstSelectedItemPosition ();
	if(pos)
	{
		int nItem = m_PadList.GetNextSelectedItem (pos);
		int PadIndex = m_PadList.GetItemData(nItem);
		if(PadIndex < 0) return;
		m_nLastSel = PadIndex;
		CString tmpStr,tmpPadCode=g_vDynaCmdTab[PadIndex].EnName,tmpPadName=g_vDynaCmdTab[PadIndex].ChName;
		tmpStr.Format("��ȷ��ɾ��\'%s\'������?",tmpPadName);
		if(TDX_MessageBox (m_hWnd,tmpStr,MB_ICONQUESTION|MB_OKCANCEL)==IDOK)
		{
			//��ʼɾ��
			CString padini; CFile ini;
			padini = g_WSXHStr+"sheet\\"+tmpPadCode+".sp";
			vector<DynaCmdTable> newDynaTab; 
			newDynaTab.clear();
			for(int i=0;i<g_vDynaCmdTab.size();i++)
			{
				if(strcmp(tmpPadName, g_vDynaCmdTab[i].ChName)==0) continue;
				newDynaTab.push_back(g_vDynaCmdTab[i]);
			}
			newDynaTab.swap(g_vDynaCmdTab);
			newDynaTab.clear();
			if(ini.Open(padini, CFile::modeReadWrite|CFile::shareDenyNone))
			{
				ini.Close();
				CFile::Remove( padini );
			}
			FillList();
			m_bModified = TRUE;
		}
	}
}

void CPadListDlg::OnDblclkPadList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnOK();
	*pResult = 0;
}

void CPadListDlg::OnItemchangedPadlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	POSITION pos = m_PadList.GetFirstSelectedItemPosition ();
	if(pos)
	{
		int nItem = m_PadList.GetNextSelectedItem (pos);
		int PadIndex = m_PadList.GetItemData(nItem);
		if(PadIndex < 0) return;
		if(g_vDynaCmdTab[PadIndex].ShowShortCut)
			GetDlgItem(IDC_SHOWSHORTCUT)->SetWindowText(_F("���ؿ��"));
		else 
			GetDlgItem(IDC_SHOWSHORTCUT)->SetWindowText(_F("��ʾ���"));
	}
	
	*pResult = 0;
}

void CPadListDlg::OnShowshortcut() 
{
	POSITION pos = m_PadList.GetFirstSelectedItemPosition ();
	if(pos)
	{
		int nItem = m_PadList.GetNextSelectedItem (pos);
		int PadIndex = m_PadList.GetItemData(nItem);
		if(PadIndex < 0) return;
		m_bSwitchShow = TRUE;
		if(g_vDynaCmdTab[PadIndex].ShowShortCut)
		{
			g_vDynaCmdTab[PadIndex].ShowShortCut = !g_vDynaCmdTab[PadIndex].ShowShortCut;
			//���ؿ��
			CString tmpPadIni;
			tmpPadIni.Format("%ssheet\\%s.sp",g_WSXHStr,g_vDynaCmdTab[PadIndex].EnName);
			WritePrivateProfileString("DEAFULTGP", "ShowShortCut", "0", tmpPadIni);
			//ˢ��list
			m_PadList.SetItemText(PadIndex,3,_F("  ����"));
			GetDlgItem(IDC_SHOWSHORTCUT)->SetWindowText(_F("��ʾ���"));
		}
		else
		{
			g_vDynaCmdTab[PadIndex].ShowShortCut = !g_vDynaCmdTab[PadIndex].ShowShortCut;
			//���ؿ��
			CString tmpPadIni;
			tmpPadIni.Format("%ssheet\\%s.sp",g_WSXHStr,g_vDynaCmdTab[PadIndex].EnName);
			WritePrivateProfileString("DEAFULTGP", "ShowShortCut", "1", tmpPadIni);
			//ˢ��list
			m_PadList.SetItemText(PadIndex,3,_F("��ʾ"));
			GetDlgItem(IDC_SHOWSHORTCUT)->SetWindowText(_F("���ؿ��"));
		}
	}
}
