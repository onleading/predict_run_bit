// GetBlockDlg.cpp : implementation file

#include "stdafx.h"
#include "tdxw.h"
#include "MainFrm.h"
#include "GetBlockDlg.h"
#include "EditBlockDlg.h"
#include "GridView.h"
#include "ComView2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetBlockDlg dialog

static int s_lastselblock=0;
CGetBlockDlg::CGetBlockDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGetBlockDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGetBlockDlg)
	//}}AFX_DATA_INIT
	bBlockModify = FALSE;
	m_bGetBlock = TRUE;

	m_nStkNum = 0;
}


void CGetBlockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetBlockDlg)
	DDX_Control(pDX, IDC_BLOCKLIST, m_BlockList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetBlockDlg, CDialog)
	//{{AFX_MSG_MAP(CGetBlockDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_BLOCKLIST, OnDblclkBlocklist)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_BLOCKLIST, OnItemchangedBlocklist)
	ON_BN_CLICKED(IDC_NEWBLOCK, OnNewblock)
	ON_BN_CLICKED(IDC_DELETEBLOCK, OnDeleteblock)
	ON_BN_CLICKED(IDC_RENAMEBLOCK, OnRenameblock)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetBlockDlg message handlers

BOOL CGetBlockDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,0);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_BLOCK));
	m_ImageList.Add(hIcon);
	m_BlockList.SetImageList(&m_ImageList,LVSIL_SMALL);
	FillBlock();
	if(!m_bGetBlock)
	{
		SetWindowText(_F("���뵽��ѡ��/���"));
		if(s_lastselblock<0 || s_lastselblock>=BlockNum)
			s_lastselblock = 0;
		m_BlockList.SetItemState(s_lastselblock,LVIS_FOCUSED,LVIS_FOCUSED);
		m_BlockList.SetItemState(s_lastselblock,LVIS_SELECTED,LVIS_SELECTED);
	}

	return TRUE;
}

void CGetBlockDlg::OnDblclkBlocklist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int i = m_BlockList.GetNextItem(-1,LVNI_SELECTED);
	if(i != -1)
		OnOK();
}

void CGetBlockDlg::OnItemchangedBlocklist(NMHDR* pNMHDR, LRESULT* pResult)
{
	int i = m_BlockList.GetNextItem(-1,LVNI_SELECTED);
	if( i != -1 )
	{
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		if(i == 0 || i == 1)
		{
			GetDlgItem(IDC_DELETEBLOCK)->EnableWindow(FALSE);
			GetDlgItem(IDC_RENAMEBLOCK)->EnableWindow(FALSE);
		}
		else
		{
			GetDlgItem(IDC_DELETEBLOCK)->EnableWindow(TRUE);
			GetDlgItem(IDC_RENAMEBLOCK)->EnableWindow(TRUE);
		}
	}
	else			
		GetDlgItem(IDOK)->EnableWindow(FALSE);
}

extern void SaveBlockCfg();
void CGetBlockDlg::OnOK()
{	
	if(m_bGetBlock) //���õ��ڼ��Ű��
	{
		int i = m_BlockList.GetNextItem(-1,LVNI_SELECTED);
		if( i == -1 )
			nSelBlock = -3;
		nSelBlock = i-2; //���Ϊ-2��ʾѡ����ѡ�ɣ�-1��ʾѡ�й�Ʊ��
	}
	else			//����֮
	{
		int i = m_BlockList.GetNextItem(-1,LVNI_SELECTED);
		if( i != -1 )
		{
			if(m_nStkNum==0) //����ǵ�������
				AddGPToBlock(i,m_CurGPCode,m_CurSetCode);
			else
			{
				MemStkInfo *tmpInfo;
				for(int k=0;k < m_nStkNum;k++)
				{
					tmpInfo = (*g_pStockIO)[m_nStkIndex[k]];
					if(tmpInfo)
						AddGPToBlock(i,tmpInfo->Code,tmpInfo->setcode);
				}
			}
			s_lastselblock = i; //��¼�ϴ�ѡ�еİ��λ��
		}
	}
	if(bBlockModify)
	{
		//����������ļ�����
		SaveBlockCfg();
		((CMainFrame *)AfxGetMainWnd())->FillGPTree();
		RefreshCusGridTab(); //����ˢ���Զ����ǩ
		for(int i = 0;i<theViewManager.GetWindowNum();i++)
		{
			CView *tempView = (CView *)theViewManager.arViews[i];
			if( tempView->IsKindOf(RUNTIME_CLASS(CGridView)) )
				((CGridView *)tempView)->SendMessage(UM_RESETGRIDTAB);
			else if(tempView->IsKindOf(RUNTIME_CLASS(CComView2)))
				((CComView2 *)tempView)->SendMessage(UM_RESETGRIDTAB);
		}
	}
	CDialog::OnOK();
}

void CGetBlockDlg::OnNewblock() 
{
	if(BlockNum == USERBLOCK_NUM+2)
	{
		TDX_MessageBox(m_hWnd,"����̫�࣬�������½���",MB_OK);
		return;
	}
	CEditBlockDlg	dlg;
	dlg.m_BlockName  = "";
	dlg.m_KeyGuyCode = "";
	dlg.IsAdd = TRUE;
	if( dlg.DoModal() == IDOK )
	{
		TrimString(dlg.m_BlockName);
		TrimString(dlg.m_KeyGuyCode);
		for(int i = 0;i < BlockNum;i++)
		{
			if(BlockGPName[i] == dlg.m_BlockName)
			{
				TDX_MessageBox(m_hWnd,"����ͬ���İ��,�����½���",MB_OK|MB_ICONEXCLAMATION);
				return;
			}
			if(i >= 2 && strcmp(g_BlockInfo[i-2].KeyGuyCode,dlg.m_KeyGuyCode)==0)
			{
				TDX_MessageBox(m_hWnd,"����ͬ���İ����,�����½�!����������!",MB_OK|MB_ICONEXCLAMATION);
				return;
			}
		}
		//�������ļ�
		CString tempEmptyFile =(CString)g_strBlockNew+dlg.m_KeyGuyCode+(CString)".blk";
		CStdioFile m_EmptyFile;
		if( m_EmptyFile.Open(HomePath+tempEmptyFile,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone) == NULL)
		{
			TDX_MessageBox(m_hWnd,"���ܴ�������ļ�",MB_OK|MB_ICONEXCLAMATION);
			return;
		}
		else
			m_EmptyFile.Close();

		g_BlockInfo[g_nBlockNum].tag = -1;
		strcpy(g_BlockInfo[g_nBlockNum].Name,dlg.m_BlockName);
		strcpy(g_BlockInfo[g_nBlockNum].KeyGuyCode,dlg.m_KeyGuyCode);
		g_nBlockNum++;
		//��ǰ�Ի����и��£�ͬʱѡ�д���
		m_BlockList.InsertItem(BlockNum,dlg.m_BlockName,0);
		m_BlockList.SetItemState(BlockNum,LVIS_FOCUSED,LVIS_FOCUSED);
		m_BlockList.SetItemState(BlockNum,LVIS_SELECTED,LVIS_SELECTED);
		BlockGPName[BlockNum] = dlg.m_BlockName ;
		BlockNum++;

		bBlockModify = TRUE;
	}
}

void CGetBlockDlg::OnDeleteblock() 
{
	POSITION pos = m_BlockList.GetFirstSelectedItemPosition();
	if(pos == NULL) return;
	int BlockIndex = m_BlockList.GetNextSelectedItem(pos);
	CString theBlockName,theBlockFileName;
	theBlockName = GetBlockFileName(BlockIndex);
	int bRet = TDX_MessageBox(m_hWnd,"��ȷ��ɾ�����\""+theBlockName+"\"��",MB_ICONQUESTION|MB_OKCANCEL);
	if(bRet == IDOK)
	{
		theBlockFileName = (CString)g_strBlockNew+theBlockName+(CString)".blk";
		CFile tempFile;	//�������жϸ��ļ��Ƿ����,��������ڣ�����
		if(tempFile.Open(HomePath+theBlockFileName,CFile::modeRead|CFile::shareDenyNone))
		{
			tempFile.Close();
			try
			{
				CFile::Remove(HomePath+theBlockFileName);
			}
			catch(...)
			{
				TDX_MessageBox(m_hWnd,"���ɾ������!",MB_OK|MB_ICONEXCLAMATION);
				return;
			}
		}
		for(int i = BlockIndex-2;i < g_nBlockNum-1;i++)
			g_BlockInfo[i] = g_BlockInfo[i+1];
		g_nBlockNum--;
		FillBlock();

		bBlockModify = TRUE;
	}
}

void CGetBlockDlg::OnRenameblock() 
{
	POSITION pos = m_BlockList.GetFirstSelectedItemPosition();
	if(pos == NULL) return;
	int BlockIndex = m_BlockList.GetNextSelectedItem(pos);

	CEditBlockDlg dlg;
	dlg.m_BlockName  = g_BlockInfo[BlockIndex-2].Name;
	CString OldKeyGuyCode = g_BlockInfo[BlockIndex-2].KeyGuyCode;
	dlg.m_KeyGuyCode = OldKeyGuyCode;
	dlg.IsAdd = FALSE;
	if( dlg.DoModal() == IDOK )
	{
		TrimString(dlg.m_BlockName);
		TrimString(dlg.m_KeyGuyCode);
		for(int i = 0;i < BlockNum;i++)
		{
			if(i == BlockIndex) continue;
			if(BlockGPName[i] == dlg.m_BlockName)
			{
				TDX_MessageBox(m_hWnd,"����ͬ���İ��,���ܸ�����",MB_OK|MB_ICONEXCLAMATION);
				return;
			}
			if(i >= 2 && strcmp(g_BlockInfo[i-2].KeyGuyCode,dlg.m_KeyGuyCode)==0)
			{
				TDX_MessageBox(m_hWnd,"����ͬ���İ����,���ܸ�����",MB_OK|MB_ICONEXCLAMATION);
				return;
			}
		}
		try
		{
			CFile::Rename(HomePath+g_strBlockNew+OldKeyGuyCode+".blk",HomePath+g_strBlockNew+dlg.m_KeyGuyCode+".blk");
		}
		catch(...)
		{
			TDX_MessageBox(m_hWnd,"����������!",MB_OK|MB_ICONEXCLAMATION);
			return;
		}
		strcpy(g_BlockInfo[BlockIndex-2].Name,dlg.m_BlockName);
		strcpy(g_BlockInfo[BlockIndex-2].KeyGuyCode,dlg.m_KeyGuyCode);
		//��ǰ�Ի����и���
		m_BlockList.SetItemText(BlockIndex,0,dlg.m_BlockName);
		BlockGPName[BlockIndex] = dlg.m_BlockName;
		RenameCusGridTab(BlockIndex-2,dlg.m_BlockName); //����ˢ���Զ����ǩ

		bBlockModify = TRUE;
	}
}

void CGetBlockDlg::FillBlock()
{
	m_BlockList.SetRedraw(FALSE);

	m_BlockList.DeleteAllItems();
	m_BlockList.InsertItem (0,_F("��ѡ��"),0);
	m_BlockList.InsertItem (1,_F("��Ʊ��"),0);
	BlockGPName[0] = _F("��ѡ��");
	BlockGPName[1] = _F("��Ʊ��");
	for(int i = 0;i < g_nBlockNum;i++)
	{
		m_BlockList.InsertItem (i+2,g_BlockInfo[i].Name,0);
		BlockGPName[i+2] = g_BlockInfo[i].Name;
	}
	BlockNum = i + 2;

	m_BlockList.SetRedraw(TRUE);
}
