// ChoiceBlockDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "ChoiceBlockDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int CycleForUse[] = {DAY_ST, WEEK_ST, MONTH_ST, -1};
/////////////////////////////////////////////////////////////////////////////
// CChoiceBlockDlg dialog


CChoiceBlockDlg::CChoiceBlockDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChoiceBlockDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChoiceBlockDlg)
	//}}AFX_DATA_INIT
	m_lPos = -1;
	m_nSet = TJ_TYPE;
	m_nPeriod = DAY_ST;
	m_BeginTime = 0;
	m_EndTime = 0;
	m_bTimerRefresh = g_GH.bDynaTJBlock;
	m_bQJ = FALSE;
	CurrentIndex = -1;
}


void CChoiceBlockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChoiceBlockDlg)
	DDX_Control(pDX, IDC_TREE1, m_tree);
	DDX_Control(pDX, IDC_STATICPOS, m_OutStatic);
	DDX_Control(pDX, IDC_CONDITION_LIST, m_Condition_List);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_BeginTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_EndTime);
	DDX_Control(pDX, IDC_CARRY_PROGRESS, m_Carry_Progress);
	DDX_Check(pDX, IDC_CHECKTIMER, m_bTimerRefresh);
	DDX_Control(pDX, IDC_CALCCYCLE_COMBO, m_CalcCycle_Combo);
	DDX_Check(pDX, IDC_QJ_CHECK, m_bQJ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChoiceBlockDlg, CDialog)
	//{{AFX_MSG_MAP(CChoiceBlockDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, OnDblclkTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnSelchangedTree1)
	ON_BN_CLICKED(IDC_ADDCONDITION, OnAddcondition)
	ON_BN_CLICKED(IDC_DELETECONDITION, OnDeletecondition)
	ON_CBN_SELCHANGE(IDC_CALCCYCLE_COMBO, OnSelchangeCalccycleCombo)
	ON_BN_CLICKED(IDC_QJ_CHECK, OnQjCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChoiceBlockDlg message handlers
BOOL CChoiceBlockDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString tmpStr;
	
	CRect rc;

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
	FillTree();
	m_EndTime = m_EndTime.GetCurrentTime();

	m_Condition_List.SetItemHeight (0,16);
	ConditionNum = 0;
	IsCarrying = FALSE;
	IsCarried = TRUE;

	OnLoadcondition();

	//填充计算周期列表框
	for(int i = 0; CycleForUse[i] >= 0 ;i++)
		m_CalcCycle_Combo.AddString (_F(g_CalcPeriodStr[CycleForUse[i]]));
	m_CalcCycle_Combo.SetCurSel (0);	//默认是日线

	CTimeSpan TimeSpan(7, 0, 0, 0);
	m_BeginTime = m_BeginTime.GetCurrentTime() - TimeSpan;
	m_bQJ = FALSE;
	GetDlgItem(IDC_DATETIMEPICKER1)->EnableWindow(FALSE);
	GetDlgItem(IDC_DATETIMEPICKER2)->EnableWindow(FALSE);

	UpdateData(FALSE);

	return TRUE;
}

extern void SaveBlockCfg();
void CChoiceBlockDlg::OnOK() 
{
	UpdateData(TRUE);
	g_GH.bDynaTJBlock = m_bTimerRefresh;

	int i,j,k;
	//清空条件板块
	vector<BlockInfo_Struct> tmpBlockInfo;
	for(i=0;i<g_nBlockNum;i++)
	{
		if(strstr(g_BlockInfo[i].KeyGuyCode, "@ConBlk")==NULL)
			tmpBlockInfo.push_back(g_BlockInfo[i]);
	}
	memset(g_BlockInfo, 0, sizeof(g_BlockInfo));
	g_nBlockNum = tmpBlockInfo.size();
	for(i=0;i<g_nBlockNum;i++)
		g_BlockInfo[i] = tmpBlockInfo[i];
	SaveBlockCfg();

	UpdateData(TRUE);
	int		nCurrPeriod = DAY_ST;
	nCurrPeriod = CycleForUse[m_CalcCycle_Combo.GetCurSel()];

	if(ConditionNum < 1)
	{
		TDX_MessageBox(m_hWnd,"请加入选股条件!",MB_OK|MB_ICONINFORMATION);
		return;
	}
	if (m_bQJ && m_BeginTime > m_EndTime)
	{
		TDX_MessageBox(m_hWnd,"起始日期应在终至日期之前!",MB_OK|MB_ICONINFORMATION);
		return;
	}

	char acCode[ZBCODE_LEN+1];
	short TjIndexNo[MAX_TJCOMB_NUM];
	memset(TjIndexNo,0,MAX_TJCOMB_NUM*sizeof(short));
	for (i = 0; i < ConditionNum; i++ )
	{
		strcpy(acCode, ConditionInfo[i].acCode);
		TINDEXINFO	* pTJIndex = g_pCalc->_CallDTopViewCalc_8(TJ_TYPE,(long)0);
		for(j = 0;j < g_pCalc->_CallDTopViewCalc_7(TJ_TYPE);j++)//如果是从存盘文件中取得的条件，则应检查有否此条件
		{
			if ( strcmp(acCode,pTJIndex[j].acCode) == 0 )
			{	
				TjIndexNo[i] = j;
				for ( k = 0; k < min(pTJIndex[TjIndexNo[i]].nParaNum,ConditionInfo[i].ParamNum); k++)
					pTJIndex[TjIndexNo[i]].aPara[k].nValue[nCurrPeriod] = ConditionInfo[i].ParamValue[k];
				break;
			}
		}
	}

	//以下是取出所有符合条件的股票
	GpDomain_Struct m_GpDomain[100]={0}; 
	m_GpDomain[0].flag = DOMAIN_TYPE;
	m_GpDomain[0].which = TYPE_SHAG;
	strcpy(m_GpDomain[0].Name,GPType[TYPE_SHAG]);
	m_GpDomain[1].flag = DOMAIN_TYPE;
	m_GpDomain[1].which = TYPE_SZAG;
	strcpy(m_GpDomain[1].Name,GPType[TYPE_SZAG]);
	int m_nSelDomain = 2;
	short  awUsedStkp[MAXGPNUM]={-1};
	vector<short> resStkp[MAX_TJCOMB_NUM];
	int TotalStockNum = FiltMulGpDomain(m_GpDomain,m_nSelDomain,awUsedStkp);
	if(TotalStockNum <= 0)
		return;

	//设置进度条
	m_Carry_Progress.SetRange32(0,TotalStockNum);
	m_Carry_Progress.SetStep(1);
	m_Carry_Progress.SetPos(0);

	IsCarried = FALSE;
	IsCarrying = TRUE;	//设置选股标志
	//在Disable其它之后，需设一控件为焦点,否则ESC不起作用
	GetDlgItem(IDCANCEL)->SetFocus();
	RefreshAllHq(1*60);
	g_bDonotClose = TRUE;

	int SuccessNum = 0,ReadNum;
	int SuccessFlag;					//0:待定 1:成功	-1:失败 -2:阶段满足
	CString tempStr;


	MemStkInfo *pZSCode = g_pStockIO->Get_MemStkInfo("999999", 1);
	int ReadNumMax = g_pStockIO->GetAnalyDataNum(pZSCode, PER_DAY, ASK_ALL)+100;
	AnalyData *pAnalyDataTmp = new AnalyData[ReadNumMax];
	memset(pAnalyDataTmp, 0, ReadNumMax*sizeof(AnalyData));
	HISDAT *pCalcData = new HISDAT[ReadNumMax];
	memset(pCalcData, 0, ReadNumMax*sizeof(HISDAT));

	float * fPut[MAX_LINEOUT];
	memset(fPut,0,MAX_LINEOUT*sizeof(float*));
	long time1=TimeToLong(m_BeginTime);
	long time2=TimeToLong(m_EndTime);
	MemStkInfo *pStkInfo=NULL;
	for ( i = 0; i < TotalStockNum; i++ )
	{
		if( i % 2 == 0 )
		{
			PeekMsg();
			if( !IsCarrying)
			{
				m_Carry_Progress.SetPos(0);
				break;
			}
		}
		pStkInfo = (*g_pStockIO)[awUsedStkp[i]];
		SuccessFlag = 0;
		int nCalcPer = 0,nPer;
		CurrStockData hqtmp={0};
		while(pStkInfo && pStkInfo->Close>COMPPREC && nCalcPer<ConditionNum)
		{
			nPer = ConditionInfo[nCalcPer].CalcCycle;
			int nPerCalc = CalcPeriodToTdxPeriod(nPer);
			
			memset(pAnalyDataTmp, 0, ReadNumMax*sizeof(AnalyData));
			ReadNum = g_pStockIO->ReadAnalyDataFromTime(pStkInfo,pAnalyDataTmp,ReadNumMax,-1,time2,nPerCalc,GetMulNum(CalcPeriodToTdxPeriod(nPer)));
			
			if(ReadNum>0)
			{
				RawData2CalcData(pStkInfo->Code,pStkInfo->setcode,nPerCalc,pAnalyDataTmp,pCalcData,ReadNum,g_NoTime);
				ReadNum = g_pCalc->_CallDTopViewCalc_3(pStkInfo->Code,pStkInfo->setcode,nPerCalc,pCalcData,ReadNum);
				for ( j = nCalcPer; j < ConditionNum && ConditionInfo[j].CalcCycle==nPer; j++ )
				{
					TINDEXINFO	* pTJIndex = g_pCalc->_CallDTopViewCalc_8(TJ_TYPE,TjIndexNo[j]);
					if(!pTJIndex) continue;
					g_pCalc->_CallDTopViewCalc_14(fPut[0]);
					memset(fPut,0,MAX_LINEOUT*sizeof(float*));
					fPut[0]  = g_pCalc->_CallDTopViewCalc_13(ReadNum);
					for(k=0;k<min(ConditionInfo[j].ParamNum,pTJIndex->nParaNum);k++)
						pTJIndex->aPara[k].nValue[nCurrPeriod] = ConditionInfo[j].ParamValue[k];
					if ( -1  != g_pCalc->_CallDTopViewCalc_5(TJ_TYPE,TjIndexNo[j],fPut) )
					{
						if(!m_bQJ)
						{
							if((TBE_MEANLESS(fPut[0][ReadNum-1]) != 0) && fabs(fPut[0][ReadNum-1]) > 0.1)
								resStkp[j].push_back(awUsedStkp[i]);
						}
						else 
						{
							for(int k=ReadNum-1;k>=0;k--)
							{
								if (pAnalyDataTmp[k].Time.Date>=time1 && pAnalyDataTmp[k].Time.Date<=time2)
								{
									if((TBE_MEANLESS(fPut[0][k]) != 0) && fabs(fPut[0][k]) > 0.1)
									{
										resStkp[j].push_back(awUsedStkp[i]);
										break;
									}
								}
								else if(pAnalyDataTmp[k].Time.Date<time1)
									break;
							}
						}
					}
				}
				nCalcPer = j;
			}
			else 
			{
				++nCalcPer;
				continue;
			}
		}
		m_Carry_Progress.StepIt();
	}
	TDELA(pCalcData);
	TDELA(pAnalyDataTmp);
	g_pCalc->_CallDTopViewCalc_14(fPut[0]);
	if(IsCarrying)
	{
		for(i=0;i<ConditionNum;i++)
		{
			CString Name;
			m_Condition_List.GetText(i, Name);
			char Code[100]={0};
			sprintf(Code, "@ConBlk%d", i);
			SeveToNewBlock(resStkp[i], Code, (char*)(LPCSTR)Name,i);
		}
		SaveBlockCfg();
		IsCarrying = FALSE;
	}
	InterlockedIncrement(&g_nCountCalcBlockTJ);
	g_bDonotClose = FALSE;
	IsCarried = TRUE;

	//将条件板块存盘
	memset(&FileHead, 0, sizeof(FileHead));
	CString tempFileName = g_WSXHStr+"conblk.cos";
	CFileException ex;
	CFile m_File;
	if( m_File.Open(tempFileName,CFile::modeCreate|CFile::modeWrite,&ex) )
	{
		FileHead.ConditionNum = ConditionNum;
		FileHead.nDate = TimeToLong(m_EndTime);
		FileHead.nRefreshTime = -1;
		m_File.Write(&FileHead,sizeof(FILEHEAD));
		for(int i = 0; i < ConditionNum ;i++)
			m_File.Write(&ConditionInfo[i],sizeof(CONDITIONINFO));
		m_File.Close();
	}
	else
	{
		char szError[1024];
		ex.GetErrorMessage(szError, 1024);
		TDX_MessageBox(m_hWnd,"文件创建错误:\n"+(CString)szError,MB_OK+MB_ICONEXCLAMATION);
	}

	return;
}

void CChoiceBlockDlg::OnCancel()
{
	UpdateData(TRUE);
	g_GH.bDynaTJBlock = m_bTimerRefresh;
	
	//将条件板块存盘
	memset(&FileHead, 0, sizeof(FileHead));
	CString tempFileName = g_WSXHStr+"conblk.cos";
	CFileException ex;
	CFile m_File;
	if( m_File.Open(tempFileName,CFile::modeCreate|CFile::modeWrite,&ex) )
	{
		FileHead.ConditionNum = ConditionNum;
		FileHead.nDate = TimeToLong(m_EndTime);
		FileHead.nRefreshTime = -1;
		m_File.Write(&FileHead,sizeof(FILEHEAD));
		for(int i = 0; i < ConditionNum ;i++)
			m_File.Write(&ConditionInfo[i],sizeof(CONDITIONINFO));
		m_File.Close();
	}
	else
	{
		char szError[1024];
		ex.GetErrorMessage(szError, 1024);
		TDX_MessageBox(m_hWnd,"文件创建错误:\n"+(CString)szError,MB_OK+MB_ICONEXCLAMATION);
	}

	m_Form.SendMessage(UM_RESTORE, 0, 0);
	GetParent()->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);	
}

void CChoiceBlockDlg::OnLoadcondition() //装入条件
{
	CString tempFileName = g_WSXHStr+"conblk.cos";
	CFile m_File;
	if( m_File.Open(tempFileName,CFile::modeRead|CFile::shareDenyNone) )
	{
		//读入文件头
		m_File.Read(&FileHead,sizeof(struct FILEHEAD));
		ConditionNum = FileHead.ConditionNum;
		CString tempStr;
		m_Condition_List.ResetContent ();	//先将列表清空
		for(int i = 0; i < ConditionNum ;i++)
		{
			//读入组合条件
			m_File.Read(&ConditionInfo[i],sizeof(struct CONDITIONINFO));
			TINDEXINFO	* pTJIndex = g_pCalc->_CallDTopViewCalc_8(TJ_TYPE,ConditionInfo[i].acCode);
			if(!pTJIndex) continue;
			//构造列表项
			tempStr = (CString)pTJIndex->acCode + pTJIndex->acName + " ";
			if(ConditionInfo[i].ParamNum != 0)
				tempStr = tempStr + "(";
			for (int j =0 ;j < ConditionInfo[i].ParamNum; j++)
			{
				if( j == ConditionInfo[i].ParamNum-1 )
					tempStr = tempStr + IntToStr(ConditionInfo[i].ParamValue [j]);
				else
					tempStr = tempStr + IntToStr(ConditionInfo[i].ParamValue [j])+",";
			}
			if(ConditionInfo[i].ParamNum != 0)
				tempStr = tempStr + ") ";
			tempStr = tempStr + _F(g_CalcPeriodStr[ConditionInfo[i].CalcCycle]);
			//
			
			m_Condition_List.AddString (tempStr);
		}
		m_File.Close();
		UpdateData(FALSE);
	}
}

void CChoiceBlockDlg::InsertType(HTREEITEM hFatherItem, long nTypeNo)
{
	TYPEX te={0};
	if(!g_pCalc->_CallDTopViewCalc_12(&te, TJ_TYPE, nTypeNo)) return;
	if(!te.bEnable) return;
	
	int i=0, j=0;
	HTREEITEM hMyItem = TVI_ROOT, hChild = TVI_ROOT;
	char tempItemStr[125]={0};
	TINDEXINFO *tmpIndexInfo=NULL;
	int  nTotalType = g_pCalc->_CallDTopViewCalc_10(TJ_TYPE);
	int nTotalNum = g_pCalc->_CallDTopViewCalc_7(TJ_TYPE);

	hMyItem = m_tree.InsertItem(_F(te.lpszName),0,0,hFatherItem,TVI_LAST);
	m_tree.SetItemData(hMyItem,65535);
	for(i = 0;i < nTotalType;i++)
	{
		if(nTypeNo==i) continue;
		TYPEX te1={0};
		if(!g_pCalc->_CallDTopViewCalc_12(&te1, TJ_TYPE, i)) continue;
		if(te1.nFatherType==te.nTypeID)		//找到子类型
			InsertType(hMyItem, i);
	}
	for(j = 0;j < nTotalNum;j++)
	{
		tmpIndexInfo = g_pCalc->_CallDTopViewCalc_8(TJ_TYPE,j);
		if(tmpIndexInfo && tmpIndexInfo->nType == te.nTypeID )
		{
			sprintf(tempItemStr,"%s %s",tmpIndexInfo->acCode,tmpIndexInfo->acName);
			hChild = m_tree.InsertItem(_F(tempItemStr),1,1,hMyItem,TVI_LAST);
			m_tree.SetItemData(hChild,j);
			continue;
		}
	}
}

void CChoiceBlockDlg::FillTree()
{
	int i,nTotalNum,nTotalType;
	HTREEITEM hRoot = TVI_ROOT,hChild = TVI_ROOT;
	
	m_tree.SetRedraw(FALSE);
	m_tree.DeleteAllItems(); //首先清空原来的树内容

	TINDEXINFO *tmpIndexInfo=NULL;
	nTotalNum = g_pCalc->_CallDTopViewCalc_7(TJ_TYPE);
	nTotalType = g_pCalc->_CallDTopViewCalc_10(TJ_TYPE);
	for(i = 0;i < nTotalType;i++)
	{
		TYPEX te={0};
		if(!g_pCalc->_CallDTopViewCalc_12(&te, TJ_TYPE, i)) continue;
		if(te.nFatherType!=-1) continue;
		InsertType(TVI_ROOT, i);
	}
	m_tree.SetRedraw(TRUE);
}

void CChoiceBlockDlg::OnDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	HTREEITEM pos = m_tree.GetSelectedItem();
	if(pos==NULL) return;
	DWORD dwPos=m_tree.GetItemData(pos);
	if (dwPos>0&&!m_tree.ItemHasChildren(pos)&&dwPos<10000)
		OnAddcondition();	
}

void CChoiceBlockDlg::OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;	
	
	HTREEITEM pos = m_tree.GetSelectedItem();
	if(pos==NULL) return;
	DWORD dwPos=m_tree.GetItemData(pos);
	if (dwPos>=0&&!m_tree.ItemHasChildren(pos)&&dwPos<10000)
	{
		CurrentIndex = dwPos;
		if (CurrentIndex >= 0)
		{
			//先将所有的edit和spin隐藏,然后在m_Form.SetStatus的处理中做相应调整
			m_Form.HideSelf();
			m_Form.SetStatus(m_nSet, CurrentIndex, m_nPeriod, FALSE);
		}
	}
	else
	{
		m_Form.HideSelf();
		m_Form.SetStatus(m_nSet, 0, m_nPeriod, TRUE);
	}
}
//////////////////////////////////////////////////////////////////////////
void CChoiceBlockDlg::OnAddcondition() 
{	
	HTREEITEM pos=m_tree.GetSelectedItem();
	if(pos==NULL) return;
	if(m_tree.ItemHasChildren(pos)) return;
	DWORD dwPos=m_tree.GetItemData(pos);
	CurrentIndex = dwPos;
	if(CurrentIndex < 0 || CurrentIndex > 10000)	
		return;

	float fCurrParam[MAX_PARAMNUM]={0};
	if(ConditionNum >= MAX_TJCOMB_NUM)
	{
		TDX_MessageBox(m_hWnd,"条件太多，不能再加入!",MB_OK+MB_ICONINFORMATION);
		return;
	}

	CString m_strParam[MAX_PARAMNUM];
	memset(fCurrParam, 0, sizeof(float) * MAX_PARAMNUM);
	m_Form.GetCurrData(fCurrParam);

	for(int i=0;i<MAX_PARAMNUM;i++)
	{
		m_strParam[i].Format("%5.f",fCurrParam[i]);
		TrimString(m_strParam[i]);
	}

	CString tempstr = "";
	int ParamNum = (*g_pCalc)[TJ_TYPE][CurrentIndex].nParaNum;
	if(ParamNum>0)
	{
		tempstr += "(";
		for(i=0;i<ParamNum;i++)
		{
			tempstr += m_strParam[i];
			if(i<ParamNum-1) tempstr += ",";
		}
		tempstr += ")";
	}

	tempstr = (CString)(*g_pCalc)[TJ_TYPE][CurrentIndex].acCode + (CString)(*g_pCalc)[TJ_TYPE][CurrentIndex].acName + " " + g_CalcPeriodStr[CycleForUse[m_CalcCycle_Combo.GetCurSel()]] + " " + tempstr;
	m_Condition_List.AddString (tempstr);	

	CONDITIONINFO tmpCon={0};
	strcpy(tmpCon.acCode , (*g_pCalc)[TJ_TYPE][CurrentIndex].acCode);
	tmpCon.ParamNum = ParamNum ;
	tmpCon.CalcCycle = CycleForUse[m_CalcCycle_Combo.GetCurSel()];
	
	for(i=0;i<ParamNum;i++)
		tmpCon.ParamValue[i] = atoi(m_strParam[i]);

	if(ConditionNum==0) 
		memcpy(&ConditionInfo[ConditionNum], &tmpCon, sizeof(CONDITIONINFO));
	for(i=ConditionNum-1;i>=0;i--)
	{
		if(ConditionInfo[i].CalcCycle==tmpCon.CalcCycle)
		{
			memmove(&ConditionInfo[i+2], &ConditionInfo[i+1], (ConditionNum-i-1)*sizeof(CONDITIONINFO));
			memcpy(&ConditionInfo[i+1], &tmpCon, sizeof(CONDITIONINFO));
			break;
		}
	}
	if(i==-1) memcpy(&ConditionInfo[ConditionNum], &tmpCon, sizeof(CONDITIONINFO));
	ConditionNum++;
	m_Condition_List.SetCurSel(ConditionNum-1);
}

void CChoiceBlockDlg::OnDeletecondition()
{
	int nSelCondition = m_Condition_List.GetCurSel();
	if(nSelCondition != LB_ERR)
	{
		m_Condition_List.DeleteString (nSelCondition);

		for(int i = nSelCondition+1; i < ConditionNum;i++)
			ConditionInfo[i-1] = ConditionInfo[i];
		ConditionNum--;
		m_Condition_List.SetCurSel(max(nSelCondition-1,0));
	}
}

void CChoiceBlockDlg::OnQjCheck() 
{
	UpdateData(TRUE);
	if(m_bQJ)
		TDX_MessageBox(m_hWnd,"历史阶段选股的含义是:\r\n如果在设置的历史时间段满足条件(而不只是当天满足),就将此品种选出,同时将不会盘中刷新选股",MB_OK|MB_ICONINFORMATION);
	GetDlgItem(IDC_DATETIMEPICKER1)->EnableWindow(m_bQJ);
	GetDlgItem(IDC_DATETIMEPICKER2)->EnableWindow(m_bQJ);
	m_bTimerRefresh = FALSE;
	GetDlgItem(IDC_CHECKTIMER)->EnableWindow(!m_bQJ);
	UpdateData(FALSE);
}

void CChoiceBlockDlg::OnSelchangeCalccycleCombo() 
{
	int nSel = CycleForUse[m_CalcCycle_Combo.GetCurSel()];
	if (nSel != DAY_ST && nSel != WEEK_ST && nSel != MONTH_ST && nSel != DAYN_ST && nSel != SEASON_ST && nSel != YEAR_ST)
	{
		m_bQJ = FALSE;
		UpdateData(FALSE);
	}
	//得新设置m_Form的状态
	m_Form.HideSelf();
	m_Form.SetStatus(TJ_TYPE, CurrentIndex, nSel);
	OnQjCheck();
}
