// ZstTickDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "UZst.h"
#include "ZstTickDlg.h"
#include "MyInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT ctlNormal[] = {IDC_STATIC1, IDC_STATIC2, IDC_STATIC3, IDC_STATIC4, IDC_STATIC5, IDC_STATIC6, IDC_STATIC7, 
IDC_STATIC8, IDC_STATIC9, IDC_STATIC10, IDC_STATIC11, IDC_STATIC12, IDC_STATIC13, IDC_STATIC14, IDC_STATIC15, IDC_STATIC16, IDC_STATIC17, 
IDC_PIC, IDC_MINNUM, IDC_OPEN, IDC_MAX, IDC_MIN, IDC_CLOSE, IDC_AVERPRICE, IDC_VOLUME, IDC_TICKVOL, IDC_ZANGFU, IDC_ZENFU, 
IDC_INVOL, IDC_OUTVOL, IDC_DATANUM, IDC_BIGTICKNUM, IDC_OUTBIGTICK, IDC_INBIGTICK};

extern long GetManyDayBackDate(struct AnalyData * AnalyDatap,short nCacheNum,long date);

int g_LastSelTab=0;

/////////////////////////////////////////////////////////////////////////////
// ZstTickDlg dialog
ZstTickDlg::ZstTickDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ZstTickDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ZstTickDlg)
	m_strClose = _T("");
	m_strTickNum = _T("");
	m_strInVol = _T("");
	m_strHigh = _T("");
	m_strLow = _T("");
	m_strOpen = _T("");
	m_strOutVol = _T("");
	m_strTickVol = _T("");
	m_strVol = _T("");
	m_strSartTime = _T("");
	m_strEndTime = _T("");
	m_strZF = _T("");
	m_strMinNum = _T("");
	m_strAver = _T("");
	m_strBigTickNum = _T("");
	m_strInBigTick = _T("");
	m_strOutBigTick = _T("");
	m_strZenFu = _T("");
	//}}AFX_DATA_INIT
	m_nMainID = 0;
	memset(m_pTick, 0, (MAX_ZST_MANYDAY+1)*sizeof(TICKDATA*));
	memset(m_nTickNum, 0, (MAX_ZST_MANYDAY+1)*sizeof(long));
	memset(m_bHeadTag, 0, (MAX_ZST_MANYDAY+1)*sizeof(BOOL));
	memset(m_HeadTagDate, 0, (MAX_ZST_MANYDAY+1)*sizeof(long));
	memset(&m_CalcData, 0, sizeof(ZsTickCalcData));

	m_mapPriceVol.clear();
	m_BigTick.clear();
	m_BigTickDate.clear();

	m_imagelist.Create(16,16,ILC_COLOR24|ILC_MASK,3,1);
	CBitmap tmpbm;
	tmpbm.LoadBitmap(IDB_IMAGELIST_QJTJ);
	m_imagelist.Add(&tmpbm,RGB(0,0,255));
}


void ZstTickDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ZstTickDlg)
	DDX_Control(pDX, IDC_EDIT_LIMIT, m_bigVol);
	DDX_Control(pDX, IDC_ZSTTICKTAB, m_Tab);
	DDX_Control(pDX, IDC_ZSTTICKLIST, m_List);
	DDX_Text(pDX, IDC_CLOSE, m_strClose);
	DDX_Text(pDX, IDC_DATANUM, m_strTickNum);
	DDX_Text(pDX, IDC_INVOL, m_strInVol);
	DDX_Text(pDX, IDC_MAX, m_strHigh);
	DDX_Text(pDX, IDC_MIN, m_strLow);
	DDX_Text(pDX, IDC_OPEN, m_strOpen);
	DDX_Text(pDX, IDC_OUTVOL, m_strOutVol);
	DDX_Text(pDX, IDC_TICKVOL, m_strTickVol);
	DDX_Text(pDX, IDC_VOLUME, m_strVol);
	DDX_Text(pDX, IDC_TIME1, m_strSartTime);
	DDX_Text(pDX, IDC_TIME2, m_strEndTime);
	DDX_Text(pDX, IDC_ZANGFU, m_strZF);
	DDX_Text(pDX, IDC_MINNUM, m_strMinNum);
	DDX_Text(pDX, IDC_AVERPRICE, m_strAver);
	DDX_Text(pDX, IDC_BIGTICKNUM, m_strBigTickNum);
	DDX_Text(pDX, IDC_INBIGTICK, m_strInBigTick);
	DDX_Text(pDX, IDC_OUTBIGTICK, m_strOutBigTick);
	DDX_Text(pDX, IDC_ZENFU, m_strZenFu);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ZstTickDlg, CDialog)
	//{{AFX_MSG_MAP(ZstTickDlg)
	ON_MESSAGE(UM_GETDATA_ACK,OnGetDataAck)
	ON_NOTIFY(TCN_SELCHANGE, IDC_ZSTTICKTAB, OnSelchangeZstticktab)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_ZSTTICKLIST, OnColumnclickZstticklist)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, OnButtonUpdate)
	ON_NOTIFY(NM_RCLICK, IDC_ZSTTICKLIST, OnRclickZstticklist)
	ON_COMMAND(ID_SAVEBIGVOL,OnSaveBigVol)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ZstTickDlg message handlers
BOOL ZstTickDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
		
	if(!m_pUZst) return FALSE;
	
	CString	tempstr;
	tempstr.Format("%d",g_GH.StatBigVol);
	m_bigVol.SetWindowText(tempstr);

	m_Pie.SubclassDlgItem(IDC_PIC, this); 
	m_Pie.SetTitle(_F("龙虎统计:"));
	m_Pie.Reset();

	m_Pie.ShowWindow(SW_HIDE);

	m_Tab.InsertItem(0, _F("统计信息"));
	m_Tab.InsertItem(1, _F("大单成交"));
	m_Tab.InsertItem(2, _F("价量分布"));
	m_Tab.SetCurSel(g_LastSelTab);
	m_List.ShowWindow(SW_HIDE);
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_List.SetImageList(&m_imagelist,LVSIL_SMALL);
	m_List.m_nSortedCol=4;

	WantDataMayContinue(m_nStartDate,TRUE);		//第一次要数据
	
	m_List.GetWindowRect(m_yucitu_rect);
	ScreenToClient(&m_yucitu_rect);

	ShowIt();
	Init();
	return TRUE; 
}

//////////////////////////////////////////////////////////////////////////
void ZstTickDlg::CalcOneData(BYTE nCurReqDate)
{
	if(m_pTick[nCurReqDate]&&m_nTickNum[nCurReqDate]>0)		//先统计已有的数据
	{
		int nStartPos=0,nEndPos=0;
		int startMinute=m_nEndMinuteXh;
		int endMinute=m_nStartMinuteXh;
		for(int i=0;i<m_nTickNum[m_nStartDate] && m_pTick[m_nStartDate][i].Minute<=startMinute;i++);
		if(i>0) nStartPos=i-1;
		for(i=0;i<m_nTickNum[m_nEndDate] && m_pTick[m_nEndDate][i].Minute<endMinute;i++);
		if(i>0) nEndPos=i;
		for(i=0;i<m_nTickNum[nCurReqDate] && m_pTick[nCurReqDate][i].Minute<=startMinute;i++);
		if(i>0) nStartPos=i-1;
		for(i=0;i<m_nTickNum[nCurReqDate] && m_pTick[nCurReqDate][i].Minute<endMinute;i++);
		if(i>0) nEndPos=i-1;
		
		if(nStartPos>=0&&nEndPos>=0&&nEndPos>=nStartPos)
		{
			MemStkInfo *pStkInfo = (*g_pStockIO)[m_nCurGPNo];
			TickData *tmpTickBuf = m_pTick[nCurReqDate];
			for(int i=nStartPos;i<=nEndPos;i++)
			{
				m_CalcData.nItemNum++;
				if(tmpTickBuf[i].Now>0)
				{
					m_CalcData.High = max(m_CalcData.High, tmpTickBuf[i].Now);
					if(m_CalcData.Low<1) m_CalcData.Low = tmpTickBuf[i].Now;
					m_CalcData.Low = min(m_CalcData.Low, tmpTickBuf[i].Now);
					m_CalcData.fAmount += tmpTickBuf[i].NowVol*tmpTickBuf[i].Now;
				}
				m_CalcData.dwVolume += tmpTickBuf[i].NowVol;
				if(!testzs(pStkInfo))
				{
					m_CalcData.dwInFlagVol += (tmpTickBuf[i].InOutFlag==1?tmpTickBuf[i].NowVol:0);
					m_CalcData.dwOutFalgVol += (tmpTickBuf[i].InOutFlag==0?tmpTickBuf[i].NowVol:0);
				}
				if(tmpTickBuf[i].NowVol>=g_GH.StatBigVol && !testzs(pStkInfo))
				{
					m_CalcData.nBigItemNum++;
					m_CalcData.dwBigTickVol += tmpTickBuf[i].NowVol;
					if(tmpTickBuf[i].InOutFlag==0) 
					{
						m_CalcData.dwOutBigTickVol += tmpTickBuf[i].NowVol;
						m_CalcData.nOutBigItem++;
					}
					if(tmpTickBuf[i].InOutFlag==1) 
					{
						m_CalcData.dwInBigTickVol += tmpTickBuf[i].NowVol;
						m_CalcData.nInBigItem++;
					}
					
					int nSize=m_BigTickDate.size();
					vector<TickData>::iterator iter1=m_BigTick.begin();
					vector<long>::iterator iter2=m_BigTickDate.begin();
					long date=GetDate(nCurReqDate);
					for(int j=0;j<nSize;j++)
					{
						if(date>m_BigTickDate[j])
							break;
						if(date==m_BigTickDate[j] && tmpTickBuf[i].Minute>=m_BigTick[j].Minute)
							break;									
					}
					m_BigTick.insert(iter1+j,tmpTickBuf[i]);
					m_BigTickDate.insert(iter2+j,date);
				}
			}
			if(m_CalcData.Close<1)						//无缓存可以这么处理
				m_CalcData.Close = tmpTickBuf[nEndPos].Now;
			m_CalcData.Open = tmpTickBuf[nStartPos].Now;
			
			ReadPriceVolTable(tmpTickBuf, nStartPos, nEndPos);
		}
	}
}

void ZstTickDlg::WantDataMayContinue(BYTE nWantDate, BOOL bFirstWant)
{
	m_nCurReqDate = nWantDate;
	MemStkInfo *pStkInfo = (*g_pStockIO)[m_nCurGPNo];
	if(!pStkInfo) return;
	
	if(m_bHeadTag[m_nCurReqDate]==TRUE) return;
	//得到日期
	struct AnalyData * AnalyDatap = new struct AnalyData[RECENTNUM+50];
	short nCacheNum = ReadCacheData(pStkInfo,pStkInfo->setcode,AnalyDatap,RECENTNUM,PER_DAY,1);
	if(nCacheNum<1)
		nCacheNum = g_pStockIO->ReadVipDocData(pStkInfo,AnalyDatap,RECENTNUM,PER_DAY,1,0);
	long nDate = m_nCurDate; short nBackDate = m_nCurReqDate;
	while(nBackDate-->0) nDate = GetManyDayBackDate(AnalyDatap,nCacheNum,nDate);
	delete AnalyDatap;
	//
	g_nMainID = MainIDPlus(m_nMainID,ZSTICKDATA_WIN);

	if(m_nCurReqDate>0||nDate!=g_lOpenRq) 
	{
		long nCacheNum = GetHisTickCache(pStkInfo, nDate);
		if(nCacheNum>0)
		{
			TDELA(m_pTick[m_nCurReqDate]);
			m_pTick[m_nCurReqDate] = new TICKDATA[nCacheNum];
			m_nTickNum[m_nCurReqDate] = GetHisTickCache(pStkInfo, nDate, m_pTick[m_nCurReqDate], nCacheNum);
			m_HeadTagDate[m_nCurReqDate] = nDate;
			CalcOneData(m_nCurReqDate);

			m_bHeadTag[m_nCurReqDate] = TRUE;
			if(m_nEndDate>m_nCurReqDate)	//准备第一次要下一天的
				WantDataMayContinue(m_nCurReqDate+1,TRUE);
			else 
				ShowIt();
		}
		else
		{
			g_nAssisID = TICKRANGEHIS_ASSIST;
			g_pGetData->GetHisTickData(pStkInfo, nDate, m_nTickNum[m_nCurReqDate], WANT_ZSTTICKS);
			m_HeadTagDate[m_nCurReqDate] = nDate;
		}		
	}
	else if(m_nCurReqDate==0)
	{
		g_nAssisID = TICKRANGECUR_ASSIST;
		if(g_pStockIO->BeDSStock(pStkInfo))
			g_pGetData->DSGetTick(pStkInfo, m_nTickNum[m_nCurReqDate], WANT_ZSTTICKS);
		else 
			g_pGetData->GetTickData(pStkInfo, m_nTickNum[m_nCurReqDate], WANT_ZSTTICKS, TRUE);
	}
}

//////////////////////////////////////////////////////////////////////////
BOOL ZstTickDlg::SaveHisTickCache(BYTE nCurReqDate)
{
	return TRUE;
	if(m_nTickNum[nCurReqDate]<1||m_pTick[nCurReqDate]==NULL) return FALSE;
	MemStkInfo *pStkInfo = (*g_pStockIO)[m_nCurGPNo];
	if(!pStkInfo) return FALSE;
	long nGPIndex = g_pStockIO->GetIndex(pStkInfo->Code, pStkInfo->setcode);
	if(nGPIndex<0) return FALSE;
	if(m_HeadTagDate[nCurReqDate]<0) return FALSE;

	char  filename[MAX_PATH]={0};
	sprintf(filename,"%szst_cache\\histicache.htc",g_WSXHStr);
	FILE *fpt = _fsopen(filename,"a+b",SH_DENYNO);
	if(fpt)
	{
		fseek(fpt, 0, SEEK_END);
		long filelen = ftell(fpt);
		fwrite(m_pTick[nCurReqDate],sizeof(TICKDATA),m_nTickNum[nCurReqDate],fpt);
		g_HisTickCacheHdr[g_nNextHTCHPos].nGPIndex = nGPIndex;
		g_HisTickCacheHdr[g_nNextHTCHPos].nDate = m_HeadTagDate[nCurReqDate];
		g_HisTickCacheHdr[g_nNextHTCHPos].nStartPos = filelen;
		g_HisTickCacheHdr[g_nNextHTCHPos].nTickNum = m_nTickNum[nCurReqDate];
		fclose(fpt);
		if(++g_nNextHTCHPos>=MAX_HISCACHE) g_nNextHTCHPos=0;
	}
	return FALSE;
}

void ZstTickDlg::OnGetDataAck(WPARAM wParam,LPARAM lParam)
{
	MemStkInfo *pStkInfo = (*g_pStockIO)[m_nCurGPNo];
	short AssistID;
	if(g_pStockIO->BeDSStock(pStkInfo))
	{
		AssistID = g_AnsHeader3.AssisID;
		if(m_nMainID!=g_AnsHeader3.MainID) 
			return;
	}
	else 
	{
		AssistID = g_AnsHeader.AssisID;
		if(m_nMainID!=g_AnsHeader.MainID) 
			return;
	}

	TickData *tmpTickBuf = new TickData[WANT_ZSTTICKS+m_nTickNum[m_nCurReqDate]+10];
	float tmpClose;
	short tmpTickNum;
	if(AssistID==TICKRANGEHIS_ASSIST)
		tmpTickNum = g_pGetData->GetHisTickDataACK(pStkInfo,tmpTickBuf,WANT_ZSTTICKS,&tmpClose);
	else if(AssistID==TICKRANGECUR_ASSIST)
	{
		if(g_pStockIO->BeDSStock(pStkInfo))
			tmpTickNum = g_pGetData->DSGetTickACK(tmpTickBuf);
		else 
			tmpTickNum = g_pGetData->GetTickDataACK(pStkInfo, tmpTickBuf, WANT_ZSTTICKS);
	}

	if(tmpTickNum == 0)			//要完了一天的
	{
		m_bHeadTag[m_nCurReqDate] = TRUE;
		TDEL(tmpTickBuf);
		SaveHisTickCache(m_nCurReqDate);
		if(m_nEndDate>m_nCurReqDate)	//准备第一次要下一天的
			WantDataMayContinue(m_nCurReqDate+1,TRUE);
	}
	else
	{
		BOOL bStartNow = FALSE;
		int nStartPos1=-1, nEndPos1=-1;
		//继续统计数据
		if(m_nStartDate==m_nCurReqDate)					//起始日
		{
			for(int i=0;i<tmpTickNum&&tmpTickBuf[i].Minute<=m_nStartMinuteXh;i++);
			if(i>0) nEndPos1 = i-1;
			if(tmpTickBuf[0].Minute<=m_nStartMinuteXh) nStartPos1 = 0;
			if(nStartPos1>=0&&nEndPos1>=0) bStartNow = TRUE;
		}
		BOOL bEndNow = FALSE;
		int nStartPos2=-1, nEndPos2=-1;
		if(m_nEndDate==m_nCurReqDate)
		{
			for(int i=0;i<tmpTickNum && tmpTickBuf[i].Minute<m_nEndMinuteXh;i++);
			if(i>0) nStartPos2 = i;
			if(tmpTickBuf[0].Minute>=m_nEndMinuteXh) nStartPos2 = 0;

			for(int j=tmpTickNum-1;j>=0 && tmpTickBuf[j].Minute>m_nStartMinuteXh;j--);
			if(j>0) nEndPos2 = j;
			if(tmpTickBuf[tmpTickNum-1].Minute<=m_nStartMinuteXh) nEndPos2 = tmpTickNum-1;

			if(nStartPos2>=0&&nEndPos2>=0) bEndNow = TRUE;
		}

		int nStartPos=-1, nEndPos=-1;
		if(bStartNow&&bEndNow)
		{
			nStartPos = max(nStartPos1,nStartPos2);
			nEndPos = max(nEndPos1,nEndPos2);
		}
		else if(bStartNow&&!bEndNow)
		{
			nStartPos = nStartPos1;
			nEndPos = nEndPos1;
		}
		else if(!bStartNow&&bEndNow)
		{
			nStartPos = nStartPos2;
			nEndPos = nEndPos2;
		}

		if(m_nStartDate!=m_nCurReqDate&&m_nEndDate!=m_nCurReqDate)
		{
			nStartPos = 0;
			nEndPos = tmpTickNum-1;
		}

		if(nStartPos>=0&&nEndPos>=0&&nEndPos>=nStartPos)
		{
			for(int i=nStartPos;i<=nEndPos;i++)
			{
				m_CalcData.nItemNum++;
				if(tmpTickBuf[i].Now>0)
				{
					m_CalcData.High = max(m_CalcData.High, tmpTickBuf[i].Now);
					if(m_CalcData.Low<1) m_CalcData.Low = tmpTickBuf[i].Now;
					m_CalcData.Low = min(m_CalcData.Low, tmpTickBuf[i].Now);
					m_CalcData.fAmount += tmpTickBuf[i].NowVol*tmpTickBuf[i].Now;
				}
				m_CalcData.dwVolume += tmpTickBuf[i].NowVol;
				if(!testzs(pStkInfo))
				{
					m_CalcData.dwInFlagVol += (tmpTickBuf[i].InOutFlag==1?tmpTickBuf[i].NowVol:0);
					m_CalcData.dwOutFalgVol += (tmpTickBuf[i].InOutFlag==0?tmpTickBuf[i].NowVol:0);
				}
				if(tmpTickBuf[i].NowVol>=g_GH.StatBigVol && !testzs(pStkInfo))
				{
					m_CalcData.nBigItemNum++;
					m_CalcData.dwBigTickVol += tmpTickBuf[i].NowVol;
					if(tmpTickBuf[i].InOutFlag==0) 
					{
						m_CalcData.dwOutBigTickVol += tmpTickBuf[i].NowVol;
						m_CalcData.nOutBigItem++;
					}
					if(tmpTickBuf[i].InOutFlag==1) 
					{
						m_CalcData.dwInBigTickVol += tmpTickBuf[i].NowVol;
						m_CalcData.nInBigItem++;
					}

					int nSize=m_BigTickDate.size();
					vector<TickData>::iterator iter1=m_BigTick.begin();
					vector<long>::iterator iter2=m_BigTickDate.begin();
					long date=GetDate(m_nCurReqDate);
					for(int j=0;j<nSize;j++)
					{
						if(date>m_BigTickDate[j])
							break;
						if(date==m_BigTickDate[j] && tmpTickBuf[i].Minute>=m_BigTick[j].Minute)
							break;									
					}
					m_BigTick.insert(iter1+j,tmpTickBuf[i]);
					m_BigTickDate.insert(iter2+j,date);
				}
			}
			if(m_CalcData.Close<1)						//无缓存可以这么处理
				m_CalcData.Close = tmpTickBuf[nEndPos].Now;
			m_CalcData.Open = tmpTickBuf[nStartPos].Now;

			ReadPriceVolTable(tmpTickBuf, nStartPos, nEndPos);
		}

		if(m_pTick[m_nCurReqDate]&&m_nTickNum[m_nCurReqDate]>0)
			memcpy(tmpTickBuf+tmpTickNum,m_pTick[m_nCurReqDate],m_nTickNum[m_nCurReqDate]*sizeof(TickData));
		TDEL(m_pTick[m_nCurReqDate]);
		m_pTick[m_nCurReqDate] = tmpTickBuf;
		m_nTickNum[m_nCurReqDate] += tmpTickNum;

		//显示刷新
		ShowIt();

		if(m_nEndDate==m_nCurReqDate)
		{
			if(m_pTick[m_nCurReqDate][0].Minute < m_nEndMinuteXh)
			{
				int nCurSel = m_Tab.GetCurSel();
				if(nCurSel!=0)			//列表
					FillList();

				return;
			}
		}
		WantDataMayContinue(m_nCurReqDate);		//继续要这一天的
	}
}

void ZstTickDlg::ReCalcData()
{
	int nStartPos=0,nEndPos=0;
	int startMinute=m_nEndMinuteXh;
	int endMinute=m_nStartMinuteXh;
	for(int i=0;i<m_nTickNum[m_nStartDate] && m_pTick[m_nStartDate][i].Minute<=startMinute;i++);
	if(i>0) nStartPos=i-1;
	for(i=0;i<m_nTickNum[m_nEndDate] && m_pTick[m_nEndDate][i].Minute<endMinute;i++);
	if(i>0) nEndPos=i;
	m_CalcData.nBigItemNum=0;
	m_CalcData.dwBigTickVol=0;
	m_CalcData.dwOutBigTickVol=0;
	m_CalcData.nOutBigItem=0;
	m_CalcData.dwInBigTickVol=0;
	m_CalcData.nInBigItem=0;
	m_BigTick.clear();
	m_BigTickDate.clear();

	for(i=m_nEndDate;i>=m_nStartDate;i--)
	{
		int j=(i==m_nStartDate)?nStartPos:0;
		int k=(i==m_nEndDate)?nEndPos:(m_nTickNum[i]-1);
		for(int l=j;l<=k;l++)
		{
			MemStkInfo *pStkInfo = (*g_pStockIO)[m_nCurGPNo];
			if(m_pTick[i][l].NowVol>=g_GH.StatBigVol&&!testzs(pStkInfo))
			{
				m_CalcData.nBigItemNum++;
				m_CalcData.dwBigTickVol += m_pTick[i][l].NowVol;
				if(m_pTick[i][l].InOutFlag==0) 
				{
					m_CalcData.dwOutBigTickVol += m_pTick[i][l].NowVol;
					m_CalcData.nOutBigItem++;
				}
				if(m_pTick[i][l].InOutFlag==1) 
				{
					m_CalcData.dwInBigTickVol += m_pTick[i][l].NowVol;
					m_CalcData.nInBigItem++;
				}

				m_BigTick.push_back(m_pTick[i][l]);
				m_BigTickDate.push_back(GetDate(i));
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void ZstTickDlg::ShowIt()
{
	MemStkInfo *pStkInfo = (*g_pStockIO)[m_nCurGPNo];
	if(!pStkInfo) return;
	BOOL bZS=testzs(pStkInfo);
	SetWindowText(pStkInfo->Name+(CString)(_F(" 分时区间统计(基于分时成交明细统计)")));
	m_strSartTime = GetDateString(m_nEndDate)+"/"+GetMinuteString(m_nEndMinuteXh);
	m_strEndTime = GetDateString(m_nStartDate)+"/"+GetMinuteString(m_nStartMinuteXh);

	int nCurSel = m_Tab.GetCurSel();
	FillList();
	if(nCurSel==1)
	{
		UpdateData(FALSE);
		return;
	}
	else if(nCurSel==2)
	{
		UpdateData(FALSE);
		InvalidateRect(m_yucitu_rect);
		return;
	}
	
	m_strMinNum=IntToStr(GetMinuteNum())+_F("个");

	if(GetXSFlag(pStkInfo)!=3)
	{
		m_strHigh.Format("%9.2f",(float)m_CalcData.High/1000.0f);
		m_strOpen.Format("%9.2f",(float)m_CalcData.Open/1000.0f);
		m_strLow.Format("%9.2f",(float)m_CalcData.Low/1000.0f);
		m_strClose.Format("%9.2f",(float)m_CalcData.Close/1000.0f);
		if(m_CalcData.fAmount>COMPPREC && !bZS) 
			m_strAver.Format("%9.2f",0.001f*m_CalcData.fAmount/(float)m_CalcData.dwVolume);
	}
	else
	{
		m_strHigh.Format("%9.3f",(float)m_CalcData.High/1000.0f);
		m_strOpen.Format("%9.3f",(float)m_CalcData.Open/1000.0f);
		m_strLow.Format("%9.3f",(float)m_CalcData.Low/1000.0f);
		m_strClose.Format("%9.3f",(float)m_CalcData.Close/1000.0f);
		if(m_CalcData.fAmount>COMPPREC && !bZS) 
			m_strAver.Format("%9.3f",0.001f*m_CalcData.fAmount/(float)m_CalcData.dwVolume);
	}
	m_strVol = MakeVol3(m_CalcData.dwVolume);
	if(m_CalcData.Open>1)
	{
		if(GetXSFlag(pStkInfo)!=3)
			m_strZF.Format("%.2f(%.2f%%)",(float)(m_CalcData.Close-m_CalcData.Open)/1000.0f,100.0f*(float)(m_CalcData.Close-m_CalcData.Open)/(float)m_CalcData.Open);
		else
			m_strZF.Format("%.3f(%.2f%%)",(float)(m_CalcData.Close-m_CalcData.Open)/1000.0f,100.0f*(float)(m_CalcData.Close-m_CalcData.Open)/(float)m_CalcData.Open);
	}
	if(m_CalcData.Low>1)
		m_strZenFu.Format("%.2f",100.0f*(float)(m_CalcData.High-m_CalcData.Low)/(float)m_CalcData.Low);
	if(m_CalcData.nItemNum>0) 
		m_strTickVol = IntToStr(m_CalcData.dwVolume/m_CalcData.nItemNum);
	m_strInVol = MakeVol3(m_CalcData.dwInFlagVol);
	m_strOutVol = MakeVol3(m_CalcData.dwOutFalgVol);
	m_strTickNum=IntToStr(m_CalcData.nItemNum)+_F("个");
	m_strBigTickNum=MakeVol3(m_CalcData.dwBigTickVol)+(CString)"["+IntToStr(m_CalcData.nBigItemNum)+_F("个")+"]";
	m_strOutBigTick=MakeVol3(m_CalcData.dwOutBigTickVol)+(CString)"["+IntToStr(m_CalcData.nOutBigItem)+_F("个")+"]";
	m_strInBigTick=MakeVol3(m_CalcData.dwInBigTickVol)+(CString)"["+IntToStr(m_CalcData.nInBigItem)+_F("个")+"]";
	
	if(!testzs(pStkInfo) && m_CalcData.dwVolume>0)
	{
		//填充饼图
		m_Pie.Reset();
		m_Pie.ShowWindow(SW_SHOW);
		int tmpValue1=m_CalcData.dwOutBigTickVol*360/m_CalcData.dwVolume;
		int tmpValue2=m_CalcData.dwInBigTickVol*360/m_CalcData.dwVolume;
		int tmpValue3=(m_CalcData.dwOutFalgVol-m_CalcData.dwOutBigTickVol)*360/m_CalcData.dwVolume;
		int tmpValue4=(m_CalcData.dwInFlagVol-m_CalcData.dwInBigTickVol)*360/m_CalcData.dwVolume;
		CString tmpStr;
		tmpStr.Format("主买大单:%2.0f%%",(double)tmpValue1*100.0/360.0);
		m_Pie.AddPiece(RGB(200,0,0), RGB(255,255,255), tmpValue1, _F(tmpStr));

		tmpStr.Format("主买小单:%2.0f%%",(double)tmpValue3*100.0/360.0);
		m_Pie.AddPiece(RGB(120,0,0), RGB(255,255,255), tmpValue3, _F(tmpStr));

		tmpStr.Format("主卖大单:%2.0f%%",(double)tmpValue2*100.0/360.0);
		m_Pie.AddPiece(RGB(0,190,0), RGB(255,255,255), tmpValue2, _F(tmpStr));	
				
		tmpStr.Format("主卖小单:%2.0f%%",(double)tmpValue4*100.0/360.0);
		m_Pie.AddPiece(RGB(0,110,0), RGB(255,255,255), tmpValue4, _F(tmpStr));
		
		tmpStr.Format("不明成交:%2.0f%%",(double)(max(0,360-tmpValue1-tmpValue2-tmpValue3-tmpValue4))*100.0/360.0);
		m_Pie.AddPiece(RGB(0,0,0), RGB(255,255,255), max(0,360-tmpValue1-tmpValue2-tmpValue3-tmpValue4), _F(tmpStr));
	}
	else
		m_Pie.ShowWindow(SW_HIDE);
	UpdateData(FALSE);
}

void	ZstTickDlg::FillList()
{
	MemStkInfo *pStkInfo = (*g_pStockIO)[m_nCurGPNo];
	if(!pStkInfo) return;

	m_List.SetRedraw(FALSE);
	
	m_List.DeleteAllItems();
	CHeaderCtrl *pHead = m_List.GetHeaderCtrl();
	int nCol = pHead->GetItemCount();
	for (int i=0;i < nCol;i++)
		m_List.DeleteColumn(0);
	
	static char *BigVolColStr[5] = {"方向","成交价","成交量","日期","时间"};
	static int BigVolColWidth[5] = {60, 50, 70, 50,60};
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	for(i = 0; i < 5;i++)
	{
		lvColumn.pszText = _F(BigVolColStr[i]);
		lvColumn.cx = BigVolColWidth[i];
		lvColumn.iSubItem = i;
		m_List.InsertColumn (i,&lvColumn);
	}
	
	CString tempStr;
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_STATE|LVIF_IMAGE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	for(i=0;i<m_BigTick.size();i++)
	{
		lvItem.iItem = i;
		char strTime[128]={0};
		if(m_BigTick[i].InOutFlag==0)
		{
			sprintf(strTime,"%s","主买");
			lvItem.iImage=0;
		}
		else if(m_BigTick[i].InOutFlag==1)
		{
			sprintf(strTime,"%s","主卖");
			lvItem.iImage=1;
		}
		else
		{
			sprintf(strTime,"%s","中性");
			lvItem.iImage=2;
		}
		
		lvItem.pszText = strTime;
		m_List.InsertItem (&lvItem);
		
		short xsflag = GetXSFlag(pStkInfo);
		float fNow = GetAbsPrice(pStkInfo, m_BigTick[i].Now, xsflag);
		if(xsflag==2)
			tempStr.Format("%-5.2f",fNow);
		else
			tempStr.Format("%-5.3f",fNow);
		m_List.SetItemText (i,1,tempStr);
		
		tempStr.Format("%d",m_BigTick[i].NowVol);
		m_List.SetItemText (i,2,tempStr);
		
		tempStr.Format("%s",GetTdxDateStr(m_BigTickDate[i]));
		m_List.SetItemText (i,3,tempStr);
		
		tempStr.Format("%02d:%02d",m_BigTick[i].Minute/60, m_BigTick[i].Minute%60);
		m_List.SetItemText(i,4,tempStr);
	}
	m_List.SetRedraw(TRUE);
}

void	ZstTickDlg::HideNormal(BOOL bHide)
{
	SetRedraw(FALSE);
	int nCtlNum = sizeof(ctlNormal)/sizeof(UINT);
	if(bHide)
	{
		for(int i=0;i<nCtlNum;i++)
			((CWnd*)GetDlgItem(ctlNormal[i]))->ShowWindow(SW_HIDE);
		m_Pie.ShowWindow(SW_HIDE);
	}
	else
	{
		for(int i=0;i<nCtlNum;i++)
			((CWnd*)GetDlgItem(ctlNormal[i]))->ShowWindow(SW_SHOW);
		m_Pie.ShowWindow(SW_SHOW);
	}
	SetRedraw(TRUE);
	Invalidate();
}

long ZstTickDlg::GetDate(short nBackDate)
{
	long nDate = m_nCurDate;
	if(nBackDate!=0)
	{
		short whichHis = g_nZSTManyDay-nBackDate;
		nDate = m_pUZst->GetDate(whichHis);
	}
	return nDate;
}

CString ZstTickDlg::GetDateString(short nBackDate)
{
	long nDate = m_nCurDate;
	if(nBackDate!=0)
	{
		short whichHis = g_nZSTManyDay-nBackDate;
		nDate = m_pUZst->GetDate(whichHis);
	}
	return GetTdxDateStr(nDate);
}


CString ZstTickDlg::GetMinuteString(short nMinuteXh)
{
	nMinuteXh = min(max(m_pUZst->GetFirstMinute(), nMinuteXh),m_pUZst->GetLastMinute());
	return GetTdxTimeStr(nMinuteXh);
}

long	ZstTickDlg::GetMinuteNum()
{
	short nStartMinuteXh = min(max(m_pUZst->GetFirstMinute(), m_nStartMinuteXh),
		m_pUZst->GetLastMinute());
	short nEndMinuteXh = min(max(m_pUZst->GetFirstMinute(), m_nEndMinuteXh),
		m_pUZst->GetLastMinute());

	long nTotalMinute = 0;
	for(int i=m_nStartDate;i<=m_nEndDate;i++)
		nTotalMinute+=m_pUZst->GetRealTotalMinuteNum();
	nTotalMinute -= m_pUZst->Xh2TdxMinute(nEndMinuteXh);
	nTotalMinute -= (m_pUZst->GetRealTotalMinuteNum()-m_pUZst->Xh2TdxMinute(nStartMinuteXh)-1);
	return nTotalMinute;
}

void ZstTickDlg::OnOK()
{

}

void ZstTickDlg::OnCancel() 
{
	for(int i=0;i<MAX_ZST_MANYDAY+1;i++)
	{
		if(m_pTick[i])
		{
			delete []m_pTick[i];
			m_pTick[i] = NULL;
		}
	}
	g_LastSelTab=m_Tab.GetCurSel();
	CDialog::OnCancel();
}

void ZstTickDlg::Init()
{
	int nCurSel = m_Tab.GetCurSel();
	switch(nCurSel) 
	{
	case 1:
		{
			HideNormal(TRUE);
			m_List.ShowWindow(SW_SHOW);
			FillList();
		}
		break;
	case 2:
		{
			HideNormal(TRUE);
			m_List.ShowWindow(SW_HIDE);
		}
		break;
	case 0:
	default:
		{
			HideNormal(FALSE);
			m_List.ShowWindow(SW_HIDE);
			ShowIt();
		}
		break;
	}
}

void ZstTickDlg::OnSelchangeZstticktab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int nCurSel = m_Tab.GetCurSel();
	switch(nCurSel) 
	{
	case 1:
		{
			HideNormal(TRUE);
			m_List.ShowWindow(SW_SHOW);
		}
		break;
	case 2:
		{
			HideNormal(TRUE);
			m_List.ShowWindow(SW_HIDE);
		}
		break;
	case 0:
	default:
		{
			HideNormal(FALSE);
			m_List.ShowWindow(SW_HIDE);
			ShowIt();
		}
		break;
	}
	*pResult = 0;
}

//////////////////////////////////////////////////////////////////////////
short ZstTickDlg::ReadPriceVolTable(TickData *indapbase, int nBgnPos, int nEndPos)
{	
	for (int i=nBgnPos;i<=nEndPos;i++)
	{
		if (indapbase[i].NowVol<1 || indapbase[i].Now<1)
			continue;
	
		map<long, PriceVol, less<long> >::iterator iter = m_mapPriceVol.find(indapbase[i].Now);
		if(iter==m_mapPriceVol.end())		//新
		{
			PriceVol pv={0};
			pv.Price = indapbase[i].Now;
			pv.Volume = indapbase[i].NowVol;
			if(indapbase[i].InOutFlag == 0)
			{
				pv.Kcl = indapbase[i].NowVol;
				pv.Knum++;
			}
			else if(indapbase[i].InOutFlag == 1)
			{
				pv.Pcl = indapbase[i].NowVol;
				pv.Pnum++;
			}
			pv.Num++;
			m_mapPriceVol[pv.Price] = pv;
		}
		else
		{
			PriceVol *ppv = (PriceVol *)&((*iter).second);
			ppv->Volume += indapbase[i].NowVol;
			if(indapbase[i].InOutFlag == 0)
			{
				ppv->Kcl += indapbase[i].NowVol;
				ppv->Knum++;
			}
			else if(indapbase[i].InOutFlag == 1)
			{
				ppv->Pcl += indapbase[i].NowVol;
				ppv->Pnum++;
			}
			ppv->Num++;
		}
	}
	return m_mapPriceVol.size();
}

void ZstTickDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	switch(m_Tab.GetCurSel())  
	{
	case 2:
	    {
			DrawYuCiTu(&dc);
	    }
		break;
	case 0:
	case 1:
	default:
	    {
	    }
	    break;
	}
}

BOOL ZstTickDlg::OnEraseBkgnd(CDC* pDC) 
{
	return CDialog::OnEraseBkgnd(pDC);
}

void ModifyRect(CRect &rc)
{
	if(rc.Height()==0)
		rc.top-=1;
	if(rc.Height()>20)
	{
		int tmp=(rc.Height()-20)/2;
		rc.top+=tmp;
		rc.bottom-=tmp;
	}
}

void ZstTickDlg::DrawYuCiTu(CDC *pDC)
{
	MemStkInfo *pStkInfo = (*g_pStockIO)[m_nCurGPNo];
	if(!pStkInfo) return;
	BOOL bZS=testzs(pStkInfo);
	if(m_mapPriceVol.size()==0) return;

	double	price_max=0,price_min=0xffff;
	ULONG buy_max=0,sell_max=0,volume_max=0,nRow=0;
	short xsflag=2;
	CString tempStr;
	CRect ycrc;
	map<long, PriceVol, less<long> >::iterator iter = m_mapPriceVol.begin();
	for(;iter!=m_mapPriceVol.end();iter++)
	{
		PriceVol pv = (*iter).second;
		
		xsflag = GetXSFlag(pStkInfo);
		float fNow = GetAbsPrice(pStkInfo, pv.Price, xsflag);
		if(price_max<fNow) price_max=fNow;
		if(price_min>fNow) price_min=fNow; 
		if(bZS)
		{
			if(volume_max<pv.Volume) volume_max=pv.Volume;
		}
		else
		{
			if(buy_max<pv.Kcl) buy_max=pv.Kcl;
			if(sell_max<pv.Pcl) sell_max=pv.Pcl;
		}
		nRow++;
	}

	CDC memdc;
	memdc.CreateCompatibleDC(pDC);
	CBitmap membm;
	membm.CreateCompatibleBitmap(pDC,m_yucitu_rect.Width(),m_yucitu_rect.Height());
	CBitmap *poldbm=memdc.SelectObject(&membm);
	memdc.FillSolidRect(0,0,m_yucitu_rect.Width(),m_yucitu_rect.Height(),RGB(240,240,240));
	CFont btFont;
	btFont.CreateFont(12 ,0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DEFAULT_QUALITY,VARIABLE_PITCH|FF_MODERN,SONG_FONT);
	CFont *poldfont=memdc.SelectObject(&btFont);
	memdc.SetTextColor(RGB(0,0,0));
	memdc.SetBkMode(TRANSPARENT);

	ycrc.SetRect(48,5-1,m_yucitu_rect.Width()-10,m_yucitu_rect.Height()-5+1);
	memdc.FrameRect(ycrc,&CBrush(RGB(200,200,200)));
	ycrc.DeflateRect(1,1,1,1);

	double tmpd,tmpd2;
	tmpd=(ycrc.Height()/(double)nRow);
	if(bZS)
		tmpd2=ycrc.Width()/(double)volume_max;
	else
		tmpd2=ycrc.Width()/(double)(buy_max+sell_max);
	double xmid=bZS?ycrc.left:ycrc.left+buy_max*tmpd2;

	iter=m_mapPriceVol.begin();
	double cx=0,cy=tmpd,y=ycrc.bottom,oldy;
	int yNow=ycrc.bottom;

	for(int i=0;i<nRow;i++)
	{
		PriceVol pv = (*iter).second;

		oldy=y+0.5;
		y=ycrc.bottom-cy*(i+1);
		
		if((cy<1. && y+0.5<=yNow) || cy>1.)
		{
			if(bZS)
			{
				cx=pv.Volume*tmpd2;
				CRect rc1(xmid+0.5,y+0.5,xmid+cx+0.5,oldy);
				ModifyRect(rc1);
				g_GUI.DrawGradient(&memdc,rc1,RGB(0,0,255),RGB(0,0,180),FALSE);
			}
			else
			{
				cx=pv.Kcl*tmpd2;
				CRect rc1(xmid-cx+0.5,y+0.5,xmid+0.5,oldy);
				ModifyRect(rc1);
				g_GUI.DrawGradient(&memdc,rc1,RGB(255,40,40),RGB(210,0,0),FALSE);
				cx=pv.Pcl*tmpd2;
				CRect rc2(xmid+0.5,y+0.5,xmid+cx+0.5,oldy);
				ModifyRect(rc2);
				g_GUI.DrawGradient(&memdc,rc2,RGB(0,255,0),RGB(0,180,0),FALSE);
			}
			if(cy<1.) yNow--;
		}
		iter++;
	}
	CRect tmprc(3,ycrc.top,44,ycrc.top+10);
	int nRow2=ycrc.Height()/tmprc.Height();
	if(price_max!=price_min)
	{
		double step=(price_max-price_min)/(nRow2-1+0.001);
		if(xsflag==2 && step<0.01)
		{
			step=0.01;
			nRow2=(price_max-price_min)/step+0.5+1;
		}
		else if(step<0.001)
		{
			step=0.001;
			nRow2=(price_max-price_min)/step+0.5+1;
		}
		if(nRow2>nRow)
		{
			nRow2=nRow;
			step=(price_max-price_min)/(nRow2-1+0.001);
		}
		double blank=(ycrc.Height()-nRow2*tmprc.Height())/(double)(nRow2-1+0.001);
		for(i=0;i<nRow2;i++)
		{
			tmprc.OffsetRect(0,-tmprc.top+ycrc.top+i*(tmprc.Height()+blank)+0.5);
			double price=price_max-i*step;
			if(xsflag==2)
				tempStr.Format("%5.2f",price);
			else
				tempStr.Format("%5.3f",price);

			memdc.DrawText(tempStr,tmprc,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		}
	}
	
	pDC->BitBlt(m_yucitu_rect.left,m_yucitu_rect.top,m_yucitu_rect.Width(),m_yucitu_rect.Height(),&memdc,0,0,SRCCOPY);
	memdc.SelectObject(poldfont);
	btFont.DeleteObject();
	memdc.SelectObject(poldbm);
	membm.DeleteObject();
	memdc.DeleteDC();
}

int CALLBACK  MyListCompare_ZstTick(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	//通过传递的参数来得到CSortList对象指针，从而得到排序方式
	CSortListCtrl * pV=(CSortListCtrl *)lParamSort;
	//通过ItemData来确定数据
	CString szComp1,szComp2;
	int iCompRes;
	szComp1=pV->GetItemText(lParam1,pV->m_nSortedCol);
	szComp2=pV->GetItemText(lParam2,pV->m_nSortedCol);
	switch(pV->m_nSortedCol)
	{
	case 1:
	case 2:
		iCompRes=atof(szComp1)<=atof(szComp2)?-1:1;
		break;
	default:
		iCompRes=szComp1.Compare(szComp2);
		break;
	}
	//根据当前的排序方式进行调整
	if(pV->m_fAsc)
		return iCompRes;
	else
		return -iCompRes;
}

void ZstTickDlg::OnColumnclickZstticklist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	for (int i = 0; i < m_List.GetItemCount(); i++)
	{
		m_List.SetItemData(i, i);//供排序使用的item编号
	}
	
	//设置排序方式
	if( pNMListView->iSubItem ==m_List.m_nSortedCol )
		m_List.m_fAsc = !m_List.m_fAsc;
	else
	{
		m_List.m_fAsc = TRUE;
		m_List.m_nSortedCol = pNMListView->iSubItem;
	}
	//调用排序函数,此函数为CListCtrl定义好的,但是需要调用我们定义的函数才比较任意两个项目的值
	m_List.SortItems(MyListCompare_ZstTick, (LPARAM)&m_List); 
	
	for ( i = 0; i < m_List.GetItemCount(); ++i)
		m_List.SetItemData(i, i);//供排序使用的item编号

	*pResult = 0;
}

void ZstTickDlg::OnButtonUpdate() 
{
	CString tempStr;
	m_bigVol.GetWindowText(tempStr);
	int StatBigVol=atoi(tempStr);
	if(StatBigVol<10)
	{
		TDX_MessageBox(m_hWnd,"门限值必须大于10!",MB_OK|MB_ICONERROR);
		tempStr.Format("%d",10);
		m_bigVol.SetWindowText(tempStr);
		return;
	}
	if(StatBigVol==g_GH.StatBigVol) return;
	g_GH.StatBigVol=StatBigVol;
	ReCalcData();
	ShowIt();
}

void ZstTickDlg::OnRclickZstticklist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint point;
	GetCursorPos(&point);
	
	CNewMenu menu,*pPopup;
	menu.LoadMenu(IDR_LONELY_MENU);
	pPopup = (CNewMenu *)menu.GetSubMenu(24);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,this);
	
	*pResult = 0;
}

#include "ExportDlg.h"
void ZstTickDlg::OnSaveBigVol()
{
	MemStkInfo *tmpInfo=(*g_pStockIO)[m_nCurGPNo];
	if(!tmpInfo) return;

	CExportDlg dlg;
	dlg.m_strInitFileName = tmpInfo->Code;
	if(dlg.DoModal() == IDOK)
	{
		if(dlg.m_nTabSel == 0 || dlg.m_nTabSel == 1)
		{
			CFileException ex;
			CStdioFile  File;
			if(File.Open(dlg.m_strFileName,CFile::modeCreate|CFile::modeWrite,&ex) )
			{
				char Buffer[1024];
				long i = 0;
				sprintf(Buffer,"                  %s (%s)\n",tmpInfo->Name,tmpInfo->Code);
				File.Write(Buffer,strlen(Buffer));
				File.Write("\r\n",2);

				sprintf(Buffer,"%8s\t%6s\t%8s\t%15s\t%8s\t","日期","时间","价格","数量","方向");
				File.Write(_F(Buffer),strlen(Buffer));
				File.Write("\r\n",2);
				i=m_BigTick.size();
				for(i=i-1;i>=0;i--)
				{
					sprintf(Buffer,"%8s\t% 02d:%02d\t",GetTdxDateStr(m_BigTickDate[i]),m_BigTick[i].Minute/60,m_BigTick[i].Minute%60);
					File.Write(_F(Buffer),strlen(Buffer));

					short xsflag = GetXSFlag(tmpInfo);
					float fNow = GetAbsPrice(tmpInfo, m_BigTick[i].Now, xsflag);
					if(xsflag==2) sprintf(Buffer,"%-8.2f\t",fNow);
					else sprintf(Buffer,"%-8.3f\t",fNow);
					File.Write(_F(Buffer),strlen(Buffer));

					sprintf(Buffer,"%15u\t",m_BigTick[i].NowVol);
					File.Write(_F(Buffer),strlen(Buffer));

					if(m_BigTick[i].InOutFlag==0)	sprintf(Buffer,"%8s","主买");
					else if(m_BigTick[i].InOutFlag==1)	sprintf(Buffer,"%8s","主卖");
					else sprintf(Buffer,"%8s","中性");
					File.Write(_F(Buffer),strlen(Buffer));

					File.Write("\n",1);
				}
				File.Close();
				if(TDX_MessageBox(this->m_hWnd,"导出成功,是否打开此文件?",MB_ICONQUESTION|MB_OKCANCEL)==IDOK)
				{
					CString tmpStr;
					tmpStr.Format("\"%s\"",dlg.m_strFileName);
					::ShellExecute(NULL,NULL,tmpStr,NULL,NULL,SW_SHOW);
				}
			}
			else
			{
				char strError[1024];
				ex.GetErrorMessage(strError,1024);
				TDX_MessageBox(this->m_hWnd,"文件创建错误!\n"+(CString)strError,MB_ICONEXCLAMATION|MB_OK);
			}
			return;
		}
		else
		{
			TDX_MessageBox(this->m_hWnd,"不可导出为图片!",MB_OK|MB_ICONEXCLAMATION);
			return;
		}
	}
}
