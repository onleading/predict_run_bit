// MySelGpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "GridView.h"
#include "MainFrm.h"
#include "MySelGpDlg.h"
#include "MyStrategy.h"
#include "Znxg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const char *g_name[5] = {"ָ��ѡ��", "���²���ѡ��", "ʵʱ����ѡ��" ,"����ѡ��", "K��ѡ��"};

const char *g_procName[] = {"����","����","С��","����Ϊ","����ǰ","������",
"�ϴ�","����","����","�µ�","�Ϲ�","�¹�",NULL};                 //����仯��Ӧ������������

static char *CleConTabStr[] = {"�ͽ���ʽ","�߽���ʽ","�߳���ʽ","�ͳ���ʽ",NULL};
/////////////////////////////////////////////////////////////////////////////
// CMySelGpDlg dialog

CMySelGpDlg::CMySelGpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMySelGpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMySelGpDlg)
	m_endTime = 0;
	m_otherLine = _T("");
	m_isAndRadio = -1;
	m_dynaStatic = _T("");
	m_bTQ = TRUE;
	//}}AFX_DATA_INIT
	m_nCurType=-1;
	m_nCurIndex=-1;
	m_bSort=FALSE;
	m_pMyStrategy = NULL;
	m_pCurSiCon = NULL;
	m_pCurIndexInfo = NULL;
}

CMySelGpDlg::~CMySelGpDlg()
{
	delete m_pMyStrategy;m_pMyStrategy=NULL;
	delete m_pCurSiCon;m_pCurSiCon=NULL;
}

void CMySelGpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMySelGpDlg)
	DDX_Control(pDX, IDC_TAB1, m_tabctrl);
	DDX_Control(pDX, IDC_STATUS, m_status);
	DDX_Control(pDX, IDC_STATICPARAM, m_outStatic);
	DDX_Control(pDX, IDC_SELECTED_NUM, m_selectedNum);
	DDX_Control(pDX, IDC_CALCCYCLE_COMBO, m_CalcCycle_Combo);
	DDX_Control(pDX, IDC_CARRY_PROGRESS, m_Carry_Progress);
	DDX_Control(pDX, IDC_ZBTREE, m_zbTree);
	DDX_Control(pDX, IDC_TOTALGP_NUM, m_totalGpNum);
	DDX_Control(pDX, IDC_PROC, m_calcProc);
	DDX_Control(pDX, IDC_MAINLINE, m_mainLine);
	DDX_Control(pDX, IDC_CONDITION_LIST, m_Condition_List);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_endTime);
	DDX_CBString(pDX, IDC_OTHERLINE, m_otherLine);
	DDX_Radio(pDX, IDC_ADD_RADIO, m_isAndRadio);
	DDX_Text(pDX, IDC_STATICDYNA, m_dynaStatic);
	DDX_Check(pDX, IDC_TQ_CHECK, m_bTQ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMySelGpDlg, CDialog)
	//{{AFX_MSG_MAP(CMySelGpDlg)
	ON_BN_CLICKED(IDC_ADDCONDITION, OnAddcondition)
	ON_BN_CLICKED(IDC_CARRYOUT, OnCarryout)
	ON_BN_CLICKED(IDC_CARRYOUT_BLOCK, OnCarryoutBlock)
	ON_BN_CLICKED(IDC_CHANGEDOMAIN_BUTTON, OnChangedomainButton)
	ON_BN_CLICKED(IDC_DELETECONDITION, OnDeletecondition)
	ON_BN_CLICKED(IDC_INDEX_INFOMATION, OnIndexInfomation)
	ON_BN_CLICKED(IDC_LOADCONDITION, OnLoadcondition)
	ON_BN_CLICKED(IDC_SAVECONDITION, OnSavecondition)
	ON_CBN_SELCHANGE(IDC_CALCCYCLE_COMBO, OnSelchangeCalccycleCombo)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(TVN_SELCHANGED, IDC_ZBTREE, OnSelchangedZbtree)
	ON_CBN_SELCHANGE(IDC_PROC, OnSelchangeProc)
	ON_CBN_SELCHANGE(IDC_MAINLINE, OnSelchangeMainline)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelchangeTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMySelGpDlg::SetDomainStr()
{
	CString tmpStr = "";
	for(int i=0;i < m_nSelDomain;i++)
		tmpStr+=m_GpDomain[i].Name+(CString)" ";
	GetDlgItem(IDC_DOMAIN_STATIC)->SetWindowText(_F(tmpStr));
}

void CMySelGpDlg::FillTree(int nTab)
{
	HTREEITEM htm, StyleHtm, ChildHtm;
	int tnum, i,j,k;
	CString strTmp;
	LockWindowUpdate();
	m_zbTree.DeleteAllItems();

	for (tnum = 0; tnum < 5; tnum++)
	{
		htm = m_zbTree.InsertItem(_F(g_name[tnum]), 2, 2);
		m_zbTree.SetItemData(htm, MAKELONG(-1, tnum)); //�����          
		if (strcmp(g_name[tnum], "ָ��ѡ��")==0)           //ָ��
		{
			int nZBTypeNum = g_pCalc->_CallDTopViewCalc_6(NULL, ZB_TYPE);
			TYPEX *paZBTypeInfo = new TYPEX[nZBTypeNum];
			typeinfo *paZBTreeInfo = new typeinfo[nZBTypeNum]; 
			memset(paZBTypeInfo, 0, nZBTypeNum*sizeof(TYPEX));
			memset(paZBTreeInfo, 0, nZBTypeNum*sizeof(typeinfo));
			int num = g_pCalc->_CallDTopViewCalc_6(paZBTypeInfo, ZB_TYPE);
			ASSERT(num==nZBTypeNum);
			for(i=0;i<nZBTypeNum;i++)
			{
				typeinfo TypeInfo;
				TypeInfo.nType = paZBTypeInfo[i].nTypeID;
				strcpy(TypeInfo.name, paZBTypeInfo[i].lpszName);
				TypeInfo.htm = 0;
				paZBTreeInfo[i]=TypeInfo;
			}
			if(nTab!=1)
			{
				for(i=0;i<nZBTypeNum;i++)
				{
					if(paZBTypeInfo[i].nRootType == ZB_TYPE)
					{
						paZBTreeInfo[i].htm = m_zbTree.InsertItem(_F(paZBTypeInfo[i].lpszName), 1, 1, htm);	
						m_zbTree.SetItemData(paZBTreeInfo[i].htm, MAKELONG(ZB_TYPESEL, paZBTreeInfo[i].nType));
					}
					else
						for(j=0;j<nZBTypeNum;j++)
						{
							if(paZBTypeInfo[i].nRootType == paZBTypeInfo[j].nTypeID && paZBTreeInfo[j].htm)
							{
								paZBTreeInfo[i].htm = m_zbTree.InsertItem(_F(paZBTypeInfo[i].lpszName), 1, 1, paZBTreeInfo[j].htm);
								m_zbTree.SetItemData(paZBTreeInfo[i].htm, MAKELONG(ZB_TYPESEL, paZBTreeInfo[i].nType));
							}
						}
				}
			}
			int nZBNum=g_pCalc->_CallDTopViewCalc_7(ZB_TYPE);
			for(k=0;k<nZBNum;k++)
			{
				strTmp.Format("%s %s", (*g_pCalc)[ZB_TYPE][k].acCode, (*g_pCalc)[ZB_TYPE][k].acName);
				if(nZBTypeNum==0 || nTab==1)
				{
					ChildHtm = m_zbTree.InsertItem(_F(strTmp), 0, 0, htm);			
					m_zbTree.SetItemData(ChildHtm, MAKELONG(ZB_TYPE, k));	//��item��data�ĵ�λ����ţ���λ�����
				}
				else
				{
					for(j=0;j<nZBTypeNum;j++)
					{
						if((*g_pCalc)[ZB_TYPE][k].nType==paZBTypeInfo[j].nTypeID)
						{
							BOOL bSystem=(*g_pCalc)[ZB_TYPE][k].nDelStatus & SYSTEM_INDEX;
							if(nTab==0 || (nTab==2 && bSystem) || (nTab==3 && !bSystem))
							{
								ChildHtm = m_zbTree.InsertItem(_F(strTmp), 0, 0, paZBTreeInfo[j].htm);
								m_zbTree.SetItemData(ChildHtm, MAKELONG(ZB_TYPE, k));
							}
						}
					}
				}
			}
			TDELA(paZBTypeInfo);
			TDELA(paZBTreeInfo);
		}

		else if(tnum == 1 && nTab!=3)									        //����
		{
			for(int k = 0;;k++)
			{
				if(g_nCustCon[k].topic[0] == 0)
					break;
				if(g_nCustCon[k].ConType != 0)
					continue;
				ChildHtm = m_zbTree.InsertItem(g_nCustCon[k].topic, 0, 0, htm, TVI_LAST);
				m_zbTree.SetItemData(ChildHtm, MAKELONG(CWHQ_SEL, g_nCustCon[k].MsgNo));
			}
		}

		else if(tnum == 2 && nTab!=3)											//����
		{
			for(int k = 0;;k++)
			{
				if(g_nCustCon[k].topic[0] == 0)
					break;
				if(g_nCustCon[k].ConType != 1)
					continue;
				ChildHtm = m_zbTree.InsertItem (g_nCustCon[k].topic, 0, 0, htm, TVI_LAST);
				m_zbTree.SetItemData(ChildHtm, MAKELONG(CWHQ_SEL, g_nCustCon[k].MsgNo));
			}
		}

		else if (strcmp(g_name[tnum], "����ѡ��")==0)          //����
		{
			int nTJTypeNum = g_pCalc->_CallDTopViewCalc_6(NULL, TJ_TYPE);
			TYPEX *paTJTypeInfo = new TYPEX[nTJTypeNum];
			typeinfo *paTJTreeInfo = new typeinfo[nTJTypeNum]; 
			memset(paTJTypeInfo, 0, nTJTypeNum*sizeof(TYPEX));
			memset(paTJTreeInfo, 0, nTJTypeNum*sizeof(typeinfo));
			int num = g_pCalc->_CallDTopViewCalc_6(paTJTypeInfo, TJ_TYPE);
			ASSERT(num==nTJTypeNum);
			for(i=0;i<nTJTypeNum;i++)
			{
				typeinfo TypeInfo;
				TypeInfo.nType = paTJTypeInfo[i].nTypeID;
				strcpy(TypeInfo.name, paTJTypeInfo[i].lpszName);
				TypeInfo.htm = 0;
				paTJTreeInfo[i]=TypeInfo;
			}
			if(nTab!=1)
			{
				for(i=0;i<nTJTypeNum;i++)
				{
					if(paTJTypeInfo[i].nRootType == TJ_TYPE)
					{
						paTJTreeInfo[i].htm = m_zbTree.InsertItem(_F(paTJTypeInfo[i].lpszName), 1, 1, htm);	
						m_zbTree.SetItemData(paTJTreeInfo[i].htm, MAKELONG(TJ_TYPESEL, paTJTreeInfo[i].nType));
					}
					else
						for(j=0;j<nTJTypeNum;j++)
						{
							if(paTJTypeInfo[i].nRootType == paTJTypeInfo[j].nTypeID && paTJTreeInfo[j].htm)
							{
								paTJTreeInfo[i].htm = m_zbTree.InsertItem(_F(paTJTypeInfo[i].lpszName), 1, 1, paTJTreeInfo[j].htm);
								m_zbTree.SetItemData(paTJTreeInfo[i].htm, MAKELONG(TJ_TYPESEL, paTJTreeInfo[i].nType));
							}
						}
				}
			}
			int nTJNum=g_pCalc->_CallDTopViewCalc_7(TJ_TYPE);
			for(k=0;k<nTJNum;k++)
			{
				strTmp.Format("%s %s", (*g_pCalc)[TJ_TYPE][k].acCode, (*g_pCalc)[TJ_TYPE][k].acName);
				if(nTJTypeNum==0 || nTab==1)
				{
					ChildHtm = m_zbTree.InsertItem(_F(strTmp), 0, 0, htm);			
					m_zbTree.SetItemData(ChildHtm, MAKELONG(TJ_TYPE, k));	//��item��data�ĵ�λ����ţ���λ�����
				}
				else
					for(j=0;j<nTJTypeNum;j++)
					{
						if((*g_pCalc)[TJ_TYPE][k].nType==paTJTypeInfo[j].nTypeID)
						{
							BOOL bSystem=(*g_pCalc)[TJ_TYPE][k].nDelStatus & SYSTEM_INDEX;
							if(nTab==0 || (nTab==2 && bSystem) || (nTab==3 && !bSystem))
							{
								ChildHtm = m_zbTree.InsertItem(_F(strTmp), 0, 0, paTJTreeInfo[j].htm);
								m_zbTree.SetItemData(ChildHtm, MAKELONG(TJ_TYPE, k));
							}
						}
					}
			}
			TDELA(paTJTypeInfo);
			TDELA(paTJTreeInfo);
		}

		else if(tnum == 4 && nTab!=3)											//K��
		{
			int nZBNum=g_pCalc->_CallDTopViewCalc_7(KL_TYPE);
			for(k=0;k<nZBNum;k++)
			{
				char tmp[64];
				strcpy(tmp, (*g_pCalc)[KL_TYPE][k].acCode);
				strcat(tmp, "��");
				strcat(tmp,(*g_pCalc)[KL_TYPE][k].acName);
				ChildHtm = m_zbTree.InsertItem(_F(tmp), 0, 0, htm, TVI_LAST);
				m_zbTree.SetItemData(ChildHtm, MAKELONG(KL_SEL, k));
			}
		}

		else if(tnum == 5 && nTab!=3)											//��ʽѡ��
		{
			for (j = 0; CleConTabStr[j]!=NULL; j++) 
			{
				StyleHtm = m_zbTree.InsertItem(_F(CleConTabStr[j]),1,1,htm);
				m_zbTree.SetItemData(StyleHtm, MAKELONG(-20, j));
				CLE_CONTABLE *tmpTable;
				switch(j)
				{
				case 0:	tmpTable = g_pCleKLine1;break;
				case 1:	tmpTable = g_pCleKLine2;break;
				case 2:	tmpTable = g_pCleKLine3;break;
				case 3:	tmpTable = g_pCleKLine4;break;
				case 4:	tmpTable = g_pCleJBM;	break;
				default:tmpTable = NULL;		break;
				}
				TVINSERTSTRUCT tviItem;
				tviItem.hParent = StyleHtm;
				tviItem.hInsertAfter = TVI_LAST;
				tviItem.item.mask = TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;

				for(int k = 0;;k++)
				{
					tviItem.item.lParam = k;
					if(!tmpTable || tmpTable[k].topic[0] == 0)
						break;
					tviItem.item.pszText = _F(tmpTable[k].topic);
					tviItem.item.iImage = 0;
					tviItem.item.iSelectedImage = 0;
					ChildHtm = m_zbTree.InsertItem (&tviItem);
					m_zbTree.SetItemData(ChildHtm, MAKELONG(STYLE_SEL, 100*j+k));
				}
			}
		}
	}
	UnlockWindowUpdate();
}

void CMySelGpDlg::ShowTypeInfo()
{
	m_Form.ShowWindow(SW_HIDE);
	for(int i=0;i<3;i++)
	{
		GetDlgItem(IDC_MAINLINE+i)->ShowWindow(SW_HIDE);
	}
}

void CMySelGpDlg::ShowConInfo()		
{
	if(m_nCurType==ZB_SEL||m_nCurType==TJ_TYPE||m_nCurType==KL_SEL)     //����
	{
		m_Form.ShowWindow(SW_SHOW);
		int	nCurrPeriod = m_CalcCycle_Combo.GetCurSel();
		m_Form.HideSelf();
		m_Form.SetStatus(m_nCurType, m_nCurIndex, nCurrPeriod);
	}

	if(m_nCurType==ZB_SEL||m_nCurType==CWHQ_SEL)                        //����
	{
		for(int i=1;i<3;i++)
		{
			GetDlgItem(IDC_MAINLINE+i)->ShowWindow(SW_SHOW);
		}
	}
	if(m_nCurType==ZB_SEL)
		GetDlgItem(IDC_MAINLINE)->ShowWindow(SW_SHOW);
}

void CMySelGpDlg::FillConInfo()
{
	if(m_nCurType!=0&&m_nCurType!=2) return;
	int i;
	m_mainLine.ResetContent();
	m_calcProc.ResetContent();
	CComboBox *otherLine=(CComboBox *)GetDlgItem(IDC_OTHERLINE);
	otherLine->ResetContent();
	if(m_nCurType==ZB_SEL&&m_pCurIndexInfo)
	{
		if(m_pCurIndexInfo->nDrawMode==OnlyMain||m_pCurIndexInfo->nDrawMode==KOverlay)
		{
			m_mainLine.AddString(_F("��߼�"));
			m_mainLine.AddString(_F("���̼�"));
			m_mainLine.AddString(_F("��ͼ�"));
			m_mainLine.AddString(_F("���̼�"));
		}
		for(i=0;i<m_pCurIndexInfo->nLineNum;i++)
		{
			m_mainLine.AddString(m_pCurIndexInfo->aLine[i].acLineName);
		}
	}
	for(i=0;g_procName[i]!=NULL;i++)
	{
		m_calcProc.AddString(_F(g_procName[i]));
	}
	if(m_nCurType==CWHQ_SEL)
	{
		for(i=6;i<12;i++)
			m_calcProc.DeleteString(6);
	}

	m_mainLine.SetCurSel(0);
	m_calcProc.SetCurSel(0);
	
	RefreshOther();
}

void CMySelGpDlg::EnableOther()
{
	m_zbTree.EnableWindow(TRUE);
	ShowConInfo();
	GetDlgItem(IDC_DATETIMEPICKER2)->EnableWindow(TRUE);
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
	GetDlgItem(IDC_STATICDYNA)->ShowWindow(SW_SHOW);
	SetDlgItemText(IDCANCEL,_F("�ر�"));
}

void CMySelGpDlg::DisableOther()
{
	m_zbTree.EnableWindow(FALSE);
	ShowTypeInfo();
	GetDlgItem(IDC_DATETIMEPICKER2)->EnableWindow(FALSE);
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
	GetDlgItem(IDC_STATICDYNA)->ShowWindow(SW_HIDE);
	SetDlgItemText(IDCANCEL,_F("ֹͣѡ��"));
}

void CMySelGpDlg::DynaShow()
{
	if(m_nCurType!=CWHQ_SEL) return;
	for(int k=0;;k++)
	{
		if(m_nCurIndex==g_nCustCon[k].MsgNo)
		{
			m_dynaStatic= g_nCustCon[k].UnitStr;
			break;
		}
	}
}

void CMySelGpDlg::CarryoutToBlock(int WhichBlock)
{
	int		nCurrPeriod;
	NTime	time1, time2;

	UpdateData(TRUE);
	if(ConditionNum < 1)
	{
		TDX_MessageBox(m_hWnd,"���趨ѡ������!",MB_OK|MB_ICONINFORMATION);
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
	m_pMyStrategy->SetIsAndRatio(m_isAndRadio);
	int i;
	short TjIndexNo[MAX_TJCOMB_NUM];
	memset(TjIndexNo,0,MAX_TJCOMB_NUM*sizeof(short));

	//������ȡ�����з��������Ĺ�Ʊ
	int TotalStockNum = FiltMulGpDomain(m_GpDomain,m_nSelDomain,awUsedStkp);
	if(TotalStockNum <= 0)
	{
		TDX_MessageBox(m_hWnd,"��ѡ��ѡ�ɷ�Χ!��ѡ�ɷ�Χ��û�й�Ʊ!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	m_totalGpNum.SetWindowText(IntToStr(TotalStockNum));
	if(m_bSort) m_selectedNum.SetWindowText("");
	else m_selectedNum.SetWindowText("0");

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
	m_status.SetWindowText(_F("���ڽ���ѡ��,���Ե�... "));

	memset(SelectedStk, -1, MAXGPNUM*sizeof(short));
	int SuccessNum = 0;
	BOOL SuccessFlag;
	CString tempStr;

	float * fPut[MAX_LINEOUT];
	memset(fPut,0,MAX_LINEOUT*sizeof(float*));
	m_endTime=m_pMyStrategy->ParseCalcDate();
	CTimeSpan TimeSpan(3*366, 0, 0, 0);
	m_beginTime = m_endTime - TimeSpan;
	memset(&time1, 0, sizeof(NTime));
	memset(&time2, 0, sizeof(NTime));
	time1.year = m_beginTime.GetYear();
	time1.month = m_beginTime.GetMonth();
	time1.day = m_beginTime.GetDay();
	time2.year = m_endTime.GetYear();
	time2.month = m_endTime.GetMonth();
	time2.day = m_endTime.GetDay();
	m_pMyStrategy->SetTQFlag(m_bTQ);
	m_pMyStrategy->InputCalcDataTime(time1, time2, nCurrPeriod);
	m_pMyStrategy->CalcEndGap();
	m_pMyStrategy->InitAllData();
	m_pMyStrategy->ParseType();

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
				m_status.SetWindowText("");
				break;
			}
		}
		SuccessFlag = FALSE;

		m_pMyStrategy->SetCurGpIndex(awUsedStkp[i]);
		if(m_pMyStrategy->GetMyAnalData())
			SuccessFlag = m_pMyStrategy->CalcMyPlan();
		if ( SuccessFlag )		//��ѡ�е����
		{	
			SelectedStk[SuccessNum] = awUsedStkp[i];
			SuccessNum++;
			if(!m_bSort)
			{
				tempStr.Format("%d/%.1f%%",SuccessNum,SuccessNum*100.0/TotalStockNum);
				m_selectedNum.SetWindowText(tempStr);
			}
		}
		//������ǰ��һ��
		m_Carry_Progress.StepIt();
	}
	if(IsCarrying&&m_bSort&&m_pMyStrategy->GetConsInfo()&&SuccessNum>0)
	{
		for(i=SuccessNum;i<MAXGPNUM;i++) SelectedStk[i]=-1;
		SuccessNum=m_pMyStrategy->FiltSortRes(!m_isAndRadio, TotalStockNum, SuccessNum, SelectedStk);
		tempStr.Format("%d/%.1f%%",SuccessNum,SuccessNum*100.0/TotalStockNum);
		m_selectedNum.SetWindowText(tempStr);
	}
	else if(IsCarrying&&m_bSort&&!m_pMyStrategy->GetConsInfo())
	{
		SuccessNum=m_isAndRadio?0:TotalStockNum;
		for(i=0;i<TotalStockNum;i++) SelectedStk[i]=m_isAndRadio?-1:awUsedStkp[i];
		SuccessNum=m_pMyStrategy->FiltSortRes(!m_isAndRadio, TotalStockNum, SuccessNum, SelectedStk);
		tempStr.Format("%d/%.1f%%",SuccessNum,SuccessNum*100.0/TotalStockNum);
		m_selectedNum.SetWindowText(tempStr);
	}
	if(IsCarrying)
	{
		m_status.SetWindowText(_F("ѡ�����. "));
		MakeBlock(SelectedStk,SuccessNum,WhichBlock);//���ɰ�鲢��ʾ���
		CanClearList = TRUE;
		IsCarrying = FALSE;
	}
	m_pMyStrategy->UnInitAllData();
	EnableOther();
	g_bDonotClose = FALSE;
	IsCarried = TRUE;
	return;
}

void CMySelGpDlg::RefreshOther()
{
	if(m_nCurType<0||m_nCurType>1) return;
	int mainSel=m_mainLine.GetCurSel();
	char mainSelCs[64];
	CString strOtherSel;
	m_mainLine.GetLBText(mainSel, mainSelCs);
	CString strMainSel(mainSelCs);
	strOtherSel=m_otherLine;
	CComboBox *otherLine=(CComboBox *)GetDlgItem(IDC_OTHERLINE);
	otherLine->ResetContent();
	if(m_pCurIndexInfo->nDrawMode==OnlyMain||m_pCurIndexInfo->nDrawMode==KOverlay)
	{
		otherLine->AddString(_F("��߼�"));
		otherLine->AddString(_F("���̼�"));
		otherLine->AddString(_F("��ͼ�"));
		otherLine->AddString(_F("���̼�"));
	}
	for(int i=0;i<m_pCurIndexInfo->nLineNum;i++)
	{
		otherLine->AddString(_F(m_pCurIndexInfo->aLine[i].acLineName));
	}
	int selOtherIndex=otherLine->FindStringExact(-1, (LPCSTR)strOtherSel);
	int delOtherIndex=otherLine->FindStringExact(-1,(LPCSTR)strMainSel);
	if(delOtherIndex!=CB_ERR)
	{
		otherLine->DeleteString(delOtherIndex);
	}
	if(selOtherIndex!=delOtherIndex) otherLine->SetCurSel(selOtherIndex);
	else otherLine->SetCurSel(0);
}

void CMySelGpDlg::ParsStyle(int &nStyleType, int &nStyleIndex)
{
	if(m_nCurType!=STYLE_SEL) return;
	nStyleType=m_nCurIndex/100;
	nStyleIndex=m_nCurIndex%100;
}
/////////////////////////////////////////////////////////////////////////////
// CMySelGpDlg message handlers

void CMySelGpDlg::OnAddcondition() 
{
	UpdateData(TRUE);
	float	fCurrParam[MAX_PARAMNUM];
	if(m_nCurIndex < 0 ||m_nCurType <0)	return;
	if(ConditionNum >= MAX_TJCOMB_NUM)
	{
		TDX_MessageBox(m_hWnd,"����̫�࣬�����ټ���!",MB_OK+MB_ICONINFORMATION);
		return;
	}
	CComboBox *otherLine=(CComboBox*)GetDlgItem(IDC_OTHERLINE);
	if(otherLine->IsWindowVisible()&&m_otherLine=="")
	{
		TDX_MessageBox(m_hWnd,"�������������������������������������ȷ��ֵ��",MB_OK|MB_ICONERROR);
		return;
	}
	int nSelCycle = m_CalcCycle_Combo.GetCurSel();
	CString tempstr = m_endTime.Format("%y-%m-%d ");
	if(m_nCurType==ZB_SEL)
	{
		CString m_strParam[MAX_PARAMNUM];
		memset(fCurrParam, 0, sizeof(float) * MAX_PARAMNUM);
		m_Form.GetCurrData(fCurrParam);
		TINDEXINFO indexInfo=(*g_pCalc)[m_nCurType][m_nCurIndex];
		for(int i=0;i<indexInfo.nParaNum;i++)
		{
			m_strParam[i].Format("%5.f", fCurrParam[i]);
			TrimString(m_strParam[i]);
		}
		int nParam=indexInfo.nParaNum;
		CString tempstr1=nParam?"(":"";
		for(i=0;i<nParam;i++)
		{
			tempstr1+=m_strParam[i]+(i==nParam-1?"":",");
		}
		tempstr1+=nParam?")":"";
		tempstr += (CString)indexInfo.acCode + tempstr1 + ":";
		char mainCs[64], procCs[64];
		int curMainSel=m_mainLine.GetCurSel();
		int curProcSel=m_calcProc.GetCurSel();
		if(curProcSel>2&&curProcSel<6)
		{
			if(fabs(atof(m_otherLine)-atoi(m_otherLine))>0.00001)
			{
				TDX_MessageBox(m_hWnd,"�������������������������������������ȷ��ֵ��",MB_OK|MB_ICONERROR);
				return;
			}
		}
		m_mainLine.GetLBText(curMainSel, mainCs);
		CString strMain(mainCs);
		m_calcProc.GetLBText(curProcSel, procCs);
		CString strProc(procCs);
		tempstr += strMain + strProc + m_otherLine;
	}

	if(m_nCurType==CWHQ_SEL)
	{
		for(int k=0;;k++)
		{
			if(m_nCurIndex==g_nCustCon[k].MsgNo)
			{
				tempstr += g_nCustCon[k].topic;
				break;
			}
		}
		char procCs[64];
		int curProcSel=m_calcProc.GetCurSel();

		if(curProcSel>2&&curProcSel<6)
		{
			if(fabs(atof(m_otherLine)-atoi(m_otherLine))>0.00001)
			{
				TDX_MessageBox(m_hWnd,"�������������������������������������ȷ��ֵ��",MB_OK|MB_ICONERROR);
				return;
			}
		}
		m_calcProc.GetLBText(curProcSel, procCs);
		CString strProc(procCs);
		tempstr += strProc + m_otherLine + m_dynaStatic;
	}
	if(m_nCurType==KL_SEL)
	{
		tempstr += (CString)(*g_pCalc)[KL_TYPE][m_nCurIndex].acCode; 
	}
	if(m_nCurType==TJ_SEL)
	{
		CString m_strParam1,m_strParam2,m_strParam3,m_strParam4,m_strParam5,m_strParam6;
		memset(fCurrParam, 0, sizeof(float) * MAX_PARAMNUM);
		m_Form.GetCurrData(fCurrParam);
		
		m_strParam1.Format("%5.f",fCurrParam[0]);
		m_strParam2.Format("%5.f",fCurrParam[1]);
		m_strParam3.Format("%5.f",fCurrParam[2]);
		m_strParam4.Format("%5.f",fCurrParam[3]);
		m_strParam5.Format("%5.f",fCurrParam[4]);
		m_strParam6.Format("%5.f",fCurrParam[5]);
		TrimString(m_strParam1);
		TrimString(m_strParam2);
		TrimString(m_strParam3);
		TrimString(m_strParam4);
		TrimString(m_strParam5);
		TrimString(m_strParam6);
		
		int ParamNum = (*g_pCalc)[TJ_TYPE][m_nCurIndex].nParaNum;
		CString tempstr1;
		switch(ParamNum)
		{
		case 6:
			tempstr1 = "("+m_strParam1+","+m_strParam2+","+m_strParam3+","+m_strParam4+","+m_strParam5+","+m_strParam6+")";
			break;
		case 5:
			tempstr1 = "("+m_strParam1+","+m_strParam2+","+m_strParam3+","+m_strParam4+","+m_strParam5+")";
			break;
		case 4:
			tempstr1 = "("+m_strParam1+","+m_strParam2+","+m_strParam3+","+m_strParam4+")";
			break;
		case 3:
			tempstr1 = "("+m_strParam1+","+m_strParam2+","+m_strParam3+")";
			break;
		case 2:
			tempstr1 = "("+m_strParam1+","+m_strParam2+")";
			break;
		case 1:
			tempstr1 = "("+m_strParam1+")";
			break;
		}
		tempstr += (CString)(*g_pCalc)[TJ_TYPE][m_nCurIndex].acCode + " " + tempstr1;
	}
	if(m_nCurType==STYLE_SEL)
	{
		int nStyleType, nStyleIndex;
		CLE_CONTABLE *tmpTable;
		ParsStyle(nStyleType, nStyleIndex);
		switch(nStyleType)
		{
		case 0:	tmpTable = g_pCleKLine1;break;
		case 1:	tmpTable = g_pCleKLine2;break;
		case 2:	tmpTable = g_pCleKLine3;break;
		case 3:	tmpTable = g_pCleKLine4;break;
		case 4:	tmpTable = g_pCleJBM;	break;
		default:tmpTable = NULL;		break;
		}
		if(tmpTable)
		{
			CString str1(CleConTabStr[nStyleType]), str2(tmpTable[nStyleIndex].topic);
			tempstr += str1 + ":" + str2;
		}
	}
	if(m_pMyStrategy->CompCurCon(tempstr)) 
	{
		TDX_MessageBox(m_hWnd,"��ӵ����������ظ������������ã�",MB_OK);
		return;
	}
	TrimString(tempstr);
	m_Condition_List.AddString(_F(tempstr));
	CString str;
	CSize   sz;
	int     dx=0;
	CDC*    pDC = m_Condition_List.GetDC();
	for (int i=0;i < m_Condition_List.GetCount();i++)
	{
		m_Condition_List.GetText( i, str );
		sz = pDC->GetTextExtent(str);
		
		if (sz.cx > dx)
			dx = sz.cx;
	}
	// Set the horizontal extent so every character of all strings 
	// can be scrolled to.
	CRect rtList;
	m_Condition_List.GetWindowRect(&rtList);
	SCROLLINFO scroll;
	m_Condition_List.GetScrollInfo(SB_HORZ, &scroll);
	scroll.nMax = dx + 3;
	scroll.nPage = rtList.Width()/3;
	scroll.nMin = rtList.Width()/9;
	m_Condition_List.SetScrollInfo(SB_HORZ, &scroll);
	if(dx>rtList.Width()) m_Condition_List.ShowScrollBar(SB_HORZ);
	m_Condition_List.SetHorizontalExtent(dx+3);
	m_Condition_List.ReleaseDC(pDC);
///////////////////////////////////////////����������Ϣ
	m_pCurSiCon->sList=_F(tempstr);
	m_pCurSiCon->mainType=m_nCurType;
	m_pCurSiCon->indexNo=m_nCurIndex;
	m_pCurSiCon->endDate=m_endTime.GetDay()+m_endTime.GetMonth()*100+m_endTime.GetYear()*10000;
	m_pCurSiCon->otherLines.clear();
	if(m_nCurType==ZB_SEL)
	{
		BOOL bHasKLine=m_pCurIndexInfo->nDrawMode==OnlyMain||m_pCurIndexInfo->nDrawMode==KOverlay;
		short curSel=m_mainLine.GetCurSel();
		m_pCurSiCon->mainLine.lineType=curSel<=3&&bHasKLine?K_LINE:CALC_LINE;
		m_pCurSiCon->mainLine.lineIndex=curSel<=3?curSel:(bHasKLine?curSel-4:curSel);
		m_pCurSiCon->mainLine.lineValue=0.0f;

		int nCurProc=m_calcProc.GetCurSel();
		m_pCurSiCon->procIndex=nCurProc;
		CalcLine otherLine;
		if(nCurProc>2&&nCurProc<6)
		{
			m_bSort=TRUE;
			otherLine.lineType=NUM_LINE;
			otherLine.lineIndex=0;
			otherLine.lineValue=atof(m_otherLine);
			m_pMyStrategy->AddSortCon(ConditionNum);
		}

		else
		{
			int iStrMatch=m_mainLine.FindStringExact(-1,m_otherLine);
			if(iStrMatch==CB_ERR&&m_otherLine!="")
			{
				otherLine.lineType=NUM_LINE;
				otherLine.lineIndex=0;
				otherLine.lineValue=atof(m_otherLine);
			}
			else 
			{
				iStrMatch=iStrMatch==CB_ERR?curSel:iStrMatch;
				otherLine.lineType=iStrMatch<=3&&bHasKLine?K_LINE:CALC_LINE;
				otherLine.lineIndex=iStrMatch<=3?iStrMatch:(bHasKLine?iStrMatch-4:iStrMatch);
				otherLine.lineValue=0.0f;
			}
		}
		m_pCurSiCon->otherLines.push_back(otherLine);
	}

	else if(m_nCurType==CWHQ_SEL)
	{
		m_pCurSiCon->mainLine.lineType=STATIC_LINE;
		m_pCurSiCon->mainLine.lineIndex=0;
		m_pCurSiCon->mainLine.lineValue=0.0f;

		CalcLine otherLine;
		otherLine.lineType=NUM_LINE;
		otherLine.lineIndex=0;
		otherLine.lineValue=atof(m_otherLine);

		int nCurProc=m_calcProc.GetCurSel();
		m_pCurSiCon->procIndex=nCurProc;
		if(nCurProc>2&&nCurProc<6)
		{
			m_bSort=TRUE;
			otherLine.lineType=NUM_LINE;
			otherLine.lineIndex=0;
			otherLine.lineValue=atof(m_otherLine);
			m_pMyStrategy->AddSortCon(ConditionNum);
		}
		m_pCurSiCon->otherLines.push_back(otherLine);
	}
	
	else if(m_nCurType==TJ_SEL||m_nCurType==KL_SEL)
	{
		m_pCurSiCon->mainLine.lineType=CALC_LINE;
		m_pCurSiCon->mainLine.lineIndex=0;
		m_pCurSiCon->mainLine.lineValue=0.0f;
		m_pCurSiCon->procIndex=-1;
		m_pCurSiCon->otherLines.clear();
	}

	else if(m_nCurType==STYLE_SEL)
	{
		m_pCurSiCon->mainLine.lineType=DS_LINE;
		m_pCurSiCon->mainLine.lineIndex=0;
		m_pCurSiCon->mainLine.lineValue=0.0f;
		m_pCurSiCon->procIndex=-1;
		m_pCurSiCon->otherLines.clear();
	}

	else if(m_nCurType==DS_SEL)
	{
		m_pCurSiCon->mainLine.lineType=DS_LINE;
		m_pCurSiCon->mainLine.lineIndex=0;
		m_pCurSiCon->mainLine.lineValue=0.0f;
		m_pCurSiCon->procIndex=-1;
		m_pCurSiCon->otherLines.clear();
	}

	m_pMyStrategy->AddSingleCon((*m_pCurSiCon));
	ConditionNum++;
	m_Condition_List.SetCurSel(ConditionNum-1);
	if(ConditionNum > 1)
	{
		GetDlgItem(IDC_ADD_RADIO)->EnableWindow(TRUE);
		GetDlgItem(IDC_OR_RADIO)->EnableWindow(TRUE);
	}
}

void CMySelGpDlg::OnOK() 
{
}

void CMySelGpDlg::OnCancel()
{
	if(!IsCarrying && IsCarried)	//����رմ���
	{
		m_Form.SendMessage(UM_RESTORE, 0, 0);
		GetParent()->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);	
		g_bDonotClose = FALSE;
	}
	else	//���ȡ��ִ��ѡ��
		IsCarrying = FALSE;
}

void CMySelGpDlg::OnCarryout() 
{
	CarryoutToBlock(-1);//ȱʡΪ����ѡ��
}

#include "GetBlockDlg.h"
void CMySelGpDlg::OnCarryoutBlock() 
{
	CGetBlockDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		if(HasGPInBlockAndContinue(dlg.nSelBlock))
			CarryoutToBlock(dlg.nSelBlock);
	}
}

#include "ChangeDomainDlg.h"
void CMySelGpDlg::OnChangedomainButton()
{
	CChangeDomainDlg dlg;
	dlg.m_nSelDomain = m_nSelDomain;
	memcpy(dlg.m_GpDomain,m_GpDomain,m_nSelDomain*sizeof(struct GpDomain_Struct));
	if(dlg.DoModal()==IDOK)
	{
		m_nSelDomain = min(MAX_DOMAINNUM,dlg.m_nSelDomain);
		memcpy(m_GpDomain,dlg.m_GpDomain,m_nSelDomain*sizeof(struct GpDomain_Struct));
		SetDomainStr();
		int TotalStockNum = FiltMulGpDomain(m_GpDomain,m_nSelDomain,awUsedStkp);
		m_totalGpNum.SetWindowText(IntToStr(TotalStockNum));
	}
}

void CMySelGpDlg::OnDeletecondition()
{
	int nSelCondition = m_Condition_List.GetCurSel();
	if(nSelCondition != LB_ERR)
	{
		m_Condition_List.DeleteString (nSelCondition);
		m_pMyStrategy->DelSingleCon(nSelCondition);
		m_bSort=m_pMyStrategy->HasSortCon();
		ConditionNum--;
		if(ConditionNum <= 1)
		{
			GetDlgItem(IDC_ADD_RADIO)->EnableWindow(FALSE);
			GetDlgItem(IDC_OR_RADIO)->EnableWindow(FALSE);
		}
		m_Condition_List.SetCurSel(max(nSelCondition-1,0));
	}
}

#include "DescZBDlg.h"
void CMySelGpDlg::OnIndexInfomation() 
{
	if(m_nCurType >= 0&&m_nCurIndex >= 0)
	{
		CDescZBDlg dlg;
		TINDEXINFO	*pIndex = g_pCalc->_CallDTopViewCalc_8(m_nCurType,(long)m_nCurIndex);
		if(pIndex)
		{
			dlg.Init(m_nCurType, pIndex->acCode);
			dlg.DoModal();
			return;
		}
		
		CLE_CONTABLE *tmpTable;
		int nStyleType, nStyleIndex;
		ParsStyle(nStyleType, nStyleIndex);
		switch(nStyleType)
		{
		case 0:	tmpTable = g_pCleKLine1;break;
		case 1:	tmpTable = g_pCleKLine2;break;
		case 2:	tmpTable = g_pCleKLine3;break;
		case 3:	tmpTable = g_pCleKLine4;break;
		//	case 4:	tmpTable = g_pCleJBM;	break;
		default:tmpTable = NULL;		break;
		}
		CString tmpStr;
		if(nStyleType >= 0 && nStyleType< 4) 
			tmpStr.Format("XG%04d",tmpTable[nStyleIndex].MsgNo);
		CString tmpLine,tmpidStr,TotalStr;
		CStdioFile theFile;
		if(theFile.Open("cleinfo.dat",CFile::modeRead|CFile::shareDenyNone))
		{
			while(theFile.ReadString(tmpLine))
			{
				if(tmpLine.IsEmpty()) continue;
				if(tmpLine[0] == '?')
				{
					tmpidStr = tmpLine.Mid(1,6);
					if(tmpidStr == tmpStr)
					{
						while(theFile.ReadString(tmpLine))
						{
							if(tmpLine[0] == '?') break;
							TotalStr+=tmpLine+"\r\n";
						}
						break;
					}
				}
			}
			dlg.m_strDesc=TotalStr;
			HTREEITEM hItem = m_zbTree.GetSelectedItem();
			dlg.m_strTitle = m_zbTree.GetItemText(hItem);
			dlg.DoModal();
		}
	}
}

BOOL CMySelGpDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ImageList.Create(16, 16, ILC_COLORDDB | ILC_MASK, 0, 1);
	HICON	HPlan1 = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON5));
	m_ImageList.Add(HPlan1);
	DestroyIcon(HPlan1);
	HICON	HPlan2 = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON3));
	m_ImageList.Add(HPlan2);
	DestroyIcon(HPlan2);
	HICON	HPlan3 = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON7));
	m_ImageList.Add(HPlan3);
	DestroyIcon(HPlan3);
	m_zbTree.SetImageList(&m_ImageList, TVSIL_NORMAL);

	SetDateCtrlRange(GetDlgItem(IDC_DATETIMEPICKER2));
	GetDlgItem(IDC_DATETIMEPICKER2)->EnableWindow(FALSE);

	m_status.SetTextColor(RGB(240,0,0));
	m_status.SetText(_F("ѡ�������صı��������н���,��ȷ����������."));
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
	int TotalStockNum = FiltMulGpDomain(m_GpDomain,m_nSelDomain,awUsedStkp);
	m_totalGpNum.SetWindowText(IntToStr(TotalStockNum));

	CRect rc;
	m_outStatic.GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.DeflateRect(4,4);
	rc.top += 10;
	m_Form.Create(NULL, "", WS_CHILD|WS_VISIBLE, rc, this, ID_CONTROL_VIEW , NULL);
	m_Form.Init(rc);
	m_Form.HideSelf();

	ShowTypeInfo();

	m_tabctrl.InsertItem(0,_F(" ������ "));
	m_tabctrl.InsertItem(1,_F("ȫ������ "));
	m_tabctrl.InsertItem(2,_F("ϵͳ���� "));
	m_tabctrl.InsertItem(3,_F("�û����� "));
	m_tabctrl.SetCurSel(0);
	
	FillTree(0);
	m_pMyStrategy = new MyStrategy;
	m_pCurSiCon = new SingleCon;

	//�����������б��
	for(int i = 0; g_CalcPeriodStr[i] != NULL ;i++)
		m_CalcCycle_Combo.AddString (_F(g_CalcPeriodStr[i]));
	m_CalcCycle_Combo.SetCurSel (DAY_ST);	//Ĭ��������

	m_Condition_List.SetItemHeight (0,16);
	m_Condition_List.ModifyStyle(0, WS_HSCROLL|WS_VSCROLL|LBS_HASSTRINGS);
	ConditionNum = 0;

	GetDlgItem(IDC_ADD_RADIO)->EnableWindow(FALSE);
	GetDlgItem(IDC_OR_RADIO)->EnableWindow(FALSE);

	m_isAndRadio = 0;	//Ϊ0��ʾѡ���1��

	CTimeSpan TimeSpan(300, 0, 0, 0);
	m_endTime = CTime::GetCurrentTime();
	m_beginTime = m_endTime - TimeSpan;

	CanClearList = FALSE;
	IsCarrying = FALSE;
	IsCarried = TRUE;

	UpdateData(FALSE);
	return TRUE;
}

#include "ConditionLoadDlg.h"
void CMySelGpDlg::OnLoadcondition() 
{
	CConditionLoadDlg dlg;
	dlg.ConFlag = 2;
	if( dlg.DoModal () == IDOK )
	{
		if(dlg.ReturnStr != "")
		{
			m_Condition_List.SetCurSel(0);
			while(ConditionNum>0)
			{
				OnDeletecondition();
			}
			m_Condition_List.ResetContent();
			ASSERT(ConditionNum==0);
			CString tempFileName = g_WSXHStr+dlg.ReturnStr;
			CFile m_File;
			if( m_File.Open(tempFileName,CFile::modeRead|CFile::shareDenyNone) )
			{
				CArchive ar(&m_File, CArchive::load);
				m_pMyStrategy->SLPlan(ar);
				m_bSort=m_pMyStrategy->IsSort();
				vector<CString> aListStrings;
				m_pMyStrategy->GetListStrings(aListStrings);
				for(int i=0;i<aListStrings.size();i++)
				{
					ConditionNum++;
					m_Condition_List.AddString(aListStrings[i]);
				}
				ar.Close();
				m_File.Close();
				m_Condition_List.SetCurSel(ConditionNum-1);
				if(ConditionNum > 1)
				{
					GetDlgItem(IDC_ADD_RADIO)->EnableWindow(TRUE);
					GetDlgItem(IDC_OR_RADIO)->EnableWindow(TRUE);
				}
				UpdateData(FALSE);
			}
			else
				TDX_MessageBox(m_hWnd,"���ܴ����ļ���",MB_OK+MB_ICONEXCLAMATION);
		}
	}
	return;
}

#include "InputName.h"
void CMySelGpDlg::OnSavecondition() 
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
	dlg.m_NewName = "my1";
	if( dlg.DoModal() == IDOK )
	{
		TrimString(dlg.m_NewName);
		dlg.m_NewName.MakeLower();
		if(dlg.m_NewName == "")	return;
		BOOL bNewEndTime = TRUE;
		if(TDX_MessageBox(m_hWnd, "�´����뷽��ʱ�Ƿ�����Ϊ��������ڣ�",MB_ICONINFORMATION|MB_YESNO)==IDNO)
			bNewEndTime = FALSE;
		//����������
		CString tempFileName = g_WSXHStr+dlg.m_NewName+".czs";
		CFileException ex;
		CFile m_File;
		if( m_File.Open(tempFileName,CFile::modeCreate|CFile::modeWrite,&ex) )
		{
			CArchive ar(&m_File, CArchive::store);
			UpdateData(TRUE);
			m_pMyStrategy->SLPlan(ar, bNewEndTime);
			ar.Close();
			m_File.Close();
		}
		else
		{
			char strError[1024];
			ex.GetErrorMessage(strError,1024);
			TDX_MessageBox(m_hWnd,"�ļ���������!\n"+(CString)strError,MB_ICONEXCLAMATION|MB_OK);
		}
	}
}

void CMySelGpDlg::OnSelchangeCalccycleCombo() 
{
	int nSel = m_CalcCycle_Combo.GetCurSel();
	//��������m_Form��״̬
	if(m_nCurType>-1)
	{
		m_Form.HideSelf();
		m_Form.SetStatus(m_nCurType, m_nCurIndex, nSel);
	}
}

void CMySelGpDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);	
}

void CMySelGpDlg::OnSelchangedZbtree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	if(!UpdateData(TRUE)) return;
	LPARAM	lParam = pNMTreeView->itemNew.lParam;
	GetDlgItem(IDC_INDEX_INFOMATION)->EnableWindow(TRUE);
	m_nCurType=LOWORD(lParam);
	m_nCurIndex=HIWORD(lParam);
	if(m_nCurType>-1&&m_nCurType<ZB_TYPESEL&&m_nCurType!=CWHQ_SEL&&m_nCurIndex>-1)
	{
		SetDateCtrlRange(GetDlgItem(IDC_DATETIMEPICKER2));
		GetDlgItem(IDC_DATETIMEPICKER2)->EnableWindow(TRUE);
	}
	else
	{
		SetDateCtrlRange(GetDlgItem(IDC_DATETIMEPICKER2));
		GetDlgItem(IDC_DATETIMEPICKER2)->EnableWindow(FALSE);
	}
	CComboBox *otherLine=(CComboBox*)GetDlgItem(IDC_OTHERLINE);		
 	m_mainLine.ResetContent();
	otherLine->ResetContent();
	
	m_dynaStatic = "";
	if(m_nCurType==ZB_SEL||m_nCurType==TJ_SEL||m_nCurType==KL_SEL) 
		m_pCurIndexInfo = g_pCalc->_CallDTopViewCalc_8(m_nCurType,m_nCurIndex);
	else if(m_nCurType==CWHQ_SEL) 
	{
		GetDlgItem(IDC_INDEX_INFOMATION)->EnableWindow(FALSE);
		DynaShow();
	}
	UpdateData(FALSE);
	ShowTypeInfo();
	ShowConInfo();
	FillConInfo();
	
	*pResult = 0;
}

void CMySelGpDlg::OnSelchangeProc() 
{
	if(!UpdateData(TRUE)) return;
	CComboBox *otherLine=(CComboBox*)GetDlgItem(IDC_OTHERLINE);
	CString selText;
	int nCurProc=m_calcProc.GetCurSel();
	if(nCurProc>7) otherLine->ShowWindow(SW_HIDE);
	else if(nCurProc>2&&nCurProc<6) 
	{
		otherLine->ShowWindow(SW_SHOW);
		otherLine->ResetContent();
		m_dynaStatic="";
	}
	else 
	{
		otherLine->ShowWindow(SW_SHOW);
		RefreshOther();
		DynaShow();
	}
	UpdateData(FALSE);
}

void CMySelGpDlg::OnSelchangeMainline()
{
	RefreshOther();
}

void CMySelGpDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int n=m_tabctrl.GetCurSel();
	FillTree(n);
	*pResult = 0;
}
