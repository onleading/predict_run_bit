// QuickFlushTjDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "QuickFlushTjDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQuickFlushTjDlg dialog


CQuickFlushTjDlg::CQuickFlushTjDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQuickFlushTjDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CQuickFlushTjDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bCalcFinished = FALSE;
	m_bCalcCanceled = FALSE;
}


void CQuickFlushTjDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuickFlushTjDlg)
	DDX_Control(pDX, IDC_PROGRESS, m_ctlProgress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQuickFlushTjDlg, CDialog)
	//{{AFX_MSG_MAP(CQuickFlushTjDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuickFlushTjDlg message handlers

BOOL CQuickFlushTjDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	long VipDocLastDate=GetVipDocLastDate();
	if( (VipDocLastDate<g_lOpenRq && GetForwardDate(VipDocLastDate)<g_lOpenRq) && !g_bLastRqDown)
	{
		if(OnLine && g_bCanDown)
		{
			if(DownDayData_AtOnce(AfxGetMainWnd(),"�����������ݿ��ܲ�ȫ,��������Ҫʹ����������������,\n\n�Ƿ�����?(�������ز�ȫ,���ܻᵼ�½����׼ȷ)",FALSE))
			{
				if(!g_bLastRqDown) //���ȷ�������˵�û����������,�����½���
					;
			}
		}
		else
		{
			if(TDX_MessageBox(AfxGetMainWnd()->GetSafeHwnd(),"�����������ݲ�ȫ,������[�̺���������]���ܲ�����������,\n������ܻᵼ�½����׼ȷ,\n�Ծɼ�����?",MB_OKCANCEL|MB_ICONEXCLAMATION) == IDCANCEL)
				;
		}
	}

	PostMessage(WM_COMMAND, IDOK);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

extern void SaveBlockCfg();
DTopViewCalc * m_pCalcTj = NULL;
void CQuickFlushTjDlg::OnOK() 
{
	if(m_nCalcType==CALC_TJBLOCK)
		CalcTJBlock();
	else if(m_nCalcType==CALC_ZSBLOCK)
		CalcZSBlcok();
	else if(m_nCalcType==CALC_DXINDEX1)
		CalcDXTJIndex1();
	else if(m_nCalcType==CALC_DXINDEX2)
		CalcDXTJIndex2();
	
	if(m_bCalcFinished)
		CDialog::OnOK();
	else CDialog::OnCancel();
}

void CQuickFlushTjDlg::OnCancel() 
{
	m_bCalcCanceled = TRUE;
}
//////////////////////////////////////////////////////////////////////////
void	CQuickFlushTjDlg::CalcDXTJIndex1()
{
	int stocknum = g_pStockIO->GetStockNum(ALL_ALL);
	//���ý�����
	m_ctlProgress.SetRange32(0,stocknum);
	m_ctlProgress.SetStep(1);
	m_ctlProgress.SetPos(0);
	
	GetDlgItem(IDC_STATIC1)->SetWindowText("���ڼ�������ѡ���źţ������ĵȴ�������������");
	
	for(int j = 0;j < stocknum;j++)
	{
		if(m_bCalcCanceled) return;

		MemStkInfo *pInfo = NULL;
		
		m_ctlProgress.StepIt();
		pInfo = (*g_pStockIO)[j];
		if(!pInfo) continue;
		short nType = GetStockType(pInfo->setcode, pInfo->Code);
		if(nType!=CODE_SZAG&&nType!=CODE_ZXB&&nType!=CODE_CYB&&nType!=CODE_SHAG) continue;

		time_t ct = time(NULL);
		if(ct-pInfo->CalcRes1.CalcTmie<60*5) continue;
		g_DXIndexCalc.Lock();
		CalcDXIndex1(pInfo);
		g_DXIndexCalc.Unlock();
		PeekMsg();
	}
	m_bCalcFinished = TRUE;
}

void	CQuickFlushTjDlg::CalcDXTJIndex2()
{
	int stocknum = g_pStockIO->GetStockNum(ALL_ALL);
	//���ý�����
	m_ctlProgress.SetRange32(0,stocknum);
	m_ctlProgress.SetStep(1);
	m_ctlProgress.SetPos(0);
	
	GetDlgItem(IDC_STATIC1)->SetWindowText("���ڼ�������ѡ���źţ������ĵȴ�������������");
	
	for(int j = 0;j < stocknum;j++)
	{
		if(m_bCalcCanceled) return;

		MemStkInfo *pInfo = NULL;
		
		m_ctlProgress.StepIt();
		pInfo = (*g_pStockIO)[j];
		if(!pInfo) continue;
		short nType = GetStockType(pInfo->setcode, pInfo->Code);
		if(nType!=CODE_SZAG&&nType!=CODE_ZXB&&nType!=CODE_CYB&&nType!=CODE_SHAG) continue;

		time_t ct = time(NULL);
		if(ct-pInfo->CalcRes2.CalcTmie<60*5) continue;
		g_DXIndexCalc.Lock();
		CalcDXIndex2(pInfo);
		g_DXIndexCalc.Unlock();
		PeekMsg();
	}
	m_bCalcFinished = TRUE;
}

void	CQuickFlushTjDlg::CalcZSBlcok()
{
	if(!g_pLcIdxMng) return;
	int nIdxNum = g_pLcIdxMng->GetIdxNum();
	if(nIdxNum<1) return;

//	InterlockedExchange(&g_nBlockTypeCalcing, -1);
	RefreshAllHq(5*60);

	//���ý�����
	m_ctlProgress.SetRange32(0,nIdxNum);
	m_ctlProgress.SetStep(1);
	m_ctlProgress.SetPos(0);

	GetDlgItem(IDC_STATIC1)->SetWindowText("���ڼ�����ҵ�������...");

	for (int i = 0; i < nIdxNum; i++ )
	{
		if(m_bCalcCanceled) return;

		LPLCIDXSET pLs =g_pLcIdxMng->GetIdxInfo(i);
		if(!pLs) continue;
		int nHYID = pLs->nHYID;
		ZJHHY_Cfg *pZf = g_pConstMng->GetHyFromHYID(nHYID);
		if(pZf) 
		{	
			g_pLcIdxMng->ReCalc(i);
			m_ctlProgress.StepIt();
			PeekMsg();
		}
	}
	m_bCalcFinished = TRUE;

//	InterlockedExchange(&g_nBlockTypeCalcing, m_nBlockType);

	//��¼ˢ��ʱ��
	struct tm t;            //����tmʱ��ṹ�������洢ʱ���ʽ��������Ϣ
	time_t t_of_data;     //����time_tʱ��ṹ
	CTime ct = CTime::GetCurrentTime();
	t.tm_year=ct.GetYear()-1900;//��1900��Ϊ��׼����ʱ��
	t.tm_mon=ct.GetMonth();                 //Ϊ�ṹ���Ա��ֵ
	t.tm_mday=ct.GetDay();
	t.tm_hour=ct.GetHour();
	t.tm_min=ct.GetMinute();
	t.tm_sec=ct.GetSecond();
	t.tm_isdst=0;
	t_of_data=mktime(&t);		
	char ltm[20]={0};
	sprintf(ltm, "%d", t_of_data);
	WritePrivateProfileString("OTHER","LocalDxFreshTime",ltm,g_WSXHStr+g_strUserCfg);
}

void	CQuickFlushTjDlg::CalcTJBlock()
{
	int ConditionNum  = 0;
	CONDITIONINFO ConditionInfo[MAX_TJCOMB_NUM]={0};
	FILEHEAD FileHead={0};
	FileHead.nRefreshTime = -1;
	int i,j,k;
	//װ������
	CString tempFileName = g_WSXHStr+"conblk.cos";
	CFile m_File;
	if( m_File.Open(tempFileName,CFile::modeRead|CFile::shareDenyNone) )
	{
		//�����ļ�ͷ
		m_File.Read(&FileHead,sizeof(FILEHEAD));
		ConditionNum = FileHead.ConditionNum;
		CString tempStr;
		for(int i = 0; i < ConditionNum ;i++)
			//�����������
			m_File.Read(&ConditionInfo[i],sizeof(CONDITIONINFO));
		m_File.Close();
	}

	int		nCurrPeriod = DAY_ST;
	if(ConditionNum < 1)
	{
		//����������
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
		
		CDialog::OnCancel();
		return;
	}

	char acCode[ZBCODE_LEN+1];
	short TjIndexNo[MAX_TJCOMB_NUM];
	memset(TjIndexNo,0,MAX_TJCOMB_NUM*sizeof(short));
	if(m_pCalcTj==NULL)
	{	
		m_pCalcTj = new DTopViewCalc;
		m_pCalcTj->_CallDTopViewCalc_1(FALSE,FALSE,g_lOpenRq,0,NULL,NULL);
		if(!m_pCalcTj->_CallDTopViewCalc_0(HomePath.GetBuffer(0),g_WSXHStr.GetBuffer(0),(g_WSXHStr+"100nian.dat").GetBuffer(0)))
		{
			CDialog::OnCancel();
			return;
		}
		m_pCalcTj->_CallDTopViewCalc_2((PDATAIOFUNC)CalcCallBackFunc);
	}
	for (i = 0; i < ConditionNum; i++ )
	{
		strcpy(acCode, ConditionInfo[i].acCode);
		TINDEXINFO	* pTJIndex = m_pCalcTj->_CallDTopViewCalc_8(TJ_TYPE,(long)0);
		for(j = 0;j < m_pCalcTj->_CallDTopViewCalc_7(TJ_TYPE);j++)//����ǴӴ����ļ���ȡ�õ���������Ӧ����з������
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

	//������ȡ�����з��������Ĺ�Ʊ
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
	{
		//����������
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

		CDialog::OnCancel();
		return;
	}

	RefreshAllHq(5*60);
	if(g_nAutoCalcLock>0)
	{
		CDialog::OnCancel();
		return;
	}

	//���ý�����
	m_ctlProgress.SetRange32(0,TotalStockNum);
	m_ctlProgress.SetStep(1);
	m_ctlProgress.SetPos(0);

	GetDlgItem(IDC_STATIC1)->SetWindowText("���ڼ�������ѡ���źţ������ĵȴ�������������");

	int SuccessNum = 0,ReadNum;
	int SuccessFlag;					//0:���� 1:�ɹ�	-1:ʧ�� -2:�׶�����
	CString tempStr;

	float * fPut[MAX_LINEOUT]={0};
	long time2=FileHead.nDate;
	MemStkInfo *pStkInfo=NULL;
	for ( i = 0; i < TotalStockNum; i++ )
	{
		if(m_bCalcCanceled) return;

		if(g_nAutoCalcLock>0)
		{
			CDialog::OnCancel();
			return;
		}

		PeekMsg();

		pStkInfo = (*g_pStockIO)[awUsedStkp[i]];
		SuccessFlag = 0;
		int nCalcPer = 0,nPer;
		CurrStockData hqtmp={0};
		while(pStkInfo && pStkInfo->Close>COMPPREC && nCalcPer<ConditionNum)
		{
			if(m_bCalcCanceled) return;

			if(g_nAutoCalcLock>0)
			{
				CDialog::OnCancel();
				return;
			}

			nPer = ConditionInfo[nCalcPer].CalcCycle;
			int nPerCalc = CalcPeriodToTdxPeriod(nPer);
			
			ReadNum = m_pCalcTj->_CallDTopViewCalc_4(pStkInfo->Code,pStkInfo->setcode,nPerCalc,ASK_ALL,1);
			if(ReadNum>0)
			{
				for ( j = nCalcPer; j < ConditionNum && ConditionInfo[j].CalcCycle==nPer; j++ )
				{
					TINDEXINFO	* pTJIndex = m_pCalcTj->_CallDTopViewCalc_8(TJ_TYPE,TjIndexNo[j]);
					if(!pTJIndex) continue;
					m_pCalcTj->_CallDTopViewCalc_14(fPut[0]);
					memset(fPut,0,MAX_LINEOUT*sizeof(float*));
					fPut[0]  = m_pCalcTj->_CallDTopViewCalc_13(ReadNum);
					for(k=0;k<min(ConditionInfo[j].ParamNum,pTJIndex->nParaNum);k++)
						pTJIndex->aPara[k].nValue[nCurrPeriod] = ConditionInfo[j].ParamValue[k];
					if ( -1  != m_pCalcTj->_CallDTopViewCalc_5(TJ_TYPE,TjIndexNo[j],fPut) )
					{
						if((TBE_MEANLESS(fPut[0][ReadNum-1]) != 0) && fabs(fPut[0][ReadNum-1]) > 0.1)
							resStkp[j].push_back(awUsedStkp[i]);
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
		m_ctlProgress.StepIt();
	}
	m_pCalcTj->_CallDTopViewCalc_14(fPut[0]);

	//����������
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

	for(i=0;i<ConditionNum;i++)
	{
		TINDEXINFO	*pTJIndex = m_pCalcTj->_CallDTopViewCalc_8(TJ_TYPE,ConditionInfo[i].acCode);
		//�����б���
		CString tempStr = (CString)(pTJIndex?pTJIndex->acName:ConditionInfo[i].acCode) + " ";
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
		
		char Code[100]={0};
		sprintf(Code, "@ConBlk%d", i);
		SeveToNewBlock(resStkp[i], Code, (char*)(LPCSTR)tempStr, i);
	}
	SaveBlockCfg();
	InterlockedIncrement(&g_nCountCalcBlockTJ);

	m_bCalcFinished = TRUE;
}