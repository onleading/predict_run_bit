// blockcalcdlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "blockcalcdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBlockCalcDlg dialog


CBlockCalcDlg::CBlockCalcDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBlockCalcDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBlockCalcDlg)
	//}}AFX_DATA_INIT
	ResultGPNum = 0;
	m_GpDomain[0].flag = -1;
	m_GpDomain[0].which = -1;
	m_GpDomain[1].flag = -1;
	m_GpDomain[1].which = -1;
}


void CBlockCalcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBlockCalcDlg)
	DDX_Control(pDX, IDC_CALCSTYLE_COMBO, m_CalcStyle_Combo);
	DDX_Control(pDX, IDC_GP_LIST, m_GPList);
	DDX_Control(pDX, IDC_BLOCK2_STATIC, m_Block2_Static);
	DDX_Control(pDX, IDC_BLOCK1_STATIC, m_Block1_Static);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBlockCalcDlg, CDialog)
	//{{AFX_MSG_MAP(CBlockCalcDlg)
	ON_BN_CLICKED(IDC_BLOCK1, OnBlock1)
	ON_BN_CLICKED(IDC_BLOCK2, OnBlock2)
	ON_BN_CLICKED(IDC_CALC, OnCalc)
	ON_NOTIFY(NM_DBLCLK, IDC_GP_LIST, OnDblclkGpList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBlockCalcDlg message handlers

BOOL CBlockCalcDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Block1_Static.SetTextColor(RGB(0,0,255));	
	m_Block2_Static.SetTextColor(RGB(0,0,255));	

	m_CalcStyle_Combo.AddString (_F("并集 (+)"));
	m_CalcStyle_Combo.AddString (_F("差集 (-)"));	
	m_CalcStyle_Combo.AddString (_F("交集 (*)"));

	m_CalcStyle_Combo.SetCurSel(0);

	//股票列表框
	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_GP));
	m_ImageList.Add(hIcon);
	m_GPList.SetImageList(&m_ImageList,LVSIL_SMALL);

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 120;
	lvColumn.iSubItem = 0;
	lvColumn.pszText  = "F0514M0824";
	m_GPList.InsertColumn (0,&lvColumn);

	return TRUE;
}

#include "ChangeDomainDlg.h"
void CBlockCalcDlg::OnBlock1() 
{
	CChangeDomainDlg dlg;
	dlg.m_bSelectOne = TRUE;
	if(dlg.DoModal()==IDOK)
	{
		m_GpDomain[0] = dlg.m_GpDomain[0];
		m_Block1_Static.SetWindowText(_F(m_GpDomain[0].Name));
	}	
}

void CBlockCalcDlg::OnBlock2() 
{
	CChangeDomainDlg dlg;
	dlg.m_bSelectOne = TRUE;
	if(dlg.DoModal()==IDOK)
	{
		m_GpDomain[1] = dlg.m_GpDomain[0];
		m_Block2_Static.SetWindowText(_F(m_GpDomain[1].Name));
	}	
}

void CBlockCalcDlg::OnCalc() //开始计算
{
	if(m_GpDomain[0].flag >= 0 && m_GpDomain[1].flag >= 0)
	{
		int nCalcSel = m_CalcStyle_Combo.GetCurSel();
		int i,j;
		short	TmpGPIndex[2][MAXGPNUM];
		int		TmpGPNum[2];
		TmpGPNum[0]=GetDomainGP(m_GpDomain[0],TmpGPIndex[0]); //得到股票的索引
		TmpGPNum[1]=GetDomainGP(m_GpDomain[1],TmpGPIndex[1]);
		ResultGPNum = 0; //先清为0
		switch(nCalcSel)
		{
		case 0:	//合集
			ResultGPNum = FiltMulGpDomain(m_GpDomain,2,ResultIndex);
			break;
		case 1:	//差集=并集-交集
			{
				short	MidGPIndex1[MAXGPNUM],MidGPIndex2[MAXGPNUM];
				int		MidGPNum1 = 0,MidGPNum2 = 0;//MidGPNum1为并集结果 MidGPNum2为交集结果
				MidGPNum1 = FiltMulGpDomain(m_GpDomain,2,MidGPIndex1);	//先算出并集
				for(i=0;i < TmpGPNum[1];i++)							//再算出交集
				{
					for(j=0;j < TmpGPNum[0];j++)
					{
						if(TmpGPIndex[1][i] == TmpGPIndex[0][j])
						{
							MidGPIndex2[MidGPNum2++] = TmpGPIndex[1][i];
							break;
						}
					}
				}
				for(i=0;i < MidGPNum1;i++)
				{
					for(j=0;j < MidGPNum2;j++)
					{
						if(MidGPIndex1[i] == MidGPIndex2[j])
							break;
					}
					if(j >= MidGPNum2)
						ResultIndex[ResultGPNum++] = MidGPIndex1[i];
				}
			}
			break;
		default://交集
			for(i=0;i < TmpGPNum[1];i++)
			{
				for(j=0;j < TmpGPNum[0];j++)
				{
					if(TmpGPIndex[1][i] == TmpGPIndex[0][j])
					{
						ResultIndex[ResultGPNum++] = TmpGPIndex[1][i];
						break;
					}
				}
			}
			break;
		}
		//填充列表
		m_GPList.SetRedraw(FALSE);
		m_GPList.DeleteAllItems();	
		MemStkInfo *tmpInfo;
		char tmpStr[255];
		LV_ITEM lvItem;
		lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
		lvItem.state = 0;
		lvItem.stateMask = 0;
		lvItem.iSubItem = 0;
		for(i = 0;i < ResultGPNum;i++)
		{
			lvItem.iItem = i;
			lvItem.iImage = 0;
			tmpInfo = (*g_pStockIO)[ResultIndex[i]];
			if(!tmpInfo) continue;
			sprintf(tmpStr,"%6s  %s",tmpInfo->Code,tmpInfo->Name);
			lvItem.pszText = tmpStr;
			m_GPList.InsertItem(&lvItem);
			m_GPList.SetItemData(i,ResultIndex[i]);
		}
		m_GPList.SetRedraw(TRUE);
		sprintf(tmpStr,"计算结果,共%d只",ResultGPNum);
		GetDlgItem(IDC_RESULT_STATIC)->SetWindowText(_F(tmpStr));
	}
	else
		TDX_MessageBox(m_hWnd,"请选择两个股票组合!",MB_OK|MB_ICONERROR);
}

//存为板块
#include "GetBlockDlg.h"
void CBlockCalcDlg::OnOK() 
{
	if(ResultGPNum < 1) return;
	int gpnum = ResultGPNum;
	short *pIndex = ResultIndex;
	CGetBlockDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		if(HasGPInBlockAndContinue(dlg.nSelBlock))
		{
			CStdioFile m_File;
			DWORD EndPosition;
			CString strBlock = GetBlockFileName(dlg.nSelBlock+2),tempLine;
			CString tempStr = (CString)g_strBlockNew+strBlock+".blk";
			if( m_File.Open(HomePath+tempStr,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone) )
			{
				for(int i = 0; i < gpnum; i++)
				{
					BOOL bDSStock = g_pStockIO->BeDSStock((*g_pStockIO)[pIndex[i]]->setcode,(*g_pStockIO)[pIndex[i]]->Code);
					tempLine.Format(bDSStock?"%d#%s":"%d%s",(*g_pStockIO)[pIndex[i]]->setcode,(*g_pStockIO)[pIndex[i]]->Code);
					m_File.WriteString((CString)"\n"+tempLine);
				}
				EndPosition = m_File.GetPosition();
				m_File.SetLength(EndPosition);
				m_File.Close();
			}
			else
				TDX_MessageBox(m_hWnd,"存盘错误,不能将结果存盘!可能无写权限或系统资源不足!",MB_ICONEXCLAMATION|MB_OK);
		}
	}	
}

void CBlockCalcDlg::OnCancel() 
{
	GetParent()->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);	
}


void CBlockCalcDlg::OnDblclkGpList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_GPList.GetFirstSelectedItemPosition ();
	if(pos)
	{
		int nItem = m_GPList.GetNextSelectedItem (pos);
		int GPIndex = m_GPList.GetItemData(nItem);
		if(GPIndex < 0) return;
		ToZst((*g_pStockIO)[GPIndex]->Code,(*g_pStockIO)[GPIndex]->setcode,FALSE);
	}			
	*pResult = 0;
}
