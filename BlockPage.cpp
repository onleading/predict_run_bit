// BlockPage.cpp : implementation file
#include "stdafx.h"
#include "TdxW.h"
#include "BlockPage.h"
#include "KeyGuy.h"
#include "EditBlockDlg.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CBlockPage property page

IMPLEMENT_DYNCREATE(CBlockPage, CPropertyPage)

CBlockPage::CBlockPage() : CPropertyPage(CBlockPage::IDD)
{
	KeyGuyFlag = FALSE;
	AddedGPNum = 0;

	nCurBlockIndex = -1;
	bBlockGPModify = FALSE;
	bBlockModify = FALSE;
}

CBlockPage::~CBlockPage()
{
}

void CBlockPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBlockPage)
	DDX_Control(pDX, IDC_OPTION_BLOCK_NAME, m_BlockName);
	DDX_Control(pDX, IDC_OPTION_BLOCKGP_LIST, m_BlockGPList);
	DDX_Control(pDX, IDC_OPTION_BLOCK_LIST, m_BlockList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBlockPage, CPropertyPage)
	//{{AFX_MSG_MAP(CBlockPage)
	ON_BN_CLICKED(IDC_OPTION_RENAMEBLOCK, OnOptionRenameblock)
	ON_BN_CLICKED(IDC_OPTION_DELETEBLOCK, OnOptionDeleteblock)
	ON_BN_CLICKED(IDC_OPTION_NEWBLOCK, OnOptionNewblock)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_OPTION_BLOCK_LIST, OnItemchangedOptionBlockList)
	ON_BN_CLICKED(IDC_OPTION_ADDGP, OnOptionAddgp)
	ON_BN_CLICKED(IDC_OPTION_DELETEGP, OnOptionDeletegp)
	ON_BN_CLICKED(IDC_OPTION_DELETEALL, OnOptionDeleteall)
	ON_BN_CLICKED(IDC_EXPORTBLOCK, OnExportBlock)
	ON_BN_CLICKED(IDC_IMPORTBLOCK, OnImportBlock)
	ON_BN_CLICKED(IDC_MOVEUP, OnMoveup)
	ON_BN_CLICKED(IDC_MOVEDOWN, OnMovedown)
	ON_BN_CLICKED(IDC_IMPORT2BLOCK, OnImport2block)
	ON_BN_CLICKED(IDC_BLOCK_UP, OnBlockUp)
	ON_BN_CLICKED(IDC_BLOCK_DOWN, OnBlockDown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBlockPage message handlers

BOOL CBlockPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	m_BlockName.SetFontBold(TRUE);

	m_ImageList1.Create(16,16,ILC_COLOR8|ILC_MASK,1,0);
	m_ImageList2.Create(16,16,ILC_COLOR8|ILC_MASK,2,0);

	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_BLOCK));
	m_ImageList1.Add(hIcon);
	hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_GP));
	m_ImageList2.Add(hIcon);
	hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_GP_SELECTED));
	m_ImageList2.Add(hIcon);
	m_BlockList.SetImageList(&m_ImageList1,LVSIL_SMALL);
	m_BlockGPList.SetImageList(&m_ImageList2,LVSIL_SMALL);

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 76;
	lvColumn.iSubItem = 0;
	lvColumn.pszText  = (char*)(LPCSTR)g_strPreName;
	m_BlockList.InsertColumn(0,&lvColumn);

	FillBlock();
	
	GetDlgItem(IDC_OPTION_ADDGP)->EnableWindow(FALSE);
	GetDlgItem(IDC_OPTION_DELETEGP)->EnableWindow(FALSE);
	GetDlgItem(IDC_OPTION_DELETEALL)->EnableWindow(FALSE);
	GetDlgItem(IDC_EXPORTBLOCK)->EnableWindow(FALSE);
	GetDlgItem(IDC_IMPORTBLOCK)->EnableWindow(FALSE);
	GetDlgItem(IDC_IMPORT2BLOCK)->EnableWindow(FALSE);

	return TRUE; 
}

void CBlockPage::SaveToBlock(int nBlockIndex)
{
	CString tempFileName,tempLine;
	DWORD	EndPosition;
	CString tempStr = GetBlockFileName(nBlockIndex);
	tempFileName = g_strBlockNew+tempStr+(CString)".blk";

	CStdioFile m_File;
	if( m_File.Open(HomePath+tempFileName,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite|CFile::shareDenyNone) )
	{
		MemStkInfo *tempStkInfo;
		for(int i = 0;i < AddedGPNum;i++)
		{
			tempStkInfo = (*g_pStockIO)[AddedGPIndex[i]];
			BOOL bDSStock = g_pStockIO->BeDSStock(tempStkInfo);
			tempLine.Format(bDSStock?"%d#%s":"%d%s",tempStkInfo->setcode,tempStkInfo->Code);
			m_File.WriteString((CString)"\n"+tempLine);
		}
		EndPosition = m_File.GetPosition();
		m_File.SetLength(EndPosition);
		m_File.Close();
	}
	else
		TDX_MessageBox(m_hWnd,"不能打开板块股文件!不能将选股结果存盘!",MB_ICONEXCLAMATION|MB_OK);
}

void CBlockPage::LoadFromBlock(int nBlockIndex)
{
	AddedGPNum = 0;

	CString tempName = GetBlockFileName(nBlockIndex);
	CString tempFileName = g_strBlockNew+tempName+(CString)".blk";

	CString tempStr;
	int		tempIndex;
	CStdioFile m_File;
	if(m_File.Open(HomePath+tempFileName,CFile::modeRead|CFile::shareDenyNone))
	{
		while(m_File.ReadString(tempStr) && AddedGPNum<MAXGPNUM)
		{
			TrimString(tempStr);
			if(tempStr.IsEmpty()) continue;
			tempIndex = g_pStockIO->GetIndex_FromMultiBuf(tempStr);
			if( tempIndex != -1)
			{
				AddedGPIndex[AddedGPNum] = tempIndex;
				AddedGPNum++;
			}
		}
		m_File.Close();
	}
}

//更新某一板块的股票
void CBlockPage::UpdateBlockGPList(int OldIndex,int NewIndex)
{
	//将上一个板块的结果存盘
	if(bBlockGPModify && OldIndex != -1)
		SaveToBlock(OldIndex);

	bBlockGPModify = FALSE;

	//重新装入下一个
	LoadFromBlock(NewIndex);
	FillStock();
}

void CBlockPage::FillStock()
{
	m_BlockGPList.SetRedraw(FALSE);
	m_BlockGPList.DeleteAllItems();

	CString tempName;
	for(int i = 0;i < AddedGPNum;i++)
	{
		tempName = (*g_pStockIO)[AddedGPIndex[i]]->Name;
		tempName.TrimRight();	//防止出现显示不完整问题
		m_BlockGPList.InsertItem(i,tempName,0);
		m_BlockGPList.SetItemData(i,AddedGPIndex[i]);
	}
	int nIndex=-2;
	POSITION pos = m_BlockList.GetFirstSelectedItemPosition();
	if(pos != NULL) nIndex = m_BlockList.GetNextSelectedItem(pos);
	char sGpNum[8]={0};
	sprintf(sGpNum, "%d", AddedGPNum);
	if(nIndex>-1) m_BlockName.SetText(BlockGPName[nIndex]+"("+(LPCSTR)sGpNum+")");
	m_BlockGPList.SetRedraw(TRUE);
}

BOOL CBlockPage::TestSameBlockName(int blockindex,CString blockname)
{
	int i;
	for(i = 0;i < BlockNum;i++)
	{
		if(i == blockindex) continue;
		if(BlockGPName[i] == blockname)
			return TRUE;
	}
	return FALSE;
}

//板块改名时
void CBlockPage::OnOptionRenameblock()
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
		if(TestSameBlockName(BlockIndex,dlg.m_BlockName))
		{
			TDX_MessageBox(m_hWnd,"存在同名的板块,不能改名！",MB_OK|MB_ICONEXCLAMATION);
			return;
		}
		for(int i = 0;i < BlockNum;i++)
		{
			if(i == BlockIndex) continue;
			if(i >= 2 && strcmp(g_BlockInfo[i-2].KeyGuyCode,dlg.m_KeyGuyCode)==0)
			{
				TDX_MessageBox(m_hWnd,"存在同名的板块简称,不能改名！",MB_OK|MB_ICONEXCLAMATION);
				return;
			}
		}
		try
		{
			CFile::Rename(HomePath+g_strBlockNew+OldKeyGuyCode+".blk",HomePath+g_strBlockNew+dlg.m_KeyGuyCode+".blk");
		}
		catch(...)
		{
			TDX_MessageBox(m_hWnd,"板块改名错误!",MB_OK|MB_ICONEXCLAMATION);
			return;
		}
		strcpy(g_BlockInfo[BlockIndex-2].Name,dlg.m_BlockName);
		strcpy(g_BlockInfo[BlockIndex-2].KeyGuyCode,dlg.m_KeyGuyCode);
		//当前对话框中更新
		m_BlockList.SetItemText(BlockIndex,0,dlg.m_BlockName);
		BlockGPName[BlockIndex] = dlg.m_BlockName;
		RenameCusGridTab(BlockIndex-2,dlg.m_BlockName); //重新刷新自定义标签
		FillStock();

		//设置板块操作标志
		bBlockModify = TRUE;
	}
}

//删除板块时
void CBlockPage::OnOptionDeleteblock() 
{
	POSITION pos = m_BlockList.GetFirstSelectedItemPosition();
	if(pos == NULL) return;
	int BlockIndex = m_BlockList.GetNextSelectedItem(pos);
	CString theBlockName,theBlockFileName;
	theBlockName = GetBlockFileName(BlockIndex);
	int bRet = TDX_MessageBox(m_hWnd,"你确定删除板块\""+theBlockName+"\"吗？", MB_ICONQUESTION|MB_OKCANCEL);
	if(bRet == IDOK)
	{
		theBlockFileName = (CString)g_strBlockNew+theBlockName+(CString)".blk";
		CFile tempFile;	//仅用于判断该文件是否存在,如果不存在，则已
		if(tempFile.Open(HomePath+theBlockFileName,CFile::modeRead|CFile::shareDenyNone))
		{
			tempFile.Close();
			try
			{
				CFile::Remove(HomePath+theBlockFileName);
			}
			catch(...)
			{
				TDX_MessageBox(m_hWnd,"板块删除错误!",MB_OK|MB_ICONEXCLAMATION);
				return;
			}
		}
		for(int i = BlockIndex-2;i < g_nBlockNum-1;i++)
			g_BlockInfo[i] = g_BlockInfo[i+1];
		g_nBlockNum--;
		
		if(BlockIndex<m_BlockList.GetItemCount()-1)
			nCurBlockIndex = BlockIndex;
		else
			nCurBlockIndex = -1;
		FillBlock();

		AddedGPNum = 0;
		FillStock();

		GetDlgItem(IDC_OPTION_ADDGP)->EnableWindow(FALSE);
		GetDlgItem(IDC_OPTION_DELETEGP)->EnableWindow(FALSE);
		GetDlgItem(IDC_OPTION_DELETEALL)->EnableWindow(FALSE);
		GetDlgItem(IDC_EXPORTBLOCK)->EnableWindow(FALSE);
		GetDlgItem(IDC_IMPORTBLOCK)->EnableWindow(FALSE);
		GetDlgItem(IDC_IMPORT2BLOCK)->EnableWindow(FALSE);
		m_BlockName.SetWindowText("");
		
		//设置板块操作标志
		bBlockModify = TRUE;
	}
}

//新建板块时
void CBlockPage::OnOptionNewblock() 
{
	if(BlockNum == USERBLOCK_NUM+2)
	{
		TDX_MessageBox(m_hWnd,"板块股太多，不能再新建！",MB_OK);
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
		if(TestSameBlockName(-1,dlg.m_BlockName))
		{
			TDX_MessageBox(m_hWnd,"存在同名的板块,不能新建！",MB_OK|MB_ICONEXCLAMATION);
			return;
		}
		for(int i = 0;i < BlockNum;i++)
		{
			if(i >= 2 && strcmp(g_BlockInfo[i-2].KeyGuyCode,dlg.m_KeyGuyCode)==0)
			{
				TDX_MessageBox(m_hWnd,"存在同名的板块简称,不能新建！",MB_OK|MB_ICONEXCLAMATION);
				return;
			}
		}
		//生成新文件
		CString tempEmptyFile =(CString)g_strBlockNew+dlg.m_KeyGuyCode+(CString)".blk";
		CStdioFile m_EmptyFile;
		if( m_EmptyFile.Open(HomePath+tempEmptyFile,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone) == NULL) //如果是创建,严格的必须有modeWrite
		{
			TDX_MessageBox(m_hWnd,"不能创建板块文件",MB_OK|MB_ICONEXCLAMATION);
			return;
		}
		else
			m_EmptyFile.Close();

		g_BlockInfo[g_nBlockNum].tag = -1;
		strcpy(g_BlockInfo[g_nBlockNum].Name,dlg.m_BlockName);
		strcpy(g_BlockInfo[g_nBlockNum].KeyGuyCode,dlg.m_KeyGuyCode);
		g_nBlockNum++;
		//当前对话框中更新
		m_BlockList.InsertItem(BlockNum,dlg.m_BlockName,0);
		m_BlockList.SetItemState(BlockNum,LVIS_FOCUSED,LVIS_FOCUSED);
		m_BlockList.SetItemState(BlockNum,LVIS_SELECTED,LVIS_SELECTED);

		BlockGPName[BlockNum] = dlg.m_BlockName ;
		BlockNum++;
			
		AddedGPNum = 0;
		FillStock();
		//设置板块操作标志
		bBlockModify = TRUE;
	}
}

void CBlockPage::OnItemchangedOptionBlockList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	POSITION pos = m_BlockList.GetFirstSelectedItemPosition();
	int OldIndex = nCurBlockIndex; //保存原来的当前值
	if(pos != NULL)
	{
		int m_index = m_BlockList.GetNextSelectedItem(pos);

		if(m_index < 2) EnableButton(FALSE);
		else			EnableButton(TRUE);

		if(OldIndex != m_index)
		{
			nCurBlockIndex = m_index;
			UpdateBlockGPList(OldIndex,nCurBlockIndex);
			GetDlgItem(IDC_OPTION_ADDGP)->EnableWindow(TRUE);
			GetDlgItem(IDC_OPTION_DELETEGP)->EnableWindow(TRUE);
			GetDlgItem(IDC_OPTION_DELETEALL)->EnableWindow(TRUE);
			GetDlgItem(IDC_EXPORTBLOCK)->EnableWindow(TRUE);
			GetDlgItem(IDC_IMPORTBLOCK)->EnableWindow(TRUE);
			GetDlgItem(IDC_IMPORT2BLOCK)->EnableWindow(TRUE);
		}
	}	
	*pResult = 0;
}

void CBlockPage::FillBlock()
{
	m_BlockList.SetRedraw(FALSE);

	m_BlockList.DeleteAllItems();

	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	lvItem.iImage = 0;

	//填充板块列表
	lvItem.iItem = 0;
	lvItem.pszText = _F("自选股");
	m_BlockList.InsertItem (&lvItem);
	lvItem.iItem = 1;
	lvItem.pszText = _F("股票池");
	m_BlockList.InsertItem (&lvItem);

	BlockGPName[0] = _F("自选股");
	BlockGPName[1] = _F("股票池");
	for(int i = 0;i < g_nBlockNum;i++)
	{
		lvItem.iItem = i+2;
		lvItem.pszText = g_BlockInfo[i].Name;
		m_BlockList.InsertItem (&lvItem);

		BlockGPName[i+2] = g_BlockInfo[i].Name;
	}
	BlockNum = i + 2;

	m_BlockList.SetRedraw(TRUE);
	if(nCurBlockIndex>=0)
	{
		m_BlockList.SetItemState(nCurBlockIndex, LVIS_SELECTED , LVIS_SELECTED);
		m_BlockList.SetItemState(nCurBlockIndex, LVIS_FOCUSED , LVIS_FOCUSED);
	}
}

void CBlockPage::EnableButton(BOOL bEnable)
{
	if(bEnable)
	{
		GetDlgItem(IDC_OPTION_DELETEBLOCK)->EnableWindow(TRUE);
		GetDlgItem(IDC_OPTION_RENAMEBLOCK)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_OPTION_DELETEBLOCK)->EnableWindow(FALSE);
		GetDlgItem(IDC_OPTION_RENAMEBLOCK)->EnableWindow(FALSE);
	}
}

void CBlockPage::StartKeyBuy(BOOL bAuto,char tempchar)
{
	CKeyGuy dlg;

	CRect rect;
	GetWindowRect(&rect);
	dlg.RightBottomPoint = CPoint(rect.right,rect.bottom);

	dlg.bHasCmd = FALSE;
	dlg.bAlwaysOn = TRUE;
	KeyGuyFlag = TRUE;

	char tempstr[2];
	if(bAuto)
	{
		tempstr[0]=tempchar;
		tempstr[1]='\0';
		dlg.m_edit=(CString)tempstr;
	}
	if( dlg.DoModal() == IDOK )
	{
		int m_index=-1;
		if(dlg.ReturnType >= 20) //如果是扩展行情
			m_index = g_pStockIO->GetIndex(dlg.ReturnValue.Mid(2),dlg.ReturnType-20);
		else if(dlg.ReturnType == 0)
		{
			CString tmpCode = dlg.ReturnValue.Mid(1);
			short	setcode = dlg.ReturnValue[0]-'0';
			m_index = g_pStockIO->GetIndex(tmpCode,setcode);
		}
		if(m_index == -1)
		{	
			KeyGuyFlag = FALSE;
			return;
		}
		BOOL bFind = FALSE;
		for(int j = 0;j < AddedGPNum;j++)
		{
			if(AddedGPIndex[j] == m_index)
			{
				bFind = TRUE; 
				break;
			}
		}
		if(!bFind && AddedGPNum<MAXGPNUM) 
		{			
			AddedGPIndex[AddedGPNum] = m_index;
			AddedGPNum++;
			FillStock();
			bBlockGPModify = TRUE;
		}
	}
	KeyGuyFlag=FALSE;
}

void CBlockPage::OnOptionAddgp() 
{
	if(KeyGuyFlag || nCurBlockIndex < 0) return;
	StartKeyBuy(FALSE);
}

BOOL CBlockPage::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_CHAR && KeyGuyFlag == FALSE && nCurBlockIndex >= 0)
	{
		if((pMsg->wParam>=0x30&&pMsg->wParam<=0x39)||(pMsg->wParam>='a'&&pMsg->wParam<='z')
		||(pMsg->wParam>='A'&&pMsg->wParam<='Z')||pMsg->wParam== '.'||pMsg->wParam== '?')		
		{
			StartKeyBuy(TRUE,(char)pMsg->wParam);
			return TRUE;
		}
	}
	return CPropertyPage::PreTranslateMessage(pMsg);
}

void CBlockPage::OnOptionDeletegp() 
{
	POSITION pos = m_BlockGPList.GetFirstSelectedItemPosition();
	if( pos != NULL )
	{
		int nItem,i,j;
		while (pos) //先置删除标记-1
		{
			nItem = m_BlockGPList.GetNextSelectedItem(pos);
			AddedGPIndex[nItem] = -1;
		}
		for(i = AddedGPNum - 1;i >= 0;i--)
		{
			if(AddedGPIndex[i] == -1)
			{
				for(j = i+1;j < AddedGPNum;j++)
					AddedGPIndex[j-1] = AddedGPIndex[j];
				AddedGPNum--;
			}
		}
		FillStock();
		bBlockGPModify = TRUE;
	}	
}

void CBlockPage::OnOptionDeleteall() 
{
	if(TDX_MessageBox(m_hWnd,"你确定删除该板块中的全部股票吗?", MB_ICONQUESTION|MB_OKCANCEL)==IDCANCEL) 
		return;
	AddedGPNum = 0;
	FillStock();
	bBlockGPModify = TRUE;	
}

void CBlockPage::OnExportBlock()
{
	POSITION pos = m_BlockList.GetFirstSelectedItemPosition();
	if(pos == NULL) return;
	int BlockIndex = m_BlockList.GetNextSelectedItem(pos);

	//先存盘
	SaveToBlock(BlockIndex);
	
	CFileDialog FileDialog(FALSE/*Save AS*/,NULL,BlockGPName[BlockIndex]+".EBK",
							OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXTENSIONDIFFERENT,
							_F("导出板块文件(*.EBK)|*.EBK||"));
	if (FileDialog.DoModal() == IDOK)
	{		
		CString tmpFileName,tmpFileName2;
		tmpFileName2 = FileDialog.GetPathName();
		tmpFileName.Format("%sblocknew\\%s.blk",HomePath,GetBlockFileName(BlockIndex));
		if(CopyFile(tmpFileName,tmpFileName2,FALSE))
			TDX_MessageBox(m_hWnd,"导出板块成功!", MB_ICONINFORMATION|MB_OK);
		else
			TDX_MessageBox(m_hWnd,"导出失败!请检查文件路径和权限",MB_ICONINFORMATION|MB_OK);
	}	
}

void CBlockPage::OnImportBlock()
{
	POSITION pos = m_BlockList.GetFirstSelectedItemPosition();
	if(pos == NULL) return;
	int BlockIndex = m_BlockList.GetNextSelectedItem(pos);
	
	CFileDialog FileDialog(TRUE,NULL,NULL,
							OFN_NOCHANGEDIR,
							_F("导出板块文件(*.EBK)|*.EBK||"));
	if (FileDialog.DoModal() == IDOK)
	{
		if(AddedGPNum>0 && TDX_MessageBox(m_hWnd,"是否保留原板块中已经存在的股票？", MB_YESNO|MB_ICONINFORMATION)==IDYES)
		{
			CString tempStr;
			int		tempIndex;
			CStdioFile m_File;
			if(m_File.Open(FileDialog.GetPathName(),CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareDenyNone))
			{
				while(m_File.ReadString(tempStr))
				{
					TrimString(tempStr);
					if(tempStr.IsEmpty()) continue;
					tempIndex = g_pStockIO->GetIndex_FromMultiBuf(tempStr);
					if( tempIndex != -1)
					{
						BOOL bFind = FALSE;
						for(int j = 0;j < AddedGPNum;j++)
						{
							if(AddedGPIndex[j] == tempIndex)
							{
								bFind = TRUE; 
								break;
							}
						}
						if(!bFind && AddedGPNum<MAXGPNUM)
						{
							AddedGPIndex[AddedGPNum] = tempIndex;
							AddedGPNum++;
						}
					}
				}
				m_File.Close();
				FillStock();
				bBlockGPModify = TRUE;
				TDX_MessageBox(m_hWnd,"导入板块成功!", MB_ICONINFORMATION|MB_OK);
			}
			else
				TDX_MessageBox(m_hWnd,"不能打开此导入文件!",MB_ICONEXCLAMATION|MB_OK);
			return;
		}
		CString tmpFileName;
		tmpFileName.Format("%sblocknew\\%s.blk",HomePath,GetBlockFileName(BlockIndex));
		if(CopyFile(FileDialog.GetPathName(),tmpFileName,FALSE))
		{
			//重新装入
			LoadFromBlock(BlockIndex);
			FillStock();
			bBlockGPModify = TRUE;
			TDX_MessageBox(m_hWnd,"导入板块成功!", MB_ICONINFORMATION|MB_OK);
		}
		else
			TDX_MessageBox(m_hWnd,"导入失败!请检查文件路径和权限",MB_ICONINFORMATION|MB_OK);
	}
}

//////////////////////////////////////////////////////////////////////////
BOOL CBlockPage::SwapGpPos(int nNew, int nOld)
{
	if(nNew<0||nOld<0||nNew>AddedGPNum-1||nOld>AddedGPNum-1||nNew==nOld) return FALSE;

	int nTmpPos = AddedGPIndex[nOld];
	AddedGPIndex[nOld] = AddedGPIndex[nNew];
	AddedGPIndex[nNew] = nTmpPos;

	CString tempName1 = (*g_pStockIO)[AddedGPIndex[nOld]]->Name;
	tempName1.TrimRight();	//防止出现显示不完整问题
	m_BlockGPList.SetItemText(nOld, 0, tempName1);
	m_BlockGPList.SetItemData(nOld,AddedGPIndex[nOld]);

	CString tempName2 = (*g_pStockIO)[AddedGPIndex[nNew]]->Name;
	tempName2.TrimRight();	//防止出现显示不完整问题
	m_BlockGPList.SetItemText(nNew, 0, tempName2);
	m_BlockGPList.SetItemData(nNew,AddedGPIndex[nNew]);

	return TRUE;
}

void CBlockPage::OnMoveup() 
{
	POSITION pos = m_BlockGPList.GetFirstSelectedItemPosition();
	if( pos != NULL )
	{
		m_BlockGPList.SetFocus();
		int nItem = m_BlockGPList.GetNextSelectedItem(pos);
		int nTmp = nItem-1;
		if(nItem==0) nTmp=AddedGPNum-1;
		if(SwapGpPos(nTmp, nItem))
		{
			m_BlockGPList.SetItemState(nItem, 0 , LVIS_SELECTED);
			m_BlockGPList.SetItemState(nItem, 0 , LVIS_FOCUSED);

			m_BlockGPList.SetItemState(nTmp, LVIS_SELECTED , LVIS_SELECTED);
			m_BlockGPList.SetItemState(nTmp, LVIS_FOCUSED , LVIS_FOCUSED);

			bBlockGPModify = TRUE;
		}
	}
	else
		TDX_MessageBox(m_hWnd,"请选中股票",MB_ICONINFORMATION|MB_OK);
}

void CBlockPage::OnMovedown() 
{
	POSITION pos = m_BlockGPList.GetFirstSelectedItemPosition();
	if( pos != NULL )
	{
		m_BlockGPList.SetFocus();
		int nItem = m_BlockGPList.GetNextSelectedItem(pos);
		int nTmp = nItem+1;
		if(nItem==AddedGPNum-1) nTmp=0;
		if(SwapGpPos(nTmp, nItem))
		{
			m_BlockGPList.SetItemState(nItem, 0 , LVIS_SELECTED);
			m_BlockGPList.SetItemState(nItem, 0 , LVIS_FOCUSED);

			m_BlockGPList.SetItemState(nTmp, LVIS_SELECTED , LVIS_SELECTED);
			m_BlockGPList.SetItemState(nTmp, LVIS_FOCUSED , LVIS_FOCUSED);

			bBlockGPModify = TRUE;
		}
	}
	else
		TDX_MessageBox(m_hWnd,"请选中股票",MB_ICONINFORMATION|MB_OK);
}

#include "GetTxtDlg.h"
#include "KeyWordSeach.h"
void CBlockPage::OnImport2block() 
{
	int i,j,iNo,gpnum,stknum,addnum;
	CGetTxtDlg dlg;
	if(dlg.DoModal()==IDOK)
	{
		DWORD nLen = dlg.m_strContent.GetLength();
		if(nLen>0)
		{
			if(!g_pKeyWordSearch)
			{
				g_pKeyWordSearch = new KeyWordSeach;
				gpnum = g_pStockIO->GetStockNum(ALL_ALL);
				for(i=0;i<gpnum;i++)
					g_pKeyWordSearch->RegiseterStk(i,TRUE,TRUE);
			}
			addnum = 0;
			char *lpstr = new char[nLen+1];
			strncpy(lpstr,dlg.m_strContent,nLen);
			lpstr[nLen]=0;
			char *lpstrstart = lpstr;
			char *lpstrend = &lpstr[nLen-1]; 			
			//关键字查找
			g_pKeyWordSearch->InitResult();
			while (lpstrstart<lpstrend)
				lpstrstart = g_pKeyWordSearch->SearchKeyWord(STK_NUMCODE_KEY|STK_NAME_KEY, lpstrstart, lpstrend);
			if(!g_pKeyWordSearch->m_KeyWordOK.empty())
			{
				stknum = g_pKeyWordSearch->m_KeyWordOK.size();
				for(i=0;i<stknum;i++)
				{
					iNo = g_pKeyWordSearch->m_KeyWordOK[i].nIDIndex;
					BOOL bFind = FALSE;
					for(j = 0;j < AddedGPNum;j++)
					{
						if(AddedGPIndex[j] == iNo)
						{
							bFind = TRUE; 
							break;
						}
					}
					if(!bFind && AddedGPNum<MAXGPNUM) 
					{			
						AddedGPIndex[AddedGPNum] = iNo;
						AddedGPNum++;
						addnum++;
					}
				}
			}
			delete lpstr;
			if(addnum>0)
			{
				FillStock();
				bBlockGPModify = TRUE;
				CString tmpStr;
				tmpStr.Format("从文本中导入了%d只股票",addnum);
				TDX_MessageBox(m_hWnd,tmpStr,MB_ICONINFORMATION|MB_OK);
			}
			else
				TDX_MessageBox(m_hWnd,"从文本中没有导入股票",MB_ICONINFORMATION|MB_OK);
		}
	}	
}

void CBlockPage::SwapBlockPos(BOOL bUp)
{
	POSITION pos = m_BlockList.GetFirstSelectedItemPosition();
	if(pos == NULL) 
	{
		TDX_MessageBox(m_hWnd,"请选择需要移动位置的板块",MB_OK|MB_ICONEXCLAMATION);
		GetDlgItem(IDC_OPTION_BLOCK_LIST)->SetFocus();
		return;
	}
	int BlockIndex = m_BlockList.GetNextSelectedItem(pos);
	if(BlockIndex<2)
	{
		TDX_MessageBox(m_hWnd,"自选股和股票池板块不能移动位置!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	int nPos=BlockIndex-2;
	int nNewPos=nPos-1;
	if(!bUp)	
		nNewPos=nPos+1;
	if(nNewPos<0 || nNewPos>=g_nBlockNum)
		return;
	struct BlockInfo_Struct tmpStruct=g_BlockInfo[nPos];
	g_BlockInfo[nPos]=g_BlockInfo[nNewPos];
	g_BlockInfo[nNewPos]=tmpStruct;	

	m_BlockList.SetItemText(2+nPos,0,g_BlockInfo[nPos].Name);
	m_BlockList.SetItemText(2+nNewPos,0,g_BlockInfo[nNewPos].Name);
	m_BlockList.SetItemState(2+nNewPos,LVIS_FOCUSED,LVIS_FOCUSED);
	m_BlockList.SetItemState(2+nNewPos,LVIS_SELECTED,LVIS_SELECTED);

	BlockGPName[nNewPos+2] = g_BlockInfo[nNewPos].Name;
	BlockGPName[nPos+2] = g_BlockInfo[nPos].Name;
	char sGpNum[8]={0};
	sprintf(sGpNum, "%d", AddedGPNum);
	m_BlockName.SetText(BlockGPName[nNewPos+2]+"("+(LPCSTR)sGpNum+")");

	bBlockModify = TRUE;
}


void CBlockPage::OnBlockUp() 
{
	SwapBlockPos(TRUE);
}

void CBlockPage::OnBlockDown() 
{
	SwapBlockPos(FALSE);
}
