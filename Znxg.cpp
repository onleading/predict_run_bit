#include "stdafx.h"
#include "Znxg.h"
#include "ProgressWnd.h"

CZnxg::CZnxg(CWnd *pWnd)
{
	m_pParent = pWnd;
	m_nABGNum = 0;
	//���ֽ�ֵ��������
	g_pXGSign.Xz_sign = 4500;		//С��,���̷ֽ�  4500���
	g_pXGSign.Dz_sign = 15000;		//����,���̷ֽ�  15000���

	g_pXGSign.Yp_sign = 0.60f;		//����,��ƽ�ֽ�  ÿ��˰������ÿ�����棩>= 0.60Ԫ
	g_pXGSign.Pc_sign = 0.10f;		//��ƽ,����ֽ�  ÿ��˰������ÿ�����棩<  0.60Ԫ ����
									//ÿ��˰������ÿ�����棩>= 0.10Ԫ
	g_pXGSign.Ks_sign = 0.0;		//�����־

	g_pXGSign.Cxg_Day_L = 120;		//���¹�����ʱ����±�־

	g_pXGSign.Jzc_G_sign = 2.50f;	//���ʲ��߱�־
	g_pXGSign.Jzc_D_sign = 1.00f;	//���ʲ��ͱ�־

	g_pXGSign.Gjx_G_sign = 1.50f;	//������߱�־
	g_pXGSign.Gclr_G_sign = 0.50f;	//��������߱�־
	g_pXGSign.DMI_Day = 14;			//DMI�� 14��DMI
	g_pXGSign.RSI_Day = 12;			//������RSI��12��RSI
	g_pXGSign.WVAD_Day = 24;		//24��WVAD
}

CZnxg::~CZnxg()
{
}

//֧�ֶϵ����� CalcFlag 0:���ؼ����������,������ 1:Զ�������ļ�
BOOL CZnxg::CalcAllStockStyle()
{
	BOOL bRet = FALSE;
	int	 nStartNo = 0,i,k;
	memset(m_pGPCode, 0, sizeof(m_pGPCode));
	//�õ��AB�ɵļ���
	union   flunion ftype;
	ftype.fshort = 0x81;//������С��ҵ
	m_nABGNum = g_pStockIO->FiltCode(SZ_SH,ftype,m_pABGXh);
	ftype.fshort = 0x02;
	m_nABGNum += g_pStockIO->FiltCode(SZ_SH,ftype,m_pABGXh+m_nABGNum);

	BOOL bReadSuccess = FALSE;
	//�˴��д���ȶ,Cache�ļ��ڼ����汾�е�����ô��
	CFile RCachFile;
	if(RCachFile.Open(g_WSXHStr+(CString)"tmp\\clecache.tmp",CFile::modeRead|CFile::shareDenyNone))
	{
		CTime theTime,NowTime = CTime::GetCurrentTime();
		RCachFile.Read(&theTime,sizeof(theTime));
		long Lastdate;
		RCachFile.Read(&Lastdate,sizeof(long));
		char version;
		RCachFile.Read(&version,sizeof(char));
		if(version == ZNXG_VERSION && Lastdate == g_lOpenRq) 	//����ѡ�ɰ汾��һ�µȣ���ֱ�ӷ���
		{
			DWORD FileLen = RCachFile.GetLength();
			if(!g_bZnxg_Down && (NowTime-theTime).GetHours() < 1)
				bReadSuccess = TRUE;
			if(bReadSuccess)
			{
				nStartNo = (FileLen-sizeof(theTime)-sizeof(long)-sizeof(char))/(SH_CODE_LEN+sizeof(long)+sizeof(short));
				nStartNo = min(m_nABGNum,nStartNo);	//nStartNo���ܴ���m_nABGNum
				for(k = 0;k < nStartNo;k++)
				{
					RCachFile.Read(m_pGPCode+k,SH_CODE_LEN);
					m_pGPCode[k][SH_CODE_LEN] = 0;
					RCachFile.Read(m_pStkAttrib+k,sizeof(long));
					RCachFile.Read(m_pStkXh+k,sizeof(short));
				}
			}
		}
		RCachFile.Close();
	}
	if(bReadSuccess && nStartNo == m_nABGNum)	return TRUE;

	if(!CanProcesLocalData(m_pParent))
		return FALSE;	

	RefreshAllHq();

	g_bZnxg_Down = FALSE;

	int		nTodayDataNum;
	short   nDataNum;
	MemStkInfo *tmpInfo;
	struct AnalyData *pAnalyData = new AnalyData[200+10];
	memset(pAnalyData, 0, (200+10)*sizeof(AnalyData));
	struct CurrStockData  CurrHQ={0};
	//�趨ģ̬�������Ի���
	CProgressWnd wndProgress(m_pParent,_F("���ڼ��㣬���Ե�..."),TRUE);
	wndProgress.GoModal();
	wndProgress.SetRange(0,m_nABGNum-nStartNo);
	wndProgress.SetStep(3);
	g_bDonotClose = TRUE;
	for(i = 0; i < m_nABGNum;i++)
	{
		tmpInfo = (*g_pStockIO)[m_pABGXh[i]];
		if(!tmpInfo || tmpInfo->Close<COMPPREC) 
			continue;
		for(k=0;k < nStartNo;k++)
			if(strcmp(tmpInfo->Code,m_pGPCode[k])==0) break;
		if(k >= nStartNo)	//�������У�������
		{
			if(i%3 == 0)
			{
				wndProgress.SetText(tmpInfo->Name);
				wndProgress.StepIt();
			}
			if(i%10 == 0)
			{
				wndProgress.PeekAndPump();
				if(wndProgress.Cancelled())
				{
					if(i > 0)	bRet = TRUE; //�����;ȡ������Ҳ����
					else		bRet = FALSE;
					m_nABGNum = i;
					g_bDonotClose = FALSE;
					goto writecache;
				}
			}
			nDataNum = g_pStockIO->ReadAnalyDataAndCurrHQ(tmpInfo,pAnalyData,200,PER_DAY,&CurrHQ,nTodayDataNum);
			//�����Ȩ����
			g_pStockIO->TQData(tmpInfo,pAnalyData,0,nDataNum-1,nDataNum,1,PER_DAY);
			
			if(nDataNum <= 20||GetKLineStyleSet(tmpInfo,pAnalyData,nDataNum,nDataNum-1,&m_pStkXh[i])<=0)
				m_pStkXh[i] = 0;
			m_pStkAttrib[i] = GetGP_Tj_Attrib(m_pABGXh[i],pAnalyData,nDataNum);

			strncpy(m_pGPCode[i],tmpInfo->Code,SH_CODE_LEN);
		}
	}
	m_nABGNum = i;
	bRet = TRUE;
	g_bDonotClose = FALSE;
writecache:;
	delete []pAnalyData;
	CFile WCachFile;
	if(nStartNo == 0)
	{
		if(WCachFile.Open(g_WSXHStr+(CString)"tmp\\clecache.tmp",CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive))
		{
			CTime theTime = CTime::GetCurrentTime();
			WCachFile.Write(&theTime,sizeof(theTime));
			long NowDate = g_lOpenRq;
			WCachFile.Write(&NowDate,sizeof(long));
			char version = ZNXG_VERSION;
			WCachFile.Write(&version,sizeof(char));
			for(k = 0;k < m_nABGNum;k++)
			{
				WCachFile.Write(m_pGPCode+k,SH_CODE_LEN);
				WCachFile.Write(m_pStkAttrib+k,sizeof(long));
				WCachFile.Write(m_pStkXh+k,sizeof(short));
			}
			WCachFile.Close();
		}
	}
	else
	{
		if(WCachFile.Open(g_WSXHStr+(CString)"tmp\\clecache.tmp",CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite|CFile::shareExclusive))
		{
			WCachFile.SeekToEnd();
			for(k = 0;k < m_nABGNum-nStartNo;k++)
			{
				WCachFile.Write(m_pGPCode+nStartNo+k,SH_CODE_LEN);
				WCachFile.Write(m_pStkAttrib+nStartNo+k,sizeof(long));
				WCachFile.Write(m_pStkXh+nStartNo+k,sizeof(short));
			}
			WCachFile.Close();
		}
	}
	return bRet;
}

short CZnxg::GetKLineStyleGP(short Type,short *lpnGPIndex)
{
	int nCount = 0;
	for(short i = 0;i < m_nABGNum;i++)
	{
		if(m_pStkXh[i] == Type)
			lpnGPIndex[nCount++] = m_pABGXh[i];
	}
	return nCount;
}

short CZnxg::GetKLineStyleSet(MemStkInfo *tmpInfo,struct AnalyData *pAnalyData,short nDataNum,short nNo,short * npoutStyle,short nMaxNum)
{
	short	nStyleKind=0;
	short   nTrendSet[128];
	short	npStyle[256];
	long    npLoc[256];
	short   nSpan=5;
	short	bStyle;
	short   nFirstLoc;
	short   nSecendLoc;
	short   nThirdLoc;
	short   i,j;
	if(nDataNum<3*nSpan||nNo<3*nSpan-1) return nStyleKind;
	for(i=0; i<34; i++)
	{	if(isTrendK_Style(tmpInfo,pAnalyData,nDataNum,nSpan,nNo,TrendCodeTable[i])!=0) nTrendSet[i] = TRUE;
		else                                                                   nTrendSet[i] = FALSE;
	}
	CLE_CONTABLE *tmpTable;
	int tmpInt;
	for(i=0; i<121; i++)
	{
		if(i <= 31)
		{
			tmpTable = g_pCleKLine1;
			tmpInt = i;
		}
		else if(i <= 58)
		{
			tmpTable = g_pCleKLine2;
			tmpInt = i-32;
		}
		else if(i <= 93)
		{
			tmpTable = g_pCleKLine3;
			tmpInt = i-59;
		}
		else 
		{
			tmpTable = g_pCleKLine4;
			tmpInt = i-94;
		}
		if(nTrendSet[tmpTable[tmpInt].Status-1]==TRUE)
		{	bStyle = FALSE;
			switch(tmpTable[tmpInt].MsgNo)
			{
			case KLINE_1001://�͵�������������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDUP,2,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1002://�͵�������������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDDOWN,2,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1003://�͵������н�����
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,0,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1004://�͵������й�����
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,0,KLINEO_SMALL,KLINEO_UVLSHADOW,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1005://�͵�����������������
				nSecendLoc = nSpan;
				nFirstLoc  = nNo;
				do
				{	nFirstLoc = E_OddK_Style(pAnalyData,nSecendLoc,nFirstLoc,KLINEO_UP,KLINEO_BIG,0,1,1);
					if(nFirstLoc>0&&nFirstLoc<nNo&&
					   OddK_Size  (pAnalyData,nFirstLoc-1)==KLINEO_SMALL&&
					   OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_DOWN &&
					   pAnalyData[nFirstLoc].Close<pAnalyData[nFirstLoc-1].Open )
						{ bStyle=TRUE; break; }
					nSecendLoc--;
					nFirstLoc --;
				}
				while(nFirstLoc!=-1&&nSecendLoc>0);
				break;
			case KLINE_1006://�͵�����������������
				nSecendLoc = nSpan;
				nFirstLoc  = nNo;
				do
				{	nFirstLoc = E_OddK_Style(pAnalyData,nSecendLoc,nFirstLoc,KLINEO_DOWN,KLINEO_BIG,0,1,1);
					if(nFirstLoc>0&&nFirstLoc<nNo&&
					   OddK_Size  (pAnalyData,nFirstLoc-1)==KLINEO_SMALL&&
					   OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP   &&
					   pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-1].Open )
						{ bStyle=TRUE; break; }
					nSecendLoc--;
					nFirstLoc --;
				}
				while(nFirstLoc!=-1&&nSecendLoc>0);
				break;
			case KLINE_1007://�͵�����ĩ�ڹ�����
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,0,KLINEO_SMALL,KLINEO_UVLSHADOW,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1008://�͵�����ĩ�ڽ�����
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,0,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1009://�͵�����ĩ�ڴ���ͻ��
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_BIG,0,1,1);
				if(nFirstLoc==nNo)  { bStyle=TRUE; break; }
				break;
			case KLINE_1010://С��������������
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_BIG,0,1,1);
				if(nFirstLoc==nNo&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,3,0)==nNo-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1011://�͵�����ĩ�ں�����
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_SMALL,0,3,1);
				if(nFirstLoc==nNo)  { bStyle=TRUE; break; }
				break;
			case KLINE_1012://�͵�����������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,3,1);
				if(nFirstLoc>1&&OddK_Size(pAnalyData,nFirstLoc-2)!=KLINEO_SMALL&&
				   OddK_Size(pAnalyData,nFirstLoc-1)!=KLINEO_SMALL&&
				   OddK_Size(pAnalyData,nFirstLoc)!=KLINEO_SMALL)
					 { bStyle=TRUE; break; }
				break;
			case KLINE_1013://��������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,2,1);
				if(nFirstLoc>0&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG&&
				   OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_BIG&&
				   OddK_Size(pAnalyData,nFirstLoc-2)==KLINEO_BIG)
					 { bStyle=TRUE; break; }
				break;
			case KLINE_1014://�͵�����������ʮ����
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&OddK_UpDown(pAnalyData,nFirstLoc)==KLINEO_LEVEL&&
				   OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOHEADFOOT&&
				   OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOHEAD    &&
				   OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOFOOT    )
					 { bStyle=TRUE; break; }
				break;
			case KLINE_1015://�͵�����������������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL&&
				   OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_DVLSHADOW)
					 { bStyle=TRUE; break; }
				break;
			case KLINE_1016://�͵�����������������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL&&
				   OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_UVLSHADOW)
					 { bStyle=TRUE; break; }
				break;
			case KLINE_1017://�͵�������ͷ����(���³�Ӱ)
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-1].Low)  { bStyle=TRUE; break; }
				break;
			case KLINE_1018://�͵������������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FAKECUTIN,1,1);
				if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG)  { bStyle=TRUE; break; }
				break;
			case KLINE_1019://�͵������������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
				if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG)  { bStyle=TRUE; break; }
				break;
			case KLINE_1020://�͵�������������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUD,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1021://�͵�����������������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDU,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1022://�͵�������������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDU,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1023://�͵�������������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDD,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1024://�͵�����������ʮ����
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDL,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1025://�͵����������й�����
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDD,1,1);
				while(nFirstLoc>=0)
				{   if(OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL&&
					   OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_UVLSHADOW)
					{	bStyle=TRUE;	break;	}
					else	nFirstLoc = E_TwinK_Style(pAnalyData,nSpan-(nNo-nFirstLoc),nFirstLoc-1,KLINET_GDD,1,1);
				}
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDU,1,1);
				while(nFirstLoc>=0)
				{   if(OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL&&
					   OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_UVLSHADOW)
					{	bStyle=TRUE;	break;	}
					else	nFirstLoc = E_TwinK_Style(pAnalyData,nSpan-(nNo-nFirstLoc),nFirstLoc-1,KLINET_GDU,1,1);
				}
				break;
			case KLINE_1026://�͵����������н�����
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDD,1,1);
				while(nFirstLoc>=0)
				{   if(OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL&&
					   OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_DVLSHADOW)
					{	bStyle=TRUE;	break;	}
					else	nFirstLoc = E_TwinK_Style(pAnalyData,nSpan-(nNo-nFirstLoc),nFirstLoc-1,KLINET_GDD,1,1);
				}
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDU,1,1);
				while(nFirstLoc>=0)
				{   if(OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL&&
					   OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_DVLSHADOW)
					{	bStyle=TRUE;	break;	}
					else	nFirstLoc = E_TwinK_Style(pAnalyData,nSpan-(nNo-nFirstLoc),nFirstLoc-1,KLINET_GDU,1,1);
				}
				break;
			case KLINE_1027://�͵�������������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUTIN,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1028://�͵�������������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_NDU,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1029://�͵�������������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGUP,1,1);
				if(nFirstLoc>0&&OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_BIG)  { bStyle=TRUE; break; }
				break;
			case KLINE_1030://�͵�����������������
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_SMALL,KLINEO_DVLSHADOW,2,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1031://�͵����ξѻ�������
				nSecendLoc = E_TwinK_Style(pAnalyData,3*nSpan,nNo,KLINET_ASCENT,2,1);
				if(nSecendLoc!=-1)
				{	nFirstLoc = E_TwinK_Style(pAnalyData,3*nSpan,nSecendLoc+2,KLINET_SLIDE,1,0);
					if(nFirstLoc!=-1&&nFirstLoc==nSecendLoc+2)
					{	nThirdLoc = E_TwinK_Style(pAnalyData,3*nSpan+nSecendLoc-nNo,nSecendLoc-3,KLINET_SLIDE,1,0);
						if(nThirdLoc!=-1&&nThirdLoc<nSecendLoc)
						{ bStyle=TRUE; break; }
					}
				}
				break;
			case KLINE_1032://�ϵ����������ͻ��
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
				if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG)  { bStyle=TRUE; break; }
				break;
			case KLINE_1033://����һ���ߺ󲻲�ȱ
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
				if(nFirstLoc!=-1&&E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_LEVEL,KLINEO_SMALL,KLINEO_NOHEADFOOT,1,1)==nFirstLoc)  { bStyle=TRUE; break; }
				break;
			case KLINE_1034://���θ߿�����
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,2,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1035://������Ԯ��
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
				if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,1,1)==nFirstLoc+1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1036://����������
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,0,0,3,1);
				if(nFirstLoc>2&&
				   (pAnalyData[nFirstLoc].Close-pAnalyData[nFirstLoc].Open)>(pAnalyData[nFirstLoc-1].Close-pAnalyData[nFirstLoc-1].Open+COMPPREC) &&
				   (pAnalyData[nFirstLoc-1].Close-pAnalyData[nFirstLoc-1].Open)>(pAnalyData[nFirstLoc-2].Close-pAnalyData[nFirstLoc-2].Open+COMPPREC))
					 { bStyle=TRUE; break; }
				break;
			case KLINE_1037://������;����ʮ����
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
				if(nFirstLoc!=-1&&OddK_UpDown(pAnalyData,nFirstLoc)==KLINEO_LEVEL&&
				   OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOHEADFOOT&&
				   OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOHEAD    &&
				   OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOFOOT    )
					 { bStyle=TRUE; break; }
				break;
			case KLINE_1038://������;���߽�����
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
				if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_DVLSHADOW&&
				   OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL)
					 { bStyle=TRUE; break; }
				break;
			case KLINE_1039://������;���߹�����
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
				if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_UVLSHADOW&&
				   OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL)
					 { bStyle=TRUE; break; }
				break;
			case KLINE_1040://������;Сѹ����
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CRACK,1,1);
				if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL)  { bStyle=TRUE; break; }
				break;
			case KLINE_1041://������;α����
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNDOWN,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1042://������;������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUD,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1043://������;��˳��
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SURRENDER,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1044://������;������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGUP,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1045://������;������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUD,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1046://������;������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUTIN,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1047://������;������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUU,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1048://������;������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDUP,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1049://������;������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1050://����������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,2,1);
				if(nFirstLoc>0&&nFirstLoc<nNo&&
				   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_DOWN&&
				   pAnalyData[nFirstLoc-2].Open>pAnalyData[nFirstLoc+1].Close)
					{ nFirstLoc++; bStyle=TRUE; break; }
				break;
			case KLINE_1051://��������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1);
				if(nFirstLoc>2&&nFirstLoc<nNo&&
				   OddK_UpDown(pAnalyData,nFirstLoc-3)==KLINEO_UP&&
				   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_UP&&
				   pAnalyData[nFirstLoc-2].Open<pAnalyData[nFirstLoc+1].Close)
				{ nFirstLoc++; bStyle=TRUE; break; }
				break;
			case KLINE_1052://������������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,2,1);
				if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGDOWN,1,1)==nFirstLoc+1)
				{ nFirstLoc++; bStyle=TRUE; break; }
				break;
			case KLINE_1053://��Խ�����߻�ѹ����
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,1,1);
				while(nFirstLoc>3)
				{	if(E_TwinK_Style(pAnalyData,nSpan-nNo+nFirstLoc,nFirstLoc-1,KLINET_SLIDE,2,1)==nFirstLoc-2&&
					   pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-4].Open) { bStyle=TRUE; break; }
					nFirstLoc = E_TwinK_Style(pAnalyData,nSpan-nNo+nFirstLoc,nFirstLoc-1,KLINET_ASCENT,1,1);
				}
				break;
			case KLINE_1054://������; ���������
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,0,0,3,1);
				if(nFirstLoc!=-1&&
				  (pAnalyData[nFirstLoc].Close-pAnalyData[nFirstLoc].Open)<(pAnalyData[nFirstLoc-1].Close-pAnalyData[nFirstLoc-1].Open) &&
				  (pAnalyData[nFirstLoc-1].Close-pAnalyData[nFirstLoc-1].Open)<(pAnalyData[nFirstLoc-2].Close-pAnalyData[nFirstLoc-2].Open))
					{ bStyle=TRUE; break; }
				break;
			case KLINE_1055://������; ˼��������
				nFirstLoc = nNo;
				do
				{   if(nFirstLoc>1&&nFirstLoc>nNo-nSpan)
					{	if(OddK_UpDown(pAnalyData,nFirstLoc  )==KLINEO_UP &&
						   OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP &&
						   OddK_UpDown(pAnalyData,nFirstLoc-2)==KLINEO_UP &&
						   OddK_Size(pAnalyData,nFirstLoc  )==KLINEO_SMALL&&
						   OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_BIG  &&
						   OddK_Size(pAnalyData,nFirstLoc-2)==KLINEO_SMALL)
						{	bStyle=TRUE; break;
						}
					}
					nFirstLoc--;
				}
				while(nFirstLoc>1&&nFirstLoc>nNo-nSpan);
				break;
			case KLINE_1056://������;������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_NDU,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1057://������;������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUU,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1058://������;α����
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNUP,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1059://�ߵ�������������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDUP,2,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1060://�ߵ������������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDDOWN,2,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1061://����������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
				if(nFirstLoc>=0&&nFirstLoc+1==E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_BIG,0,1,1) &&
				   pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-2].Open) { bStyle=TRUE; break; }
				break;
			case KLINE_1062://�ߵ�����������������
				nSecendLoc = nSpan;
				nFirstLoc  = nNo;
				do
				{	nFirstLoc = E_OddK_Style(pAnalyData,nSecendLoc,nFirstLoc,KLINEO_DOWN,KLINEO_BIG,0,1,1);
					if(nFirstLoc>0&&nFirstLoc<nNo&&
					   OddK_Size  (pAnalyData,nFirstLoc-1)==KLINEO_SMALL&&
					   OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP   &&
					   pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-1].Open )
						{ bStyle=TRUE; break; }
					nSecendLoc--;
					nFirstLoc --;
				}
				while(nFirstLoc!=-1&&nSecendLoc>0);
				break;
			case KLINE_1063://�ߵ������й�����
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,0,KLINEO_SMALL,KLINEO_UVLSHADOW,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1064://�ߵ������н�����
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,0,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1065://�����߽ӻ�����
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
				if(nFirstLoc>3&&E_TwinK_Style(pAnalyData,nSpan,nFirstLoc-2,KLINET_SLIDE,1,1)==nFirstLoc-3&&
				   OddK_UpDown(pAnalyData,nFirstLoc-2)==KLINEO_UP)
					{ bStyle=TRUE; break; }
				break;
			case KLINE_1066://������ѻ
				nSecendLoc = nSpan;
				nFirstLoc  = nNo;
				do
				{	nFirstLoc = E_TwinK_Style(pAnalyData,nSecendLoc,nFirstLoc,KLINET_FEIGNDOWN,1,1);
					if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSecendLoc,nFirstLoc+1,KLINET_SLIDE,1,1)==nFirstLoc+1)
					{ bStyle=TRUE; break; }
					nSecendLoc--;
					nFirstLoc --;
				}
				while(nFirstLoc!=-1&&nSecendLoc>0);
				break;
			case KLINE_1067://�ߵ�����ĩ�ڽ�����
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,0,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1068://�ߵ�����ĩ�ڴ�������
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1069://�ߵ�����ĩ�ں�����
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1070://С���������������
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,1,1);
				if(nFirstLoc>5&&pAnalyData[nFirstLoc].Close<MIN(pAnalyData[nFirstLoc-6].Close,pAnalyData[nFirstLoc-6].Open)) { bStyle=TRUE; break; }
				break;
			case KLINE_1071://������ѻ
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1072://��¥��
				nFirstLoc  = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1);
				nSecendLoc = E_TwinK_Style(pAnalyData,nSpan,nFirstLoc,KLINET_ASCENT,2,1);
				if(nFirstLoc>10&&nSecendLoc<nFirstLoc&&nSecendLoc>nFirstLoc-10) { bStyle=TRUE; break; }
				break;
			case KLINE_1073://�ߵ�����������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,3,1);
				if(nFirstLoc>1&&OddK_Size(pAnalyData,nFirstLoc-2)!=KLINEO_SMALL&&
				   OddK_Size(pAnalyData,nFirstLoc-1)!=KLINEO_SMALL&&
				   OddK_Size(pAnalyData,nFirstLoc)!=KLINEO_SMALL)
					{ bStyle=TRUE; break; }
				break;
			case KLINE_1074://��������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,3,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1075://�ߵ���ʮ����
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
				if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1)==nFirstLoc+2&&
				   OddK_Size(pAnalyData,nFirstLoc+1)==KLINEO_SMALL&&
				   OddK_Shadow(pAnalyData,nFirstLoc+1)!=KLINEO_NOHEAD&&
				   OddK_Shadow(pAnalyData,nFirstLoc+1)!=KLINEO_NOFOOT)
					{ bStyle=TRUE; break; }
				break;
			case KLINE_1076://�ߵ���������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ABANDON,1,1);
				if(nFirstLoc!=-1&&nFirstLoc<nNo)
				{	if(OddK_UpDown(pAnalyData,nFirstLoc)==KLINEO_UP       &&
					   OddK_Size  (pAnalyData,nFirstLoc)==KLINEO_SMALL    &&
					   OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_UVLSHADOW&&
					   pAnalyData[nFirstLoc+1].Close<pAnalyData[nFirstLoc].Close)
						{ bStyle=TRUE; break; }
				}
				break;
			case KLINE_1077://�ߵ���������
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_SMALL,KLINEO_DVLSHADOW,1,0);
				if(nFirstLoc>0&&nFirstLoc<nNo)
				{	if(OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP  &&
					   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_DOWN&&
					   pAnalyData[nFirstLoc-1].Close<pAnalyData[nFirstLoc].Close&&
					   pAnalyData[nFirstLoc+1].Open <pAnalyData[nFirstLoc].Close)
						{ bStyle=TRUE; break; }
				}
				break;
			case KLINE_1078://�ߵ���ͷ����
				nSecendLoc = nSpan;
				nFirstLoc  = nNo;
				do
				{	nFirstLoc = E_OddK_Style(pAnalyData,nSecendLoc,nFirstLoc,KLINEO_UP,0,0,1,1);
					if(nFirstLoc!=-1&&nFirstLoc<nNo&&
					   OddK_Size  (pAnalyData,nFirstLoc  )!=KLINEO_SMALL&&
					   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_UP   &&
					   pAnalyData[nFirstLoc+1].Open >pAnalyData[nFirstLoc].Close&&
					   pAnalyData[nFirstLoc+1].Close<pAnalyData[nFirstLoc].High )
					{ nFirstLoc++; bStyle=TRUE; break; }
					nSecendLoc--;
					nFirstLoc --;
				}
				while(nFirstLoc!=-1&&nSecendLoc>0);
				break;
			case KLINE_1079://����������
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,1,1);
				while(nFirstLoc>0)
				{   if(nFirstLoc<nNo&&OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP&&
					   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_UP&&
					   pAnalyData[nFirstLoc].Open>pAnalyData[nFirstLoc-1].Close&&
					   pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-1].Close-0.2*(pAnalyData[nFirstLoc-1].Close-pAnalyData[nFirstLoc-1].Open)&&
					   pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc+1].Close)
					{	nFirstLoc++;
						bStyle=TRUE;
						break;
					}
					else
						nFirstLoc = E_OddK_Style(pAnalyData,nSpan-(nNo-nFirstLoc),nFirstLoc-1,KLINEO_DOWN,KLINEO_BIG,0,1,1);
				}
				break;
			case KLINE_1080://����������
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_BIG,0,1,1);
				while(nFirstLoc>0)
				{   if(nFirstLoc<nNo&&OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP&&
					   pAnalyData[nFirstLoc].Open>MAX(pAnalyData[nFirstLoc-1].Open,pAnalyData[nFirstLoc-1].Close)&&
					   pAnalyData[nFirstLoc].Open>MAX(pAnalyData[nFirstLoc+1].Open,pAnalyData[nFirstLoc+1].Close))
					{	nFirstLoc++;
						bStyle=TRUE;
						break;
					}
					nFirstLoc = E_OddK_Style(pAnalyData,nSpan-(nNo-nFirstLoc),nFirstLoc-1,KLINEO_UP,KLINEO_BIG,0,1,1);
				}
				break;
			case KLINE_1081://�ߵ����ջ�����
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
				while(nFirstLoc>1)
				{   if(pAnalyData[nFirstLoc-1].Close>MAX(pAnalyData[nFirstLoc-2].Open,pAnalyData[nFirstLoc-2].Close))
					{	bStyle=TRUE;	break;	}
					nFirstLoc = E_TwinK_Style(pAnalyData,nSpan-(nNo-nFirstLoc),nFirstLoc-1,KLINET_SLIDE,1,1);
				}
				break;
			case KLINE_1082://�ߵ�������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDU,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1083://�ߵ�����������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUD,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1084://�ߵ�������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDD,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1085://�ߵ�����������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDD,1,1);
				if(nFirstLoc>0&&OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_SMALL&&OddK_Shadow(pAnalyData,nFirstLoc-1)==KLINEO_DVLSHADOW) { bStyle=TRUE; break; }
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDU,1,1);
				if(nFirstLoc>0&&OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_SMALL&&OddK_Shadow(pAnalyData,nFirstLoc-1)==KLINEO_DVLSHADOW) { bStyle=TRUE; break; }
				break;
			case KLINE_1086://�ߵ�������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUD,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1087://�ߵ�������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUU,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1088://�ߵ�����ʮ����
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUL,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1089://�ߵ����е�����
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUD,1,1);
				if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_DVLSHADOW)
					{ bStyle=TRUE; break; }
				break;
			case KLINE_1090://�ߵ�������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COVER,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1091://�ߵ�������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_NDU,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1092://�ߵ�������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGDOWN,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1093://�ߵ����ξѻ�������
				nSecendLoc = E_TwinK_Style(pAnalyData,3*nSpan,nNo,KLINET_SLIDE,2,1);
				if(nSecendLoc!=-1)
				{	nFirstLoc = E_TwinK_Style(pAnalyData,3*nSpan,nSecendLoc+2,KLINET_ASCENT,1,0);
					if(nFirstLoc!=-1&&nFirstLoc==nSecendLoc+2)
					{	nThirdLoc = E_TwinK_Style(pAnalyData,3*nSpan+nSecendLoc-nNo,nSecendLoc-3,KLINET_ASCENT,1,0);
						if(nThirdLoc!=-1&&nThirdLoc<nSecendLoc)
						{ bStyle=TRUE; break; }
					}
				}
				break;
			case KLINE_1094://�µ����������ͻ��
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc>0&&E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,1,0)<nFirstLoc-1)
					{ bStyle=TRUE; break; }
				break;
			case KLINE_1095://��ˮһ���ߺ󲻲�ȱ
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&
				   OddK_UpDown(pAnalyData,nFirstLoc)==KLINEO_LEVEL&&
				   OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_NOHEADFOOT)
					{ bStyle=TRUE; break; }
				break;
			case KLINE_1096://������������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,2,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1097://���ջ�����
			nSpan = nSpan;
				nSecendLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				while(nSecendLoc>1&&nSecendLoc<nNo)
				{   if(pAnalyData[nSecendLoc-1].Open <pAnalyData[nSecendLoc-2].Open &&
					   pAnalyData[nSecendLoc-1].Open >pAnalyData[nSecendLoc-2].Close&&
					   pAnalyData[nSecendLoc-1].Close<pAnalyData[nSecendLoc-2].Close&&
					   pAnalyData[nSecendLoc+1].Open <pAnalyData[nSecendLoc  ].Open &&
					   pAnalyData[nSecendLoc+1].Open >pAnalyData[nSecendLoc  ].Close&&
					   pAnalyData[nSecendLoc+1].Close<pAnalyData[nSecendLoc  ].Close)
					{	nFirstLoc=nSecendLoc+1; bStyle=TRUE; break;	}
					nSecendLoc = E_TwinK_Style(pAnalyData,nSpan-(nNo-nSecendLoc),nSecendLoc-1,KLINET_JMPDOWN,1,1);
				}
				break;
			case KLINE_1098://����������
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,0,0,3,1);
				if(nFirstLoc!=-1&&nFirstLoc>1)
				{   if(pAnalyData[nFirstLoc].Open-pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-1].Open-pAnalyData[nFirstLoc-1].Close&&
					   pAnalyData[nFirstLoc-1].Open-pAnalyData[nFirstLoc-1].Close>pAnalyData[nFirstLoc-2].Open-pAnalyData[nFirstLoc-2].Close)
						{ bStyle=TRUE; break; }
				}
				break;
			case KLINE_1099://�µ���;��ˮʮ����
				nFirstLoc = nNo;
				do
				{	if(nFirstLoc>1&&
					   MIN(pAnalyData[nFirstLoc-2].Close,pAnalyData[nFirstLoc-2].Open)>MAX(pAnalyData[nFirstLoc-1].Close,pAnalyData[nFirstLoc-1].Open) &&
					   MIN(pAnalyData[nFirstLoc-1].Close,pAnalyData[nFirstLoc-1].Open)>MAX(pAnalyData[nFirstLoc  ].Close,pAnalyData[nFirstLoc  ].Open) &&
					   OddK_Shadow(pAnalyData,nFirstLoc  )!=KLINEO_NOHEADFOOT&&
					   OddK_Shadow(pAnalyData,nFirstLoc  )!=KLINEO_NOHEAD    &&
					   OddK_Shadow(pAnalyData,nFirstLoc  )!=KLINEO_NOFOOT    &&
					   OddK_Shadow(pAnalyData,nFirstLoc-1)!=KLINEO_NOHEADFOOT&&
					   OddK_Shadow(pAnalyData,nFirstLoc-1)!=KLINEO_NOHEAD    &&
					   OddK_Shadow(pAnalyData,nFirstLoc-1)!=KLINEO_NOFOOT    &&
					   OddK_Size  (pAnalyData,nFirstLoc  )==KLINEO_SMALL	 &&
					   OddK_Size  (pAnalyData,nFirstLoc-1)==KLINEO_SMALL	 )
					{	bStyle=TRUE;
						break;
					}
					nFirstLoc--;
				}
				while(nFirstLoc>1&&nFirstLoc>nNo-nSpan);
				break;
			case KLINE_1100://�µ���;��ˮ������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_ULSHADOW)
					{ bStyle=TRUE; break; }
				break;
			case KLINE_1101://�µ���;��ˮ������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_DVLSHADOW)
					{ bStyle=TRUE; break; }
				break;
			case KLINE_1102://�µ���;С�����
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FLATTER,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1103://�µ���;α����
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNUP,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1104://�µ���;������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDU,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1105://�µ���;������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_REBELL,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1106://�µ���;������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGDOWN,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1107://�µ���;������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDU,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1108://�µ���;������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COVER,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1109://�µ���;������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDD,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1110://�µ���;������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDUP,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1111://�µ���;������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDDOWN,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1112://�µ���;��Ԯ��
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,1,1);
				if(nFirstLoc==nNo)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1113://�½�������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1);
				if(nFirstLoc!=-1&&nFirstLoc<nNo&&
				   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_UP&&
				   pAnalyData[nFirstLoc-2].Open<pAnalyData[nFirstLoc+1].Close)
				{ nFirstLoc++; bStyle=TRUE; break; }
				break;
			case KLINE_1114://�½�����
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,2,1);
				if(nFirstLoc>2&&nFirstLoc<nNo&&
				   OddK_UpDown(pAnalyData,nFirstLoc-3)==KLINEO_DOWN&&
				   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_DOWN&&
				   pAnalyData[nFirstLoc-2].Open>pAnalyData[nFirstLoc+1].Close)
				{ nFirstLoc++; bStyle=TRUE; break; }
				break;
			case KLINE_1115://�½���������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&nFirstLoc<nNo&&
				   E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGUP,1,1)==nFirstLoc+1)
					{ bStyle=TRUE; break; }
				break;
			case KLINE_1116://��Խ�����߻�����
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUTIN,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FLATTER,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1117://�µ���;���������
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,0,0,3,1);
				if(nFirstLoc!=-1)
				{   if(pAnalyData[nFirstLoc].Open-pAnalyData[nFirstLoc].Close<pAnalyData[nFirstLoc-1].Open-pAnalyData[nFirstLoc-1].Close&&
					   pAnalyData[nFirstLoc-1].Open-pAnalyData[nFirstLoc-1].Close<pAnalyData[nFirstLoc-2].Open-pAnalyData[nFirstLoc-2].Close)
						{ bStyle=TRUE; break; }
				}
				break;
			case KLINE_1118://�µ���;˼��������
				nFirstLoc = nNo;
				do
				{   if(nFirstLoc>1&&nFirstLoc>nNo-nSpan)
					{	if(OddK_UpDown(pAnalyData,nFirstLoc  )==KLINEO_DOWN &&
						   OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_DOWN &&
						   OddK_UpDown(pAnalyData,nFirstLoc-2)==KLINEO_DOWN &&
						   OddK_Size(pAnalyData,nFirstLoc  )==KLINEO_SMALL  &&
						   OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_BIG    &&
						   OddK_Size(pAnalyData,nFirstLoc-2)==KLINEO_SMALL  )
						{	bStyle=TRUE; break;
						}
					}
					nFirstLoc--;
				}
				while(nFirstLoc>1&&nFirstLoc>nNo-nSpan);
				break;
			case KLINE_1119://�µ���;������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_NUD,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1120://�µ���;������
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDD,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1121://�µ���;α����
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNDOWN,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			default        :
				break;
			}
			if(bStyle)
			{   j=0;
				while(j<nStyleKind&&nFirstLoc<=npLoc[j]) j++;
				if(j<nStyleKind)
				{	memmove(&npStyle[j],&npStyle[j+1],sizeof(short)*(nStyleKind-j));
					memmove(&npLoc  [j],&npLoc  [j+1],sizeof(long )*(nStyleKind-j));
				}
				npStyle[j] = tmpTable[tmpInt].MsgNo;
				npLoc  [j] = nFirstLoc;
				nStyleKind++;
			}
		}
	}
	memmove(npoutStyle,npStyle,sizeof(short)*nMaxNum);
	if(nStyleKind!=0&&npLoc[0]!=nNo) return 0;
	return nStyleKind;
}

//���Իع����(����б��,fDiff�������ϵ��,fBase���ؽؾ�)
//���ع�������������ϵ��У��
//�������Գ̶ȱ�������fDiff�ж�
float CZnxg::LineRegress(float *fpOrigin,short nDataNum,float &fDiff,float &fBase)
{   
	float   fAverX;
	float	fAverY;
	float   fLxx;
	float   fLxy;
	float   fLyy;
	short   i;
	fAverX = (nDataNum-1.0)/2.0;
	fAverY = 0.0;
	for(i=0; i<nDataNum; i++)  fAverY+=fpOrigin[i];
	fAverY = fAverY/nDataNum;
	fLxx = 0.0;
	fLxy = 0.0;
	fLyy = 0.0;
	for(i=0; i<nDataNum; i++) fLxx += (i-fAverX)*(i-fAverX);
	for(i=0; i<nDataNum; i++) fLxy += (i-fAverX)*(fpOrigin[i]-fAverY);
	for(i=0; i<nDataNum; i++) fLyy += (fpOrigin[i]-fAverY)*(fpOrigin[i]-fAverY);
	fBase = fAverY-fAverX*fLxy/fLxx;
	fDiff = fLxy/sqrt(fLxx*fLyy);
	return fLxy/fLxx;
}

//ȡ�����Сֵ
void CZnxg::MaxMinVal(float *fpOrigin,short nDataNum,float &fMax,float &fMin)
{
	fMax = fpOrigin[0];
	fMin = fpOrigin[0];
	for(short i=0; i<nDataNum; i++)
	{   if(fpOrigin[i]>fMax) fMax=fpOrigin[i];
		if(fpOrigin[i]<fMin) fMin=fpOrigin[i];
	}
}

//�ж�һ��K�����Ƿ����ָ���ĵ���K����̬
//ʱ���ΪpAnalyData[nNo-nSpan+1<=(...)<=nNo]
//nMode==1Ϊ����ͳ��,nMode==0Ϊ���ж�����ͳ��
//������nKLineStyle��̬��K����̬�������
//���򷵻�-1
short CZnxg::E_OddK_Style(struct AnalyData *pAnalyData,short nSpan,short nNo,short nUpDown,short nSize,short nShadow,short nWeight,short nMode)
{	
	short   cCount=0;
	short   nFirstLoc;
	if(nSpan<0||nNo<0) return -1;
	for(short i=nNo; i>MAX(nNo-nSpan,0); i--)
	{	if((nUpDown==0||OddK_UpDown(pAnalyData,i)==nUpDown) &&
		   (nSize  ==0||OddK_Size  (pAnalyData,i)==nSize  ) &&
		   (nShadow==0||OddK_Shadow(pAnalyData,i)==nShadow) )
		{	cCount++;
			if(cCount==1) 		nFirstLoc=i;    //ȷ����һ�����ϵ���̬��λ��
			if(cCount>=nWeight) break;			//����ɲ���
		}
		else
		{	if(nMode==1)  cCount=0;		//����ͳ��
		}
	}
	if(cCount>=nWeight) return nFirstLoc;
	return -1;
}

//�ж�һ��K�����Ƿ����ָ����nWeight��K����̬
//ʱ���ΪpAnalyData[nNo-nSpan+1<=(...)<=nNo]
//nMode==1Ϊ����ͳ��,nMode==0Ϊ���ж�����ͳ��
//������nKStyle��̬��K����̬�������
//���򷵻�-1
short CZnxg::E_TwinK_Style(struct AnalyData *pAnalyData,short nSpan,short nNo,short nKStyle,short nWeight,short nMode)
{	
	short   cCount=0;
	short   nFirstLoc;
	for(short i=nNo; i>MAX(nNo-nSpan,0); i--)
	{	if(isTwinK_Style(pAnalyData,i,nKStyle)==nKStyle)
		{
			cCount++;
			if(cCount==1) 		nFirstLoc=i;    //ȷ����һ�����ϵ���̬��λ��
			if(cCount>=nWeight) break;			//����ɲ���
		}
		else
		{	if(nMode==1)  cCount=0;		//����ͳ��
		}
	}
	if(cCount>=nWeight) return nFirstLoc;
	return -1;
}

//�ο�����:
//1.����		���ȱ�
//2.�Ƿ�		���ȱ�
//3.����		�ٶȱ�,ˮƽ��
//4.��λ		��λ��
//5.����		���ű�
//6.�ܶ�		�ܶ�

//�ж�pAnalyData[0,...,nDataNum-1]�����ΪnNo,���ΪnSpan��K�����ε���̬
//�����ƶ�ƽ���ߵ͵�������6*nSpan����,
short CZnxg::isTrendK_Style(MemStkInfo *tmpInfo,struct AnalyData *pAnalyData,short nDataNum,short nSpan,short nNo,short nKStyle)
{	
	short    naLoc[60*2];     //��ĩλ��
	short	 naLen[60*2];     //�γ�
	float    faSlope[60*2];	//б��(�ٶ�)
	float    faWeight[60*2];  //��Ȩ��(�Ƿ���,��Ա���)
	float    faDiff[60*2];  	//���Ƿ�
	float    fPriceLv;		//��λ��
	short    nNodeNum;
	short    nNearNode;
	short    nLastNear;
	short    i,j;
	if(nDataNum<6*nSpan) return 0;

	HISDAT *tmpHisData = new HISDAT[nDataNum];
	memset(tmpHisData, 0, nDataNum*sizeof(HISDAT));
	RawData2CalcData(tmpInfo->Code,tmpInfo->setcode,PER_DAY,pAnalyData,tmpHisData,nDataNum,g_NoTime); //ת��TCALC����ʶ������
	int 	nRealNum = g_pCalc->_CallDTopViewCalc_3(tmpInfo->Code,tmpInfo->setcode,PER_DAY,tmpHisData,nDataNum);
	delete []tmpHisData;
	if(nRealNum < 0)  return 0;
	float *pOutData[MAX_LINEOUT];
	for(j=0; j<MAX_LINEOUT;j++) 
		pOutData[j] = g_pCalc->_CallDTopViewCalc_13(nRealNum);
	int IndexNo = g_pCalc->_CallDTopViewCalc_9(ZB_TYPE,"ZXNH");
	if(IndexNo < 0)
	{
		for (j=0; j<MAX_LINEOUT; j++) g_pCalc->_CallDTopViewCalc_14(pOutData[j]);
		return 0;
	}
	g_pCalc->_CallDTopViewCalc_5(ZB_TYPE, IndexNo, pOutData);

	nLastNear	= nRealNum-1;
	nNearNode	= nRealNum-2;
	nNodeNum	= 0;
	while(nNearNode>=0&&nNearNode<nRealNum)
	{	while(nNearNode>=0&&nNearNode<nRealNum)
		{	if(nNearNode==0) break;
			if(fabs(pOutData[0][nNearNode])<0.05) { nNearNode--; continue; }
			if(nLastNear-nNearNode<2) { nNearNode--; continue; }
			break;
		}
		if(nNearNode>=0&&nNearNode<nRealNum)
		{
			naLoc[nNodeNum]     = nNearNode;
			naLen[nNodeNum]		= nLastNear-nNearNode+1;
			faDiff[nNodeNum]	= pAnalyData[nLastNear].Close-pAnalyData[nNearNode].Close;
			if(naLen[nNodeNum]*pAnalyData[nNearNode].Close>COMPPREC)
				faSlope[nNodeNum]	= faDiff[nNodeNum]/(naLen[nNodeNum]*pAnalyData[nNearNode].Close);
			faWeight[nNodeNum]  = 0.0;
			for(i=nNearNode; i<=nLastNear; i++)
			{
				if(pAnalyData[nLastNear].Close>COMPPREC)
					faWeight[nNodeNum] += (pAnalyData[i].Close-pAnalyData[nNearNode].Close)/pAnalyData[nLastNear].Close;
			}
			nNodeNum++;
			nLastNear = nNearNode;
			nNearNode --;
		}
	}
	for (j=0; j<MAX_LINEOUT; j++) g_pCalc->_CallDTopViewCalc_14(pOutData[j]);

	float	fMax;
	float	fMin;
	fMax = pAnalyData[0].Close;
	fMin = pAnalyData[0].Close;
	for(i=1; i<nDataNum; i++)
	{   if(fMax<pAnalyData[i].Close) fMax = pAnalyData[i].Close;
		if(fMin>pAnalyData[i].Close) fMin = pAnalyData[i].Close;
	}
	fPriceLv = 0.0;
	for(i=nDataNum-5; i<nDataNum; i++)	fPriceLv += pAnalyData[i].Close;
	fPriceLv = fPriceLv/5.0;
	if(fabs(fMax-fMin)>COMPPREC) fPriceLv = (fPriceLv-fMin)/(fMax-fMin);

	float HIGHPRICERATE	=	 0.80f;			//�ߵ�
	float LOWPRICERATE	=	 0.30f;			//�͵�
	float UPLIMIT		=	 COMPPREC;		//����;��(����)
	float DOWNLIMIT		=	-COMPPREC;		//�µ�;��(����)
	float UPLIMITW		=	 COMPPREC;		//����;��(Ȩ��)
	float DOWNLIMITW	=	-COMPPREC;		//�µ�;��(Ȩ��)
	float DULLRATE		=	 0.15f*nDataNum;//���̱���
	float DULLLEN		=    0.90f*nSpan;	//���̿��
	float FASTUPLIMIT	=	 0.03f;			//����(����)
	float FASTDOWNLIMIT	=	-0.03f;			//����(����)
	float SLOWUPLIMIT	=	 0.025f;		//����(����)
	float SLOWDOWNLIMIT	=	-0.025f;		//����(����)
	float FASTUPLIMITW	=	 0.03f;			//����(Ȩ��)
	float FASTDOWNLIMITW=	-0.03f;			//����(Ȩ��)
	float SLOWUPLIMITW	=	 0.025f;		//����(Ȩ��)
	float SLOWDOWNLIMITW=	-0.025f;		//����(Ȩ��)
	switch(nKStyle)
	{
	case KLINETL_HIGH:
	//�ߵ�(*),���ھ�����Գ������������ļ۶���,
		if(fPriceLv>HIGHPRICERATE)	return nKStyle;
		break;
	case KLINETL_LOW:
	//�͵�(*),���ھ�����Գ������������ļ۶���,
		if(fPriceLv<LOWPRICERATE)	return nKStyle;
		break;
	case KLINETL_UP:
	//����;��(*),���ھ�����Գ������������ļ۶���,
		if(fPriceLv>HIGHPRICERATE)	return 0;
		if(fPriceLv<LOWPRICERATE )	return 0;
		if((faSlope[0]>UPLIMIT||faWeight[0]>UPLIMITW)&&(naLen[0]>DULLLEN||nNodeNum<DULLRATE))
			return nKStyle;
		break;
	case KLINETL_DOWN:
	//�µ�;��(*),���ھ�����Գ������������ļ۶���
		if(fPriceLv>HIGHPRICERATE)	return 0;
		if(fPriceLv<LOWPRICERATE )	return 0;
		if((faSlope[0]<DOWNLIMIT||faWeight[0]<DOWNLIMITW)&&(naLen[0]>DULLLEN||nNodeNum<DULLRATE))
			return nKStyle;
		break;
	case KLINETL_TOSS:
	case KLINETL_DULL:
	//����(*),�Դ����ζ���
		if(nNodeNum>DULLRATE)	return nKStyle;
		if(faWeight[0]<UPLIMITW&&faWeight[0]>DOWNLIMITW) return nKStyle;
		break;
	case KLINETL_HIGHTOSS:
	//�ߵ���(*)
	case KLINETL_HIGHDULL:
	//�ߵ�����(*)
		if(fPriceLv<HIGHPRICERATE)	return 0;
		if(nNodeNum>DULLRATE)	return nKStyle;
		if(faWeight[0]<UPLIMITW&&faWeight[0]>DOWNLIMITW) return nKStyle;
		break;
	case KLINETL_LOWTOSS:
	//�͵���(*)
	case KLINETL_LOWDULL:
	//�͵�����(*)
		if(fPriceLv>LOWPRICERATE)	return 0;
		if(nNodeNum>DULLRATE)	return nKStyle;
		if(faWeight[0]<UPLIMITW&&faWeight[0]>DOWNLIMITW) return nKStyle;
		break;
	case KLINETL_FASTUP:
	//����(*)
		if(faSlope[0]>FASTUPLIMIT)   return nKStyle;
		break;
	case KLINETL_FASTDOWN:
	//����(*)
		if(faSlope[0]<FASTDOWNLIMIT) return nKStyle;
		break;
	case KLINETL_SLOWUP:
	//����(*)
		if(faSlope[0]>SLOWUPLIMIT&&faWeight[0]>SLOWUPLIMITW) return 0;
		if(faSlope [0]>UPLIMIT)		return nKStyle;
		if(faWeight[0]>UPLIMITW)	return nKStyle;
		break;
	case KLINETL_SLOWDOWN:
	//����(*)
		if(faSlope[0]<SLOWDOWNLIMIT&&faWeight[0]<SLOWDOWNLIMITW) return 0;
		if(faSlope [0]<DOWNLIMIT)		return nKStyle;
		if(faWeight[0]<DOWNLIMITW)		return nKStyle;
		break;
	case KLINETL_HIGHFASTUP:
	//�ߵ�����
		if(fPriceLv<HIGHPRICERATE)		return 0;
		if(faSlope[0]>FASTUPLIMIT)   	return nKStyle;
		break;
	case KLINETL_HIGHFASTDOWN:
	//�ߵ�����
		if(fPriceLv<HIGHPRICERATE)		return 0;
		if(faSlope[0]<FASTDOWNLIMIT) 	return nKStyle;
		break;
	case KLINETL_HIGHSLOWUP:
	//�ߵ�����
		if(fPriceLv<HIGHPRICERATE) return 0;
		if(faSlope[0]<SLOWUPLIMIT&&faSlope[0]>UPLIMIT)			return nKStyle;
		if(faWeight[0]<SLOWUPLIMITW&&faWeight[0]>UPLIMITW)		return nKStyle;
		break;
	case KLINETL_HIGHSLOWDOWN:
	//�ߵ�����
		if(fPriceLv<HIGHPRICERATE)		return 0;
		if(faSlope[0]>SLOWDOWNLIMIT&&faSlope[0]<DOWNLIMIT)		return nKStyle;
		if(faWeight[0]>SLOWDOWNLIMITW&&faWeight[0]<DOWNLIMITW)	return nKStyle;
		break;
	case KLINETL_LOWFASTUP:
	//�͵�����
		if(fPriceLv>LOWPRICERATE)		return 0;
		if(faSlope[0]>FASTUPLIMIT)   	return nKStyle;
		break;
	case KLINETL_LOWFASTDOWN:
	//�͵�����
		if(fPriceLv>LOWPRICERATE)		return 0;
		if(faSlope[0]<FASTDOWNLIMIT) 	return nKStyle;
		break;
	case KLINETL_LOWSLOWUP:
	//�͵�����
		if(fPriceLv>LOWPRICERATE)		return 0;
		if(faSlope[0]<SLOWUPLIMIT&&faSlope[0]>UPLIMIT)			return nKStyle;
		if(faWeight[0]<SLOWUPLIMITW&&faWeight[0]>UPLIMITW)		return nKStyle;
		break;
	case KLINETL_LOWSLOWDOWN:
	//�͵�����
		if(fPriceLv>LOWPRICERATE)		return 0;
		if(faSlope[0]>SLOWDOWNLIMIT&&faSlope[0]<DOWNLIMIT)		return nKStyle;
		if(faWeight[0]>SLOWDOWNLIMITW&&faWeight[0]<DOWNLIMITW)	return nKStyle;
		break;

	case KLINETL_HIGHTOSS1:
	//�ߵ��𵴳���
		break;
	case KLINETL_HIGHTOSS2:
	//�ߵ�������
		break;
	case KLINETL_HIGHTOSS3:
	//�ߵ���ĩ��
		break;
	case KLINETL_LOWTOSS1:
	//�͵��𵴳���
		break;
	case KLINETL_LOWTOSS2:
	//�͵�������
		break;
	case KLINETL_LOWTOSS3:
	//�͵���ĩ��
		break;
	case KLINETL_HIGHDULL1:
	//�ߵ����̳���
		break;
	case KLINETL_HIGHDULL2:
	//�ߵ���������
		break;
	case KLINETL_HIGHDULL3:
	//�ߵ�����ĩ��
		break;
	case KLINETL_LOWDULL1:
	//�͵����̳���
		break;
	case KLINETL_LOWDULL2:
	//�͵���������
		break;
	case KLINETL_LOWDULL3:
	//�͵�����ĩ��
		break;
	}
	return 0;
}

short CZnxg::GetKLineStyleInside(struct AnalyData *pAnalyData,short nDataNum,short nSpan,short nNo,short nTrendKStyle)
{
	short   nFirstLoc;
	short   nSecendLoc;
	short   nThirdLoc;
	switch(nTrendKStyle)
	{
	case KLINETL_HIGH:
	//�ߵ�(*)
		//�ϵ����������ͻ��
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
		if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG) return KLINE_1032;
		//����һ���ߺ󲻲�ȱ
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
		if(nFirstLoc!=-1&&E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_LEVEL,KLINEO_SMALL,KLINEO_NOHEADFOOT,1,1)==nFirstLoc) return KLINE_1033;
		//���θ߿�����
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,2,1)!=-1) return KLINE_1034;
		//������Ԯ��
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
		if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,1,1)==nFirstLoc+1) return KLINE_1035;
		//�ߵ�������������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDUP,4,1)!=-1) return KLINE_1059;
		//�ߵ������������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDDOWN,4,1)!=-1) return KLINE_1060;
		//����������
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
		if(nFirstLoc>=0&&nFirstLoc+1==E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_BIG,0,1,1) &&
		   pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-2].Open) return KLINE_1061;
		//�ߵ�����������������
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,1,1);
		if(nFirstLoc>0&&nFirstLoc<nNo&&
		   OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_SMALL&&OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP&&
		   OddK_Size(pAnalyData,nFirstLoc+1)==KLINEO_SMALL&&OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_UP&&
		   pAnalyData[nFirstLoc].Open>pAnalyData[nFirstLoc-1].Open&& pAnalyData[nFirstLoc].Open>pAnalyData[nFirstLoc+1].Open &&
		   pAnalyData[nFirstLoc].Close<pAnalyData[nFirstLoc-1].Close&&pAnalyData[nFirstLoc].Close<pAnalyData[nFirstLoc+1].Close)
			return KLINE_1062;
		//�ߵ������й�����
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_SMALL,KLINEO_UVLSHADOW,1,1)!=-1) return KLINE_1063;
		//�ߵ������н�����
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1)!=-1) return KLINE_1064;
		//�����߽ӻ�����
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
		if(nFirstLoc>3&&E_TwinK_Style(pAnalyData,nSpan,nFirstLoc-2,KLINET_SLIDE,1,1)==nFirstLoc-3&&
		   OddK_UpDown(pAnalyData,nFirstLoc-2)==KLINEO_UP)
			return KLINE_1065;
		//������ѻ
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNDOWN,2,1);
		if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1)==nFirstLoc+1) return KLINE_1066;
		//С���������������
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,1,1)!=-1) return KLINE_1070;
		//������ѻ
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1)!=-1) return KLINE_1071;
		//��¥��
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,2,1);
		if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1)>nFirstLoc) return KLINE_1072;
		//�ߵ���ʮ����
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
		if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1)==nFirstLoc+2&&
		   OddK_Size(pAnalyData,nFirstLoc+1)==KLINEO_SMALL&&
		   OddK_Shadow(pAnalyData,nFirstLoc+1)!=KLINEO_NOHEAD&&
		   OddK_Shadow(pAnalyData,nFirstLoc+1)!=KLINEO_NOFOOT)
			return KLINE_1075;
		//�ߵ���������
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ABANDON,1,1);
		if(nFirstLoc!=-1&&nFirstLoc<nNo)
		{	if(OddK_UpDown(pAnalyData,nFirstLoc)==KLINEO_UP       &&
			   OddK_Size  (pAnalyData,nFirstLoc)==KLINEO_SMALL    &&
			   OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_UVLSHADOW&&
			   pAnalyData[nFirstLoc+1].Close<pAnalyData[nFirstLoc].Close)
				return KLINE_1076;
		}
		//�ߵ���ͷ����
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_BIG,KLINEO_ULSHADOW,1,1);
		if(nFirstLoc!=-1&&nFirstLoc<nNo)
		{   if(OddK_UpDown(pAnalyData,nFirstLoc)==KLINEO_UP&&
			   pAnalyData[nFirstLoc+1].Open >pAnalyData[nFirstLoc].Close&&
			   pAnalyData[nFirstLoc+1].Close<pAnalyData[nFirstLoc].High)
				return KLINE_1078;
		}
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_NORMAL,KLINEO_ULSHADOW,1,1);
		if(nFirstLoc!=-1&&nFirstLoc<nNo)
		{   if(OddK_UpDown(pAnalyData,nFirstLoc)==KLINEO_UP&&
			   pAnalyData[nFirstLoc+1].Open >pAnalyData[nFirstLoc].Close&&
			   pAnalyData[nFirstLoc+1].Close<pAnalyData[nFirstLoc].High)
				return KLINE_1078;
		}
		//����������
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,1,1);
		while(nFirstLoc>0)
		{   if(nFirstLoc<nNo&&OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP&&
			   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_UP&&
			   pAnalyData[nFirstLoc].Open>pAnalyData[nFirstLoc-1].Close&&
			   pAnalyData[nFirstLoc].Open>pAnalyData[nFirstLoc+1].Close)
				return KLINE_1079;
			nFirstLoc = E_OddK_Style(pAnalyData,nSpan-(nNo-nFirstLoc),nFirstLoc,KLINEO_DOWN,KLINEO_BIG,0,1,1);
		}
		//����������
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
		if(nFirstLoc!=-1)
		{	if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNUP,1,1)==nFirstLoc+1||
			   E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_REBELL ,1,1)==nFirstLoc+1)
				return KLINE_1080;
		}
		//�ߵ����ջ�����
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
		if(nFirstLoc>0)
		{	if(OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP&&
			   pAnalyData[nFirstLoc-1].Close<pAnalyData[nFirstLoc-1].Close)
				return KLINE_1081;
		}
		//�ߵ�������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDU,1,1)!=-1) return KLINE_1082;
		//�ߵ�����������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUD,1,1)!=-1) return KLINE_1083;
		//�ߵ�����������
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDD,1,1);
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_SMALL,KLINEO_DLSHADOW,1,1)==nFirstLoc&&
		   nFirstLoc!=-1)
			return KLINE_1085;
		//�ߵ�������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDD,1,1)!=-1) return KLINE_1084;
		//�ߵ�������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUU,1,1)!=-1) return KLINE_1087;
		//�ߵ�����ʮ����
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUL,1,1);
		if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOHEAD&&
		   OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOFOOT)
			return KLINE_1088;
		//�ߵ����е�����
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUD,1,1);
		if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_DVLSHADOW)
			return KLINE_1089;
		//�ߵ�������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUD,1,1)!=-1) return KLINE_1086;
		//�ߵ�������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COVER,1,1)!=-1) return KLINE_1090;
		//�ߵ�������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_NDU,1,1)!=-1) return KLINE_1091;
		//�ߵ�������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGDOWN,1,1)!=-1) return KLINE_1092;
		//�ߵ����ξѻ�������
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1);
		if(nFirstLoc!=-1)
		{	nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,1,0);
			if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,1,0)!=nFirstLoc)
			{   if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,0,0,1,0)>nFirstLoc) return KLINE_1093;
			}
		}
		break;
	case KLINETL_LOW:
	//�͵�(*)
		//�͵�������������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDUP,4,1)!=-1) return KLINE_1001;
		//�͵�������������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDDOWN,4,1)!=-1) return KLINE_1002;
		//�͵����ξѻ�������
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,2,1);
		if(nFirstLoc!=-1)
		{	nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,0);
			if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,0)!=nFirstLoc)
			{   if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,0,0,1,0)>nFirstLoc) return KLINE_1031;
			}
		}
		break;
	case KLINETL_TOSS:
	//��(*)
		break;
	case KLINETL_DULL:
	//����(*)
		break;
	case KLINETL_UP:
	//����;��(*)
		//����������
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,0,0,3,1);
		if(nFirstLoc>2&&
		   (pAnalyData[nFirstLoc].Close-pAnalyData[nFirstLoc].Open)>(pAnalyData[nFirstLoc-1].Close-pAnalyData[nFirstLoc-1].Open+COMPPREC) &&
		   (pAnalyData[nFirstLoc-1].Close-pAnalyData[nFirstLoc-1].Open)>(pAnalyData[nFirstLoc-2].Close-pAnalyData[nFirstLoc-2].Open+COMPPREC))
			return KLINE_1036;
		//������;����ʮ����
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,2,1);
		if(nFirstLoc!=-1&&E_OddK_Style(pAnalyData,nSpan,nNo,0,KLINEO_LEVEL,0,2,1)==nFirstLoc) return KLINE_1037;
		//������;���߽�����
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
		if(nFirstLoc!=-1&&E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1)==nFirstLoc) return KLINE_1038;
		//������;���߹�����
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_SMALL,KLINEO_UVLSHADOW,1,1);
		if(nFirstLoc>0&&OddK_UpDown(pAnalyData,nFirstLoc)==KLINEO_UP&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG) return KLINE_1039;
		//������;Сѹ����
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CRACK,1,1);
		if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL) return KLINE_1040;
		//������;α����
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNDOWN,1,1)!=-1) return KLINE_1041;
		//������;������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUD,1,1)!=-1) return KLINE_1042;
		//������;��˳��
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SURRENDER,1,1)!=-1) return KLINE_1043;
		//������;������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGUP,1,1)!=-1) return KLINE_1044;
		//������;������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUD,1,1)!=-1) return KLINE_1045;
		//������;������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUTIN,1,1)!=-1) return KLINE_1046;
		//������;������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUU,1,1)!=-1) return KLINE_1047;
		//������;������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDUP,3,1)!=-1) return KLINE_1048;
		//������;������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1)!=-1) return KLINE_1049;
		//����������
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,2,1);
		if(nFirstLoc>0&&nFirstLoc<nNo&&
		   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_DOWN&&
		   OddK_Size  (pAnalyData,nFirstLoc+1)==KLINEO_BIG&&
		   pAnalyData[nFirstLoc-2].Open>pAnalyData[nFirstLoc+1].Close)
			return KLINE_1050;
		//��������
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1);
		if(nFirstLoc!=-1&&nFirstLoc<nNo&&pAnalyData[nFirstLoc-3].Open<pAnalyData[nFirstLoc+1].Close) return KLINE_1051;
		//������������
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,2,1);
		if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGDOWN,1,1)==nFirstLoc+2) return KLINE_1052;
		//��Խ�����߻�ѹ����
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,1,1);
		if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,3,1)==nFirstLoc-5&&
		   pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-5].Open) return KLINE_1053;
		//������; ���������
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,0,0,3,1);
		if(nFirstLoc!=-1&&
		  (pAnalyData[nFirstLoc].Close-pAnalyData[nFirstLoc].Open)<(pAnalyData[nFirstLoc-1].Close-pAnalyData[nFirstLoc-1].Open) &&
		  (pAnalyData[nFirstLoc-1].Close-pAnalyData[nFirstLoc-1].Open)<(pAnalyData[nFirstLoc-2].Close-pAnalyData[nFirstLoc-2].Open))
			return KLINE_1054;
		//������; ˼��������
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,0,0,3,1);
		if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc-2)==KLINEO_SMALL&&
		  OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_BIG &&
		  OddK_Size(pAnalyData,nFirstLoc  )==KLINEO_SMALL)
			return KLINE_1055;
		//������;������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_NDU,1,1)!=-1) return KLINE_1056;
		//������;������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUU,1,1)!=-1) return KLINE_1057;
		//������;α����
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNUP,1,1)!=-1) return KLINE_1058;
		break;
	case KLINETL_DOWN:
	//�µ�;��(*)
		//������������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,2,1)!=-1)
			return KLINE_1096;
		//���ջ�����
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
		if(nFirstLoc>1)
		{	nSecendLoc = E_TwinK_Style(pAnalyData,nSpan,nFirstLoc-1,KLINET_JMPDOWN,1,1);
			if(nSecendLoc==nFirstLoc-1)
			{   nThirdLoc = E_TwinK_Style(pAnalyData,nSpan,nSecendLoc-1,KLINET_JMPDOWN,1,1);
				if(nThirdLoc>0&&nThirdLoc==nFirstLoc-1) return KLINE_1097;
			}
		}
		//����������
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,0,0,3,1);
		if(nFirstLoc!=-1)
		{   if(pAnalyData[nFirstLoc].Open-pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-1].Open-pAnalyData[nFirstLoc-1].Close&&
			   pAnalyData[nFirstLoc-1].Open-pAnalyData[nFirstLoc-1].Close<pAnalyData[nFirstLoc-2].Open-pAnalyData[nFirstLoc-2].Close)
				return KLINE_1098;
		}
		//�µ���;��ˮʮ����
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_LEVEL,0,0,3,1);
		if(nFirstLoc!=-1)
		{   if(OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOHEADFOOT&&
			   OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOHEAD    &&
			   OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOFOOT    &&
			   OddK_Shadow(pAnalyData,nFirstLoc-1)!=KLINEO_NOHEADFOOT&&
			   OddK_Shadow(pAnalyData,nFirstLoc-1)!=KLINEO_NOHEAD    &&
			   OddK_Shadow(pAnalyData,nFirstLoc-1)!=KLINEO_NOFOOT    &&
			   OddK_Shadow(pAnalyData,nFirstLoc-2)!=KLINEO_NOHEADFOOT&&
			   OddK_Shadow(pAnalyData,nFirstLoc-2)!=KLINEO_NOHEAD    &&
			   OddK_Shadow(pAnalyData,nFirstLoc-2)!=KLINEO_NOFOOT    )
				return KLINE_1099;
		}
		//�µ���;��ˮ������
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
		if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_ULSHADOW)
			return KLINE_1100;
		//�µ���;��ˮ������
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
		if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_DLSHADOW)
			return KLINE_1101;
		//�µ���;С�����
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FLATTER,1,1)!=-1)
			return KLINE_1102;
		//�µ���;α����
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNUP,1,1)!=-1)
			return KLINE_1103;
		//�µ���;������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDU,1,1)!=-1)
			return KLINE_1104;
		//�µ���;������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_REBELL,1,1)!=-1)
			return KLINE_1105;
		//�µ���;������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGDOWN,1,1)!=-1)
			return KLINE_1106;
		//�µ���;������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDU,1,1)!=-1)
			return KLINE_1107;
		//�µ���;������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COVER,1,1)!=-1)
			return KLINE_1108;
		//�µ���;������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDD,1,1)!=-1)
			return KLINE_1109;
		//�µ���;������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDUP,1,1)==nNo)
			return KLINE_1110;
		//�µ���;������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDDOWN,1,1)==nNo)
			return KLINE_1111;
		//�µ���;��Ԯ��
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,1,1)==nNo)
			return KLINE_1112;
		//�½�������
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1);
		if(nFirstLoc>0&&nFirstLoc<nNo&&
		   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_UP&&
		   OddK_Size  (pAnalyData,nFirstLoc+1)==KLINEO_BIG&&
		   pAnalyData[nFirstLoc-2].Open<pAnalyData[nFirstLoc+1].Close)
			return KLINE_1113;
		//�½�����
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,2,1);
		if(nFirstLoc>0&&nFirstLoc<nNo&&
		   OddK_Size  (pAnalyData,nFirstLoc-3)==KLINEO_BIG&&
		   OddK_Size  (pAnalyData,nFirstLoc+1)==KLINEO_BIG&&
		   OddK_UpDown(pAnalyData,nFirstLoc-3)==KLINEO_DOWN&&
		   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_DOWN)
			return KLINE_1114;
		//�½���������
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
		if(nFirstLoc!=-1&&nFirstLoc<nNo&&
		   E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGUP,1,1)==nFirstLoc+1)
			return KLINE_1115;
		//��Խ�����߻�����
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUTIN,1,1)!=-1)
			return KLINE_1116;
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FLATTER,1,1)!=-1)
			return KLINE_1116;
		//�µ���;���������
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,0,0,3,1);
		if(nFirstLoc!=-1)
		{   if(pAnalyData[nFirstLoc].Open-pAnalyData[nFirstLoc].Close<pAnalyData[nFirstLoc-1].Open-pAnalyData[nFirstLoc-1].Close&&
			   pAnalyData[nFirstLoc-1].Open-pAnalyData[nFirstLoc-1].Close<pAnalyData[nFirstLoc-2].Open-pAnalyData[nFirstLoc-2].Close)
				return KLINE_1117;
		}
		//�µ���;˼��������
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,0,0,5,1);
		if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL&&
		   OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_BIG)
			return KLINE_1118;
		//�µ���;������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_NUD,1,1)==nNo)
			return KLINE_1119;
		//�µ���;������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDD,1,1)==nNo)
			return KLINE_1120;
		//�µ���;α����
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNDOWN,1,1)==nNo)
			return KLINE_1121;
		break;
	case KLINETL_HIGHTOSS:
	//�ߵ���(*)
		break;
	case KLINETL_LOWTOSS:
	//�͵���(*)
		break;
	case KLINETL_HIGHDULL:
	//�ߵ�����(*)
		break;
	case KLINETL_LOWDULL:
	//�͵�����(*)
		//�͵������н�����
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1)!=-1) return KLINE_1003;
		//�͵������й�����
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_SMALL,KLINEO_UVLSHADOW,1,1)!=-1) return KLINE_1004;
		//�͵�����������������
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FAKECUTIN,1,1);
		if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG) return KLINE_1005;
		//�͵�����������������
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FAKECOVER,1,1);
		if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG) return KLINE_1006;
		//�͵�����ĩ�ں�����
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_SMALL,0,3,1)==nNo) return KLINE_1011;
		//�µ����������ͻ��
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
		if(nFirstLoc>0&&E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,1,0)<nFirstLoc-1)
			return KLINE_1094;
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
		if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_NOHEADFOOT)
			return KLINE_1095;
		break;
	case KLINETL_FASTUP:
	//����(*)
		break;
	case KLINETL_FASTDOWN:
	//����(*)
		break;
	case KLINETL_SLOWUP:
	//����(*)
		break;
	case KLINETL_SLOWDOWN:
	//����(*)
		break;
	case KLINETL_HIGHTOSS1:
	//�ߵ��𵴳���
		break;
	case KLINETL_LOWTOSS1:
	//�͵��𵴳���
		break;
	case KLINETL_HIGHTOSS2:
	//�ߵ�������
		break;
	case KLINETL_LOWTOSS2:
	//�͵�������
		break;
	case KLINETL_HIGHTOSS3:
	//�ߵ���ĩ��
		break;
	case KLINETL_LOWTOSS3:
	//�͵���ĩ��
		break;
	case KLINETL_HIGHDULL1:
	//�ߵ����̳���
		break;
	case KLINETL_LOWDULL1:
	//�͵����̳���
		break;
	case KLINETL_HIGHDULL2:
	//�ߵ���������
		break;
	case KLINETL_LOWDULL2:
	//�͵���������
		break;
	case KLINETL_HIGHDULL3:
	//�ߵ�����ĩ��
		//�ߵ�����ĩ�ڽ�����
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1)!=-1) return KLINE_1067;
		//�ߵ�����ĩ�ڴ�������
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,1,1)!=-1) return KLINE_1068;
		//�ߵ�����ĩ�ں�����
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,3,1)!=-1) return KLINE_1069;
		break;
	case KLINETL_LOWDULL3:
	//�͵�����ĩ��
		//�͵�����ĩ�ڹ�����
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_SMALL,KLINEO_UVLSHADOW,1,1)!=-1) return KLINE_1007;
		//�͵�����ĩ�ڽ�����
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1)!=-1) return KLINE_1008;
		//�͵�����ĩ�ڴ���ͻ��
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_BIG,0,1,1)==nNo) return KLINE_1009;
		break;
	case KLINETL_HIGHFASTUP:
	//�ߵ�����
		//�ߵ�����������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,3,1)!=-1) return KLINE_1073;
		//��������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,3,1)!=-1) return KLINE_1074;
		//�ߵ���������
		nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_SMALL,KLINEO_DLSHADOW,1,0);
		if(nFirstLoc>0&&nFirstLoc<nNo)
		{	if(OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP  &&
			   OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_DOWN&&
			   pAnalyData[nFirstLoc-1].Close<pAnalyData[nFirstLoc].Close&&
			   pAnalyData[nFirstLoc+1].Open <pAnalyData[nFirstLoc].Close)
				return KLINE_1077;
		}
		break;
	case KLINETL_HIGHFASTDOWN:
	//�ߵ�����
		break;
	case KLINETL_HIGHSLOWUP:
	//�ߵ�����
		break;
	case KLINETL_HIGHSLOWDOWN:
	//�ߵ�����
		break;
	case KLINETL_LOWFASTUP:
	//�͵�����
		break;
	case KLINETL_LOWFASTDOWN:
	//�͵�����
		//�͵�����������
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,3,1);
		if(nFirstLoc!=-1&&nFirstLoc==E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,3,1)) return KLINE_1012;
		//��������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,3,1)!=-1) return KLINE_1013;
		//�͵�����������ʮ����
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1);
		if( nFirstLoc<nNo &&
			E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1)==nFirstLoc+1 &&
			OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_LEVEL  &&
			OddK_Size  (pAnalyData,nFirstLoc+1)==KLINEO_SMALL  &&
			OddK_Shadow(pAnalyData,nFirstLoc+1)!=KLINEO_NOHEAD &&
			OddK_Shadow(pAnalyData,nFirstLoc+1)!=KLINEO_NOFOOT ) return KLINE_1014;
		//�͵�����������������
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
		if(nFirstLoc!=-1&&E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_SMALL,KLINEO_DVLSHADOW,1,0)==nFirstLoc+1&&
			pAnalyData[nFirstLoc+1].Close<pAnalyData[nFirstLoc].Open) return KLINE_1015;
		//�͵�����������������
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
		if(nFirstLoc!=-1&&E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_SMALL,KLINEO_UVLSHADOW,1,0)==nFirstLoc+1&&
			pAnalyData[nFirstLoc+1].Close<pAnalyData[nFirstLoc].Open) return KLINE_1016;
		//�͵�������ͷ����(���³�Ӱ)
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
		if(nFirstLoc!=-1&&pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-1].Low) return KLINE_1017;
		//�͵������������
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FAKECUTIN,1,1);
		if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG) return KLINE_1018;
		//�͵������������
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
		if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG) return KLINE_1019;
		//�͵�����������ʮ����
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDL,1,1);
		if(nFirstLoc!=-1&&nFirstLoc<nNo&&
			OddK_Shadow(pAnalyData,nFirstLoc+1)!=KLINEO_NOHEAD &&
			OddK_Shadow(pAnalyData,nFirstLoc+1)!=KLINEO_NOFOOT ) return KLINE_1024;
		//�͵����������й�����
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDD,1,1);
		if(nFirstLoc!=-1&&E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_SMALL,KLINEO_UVLSHADOW,1,1)==nFirstLoc) return KLINE_1025;
		//�͵����������н�����
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDU,1,1);
		if(nFirstLoc!=-1&&E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1)==nFirstLoc) return KLINE_1026;
		//�͵�������������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUTIN,1,1)!=-1) return KLINE_1027;
		//�͵�������������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_NDU,1,1)!=-1) return KLINE_1028;
		//�͵�������������
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGUP,1,1);
		if(nFirstLoc>0&&OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_BIG) return KLINE_1029;
		//�͵�����������������
		if(E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_SMALL,KLINEO_DVLSHADOW,2,1)!=-1) return KLINE_1030;
		//�͵�������������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUD,1,1)!=-1) return KLINE_1020;
		//�͵�����������������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDU,1,1)!=-1) return KLINE_1021;
		//�͵�������������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDU,1,1)!=-1) return KLINE_1022;
		//�͵�������������
		if(E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDD,1,1)!=-1) return KLINE_1023;
		break;
	case KLINETL_LOWSLOWUP:
	//�͵�����
		break;
	case KLINETL_LOWSLOWDOWN:
	//�͵�����
		//С��������������
		nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,4,0);
		if(nFirstLoc!=-1&&nFirstLoc==nNo-1&&E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_BIG,0,1,1)==nNo) return KLINE_1010;
		break;
	}
	return 0;
}

//ȡ��K������,����,ƽ����
//ƽ����Ϊ����ƽ��
short CZnxg::OddK_UpDown(struct AnalyData *pAnalyData,short nNo)
{
	if(fabs(pAnalyData[nNo].Close-pAnalyData[nNo].Open)<COMPPREC) return KLINEO_LEVEL;
	if(pAnalyData[nNo].Close>pAnalyData[nNo].Open) return KLINEO_UP;
	return KLINEO_DOWN;
}

//ȡ��K�ߴ�С��̬(��,��,С)
//��1/5Ϊ��
short CZnxg::OddK_Size(struct AnalyData *pAnalyData,short nNo)
{
	float   fRate;
	fRate = fabs(pAnalyData[nNo].Close-pAnalyData[nNo].Open)/pAnalyData[nNo].Close;
	if(fRate>0.08 ) return KLINEO_BIG;
	if(fRate<0.02 ) return KLINEO_SMALL;
	return KLINEO_NORMAL;
}

//ȡ��K��Ӱ����̬(ͺͷ,���,ͺͷ���...)
//ͺͷ���  ������Ӱ��
//��ͷ      ����Ӱ��
//���      ����Ӱ��
short CZnxg::OddK_Shadow(struct AnalyData *pAnalyData,short nNo)
{
	float fHeadRate;
	float fFootRate;
	fHeadRate = (pAnalyData[nNo].High-MAX(pAnalyData[nNo].Close,pAnalyData[nNo].Open))/fabs(pAnalyData[nNo].Close-pAnalyData[nNo].Open);
	fFootRate = (MIN(pAnalyData[nNo].Close,pAnalyData[nNo].Open)-pAnalyData[nNo].Low)/fabs(pAnalyData[nNo].Close-pAnalyData[nNo].Open);
	if(fHeadRate<COMPPREC&&fFootRate<COMPPREC) return KLINEO_NOHEADFOOT;
	if(fHeadRate<COMPPREC&&fFootRate>COMPPREC) return KLINEO_NOHEAD;
	if(fHeadRate>COMPPREC&&fFootRate<COMPPREC) return KLINEO_NOFOOT;
	if(fabs(fHeadRate-fFootRate)<COMPPREC  ) return KLINEO_ELSHADOW;
	if(fHeadRate>3.0&&fHeadRate/fFootRate>3.0) return KLINEO_UVLSHADOW;
	if(fFootRate>3.0&&fFootRate/fHeadRate>3.0) return KLINEO_DVLSHADOW;
	if(fHeadRate/fFootRate>1.0&&fFootRate>0.1) return KLINEO_ULSHADOW;
	return KLINEO_DLSHADOW;
}

//�ж�˫��K����̬
//�Ƿ�pAnalyData�����ΪnNo��K��Ƭ�κ���ǰһK��Ƭ�ξ���nKLineStyle��̬
//������ʱ����0,���򷵻�nKStyle
short CZnxg::isTwinK_Style(struct AnalyData *pAnalyData,short nNo,short nKStyle)
{
	short   nPrevUpDown,nThisUpDown;
	short   nPrevSize ,nThisSize ;
	float   fPrevOpen ,fThisOpen ;
	float   fPrevClose,fThisClose;
	if(nNo<1)  return 0;
	nPrevUpDown = OddK_UpDown(pAnalyData,nNo-1);
	nThisUpDown = OddK_UpDown(pAnalyData,nNo  );
	nPrevSize   = OddK_Size (pAnalyData,nNo-1);
	nThisSize   = OddK_Size (pAnalyData,nNo  );
	fPrevOpen   = pAnalyData[nNo-1].Open;
	fThisOpen   = pAnalyData[nNo].Open;
	fPrevClose  = pAnalyData[nNo-1].Close;
	fThisClose  = pAnalyData[nNo].Close;
	switch(nKStyle)
	{
	case KLINET_ASCENT:
	//��Ԯ��
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_UP&&FloatComp(fThisOpen,fPrevClose)<0&&
		   FloatComp(fThisOpen,fPrevOpen)>0&&FloatComp(fThisClose,fPrevClose)>0)
			return nKStyle;
		break;
	case KLINET_SLIDE:
	//������
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_DOWN&&FloatComp(fThisOpen,fPrevClose)>0&&
		   FloatComp(fThisOpen,fPrevOpen)<0&&FloatComp(fThisClose,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_JMPUP:
	//������
		if(nPrevUpDown!=KLINEO_DOWN&&nThisUpDown!=KLINEO_DOWN&&
		   FloatComp(fThisOpen,fPrevClose)>0)
			return nKStyle;
		break;
	case KLINET_JMPDOWN:
	//��ˮ��
		if(nPrevUpDown!=KLINEO_UP&&nThisUpDown!=KLINEO_UP&&
		   FloatComp(fThisOpen,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_CRACK:
	//ѹ����
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_DOWN&&
		   FloatComp(fThisOpen,fPrevOpen)>0)
			return nKStyle;
		break;
	case KLINET_FLATTER:
	//�����
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_UP&&
		   nPrevSize==KLINEO_BIG&&nThisUpDown!=KLINEO_SMALL&&
		   FloatComp(fThisClose,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_ABANDON:
	//������
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_UP&&
		   FloatComp(fThisOpen,fPrevClose)>0&&nThisSize==KLINEO_SMALL)
			return nKStyle;
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_DOWN&&
		   FloatComp(fThisOpen,fPrevClose)<0&&nThisSize==KLINEO_SMALL)
			return nKStyle;
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_DOWN&&
		   nPrevSize==KLINEO_BIG&&nThisUpDown==KLINEO_SMALL&&
		   FloatComp(fThisOpen,fPrevClose)>0)
			return nKStyle;
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_UP&&
		   nPrevSize==KLINEO_BIG&&nThisUpDown==KLINEO_SMALL&&
		   FloatComp(fThisOpen,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_COVER:
	//������
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_DOWN&&
		   nPrevSize==KLINEO_BIG&&nThisUpDown!=KLINEO_SMALL&&
		   FloatComp(fThisOpen,fPrevClose)>0&&FloatComp(fThisClose,fPrevOpen)>0&&FloatComp(fThisClose,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_CUTIN:
	//������
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_UP&&
		   nPrevSize==KLINEO_BIG&&nThisUpDown!=KLINEO_SMALL&&
		   FloatComp(fThisClose,fPrevClose)>0&&FloatComp(fThisClose,fPrevOpen)<0&&FloatComp(fThisOpen,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_FAKECOVER:
	//�า����
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_DOWN&&
		   FloatComp(fThisOpen,fPrevClose)>0&&FloatComp(fThisOpen,fPrevOpen)<0)
			return nKStyle;
		break;
	case KLINET_FAKECUTIN:
	//��������
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_UP&&
		   FloatComp(fThisClose,fPrevClose)>0&&FloatComp(fThisClose,fPrevOpen)<0&&FloatComp(fThisOpen,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_COORD:
	//������(*)
		if(isTwinK_Style(pAnalyData,nNo,KLINET_COORDUP  )==KLINET_COORDUP  ) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_COORDDOWN)==KLINET_COORDDOWN) return nKStyle;
		break;
	case KLINET_COORDUP:
	//��������
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_UP     &&
		   fabs(fPrevOpen -fThisOpen )<MIN(fPrevClose-fPrevOpen,fThisClose-fThisOpen)*0.6 &&
		   fabs(fPrevClose-fThisClose)<MIN(fPrevClose-fPrevOpen,fThisClose-fThisOpen)*0.6 &&
		   (fPrevClose-fPrevOpen )/(fThisClose-fThisOpen)<1.3 &&
		   (fPrevClose-fPrevOpen )/(fThisClose-fThisOpen)>0.7 )
				return nKStyle;
		break;
	case KLINET_COORDDOWN:
	//��������
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_DOWN&&
		   fabs(fPrevOpen -fThisOpen )<MIN(fPrevOpen-fPrevClose,fThisOpen-fThisClose)*0.6 &&
		   fabs(fPrevClose-fThisClose)<MIN(fPrevOpen-fPrevClose,fThisOpen-fThisClose)*0.6 &&
		   (fPrevOpen-fPrevClose)/(fThisOpen-fThisClose)<1.3 &&
		   (fPrevOpen-fPrevClose)/(fThisOpen-fThisClose)>0.7 )
			return nKStyle;
		break;
	case KLINET_CONTAIN:
	//������(*)
		if(isTwinK_Style(pAnalyData,nNo,KLINET_CUU)==KLINET_CUU) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_CUD)==KLINET_CUD) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_CUL)==KLINET_CUL) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_CDU)==KLINET_CDU) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_CDD)==KLINET_CDD) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_CDL)==KLINET_CDL) return nKStyle;
		break;
	case KLINET_CUU:
	//������
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)>-COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)< COMPPREC) break;
		if(nThisUpDown!=KLINEO_UP) break;
		if(nPrevUpDown!=KLINEO_UP) break;
		return nKStyle;
	case KLINET_CUD:
	//������
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)>-COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)< COMPPREC) break;
		if(nThisUpDown!=KLINEO_UP  ) break;
		if(nPrevUpDown!=KLINEO_DOWN) break;
		return nKStyle;
	case KLINET_CUL:
	//������
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)>-COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)< COMPPREC) break;
		if(nThisUpDown!=KLINEO_UP   ) break;
		if(nPrevUpDown!=KLINEO_LEVEL) break;
		if(OddK_Shadow(pAnalyData,nNo-1)==KLINEO_NOHEADFOOT)	break;
		if(OddK_Shadow(pAnalyData,nNo-1)==KLINEO_NOHEAD    )	break;
		if(OddK_Shadow(pAnalyData,nNo-1)==KLINEO_NOFOOT    )	break;
		return nKStyle;
	case KLINET_CDU:
	//������
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)>-COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)< COMPPREC) break;
		if(nThisUpDown!=KLINEO_DOWN) break;
		if(nPrevUpDown!=KLINEO_UP  ) break;
		return nKStyle;
	case KLINET_CDD:
	//������
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)>-COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)< COMPPREC) break;
		if(nThisUpDown!=KLINEO_DOWN) break;
		if(nPrevUpDown!=KLINEO_DOWN) break;
		return nKStyle;
	case KLINET_CDL:
	//������
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)>-COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)< COMPPREC) break;
		if(nThisUpDown!=KLINEO_DOWN ) break;
		if(nPrevUpDown!=KLINEO_LEVEL) break;
		if(OddK_Shadow(pAnalyData,nNo-1)==KLINEO_NOHEADFOOT)	break;
		if(OddK_Shadow(pAnalyData,nNo-1)==KLINEO_NOHEAD    )	break;
		if(OddK_Shadow(pAnalyData,nNo-1)==KLINEO_NOFOOT    )	break;
		return nKStyle;
	case KLINET_GRAVID:
	//������(*)
		if(isTwinK_Style(pAnalyData,nNo,KLINET_GUU)==KLINET_GUU) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_GUD)==KLINET_GUD) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_GUL)==KLINET_GUL) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_GDU)==KLINET_GDU) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_GDD)==KLINET_GDD) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_GDL)==KLINET_GDL) return nKStyle;
		break;
	case KLINET_GUU:
	//������
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)< COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)>-COMPPREC) break;
		if(nPrevUpDown!=KLINEO_UP) break;
		if(nThisUpDown!=KLINEO_UP) break;
		return nKStyle;
	case KLINET_GUD:
	//������
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)< COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)>-COMPPREC) break;
		if(nPrevUpDown!=KLINEO_UP  ) break;
		if(nThisUpDown!=KLINEO_DOWN) break;
		return nKStyle;
	case KLINET_GUL:
	//������
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)< COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)>-COMPPREC) break;
		if(nPrevUpDown!=KLINEO_UP   ) break;
		if(nThisUpDown!=KLINEO_LEVEL) break;
		if(OddK_Shadow(pAnalyData,nNo)==KLINEO_NOHEADFOOT)	break;
		if(OddK_Shadow(pAnalyData,nNo)==KLINEO_NOHEAD    )	break;
		if(OddK_Shadow(pAnalyData,nNo)==KLINEO_NOFOOT    )	break;
		return nKStyle;
	case KLINET_GDU:
	//������
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)< COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)>-COMPPREC) break;
		if(nPrevUpDown!=KLINEO_DOWN) break;
		if(nThisUpDown!=KLINEO_UP  ) break;
		return nKStyle;
	case KLINET_GDD:
	//������
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)< COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)>-COMPPREC) break;
		if(nPrevUpDown!=KLINEO_DOWN) break;
		if(nThisUpDown!=KLINEO_DOWN) break;
		return nKStyle;
	case KLINET_GDL:
	//������
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)< COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)>-COMPPREC) break;
		if(nPrevUpDown!=KLINEO_DOWN ) break;
		if(nThisUpDown!=KLINEO_LEVEL) break;
		if(OddK_Shadow(pAnalyData,nNo)==KLINEO_NOHEADFOOT)  break;
		if(OddK_Shadow(pAnalyData,nNo)==KLINEO_NOHEAD    )	break;
		if(OddK_Shadow(pAnalyData,nNo)==KLINEO_NOFOOT    )	break;
		return nKStyle;
	case KLINET_DRAGUP:
	//������
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_UP&&FloatComp(fThisClose,fPrevOpen)>0&&
		   FloatComp(fThisOpen,fPrevClose)>0&&FloatComp(fThisOpen,fPrevOpen)<0)
			return nKStyle;
		break;
	case KLINET_DRAGDOWN:
	//������
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_DOWN&&
		   FloatComp(fThisOpen,fPrevOpen)>0&&FloatComp(fThisOpen,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_SURRENDER:
	//��˳��
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_UP&&
		   FloatComp(fThisOpen,fPrevOpen)==0)
			return nKStyle;
		break;
	case KLINET_REBELL:
	//������
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_DOWN&&
		   FloatComp(fThisOpen,fPrevOpen)==0)
			return nKStyle;
		break;
	case KLINET_FEIGN:
	//αK��(*)
		if(isTwinK_Style(pAnalyData,nNo,KLINET_FEIGNUP  )==KLINET_FEIGNUP  ) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_FEIGNDOWN)==KLINET_FEIGNDOWN) return nKStyle;
		break;
	case KLINET_FEIGNUP:
	//α����
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_UP&&
		   FloatComp(fThisClose,fPrevClose)<0&&FloatComp(fThisOpen,fPrevOpen)<0)
			return nKStyle;
		break;
	case KLINET_FEIGNDOWN:
	//α����
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_DOWN&&
		   FloatComp(fThisClose,fPrevClose)>0&&FloatComp(fThisOpen,fPrevOpen)>0)
			return nKStyle;
		break;
	case KLINET_NIP:
	//�м���(*)
		if(isTwinK_Style(pAnalyData,nNo,KLINET_NDU)==KLINET_NDU) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_NUD)==KLINET_NUD) return nKStyle;
		break;
	case KLINET_NDU:
	//������
		if(nNo<2) return 0;
		if(nPrevUpDown!=KLINEO_UP  ) return 0;
		if(nThisUpDown!=KLINEO_DOWN) return 0;
		if(OddK_UpDown(pAnalyData,nNo-2)!=KLINEO_DOWN) 	 return 0;
		if(OddK_Size  (pAnalyData,nNo-2)==KLINEO_SMALL)  return 0;
		if(nPrevSize==KLINEO_SMALL)  					 return 0;
		if(nThisSize==KLINEO_SMALL)  					 return 0;
		if(pAnalyData[nNo-2].Close>fPrevClose) return 0;
		if(pAnalyData[nNo-2].Close>fPrevOpen)  return nKStyle;
		break;
	case KLINET_NUD:
	//������
		if(nNo<2) return 0;
		if(nPrevUpDown!=KLINEO_DOWN) return 0;
		if(nThisUpDown!=KLINEO_UP  ) return 0;
		if(OddK_UpDown(pAnalyData,nNo-2)!=KLINEO_UP) 	 return 0;
		if(OddK_Size  (pAnalyData,nNo-2)==KLINEO_SMALL)  return 0;
		if(nPrevSize==KLINEO_SMALL)  					 return 0;
		if(nThisSize==KLINEO_SMALL)  					 return 0;
		if(pAnalyData[nNo-2].Close>fPrevOpen) return 0;
		if(pAnalyData[nNo-2].Close>fPrevClose)  return nKStyle;
		break;
	}
	return 0;
}

//�����漰������ѡ��
short CZnxg::GetOtherStyleGP(short Type,short *lpnGPIndex)
{
	int nCount = 0;
	for(int i = 0;i < m_nABGNum;i++)
	{
		if(Is_X_Type(Type, m_pStkAttrib[i]) == 1)
			lpnGPIndex[nCount++] = m_pABGXh[i];
	}
	return nCount;
}

short CZnxg::Is_X_Type(short Type, long Stk_Attrib)
{
	if (Stk_Attrib == -1)	return 0;
	long Att = Stk_Attrib;
	CLE_OTHERXG_RES	p_TJ;
	memcpy(&p_TJ,&Att,sizeof(long));
	switch (Type)
	{
	case JBMXG_1001://  С�̴��¹�
		if (p_TJ.P_flag == 2 &&
			p_TJ.Cxg_flag == 1)
			return 1;
		break;
	case JBMXG_1002://  ���̴��¹�
		if (p_TJ.P_flag == 1 &&
			p_TJ.Cxg_flag == 1)
			return 1;
		break;
	case JBMXG_1003://  ���̴��¹�
		if (p_TJ.P_flag == 0 &&
			p_TJ.Cxg_flag == 1)
			return 1;
		break;
	case JBMXG_1004://	С�̼��Ź�
		if( p_TJ.P_flag == 2 &&
			p_TJ.J_flag == 0)
			return 1;
		break;
	case JBMXG_1005://	С�̼�ƽ��
		if( p_TJ.P_flag == 2 &&
			p_TJ.J_flag == 1)
			return 1;
		break;
	case JBMXG_1006://	С�̼����
		if( p_TJ.P_flag == 2 &&
			p_TJ.J_flag == 2)
			return 1;
		break;
	case JBMXG_1007://	���̼��Ź�
		if( p_TJ.P_flag == 1 &&
			p_TJ.J_flag == 0)
			return 1;
		break;
	case JBMXG_1008://	���̼�ƽ��
		if( p_TJ.P_flag == 1 &&
			p_TJ.J_flag == 1)
			return 1;
		break;
	case JBMXG_1009://	���̼����
		if( p_TJ.P_flag == 1 &&
			p_TJ.J_flag == 2)
			return 1;
		break;
	case JBMXG_1010://	���̼��Ź�
		if( p_TJ.P_flag == 0 &&
			p_TJ.J_flag == 0)
			return 1;
		break;
	case JBMXG_1011://	���̼�ƽ��
		if( p_TJ.P_flag == 0 &&
			p_TJ.J_flag == 1)
			return 1;
		break;
	case JBMXG_1012://	���̼����
		if( p_TJ.P_flag == 0 &&
			p_TJ.J_flag == 2)
			return 1;
		break;
	case JBMXG_1013://   �����
		if ( p_TJ.J_flag == 3 )
			return 1;
		break;
	case JBMXG_1014://   �߹ɱ�����������	���ʲ��߲��ҹ������
		if (p_TJ.Jzc_GD_flag == 1 &&
			p_TJ.Gjj_G_flag  == 1)
			return 1;
		break;
	case JBMXG_1015://   �߸�����������	���ʲ��߲��ҹ�����߲���С��
		if (p_TJ.Jzc_GD_flag == 1 &&
			p_TJ.Gjj_G_flag  == 1 &&
			p_TJ.P_flag == 2)
			return 1;
		break;
	case JBMXG_1016://   �߷����������
		if (p_TJ.Gclr_G_flag == 1)
			return 1;
		break;

	case JSMXG_1001://   BIAS����ָʾ��־
		if (p_TJ.BIAS_Buy_flag == 1)
			return 1;
		break;
	case JSMXG_1002://   SAR����ָʾ
		if (p_TJ.SAR_Buy_flag == 1)
			return 1;
		break;
	case JSMXG_1003://   KDJ����ָʾ
		if (p_TJ.KDJ_Buy_flag == 1)
			return 1;
		break;
	case JSMXG_1004://   RSI����ָʾ
		if (p_TJ.RSI_Buy_flag == 1)
			return 1;
		break;
	case JSMXG_1005://   MACD����ָʾ
		if (p_TJ.MACD_Buy_flag == 1)
			return 1;
		break;
	case JSMXG_1006://   MTM����ָʾ
		if (p_TJ.MTM_Buy_flag == 1)
			return 1;
		break;
	}
	return 0;
}

//�����Ʊ������������
long CZnxg::GetGP_Tj_Attrib(short Index,struct AnalyData  * pTJ_AnalyData,short ItNum)
{
	short Data_Num;
	long Res = 0;
	CLE_OTHERXG_RES * p_Res = (CLE_OTHERXG_RES *)(&Res);
	MemStkInfo * p_MemStkInfo = (*g_pStockIO)[Index];
	Data_Num = ItNum;
	TJ_AnalyData = pTJ_AnalyData;
///////////////////////////////////////////////////////////
	if (Get_SS_Day(Index) <= g_pXGSign.Cxg_Day_L)
		p_Res -> Cxg_flag = 1;
	else p_Res -> Cxg_flag = 0;
///////////////////////////////////////////////////////////
	if (p_MemStkInfo -> ActiveCapital > g_pXGSign.Dz_sign)
		p_Res -> P_flag = DPG;
	else if (p_MemStkInfo -> ActiveCapital <= g_pXGSign.Xz_sign)
		p_Res -> P_flag = XPG;
	else 
		p_Res -> P_flag = ZPG;
///////////////////////////////////////////////////////////
	if (p_MemStkInfo -> J_zgb < 1)
		p_Res -> J_flag = JPG;
	else
	{
		if ((p_MemStkInfo->J_jly*12/p_MemStkInfo->J_HalfYearFlag)/(p_MemStkInfo -> J_zgb) >= g_pXGSign.Yp_sign)
			p_Res -> J_flag = JYG;
		else if ((p_MemStkInfo->J_jly*12/p_MemStkInfo->J_HalfYearFlag)/(p_MemStkInfo -> J_zgb) >= g_pXGSign.Pc_sign)
			p_Res -> J_flag = JPG;
		else if ((p_MemStkInfo->J_jly*12/p_MemStkInfo->J_HalfYearFlag)/(p_MemStkInfo -> J_zgb) >= g_pXGSign.Ks_sign)
			p_Res -> J_flag = JCG;
		else 
			p_Res -> J_flag = KCG;
	}
///////////////////////////////////////////////////////////
	if (p_MemStkInfo -> J_zgb < 1)		
		p_Res -> Jzc_GD_flag = 0;
	else
	{
		if (p_MemStkInfo -> J_jzc/10/p_MemStkInfo -> J_zgb >= g_pXGSign.Jzc_G_sign)
			p_Res -> Jzc_GD_flag = 1;
		else 	
			p_Res -> Jzc_GD_flag = 0;
	}
///////////////////////////////////////////////////////////
	if (p_MemStkInfo -> J_zgb < 1)	
		p_Res -> Gjj_G_flag = 0;
	else
	{
		if (p_MemStkInfo -> J_zbgjj/10/p_MemStkInfo -> J_zgb >= g_pXGSign.Gjx_G_sign)
			p_Res -> Gjj_G_flag = 1;
		else 
			p_Res -> Gjj_G_flag = 0;
	}
//////////////////////////////////////////////////////////
	if (p_MemStkInfo -> J_zgb < 1) 	
		p_Res -> Gclr_G_flag = 0;
	else
	{
		if(p_MemStkInfo -> J_wfply/10/p_MemStkInfo -> J_zgb >= g_pXGSign.Gclr_G_sign)
			p_Res -> Gclr_G_flag = 1;
		else 
			p_Res -> Gclr_G_flag = 0;
	}
	if (Data_Num <= 3)	return Res;
//////////////////////////////////////////////////////////
	if (Is_BIAS_Buy(Index,pTJ_AnalyData,Data_Num) == TRUE)	   p_Res -> BIAS_Buy_flag = TRUE;
	if (Is_SAR_Buy(Index,pTJ_AnalyData,Data_Num)  == TRUE)     p_Res -> SAR_Buy_flag  = TRUE;
	if (Is_KDJ_Buy(Index,pTJ_AnalyData,Data_Num)  == TRUE)     p_Res -> KDJ_Buy_flag  = TRUE;
	if (Is_RSI_Buy(Index,pTJ_AnalyData,Data_Num)  == TRUE)     p_Res -> RSI_Buy_flag  = TRUE;
	if (Is_MACD_Buy(Index,pTJ_AnalyData,Data_Num) == TRUE)     p_Res -> MACD_Buy_flag = TRUE;
	if (Is_MTM_Buy(Index,pTJ_AnalyData,Data_Num) == TRUE)      p_Res -> MTM_Buy_flag = TRUE;
	return Res;
}

BOOL CZnxg::Is_BIAS_Buy(short Index,struct AnalyData  * pTJ_AnalyData,short ItNum)
{
	MemStkInfo * p_MemStkInfo;
	p_MemStkInfo = (*g_pStockIO)[Index];
	HISDAT *tmpHisData = new HISDAT[ItNum];
	RawData2CalcData(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,pTJ_AnalyData,tmpHisData,ItNum,g_NoTime); //ת��TCALC����ʶ������
	int nRealNum = g_pCalc->_CallDTopViewCalc_3(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,tmpHisData,ItNum);
	delete tmpHisData;
	if(nRealNum < 1) return FALSE;
	int IndexNo = g_pCalc->_CallDTopViewCalc_9(TJ_TYPE,"BIAS����");
	if(IndexNo < 0) return FALSE;
	float *pOutData[MAX_LINEOUT];
	for(int j=0; j<MAX_LINEOUT;j++) 
		pOutData[j] = g_pCalc->_CallDTopViewCalc_13(nRealNum);
	g_pCalc->_CallDTopViewCalc_5(TJ_TYPE,IndexNo, pOutData);
	float fRet = pOutData[0][nRealNum-1];
	for (j=0; j<MAX_LINEOUT; j++) g_pCalc->_CallDTopViewCalc_14(pOutData[j]);
	return ((TBE_MEANLESS(fRet) != 0) && fabs(fRet) > 0.1 );
}

BOOL CZnxg::Is_SAR_Buy(short  Index,struct AnalyData  * pTJ_AnalyData,short ItNum)
{
	MemStkInfo * p_MemStkInfo;
	p_MemStkInfo = (*g_pStockIO)[Index];
	HISDAT *tmpHisData = new HISDAT[ItNum];
	RawData2CalcData(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,pTJ_AnalyData,tmpHisData,ItNum,g_NoTime); //ת��TCALC����ʶ������
	int nRealNum = g_pCalc->_CallDTopViewCalc_3(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,tmpHisData,ItNum);
	delete tmpHisData;
	if(nRealNum < 1) return FALSE;
	int IndexNo = g_pCalc->_CallDTopViewCalc_9(TJ_TYPE,"SAR����");
	if(IndexNo < 0) return FALSE;
	float *pOutData[MAX_LINEOUT];
	for(int j=0; j<MAX_LINEOUT;j++) 
		pOutData[j] = g_pCalc->_CallDTopViewCalc_13(nRealNum);
	g_pCalc->_CallDTopViewCalc_5(TJ_TYPE,IndexNo, pOutData);
	float fRet = pOutData[0][nRealNum-1];
	for (j=0; j<MAX_LINEOUT; j++) g_pCalc->_CallDTopViewCalc_14(pOutData[j]);
	return ((TBE_MEANLESS(fRet) != 0) && fabs(fRet) > 0.1 );
}

BOOL CZnxg::Is_KDJ_Buy(short Index,struct AnalyData  * pTJ_AnalyData,short ItNum)
{
	MemStkInfo * p_MemStkInfo;
	p_MemStkInfo = (*g_pStockIO)[Index];
	HISDAT *tmpHisData = new HISDAT[ItNum];
	RawData2CalcData(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,pTJ_AnalyData,tmpHisData,ItNum,g_NoTime); //ת��TCALC����ʶ������
	int nRealNum = g_pCalc->_CallDTopViewCalc_3(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,tmpHisData,ItNum);
	delete tmpHisData;
	if(nRealNum < 1) return FALSE;
	int IndexNo = g_pCalc->_CallDTopViewCalc_9(TJ_TYPE,"KDJ����");
	if(IndexNo < 0) return FALSE;
	float *pOutData[MAX_LINEOUT];
	for(int j=0; j<MAX_LINEOUT;j++) 
		pOutData[j] = g_pCalc->_CallDTopViewCalc_13(nRealNum);
	g_pCalc->_CallDTopViewCalc_5(TJ_TYPE,IndexNo, pOutData);
	float fRet = pOutData[0][nRealNum-1];
	for (j=0; j<MAX_LINEOUT; j++) g_pCalc->_CallDTopViewCalc_14(pOutData[j]);
	return ((TBE_MEANLESS(fRet) != 0) && fabs(fRet) > 0.1 );
}

BOOL CZnxg::Is_RSI_Buy(short  Index,struct AnalyData  * pTJ_AnalyData,short ItNum)
{
	MemStkInfo * p_MemStkInfo;
	p_MemStkInfo = (*g_pStockIO)[Index];
	HISDAT *tmpHisData = new HISDAT[ItNum];
	RawData2CalcData(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,pTJ_AnalyData,tmpHisData,ItNum,g_NoTime); //ת��TCALC����ʶ������
	int nRealNum = g_pCalc->_CallDTopViewCalc_3(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,tmpHisData,ItNum);
	delete tmpHisData;
	if(nRealNum < 1) return FALSE;
	int IndexNo = g_pCalc->_CallDTopViewCalc_9(TJ_TYPE,"RSI����");
	if(IndexNo < 0) return FALSE;
	float *pOutData[MAX_LINEOUT];
	for(int j=0; j<MAX_LINEOUT;j++) 
		pOutData[j] = g_pCalc->_CallDTopViewCalc_13(nRealNum);
	g_pCalc->_CallDTopViewCalc_5(TJ_TYPE,IndexNo, pOutData);
	float fRet = pOutData[0][nRealNum-1];
	for (j=0; j<MAX_LINEOUT; j++) g_pCalc->_CallDTopViewCalc_14(pOutData[j]);
	return ((TBE_MEANLESS(fRet) != 0) && fabs(fRet) > 0.1 );
}

BOOL CZnxg::Is_MACD_Buy(short Index,struct AnalyData  * pTJ_AnalyData,short ItNum)
{
	MemStkInfo * p_MemStkInfo;
	p_MemStkInfo = (*g_pStockIO)[Index];
	HISDAT *tmpHisData = new HISDAT[ItNum];
	RawData2CalcData(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,pTJ_AnalyData,tmpHisData,ItNum,g_NoTime); //ת��TCALC����ʶ������
	int nRealNum = g_pCalc->_CallDTopViewCalc_3(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,tmpHisData,ItNum);
	delete tmpHisData;
	if(nRealNum < 1) return FALSE;
	int IndexNo = g_pCalc->_CallDTopViewCalc_9(TJ_TYPE,"MACD����");
	if(IndexNo < 0) return FALSE;
	float *pOutData[MAX_LINEOUT];
	for(int j=0; j<MAX_LINEOUT;j++) 
		pOutData[j] = g_pCalc->_CallDTopViewCalc_13(nRealNum);
	g_pCalc->_CallDTopViewCalc_5(TJ_TYPE,IndexNo, pOutData);
	float fRet = pOutData[0][nRealNum-1];
	for (j=0; j<MAX_LINEOUT; j++) g_pCalc->_CallDTopViewCalc_14(pOutData[j]);
	return ((TBE_MEANLESS(fRet) != 0) && fabs(fRet) > 0.1 );
}

BOOL CZnxg::Is_MTM_Buy(short Index,struct AnalyData  * pTJ_AnalyData,short ItNum)
{
	MemStkInfo * p_MemStkInfo;
	p_MemStkInfo = (*g_pStockIO)[Index];
	HISDAT *tmpHisData = new HISDAT[ItNum];
	RawData2CalcData(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,pTJ_AnalyData,tmpHisData,ItNum,g_NoTime); //ת��TCALC����ʶ������
	int nRealNum = g_pCalc->_CallDTopViewCalc_3(p_MemStkInfo->Code,p_MemStkInfo->setcode,PER_DAY,tmpHisData,ItNum);
	delete tmpHisData;
	if(nRealNum < 1) return FALSE;
	int IndexNo = g_pCalc->_CallDTopViewCalc_9(TJ_TYPE,"MTM����");
	if(IndexNo < 0) return FALSE;
	float *pOutData[MAX_LINEOUT];
	for(int j=0; j<MAX_LINEOUT;j++) 
		pOutData[j] = g_pCalc->_CallDTopViewCalc_13(nRealNum);
	g_pCalc->_CallDTopViewCalc_5(TJ_TYPE,IndexNo, pOutData);
	float fRet = pOutData[0][nRealNum-1];
	for (j=0; j<MAX_LINEOUT; j++) g_pCalc->_CallDTopViewCalc_14(pOutData[j]);
	return ((TBE_MEANLESS(fRet) != 0) && fabs(fRet) > 0.1 );
}
