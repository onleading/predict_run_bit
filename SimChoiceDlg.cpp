// SimChoiceDlg.cpp : implementation file

#include "stdafx.h"
#include "tdxw.h"
#include "SimChoiceDlg.h"
#include "EditConDlg.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSimChoiceDlg dialog


CSimChoiceDlg::CSimChoiceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSimChoiceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSimChoiceDlg)
	m_IsAndRadio = -1;
	m_bTQ = FALSE;
	//}}AFX_DATA_INIT
	ConditionNum = 0;
	TabListNum[0] = TabListNum[1] = TabListNum[2] = 0;
}


void CSimChoiceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSimChoiceDlg)
	DDX_Control(pDX, IDC_CARRY_PROGRESS, m_Carry_Progress);
	DDX_Control(pDX, IDC_STATUS, m_CStatus);
	DDX_Control(pDX, IDC_SELECTED_NUM, m_SelectedNum);
	DDX_Control(pDX, IDC_TOTALGP_NUM, m_TotalGPNum);
	DDX_Control(pDX, IDC_CONTAB, m_ConTab);
	DDX_Control(pDX, IDC_CONLIST, m_ConList);
	DDX_Control(pDX, IDC_CONDITION_LIST, m_Condition_List);
	DDX_Radio(pDX, IDC_ADD_RADIO, m_IsAndRadio);
	DDX_Check(pDX, IDC_TQ_CHECK, m_bTQ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSimChoiceDlg, CDialog)
	//{{AFX_MSG_MAP(CSimChoiceDlg)
	ON_NOTIFY(TCN_SELCHANGE, IDC_CONTAB, OnSelchangeContab)
	ON_BN_CLICKED(IDC_MODIFYCONDITION, OnModifycondition)
	ON_BN_CLICKED(IDC_ADDCONDITION, OnAddcondition)
	ON_BN_CLICKED(IDC_DELETECONDITION, OnDeletecondition)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CONLIST, OnItemchangedConlist)
	ON_NOTIFY(NM_DBLCLK, IDC_CONLIST, OnDblclkConlist)
	ON_LBN_DBLCLK(IDC_CONDITION_LIST, OnDblclkConditionList)
	ON_BN_CLICKED(IDC_CARRYOUT_BLOCK, OnCarryoutBlock)
	ON_BN_CLICKED(IDC_CARRYOUT, OnCarryout)
	ON_BN_CLICKED(IDC_LOADCONDITION, OnLoadcondition)
	ON_BN_CLICKED(IDC_SAVECONDITION, OnSavecondition)
	ON_BN_CLICKED(IDC_CHANGEDOMAIN_BUTTON, OnChangedomainButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimChoiceDlg message handlers

void CSimChoiceDlg::SetDomainStr()
{
	CString tmpStr = "";
	for(int i=0;i < m_nSelDomain;i++)
		tmpStr+=m_GpDomain[i].Name+(CString)" ";
	GetDlgItem(IDC_DOMAIN_STATIC)->SetWindowText(_F(tmpStr));
}

BOOL CSimChoiceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_CStatus.SetTextColor(RGB(240,0,0));
	m_CStatus.SetText(_F("选股在下载的本地数据中进行,请确保数据完整"));

	int height = m_Condition_List.GetItemHeight(0);
	m_Condition_List.SetItemHeight(0,height+2);
	m_GpDomain[0].flag = DOMAIN_TYPE;
	m_GpDomain[0].which = TYPE_SHAG;
	strcpy(m_GpDomain[0].Name,GPType[TYPE_SHAG]);
	m_GpDomain[1].flag = DOMAIN_TYPE;
	m_GpDomain[1].which = TYPE_SZAG;
	strcpy(m_GpDomain[1].Name,GPType[TYPE_SZAG]);
	m_nSelDomain = 2;
	SetDomainStr();

	static char *ConTabStr[3] = {"基本资料"," 行情","其它条件"};
	for(int i = 0;i < 3;i++)
		m_ConTab.InsertItem(i,_F(ConTabStr[i]));

	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_INDEX));
	m_ImageList.Add(hIcon);
	m_ConList.SetImageList(&m_ImageList,LVSIL_SMALL);

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 180;
	lvColumn.iSubItem = 0;
	lvColumn.pszText  = "F0514M0824";
	m_ConList.InsertColumn (0,&lvColumn);	//插入一列

	CurTabSel  = 0;
	m_ConTab.SetCurSel(CurTabSel);
	FillList();

	//计算TabListNum数,在g_nCustCon中一般相同类别的放在一起
	for(i = 0;;i++)
	{
		if(g_nCustCon[i].topic[0] == 0)
			break;
		if(g_nCustCon[i].ConType >= C_CUSTJBM && g_nCustCon[i].ConType <= C_CUSTOTHER)
			TabListNum[g_nCustCon[i].ConType]++;
	}

	IsCarrying = FALSE;
	IsCarried = TRUE;
	m_IsAndRadio = 0;	//为0表示选择第1个

	UpdateData(FALSE);

	return TRUE;
}

void CSimChoiceDlg::OnSelchangeContab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int index= m_ConTab.GetCurSel();
	if(index != CurTabSel)
	{
		CurTabSel = index;
		FillList();
	}
	*pResult = 0;
}

void CSimChoiceDlg::FillList()
{
	m_ConList.SetRedraw(FALSE);
	m_ConList.DeleteAllItems();
	short tmpConType;
	switch(CurTabSel)
	{
		case 0:	tmpConType = C_CUSTJBM;		break;
		case 1:	tmpConType = C_CUSTHQ;		break;
		case 2: tmpConType = C_CUSTOTHER;	break;
		default:break;
	}
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	lvItem.iImage = 0;
	int nCount = 0;
	for(int i = 0;;i++)
	{
		if(g_nCustCon[i].topic[0] == 0)
			break;
		if(g_nCustCon[i].ConType != tmpConType)
			continue;
		lvItem.iItem = nCount++;
		lvItem.pszText = _F(g_nCustCon[i].topic);
		m_ConList.InsertItem (&lvItem);
	}
	CurListSel = -1;
	if(nCount > 0)
	{
		CurListSel = 0;
		m_ConList.SetItemState(CurListSel,LVIS_FOCUSED,LVIS_FOCUSED);
		m_ConList.SetItemState(CurListSel,LVIS_SELECTED,LVIS_SELECTED);
	}
	m_ConList.SetRedraw(TRUE);
}

//增加ListCtrl中开始
void CSimChoiceDlg::OnAddcondition() 
{
	if(CurListSel == -1) return;
	if(ConditionNum >= 50)
	{
		TDX_MessageBox(m_hWnd,"条件太多，不能再加入!",MB_OK|MB_ICONINFORMATION);
		return;
	}
	USER_CONS tmpUserCons;
	CEditConDlg dlg;
	dlg.bAdd = TRUE;
	if(CurTabSel == 0)
		dlg.tmpConTable = g_nCustCon+CurListSel;
	else if(CurTabSel == 1)
		dlg.tmpConTable = g_nCustCon+TabListNum[0]+CurListSel;
	else
		dlg.tmpConTable = g_nCustCon+TabListNum[0]+TabListNum[1]+CurListSel;
	dlg.tmpUserCons = &tmpUserCons;
	if(dlg.DoModal() == IDOK)
	{
		m_UserCons[ConditionNum++] = tmpUserCons;
		FillConsList(ConditionNum-1);
	}
}

//修改从组合选股条件ListBox中开始
void CSimChoiceDlg::OnModifycondition() 
{
	int iIndex = m_Condition_List.GetCurSel();
	if(iIndex == LB_ERR) return;
	short tmpMsg = m_UserCons[iIndex].MsgNo;
	BOOL bFind = FALSE;
	for(int i = 0;;i++)
	{
		if(g_nCustCon[i].topic[0] == 0) break;
		if(g_nCustCon[i].MsgNo == tmpMsg)
		{
			bFind = TRUE;
			break;
		}
	}
	if(!bFind) return;
	CEditConDlg dlg;
	dlg.bAdd = FALSE;
	dlg.tmpUserCons = m_UserCons+iIndex;
	dlg.tmpConTable = g_nCustCon+i;
	if(dlg.DoModal() == IDOK)
		FillConsList(iIndex);
}

//删除从组合选股条件ListBox中开始
void CSimChoiceDlg::OnDeletecondition() 
{
	int iIndex = m_Condition_List.GetCurSel();
	if(iIndex == LB_ERR) return;
	for(int i = iIndex;i < ConditionNum-1;i++)
		m_UserCons[i] = m_UserCons[i+1];
	ConditionNum--;
	FillConsList(max(iIndex--,0));
}

void CSimChoiceDlg::FillConsList(int whichsel)
{
	m_Condition_List.SetRedraw(FALSE);
	m_Condition_List.ResetContent();
	for(int i = 0;i < ConditionNum;i++)
		m_Condition_List.AddString(_F(m_UserCons[i].ConStr));
	if(whichsel >= 0)
		m_Condition_List.SetCurSel(whichsel);
	m_Condition_List.SetRedraw(TRUE);
}

void CSimChoiceDlg::OnCancel() 
{
	if(!IsCarrying && IsCarried)	//如果关闭窗口
	{
		GetParent()->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);
		g_bDonotClose = FALSE;
	}
	else	//如果取消执行选股
		IsCarrying = FALSE;
}

void CSimChoiceDlg::OnOK() 
{
}

void CSimChoiceDlg::OnItemchangedConlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	POSITION pos = m_ConList.GetFirstSelectedItemPosition();
	if(pos != NULL)
	{
		int m_index = m_ConList.GetNextSelectedItem(pos);
		if(CurListSel != m_index)
			CurListSel = m_index;
	}			
	*pResult = 0;
}

void CSimChoiceDlg::OnDblclkConlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnAddcondition();
	*pResult = 0;
}

void CSimChoiceDlg::OnDblclkConditionList() 
{
	OnModifycondition();	
}

#include "GetBlockDlg.h"
void CSimChoiceDlg::OnCarryoutBlock() 
{
	CGetBlockDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		if(HasGPInBlockAndContinue(dlg.nSelBlock))
			CarryoutToBlock(dlg.nSelBlock);
	}
}

void CSimChoiceDlg::OnCarryout() 
{
	CarryoutToBlock(-1);//缺省为直接选股到股票池
}

void CSimChoiceDlg::CarryoutToBlock(int WhichBlock)
{
	UpdateData(TRUE);
	if(ConditionNum < 1)
	{
		TDX_MessageBox(m_hWnd,"请设定选股条件!",MB_OK|MB_ICONINFORMATION);
		return;
	}

	//以下是取出所有符合条件的股票
	int TotalStockNum = FiltMulGpDomain(m_GpDomain,m_nSelDomain,awUsedStkp);
	if(TotalStockNum <= 0)
	{
		TDX_MessageBox(m_hWnd,"请选定选股范围!或选股范围中没有股票!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	int i;

	m_TotalGPNum.SetWindowText(IntToStr(TotalStockNum));
	m_SelectedNum.SetWindowText("0");

	//设置进度条
	m_Carry_Progress.SetRange32(0,TotalStockNum);
	m_Carry_Progress.SetStep(1);
	m_Carry_Progress.SetPos(0);

	IsCarried = FALSE;
	IsCarrying = TRUE;	//设置选股标志
	DisableOther();
	//在Disable其它之后，需设一控件为焦点,否则ESC不起作用
	GetDlgItem(IDCANCEL)->SetFocus();
	RefreshAllHq();
	g_bDonotClose = TRUE;
	m_CStatus.SetText(_F("正在进行选股..."));

	int SuccessNum = 0,j;
	BOOL SuccessFlag;
	CString tempStr;
	MemStkInfo *StkInfo;
	CurrStockData CurrHQ;
	for ( i = 0; i < TotalStockNum; i++ )
	{
		if( i % 2 == 0 )
		{
			Sleep(1);
			PeekMsg();
			if( !IsCarrying)
			{
				m_Carry_Progress.SetPos(0);
				SetDlgItemText(IDC_SELECTED_NUM,"");
				m_CStatus.SetWindowText("");
				break;
			}
		}
		StkInfo = (*g_pStockIO)[awUsedStkp[i]];
		g_pStockIO->ReadCurrStockData(StkInfo,&CurrHQ);
		SuccessFlag = FALSE;
		for ( j = 0; j < ConditionNum; j++ )
		{
			if(IsMeetCon(StkInfo,m_UserCons[j],&CurrHQ))
			{	
				if ( m_IsAndRadio == 1 )	//或运算
				{	
					SuccessFlag = TRUE;
					break;
				}
			}
			else
			{
				if ( m_IsAndRadio == 0 )	//与运算
				{	
					SuccessFlag = FALSE;
					break;
				}
			}
		}
		if ( j >= ConditionNum && m_IsAndRadio == 0 )
			SuccessFlag = TRUE;
		if ( SuccessFlag )		//被选中的情况
		{	
			SelectedStk[SuccessNum] = awUsedStkp[i];
			SuccessNum++;
			tempStr.Format("%d/%.1f%%",SuccessNum,SuccessNum*100.0/TotalStockNum);
			m_SelectedNum.SetWindowText(tempStr);
		}
		//进度条前进一格
		m_Carry_Progress.StepIt();
	}
	if(IsCarrying)
	{
		m_CStatus.SetText(_F("选股完毕."));
		MakeBlock(SelectedStk,SuccessNum,WhichBlock); //生成板块并显示板块
		IsCarrying = FALSE;
	}
	EnableOther();
	g_bDonotClose = FALSE;
	IsCarried = TRUE;
	return;
}

void CSimChoiceDlg::DisableOther()
{
	m_ConTab.EnableWindow(FALSE);
	m_ConList.EnableWindow(FALSE);
	m_Condition_List.EnableWindow(FALSE);
	GetDlgItem(IDC_ADDCONDITION)->EnableWindow(FALSE);
	GetDlgItem(IDC_MODIFYCONDITION)->EnableWindow(FALSE);
	GetDlgItem(IDC_DELETECONDITION)->EnableWindow(FALSE);
	GetDlgItem(IDC_LOADCONDITION)->EnableWindow(FALSE);
	GetDlgItem(IDC_SAVECONDITION)->EnableWindow(FALSE);
	GetDlgItem(IDC_ADD_RADIO)->EnableWindow(FALSE);
	GetDlgItem(IDC_OR_RADIO)->EnableWindow(FALSE);

	GetDlgItem(IDC_CHANGEDOMAIN_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_CARRYOUT)->EnableWindow(FALSE);
	GetDlgItem(IDC_CARRYOUT_BLOCK)->EnableWindow(FALSE);
	GetDlgItem(IDC_TQ_CHECK)->EnableWindow(FALSE);
	SetDlgItemText(IDCANCEL,_F("停止选股"));
}

void CSimChoiceDlg::EnableOther()
{
	m_ConTab.EnableWindow(TRUE);
	m_ConList.EnableWindow(TRUE);
	m_Condition_List.EnableWindow(TRUE);
	GetDlgItem(IDC_ADDCONDITION)->EnableWindow(TRUE);
	GetDlgItem(IDC_MODIFYCONDITION)->EnableWindow(TRUE);
	GetDlgItem(IDC_DELETECONDITION)->EnableWindow(TRUE);
	GetDlgItem(IDC_LOADCONDITION)->EnableWindow(TRUE);
	GetDlgItem(IDC_SAVECONDITION)->EnableWindow(TRUE);
	GetDlgItem(IDC_ADD_RADIO)->EnableWindow(TRUE);
	GetDlgItem(IDC_OR_RADIO)->EnableWindow(TRUE);

	GetDlgItem(IDC_CHANGEDOMAIN_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_CARRYOUT)->EnableWindow(TRUE);
	GetDlgItem(IDC_CARRYOUT_BLOCK)->EnableWindow(TRUE);
	GetDlgItem(IDC_TQ_CHECK)->EnableWindow(TRUE);
	SetDlgItemText(IDCANCEL,_F("关闭"));
}

BOOL CSimChoiceDlg::GetIndexValue(MemStkInfo *tmpInfo,int which,float &fValue)
{
	BOOL bRet = FALSE;
	int j,nRealNum,IndexNo;
	nRealNum = g_pCalc->_CallDTopViewCalc_4(tmpInfo->Code,tmpInfo->setcode,PER_DAY, RECENTNUM,m_bTQ?1:0);
	if(nRealNum < 0)  return FALSE;
	float *pOutData[MAX_LINEOUT];
	for(j=0; j<MAX_LINEOUT;j++) 
		pOutData[j] = g_pCalc->_CallDTopViewCalc_13(nRealNum);
	char IndexStr[16];
	int  WhichOut=0;
	switch(which)
	{
	case C_KDJ_J:
		strcpy(IndexStr,"KDJ");
		WhichOut = 2;
		break;
	case C_RSI:
		strcpy(IndexStr,"RSI");
		WhichOut = 0;
		break;
	case C_DMI_ADX:
		strcpy(IndexStr,"DMI");
		WhichOut = 2;
		break;
	case C_WR:
		strcpy(IndexStr,"WR");
		WhichOut = 0;
		break;
	case C_VR:
		strcpy(IndexStr,"VR");
		WhichOut = 0;
		break;
	default:
		strcpy(IndexStr,"SAR");
		WhichOut = 0;
		break;
	}
	IndexNo = g_pCalc->_CallDTopViewCalc_9(ZB_TYPE,IndexStr);
	if(IndexNo >= 0) 
	{	
		g_pCalc->_CallDTopViewCalc_5(ZB_TYPE, IndexNo, pOutData);
		if(TBE_MEANLESS(pOutData[WhichOut][nRealNum-1]) != 0)		
		{
			fValue = pOutData[WhichOut][nRealNum-1];
			bRet = TRUE;
		}
	}
	for (j=0; j<MAX_LINEOUT; j++) g_pCalc->_CallDTopViewCalc_14(pOutData[j]);
	return bRet;
}

BOOL CSimChoiceDlg::IsMeetCon(MemStkInfo *tmpInfo,USER_CONS tmpCons,struct CurrStockData *pHQ)
{
	//基本面类(全部是浮点比较)
	if(tmpCons.MsgNo >= C_ZGB && tmpCons.MsgNo <= C_JYL)
	{
		float tmpValue = 0.0;
		switch(tmpCons.MsgNo)
		{
		case C_ZGB:		tmpValue = tmpInfo->J_zgb;break;
		case C_LTB:		tmpValue = tmpInfo->ActiveCapital;break;

		case C_BG:		tmpValue = tmpInfo->J_bg;break;
		case C_HG:		tmpValue = tmpInfo->J_hg;break;

		case C_ZSZ:		tmpValue = (tmpInfo->J_zgb-tmpInfo->J_hg)*pHQ->Now;break;
		case C_LTSZ:	tmpValue = tmpInfo->ActiveCapital*pHQ->Now;break;
		case C_ZZC:		tmpValue = tmpInfo->J_zzc;break;
		case C_LDZC:	tmpValue = tmpInfo->J_ldzc;break;

		case C_wxzc:	tmpValue = tmpInfo->J_wxzc;break;	//无形资产
		case C_cqtz:	tmpValue = tmpInfo->J_cqtz;break;	//长期投资
		case C_ldfz:	tmpValue = tmpInfo->J_ldfz;break;	//流动负债
		case C_cqfz:	tmpValue = tmpInfo->J_cqfz;break;	//长期负债
		case C_zbgjj:	tmpValue = tmpInfo->J_zbgjj;break;	//资本公积金
		case C_yszk:	tmpValue = tmpInfo->J_yszk;break;	//应收帐款
		case C_yyly:	tmpValue = tmpInfo->J_yyly;break;	//营业利益
		case C_tzsy:	tmpValue = tmpInfo->J_tzsy;break;	//投资收益
		case C_jyxjl:	tmpValue = tmpInfo->J_jyxjl;break;	//经营现金净流量
		case C_zxjl:	tmpValue = tmpInfo->J_zxjl;break;	//总现金净流量
		case C_ch:		tmpValue = tmpInfo->J_ch;break;	//存货
		case C_jly:		tmpValue = tmpInfo->J_jly;break;	//净利益
		case C_wfply:	tmpValue = tmpInfo->J_wfply;break;	//未分配利益
		
		case C_JZC:		tmpValue = tmpInfo->J_jzc;break;
		case C_MGJZC:	
			if(tmpInfo->J_zgb > 1 && fabs(tmpInfo->J_jzc)>COMPPREC)
				tmpValue = tmpInfo->J_jzc/tmpInfo->J_zgb;
			else
				tmpValue = tmpInfo->J_tzmgjz;
			break;
		case C_GDQYB:	
			if( !testzs(tmpInfo) && tmpInfo->J_zzc > 1)
			{
				tmpValue = tmpInfo->J_jzc*100.0/tmpInfo->J_zzc;
				break;
			}
			return FALSE;
		case C_ZBGJJ:  tmpValue = tmpInfo->J_zbgjj;break;
		case C_MGGJJ:  
			if(tmpInfo->J_zgb > 1)
				tmpValue = tmpInfo->J_zbgjj/tmpInfo->J_zgb;
			else
				tmpValue = 0.00;
			break;
		case C_MGWFP:  
			if(tmpInfo->J_zgb > 1)
				tmpValue = tmpInfo->J_wfply/tmpInfo->J_zgb;
			else
				tmpValue = 0.00;
			break; //每股未分配
		case C_MGSY:   
			if(tmpInfo->J_zgb > 1)
				tmpValue = tmpInfo->J_jly/tmpInfo->J_zgb;
			else
				tmpValue = 0.00;
			break;
		case C_ZYSY:	tmpValue = tmpInfo->J_zysy;break;
		case C_LYZE:	tmpValue = tmpInfo->J_lyze;break;
		case C_SHLY:	tmpValue = tmpInfo->J_shly;break;
		case C_JYL:		
			if( !testzs(tmpInfo) && tmpInfo->J_jzc > 1 )
			{
				tmpValue = tmpInfo->J_jly/tmpInfo->J_jzc*100.0;
				break;
			}
			return FALSE;
		}
		switch(tmpCons.PreFix)
		{
		case 0:
			if(tmpValue > tmpCons.Value.fValue)
				return TRUE;
			break;
		case 1:
			if(fabs(tmpValue-tmpCons.Value.fValue) < COMPPREC)
				return TRUE;
			break;
		default:
			if(tmpValue < tmpCons.Value.fValue)
				return TRUE;
			break;
		}
		return FALSE;
	}
	//行情类(全部是浮点比较)
	else if(tmpCons.MsgNo >= C_NOW && tmpCons.MsgNo <= C_WTB)
	{
		float tmpValue = 0.0,ftmp;
		int ii;
		switch(tmpCons.MsgNo)
		{
		case C_NOW:		
			tmpValue = pHQ->Now;
			if(tmpValue < COMPPREC) return FALSE; //停板不算
			break;
		case C_MAX:		
			tmpValue = pHQ->Max;
			if(tmpValue < COMPPREC) return FALSE; //停板不算
			break;
		case C_MIN:		
			tmpValue = pHQ->Min;
			if(tmpValue < COMPPREC) return FALSE; //停板不算
			break;
		case C_OPEN:		
			tmpValue = pHQ->Open;
			if(tmpValue < COMPPREC) return FALSE; //停板不算
			break;
		case C_CLOSE:		
			tmpValue = pHQ->Close;
			if(tmpValue < COMPPREC) return FALSE; //停板不算
			break;
		case C_VOLUME:
			tmpValue = pHQ->Volume;
			if(tmpValue < COMPPREC) return FALSE; //停板不算
			break;
		case C_AMOUNT:
			tmpValue = pHQ->Amount;
			if(tmpValue < COMPPREC) return FALSE; //停板不算
			break;
		case C_ZAF:		
			if (pHQ->Close>COMPPREC)
			{	
				if ( pHQ->Now < COMPPREC )	ftmp = 0.0;
				else					ftmp = pHQ->Now - pHQ->Close;
				tmpValue = ftmp/pHQ->Close*(float)100.0;
				break;
			}
			return FALSE;
		case C_ZEF:		
			if (pHQ->Close>0)
			{
				if ( pHQ->Max > COMPPREC && pHQ->Min > COMPPREC )
					ftmp = pHQ->Max - pHQ->Min;
				else
					ftmp = 0.0;
				tmpValue = ftmp/pHQ->Close*100.0;
				break;
			}
			return FALSE;
		case C_SYL:		
			if( !testzs(tmpInfo) && tmpInfo->J_zgb > 1)
			{
				ftmp = (tmpInfo->J_jly*12/tmpInfo->J_HalfYearFlag)/tmpInfo->J_zgb;
				if(ftmp > COMPPREC && pHQ->Now > COMPPREC)
				{
					tmpValue = pHQ->Now/ftmp;
					break;
				}
			}
			return FALSE;
		case C_HSL:		
			if( !testzs(tmpInfo) && tmpInfo->ActiveCapital*10000 > 1 )
			{
				tmpValue = 1.0*pHQ->Volume*tmpInfo->Unit/(tmpInfo->ActiveCapital*100.0); //先除10000再乘100
				break;
			}
			return FALSE;
		case C_LIANGB:	
			if( !testzs(tmpInfo) )
			{
				short fz[8];
				InitStkTime(tmpInfo->setcode,fz);
				ii = GetTdxMinute(fz);
				if (ii > 0 && tmpInfo->VolBase > 0l)
				{
					tmpValue = (float)pHQ->Volume/ii/tmpInfo->VolBase;
					break;
				}
			}
			return FALSE;
		case C_WTB:		
			if( !testzs(tmpInfo) )
			{
				long wl = 0, wlc = 0;
				for (ii = 0;ii < 3;ii++)
				{   
					wlc = wlc+(long)pHQ->Buyv[ii]-(long)pHQ->Sellv[ii];
					wl = wl +(long)pHQ->Buyv[ii]+(long)pHQ->Sellv[ii];
				}
				if (wl != 0l)
				{
					tmpValue = 100.0*wlc/wl;
					break;
				}
			}
			return FALSE;
		}
		switch(tmpCons.PreFix)
		{
		case 0:
			if(tmpValue > tmpCons.Value.fValue)
				return TRUE;
			break;
		case 1:
			if(fabs(tmpValue-tmpCons.Value.fValue) < COMPPREC)
				return TRUE;
			break;
		default:
			if(tmpValue < tmpCons.Value.fValue)
				return TRUE;
			break;
		}
		return FALSE;
	}
	//其它类(不一定是浮点比较，有其它)
	else if(tmpCons.MsgNo <= C_VR && tmpCons.MsgNo >= C_SSNUM)
	{
		float tmpValue = 0.0;
		switch(tmpCons.MsgNo)
		{
		case C_SSNUM:		
			tmpValue = Get_SS_Day(g_pStockIO->GetIndex(tmpInfo->Code,tmpInfo->setcode));
			break;
		case C_KDJ_J:
		case C_RSI:
		case C_DMI_ADX:
		case C_WR:
		case C_VR:
			if(!GetIndexValue(tmpInfo,tmpCons.MsgNo,tmpValue))
				return FALSE;
			break;
		default:
			break;
		}
		switch(tmpCons.PreFix)
		{
		case 0:
			if(tmpValue > tmpCons.Value.nValue*1.0)
				return TRUE;
			break;
		case 1:
			if(fabs(tmpValue-tmpCons.Value.nValue*1.0) < COMPPREC)
				return TRUE;
			break;
		default:
			if(tmpValue < tmpCons.Value.nValue*1.0)
				return TRUE;
			break;
		}
		return FALSE;
	}
	return FALSE;
}

#include "InputName.h"
void CSimChoiceDlg::OnSavecondition()
{
	if(ConditionNum < 1) return;
	CInputName dlg;
	dlg.Caption = "保存组合条件";
	dlg.HintInfo = "请输入存盘文件名(不写扩展名):";
	//自动为用户生成一个文件名
	dlg.m_NewName = "";
	if( dlg.DoModal() == IDOK )
	{
		TrimString(dlg.m_NewName);
		dlg.m_NewName.MakeLower();
		if(dlg.m_NewName == "")	return;
		//将条件存盘
		CString tempFileName = g_WSXHStr+dlg.m_NewName+".cus";
		CFileException ex;
		CFile m_File;
		if( m_File.Open(tempFileName,CFile::modeCreate|CFile::modeWrite,&ex) )
		{
			FileHead.ConditionNum = ConditionNum;
			FileHead.m_IsAndRadio = m_IsAndRadio;
			m_File.Write(&FileHead,sizeof(struct FILEHEAD));
			for(int i = 0; i < ConditionNum ;i++)
				m_File.Write(&m_UserCons[i],sizeof(USER_CONS));
			m_File.Close();
		}
		else
		{
			char strError[1024];
			ex.GetErrorMessage(strError,1024);
			TDX_MessageBox(m_hWnd,"文件创建错误!\n"+(CString)strError,MB_ICONEXCLAMATION|MB_OK);
		}
	}
}

#include "ConditionLoadDlg.h"
void CSimChoiceDlg::OnLoadcondition() //装入条件
{
	CConditionLoadDlg dlg;
	dlg.ConFlag = 1;
	if( dlg.DoModal () == IDOK )
	{
		if(dlg.ReturnStr != "")
		{
			CString tempFileName = g_WSXHStr+dlg.ReturnStr;
			CFile m_File;
			if( m_File.Open(tempFileName,CFile::modeRead|CFile::shareDenyNone) )
			{
				//读入文件头
				m_File.Read(&FileHead,sizeof(struct FILEHEAD));
				m_IsAndRadio = (FileHead.m_IsAndRadio == 0)?0:1;
				ConditionNum = FileHead.ConditionNum;
				CString tempStr;
				m_Condition_List.ResetContent ();	//先将列表清空
				for(int i = 0; i < ConditionNum ;i++)
				{
					//读入组合条件
					m_File.Read(&m_UserCons[i],sizeof(USER_CONS));
					FillConsList(-1);
				}
				m_File.Close();
			}
			else
				TDX_MessageBox(m_hWnd,"不能创建文件！",MB_OK|MB_ICONEXCLAMATION);
		}
	}
	return;
}

#include "ChangeDomainDlg.h"
void CSimChoiceDlg::OnChangedomainButton() 
{
	CChangeDomainDlg dlg;
	dlg.m_nSelDomain = m_nSelDomain;
	memcpy(dlg.m_GpDomain,m_GpDomain,m_nSelDomain*sizeof(struct GpDomain_Struct));
	if(dlg.DoModal()==IDOK)
	{
		m_nSelDomain = min(MAX_DOMAINNUM,dlg.m_nSelDomain);
		memcpy(m_GpDomain,dlg.m_GpDomain,m_nSelDomain*sizeof(struct GpDomain_Struct));
		SetDomainStr();
	}
}
