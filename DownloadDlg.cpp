// DownloadDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "DownloadDlg.h"
#include "SelectGPDlg.h"
#include "SelectOneDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDownloadDlg dialog

CDownloadDlg::CDownloadDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDownloadDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDownloadDlg)
	m_bDay = TRUE;
	m_bFxMin = FALSE;
	m_bMin = FALSE;
	m_bJbm = FALSE;
	m_Time1 = 0;
	m_Time2 = 0;
	m_bDownAllGp = TRUE;
	m_bExDay = FALSE;
	//}}AFX_DATA_INIT
	m_nMainID = 0;
	m_nCurGPNo = 0;	//下载到了第几号股票
	nCurRemotePos = 0;
	bRemoteFileOpen = FALSE;
	m_startdate = m_enddate = m_used_startdate = m_used_enddate = 0;
	m_startdateFm = m_enddateFm = m_used_startdateFm = m_used_enddateFm = 0;
	m_bModal = FALSE;
	m_bDown_AtOnce = FALSE;
	m_bSegmentTxt = g_bSupportETF; //采用片短TXT方式
	m_ntofl = m_whichfl = 0;
	m_nWhichJBM = 0;
	m_nTabSel = 0;

	m_bDownToken = FALSE;
	m_bTmpDownLast = FALSE;
	//
	int i = 0;
	for(i=0;i<EXFILE_SAMENUM;i++)
	{
		m_fpdateday[i]		= NULL;
		m_datedaylen[i]		= 0;
		m_datedayspc[i]		= 0;
		m_used_reqdate[i]	= 0;
	}
	m_exfilercvbuf	= NULL;
	m_exfilercvspc	= 0;
	m_reqdatenum	= 0;
	m_used_curdate	= 0;
	m_used_downday	= 0;
	m_filepos = m_filercvlen = m_filelen = 0;
}

CDownloadDlg::~CDownloadDlg()
{
	int i = 0;
	for(i=0;i<EXFILE_SAMENUM;i++)
	{
		TDELA(m_fpdateday[i]);
	}
	TDELA(m_exfilercvbuf);
}

void CDownloadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDownloadDlg)
	DDX_Control(pDX, IDC_HINT_STATIC, m_Hint_Status);
	DDX_Control(pDX, IDC_DOWNGP_LIST, m_GpReport_List);
	DDX_Control(pDX, IDC_STATUS_STATIC, m_Status_Static);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_Time1);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_Time2);
	DDX_Check(pDX, IDC_DOWNALL_CHECK, m_bDownAllGp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDownloadDlg, CDialog)
	//{{AFX_MSG_MAP(CDownloadDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_MESSAGE(UM_GETDATA_ACK,OnGetDataAck)
	ON_BN_CLICKED(IDC_DOWN_FXMIN_CHECK, OnDownFxMinCheck)
	ON_BN_CLICKED(IDC_DOWN_DAY_CHECK, OnDownDayCheck)
	ON_BN_CLICKED(IDC_DOWNALL_CHECK, OnDownallCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDownloadDlg message handlers

char *DownTabStr[4] = {"日线数据","5分钟数据","分时图数据","个股资料"};
#define HQFX_DOWN	0
#define FXMIN_DOWN	1		//分钟数据,仅5分钟
#define MIN_DOWN	2
#define JBM_DOWN	3

char *DownGpColStr[2] = {"股票代码","股票名称"};
int DownGpColWidth[2] = {60,65};

BOOL CDownloadDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetDateCtrlRange(GetDlgItem(IDC_DATETIMEPICKER1));
	SetDateCtrlRange(GetDlgItem(IDC_DATETIMEPICKER2));
	m_Status_Static.SetTextColor(RGB(0,0,255));
	m_Hint_Status.SetTextColor(RGB(255,64,64));

	if(g_GH.bOnlyDownAGData)
		GetDlgItem(IDC_DOWNALL_CHECK)->SetWindowText(_F("下载所有A股股票的数据"));
	if(CanLevel2() && g_bSupportDDE)
	{
		GetDlgItem(IDC_DOWN_DAY_CHECK)->SetWindowText(_F("日线,实时行情和日线统计数据"));
		GetDlgItem(IDC_DOWNALL_CHECK)->MoveWindow(0,0,0,0); //将选择控件移走,固定为全部股票
	}

	long lLastDate = GetVipDocLastDate(m_bDown_AtOnce);
//	lLastDate = GetForwardDate(lLastDate);
	if(lLastDate > g_lOpenRq)
		lLastDate = g_lOpenRq;

	CTime tmpTime1(lLastDate/10000,(lLastDate%10000)/100,lLastDate%100,0,0,0);
	CTime tmpTime2(g_lOpenRq/10000,(g_lOpenRq%10000)/100,g_lOpenRq%100,0,0,0);
	m_Time1 = tmpTime1;
	m_Time2 = tmpTime2;

	//股票列表框
	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_GP));
	m_ImageList.Add(hIcon);
	m_GpReport_List.SetImageList(&m_ImageList,LVSIL_SMALL);
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	for(int i = 0; i < 2;i++)
	{
		lvColumn.pszText = _F(DownGpColStr[i]);
		lvColumn.cx = DownGpColWidth[i];
		lvColumn.iSubItem = i;
		m_GpReport_List.InsertColumn (i,&lvColumn);
	}
	m_bDownAllGp = TRUE;
	MemStkInfo *tmpInfo;
	int			tmpInt;
	AddedGPNum[HQFX_DOWN]=0;
	AddedGPNum[FXMIN_DOWN]=0;
	for(i=0;i<g_pStockIO->GetStockNum(ALL_ALL);i++)
	{
		tmpInfo=(*g_pStockIO)[i];
		if(!tmpInfo) continue;
		tmpInt = GetStockType(tmpInfo->setcode,tmpInfo->Code);
		//只下载A股,中小板和指数
		if(g_GH.bOnlyDownAGData && tmpInt!=CODE_SHAG && tmpInt!=CODE_SZAG && tmpInt!=CODE_ZXB && tmpInt!=CODE_CYB && tmpInt!=CODE_SZJJ && tmpInt!=CODE_SHJJ && !testzs(tmpInfo))
			continue;
		AddedGPIndex[HQFX_DOWN][AddedGPNum[HQFX_DOWN]] = i;
		AddedGPNum[HQFX_DOWN]++;

		AddedGPIndex[FXMIN_DOWN][AddedGPNum[FXMIN_DOWN]] = i;
		AddedGPNum[FXMIN_DOWN]++;
	}		
	AddedGPNum[MIN_DOWN] = AddedGPNum[JBM_DOWN] = 0;
	AfterTabChange();
	//
	if(TRUE||m_bDown_AtOnce) //立即下载
	{
		m_bDay = TRUE;
		PostMessage(WM_COMMAND,IDOK);
	}
	UpdateData(FALSE);
	return TRUE;
}

void CDownloadDlg::FillGPReport()
{
	m_GpReport_List.SetRedraw(FALSE);	
	m_GpReport_List.DeleteAllItems();

	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	int i;
	for(i = 0;i < AddedGPNum[m_nTabSel];i++)
	{
		lvItem.iItem = i;
		lvItem.iImage = 0;
		lvItem.pszText = (*g_pStockIO)[AddedGPIndex[m_nTabSel][i]]->Code;
		m_GpReport_List.InsertItem(&lvItem);
		m_GpReport_List.SetItemText(i,1,(*g_pStockIO)[AddedGPIndex[m_nTabSel][i]]->Name);
		m_GpReport_List.SetItemData(i,AddedGPIndex[m_nTabSel][i]);//仅为了拖曳
	}

	m_GpReport_List.SetRedraw(TRUE);
}

void CDownloadDlg::EnableOther(BOOL bEnable)
{
	GetDlgItem(IDCANCEL)->EnableWindow(bEnable);
//	GetDlgItem(IDC_DATETIMEPICKER1)->EnableWindow(bEnable);
//	GetDlgItem(IDC_DATETIMEPICKER2)->EnableWindow(bEnable);
//	GetDlgItem(IDC_DOWNALL_CHECK)->EnableWindow(bEnable);
	if(m_nTabSel>0)
	{
//		GetDlgItem(IDC_ADD)->EnableWindow(bEnable);
//		GetDlgItem(IDC_DELETE)->EnableWindow(bEnable);
	}
}

//执行下载或取消下载
void CDownloadDlg::OnOK() 
{
	if(!g_bDowning)
	{
		if(!OnLine) return;
		m_nCurGPNo = 0;//下载到了第几号股票,复位
		m_bDownToken = FALSE;

		UpdateData(TRUE);	//刷新对话框变量
		if(!m_bJbm && !m_bDay && !m_bMin && !m_bFxMin)
		{
			m_Status_Static.SetText(_F("请选择下载项!"));
			return;
		}
		if(AddedGPNum[HQFX_DOWN] < 1 && AddedGPNum[MIN_DOWN] < 1 && AddedGPNum[JBM_DOWN] < 1 && AddedGPNum[FXMIN_DOWN] < 1)
		{
			m_Status_Static.SetText(_F("请添加下载的股票!"));
			return;
		}
		if((!m_bDay || m_bDay && AddedGPNum[HQFX_DOWN] < 1) && 
		   (!m_bMin || m_bMin && AddedGPNum[MIN_DOWN] < 1)  && 
		   (!m_bJbm || m_bJbm && AddedGPNum[JBM_DOWN] < 1)  &&
		   (!m_bFxMin || m_bFxMin && AddedGPNum[FXMIN_DOWN] < 1)
		  )
		{
			m_Status_Static.SetText(_F("请选择下载项并添加下载股票!"));
			return;
		}
		if(m_nTabSel==0)
		{
			m_startdate = m_Time1.GetYear()*10000+m_Time1.GetMonth()*100+m_Time1.GetDay();
			m_enddate = m_Time2.GetYear()*10000+m_Time2.GetMonth()*100+m_Time2.GetDay();
		}
		else if(m_nTabSel==1)
		{
			m_startdateFm = m_Time1.GetYear()*10000+m_Time1.GetMonth()*100+m_Time1.GetDay();
			m_enddateFm = m_Time2.GetYear()*10000+m_Time2.GetMonth()*100+m_Time2.GetDay();
		}

		if(m_bDay && (m_startdate>m_enddate || m_startdate < 19901201 || m_enddate > g_iToday))
		{
			if(m_startdate < 19901201 || m_enddate > g_iToday)
				m_Status_Static.SetText(_F("日期应在19901201和今天之间!"));
			else
				m_Status_Static.SetText(_F("日期选取不合理!必要时请确认您本机的时间是否正确!"));
			return;
		}
		long nTopStartFm = 10000*(g_iToday/10000)+100*(((g_iToday/100)%100-1+12)%12)+g_iToday%100;
		if(m_bFxMin && (m_startdateFm>m_enddateFm || m_startdateFm < nTopStartFm || m_enddateFm > g_iToday))
		{
			if(m_startdateFm < nTopStartFm || m_enddateFm > g_iToday)
				m_Status_Static.SetText(_F("日期应在最近一个月之内!"));
			else
				m_Status_Static.SetText(_F("日期选取不合理!必要时请确认您本机的时间是否正确!"));
			return;
		}
		if(m_bJbm && g_TdxCfg.JbmNum>1)
		{
			CSelectOneDlg dlg;
			dlg.m_strHintStr=_F("下载主站中有多种基本资料,请选择一种:");
			for(int n=0;n<g_TdxCfg.JbmNum;n++)
				dlg.AddString(_F(g_TdxCfg.Jbmname[n]));
			if(dlg.DoModal()!=IDOK)
				return;
			if(dlg.m_nCurSel<0) dlg.m_nCurSel=0;
			m_nWhichJBM=dlg.m_nCurSel;
		}
		//对于第二代行情,下载日线同时下载日线扩展数据
		m_bExDay = FALSE;
		if(CanLevel2() && g_bSupportDDE)
			m_bExDay = m_bDay;
		/*
		//如果现在时间还是3:30以前,不下载今天的数据
		m_bTmpDownLast=FALSE;
		if(m_enddate==g_lOpenRq)
			m_bTmpDownLast=TRUE;
		SYSTEMTIME  nowtime;
		GetCorrespondingTime( &nowtime );
		int m = nowtime.wHour*60+nowtime.wMinute;
		if(m_enddate==g_lOpenRq && !g_bYesterdayFlag && m < 940) //如果是今天的数据,但还没有到3:40,不下今天的数据
		{
			long lEndDate = GetBackDate(g_lOpenRq);
			m_enddate = lEndDate;
			if(m_startdate>m_enddate)
				m_startdate=m_enddate;
		}
		*/

		//如果日期段太长，分几次发送
		m_used_startdate = m_startdate;
		m_used_enddate   = m_enddate;
		if(m_used_startdate+40000 < m_enddate)
			m_used_enddate = m_used_startdate+40000;

		m_used_startdateFm = m_startdateFm;
		m_used_enddateFm = m_enddateFm;

		GetDlgItem(IDOK)->SetWindowText(_F("取消下载"));
		EnableOther(FALSE);
		//开始下
		for(int i=0;i < MAXGPNUM;i++)
			NeedWant_Jbm[i] = 1;
		g_bDowning = TRUE;
		m_nCurGPNo		= 0;
		m_used_curdate	= 0;
		m_used_downday	= 0;
		if(m_bDay && AddedGPNum[HQFX_DOWN] > 0)
		{
			if(g_pLcIdxMng) g_pLcIdxMng->SetNeedReCalc();
			Down_HqData();
		}
		else if(m_bExDay && AddedGPNum[HQFX_DOWN] > 0)
			Down_ExDayFile(SZ);
		else if(m_bExDay && AddedGPNum[HQFX_DOWN] > 0)
			Down_ExDayFile(SH);
		else if(m_bFxMin && AddedGPNum[FXMIN_DOWN] > 0)
			Down_FxMinData();
		else if(m_bMin && AddedGPNum[MIN_DOWN] > 0)
			Down_MinData();
		else if(m_bJbm && AddedGPNum[JBM_DOWN] > 0)
			Down_JbmFileLength();
		else
			DownFinished();
	}
	else
	{
		if(TDX_MessageBox(m_hWnd,"您确定取消下载吗？",MB_ICONQUESTION|MB_OKCANCEL)==IDCANCEL)
			return;
		g_bDowning = FALSE;
		GetDlgItem(IDOK)->SetWindowText(_F("开始下载"));
		EnableOther(TRUE);
		m_Status_Static.SetText(_F("下载被取消."));
		m_Progress.SetPos(0);
		if(m_bDown_AtOnce)
			CDialog::OnCancel();
	}
}

void CDownloadDlg::Down_HqData()
{
	if(m_nCurGPNo == 0)
	{
		m_Status_Static.SetText(_F("正在下载当天行情数据.."));
		m_Progress.SetRange(0,AddedGPNum[HQFX_DOWN]);
		m_Progress.SetPos(0);
	}
	char *pTranBuff = new char[REQBUFFER_LEN];
	int   gpnum = 0;
	MemStkInfo *pStkInfo;
	g_nMainID = MainIDPlus(m_nMainID,DOWNLOAD_WIN);
	g_nAssisID = SIMHQ_ASSISID;
	while(m_nCurGPNo < AddedGPNum[HQFX_DOWN] && gpnum < SIMHQ_WANTNUM)
	{
		pStkInfo = (*g_pStockIO)[AddedGPIndex[HQFX_DOWN][m_nCurGPNo]];
		if(pStkInfo)
		{
			memcpy(pTranBuff+gpnum*(1+SH_CODE_LEN),&pStkInfo->setcode,1);
			memcpy(pTranBuff+gpnum*(1+SH_CODE_LEN)+1,pStkInfo->Code,SH_CODE_LEN);
			gpnum++;
		}
		m_nCurGPNo++;
		m_Progress.SetPos(m_nCurGPNo);
	}
	g_pGetData->GetSimHq(pTranBuff,gpnum);
	delete pTranBuff;
}

/*
BOOL CDownloadDlg::OneStock_PH(MemStkInfo *pStkInfo)
{
	if(!pStkInfo) return FALSE;

	struct AnalyData ls,oldls;
	struct CurrStockData tmphq;
	int factor = 100;
	char   Str[MAX_PATH];
	FILE  *fp;
	short stockkind = GetStockType(pStkInfo->setcode,pStkInfo->Code);
	g_pStockIO->ReadCurrStockData(pStkInfo,&tmphq);
	//对于开放式基金和ETF的申续,要对行情中的Open,Now,Max,Min进行特别处理
	if ((pStkInfo->setcode==SZ&&strncmp(pStkInfo->Code,"16",2)==0) || (pStkInfo->setcode==SH&&strncmp(pStkInfo->Code,"519",3)==0))
	{
		if(tmphq.Open <= COMPPREC)	tmphq.Open = tmphq.Close;
		if(tmphq.Now <= COMPPREC)	tmphq.Now = tmphq.Close;
		if(tmphq.Max <= COMPPREC)	tmphq.Max = tmphq.Close;
		if(tmphq.Min <= COMPPREC)	tmphq.Min = tmphq.Close;
		tmphq.Max = max(tmphq.Max,tmphq.Now);
		tmphq.Min = min(tmphq.Min,tmphq.Now);
	}
	for(int i=0;pStkInfo->setcode==SH && i<g_nETFInfoNum;i++)
	{
		if(g_ETFInfo[i].setcode==SH && strcmp(pStkInfo->Code,g_ETFInfo[i].Code)==0)
		{
			if(tmphq.Open <= COMPPREC)	tmphq.Open = tmphq.Now;
			if(tmphq.Max <= COMPPREC)	tmphq.Max = tmphq.Now;
			if(tmphq.Min <= COMPPREC)	tmphq.Min = tmphq.Now;
			break;
		}
	}	
	//
    if (0==tmphq.ItemNum && (tmphq.Open<COMPPREC||tmphq.Max<COMPPREC||tmphq.Min<COMPPREC) )
        return TRUE;
	if (tmphq.Now<COMPPREC)
		return TRUE;
	if (!testzs(pStkInfo) && tmphq.Volume==0L && (tmphq.Open<COMPPREC||tmphq.Max<COMPPREC||tmphq.Min<COMPPREC) )
		return TRUE;

	if (GetXSFlag(pStkInfo)==3 && stockkind!=CODE_SZGZHG && stockkind!=CODE_SHGZHG)
		factor = 1000;
	else
		factor = 100;
	if ((stockkind==CODE_SZJJ || stockkind==CODE_SHJJ) && g_lOpenRq < 20030303)
		factor = 100;
	////
	ls.Time.Date = g_lOpenRq;
	ls.l_Open    = (DWORD)(tmphq.Open*factor+0.5);
	ls.l_High    = (DWORD)(tmphq.Max*factor+0.5);
	ls.l_Low     = (DWORD)(tmphq.Min*factor+0.5);
	ls.l_Close   = (DWORD)(tmphq.Now*factor+0.5);
	if (ls.l_Close!=0L)
	{
		if (ls.l_Open==0L)
			ls.l_Open = ls.l_Close;
		if (ls.l_High==0L)
			ls.l_High = ls.l_Close;
		if (ls.l_Low==0L)
			ls.l_Low = ls.l_Close;
	}
	ls.Amount    = tmphq.Amount;
	if (testzs(pStkInfo))
	{
		ls.Volume  = tmphq.Volume;
		ls.zd.up   = tmphq.Buyv[0];
		ls.zd.down = tmphq.Sellv[0];
	}
	else
	{
		if (need_justcjl(pStkInfo->setcode,pStkInfo->Code))
			ls.Volume = tmphq.Volume*pStkInfo->Unit+tmphq.Flag.RestVol;
		else
			ls.Volume = tmphq.Volume;
		ls.YClose = 0.0;
	}
	memset(&oldls,0,sizeof(struct AnalyData));
	sprintf(Str,"%s%s\\day\\%s%s.day",g_TdxCfg.DocPath,GetSetStr(pStkInfo->setcode),GetSetStr(pStkInfo->setcode),pStkInfo->Code);
	fp = _fsopen(Str,"r+b",SH_DENYNO);
	if (!fp) 
		fp = _fsopen(Str,"wb",SH_DENYNO);
	if (!fp)
	{
		if(TDX_MessageBox(m_hWnd,"文件打开或创建失败!请确定目录是否可写或是否有防病毒软件干扰!\n是否继续?",MB_ICONEXCLAMATION|MB_OKCANCEL|MB_DEFBUTTON1)==IDOK)
			return TRUE;
		return FALSE;
	}
	if (on_filelength(fp)>=sizeof(struct AnalyData))
	{
		fseek(fp,-(long)sizeof(struct AnalyData),SEEK_END);
		fread(&oldls,sizeof(struct AnalyData),1,fp);
	}
	if (ls.Time.Date==oldls.Time.Date)
		fseek(fp,-(long)sizeof(struct AnalyData),SEEK_END);
	else
		fseek(fp,0L,SEEK_END);
	fwrite(&ls,sizeof(struct AnalyData),1,fp);
	fclose(fp);
	return TRUE;
}
*/

void CDownloadDlg::Down_DayData()	//下载扩展数据
{
	MemStkInfo *pStkInfo;
	if(m_nCurGPNo == 0)
	{
		g_bZnxg_Down = TRUE;
		m_Progress.SetRange(0,AddedGPNum[HQFX_DOWN]);
		m_Progress.SetPos(0);
		//如果是当天的日期段，只需转档即可
		if(m_used_startdate==m_used_enddate && m_used_startdate==g_lOpenRq)
		{
			m_Status_Static.SetText(_F("正在对数据进行优化处理..."));
			g_bSelftPHing=TRUE;
			while(m_nCurGPNo < AddedGPNum[HQFX_DOWN])
			{
				pStkInfo = (*g_pStockIO)[AddedGPIndex[HQFX_DOWN][m_nCurGPNo]];
				if(pStkInfo)
				{
					if(!OneStock_PH(pStkInfo)) break;
					PeekMsg();
					if(!g_bDowning) break; //如果取消,直接退出
				}
				m_nCurGPNo++;
				m_Progress.SetPos(m_nCurGPNo);
			}
			g_bSelftPHing=FALSE;
			g_bAllLocalData = TRUE;
			//记录下载时间
			struct tm t;            //定义tm时间结构，用来存储时间格式的数据信息
			time_t t_of_data;     //定义time_t时间结构
			CTime ct = CTime::GetCurrentTime();
			t.tm_year=ct.GetYear()-1900;//以1900年为标准计算时间
			t.tm_mon=ct.GetMonth();                 //为结构体成员赋值
			t.tm_mday=ct.GetDay();
			t.tm_hour=ct.GetHour();
			t.tm_min=ct.GetMinute();
			t.tm_sec=ct.GetSecond();
			t.tm_isdst=0;
			t_of_data=mktime(&t);		
			char ltm[20]={0};
			sprintf(ltm, "%d", t_of_data);
			WritePrivateProfileString("OTHER","LocalDataTime",ltm,g_WSXHStr+g_strUserCfg);

			m_nCurGPNo		= 0;
			m_used_curdate	= 0;
			m_used_downday	= 0;
			if(m_bExDay && AddedGPNum[HQFX_DOWN] > 0)
				Down_ExDayFile(SZ);
			else if(m_bExDay && AddedGPNum[HQFX_DOWN] > 0)
				Down_ExDayFile(SH);
			else if(m_bMin && AddedGPNum[MIN_DOWN] > 0)
				Down_MinData();
			else if(m_bJbm && AddedGPNum[JBM_DOWN] > 0)
				Down_JbmFileLength();
			else
				DownFinished();
			return;
		}
	}
	CString tmpStr;
//	tmpStr.Format("正在下载日线数据..[%ld-%ld] %ld/%ld",m_used_startdate,m_used_enddate,m_nCurGPNo,AddedGPNum[HQFX_DOWN]);
	tmpStr.Format("正在下载日线数据.. %ld/%ld",m_nCurGPNo,AddedGPNum[HQFX_DOWN]);
	m_Status_Static.SetText(_F(tmpStr));
	g_nMainID = MainIDPlus(m_nMainID,DOWNLOAD_WIN);
	g_nAssisID = SIMFX_ASSISID;
	while(m_nCurGPNo < AddedGPNum[HQFX_DOWN])
	{
		pStkInfo = (*g_pStockIO)[AddedGPIndex[HQFX_DOWN][m_nCurGPNo]];
		if(pStkInfo)
		{
			g_pGetData->GetFxFile(pStkInfo,m_used_startdate,m_used_enddate,PER_DAY);
			break;
		}
		m_nCurGPNo++;
		m_Progress.SetPos(m_nCurGPNo);
	}
}

void CDownloadDlg::Down_ExDayFile(short setcode)
{
	if(m_used_curdate == 0)		//最初开始下载
	{
		//确定起始日期
		m_startday	= g_CF.DateToDay(m_startdate);
		m_endday	= g_CF.DateToDay(m_enddate);
		if(m_startday==0 || m_endday==0 || m_startday>m_endday)
		{
			m_startday	= g_CF.DateToDay(g_lOpenRq);
			m_endday	= g_CF.DateToDay(g_lOpenRq);
		}
		//
		m_used_startday	= m_startday;
		m_used_endday	= m_startday+29;
		if(m_used_endday>m_endday)
			m_used_endday = m_endday;
		m_reqdatenum	= m_used_endday-m_used_startday+1;
		m_used_downday	= 0;
		m_used_curdate	= g_CF.DayToDate(m_used_startday+m_used_downday);
		//
		m_Progress.SetRange(0,m_reqdatenum);
		m_Progress.SetPos(0);
		//
		m_filepos = m_filelen = m_filercvlen = 0;
		//
		int i = 0;
		for(i=0;i<EXFILE_SAMENUM;i++)
			m_datedaylen[i]		= 0;
		for(i=0;i<m_reqdatenum;i++)
			m_used_reqdate[i]	= g_CF.DayToDate(m_used_startday+i);
	}	
	CString tmpStr;
	tmpStr.Format("正在下载%s日线统计数据..[%ld-%ld] %ld/%ld",(setcode==SZ)?"深圳":"上海",
		g_CF.DayToDate(m_used_startday),g_CF.DayToDate(m_used_endday),(m_used_downday+1),m_reqdatenum);
	m_Status_Static.SetText(_F(tmpStr));

	g_nMainID = MainIDPlus(m_nMainID,DOWNLOAD_WIN);
	g_nAssisID = DOWN_EXDAYFILE_ASSISID;
	//
	if(setcode==SZ)
		g_pGetData->GetDateDayFile(m_used_curdate,m_filepos,30000,EXDAYSZ_FILE);
	else 
		g_pGetData->GetDateDayFile(m_used_curdate,m_filepos,30000,EXDAYSH_FILE);
	m_Progress.SetPos(m_used_downday);
}

void CDownloadDlg::Down_FxMinData()	//下载5分钟数据
{
	MemStkInfo *pStkInfo;
	if(m_nCurGPNo == 0)
	{
		m_Progress.SetRange(0,AddedGPNum[HQFX_DOWN]);
		m_Progress.SetPos(0);
	}
	CString tmpStr;
	tmpStr.Format("正在下载5分钟数据..[%ld-%ld] %ld/%ld",m_used_startdateFm,m_used_enddateFm,m_nCurGPNo,AddedGPNum[FXMIN_DOWN]);
	m_Status_Static.SetText(_F(tmpStr));
	g_nMainID = MainIDPlus(m_nMainID,DOWNLOAD_WIN);
	g_nAssisID = DOWN_FXMIN_ASSISID;
	while(m_nCurGPNo < AddedGPNum[FXMIN_DOWN])
	{
		pStkInfo = (*g_pStockIO)[AddedGPIndex[FXMIN_DOWN][m_nCurGPNo]];
		if(pStkInfo)
		{
			g_pGetData->GetFxFile(pStkInfo,m_used_startdateFm,m_used_enddateFm,PER_MIN5);
			break;
		}
		m_nCurGPNo++;
		m_Progress.SetPos(m_nCurGPNo);
	}
}

void CDownloadDlg::Down_JbmFileLength()
{
	if(m_nCurGPNo == 0)
	{
		m_Progress.SetRange(0,AddedGPNum[JBM_DOWN]);
		m_Progress.SetPos(0);
	}
	CString tmpStr;
	tmpStr.Format("正在搜索个股资料.. %d/%d",m_nCurGPNo,AddedGPNum[JBM_DOWN]);
	m_Status_Static.SetText(_F(tmpStr));
	MemStkInfo *pStkInfo;
	
	if(m_bSegmentTxt&&g_bHasF10Cfg&&OnLine)
	{
		g_nMainID = MainIDPlus(m_nMainID,DOWNLOAD_WIN);
		g_nAssisID = DOWN_NEW_JBMCFG_ASSISID;
		
		while(m_nCurGPNo < AddedGPNum[JBM_DOWN])
		{
			pStkInfo = (*g_pStockIO)[AddedGPIndex[JBM_DOWN][m_nCurGPNo]];
			if(pStkInfo)
			{
				g_pGetData->GetNewJbmCfg(pStkInfo->Code,pStkInfo->setcode,m_nWhichJBM);
				break;
			}
			m_nCurGPNo++;
			m_Progress.SetPos(m_nCurGPNo);
		}
		return;
	}

	g_nMainID = MainIDPlus(m_nMainID,DOWNLOAD_WIN);
	g_nAssisID = DOWN_JBMLEN_ASSISID;
	while(m_nCurGPNo < AddedGPNum[JBM_DOWN])
	{
		pStkInfo = (*g_pStockIO)[AddedGPIndex[JBM_DOWN][m_nCurGPNo]];
		if(pStkInfo)
		{
			g_pGetData->GetJbmFileLen(pStkInfo->Code,pStkInfo->setcode);
			break;
		}
		m_nCurGPNo++;
		m_Progress.SetPos(m_nCurGPNo);
	}
}

void CDownloadDlg::Down_JbmData()
{
	if(m_nCurGPNo == 0)
	{
		m_Progress.SetRange(0,AddedGPNum[JBM_DOWN]);
		m_Progress.SetPos(0);
	}
	CString tmpStr;
	tmpStr.Format("正在下载个股资料.. %d/%d",m_nCurGPNo,AddedGPNum[JBM_DOWN]);
	m_Status_Static.SetText(_F(tmpStr));
	MemStkInfo *pStkInfo;

	if(m_bSegmentTxt&&g_bHasF10Cfg&&OnLine)
	{
		g_nMainID = MainIDPlus(m_nMainID,DOWNLOAD_WIN);
		g_nAssisID = DOWN_NEW_JBMTXT_ASSISID;

		if(m_nCurGPNo < AddedGPNum[JBM_DOWN])
		{
			pStkInfo = (*g_pStockIO)[AddedGPIndex[JBM_DOWN][m_nCurGPNo]];
			if(NeedWant_Jbm[m_nCurGPNo] && pStkInfo && m_whichfl<m_ntofl)
			{
				g_pGetData->GetNewJbmText(pStkInfo->Code,pStkInfo->setcode,m_whichfl,g_NewF10Caption[m_whichfl].sFilePath,g_NewF10Caption[m_whichfl].nOffset,g_NewF10Caption[m_whichfl].nLength,m_nWhichJBM);
				return;
			}
			//继续请求下一个
			m_nCurGPNo++;
			m_Progress.SetPos(m_nCurGPNo);
			if(m_nCurGPNo < AddedGPNum[JBM_DOWN])		
			{
				Down_JbmFileLength();
				if(m_nCurGPNo >= AddedGPNum[JBM_DOWN])
					DownFinished();
				return;
			}
			DownFinished();
		}
		return;
	}

	g_nMainID = MainIDPlus(m_nMainID,DOWNLOAD_WIN);
	g_nAssisID = DOWN_JBM_ASSISID;
	while(m_nCurGPNo < AddedGPNum[JBM_DOWN])
	{
		pStkInfo = (*g_pStockIO)[AddedGPIndex[JBM_DOWN][m_nCurGPNo]];
		if(NeedWant_Jbm[m_nCurGPNo] && pStkInfo)
		{
			nCurRemotePos=0;
			g_pGetData->GetJbmText(pStkInfo->Code,pStkInfo->setcode,nCurRemotePos,g_bUseBigJBM);
			break;
		}
		m_nCurGPNo++;
		m_Progress.SetPos(m_nCurGPNo);
	}
}

void CDownloadDlg::Down_MinData()
{
	if(m_nCurGPNo == 0)
	{
		m_Progress.SetRange(0,AddedGPNum[MIN_DOWN]);
		m_Progress.SetPos(0);
	}
	CString tmpStr;
	tmpStr.Format("正在下载分时图数据.. %d/%d",m_nCurGPNo,AddedGPNum[MIN_DOWN]);
	m_Status_Static.SetText(_F(tmpStr));
	MemStkInfo *pStkInfo;
	g_nMainID = MainIDPlus(m_nMainID,DOWNLOAD_WIN);
	g_nAssisID = DOWN_MIN_ASSISID;
	while(m_nCurGPNo < AddedGPNum[MIN_DOWN])
	{
		pStkInfo = (*g_pStockIO)[AddedGPIndex[MIN_DOWN][m_nCurGPNo]];
		if(pStkInfo)
		{
			short nNT = -1;
			g_pGetData->GetMinuteData(pStkInfo,FALSE,nNT);
			break;
		}
		m_nCurGPNo++;
		m_Progress.SetPos(m_nCurGPNo);
	}
}

void CDownloadDlg::DownFinished()
{
	if(bRemoteFileOpen)
	{
		theRemoteFile.Close();
		bRemoteFileOpen=FALSE;
	}
	m_Status_Static.SetText(_F("下载完毕."));
	g_bDowning = FALSE;
	GetDlgItem(IDOK)->SetWindowText(_F("开始下载"));
	EnableOther(TRUE);
	if(m_bTmpDownLast) 
		g_bLastRqDown=TRUE;
	g_DDEDataNeedLoad=TRUE;
	m_Progress.SetPos(0);
	TDXBeep(0);

	SendMessage(WM_COMMAND, IDCANCLE);
}

void CDownloadDlg::OnGetDataAck(WPARAM wParam,LPARAM lParam)
{
	if(m_nMainID!=g_AnsHeader.MainID) return;
	if(!g_bDowning) return;	//如果已经取消，返回
	switch(g_AnsHeader.AssisID)
	{
	case SIMHQ_ASSISID:		//一次请求完毕
		g_pGetData->GetSimHqACK();
		if(m_nCurGPNo < AddedGPNum[HQFX_DOWN])		
		{
			Sleep(5); //加入延时,以免有时发数据太急造成网络卡包
			Down_HqData();
			break;
		}
		m_nCurGPNo		= 0;
		m_used_curdate	= 0;
		m_used_downday	= 0;
		if(m_bDay && AddedGPNum[HQFX_DOWN] > 0)
			Down_DayData();
		else if(m_bExDay && AddedGPNum[HQFX_DOWN] > 0)
			Down_ExDayFile(SZ);
		else if(m_bExDay && AddedGPNum[HQFX_DOWN] > 0)
			Down_ExDayFile(SH);
		else if(m_bFxMin && AddedGPNum[FXMIN_DOWN] > 0)
			Down_FxMinData();
		else if(m_bMin && AddedGPNum[MIN_DOWN] > 0)
			Down_MinData();
		else if(m_bJbm && AddedGPNum[JBM_DOWN] > 0)
			Down_JbmFileLength();
		else
			DownFinished();
		break;
	case SIMFX_ASSISID:		//分组请求
		{
			struct MemStkInfo * scode;
			if(!m_bDownToken)
				scode = (*g_pStockIO)[AddedGPIndex[HQFX_DOWN][m_nCurGPNo]];
			else
				scode = g_pStockIO->Get_MemStkInfo("000001",SH);
			m_bDownToken=FALSE;
			if(!g_pGetData->GetFxFileACK(scode))
			{
				g_bDowning = FALSE;
				GetDlgItem(IDOK)->SetWindowText(_F("开始下载"));
				EnableOther(TRUE);
				m_Status_Static.SetText(_F("下载失败."));
				m_Progress.SetPos(0);
				break;
			}
			m_nCurGPNo++;
			m_Progress.SetPos(m_nCurGPNo);
			if(m_nCurGPNo < AddedGPNum[HQFX_DOWN])
			{
				scode = (*g_pStockIO)[AddedGPIndex[HQFX_DOWN][m_nCurGPNo]];
				//标记点不下载
				if(m_nCurGPNo<(AddedGPNum[HQFX_DOWN]-1) && strcmp(scode->Code,"000001")==0 && scode->setcode==SH) 
				{
					m_nCurGPNo++;
					m_Progress.SetPos(m_nCurGPNo);
					Down_DayData();
					break;
				}
				else
				{
					Down_DayData();
					break;
				}
			}
			else if(m_nCurGPNo==AddedGPNum[HQFX_DOWN] && m_bDownAllGp)
			{
				//最后再下载标记点股票
				g_nMainID = MainIDPlus(m_nMainID,DOWNLOAD_WIN);
				g_nAssisID = SIMFX_ASSISID;
				scode = g_pStockIO->Get_MemStkInfo("000001",SH);
				if(scode)
				{
					m_bDownToken=TRUE;
					g_pGetData->GetFxFile(scode,m_used_startdate,m_used_enddate,PER_DAY);
					break;
				}
			}
			if(m_bDay && AddedGPNum[HQFX_DOWN] > 0)
			{
				if(m_used_enddate < m_enddate) //如果日期段还未到，继续下载
				{
					m_used_startdate = m_used_enddate+1;
					m_used_enddate = m_enddate;
					if(m_used_startdate+40000 < m_enddate)
						m_used_enddate = m_used_startdate+40000;
					m_nCurGPNo = 0;
					Down_DayData();
					break;
				}
			}

			//写盘最新行情
			m_nCurGPNo = 0;
			MemStkInfo *pStkInfo=NULL;
			m_Status_Static.SetText(_F("正在对数据进行优化处理..."));
			g_bSelftPHing=TRUE;
			while(m_nCurGPNo < AddedGPNum[HQFX_DOWN])
			{
				pStkInfo = (*g_pStockIO)[AddedGPIndex[HQFX_DOWN][m_nCurGPNo]];
				if(pStkInfo)
				{
					if(!OneStock_PH(pStkInfo)) break;
					PeekMsg();
					if(!g_bDowning) break; //如果取消,直接退出
				}
				m_nCurGPNo++;
				m_Progress.SetPos(m_nCurGPNo);
			}
			g_bSelftPHing=FALSE;
			g_bAllLocalData = TRUE;
			//记录下载时间
			struct tm t;            //定义tm时间结构，用来存储时间格式的数据信息
			time_t t_of_data;     //定义time_t时间结构
			CTime ct = CTime::GetCurrentTime();
			t.tm_year=ct.GetYear()-1900;//以1900年为标准计算时间
			t.tm_mon=ct.GetMonth();                 //为结构体成员赋值
			t.tm_mday=ct.GetDay();
			t.tm_hour=ct.GetHour();
			t.tm_min=ct.GetMinute();
			t.tm_sec=ct.GetSecond();
			t.tm_isdst=0;
			t_of_data=mktime(&t);		
			char ltm[20]={0};
			sprintf(ltm, "%d", t_of_data);
			WritePrivateProfileString("OTHER","LocalDataTime",ltm,g_WSXHStr+g_strUserCfg);

			m_nCurGPNo		= 0;
			m_used_curdate	= 0;
			m_used_downday	= 0;
			if(m_bExDay && AddedGPNum[HQFX_DOWN] > 0)
				Down_ExDayFile(SZ);
			else if(m_bExDay && AddedGPNum[HQFX_DOWN] > 0)
				Down_ExDayFile(SH);
			else if(m_bFxMin && AddedGPNum[FXMIN_DOWN] > 0)
				Down_FxMinData();
			else if(m_bMin && AddedGPNum[MIN_DOWN] > 0)		
				Down_MinData();
			else if(m_bJbm && AddedGPNum[JBM_DOWN] > 0)
				Down_JbmFileLength();
			else
				DownFinished();		
		}
		break;
	case DOWN_EXDAYFILE_ASSISID:
		{
			BOOL bRet = TRUE;
			int i=0,j=0;
			long tmpLong = 0;
			CHECKEXDAYSPACE(bRet,m_exfilercvbuf,m_exfilercvspc,EXFILE_DOWNBUFLEN,char);
			if(!bRet || !g_pGetData->GetDateDayFileACK(m_exdaysetcode,m_filelen,m_filercvlen,m_exfilercvbuf,EXFILE_DOWNBUFLEN))
			{
				g_bDowning = FALSE;
				GetDlgItem(IDOK)->SetWindowText(_F("开始下载"));
				EnableOther(TRUE);
				m_Status_Static.SetText(_F("下载失败."));
				m_Progress.SetPos(0);
				break;
			}
			//检查:1.单个文件是否下载完成;2.单批文件是否下载完成;3.整个时间段是否完成
			if(m_filepos==0 || m_datedaylen[m_used_downday]==0)		//检查文件缓存空间
			{
				m_datedaylen[m_used_downday] = m_filelen;
				if(m_datedaylen[m_used_downday]>0)
				{
					CHECKEXDAYSPACE(bRet,m_fpdateday[m_used_downday],m_datedayspc[m_used_downday],m_datedaylen[m_used_downday],char);
				}
			}
			//文件前后长度不一致,该文件重传
			if(m_datedaylen[m_used_downday] != m_filelen)
			{
				m_filepos	= 0;
				m_datedaylen[m_used_downday]	= 0;
				Down_ExDayFile(m_exdaysetcode);
				break;
			}
			//文件可能传完
			if(m_filercvlen<30000)
			{
				//文件长度有问题,重传
				if(m_filercvlen+m_filepos != m_filelen)
				{
					m_filepos	= 0;
					m_datedaylen[m_used_downday]	= 0;
					Down_ExDayFile(m_exdaysetcode);
					break;
				}
				//存入缓存
				memcpy(&(m_fpdateday[m_used_downday][m_filepos]),m_exfilercvbuf,m_filercvlen);
				m_used_downday++;
				//
				m_filepos	= m_filelen	= m_filercvlen = 0;
				//该组文件已经传完,考虑下个时间范围
				if(m_used_downday>=m_reqdatenum)
				{
					//这里转档存盘
					CString tmpMsgStr;
					tmpMsgStr.Format("正在转档%s日线统计数据..[%ld-%ld]",(m_exdaysetcode==SZ)?"深圳":"上海",
						g_CF.DayToDate(m_used_startday),g_CF.DayToDate(m_used_endday));
					m_Status_Static.SetText(_F(tmpMsgStr));
					if(g_pExdayMng)
						g_pExdayMng->WriteExdayFile(m_exdaysetcode,m_fpdateday,m_datedaylen,m_used_reqdate,m_reqdatenum);
					//时间没有到底,安排下个时间段
					if(m_used_endday<m_endday)
					{
						m_used_startday	= m_used_endday+1;
						m_used_endday	= m_used_startday+29;
						if(m_used_endday>m_endday)
							m_used_endday	= m_endday;
						m_reqdatenum	= m_used_endday-m_used_startday+1;
						m_used_downday	= 0;
						m_used_curdate	= g_CF.DayToDate(m_used_startday+m_used_downday);
						//
						m_Progress.SetRange(0,m_reqdatenum);
						m_Progress.SetPos(m_used_downday);
						//
						for(i=0;i<EXFILE_SAMENUM;i++)
							m_datedaylen[i]		= 0;
						for(i=0;i<m_reqdatenum;i++)
							m_used_reqdate[i]	= g_CF.DayToDate(m_used_startday+i);
						Down_ExDayFile(m_exdaysetcode);
						break;
					}
				}
				else
				{
					m_used_curdate	= g_CF.DayToDate(m_used_startday+m_used_downday);
					m_Progress.SetPos(m_used_downday);
					Down_ExDayFile(m_exdaysetcode);
					break;
				}
			}
			else
			{
				memcpy(&(m_fpdateday[m_used_downday][m_filepos]),m_exfilercvbuf,m_filercvlen);
				m_filepos += m_filercvlen;
				Down_ExDayFile(m_exdaysetcode);
				break;
			}

			m_nCurGPNo		= 0;
			m_used_curdate	= 0;
			m_used_downday	= 0;
			if(m_exdaysetcode==SZ)
				Down_ExDayFile(SH);
			else if(m_bFxMin && AddedGPNum[FXMIN_DOWN] > 0)
				Down_FxMinData();
			else if(m_bMin && AddedGPNum[MIN_DOWN] > 0)		
				Down_MinData();
			else if(m_bJbm && AddedGPNum[JBM_DOWN] > 0)
				Down_JbmFileLength();
			else
				DownFinished();		
		}
		break;
	case DOWN_FXMIN_ASSISID:
		{
			struct MemStkInfo * scode;
			if(!m_bDownToken)
				scode = (*g_pStockIO)[AddedGPIndex[FXMIN_DOWN][m_nCurGPNo]];
			else
				scode = g_pStockIO->Get_MemStkInfo("000001",SH);
			m_bDownToken=FALSE;
			if(!g_pGetData->GetFxFileACK(scode))
			{
				g_bDowning = FALSE;
				GetDlgItem(IDOK)->SetWindowText(_F("开始下载"));
				EnableOther(TRUE);
				m_Status_Static.SetText(_F("下载失败."));
				m_Progress.SetPos(0);
				break;
			}
			m_nCurGPNo++;
			m_Progress.SetPos(m_nCurGPNo);
			if(m_nCurGPNo < AddedGPNum[FXMIN_DOWN])
			{
				scode = (*g_pStockIO)[AddedGPIndex[FXMIN_DOWN][m_nCurGPNo]];
				//标记点不下载
				if(m_nCurGPNo<(AddedGPNum[FXMIN_DOWN]-1) && strcmp(scode->Code,"000001")==0 && scode->setcode==SH) 
				{
					m_nCurGPNo++;
					m_Progress.SetPos(m_nCurGPNo);
					Down_FxMinData();
					break;
				}
				else
				{
					Down_FxMinData();
					break;
				}
			}
			else if(m_nCurGPNo==AddedGPNum[FXMIN_DOWN] && m_bDownAllGp)
			{
				//最后再下载标记点股票
				g_nMainID = MainIDPlus(m_nMainID,DOWNLOAD_WIN);
				g_nAssisID = DOWN_FXMIN_ASSISID;
				scode = g_pStockIO->Get_MemStkInfo("000001",SH);
				if(scode)
				{
					m_bDownToken=TRUE;
					g_pGetData->GetFxFile(scode,m_used_startdateFm,m_used_enddateFm,PER_MIN5);
					break;
				}
			}
			if(m_bFxMin && AddedGPNum[FXMIN_DOWN] > 0)
			{
				if(m_used_enddateFm < m_enddateFm) //如果日期段还未到，继续下载
				{
					m_used_startdateFm = m_used_enddateFm+1;
					m_used_enddateFm = m_enddateFm;
					m_nCurGPNo = 0;
					Down_FxMinData();
					break;
				}
			}
			m_nCurGPNo		= 0;
			m_used_curdate	= 0;
			m_used_downday	= 0;
			if(m_bMin && AddedGPNum[MIN_DOWN] > 0)		
				Down_MinData();
			else if(m_bJbm && AddedGPNum[JBM_DOWN] > 0)
				Down_JbmFileLength();
			else
				DownFinished();		
		}
		break;
	case DOWN_MIN_ASSISID:	//一次一只股票
		{
			struct MemStkInfo * scode;
			scode = (*g_pStockIO)[AddedGPIndex[MIN_DOWN][m_nCurGPNo]];
			struct MinuteData tmpMinData[500];
			memset(tmpMinData,0,500*sizeof(MinuteData));
			g_pGetData->GetMinuteDataACK(scode,tmpMinData,NULL);
			m_nCurGPNo++;
			m_Progress.SetPos(m_nCurGPNo);
			if(m_nCurGPNo < AddedGPNum[MIN_DOWN])		
			{
				Down_MinData();
				break;
			}
			m_nCurGPNo		= 0;
			m_used_curdate	= 0;
			m_used_downday	= 0;
			if(m_bJbm && AddedGPNum[JBM_DOWN] > 0)	
				Down_JbmFileLength();
			else
				DownFinished();
		}
		break;
	case DOWN_JBMLEN_ASSISID:
		{
			struct MemStkInfo * scode;
			scode = (*g_pStockIO)[AddedGPIndex[JBM_DOWN][m_nCurGPNo]];
			long tmpLen = g_pGetData->GetFileLenACK();
			if(tmpLen > 0 && tmpLen < 10*1024*1024)
			{
				CString tempFileName,tempStr;
				if(scode->setcode == SZ) 
					tempStr = "RemoteSZ\\";
				else 
					tempStr = "RemoteSH\\";
				tempFileName.Format("%s%s.Txt",tempStr,scode->Code);
				CFile theFile;
				if(theFile.Open(tempFileName,CFile::modeRead|CFile::shareDenyNone))
				{
					DWORD len = theFile.GetLength(); //得到本地消息文件的长度
					if(len == tmpLen) 
						NeedWant_Jbm[m_nCurGPNo] = 0;
					theFile.Close();
				}
			}
			else
				NeedWant_Jbm[m_nCurGPNo] = 0;
			//继续下载
			m_nCurGPNo++;
			m_Progress.SetPos(m_nCurGPNo);
			if(m_nCurGPNo < AddedGPNum[JBM_DOWN])		
			{
				Down_JbmFileLength();
				break;
			}
			m_nCurGPNo		= 0;
			m_used_curdate	= 0;
			m_used_downday	= 0;
			Down_JbmData();
			if(m_nCurGPNo >= AddedGPNum[JBM_DOWN])
				DownFinished();
		}
		break;
	case DOWN_JBM_ASSISID:
		{
			struct MemStkInfo * scode;
			scode = (*g_pStockIO)[AddedGPIndex[JBM_DOWN][m_nCurGPNo]];
			if(!scode) 
			{
				DownFinished();
				break;
			}
			char	*TmpFileBuffer = new char[10240+20];
			int Buflen = g_pGetData->GetTextACK(TmpFileBuffer);
			if(nCurRemotePos==0)
			{
				if(bRemoteFileOpen)
				{
					theRemoteFile.Close();
					bRemoteFileOpen=FALSE;
				}
				CString tempFileName,tempStr;
				if(scode->setcode == SZ) tempStr = "RemoteSZ\\";
				else tempStr = "RemoteSH\\";
				tempFileName.Format("%s%s.Txt", tempStr,scode->Code);
				if( theRemoteFile.Open(tempFileName,CFile::modeCreate|CFile::modeReadWrite|CFile::shareDenyNone) )
				{
					theRemoteFile.Write(TmpFileBuffer,max(0,Buflen));
					bRemoteFileOpen=TRUE;
				}
			}
			else if(bRemoteFileOpen)
				theRemoteFile.Write(TmpFileBuffer,max(0,Buflen));
			delete TmpFileBuffer;
			nCurRemotePos += Buflen;
			if(Buflen == (g_bUseBigJBM?10240:2048)) //继续下载本个股资料
			{
				g_nMainID = MainIDPlus(m_nMainID,DOWNLOAD_WIN);
				g_nAssisID = DOWN_JBM_ASSISID;
				g_pGetData->GetJbmText(scode->Code,scode->setcode,nCurRemotePos,g_bUseBigJBM);
				break;
			}
			//继续下载
			m_nCurGPNo++;
			m_Progress.SetPos(m_nCurGPNo);
			if(m_nCurGPNo < AddedGPNum[JBM_DOWN])		
			{
				Down_JbmData();
				if(m_nCurGPNo >= AddedGPNum[JBM_DOWN])
					DownFinished();
				break;
			}
			DownFinished();
		}
		break;
	case DOWN_NEW_JBMCFG_ASSISID:
		{
			m_ntofl = g_pGetData->GetNewJbmCfgACK();
			if(m_ntofl < 0)  m_ntofl = 0;
			m_whichfl = 0;
			Down_JbmData();
		}
		break;
	case DOWN_NEW_JBMTXT_ASSISID:
		{	
			struct MemStkInfo * scode;
			scode = (*g_pStockIO)[AddedGPIndex[JBM_DOWN][m_nCurGPNo]];
			if(!scode) 
			{
				DownFinished();
				break;
			}
			int len = g_pGetData->GetNewJbmTextACK(FileBuffer);
			if(len>0)
			{
				for(int k = 0;k < len;k++) //将'\0'变为空格
					if(FileBuffer[k] == 0) FileBuffer[k] = 0x20;
				FileBuffer[len] = '\n';
				FileBuffer[++len] = '\n';
				FileBuffer[++len] = 0;
			}
			if(m_whichfl==0)
			{
				if(bRemoteFileOpen)
				{
					theRemoteFile.Close();
					bRemoteFileOpen=FALSE;
				}
				CString tempFileName,tempStr;
				if(scode->setcode == SZ) tempStr = "RemoteSZ\\";
				else tempStr = "RemoteSH\\";
				tempFileName.Format("%s%s.Txt", tempStr,scode->Code);
				if( theRemoteFile.Open(tempFileName,CFile::modeCreate|CFile::modeReadWrite|CFile::shareDenyNone) )
				{
					theRemoteFile.Write(FileBuffer,max(0,len));
					bRemoteFileOpen=TRUE;
				}
			}
			else if(bRemoteFileOpen)
				theRemoteFile.Write(FileBuffer,max(0,len));
			if(++m_whichfl < m_ntofl) //继续下载本个股资料
			{
				Down_JbmData();
				break;
			}
			//继续下载
			m_nCurGPNo++;
			m_Progress.SetPos(m_nCurGPNo);
			if(m_nCurGPNo < AddedGPNum[JBM_DOWN])		
			{
				Down_JbmFileLength();
				if(m_nCurGPNo >= AddedGPNum[JBM_DOWN])
					DownFinished();
				break;
			}
			DownFinished();
		}
		break;
	default:
		break;
	}
}

//关闭此对话框
void CDownloadDlg::OnCancel() 
{
	if(g_bDowning && OnLine) return;
	if(m_bModal)
		CDialog::OnCancel();
	else
		GetParent() ->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);
}

void CDownloadDlg::OnAdd() 
{
	CSelectGPDlg dlg;
	dlg.bHasDS = FALSE;
	if( dlg.DoModal() == IDOK )
	{
		int i,j;
		BOOL bFind;
		for(i = 0;i < dlg.SelectedGPNum;i++)
		{
			if(dlg.SelectedGPIndex[i]>=DS_FROM) continue; //暂时屏蔽三方行情
			bFind = FALSE;
			for(j = 0;j < AddedGPNum[m_nTabSel];j++)
			{
				if(AddedGPIndex[m_nTabSel][j] == dlg.SelectedGPIndex[i])
				{
					bFind = TRUE; 
					break;
				}
			}
			if(!bFind)  AddedGPIndex[m_nTabSel][AddedGPNum[m_nTabSel]++] = dlg.SelectedGPIndex[i];
		}
		FillGPReport();
	}
}

void CDownloadDlg::OnDelete() 
{
	POSITION pos = m_GpReport_List.GetFirstSelectedItemPosition();
	if( pos != NULL )
	{
		int nItem,i,j;
		while (pos) //先置删除标记-1
		{
			nItem = m_GpReport_List.GetNextSelectedItem(pos);
			AddedGPIndex[m_nTabSel][nItem] = -1;
		}
		for(i = AddedGPNum[m_nTabSel] - 1;i >= 0;i--)
		{
			if(AddedGPIndex[m_nTabSel][i] == -1)
			{
				for(j = i+1;j < AddedGPNum[m_nTabSel];j++)
					AddedGPIndex[m_nTabSel][j-1] = AddedGPIndex[m_nTabSel][j];
				AddedGPNum[m_nTabSel]--;
			}
		}
		FillGPReport();
	}	
}

void CDownloadDlg::AfterTabChange()
{
//	GetDlgItem(IDC_DATETIMEPICKER1)->ShowWindow(m_nTabSel<=1);
//	GetDlgItem(IDC_DATETIMEPICKER2)->ShowWindow(m_nTabSel<=1);
//	GetDlgItem(IDC_DOWNALL_CHECK)->ShowWindow(m_nTabSel<=1);
//	GetDlgItem(IDC_SEP_STATIC)->ShowWindow(m_nTabSel<=1);
	//
	if(m_nTabSel==0)
	{	
		if(m_startdate>0&&m_enddate>0)
		{
			CTime tmpTime1(m_startdate/10000,(m_startdate%10000)/100,m_startdate%100,0,0,0);
			CTime tmpTime2(m_enddate/10000,(m_enddate%10000)/100,m_enddate%100,0,0,0);
			m_Time1 = tmpTime1;
			m_Time2 = tmpTime2;
			UpdateData(FALSE);
		}

	//	GetDlgItem(IDC_DATETIMEPICKER1)->EnableWindow(m_bDay||m_bExDay);	
	//	GetDlgItem(IDC_DATETIMEPICKER2)->EnableWindow(m_bDay||m_bExDay);	
	//	GetDlgItem(IDC_DOWNALL_CHECK)->EnableWindow(m_bDay||m_bExDay);
		
	//	GetDlgItem(IDC_ADD)->EnableWindow(!m_bDownAllGp);
	//	GetDlgItem(IDC_DELETE)->EnableWindow(!m_bDownAllGp);
	}
	else if(m_nTabSel==1)					//分钟数据下载
	{
		if(m_startdateFm<1||m_enddateFm<1)
		{
			BOOL bTmpGen;
			long lLastDate = GetFxMinVipDocLastDate(bTmpGen);
			if(lLastDate > g_lOpenRq)
				lLastDate = g_lOpenRq;
			m_startdateFm = lLastDate;
			m_enddateFm = g_lOpenRq;
		}
		if(m_startdateFm>0&&m_enddateFm>0)
		{
			CTime tmpTime1(m_startdateFm/10000,(m_startdateFm%10000)/100,m_startdateFm%100,0,0,0);
			CTime tmpTime2(m_enddateFm/10000,(m_enddateFm%10000)/100,m_enddateFm%100,0,0,0);
			m_Time1 = tmpTime1;
			m_Time2 = tmpTime2;	
			UpdateData(FALSE);
		}

	//	GetDlgItem(IDC_DATETIMEPICKER1)->EnableWindow(m_bFxMin);	
	//	GetDlgItem(IDC_DATETIMEPICKER2)->EnableWindow(m_bFxMin);	
	//	GetDlgItem(IDC_DOWNALL_CHECK)->EnableWindow(m_bFxMin);

	//	GetDlgItem(IDC_ADD)->EnableWindow(!m_bDownAllGp);
	//	GetDlgItem(IDC_DELETE)->EnableWindow(!m_bDownAllGp);
	}
	else
	{
	//	GetDlgItem(IDC_ADD)->EnableWindow(TRUE);
	//	GetDlgItem(IDC_DELETE)->EnableWindow(TRUE);
	}
	FillGPReport();
}


void CDownloadDlg::OnDownDayCheck() 
{
	UpdateData(TRUE);
//	GetDlgItem(IDC_DATETIMEPICKER1)->EnableWindow(m_bDay||m_bExDay);	
//	GetDlgItem(IDC_DATETIMEPICKER2)->EnableWindow(m_bDay||m_bExDay);	
//	GetDlgItem(IDC_DOWNALL_CHECK)->EnableWindow(m_bDay||m_bExDay);
}

void CDownloadDlg::OnDownFxMinCheck()
{
	UpdateData(TRUE);
	if(!g_bSupportDownMin5 && m_bFxMin)
	{
		m_bFxMin=FALSE;
		UpdateData(FALSE);
		TDX_MessageBox(m_hWnd,"当前行情主站不允许下载5分钟数据!",MB_OK|MB_ICONEXCLAMATION);
	}
//	GetDlgItem(IDC_DATETIMEPICKER1)->EnableWindow(m_bFxMin);	
//	GetDlgItem(IDC_DATETIMEPICKER2)->EnableWindow(m_bFxMin);	
//	GetDlgItem(IDC_DOWNALL_CHECK)->EnableWindow(m_bFxMin);
}

void CDownloadDlg::OnDownallCheck() 
{
	UpdateData(TRUE);
	if(!m_bDownAllGp)
	{
		if(TDX_MessageBox(m_hWnd,"下载部分股票的数据,可能会造成全市场选股和排行的片面性,\r\n您确定吗?",MB_ICONQUESTION|MB_OKCANCEL)==IDCANCEL)
		{
			m_bDownAllGp=TRUE;
			UpdateData(FALSE);
			return;
		}
	}
	GetDlgItem(IDC_ADD)->EnableWindow(!m_bDownAllGp);
	GetDlgItem(IDC_DELETE)->EnableWindow(!m_bDownAllGp);

	if(m_bDownAllGp)
	{
		MemStkInfo *tmpInfo;
		int			tmpInt;
		AddedGPNum[0]=0;
		for(int i=0;i<g_pStockIO->GetStockNum(ALL_ALL);i++)
		{
			tmpInfo=(*g_pStockIO)[i];
			if(!tmpInfo) continue;
			tmpInt = GetStockType(tmpInfo->setcode,tmpInfo->Code);
			if(g_GH.bOnlyDownAGData && tmpInt!=CODE_SHAG && tmpInt!=CODE_SZAG && tmpInt!=CODE_ZXB && tmpInt!=CODE_CYB && tmpInt!=CODE_SZJJ && tmpInt!=CODE_SHJJ && !testzs(tmpInfo))
				continue;
			AddedGPIndex[0][AddedGPNum[0]] = i;
			AddedGPNum[0]++;
			AddedGPIndex[1][AddedGPNum[1]] = i;
			AddedGPNum[1]++;
		}
	}
	else
	{
		AddedGPNum[0] = 0;
		AddedGPNum[1] = 0;
	}
	FillGPReport();
}

BOOL CDownloadDlg::PreTranslateMessage(MSG* pMsg) 
{
	static char AboutInvoker2[6] = {'t','d','x','t','j','e'};
	static int pos = 0;
	if(pMsg->message == WM_CHAR)
	{
		if(pMsg->wParam  == AboutInvoker2[pos])
		{
			if(++pos == 6)
			{
				pos = 0;
				SetWindowText("Thanks to Use DXRJ Software! All Rights Reversed!");
			}
		}
		else pos = 0;
	}	
	return CDialog::PreTranslateMessage(pMsg);
}
