// ShowInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "ShowInfoDlg.h"
#include "Keyguy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShowInfoDlg dialog


CShowInfoDlg::CShowInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShowInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShowInfoDlg)
	//}}AFX_DATA_INIT
}


void CShowInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShowInfoDlg)
	DDX_Control(pDX, IDC_LIST, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShowInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CShowInfoDlg)
	ON_BN_CLICKED(IDC_SEEURGENT, OnSeeurgent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

long GetVipDocLastDateFromCode(short setcode,char *code,long &firstdate)
{
	long lRet=0;
	firstdate=0;
	CFile FxFile;
	CString tmpFileName;
	tmpFileName.Format("%s%s\\day\\%s%s.day",g_TdxCfg.DocPath,GetSetStr(setcode),GetSetStr(setcode),code);
	g_HisDataReader.Lock();
	if(FxFile.Open(tmpFileName,CFile::modeRead))
	{
		if(FxFile.GetLength()>=sizeof(ANALYDATA))
		{
			ANALYDATA tmpAnalyData;
		
			FxFile.Read(&tmpAnalyData,sizeof(ANALYDATA));
			firstdate = tmpAnalyData.Time.Date;
			
			FxFile.Seek(-sizeof(ANALYDATA),SEEK_END);		
			FxFile.Read(&tmpAnalyData,sizeof(ANALYDATA));
			lRet = tmpAnalyData.Time.Date;
		}
		FxFile.Close();
	}
	g_HisDataReader.Unlock();
	return lRet;
}

long GetLocalVipDocDateNum()
{
	long lTmpEndDate,lTmpStartDate;
	lTmpEndDate=GetVipDocLastDateFromCode(SH,"000001",lTmpStartDate);
	CTime endTime=LongToTime(lTmpEndDate);
	CTime startTime=LongToTime(lTmpStartDate);
	CTimeSpan tp=endTime-startTime;
	return tp.GetDays();
}

BOOL CShowInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CString tmpStr,tmpContent;
	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_INFO);
	m_ImageList.Add(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDI_INFO2);
	m_ImageList.Add(hIcon);
	m_List.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_List.SetExtendedStyle (LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	//插入列
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.pszText = "栏目";
	lvColumn.cx = 120;
	m_List.InsertColumn (0,&lvColumn);
	lvColumn.pszText = "状态";
	lvColumn.cx = 250;
	m_List.InsertColumn (1,&lvColumn);
	//
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	lvItem.iImage = 0;
	int nPos=0;
	
	//---------------------------------------------------------
	//程序名
	lvItem.iItem = nPos;
	lvItem.iImage = 0;
	lvItem.pszText = _F("程序名");
	m_List.InsertItem(&lvItem);
	m_List.SetItemText(nPos,1,_F(g_strTitleName));
	nPos++;
	//程序启动时间
	tmpStr.Format("%08d %s",g_SysInfo.Program_StartYmd,GetTimeStrFromLong(g_SysInfo.Program_StartHms));
	lvItem.iItem = nPos;
	lvItem.iImage = 0;
	lvItem.pszText = _F("程序启动时间");
	m_List.InsertItem(&lvItem);
	m_List.SetItemText(nPos,1,_F(tmpStr));
	nPos++;

	//绿色通道
	if(g_ProxyInfo.bTdxProxy)
	{
		lvItem.iItem = nPos;
		lvItem.iImage = 1;
		lvItem.pszText = _F("绿色通道");
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F("代理服务已开启"));
		nPos++;	
	}
	//行情状态
	if(OnLine)
	{
		lvItem.iItem = nPos;
		lvItem.iImage = 1;
		lvItem.pszText = _F("行情主站");
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F(g_HQNameStr));
		nPos++;
	}
	if(OnLine)
		tmpStr.Format("连接%s[%d]",g_HQIPStr,g_HQPort);
	else
		tmpStr.Format("没有连接主站");
	lvItem.iItem = nPos;
	lvItem.iImage = 1;
	lvItem.pszText = _F("行情地址");
	m_List.InsertItem(&lvItem);
	m_List.SetItemText(nPos,1,_F(tmpStr));
	nPos++;
	//行情连接时间
	if(OnLine)
	{
		tmpStr.Format("%08d %s",g_SysInfo.ConnectHQ_LastYmd,GetTimeStrFromLong(g_SysInfo.ConnectHQ_LastHms));
		lvItem.iItem = nPos;
		lvItem.iImage = 1;
		lvItem.pszText = _F("行情连接时间");
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F(tmpStr));
		nPos++;
	}
	//第三方市场
	if(g_bHasDS)
	{
		if(DSOnLine)
			tmpStr.Format("连接%s[%d]",g_DSHQIPStr,g_DSHQPort);
		else
			tmpStr.Format("没有连接主站");
		lvItem.iItem = nPos;
		lvItem.iImage = 0;
		char tmpHintInfo[MAX_PATH];
		sprintf(tmpHintInfo,"%s主站",g_DS.DSOEMName);
		lvItem.pszText = _F(tmpHintInfo);
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F(tmpStr));
		nPos++;
		//三方行情连接时间
		if(DSOnLine)
		{
			tmpStr.Format("%08d,%s",g_SysInfo.ConnectDSHQ_LastYmd,GetTimeStrFromLong(g_SysInfo.ConnectDSHQ_LastHms));
			lvItem.iItem = nPos;
			lvItem.iImage = 0;
			sprintf(tmpHintInfo,"%s连接时间",g_DS.DSOEMName);
			lvItem.pszText = _F(tmpHintInfo);
			m_List.InsertItem(&lvItem);
			m_List.SetItemText(nPos,1,_F(tmpStr));
			nPos++;
		}
	}	
	//交易系统
	if(g_OEM.bEmbedWTVersion && TC_GetJyStatus_)
	{
		char hostip[255]={0};
		UINT port = 0;
		DWORD dwStatus=TC_GetJyStatus_(hostip,&port,NULL);
		if(!(dwStatus&JY_LOGINED))
			tmpStr.Format("交易未登录");
		else if(dwStatus&JY_LOCKING)
			tmpStr.Format("已锁定 连接%s[%d]",hostip,port);
		else if(dwStatus&JY_LOGINED)
			tmpStr.Format("已登录 连接%s[%d]",hostip,port);
		lvItem.iItem = nPos;
		lvItem.iImage = 1;
		lvItem.pszText = _F("交易系统");
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F(tmpStr));
		nPos++;	
	}
	//级别等信息
	if(g_OEM.bEmbedWTVersion && g_MngRight>=0 && g_MngRight<=9)
	{
		tmpStr.Format("[%d]级 %s",g_MngRight,g_RightStr);
		lvItem.iItem = nPos;
		lvItem.iImage = 0;
		lvItem.pszText = _F("用户等级");
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F(tmpStr));
		nPos++;	
		if(g_OEM.bFormatAndUseYYBID)
		{
			if(g_JyInfo.yybid==0)
				tmpStr.Format("[未记录]");
			else
				tmpStr.Format("[%04d]",g_JyInfo.yybid);
			lvItem.iItem = nPos;
			lvItem.iImage = 0;
			lvItem.pszText = _F("营业部ID");
			m_List.InsertItem(&lvItem);
			m_List.SetItemText(nPos,1,_F(tmpStr));
			nPos++;	
		}
	}
	//到期时间
	if(g_bUserHostEverConnect && g_ExpireDate>0)
	{
		tmpStr.Format("%04d年%02d月%02d日",g_ExpireDate/10000,(g_ExpireDate%10000)/100,g_ExpireDate%100);
		lvItem.iItem = nPos;
		lvItem.iImage = 0;
		lvItem.pszText = _F("使用到期");
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F(tmpStr));
		nPos++;	
	}
	//用户类型串
	if(g_bUserHostEverConnect && strlen(g_strUserType)>0)
	{
		lvItem.iItem = nPos;
		lvItem.iImage = 0;
		lvItem.pszText = _F("用户类型");
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F(g_strUserType));
		nPos++;	
	}
	//用户性质串
	if(g_bUserHostEverConnect && strlen(g_strUserCharacter)>0)
	{
		lvItem.iItem = nPos;
		lvItem.iImage = 0;
		lvItem.pszText = _F("用户性质");
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F(g_strUserCharacter));
		nPos++;	
	}
	//用户权限串
	if(g_bUserHostEverConnect && strlen(g_strUserLimits)>0)
	{
		lvItem.iItem = nPos;
		lvItem.iImage = 0;
		lvItem.pszText = _F("用户权限");
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F(g_strUserLimits));
		nPos++;	
	}
	if(g_nNetCondition==0)
	{
		lvItem.iItem = nPos;
		lvItem.iImage = 0;
		lvItem.pszText = _F("上网环境设置");
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F("低速上网流量网络,有些数据可能维护不全面"));
		nPos++;	
	}
	if(g_nTQFlag==2 || (g_nTQFlag==3 && g_nPartTq==1))
	{
		lvItem.iItem = nPos;
		lvItem.iImage = 0;
		lvItem.pszText = _F("当前复权处理");
		m_List.InsertItem(&lvItem);
		m_List.SetItemText(nPos,1,_F("设置成后复权,最新分析数据不动态维护"));
		nPos++;	
	}
	//本地数据
	long	lTmpStartDate,lTmpEndDate;
	lTmpEndDate=GetVipDocLastDateFromCode(SH,"000001",lTmpStartDate);
	if(lTmpStartDate==0 && lTmpEndDate==0)
		tmpStr.Format("无本地日线");
	else
		tmpStr.Format("%ld-%ld",lTmpStartDate,lTmpEndDate);
	lvItem.iItem = nPos;
	lvItem.iImage = 1;
	lvItem.pszText = _F("本地完整日线数据");
	m_List.InsertItem(&lvItem);
	m_List.SetItemText(nPos,1,_F(tmpStr));
	nPos++;	

	return TRUE;
}

void CShowInfoDlg::OnSeeurgent() 
{
	AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_URGENT);
	CDialog::OnCancel();
}
