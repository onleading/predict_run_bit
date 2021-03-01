// ConditionGPDlg.cpp : implementation file

#include "stdafx.h"
#include "tdxw.h"
#include "GridView.h"
#include "ConditionGPDlg.h"
#include "MainFrm.h"

int l_lastchoicezb=0;	//��¼��һ��ѡ�ɵ�ָ�����

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConditionGPDlg dialog

CConditionGPDlg::CConditionGPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConditionGPDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConditionGPDlg)
	m_IsAndRadio = 0;
	m_bTQ = TRUE;
	m_BeginTime = 0;
	m_EndTime = 0;
	m_bDynaK = FALSE;
	m_bQJ = FALSE;
	//}}AFX_DATA_INIT
}

void CConditionGPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConditionGPDlg)
	DDX_Control(pDX, IDC_STATIC1, m_OutStatic);
	DDX_Control(pDX, IDC_SELECTED_NUM, m_SelectedNum);
	DDX_Control(pDX, IDC_TOTALGP_NUM, m_TotalGPNum);
	DDX_Control(pDX, IDC_STATUS, m_CStatus);
	DDX_Control(pDX, IDC_CARRY_PROGRESS, m_Carry_Progress);
	DDX_Control(pDX, IDC_CONDITION_LIST, m_Condition_List);
	DDX_Control(pDX, IDC_CHOICEGPINDEX_COMBO, m_ChoiceGPIndex_Combo);
	DDX_Control(pDX, IDC_CALCCYCLE_COMBO, m_CalcCycle_Combo);
	DDX_Radio(pDX, IDC_ADD_RADIO, m_IsAndRadio);
	DDX_Check(pDX, IDC_TQ_CHECK, m_bTQ);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_BeginTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_EndTime);
	DDX_Check(pDX, IDC_DYNAK_CHECK, m_bDynaK);
	DDX_Check(pDX, IDC_QJ_CHECK, m_bQJ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConditionGPDlg,CDialog)
	//{{AFX_MSG_MAP(CConditionGPDlg)
	ON_BN_CLICKED(IDC_INDEX_INFOMATION, OnIndexInfomation)
	ON_BN_CLICKED(IDC_ADDCONDITION, OnAddcondition)
	ON_BN_CLICKED(IDC_DELETECONDITION, OnDeletecondition)
	ON_BN_CLICKED(IDC_CARRYOUT, OnCarryout)
	ON_BN_CLICKED(IDC_SAVECONDITION, OnSavecondition)
	ON_BN_CLICKED(IDC_LOADCONDITION, OnLoadcondition)
	ON_CBN_SELCHANGE(IDC_CHOICEGPINDEX_COMBO, OnSelchangeChoicegpindexCombo)
	ON_BN_CLICKED(IDC_CARRYOUT_BLOCK, OnCarryoutBlock)
	ON_BN_CLICKED(IDC_CHANGEDOMAIN_BUTTON, OnChangedomainButton)
	ON_CBN_SELCHANGE(IDC_CALCCYCLE_COMBO, OnSelchangeCalccycleCombo)
	ON_BN_CLICKED(IDC_QJ_CHECK, OnQjCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CConditionGPDlg::SetDomainStr()
{
	CString tmpStr = "";
	for(int i=0;i < m_nSelDomain;i++)
		tmpStr+=m_GpDomain[i].Name+(CString)" ";
	GetDlgItem(IDC_DOMAIN_STATIC)->SetWindowText(_F(tmpStr));
}

BOOL CConditionGPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetDateCtrlRange(GetDlgItem(IDC_DATETIMEPICKER1));
	SetDateCtrlRange(GetDlgItem(IDC_DATETIMEPICKER2));

	m_CStatus.SetTextColor(RGB(240,0,0));
	m_CStatus.SetText(_F("ѡ�������صı��������н���,��ȷ����������"));
	m_ChoiceGPIndex_Combo.SetItemHeight(0,16);
	m_ChoiceGPIndex_Combo.SetItemHeight(-1,16);
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

	CRect rc;
	m_OutStatic.GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.DeflateRect(6,6);
	rc.top += 20;
	rc.bottom -= 50;
	m_Form.Create(NULL, "", WS_CHILD|WS_VISIBLE, rc, this, ID_CONTROL_VIEW , NULL);
	m_Form.Init(rc);
	m_Form.HideSelf();

	CString tempStr;
	int nTotalNum = g_pCalc->_CallDTopViewCalc_7(TJ_TYPE);
	if(l_lastchoicezb<0 || l_lastchoicezb>=nTotalNum)
		l_lastchoicezb=0;
	for(int i = 0;i < nTotalNum;i++)	//�������ѡ��ָ���б��
	{
		tempStr.Format("%-8s - %-16s",(*g_pCalc)[TJ_TYPE][i].acCode,(*g_pCalc)[TJ_TYPE][i].acName);
		m_ChoiceGPIndex_Combo.AddString (_F(tempStr));
	}
	if(i > 0)
	{
		CurrentIndex = l_lastchoicezb;
		m_ChoiceGPIndex_Combo.SetCurSel (CurrentIndex);
		int ParamNum = (*g_pCalc)[TJ_TYPE][CurrentIndex].nParaNum;
	}
	else
	{
		CurrentIndex = -1;
	}

	//�����������б��
	for(i = 0; g_CalcPeriodStr[i] != NULL ;i++)
		m_CalcCycle_Combo.AddString (_F(g_CalcPeriodStr[i]));
	m_CalcCycle_Combo.SetCurSel (DAY_ST);	//Ĭ��������
	if(l_lastchoicezb>=0)
		m_Form.SetStatus(TJ_TYPE, l_lastchoicezb, DAY_ST);

	m_Condition_List.SetItemHeight (0,16);
	ConditionNum = 0;

	GetDlgItem(IDC_ADD_RADIO)->EnableWindow(FALSE);
	GetDlgItem(IDC_OR_RADIO)->EnableWindow(FALSE);

	m_IsAndRadio = 0;	//Ϊ0��ʾѡ���1��

	CTimeSpan TimeSpan(7, 0, 0, 0);
	m_BeginTime = m_BeginTime.GetCurrentTime() - TimeSpan;
	m_EndTime = m_EndTime.GetCurrentTime();

	m_bQJ = FALSE;
	GetDlgItem(IDC_DATETIMEPICKER1)->EnableWindow(FALSE);
	GetDlgItem(IDC_DATETIMEPICKER2)->EnableWindow(FALSE);

	CanClearList = FALSE;
	IsCarrying = FALSE;
	IsCarried = TRUE;
	UpdateData(FALSE);
	return TRUE;
}

void CConditionGPDlg::OnOK()
{

}

//���丸�෢�˳�����Ϣ
void CConditionGPDlg::OnCancel()
{
	if(!IsCarrying && IsCarried)	//����رմ���
	{
		m_Form.SendMessage(UM_RESTORE, 0, 0);
		GetParent()->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);	
		g_bDonotClose = FALSE;
		l_lastchoicezb=m_ChoiceGPIndex_Combo.GetCurSel();
	}
	else	//���ȡ��ִ��ѡ��
		IsCarrying = FALSE;
}

#include "DescZBDlg.h"
void CConditionGPDlg::OnIndexInfomation() 
{
	if(CurrentIndex >= 0)
	{
		CDescZBDlg dlg;
		TINDEXINFO	* pTJIndex = g_pCalc->_CallDTopViewCalc_8(TJ_TYPE,(long)CurrentIndex);
		if(!pTJIndex) return;
		dlg.Init(TJ_TYPE, pTJIndex->acCode);
		dlg.DoModal();
	}
}

void CConditionGPDlg::OnAddcondition() 
{
	float fCurrParam[MAX_PARAMNUM];
	if(CurrentIndex < 0)	
		return;
	if(ConditionNum >= MAX_TJCOMB_NUM)
	{
		TDX_MessageBox(m_hWnd,"����̫�࣬�����ټ���!",MB_OK+MB_ICONINFORMATION);
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

	int nSelCycle = m_CalcCycle_Combo.GetCurSel();

	CString tempstr = "";
	int ParamNum = (*g_pCalc)[TJ_TYPE][CurrentIndex].nParaNum;
	tempstr += "(";
	for(i=0;i<ParamNum;i++)
	{
		tempstr += m_strParam[i];
		if(i<ParamNum-1) tempstr += ",";
	}
	tempstr += ")";

	tempstr = (CString)(*g_pCalc)[TJ_TYPE][CurrentIndex].acCode + " " + tempstr;
	CString tempstr2;
	m_CalcCycle_Combo.GetLBText(nSelCycle,tempstr2);
	tempstr = tempstr + " " + tempstr2;
	m_Condition_List.AddString (tempstr);	

	CONDITIONINFO tmpCon;
	memset(&tmpCon, 0, sizeof(CONDITIONINFO));
	strcpy(tmpCon.acCode , (*g_pCalc)[TJ_TYPE][CurrentIndex].acCode);
	tmpCon.ParamNum = ParamNum ;
	tmpCon.CalcCycle = nSelCycle;
	
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
	if(ConditionNum > 1)
	{
		GetDlgItem(IDC_ADD_RADIO)->EnableWindow(TRUE);
		GetDlgItem(IDC_OR_RADIO)->EnableWindow(TRUE);
	}
}

void CConditionGPDlg::OnDeletecondition()
{
	int nSelCondition = m_Condition_List.GetCurSel();
	if(nSelCondition != LB_ERR)
	{
		m_Condition_List.DeleteString (nSelCondition);

		for(int i = nSelCondition+1; i < ConditionNum;i++)
			ConditionInfo[i-1] = ConditionInfo[i];
		ConditionNum--;
		if(ConditionNum <= 1)
		{
			GetDlgItem(IDC_ADD_RADIO)->EnableWindow(FALSE);
			GetDlgItem(IDC_OR_RADIO)->EnableWindow(FALSE);
		}
		m_Condition_List.SetCurSel(max(nSelCondition-1,0));
	}
}

//��ʼִ��
void CConditionGPDlg::OnCarryout()
{
	CarryoutToBlock(-1);//ȱʡΪ����ѡ��
}

void CConditionGPDlg::CarryoutToBlock(int WhichBlock)
{
	UpdateData(TRUE);
	int		nCurrPeriod;
	long	time1, time2;
	BOOL	bQJ = m_bQJ;
	if(ConditionNum < 1)
	{
		TDX_MessageBox(m_hWnd,"�����ѡ������!",MB_OK|MB_ICONINFORMATION);
		return;
	}
	if (bQJ && m_BeginTime > m_EndTime)
	{
		TDX_MessageBox(m_hWnd,"��ʼ����Ӧ����������֮ǰ!",MB_OK|MB_ICONINFORMATION);
		return;
	}
	nCurrPeriod = m_CalcCycle_Combo.GetCurSel();
	if(nCurrPeriod==MIN1_ST||nCurrPeriod==MINN_ST)
	{
		TDX_MessageBox(m_hWnd,"�ݲ�֧�ִ�����,��ѡ����������!",MB_OK|MB_ICONINFORMATION);
		return;
	}
	if(!CanProcesLocalData(this, nCurrPeriod<DAY_ST))
		return;
	int i,j,k;
	BOOL bFind;
	char acCode[ZBCODE_LEN+1];
	short TjIndexNo[MAX_TJCOMB_NUM];
	memset(TjIndexNo,0,MAX_TJCOMB_NUM*sizeof(short));

	for (i = 0; i < ConditionNum; i++ )
	{
		strcpy(acCode, ConditionInfo[i].acCode);
		bFind = FALSE;
		TINDEXINFO	* pTJIndex = g_pCalc->_CallDTopViewCalc_8(TJ_TYPE,(long)0);
		for(j = 0;j < g_pCalc->_CallDTopViewCalc_7(TJ_TYPE);j++)//����ǴӴ����ļ���ȡ�õ���������Ӧ����з������
		{
			if ( strcmp(acCode,pTJIndex[j].acCode) == 0 )
			{	
				TjIndexNo[i] = j;
				for ( k = 0; k < min(pTJIndex[TjIndexNo[i]].nParaNum,ConditionInfo[i].ParamNum); k++)
					pTJIndex[TjIndexNo[i]].aPara[k].nValue[nCurrPeriod] = ConditionInfo[i].ParamValue[k];
				bFind = TRUE;
				break;
			}
		}
		//ֻҪ��һ��ָ��û���ҵ������˳�
		if(!bFind)
		{
			TDX_MessageBox(m_hWnd,"����������в����ڵ�����!",MB_OK|MB_ICONEXCLAMATION);
			CanClearList = TRUE; //�����������⣬Ӧ���
			return;
		}
	}

	//������ȡ�����з��������Ĺ�Ʊ
	int TotalStockNum = FiltMulGpDomain(m_GpDomain,m_nSelDomain,awUsedStkp);
	if(TotalStockNum <= 0)
	{
		TDX_MessageBox(m_hWnd,"��ѡ��ѡ�ɷ�Χ!��ѡ�ɷ�Χ��û�й�Ʊ!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	m_TotalGPNum.SetWindowText(IntToStr(TotalStockNum));
	m_SelectedNum.SetWindowText("0");

	//���ý�����
	m_Carry_Progress.SetRange32(0,TotalStockNum);
	m_Carry_Progress.SetStep(1);
	m_Carry_Progress.SetPos(0);

	IsCarried = FALSE;
	IsCarrying = TRUE;	//����ѡ�ɱ�־
	DisableOther();
	//��Disable����֮������һ�ؼ�Ϊ����,����ESC��������
	GetDlgItem(IDCANCEL)->SetFocus();
	RefreshAllHq();
	g_bDonotClose = TRUE;
	m_CStatus.SetText(_F("���ڽ���ѡ��,���Ե�... "));

	int SuccessNum = 0,ReadNum;
	int SuccessFlag;					//0:���� 1:�ɹ�	-1:ʧ�� -2:�׶�����
	CString tempStr;

	float * fPut[MAX_LINEOUT];
	memset(fPut,0,MAX_LINEOUT*sizeof(float*));
	time1=TimeToLong(m_BeginTime);
	time2=TimeToLong(m_EndTime);
	MemStkInfo *pStkInfo;
	for ( i = 0; i < TotalStockNum; i++ )
	{
		if( i % 2 == 0 )
		{
			PeekMsg();
			if( !IsCarrying)
			{
				m_Carry_Progress.SetPos(0);
				SetDlgItemText(IDC_SELECTED_NUM,"");
				m_CStatus.SetWindowText("");
				break;
			}
		}
		pStkInfo = (*g_pStockIO)[awUsedStkp[i]];
		SuccessFlag = 0;
		int nCalcPer = 0,nPer,nTmp;
		struct CurrStockData hqtmp;
		struct AnalyData * pAnalyDataTmp=NULL;
		while(pStkInfo && pStkInfo->Close>COMPPREC && nCalcPer<ConditionNum)
		{
			nPer = ConditionInfo[nCalcPer].CalcCycle;			
			ReadNum = g_pCalc->_CallDTopViewCalc_4(pStkInfo->Code,pStkInfo->setcode,CalcPeriodToTdxPeriod(nPer), ASK_ALL, m_bTQ?1:0,m_bDynaK?DYNAKTYPE:0);
			if(ReadNum > 0)
			{
				if(bQJ)
				{
					pAnalyDataTmp = new AnalyData[ReadNum];
					ReadNum = g_pStockIO->ReadAnalyDataAndCurrHQ(pStkInfo,pAnalyDataTmp,ReadNum,CalcPeriodToTdxPeriod(nPer),&hqtmp,nTmp,GetMulNum(CalcPeriodToTdxPeriod(nPer)));			
				}
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
						if(!bQJ)
						{
							if ((TBE_MEANLESS(fPut[0][ReadNum-1]) != 0) && fabs(fPut[0][ReadNum-1]) > 0.1 )
							{
								if ( m_IsAndRadio == 1 )	//������
								{	
									SuccessFlag = 1;		//�ɹ�
									break;
								}
							}
							else
							{
								if ( m_IsAndRadio == 0 )	//������
								{		
									SuccessFlag = -1;		//ʧ��
									break;
								}
							}
						}
						else
						{
							BOOL bSingleSuc=FALSE;
							for(k=0;k<ReadNum;k++)
							{
								if (pAnalyDataTmp[k].Time.Date>=time1 && pAnalyDataTmp[k].Time.Date<=time2)
								{
									if((TBE_MEANLESS(fPut[0][k]) != 0) && fabs(fPut[0][k]) > 0.1)
									{
										if ( m_IsAndRadio == 1 )	//������
										{	
											SuccessFlag = 1;		//�ɹ�
											break;
										}
										bSingleSuc=TRUE;
										SuccessFlag=-2;
									}
								}
							}
							if(k>=ReadNum)
							{
								if ( !bSingleSuc && m_IsAndRadio == 0 )	//������
								{		
									SuccessFlag = -1;		//ʧ��
									break;
								}
							}
							if(SuccessFlag==1 || SuccessFlag==-1) 
								break;
						}
					}
				}
				TDEL(pAnalyDataTmp);
				if(SuccessFlag==1 || SuccessFlag==-1) break;
				nCalcPer = j;
			}
			else break;
		}
		if (!bQJ && SuccessFlag==0 && nCalcPer >= ConditionNum && m_IsAndRadio == 0 )
			SuccessFlag = 1;
		if (bQJ && SuccessFlag==-2 && nCalcPer >= ConditionNum && m_IsAndRadio == 0 )
			SuccessFlag = 1;
		if ( SuccessFlag == 1 )		//��ѡ�е����
		{	
			SelectedStk[SuccessNum] = awUsedStkp[i];
			SuccessNum++;
			tempStr.Format("%d/%.1f%%",SuccessNum,SuccessNum*100.0/TotalStockNum);
			m_SelectedNum.SetWindowText(tempStr);
		}
		//������ǰ��һ��
		m_Carry_Progress.StepIt();
	}
	g_pCalc->_CallDTopViewCalc_14(fPut[0]);
	if(IsCarrying)
	{
		m_CStatus.SetText(_F("ѡ�����. "));
		MakeBlock(SelectedStk,SuccessNum,WhichBlock);//���ɰ�鲢��ʾ���
		CanClearList = TRUE;
		IsCarrying = FALSE;
	}
	EnableOther();
	g_bDonotClose = FALSE;
	IsCarried = TRUE;
	return;
}

void CConditionGPDlg::DisableOther()
{
	m_ChoiceGPIndex_Combo.EnableWindow(FALSE);
	GetDlgItem(IDC_ADDCONDITION)->EnableWindow(FALSE);
	GetDlgItem(IDC_DELETECONDITION)->EnableWindow(FALSE);
	GetDlgItem(IDC_LOADCONDITION)->EnableWindow(FALSE);
	GetDlgItem(IDC_SAVECONDITION)->EnableWindow(FALSE);
	bRadioEnable = GetDlgItem(IDC_ADD_RADIO)->EnableWindow(FALSE);
	GetDlgItem(IDC_OR_RADIO)->EnableWindow(FALSE);

	GetDlgItem(IDC_CHANGEDOMAIN_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_CALCCYCLE_COMBO)->EnableWindow(FALSE);
	GetDlgItem(IDC_CARRYOUT)->EnableWindow(FALSE);
	GetDlgItem(IDC_CARRYOUT_BLOCK)->EnableWindow(FALSE);
	GetDlgItem(IDC_TQ_CHECK)->EnableWindow(FALSE);
	SetDlgItemText(IDCANCEL,_F("ֹͣѡ��"));
}

void CConditionGPDlg::EnableOther()
{
	m_ChoiceGPIndex_Combo.EnableWindow(TRUE);
	GetDlgItem(IDC_ADDCONDITION)->EnableWindow(TRUE);
	GetDlgItem(IDC_DELETECONDITION)->EnableWindow(TRUE);
	GetDlgItem(IDC_LOADCONDITION)->EnableWindow(TRUE);
	GetDlgItem(IDC_SAVECONDITION)->EnableWindow(TRUE);
	if(!bRadioEnable)
	{
		GetDlgItem(IDC_ADD_RADIO)->EnableWindow(TRUE);
		GetDlgItem(IDC_OR_RADIO)->EnableWindow(TRUE);
	}
	GetDlgItem(IDC_CHANGEDOMAIN_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_CALCCYCLE_COMBO)->EnableWindow(TRUE);
	GetDlgItem(IDC_CARRYOUT)->EnableWindow(TRUE);
	GetDlgItem(IDC_CARRYOUT_BLOCK)->EnableWindow(TRUE);
	GetDlgItem(IDC_TQ_CHECK)->EnableWindow(TRUE);
	SetDlgItemText(IDCANCEL,_F("�ر�"));
}

#include "InputName.h"
void CConditionGPDlg::OnSavecondition()
{
	UpdateData(TRUE);
	if(ConditionNum < 1) 
	{
		TDX_MessageBox(m_hWnd,"ѡ�������б���������,���ȼ�������",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	CInputName dlg;
	dlg.Caption = "��������";
	dlg.HintInfo = "����������ļ���(��д��չ��):";
	//�Զ�Ϊ�û�����һ���ļ���
	CString tempStr = "";
	for(int i = 0; i < ConditionNum; i++)
	{
		if(i == ConditionNum-1)
			tempStr = tempStr + ConditionInfo[i].acCode;
		else
			tempStr = tempStr + ConditionInfo[i].acCode + "_";
	}
	tempStr.MakeLower();
	dlg.m_NewName = tempStr;
	if( dlg.DoModal() == IDOK )
	{
		TrimString(dlg.m_NewName);
		if(dlg.m_NewName == "")	return;
		//����������
		CString tempFileName = g_WSXHStr+dlg.m_NewName+".cos";
		CFileException ex;
		CFile m_File;
		if( m_File.Open(tempFileName,CFile::modeCreate|CFile::modeWrite,&ex) )
		{
			FileHead.ConditionNum = ConditionNum;
			FileHead.nDate = TimeToLong(m_EndTime);
			m_File.Write(&FileHead,sizeof(struct FILEHEAD));
			for(int i = 0; i < ConditionNum ;i++)
				m_File.Write(&ConditionInfo[i],sizeof(struct CONDITIONINFO));
			m_File.Close();
		}
		else
		{
			char szError[1024];
			ex.GetErrorMessage(szError, 1024);
			TDX_MessageBox(m_hWnd,"�ļ���������:\n"+(CString)szError,MB_OK+MB_ICONEXCLAMATION);
		}
	}
}

#include "ConditionLoadDlg.h"
void CConditionGPDlg::OnLoadcondition() //װ������
{
	CConditionLoadDlg dlg;
	if( dlg.DoModal () == IDOK )
	{
		if(dlg.ReturnStr != "")
		{
			CString tempFileName = g_WSXHStr+dlg.ReturnStr;
			CFile m_File;
			if( m_File.Open(tempFileName,CFile::modeRead|CFile::shareDenyNone) )
			{
				//�����ļ�ͷ
				m_File.Read(&FileHead,sizeof(struct FILEHEAD));
				m_IsAndRadio = (FileHead.m_IsAndRadio == 0)?0:1;
				ConditionNum = FileHead.ConditionNum;
				CString tempStr;
				m_Condition_List.ResetContent ();	//�Ƚ��б����
				for(int i = 0; i < ConditionNum ;i++)
				{
					//�����������
					m_File.Read(&ConditionInfo[i],sizeof(struct CONDITIONINFO));
					//�����б���
					tempStr = (CString)ConditionInfo[i].acCode + " ";
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
				//װ��ɹ���,�Խ�����һЩ�Ĵ���
				if(ConditionNum > 1)
				{
					GetDlgItem(IDC_ADD_RADIO)->EnableWindow(TRUE);
					GetDlgItem(IDC_OR_RADIO)->EnableWindow(TRUE);
				}
				UpdateData(FALSE);
				//
			}
			else
				TDX_MessageBox(m_hWnd,"���ܴ����ļ���",MB_OK+MB_ICONEXCLAMATION);
		}
	}
	return;
}

void CConditionGPDlg::OnSelchangeChoicegpindexCombo() 
{
	int nCurSel = m_ChoiceGPIndex_Combo.GetCurSel();
	if(nCurSel != CurrentIndex)
	{
		int		nCurrPeriod = m_CalcCycle_Combo.GetCurSel();
		CurrentIndex = nCurSel;
		m_Form.HideSelf();
		m_Form.SetStatus(TJ_TYPE, CurrentIndex,nCurrPeriod);
		if(CanClearList)
		{
			m_Condition_List.ResetContent();
			ConditionNum = 0;
			CanClearList = FALSE;
		}
	}
}

#include "GetBlockDlg.h"
void CConditionGPDlg::OnCarryoutBlock() 
{
	CGetBlockDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		if(HasGPInBlockAndContinue(dlg.nSelBlock))
			CarryoutToBlock(dlg.nSelBlock);
	}
}

void CConditionGPDlg::OnSelchangeCalccycleCombo() 
{
	int nSel = m_CalcCycle_Combo.GetCurSel();
	if (nSel != DAY_ST && nSel != WEEK_ST && nSel != MONTH_ST && nSel != DAYN_ST && nSel != SEASON_ST && nSel != YEAR_ST)
	{
		m_bQJ = FALSE;
		UpdateData(FALSE);
	}
	//��������m_Form��״̬
	m_Form.HideSelf();
	m_Form.SetStatus(TJ_TYPE, CurrentIndex, nSel);
	OnQjCheck();
	//���������,��,���ߣ�����ʾ��ȷ��Ȩ
	int nTdxPeriod = CalcPeriodToTdxPeriod(nSel);
	BOOL bAvailTQ = (nTdxPeriod==PER_DAY || nTdxPeriod==PER_WEEK || nTdxPeriod==PER_MONTH || nTdxPeriod==PER_DAYN || nTdxPeriod==PER_SEASON || nTdxPeriod==PER_YEAR);
	GetDlgItem(IDC_TQ_CHECK)->ShowWindow(bAvailTQ?SW_SHOW:SW_HIDE);
}

#include "ChangeDomainDlg.h"
void CConditionGPDlg::OnChangedomainButton() 
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

void CConditionGPDlg::OnQjCheck() 
{
	UpdateData(TRUE);
	if(m_bQJ)
		TDX_MessageBox(m_hWnd,"��ʷ�׶�ѡ�ɵĺ�����:\r\n��������õ���ʷʱ�����������(����ֻ�ǵ�������),�ͽ��˹�Ʊѡ��.",MB_OK|MB_ICONINFORMATION);
	GetDlgItem(IDC_DATETIMEPICKER1)->EnableWindow(m_bQJ);
	GetDlgItem(IDC_DATETIMEPICKER2)->EnableWindow(m_bQJ);	
}
