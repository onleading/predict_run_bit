// PluginDlg.cpp : implementation file

#include "stdafx.h"
#include "tdxw.h"
#include "PluginDlg.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPluginDlg dialog


CPluginDlg::CPluginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPluginDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPluginDlg)
	m_bTQ = TRUE;
	m_BeginTime = 0;
	m_EndTime = 0;
	m_nDontUseRecentData = 0;
	//}}AFX_DATA_INIT
	m_nPluginNum = 0;
	m_nCurSel = -1;
}


void CPluginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPluginDlg)
	DDX_Control(pDX, IDC_CARRY_PROGRESS, m_Carry_Progress);
	DDX_Control(pDX, IDC_STATUS, m_CStatus);
	DDX_Control(pDX, IDC_TOTALGP_NUM, m_TotalGPNum);
	DDX_Control(pDX, IDC_SELECTED_NUM, m_SelectedNum);
	DDX_Control(pDX, IDC_PARAM1, m_Param1);
	DDX_Control(pDX, IDC_PARAM2, m_Param2);
	DDX_Control(pDX, IDC_PARAM3, m_Param3);
	DDX_Control(pDX, IDC_PARAM4, m_Param4);
	DDX_Control(pDX, IDC_PARAM1_SPIN, m_Param1_Spin);
	DDX_Control(pDX, IDC_PARAM2_SPIN, m_Param2_Spin);
	DDX_Control(pDX, IDC_PARAM3_SPIN, m_Param3_Spin);
	DDX_Control(pDX, IDC_PARAM4_SPIN, m_Param4_Spin);
	DDX_Control(pDX, IDC_PARAM1_STATIC, m_Param1_Static);
	DDX_Control(pDX, IDC_PARAM2_STATIC, m_Param2_Static);
	DDX_Control(pDX, IDC_PARAM3_STATIC, m_Param3_Static);
	DDX_Control(pDX, IDC_PARAM4_STATIC, m_Param4_Static);
	DDX_Control(pDX, IDC_PLUGIN_LIST, m_Plugin_List);
	DDX_Control(pDX, IDC_CALCCYCLE_COMBO, m_CalcCycle_Combo);
	DDX_Check(pDX, IDC_TQ_CHECK, m_bTQ);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_BeginTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_EndTime);
	DDX_Radio(pDX, IDC_RADIO2, m_nDontUseRecentData);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPluginDlg, CDialog)
	//{{AFX_MSG_MAP(CPluginDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PARAM1_SPIN, OnDeltaposParam1Spin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PARAM2_SPIN, OnDeltaposParam2Spin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PARAM3_SPIN, OnDeltaposParam3Spin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PARAM4_SPIN, OnDeltaposParam4Spin)
	ON_EN_KILLFOCUS(IDC_PARAM1, OnKillfocusParam1)
	ON_EN_KILLFOCUS(IDC_PARAM2, OnKillfocusParam2)
	ON_EN_KILLFOCUS(IDC_PARAM3, OnKillfocusParam3)
	ON_EN_KILLFOCUS(IDC_PARAM4, OnKillfocusParam4)
	ON_BN_CLICKED(IDC_CARRYOUT, OnCarryout)
	ON_BN_CLICKED(IDC_CARRYOUT_BLOCK, OnCarryoutBlock)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PLUGIN_LIST, OnItemchangedPluginList)
	ON_BN_CLICKED(IDC_CHANGEDOMAIN_BUTTON, OnChangedomainButton)
	ON_BN_CLICKED(IDC_REFRESHLIST_BUTTON, OnRefreshlistButton)
	ON_BN_CLICKED(IDC_PLUGHELP_BUTTON, OnPlughelpButton)
	ON_CBN_SELCHANGE(IDC_CALCCYCLE_COMBO, OnSelchangeCalccycleCombo)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPluginDlg message handlers

char *	PlugColStr[4] =  {"DLL名","名称","设计人","选股对象"};
int		PlugColWidth[4] = {95,130,60,190};

extern long CALLBACK CalcCallBackFunc(char * Code,short nSetCode,short DataType,void * pData/*内部申请*/,short nDataNum,NTime tFrom,NTime tEnd,BYTE nTQ,unsigned long nReserved);

typedef void (* _RegisterDataInterface)(PDATAIOFUNC);
typedef void (* _GetCopyRightInfo)(LPPLUGIN);
typedef BOOL (* _NewAndInputDataThenCalc1)(char*,short,long *,short,short,BYTE,unsigned long);
typedef BOOL (* _NewAndInputDataThenCalc2)(char*,short,long *,short,NTime,NTime,BYTE,unsigned long);

_RegisterDataInterface		RegisterDataInterface_;
_GetCopyRightInfo			GetCopyRightInfo_;
_NewAndInputDataThenCalc1	NewAndInputDataThenCalc1_;
_NewAndInputDataThenCalc2	NewAndInputDataThenCalc2_;
HMODULE						plugin_mod;

void CPluginDlg::SetDomainStr()
{
	CString tmpStr = "";
	for(int i=0;i < m_nSelDomain;i++)
		tmpStr+=m_GpDomain[i].Name+(CString)" ";
	GetDlgItem(IDC_DOMAIN_STATIC)->SetWindowText(_F(tmpStr));
}

BOOL CPluginDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetDateCtrlRange(GetDlgItem(IDC_DATETIMEPICKER1));
	SetDateCtrlRange(GetDlgItem(IDC_DATETIMEPICKER2));

	m_CStatus.SetTextColor(RGB(240,0,0));
	m_CStatus.SetText(_F("选股在下载的本地数据中进行,请确保数据完整."));

	m_CalcCycle_Combo.SetItemHeight(0,16);
	m_CalcCycle_Combo.SetItemHeight(-1,16);

	//填充计算周期列表框
	for(int i = 0; g_CalcPeriodStr[i] != NULL ;i++)
		m_CalcCycle_Combo.AddString (_F(g_CalcPeriodStr[i]));
	m_CalcCycle_Combo.SetCurSel (DAY_ST);	//默认是日线

	m_CalcCycle_Combo.SetItemHeight(0,16);
	m_CalcCycle_Combo.SetItemHeight(-1,16);
	m_GpDomain[0].flag = DOMAIN_TYPE;
	m_GpDomain[0].which = TYPE_SHAG;
	strcpy(m_GpDomain[0].Name,GPType[TYPE_SHAG]);
	m_GpDomain[1].flag = DOMAIN_TYPE;
	m_GpDomain[1].which = TYPE_SZAG;
	strcpy(m_GpDomain[1].Name,GPType[TYPE_SZAG]);
	m_nSelDomain = 2;
	SetDomainStr();

	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_PLUGIN));
	m_ImageList.Add(hIcon);
	m_Plugin_List.SetImageList(&m_ImageList,LVSIL_SMALL);

	m_Plugin_List.SetExtendedStyle (LVS_EX_FULLROWSELECT);

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	for(i = 0; i < 4;i++)
	{
		lvColumn.pszText = _F(PlugColStr[i]);
		lvColumn.cx = PlugColWidth[i];
		lvColumn.iSubItem = i;
		m_Plugin_List.InsertColumn (i,&lvColumn);
	}

	FillList();

	CTimeSpan TimeSpan(365, 0, 0, 0);
	m_BeginTime = m_BeginTime.GetCurrentTime() - TimeSpan;
	m_EndTime = m_EndTime.GetCurrentTime();

	m_nDontUseRecentData = 0;

	GetDlgItem(IDC_DATETIMEPICKER1)->EnableWindow(FALSE);
	GetDlgItem(IDC_DATETIMEPICKER2)->EnableWindow(FALSE);

	IsCarrying = FALSE;
	IsCarried = TRUE;
	UpdateData(FALSE);
	return TRUE;
}

void CPluginDlg::FillList()
{
	m_nPluginNum = 0;
	m_Plugin_List.SetRedraw(FALSE);
	m_Plugin_List.DeleteAllItems();

	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;

    long handle;
	int  done;
	struct _finddata_t ffblk;
	char filename[_MAX_PATH],string[_MAX_PATH];
	sprintf(string,"%splugin\\*.dll",HomePath);
	handle = _findfirst(string,&ffblk);
	if (handle==-1) done=-1;
	else            done=0;
    while (!done)
	{   
		sprintf(filename,"%splugin\\%s",HomePath,ffblk.name);
		plugin_mod = NULL;
		GetCopyRightInfo_ = NULL;
		plugin_mod = AfxLoadLibrary(filename);
		if(plugin_mod)
		{
			GetCopyRightInfo_ =	( _GetCopyRightInfo ) ::GetProcAddress(plugin_mod,"GetCopyRightInfo");
			if(GetCopyRightInfo_)
			{
				GetCopyRightInfo_(m_PluginInfo+m_nPluginNum);
				strcpy(m_strPlugPath[m_nPluginNum],filename);
				lvItem.iItem = m_nPluginNum;
				lvItem.iImage = 0;
				lvItem.pszText = ffblk.name;
				m_Plugin_List.InsertItem (&lvItem);				
				m_Plugin_List.SetItemText (m_nPluginNum,1,m_PluginInfo[m_nPluginNum].Name);
				m_Plugin_List.SetItemText (m_nPluginNum,2,m_PluginInfo[m_nPluginNum].Author);
				m_Plugin_List.SetItemText( m_nPluginNum,3,m_PluginInfo[m_nPluginNum].Descript);
				m_nPluginNum++;
			}
			AfxFreeLibrary(plugin_mod);
		}
		done = _findnext(handle,&ffblk);
    }
	if(handle != -1)
		_findclose(handle);
	if(m_nPluginNum > 0)
	{
		m_nCurSel = 0;
		m_Plugin_List.SetItemState(m_nCurSel,LVIS_SELECTED,LVIS_SELECTED);
		m_Plugin_List.SetItemState(m_nCurSel,LVIS_SELECTED,LVIS_SELECTED);
		ShowHideParam(m_PluginInfo[m_nCurSel].ParamNum,m_nCurSel);
	}
	m_Plugin_List.SetRedraw(TRUE);
}

void CPluginDlg::OnCancel()
{
	if(!IsCarrying && IsCarried)	//如果关闭窗口
	{
		GetParent()->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);	
		g_bDonotClose = FALSE;
	}
	else	//如果取消执行选股
		IsCarrying = FALSE;
}

void CPluginDlg::OnOK() 
{
}

//显示或掩藏某一参数项，并赋值
void CPluginDlg::ShowHideParam(int ParamNum,int WhichIndex)
{
	CString tempStr;
	switch(ParamNum)
	{
	case 4:
		m_Param4_Static.ShowWindow (SW_SHOW);
		m_Param4.ShowWindow (SW_SHOW);
		m_Param4_Spin.ShowWindow (SW_SHOW);
		m_Param4_Static.SetWindowText(m_PluginInfo[WhichIndex].ParamInfo[3].acParaName);
		m_Param4.SetWindowText(IntToStr(m_PluginInfo[WhichIndex].ParamInfo[3].nDefault));
	case 3:
		m_Param3_Static.ShowWindow (SW_SHOW);
		m_Param3.ShowWindow (SW_SHOW);
		m_Param3_Spin.ShowWindow (SW_SHOW);
		m_Param3_Static.SetWindowText(m_PluginInfo[WhichIndex].ParamInfo[2].acParaName);
		m_Param3.SetWindowText(IntToStr(m_PluginInfo[WhichIndex].ParamInfo[2].nDefault));
	case 2:
		m_Param2_Static.ShowWindow (SW_SHOW);
		m_Param2.ShowWindow (SW_SHOW);
		m_Param2_Spin.ShowWindow (SW_SHOW);
		m_Param2_Static.SetWindowText(m_PluginInfo[WhichIndex].ParamInfo[1].acParaName);
		m_Param2.SetWindowText(IntToStr(m_PluginInfo[WhichIndex].ParamInfo[1].nDefault));
	case 1:
		m_Param1_Static.ShowWindow (SW_SHOW);
		m_Param1.ShowWindow (SW_SHOW);
		m_Param1_Spin.ShowWindow (SW_SHOW);
		m_Param1_Static.SetWindowText(m_PluginInfo[WhichIndex].ParamInfo[0].acParaName);
		m_Param1.SetWindowText(IntToStr(m_PluginInfo[WhichIndex].ParamInfo[0].nDefault));
		break;
	}

	int RestParam = 4 - ParamNum;
	switch(RestParam)
	{
	case 4:
		m_Param1_Static.ShowWindow (SW_HIDE);
		m_Param1.ShowWindow (SW_HIDE);
		m_Param1_Spin.ShowWindow (SW_HIDE);
	case 3:
		m_Param2_Static.ShowWindow (SW_HIDE);
		m_Param2.ShowWindow (SW_HIDE);
		m_Param2_Spin.ShowWindow (SW_HIDE);
	case 2:
		m_Param3_Static.ShowWindow (SW_HIDE);
		m_Param3.ShowWindow (SW_HIDE);
		m_Param3_Spin.ShowWindow (SW_HIDE);
	case 1:
		m_Param4_Static.ShowWindow (SW_HIDE);
		m_Param4.ShowWindow (SW_HIDE);
		m_Param4_Spin.ShowWindow (SW_HIDE);
		break;
	}
}


//微调按钮的事件处理
void CPluginDlg::OnDeltaposParam1Spin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	CString tempParam ;
	m_Param1.GetWindowText(tempParam);
    int nParam = atoi(tempParam);
	nParam -= pNMUpDown->iDelta;
	if(nParam > m_PluginInfo[m_nCurSel].ParamInfo[0].nMax)
		nParam = m_PluginInfo[m_nCurSel].ParamInfo[0].nMax;
	if(nParam < m_PluginInfo[m_nCurSel].ParamInfo[0].nMin)
		nParam = m_PluginInfo[m_nCurSel].ParamInfo[0].nMin;

	m_Param1.SetWindowText(IntToStr(nParam));

	*pResult = 0;
}

void CPluginDlg::OnDeltaposParam2Spin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	CString tempParam ;
	m_Param2.GetWindowText(tempParam);
    int nParam = atoi(tempParam);
	nParam -= pNMUpDown->iDelta;
	if(nParam > m_PluginInfo[m_nCurSel].ParamInfo[1].nMax)
		nParam = m_PluginInfo[m_nCurSel].ParamInfo[1].nMax;
	if(nParam < m_PluginInfo[m_nCurSel].ParamInfo[1].nMin)
		nParam = m_PluginInfo[m_nCurSel].ParamInfo[1].nMin;

	m_Param2.SetWindowText(IntToStr(nParam));

	*pResult = 0;
}

void CPluginDlg::OnDeltaposParam3Spin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	CString tempParam ;
	m_Param3.GetWindowText(tempParam);
    int nParam = atoi(tempParam);
	nParam -= pNMUpDown->iDelta;
	if(nParam > m_PluginInfo[m_nCurSel].ParamInfo[2].nMax)
		nParam = m_PluginInfo[m_nCurSel].ParamInfo[2].nMax;
	if(nParam < m_PluginInfo[m_nCurSel].ParamInfo[2].nMin)
		nParam = m_PluginInfo[m_nCurSel].ParamInfo[2].nMin;

	m_Param3.SetWindowText(IntToStr(nParam));

	*pResult = 0;
}

void CPluginDlg::OnDeltaposParam4Spin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	CString tempParam ;
	m_Param4.GetWindowText(tempParam);
    int nParam = atoi(tempParam);
	nParam -= pNMUpDown->iDelta;
	if(nParam > m_PluginInfo[m_nCurSel].ParamInfo[3].nMax)
		nParam = m_PluginInfo[m_nCurSel].ParamInfo[3].nMax;
	if(nParam < m_PluginInfo[m_nCurSel].ParamInfo[3].nMin)
		nParam = m_PluginInfo[m_nCurSel].ParamInfo[3].nMin;

	m_Param4.SetWindowText(IntToStr(nParam));

	*pResult = 0;
}

void CPluginDlg::OnKillfocusParam1() 
{
	CString tempStr;
	m_Param1.GetWindowText (tempStr);
	int nParam = atoi(tempStr);
	if(nParam > m_PluginInfo[m_nCurSel].ParamInfo[0].nMax )
		nParam = m_PluginInfo[m_nCurSel].ParamInfo[0].nMax;
	if(nParam < m_PluginInfo[m_nCurSel].ParamInfo[0].nMin )	
		nParam = m_PluginInfo[m_nCurSel].ParamInfo[0].nMin;

	m_Param1.SetWindowText(IntToStr(nParam));	
}

void CPluginDlg::OnKillfocusParam2() 
{
	CString tempStr;
	m_Param2.GetWindowText (tempStr);
	int nParam = atoi(tempStr);
	if(nParam > m_PluginInfo[m_nCurSel].ParamInfo[1].nMax )
		nParam = m_PluginInfo[m_nCurSel].ParamInfo[1].nMax;
	if(nParam < m_PluginInfo[m_nCurSel].ParamInfo[1].nMin )	
		nParam = m_PluginInfo[m_nCurSel].ParamInfo[1].nMin;

	m_Param2.SetWindowText(IntToStr(nParam));	
}

void CPluginDlg::OnKillfocusParam3() 
{
	CString tempStr;
	m_Param3.GetWindowText (tempStr);
	int nParam = atoi(tempStr);
	if(nParam > m_PluginInfo[m_nCurSel].ParamInfo[2].nMax )
		nParam = m_PluginInfo[m_nCurSel].ParamInfo[2].nMax;
	if(nParam < m_PluginInfo[m_nCurSel].ParamInfo[2].nMin )	
		nParam = m_PluginInfo[m_nCurSel].ParamInfo[2].nMin;

	m_Param3.SetWindowText(IntToStr(nParam));	
}

void CPluginDlg::OnKillfocusParam4() 
{
	CString tempStr;
	m_Param4.GetWindowText (tempStr);
	int nParam = atoi(tempStr);
	if(nParam > m_PluginInfo[m_nCurSel].ParamInfo[3].nMax )
		nParam = m_PluginInfo[m_nCurSel].ParamInfo[3].nMax;
	if(nParam < m_PluginInfo[m_nCurSel].ParamInfo[3].nMin )	
		nParam = m_PluginInfo[m_nCurSel].ParamInfo[3].nMin;

	m_Param4.SetWindowText(IntToStr(nParam));	
}

//开始执行
void CPluginDlg::OnCarryout()
{
	CarryoutToBlock(-1);//缺省为条件选股
}

void CPluginDlg::CarryoutToBlock(int WhichBlock)
{
	UpdateData(TRUE);

	if (m_nDontUseRecentData && m_BeginTime > m_EndTime)
	{
		TDX_MessageBox(m_hWnd,"起始日期应在终至日期之前!",MB_OK|MB_ICONINFORMATION);
		return;
	}

	int nCurrPeriod = m_CalcCycle_Combo.GetCurSel();
	if(nCurrPeriod==MIN1_ST||nCurrPeriod==MINN_ST)
	{
		TDX_MessageBox(m_hWnd,"暂不支持此周期,请选择其它周期!",MB_OK|MB_ICONINFORMATION);
		return;
	}
	//
	if(!CanProcesLocalData(this, nCurrPeriod<DAY_ST))
		return;
	//
	NTime	time1, time2;
	memset(&time1, 0, sizeof(NTime));
	memset(&time2, 0, sizeof(NTime));
	time1.year = m_BeginTime.GetYear();
	time1.month = m_BeginTime.GetMonth();
	time1.day = m_BeginTime.GetDay();
	time2.year = m_EndTime.GetYear();
	time2.month = m_EndTime.GetMonth();
	time2.day = m_EndTime.GetDay();

	//以下是取出所有符合条件的股票
	int TotalStockNum = FiltMulGpDomain(m_GpDomain,m_nSelDomain,awUsedStkp);

	m_TotalGPNum.SetWindowText(IntToStr(TotalStockNum));
	m_SelectedNum.SetWindowText("0");

	//设置进度条
	m_Carry_Progress.SetRange32(0,TotalStockNum);
	m_Carry_Progress.SetStep(1);
	m_Carry_Progress.SetPos(0);

	//找到插件处理函数
	plugin_mod = NULL;
	NewAndInputDataThenCalc1_ = NULL;
	NewAndInputDataThenCalc2_ = NULL;
	plugin_mod = AfxLoadLibrary(m_strPlugPath[m_nCurSel]);
	if(!plugin_mod) return;
	RegisterDataInterface_ =	( _RegisterDataInterface ) ::GetProcAddress(plugin_mod,"RegisterDataInterface");
	NewAndInputDataThenCalc1_ =	( _NewAndInputDataThenCalc1 ) ::GetProcAddress(plugin_mod,"InputInfoThenCalc1");
	NewAndInputDataThenCalc2_ =	( _NewAndInputDataThenCalc2 ) ::GetProcAddress(plugin_mod,"InputInfoThenCalc2");
	if(!RegisterDataInterface_ || !NewAndInputDataThenCalc1_ || !NewAndInputDataThenCalc2_)
	{
		TDX_MessageBox(m_hWnd,"不合法的插件，不能使用!",MB_OK|MB_ICONEXCLAMATION);
		AfxFreeLibrary(plugin_mod);
		return;
	}

	RegisterDataInterface_((PDATAIOFUNC)CalcCallBackFunc);

	int i;
	long Value[4],nParam[4];
	CString m_strParam1,m_strParam2,m_strParam3,m_strParam4;
	m_Param1.GetWindowText(m_strParam1);
	m_Param2.GetWindowText(m_strParam2);
	m_Param3.GetWindowText(m_strParam3);
	m_Param4.GetWindowText(m_strParam4);
	nParam[0]=atoi(m_strParam1);
	nParam[1]=atoi(m_strParam2);
	nParam[2]=atoi(m_strParam3);
	nParam[3]=atoi(m_strParam4);
	memset(Value,0,4*sizeof(long));
	for(i = 0;i < m_PluginInfo[m_nCurSel].ParamNum;i++)
		Value[i] = nParam[i];
	IsCarried = FALSE;
	IsCarrying = TRUE;	//设置选股标志
	g_bDonotClose = TRUE;
	DisableOther();
	//在Disable其它之后，需设一控件为焦点,否则ESC不起作用
	GetDlgItem(IDCANCEL)->SetFocus();
	RefreshAllHq();
	m_CStatus.SetText(_F("正在进行选股..."));
	struct AnalyData  *AnalyDatap = new AnalyData[360];			
	int SuccessNum = 0;
	CString tempStr;
	BOOL bRet;
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
		if (m_nDontUseRecentData==1)
			bRet = NewAndInputDataThenCalc2_(((*g_pStockIO)[awUsedStkp[i]])->Code,((*g_pStockIO)[awUsedStkp[i]])->setcode,Value,CalcPeriodToTdxPeriod(nCurrPeriod), time1, time2,m_bTQ?1:0,0);
		else
			bRet = NewAndInputDataThenCalc1_(((*g_pStockIO)[awUsedStkp[i]])->Code,((*g_pStockIO)[awUsedStkp[i]])->setcode,Value,CalcPeriodToTdxPeriod(nCurrPeriod), 2000,m_bTQ?1:0,0); //插件选股如果是全数据,不用ASK_ALL用2000(10年)
		if(bRet)
		{
			SelectedStk[SuccessNum] = awUsedStkp[i];
			SuccessNum++;
			tempStr.Format("%d/%.1f%%",SuccessNum,SuccessNum*100.0/TotalStockNum);
			m_SelectedNum.SetWindowText(tempStr);
		}
		//进度条前进一格
		m_Carry_Progress.StepIt();
	}
	delete AnalyDatap;
	if(IsCarrying)
	{
		m_CStatus.SetText(_F("选股完毕."));
		MakeBlock(SelectedStk,SuccessNum,WhichBlock); //生成板块并显示板块
		IsCarrying = FALSE;
	}
	EnableOther();
	g_bDonotClose = FALSE;
	IsCarried = TRUE;
	AfxFreeLibrary(plugin_mod);
}

void CPluginDlg::DisableOther()
{
	m_Plugin_List.EnableWindow(FALSE);

	GetDlgItem(IDC_CHANGEDOMAIN_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_CALCCYCLE_COMBO)->EnableWindow(FALSE);
	GetDlgItem(IDC_CARRYOUT)->EnableWindow(FALSE);
	GetDlgItem(IDC_CARRYOUT_BLOCK)->EnableWindow(FALSE);
	GetDlgItem(IDC_TQ_CHECK)->EnableWindow(FALSE);
	SetDlgItemText(IDCANCEL,_F("停止选股"));
}

void CPluginDlg::EnableOther()
{
	m_Plugin_List.EnableWindow(TRUE);
	GetDlgItem(IDC_CHANGEDOMAIN_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_CALCCYCLE_COMBO)->EnableWindow(TRUE);
	GetDlgItem(IDC_CARRYOUT)->EnableWindow(TRUE);
	GetDlgItem(IDC_CARRYOUT_BLOCK)->EnableWindow(TRUE);
	GetDlgItem(IDC_TQ_CHECK)->EnableWindow(TRUE);
	SetDlgItemText(IDCANCEL,_F("关闭"));
}

#include "GetBlockDlg.h"
void CPluginDlg::OnCarryoutBlock() 
{
	CGetBlockDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		if(HasGPInBlockAndContinue(dlg.nSelBlock))
			CarryoutToBlock(dlg.nSelBlock);
	}
}

void CPluginDlg::OnItemchangedPluginList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	POSITION pos = m_Plugin_List.GetFirstSelectedItemPosition();
	if(pos != NULL)
	{
		int m_index = m_Plugin_List.GetNextSelectedItem(pos);
		if(m_nCurSel != m_index)
		{
			m_nCurSel = m_index;
			ShowHideParam(m_PluginInfo[m_nCurSel].ParamNum,m_nCurSel);
		}
	}	
	*pResult = 0;
}

#include "ChangeDomainDlg.h"
void CPluginDlg::OnChangedomainButton() 
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

void CPluginDlg::OnRefreshlistButton() 
{
	FillList();
}

#include "IndexInfoDlg.h"
void CPluginDlg::OnPlughelpButton() 
{
	CIndexInfoDlg dlg;
	dlg.TitleStr = (CString)m_PluginInfo[m_nCurSel].Name+" 插件注释";
	//如果是系统指标，则从盘上读取注释信息(对所有公式都实用)
	dlg.m_IndexInfo = m_PluginInfo[m_nCurSel].OtherInfo;
	dlg.bReadOnly = TRUE;
	dlg.HasOKButton = FALSE;
	dlg.DoModal ();	
}

void CPluginDlg::OnSelchangeCalccycleCombo() 
{
	int nSel;

	nSel = m_CalcCycle_Combo.GetCurSel();
	if (nSel != DAY_ST && nSel != WEEK_ST && nSel != MONTH_ST && nSel != DAYN_ST && nSel != SEASON_ST && nSel != YEAR_ST)
	{
		m_nDontUseRecentData = 0;
		GetDlgItem(IDC_RADIO3)->EnableWindow(FALSE);
		UpdateData(FALSE);
	}
	else
		GetDlgItem(IDC_RADIO3)->EnableWindow(TRUE);
	OnRadio3();

	//如果不是日,周,月线，则不显示精确复权
	int nTdxPeriod = CalcPeriodToTdxPeriod(nSel);
	BOOL bAvailTQ = (nTdxPeriod==PER_DAY || nTdxPeriod==PER_WEEK || nTdxPeriod==PER_MONTH || nTdxPeriod==PER_DAYN || nTdxPeriod==PER_SEASON || nTdxPeriod==PER_YEAR);
	GetDlgItem(IDC_TQ_CHECK)->ShowWindow(bAvailTQ?SW_SHOW:SW_HIDE);
}


void CPluginDlg::OnRadio3() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_DATETIMEPICKER1)->EnableWindow(m_nDontUseRecentData==1);
	GetDlgItem(IDC_DATETIMEPICKER2)->EnableWindow(m_nDontUseRecentData==1);
}
