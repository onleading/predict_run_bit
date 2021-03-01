#include "stdafx.h"
#include "tdxw.h"
#include "Jbfxview.h"
#include "CWTableUnit.h"
#include "VList.h"
#include "CWTableUCWnd.h"
#include "CWCalc.h"

extern map<long, CString, greater<long> >	* g_pMapTabList;
extern int									* g_pMainItemNum;

CCWTableUnit::CCWTableUnit(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_pList= NULL;
	m_nClue = -1;
	m_pTUCtrlWnd = NULL;
	m_nShowStyle = 0;

	memset(&m_Cfg,0,sizeof(struct Table_GN_Cfg));
	memset(m_strGnID, 0, 21);
	m_bFirstEntry = TRUE;

	m_BlockCode = "";
	memset(&m_GpSet, 0, sizeof(GP_Set));
	memset(&m_CwParam, 0, sizeof(TableBGQ));

	CString strGnPath = HomePath+"cwcfg\\";
	memset(g_LayerPath, 0, sizeof(g_LayerPath));
	memset(g_ContentPath, 0, sizeof(g_ContentPath));
	wsprintf(g_LayerPath,"%s\\hyfl.ini",strGnPath);
	wsprintf(g_ContentPath,"%s\\hymx.ini",strGnPath);

	pSwitchBar = NULL;
	m_SwitchWidth = 0;
	m_SwitchHeight = SWITCH_HEIGHT;
	m_nTabListCmdID = m_nTabListCmdID = m_nTabListBlockCmdID = -1;

	m_pTitleDlg = NULL;
}

CCWTableUnit::~CCWTableUnit()
{
	if(m_pList) 
		DeleteTList(&m_pList);
	TDEL(pSwitchBar)

	if(m_pTitleDlg)
	{
		m_pTitleDlg->DestroyWindow();
		TDEL(m_pTitleDlg)
	}

	m_MapTabList.clear();

	if(m_nClue>=0)
	{
		TD_DeleteClue_(m_nClue);
		m_nClue = -1;
	}
	if(m_pTUCtrlWnd)
	{
		m_pTUCtrlWnd->DestroyWindow();
		delete m_pTUCtrlWnd;
	}
}

BOOL	CCWTableUnit::InitTable(LPCSTR strGnID, LPCSTR strGnIni)
{
	if(!m_pView) return FALSE;

	if(!m_pTUCtrlWnd||!m_pTUCtrlWnd->m_hWnd)
	{
		TDEL(m_pTUCtrlWnd);
		m_pTUCtrlWnd = new CCWTableUCWnd(this);
		m_pTUCtrlWnd->Create(NULL,"hidetablewnd",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),m_pView,IDC_HIDENETWND);
		if(!m_pTUCtrlWnd||!m_pTUCtrlWnd->m_hWnd) return FALSE;
	}
	
	//功能id，用来控制和传递消息等
	strncpy(m_strGnID, strGnID, 20);

	if(m_nClue==-1)
	{
		//创建一路财务处理
		m_nClue=TD_NewClue_();
		if(m_nClue==-1) return FALSE;
		InitConnect();
	}

	CString strGnPath = HomePath+"cwcfg\\";
	if(!GetGnInfo(strGnPath,strGnIni))
		return FALSE;

	m_bFirstEntry = TRUE;

	if(!m_pList)
	{	
		//创建列表
		m_pList= CreateNewTList(0, m_pTUCtrlWnd, CRect(0,0,0,0), IDC_LISTCTRLEX);
		if(!m_pList)
			return FALSE;
	}

	//解析下属性页,本地分类和远程分类
	ShowTabSwitch(m_Cfg.SwitchString);

	if(!m_pTitleDlg)
	{
		m_pTitleDlg = new CTableTitleDlg(m_pTUCtrlWnd);
		if(!m_pTitleDlg || !m_pTitleDlg->Create(IDD_TABLETITLE_DLG, m_pTUCtrlWnd))
			return FALSE;
		m_pTitleDlg->SetTableUnit(this);
		m_pTitleDlg->ShowWindow (SW_SHOW);
		m_pTitleDlg->MoveWindow(CRect(0,0,0,0));
	}
	BOOL bTime1 = strstr(m_Cfg.SQLstatement, "#time1")!=NULL;
	BOOL bTime2 = strstr(m_Cfg.SQLstatement, "#time2")!=NULL;
	m_pTitleDlg->ShowMore(bTime1+bTime2, 0);

	m_vRegChart.clear();

	return TRUE;
}

void	CCWTableUnit::ShowAll(BOOL bShow)
{
	if(bShow)
	{
		if(m_pTUCtrlWnd) m_pTUCtrlWnd->ShowWindow(SW_SHOW);
		if(m_pList) m_pList->ShowWindow(SW_SHOW);
		if(pSwitchBar) pSwitchBar->ShowWindow(SW_SHOW);
		if(m_pTitleDlg) m_pTitleDlg->ShowWindow(SW_SHOW);
	}
	else 
	{
		if(m_pTUCtrlWnd) m_pTUCtrlWnd->ShowWindow(SW_HIDE);
		if(m_pList) m_pList->ShowWindow(SW_HIDE);
		if(pSwitchBar) pSwitchBar->ShowWindow(SW_HIDE);
		if(m_pTitleDlg) m_pTitleDlg->ShowWindow(SW_HIDE);
	}
}

//////////////////////////////////////////////////////////////////////////
//写入参数
void	CCWTableUnit::SetGPSetAndGetData(GP_Set *pGp, BOOL bGetData)
{
	if(pGp&&pGp->nGpNum>0&&memcmp(&m_GpSet, pGp, sizeof(GP_Set)))			//新的集合
	{
		memcpy(&m_GpSet, pGp, sizeof(GP_Set));
		if(bGetData) GetData();
	}
}

void	CCWTableUnit::SetParamAndGetData(TableBGQ *pParam, BOOL bGetData)
{
	if(pParam&&memcmp(&m_CwParam, pParam, sizeof(TableBGQ)))
	{
		memcpy(&m_CwParam, pParam, sizeof(TableBGQ));
		if(bGetData) GetData();
	}
}

void	CCWTableUnit::SetGPCodeAndGetData(short setcode,CString GPCode, BOOL bGetData)
{
	BOOL bRefreshHead = TRUE;
	if((setcode==0||setcode==1)&&GPCode.GetLength()>0)
	{
		if(setcode!=m_GpSet.GpSetInfo[0].setcode||GPCode.CompareNoCase(m_GpSet.GpSetInfo[0].Code)!=0)		//换股
			bRefreshHead = FALSE;
		m_GpSet.nGpNum = 1;
		m_GpSet.GpSetInfo[0].setcode=setcode;
		strcpy(m_GpSet.GpSetInfo[0].Code, (char*)(LPCSTR)GPCode);
	}
	if(bGetData) GetData();
}

void	CCWTableUnit::SetBlockCodeAndGetData(CString BlockCode, BOOL bGetData)
{
	BOOL bRefreshHead = TRUE;
	if(BlockCode.GetLength()>0)
	{
		if(BlockCode.CompareNoCase(m_BlockCode)!=0)		//换板块
			bRefreshHead = FALSE;
		m_BlockCode=BlockCode;
	}
	if(bGetData) GetData();
}

//////////////////////////////////////////////////////////////////////////
void	CCWTableUnit::ArrangeWnd(CRect &rc, BOOL bHide)
{
	if(!m_pTUCtrlWnd) return;
	if(bHide)
	{
		CRect rc(0,0,0,0);
		if(m_pList&&m_pList->m_hWnd)
			m_pList->MoveWindow(rc);
		if(m_pTitleDlg&&m_pTitleDlg->m_hWnd)
			m_pTitleDlg->MoveWindow(rc);
		if(pSwitchBar)
			pSwitchBar->MoveWindow(rc);
		return;
	}

	if(!m_pView) return;

	if(m_pTUCtrlWnd&&m_pTUCtrlWnd->m_hWnd)
		m_pTUCtrlWnd->MoveWindow(&rc, FALSE);

	m_pTUCtrlWnd->GetClientRect(&rc);
	CRect rclist,rcTitle;
	rcTitle=m_rcSwitch=rclist=rc;

	int nTitleHeight = 0;
	if(m_pTitleDlg&&m_pTitleDlg->m_hWnd)
	{
		nTitleHeight = GetTitleHeight();
		rclist.top += nTitleHeight;

		rcTitle.bottom = rc.top+nTitleHeight;
		m_pTitleDlg->MoveWindow(rcTitle);
	}
	if(m_pList&&m_pList->m_hWnd)
	{
		rclist.bottom-=m_SwitchHeight;
		m_pList->MoveWindow(rclist);
	}
	if(pSwitchBar)
	{
		m_rcSwitch.top=rclist.bottom;
		pSwitchBar->MoveWindow(m_rcSwitch);
	}
}

void	CCWTableUnit::OnGetDataAck(WPARAM wParam,LPARAM lParam)
{
	if(m_nClue==-1 || !m_pList) //wParam为0表示出现错误
		return;
	int nCount = 0;
	long nLen=0;

	DWORD nReqID = atoi(m_strGnID)+1;
	try
	{
		if(wParam>0 && (wParam==(WORD)nReqID||wParam==nReqID) && TD_GetRecordBuf_(m_nClue,nLen))
			nCount = TD_GetRowCount_(m_nClue);
	}
	catch (...)
	{
		return;
	}

	if(wParam!=(WORD)nReqID&&wParam!=nReqID) 
		return;
	
	if(nCount<1) 
	{
		SetListTitle(",数据请求失败!");
		return;
	}

	SetListTitle("");
	ProcessSet(m_nClue);
}

void	CCWTableUnit::OnTabList(WPARAM wParam,LPARAM lParam)
{
	if(wParam==TYPE_ZXG&&lParam==-1)		//自选股
	{
		short lpnUsedStkIndex[MAXGPNUM]={-1};
		int nTotalNum=GetGpIndex(-1,TYPE_ZXG,lpnUsedStkIndex);
		GP_Set gs={0};
		FillGPSet(&gs, lpnUsedStkIndex, nTotalNum);
		SetGPSetAndGetData(&gs, TRUE);
	}
	else
	{	
		long nID = (long)lParam;	
		char strKey[128]={0};
		map<long, CString, greater<long> >::iterator iter = m_MapTabList.find(nID);
		if(iter==m_MapTabList.end()) return;
		strcpy(strKey, (*iter).second);
		SetBlockCodeAndGetData(strKey, TRUE);
	}
}

void	CCWTableUnit::OnZHTabCmd(UINT nID)
{
	int nXH = nID - ID_ZHTABLIST_MENU_1;

	char lpString[1024]={0};
	CString	ListIni = (CString)HomePath+(CString)"cwcfg\\"+(CString)"tablist.ini";
	GetPrivateProfileString("ZHList",g_ZHTabString,"",lpString,1024,ListIni);
	char *argv[100];
	int argc = GetCmdLine(lpString, argv, 100, "|");
	if(argc<nXH) return;
	ChangeTabSwitch(argv[nXH]);

	m_nZHTabCmdID = nID;
}

void	CCWTableUnit::OnUpdateZHTabCmd(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nZHTabCmdID>=0&&pCmdUI->m_nID==m_nZHTabCmdID);
}

void	CCWTableUnit::OnTabListCmd(UINT nID)
{
	int nXH = nID - ID_INITABLIST_MENU_1;
	for(int i=0;i<100;i++)
	{
		if(nXH>=m_MainItemNum[i]) 
			nXH -= m_MainItemNum[i];
		else break;
	}
	int nMainID = i+1, nSubID = nXH;
	long nIID = 100*100*nMainID+100*nXH;

	char strKey[128]={0};
	map<long, CString, greater<long> >::iterator iter = m_MapTabList.find(nIID);
	if(iter==m_MapTabList.end()) return;
	strcpy(strKey, (*iter).second);
	SetBlockCodeAndGetData(strKey, TRUE);

	m_nTabListCmdID = nID;
	m_nTabListBlockCmdID = -1;
}

void	CCWTableUnit::OnUpdateTabListCmd(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nTabListCmdID>=0&&pCmdUI->m_nID==m_nTabListCmdID);
}

void	CCWTableUnit::OnTabListBlockCmd(UINT nID)
{
	short lpnUsedStkIndex[MAXGPNUM]={-1};
	int nBlockFlag = nID - ID_INITABBLOCK_MENU_1;
	int nTotalNum=GetGpIndex(nBlockFlag,-1,lpnUsedStkIndex);
	GP_Set gs={0};
	FillGPSet(&gs, lpnUsedStkIndex, nTotalNum);
	SetGPSetAndGetData(&gs, TRUE);

	m_nTabListCmdID = -1;
	m_nTabListBlockCmdID = nID;
}

void	CCWTableUnit::OnUpdateTabListBlockCmd(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nTabListBlockCmdID>=0&&pCmdUI->m_nID==m_nTabListBlockCmdID);
}

void	CCWTableUnit::OnListRowGnPop()
{
	if(!m_pList) return;

	if( !m_pView->IsKindOf(RUNTIME_CLASS(CJbfxView)) )
		return;
	CJbfxView *pJbfxView = (CJbfxView*)m_pView;

//	pJbfxView->SetSwitchFromGnString("B02002");

	int nCount = m_pList->GetItemCount();
	int nCurSel = m_pList->GetCurSel();
	for(int i=0;i<nCount;i++)
	{
		for(int j=0;j<m_Cfg.OutNum;j++)
		{
			sg_cell sc;
			m_pList->GetCellData(sc, j, i);

			if(i==nCurSel) 
			{				
				break;
			}
		}
	}
	
}
//////////////////////////////////////////////////////////////////////////
void	CCWTableUnit::RefreshColorFontCfg()
{
	if(m_pList)
	{
		SuperList_ColorFont scf;
		scf= m_pList->GetColorFont();
		scf.BkColor= scf.DataBkColor = VipColor.BACKCOLOR;
		scf.DataTextColor = VipColor.TXTCOLOR;
		scf.CodeNameColor = GridColor.Grid_CodeName_Color;
		scf.ScrollBarColor= VipColor.AXISCOLOR;
		scf.ListLineColor= GridColor.GridLineColor;
		scf.SelectedColor=VipColor.FLOATCURSORCOLOR;
		if(m_nShowStyle==1) 
		{
			scf.HeadBkColor = VipColor.BACKCOLOR;
			scf.HeadTextColor = VipColor.TXTCOLOR;
			
			CFont font;	
			font.CreateFont(g_nFontSize[0],0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,g_b2000XPSys?ANSI_CHARSET:DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_MODERN,
				g_strFontName[0]
				);	
			font.GetLogFont(&scf.ListFont);
			font.DeleteObject();
		}
		else
		{
			//同ListCtrlEx.cpp
			scf.HeadBkColor = RGB(212,208,200);
			scf.HeadTextColor = RGB(10,10,10);
			
			CFont font;	
			font.CreateFont(12,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,g_b2000XPSys?ANSI_CHARSET:DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,PROOF_QUALITY,DEFAULT_PITCH|FF_MODERN,
				"宋体"
				);	
			font.GetLogFont(&scf.ListFont);
			font.DeleteObject();
		}

		m_pList->SetColorFont(scf);
	}
}

void	CCWTableUnit::BeginShowList()
{
	DWORD dwStyle=0;
	if(!m_Cfg.bVertical) dwStyle|=TLVS_HeaderOnTop;
	dwStyle|=(
		TLVS_AutoArrange
	//	|TLVS_REPORT
		|TLVS_ColumnHeader
		|TLVS_SortColumn
	//	|TLVS_SHOWSELALWAYS
	//	|TLVS_SHAREIMAGELIST
	//	|TLVS_VERTSORTLINE
	//	|TLVS_HEADERATTOP
	//	|TLVS_LISTTITLE
	//	|TLVS_SUMARY
	//	|TLVS_LEACH
		|TLVS_SaveOut
		|TLVS_ChangeMode
		|TLVS_HasListGrid
	//	|TLVS_COLORSTRIP
		);

	if(m_pList)	
	{
		SuperList_OtherInfo sloi = m_pList->GetOtherInfo();
		if(m_nShowStyle==1)
		{
			CDC *pDC = m_pList->GetDC();
			CSize sz = pDC->GetTextExtent("试");
			sloi.HeadHeight = sz.cy+3;
			sloi.LineHeight = sz.cy+6;
		}
		else 
		{
			sloi.HeadHeight = 28;
			sloi.LineHeight = 21;
		}
		sloi.SelChangeMsg = UM_LIST_SELCHANGE;
		sloi.ColSortMsg = UM_LIST_COLSORT;
		sloi.LButtonDblClkMsg = sloi.ReturnKeyMsg = UM_LIST_ROWGNPOP;

		m_pList->SetStyle(dwStyle);
		m_pList->SetOtherInfo(sloi);
	}

	RefreshColorFontCfg();
}

void	CCWTableUnit::SetListTitle(char *strExtMsg)
{
	//设置列表标题
	CString strTitle = m_Cfg.ListTitle;
	if(strTitle.GetLength()>0) 
		strTitle += (CString)strExtMsg;
	if(m_Cfg.bSingle) 
	{
		MemStkInfo *pInfo = g_pStockIO->Get_MemStkInfo(m_GpSet.GpSetInfo[0].Code, m_GpSet.GpSetInfo[0].setcode);
		if(!pInfo) pInfo = g_pStockIO->Get_MemStkInfo(JbfxCurGPCode, JbfxCurSetCode);
		if(pInfo)
			strTitle += ((CString)" - "+pInfo->Name);
	}
	m_pList->SetListTitle(strTitle);
	if(m_pTitleDlg) m_pTitleDlg->SetTitle(strTitle);
}

void	CCWTableUnit::RefreshHeadFrame()
{
	m_pList->DeleteColByIndex(-1, TRUE);

	sg_head head;
	int i, nHeadNum = 0;
	for(i=0;i < m_Cfg.OutNum;i++)
	{
		m_pList->InitHead(&head);
		head.width=m_Cfg.TitleCfg[i].OutWidth;
		strcpy(head.title,m_Cfg.TitleCfg[i].OutTitle);
		strcpy(head.name,m_Cfg.TitleCfg[i].OutName);
		head.decimaldigits=m_Cfg.TitleCfg[i].OutDataDecBit;

		switch(m_Cfg.TitleCfg[i].OutType)
		{
		case 'D':head.datatype=SG_TYPE_INT;break;
		case 'C':head.datatype=SG_TYPE_STRING;break;
		case 'F':head.datatype=SG_TYPE_DOUBLE;break;
		default: head.datatype= SG_TYPE_DOUBLE;break;
		}
		switch(m_Cfg.TitleCfg[i].OutDataAlign)
		{
		case 'L':head.align=SG_ALIGN_LEFT;break;
		case 'R':head.align=SG_ALIGN_RIGHT;break;
		case 'C':head.align=SG_ALIGN_CENTER;break;
		default: head.align=SG_ALIGN_LEFT;break;
		}
		switch(m_Cfg.TitleCfg[i].OutHeadAlign)
		{
		case 'L':head.headalign=SG_ALIGN_LEFT;break;
		case 'R':head.headalign=SG_ALIGN_RIGHT;break;
		case 'C':head.headalign=SG_ALIGN_CENTER;break;
		default: head.headalign=SG_ALIGN_LEFT;break;
		}
		switch(m_Cfg.TitleCfg[i].OutDataFormat)
		{
        case 'A':head.formattype= SG_FORMAT_COMMA_SEP;break;
		case 'B':head.formattype= SG_PERCENT_BANJIAO;break;
		case 'Q':head.formattype= SG_PERCENT_QUANJIAO;break;
		case 'C':head.formattype= SG_ONE_THOUSAND;break;
		case 'D':head.formattype= SG_FORMAT_NORMAL;break;
	    case 'E':head.formattype= SG_FORMAT_WAN;break;
        case 'F':head.formattype= SG_FORMAT_YI;break;
		case 'G':head.formattype= SG_DATE_NORMAL;break;
		case 'H':head.formattype= SG_DATE_SLASH;break;
		case 'I':head.formattype= SG_DATE_SHORTLINE;break;
		case 'J':head.formattype= SG_DATE_SLASH2;break;
		case 'K':head.formattype= SG_FORMAT_COMMA_SEP_WAN;break;
		case 'L':head.formattype= SG_FORMAT_COMMA_SEP_YI;break;
		case 'M':head.formattype= SG_FORMAT_WAN_YI;break;
		case 'N':head.formattype= SG_FORMAT_COMMA_SEP_WAN_YI;break;
		case 'R':head.formattype= SG_DATE_NORMAL_SHORT;break;
		case 'S':head.formattype= SG_DATE_SLASH_SHORT;break;
		case 'T':head.formattype= SG_DATE_SHORTLINE_SHORT;break;
		case 'U':head.formattype= SG_DATE_SLASH2_SHORT;break;
		default:head.formattype= SG_FORMAT_NORMAL;break;
		}//switch
		switch(m_Cfg.TitleCfg[i].OutDataFormatZero)
		{
		case 0:head.formatzero=SG_FORMAT_ZERO_NORMAL;break;
		case 1:head.formatzero=SG_FORMAT_ZERO_BLANK;break;
		case 2:head.formatzero=SG_FORMAT_ZERO_SHORT_LINE;break;
		default:head.formatzero=SG_FORMAT_ZERO_NORMAL;break;
		}//switch
		switch(m_Cfg.TitleCfg[i].OutDataColor)
		{
		case 'V':head.datacolormode= SG_DATACOLOR_MODE_V;break;
        case 'W':head.datacolormode= SG_DATACOLOR_MODE_W;break;
		case 'P':head.datacolormode= SG_DATACOLOR_MODE_P;break;
		case 'B':head.datacolormode= SG_DATACOLOR_MODE_B;
			     strcpy(head.outname_bj, m_Cfg.TitleCfg[i].OutName_BJ);
			     break;
		case 'Z':head.datacolormode= SG_DATACOLOR_MODE_Z;break;
		default: head.datacolormode= SG_DATACOLOR_MODE_W;break;
		}//switch
		m_pList->InsertHead(&head, (i==m_Cfg.OutNum-1));
	}
}

//////////////////////////////////////////////////////////////////////////
BOOL	CCWTableUnit::InitConnect()
{
	if (!AfxSocketInit())
		return FALSE;
	
	AfxEnableControlContainer();

	//BI服务器的配置	
	CString	Ini = (CString)g_WSXHStr+g_strYxhjCfg;
	GetPrivateProfileString("BIServer","IPAddress","127.0.0.1",g_BIHostIP,48,Ini);
	g_nBIHostPort = GetPrivateProfileInt("BIServer","Port",7717,Ini);
	GetPrivateProfileString("BIServer","BakIPAddress","",g_BakBIHostIP,48,Ini);
	g_nBakBIHostPort = GetPrivateProfileInt("BIServer","BakPort",7717,Ini);
	//代理信息
	CString	UserIni = (CString)HomePath+g_WSXHStr+g_strYxhjCfg;
	g_ProxyInfo.m_bProxy = GetPrivateProfileInt("PROXY","HasProxy",0,UserIni);
	g_ProxyInfo.m_nSockType = GetPrivateProfileInt("PROXY","SockType",2,UserIni);
	if(g_ProxyInfo.m_nSockType != 0 && g_ProxyInfo.m_nSockType != 1 && g_ProxyInfo.m_nSockType != 2)
		g_ProxyInfo.m_nSockType = 0;
	g_ProxyInfo.m_nProxyPort = GetPrivateProfileInt("PROXY","ProxyPort",80,UserIni);
	GetPrivateProfileString("PROXY","ProxyIP","",g_ProxyInfo.m_strProxyIP,99,UserIni);
	GetPrivateProfileString("PROXY","ProxyUser","",g_ProxyInfo.m_strProxyUser,99,UserIni);
	GetPrivateProfileString("PROXY","ProxyPass","",g_ProxyInfo.m_strProxyPass,99,UserIni);

	return InitConnectInfo();
}

BOOL	CCWTableUnit::InitConnectInfo()
{
	if(m_nClue==-1) 
		return FALSE;
	int PorxyID=PROXY_NONE;
	if(g_ProxyInfo.m_bProxy)
	{
		if(g_ProxyInfo.m_nSockType==0) 
			PorxyID=PROXY_SOCK4;
		else if(g_ProxyInfo.m_nSockType==1) 
			PorxyID=PROXY_SOCK5;
		else 
			PorxyID=PROXY_HTTP;
	}
	TD_SetConnect_(m_nClue,g_BIHostIP,g_nBIHostPort,PorxyID,g_ProxyInfo.m_strProxyIP,g_ProxyInfo.m_nProxyPort,g_ProxyInfo.m_strProxyUser,g_ProxyInfo.m_strProxyPass);
	TD_SetAsynInfo_(m_nClue,m_pTUCtrlWnd->GetSafeHwnd(),UM_ASYN_SQL_MSG,0);

	return TRUE;
}

BOOL	CCWTableUnit::GetGnInfo(CString GnPath,CString GnINI)
{
	if(!GetFromGNIni(&m_Cfg,GnPath+GnINI))
		return FALSE;
	strncpy(m_Cfg.GnIniFile, (char*)(LPCSTR)GnINI, MAX_PATH-1);
	return TRUE;
}

BOOL  CCWTableUnit::GetFromGNIni(struct Table_GN_Cfg *pCfg, const char *PathFile)
{
	if(!pCfg) return FALSE;
	memset(pCfg,0,sizeof(struct Table_GN_Cfg));
	CString Ini = PathFile;
	CString tempStr;
	int i;
	pCfg->GnIniVer = GetPrivateProfileInt("Gn","GNIniVer",0,Ini);
	//Report
	GetPrivateProfileString("DataSrc","SQLstatement","",pCfg->SQLstatement,1023,Ini);
	//listtitle
	GetPrivateProfileString("Report","ListTitle","",pCfg->ListTitle,99,Ini);
	//verticallist
	pCfg->bVertical=GetPrivateProfileInt("Report","VerticalList",0,Ini);
	pCfg->OutAutowidth = GetPrivateProfileInt("Report","OutAutowidth",1,Ini);//1 for autowidth
	
	pCfg->bSingle = GetPrivateProfileInt("Report","SingleFX",0,Ini);
	GetPrivateProfileString("Report","CodeColName","",pCfg->CodeColName,20,Ini);
	GetPrivateProfileString("Report","TabList","",pCfg->SwitchString,20,Ini);
	
	//Out
	pCfg->OutNum = GetPrivateProfileInt("ListOut","OutNum",0,Ini);
	if(pCfg->OutNum<=0 ) 
		return FALSE;
	pCfg->TitleCfg.resize(pCfg->OutNum);
	for(i=0;i < pCfg->OutNum;i++)
	{
		//运算结果的值的类型
		tempStr.Format("OutType%02d",i+1);
		pCfg->TitleCfg[i].OutType = GetPrivateProfileChar("ListOut",tempStr,'C',Ini);//defaut for string or say,  'C'
		//列名
		tempStr.Format("OutName%02d",i+1);
		GetPrivateProfileString("ListOut",tempStr,"",pCfg->TitleCfg[i].OutName,30,Ini);
		//列的标题
		tempStr.Format("OutTitle%02d",i+1);
		GetPrivateProfileString("ListOut",tempStr,"",pCfg->TitleCfg[i].OutTitle,100,Ini);
		//列的宽度,若pCfg->OutAutowidth,则OutWidth被忽略;
		if(!pCfg->OutAutowidth)
		{
		  tempStr.Format("OutWidth%02d",i+1);
		  pCfg->TitleCfg[i].OutWidth=GetPrivateProfileInt("ListOut",tempStr,10,Ini);
		}
		//数据对齐方式
		tempStr.Format("OutDataAlign%02d",i+1);
		pCfg->TitleCfg[i].OutDataAlign=GetPrivateProfileChar("ListOut",tempStr,'L',Ini);
		//表头对齐方式
		tempStr.Format("OutHeadAlign%02d",i+1);
		pCfg->TitleCfg[i].OutHeadAlign=GetPrivateProfileChar("ListOut",tempStr,'L',Ini);
		//数据小数位数
		tempStr.Format("OutDataDecBit%02d",i+1);
		pCfg->TitleCfg[i].OutDataDecBit=GetPrivateProfileInt("ListOut",tempStr,0,Ini);
		//数据显示格式
		tempStr.Format("OutDataFormat%02d",i+1);
		pCfg->TitleCfg[i].OutDataFormat=GetPrivateProfileChar("ListOut",tempStr,'D',Ini);
		//数据颜色方式
		tempStr.Format("OutDataColor%02d",i+1);
		pCfg->TitleCfg[i].OutDataColor=GetPrivateProfileChar("ListOut",tempStr,'W',Ini);
		//用于比较的列名
		tempStr.Format("OutName_BJ%02d",i+1);
		GetPrivateProfileString("ListOut",tempStr,"",pCfg->TitleCfg[i].OutName_BJ,30,Ini);
		//数据为零时显示格式
		tempStr.Format("OutDataFormatZero%02d",i+1);
		pCfg->TitleCfg[i].OutDataFormatZero=GetPrivateProfileInt("ListOut",tempStr,0,Ini);
		//公式运算
		tempStr.Format("FormulaString%02d", i+1 );
		char strformula[1024]={0};
		GetPrivateProfileString("ListOut",tempStr,"",strformula,1023,Ini);
		if(strlen(strformula)>0) 
		{
			pCfg->TitleCfg[i].OutFormula = strformula;
		}
		pCfg->TitleCfg[i].OutFormula.Replace("^", "\r\n");
		//计算代码列名
		tempStr.Format("CalcCode%02d",i+1);
		GetPrivateProfileString("ListOut",tempStr,"",pCfg->TitleCfg[i].OutCalcCode,20,Ini);
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////

void	CCWTableUnit::ShowTabSwitch(char *SwitchString)
{
	TDEL(pSwitchBar)
	if(!pSwitchBar)
	{
		pSwitchBar = new CNewZfjsTab(m_pView->GetSafeHwnd());
		if(!pSwitchBar || !pSwitchBar->Create(_T("NewZfjsTab"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),m_pTUCtrlWnd,ID_CWSWITCHBAR))
			return;
		pSwitchBar->SetMenuCallBack((PMENUCALLBACK)SwitchMenuCallBack);
	}
	pSwitchBar->ClearAllLabel();

	m_MapTabList.clear();
	int iNo = 0, iSelNo = -1;

	memset(m_MainItemNum, 0, sizeof(m_MainItemNum));

	char lpString[1024]={0};
	CString	ListIni = (CString)HomePath+(CString)"cwcfg\\"+(CString)"tablist.ini";

	//直接找到了，不需要
	BOOL bHasZHTabList = FALSE;
	GetPrivateProfileString("TabList",SwitchString,"",lpString,1024,ListIni);
	if(strlen(lpString)<1) 
	{
		GetPrivateProfileString("ZHList",SwitchString,"",lpString,1024,ListIni);
		if(strlen(lpString)<1) 
		{
			m_SwitchHeight = 0;
			return;
		}
		bHasZHTabList = TRUE;
	}

	m_SwitchHeight = SWITCH_HEIGHT;

	//分类List
	if(bHasZHTabList)
	{
		strncpy(g_ZHTabString, SwitchString, 20);
		pSwitchBar->InsertSwitchLabel(iNo++,_F("分类"),UM_ZHTAB_MSG,0,0,TRUE);

		char strTabCache[128]={0};
		GetPrivateProfileString("ZHList","TabCache","",strTabCache,127,ListIni);	
		GetPrivateProfileString("TabList",strTabCache,"",lpString,1024,ListIni);
		if(strlen(lpString)<1) 
		{
			m_SwitchHeight = 0;
			return;
		}
		char *argv[100];
		char tmpString[1024]={0};
		GetPrivateProfileString("ZHList",g_ZHTabString,"",tmpString,1024,ListIni);
		int argc = GetCmdLine(tmpString, argv, 100, "|");
		for(int i=0;i<argc;i++)
		{
			if(strcmp(argv[i], strTabCache)==0)
			{
				m_nZHTabCmdID = i+ID_ZHTABLIST_MENU_1;
				break;
			}
		}
	}

	char strItemName[128]={0};
	char strKey[128]={0};
	char *argv[100];
	int argc = GetCmdLine(lpString, argv, 100, "|");
	for(int i=0;i<argc;i++)
	{
		long nMainID = 100*(i+1);
		char *subString = NULL;
		if(subString = strrchr((const char*)argv[i], '#'))		//有子菜单
		{
			subString[0] = 0;

			char *sargv[100];
			int sargc = GetCmdLine(&subString[1], sargv, 100, ",");
			for(int j=0;j<sargc;j++)
			{
				long nSubID = 100*j;
				long nID = 100*nMainID+nSubID;
				
				strncpy(strKey, sargv[j], 127);
				m_MapTabList[nID] = (CString)strKey;
			}
			m_MainItemNum[i] = sargc;

			long nID = nMainID;
			nMainID+=sargc;
			strncpy(strKey, argv[i], 127);
			m_MapTabList[nID] = (CString)strKey;

			memset(strItemName, 0, sizeof(strItemName));
			GetPrivateProfileString("ItemMap",strKey,"",strItemName,1024,ListIni);
			if(strItemName[0]==0)			//没有找到,从行业中找
			{
				char *sargv[2];
				int sargc = GetCmdLine(strKey, sargv, 2, ".");		//分类标准.分类
				if(sargc==2)										//
				{
					char strNodes[2555]={0};
					char *strAppFlag = sargv[0];
					char *NodeCode = sargv[1];
					GetHYCodeName(strAppFlag, strItemName, NodeCode);
				}
			}
			if(strItemName[0]==0) strcpy(strItemName, strKey);
			pSwitchBar->InsertSwitchLabel(iNo++,_F(strItemName),UM_TABLIST_MSG,nMainID,nID,TRUE);
		}
		else if(argv[i][0]=='$')								//行业类别
		{
			char *sargv[2];
			int sargc = GetCmdLine(&argv[i][1], sargv, 2, ".");		//分类标准.分类
			if(sargc==2)										//
			{
				char strNodes[2555]={0};
				char *strAppFlag = sargv[0];
				char *NodeCode = sargv[1];
				char strApp[100]={0};
				sprintf(strApp, "layer%s", strAppFlag);
				GetPrivateProfileString(strApp, NodeCode, "", strNodes, 2554, g_LayerPath);
				char *argv1[100]={0};
				int argc1 = 0;
				if(strNodes[0])
				{
					argc1 = GetCmdLine(strNodes, argv1, 99, ",\r\n");
					for(int j=0;j<argc1;j++)
					{
						long nSubID = 100*j;
						long nID = 100*nMainID+nSubID;
						
						strcpy(strKey, strAppFlag);
						strcat(strKey, ".");
						strcat(strKey, argv1[j]);
						m_MapTabList[nID] = (CString)strKey;
					}
					m_MainItemNum[i] = argc1;
				}
				long nID = nMainID;
				nMainID+=argc1;
				
				strcpy(strKey, strAppFlag);
				strcat(strKey, ".");
				strcat(strKey, NodeCode);

				m_MapTabList[nID] = (CString)strKey;
				
				memset(strItemName, 0, sizeof(strItemName));
				GetHYCodeName(strAppFlag, strItemName, NodeCode);
				pSwitchBar->InsertSwitchLabel(iNo++,_F(strItemName),UM_TABLIST_MSG,nMainID,nID,strNodes[0]);
			}
		}
		else
		{
			long nID = 100*nMainID;

			strncpy(strKey, argv[i], 127);
			m_MapTabList[nID] = (CString)strKey;

			if(stricmp(strKey, "ZX")==0)				//自选板块需要自己填充股票集
			{
				pSwitchBar->InsertSwitchLabel(iNo++,_F("自选"),UM_TABLIST_MSG,TYPE_ZXG,-1,FALSE);
			}
			else if(stricmp(strKey, "ZD")==0)
			{
				pSwitchBar->InsertSwitchLabel(iNo++,_F("自定"),UM_TABLIST_MSG,IDR_LONELY_MENU,-3,TRUE);//-3表示是很特殊的自定板块
			}
			else
			{
				memset(strItemName, 0, sizeof(strItemName));
				GetPrivateProfileString("ItemMap",strKey,"",strItemName,1024,ListIni);
				if(strItemName[0]==0)			//没有找到,从行业中找
				{
					char *sargv[2];
					int sargc = GetCmdLine(strKey, sargv, 2, ".");		//分类标准.分类
					if(sargc==2)										//
					{
						char strNodes[2555]={0};
						char *strAppFlag = sargv[0];
						char *NodeCode = sargv[1];
						memset(strItemName, 0, sizeof(strItemName));
						GetHYCodeName(strAppFlag, strItemName, NodeCode);
					}
				}
				if(strItemName[0]==0) strcpy(strItemName, strKey);
				pSwitchBar->InsertSwitchLabel(iNo++,_F(strItemName),UM_TABLIST_MSG,nMainID,nID,FALSE);
			}
		}
	}
	g_pMapTabList = &m_MapTabList;
	g_pMainItemNum = m_MainItemNum;
	pSwitchBar->SetCurLabel(0);
}

void	CCWTableUnit::ChangeTabSwitch(char *SwitchString)
{
	TDEL(pSwitchBar)
	if(!pSwitchBar)
	{
		pSwitchBar = new CNewZfjsTab(m_pView->GetSafeHwnd());
		if(!pSwitchBar || !pSwitchBar->Create(_T("NewZfjsTab"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),m_pTUCtrlWnd,ID_CWSWITCHBAR))
			return;
		pSwitchBar->SetMenuCallBack((PMENUCALLBACK)SwitchMenuCallBack);
	}
	pSwitchBar->ClearAllLabel();

	m_MapTabList.clear();
	int iNo = 0, iSelNo = -1;

	memset(m_MainItemNum, 0, sizeof(m_MainItemNum));

	char lpString[1024]={0};
	CString	ListIni = (CString)HomePath+(CString)"cwcfg\\"+(CString)"tablist.ini";

	//直接找到了，不需要
	GetPrivateProfileString("TabList",SwitchString,"",lpString,1024,ListIni);
	if(strlen(lpString)<1) 
	{
		GetPrivateProfileString("ZHList",g_ZHTabString,"",lpString,1024,ListIni);
		if(strlen(lpString)<1) 
		{
			m_SwitchHeight = 0;
			return;
		}
	}

	m_SwitchHeight = SWITCH_HEIGHT;

	//综合List
	pSwitchBar->InsertSwitchLabel(iNo++,_F("分类"),UM_ZHTAB_MSG,0,0,TRUE);

	char strItemName[128]={0};
	char strKey[128]={0};
	char *argv[100];
	int argc = GetCmdLine(lpString, argv, 100, "|");
	for(int i=0;i<argc;i++)
	{
		long nMainID = 100*(i+1);
		char *subString = NULL;
		if(subString = strrchr((const char*)argv[i], '#'))		//有子菜单
		{
			subString[0] = 0;

			char *sargv[100];
			int sargc = GetCmdLine(&subString[1], sargv, 100, ",");
			for(int j=0;j<sargc;j++)
			{
				long nSubID = 100*j;
				long nID = 100*nMainID+nSubID;
				
				strncpy(strKey, sargv[j], 127);
				m_MapTabList[nID] = (CString)strKey;
			}
			m_MainItemNum[i] = sargc;

			long nID = nMainID;
			nMainID+=sargc;
			strncpy(strKey, argv[i], 127);
			m_MapTabList[nID] = (CString)strKey;

			memset(strItemName, 0, sizeof(strItemName));
			GetPrivateProfileString("ItemMap",strKey,"",strItemName,1024,ListIni);
			if(strItemName[0]==0)			//没有找到,从行业中找
			{
				char *sargv[2];
				int sargc = GetCmdLine(strKey, sargv, 2, ".");		//分类标准.分类
				if(sargc==2)										//
				{
					char strNodes[2555]={0};
					char *strAppFlag = sargv[0];
					char *NodeCode = sargv[1];
					GetHYCodeName(strAppFlag, strItemName, NodeCode);
				}
			}
			if(strItemName[0]==0) strcpy(strItemName, strKey);
			pSwitchBar->InsertSwitchLabel(iNo++,_F(strItemName),UM_TABLIST_MSG,
				nMainID,nID,TRUE);
		}
		else if(argv[i][0]=='$')								//行业类别
		{
			char *sargv[2];
			int sargc = GetCmdLine(&argv[i][1], sargv, 2, ".");		//分类标准.分类
			if(sargc==2)										//
			{
				char strNodes[2555]={0};
				char *strAppFlag = sargv[0];
				char *NodeCode = sargv[1];
				char strApp[100]={0};
				sprintf(strApp, "layer%s", strAppFlag);
				GetPrivateProfileString(strApp, NodeCode, "", strNodes, 2554, g_LayerPath);
				char *argv1[100]={0};
				int argc1 = 0;
				if(strNodes[0])
				{
					argc1 = GetCmdLine(strNodes, argv1, 99, ",\r\n");
					for(int j=0;j<argc1;j++)
					{
						long nSubID = 100*j;
						long nID = 100*nMainID+nSubID;
						
						strcpy(strKey, strAppFlag);
						strcat(strKey, ".");
						strcat(strKey, argv1[j]);
						m_MapTabList[nID] = (CString)strKey;
					}
					m_MainItemNum[i] = argc1;
				}
				long nID = nMainID;
				nMainID+=argc1;
				
				strcpy(strKey, strAppFlag);
				strcat(strKey, ".");
				strcat(strKey, NodeCode);

				m_MapTabList[nID] = (CString)strKey;
				
				memset(strItemName, 0, sizeof(strItemName));
				GetHYCodeName(strAppFlag, strItemName, NodeCode);
				pSwitchBar->InsertSwitchLabel(iNo++,_F(strItemName),UM_TABLIST_MSG,
					nMainID,nID,strNodes[0]);
			}
		}
		else
		{
			long nID = 100*nMainID;

			strncpy(strKey, argv[i], 127);
			m_MapTabList[nID] = (CString)strKey;

			if(stricmp(strKey, "ZX")==0)				//自选板块需要自己填充股票集
			{
				pSwitchBar->InsertSwitchLabel(iNo++,_F("自选"),UM_TABLIST_MSG,TYPE_ZXG,-1,FALSE);
			}
			else if(stricmp(strKey, "ZD")==0)
			{
				pSwitchBar->InsertSwitchLabel(iNo++,_F("自定"),UM_TABLIST_MSG,IDR_LONELY_MENU,-3,TRUE);//-3表示是很特殊的自定板块
			}
			else
			{
				memset(strItemName, 0, sizeof(strItemName));
				GetPrivateProfileString("ItemMap",strKey,"",strItemName,1024,ListIni);
				if(strItemName[0]==0)			//没有找到,从行业中找
				{
					char *sargv[2];
					int sargc = GetCmdLine(strKey, sargv, 2, ".");		//分类标准.分类
					if(sargc==2)										//
					{
						char strNodes[2555]={0};
						char *strAppFlag = sargv[0];
						char *NodeCode = sargv[1];
						GetHYCodeName(strAppFlag, strItemName, NodeCode);
					}
				}
				if(strItemName[0]==0) strcpy(strItemName, strKey);
				pSwitchBar->InsertSwitchLabel(iNo++,_F(strItemName),UM_TABLIST_MSG,nMainID,nID,FALSE);
			}
		}
	}
	g_pMapTabList = &m_MapTabList;
	g_pMainItemNum = m_MainItemNum;
	pSwitchBar->SetCurLabel(0);

	pSwitchBar->MoveWindow(m_rcSwitch);
}

//异步
BOOL	CCWTableUnit::GetData(BOOL bRefreshHead)
{	
	CString tmpCfgStr=m_Cfg.SQLstatement;
	
	m_pTitleDlg->GetTimeParams(&m_CwParam);
	
	CString tmpRepStr=_T(""), tmpReqStrMul=_T("");
	if(!m_Cfg.bSingle)			//多股的
	{
		CString strUnitCode=_T("");
		for(int j=0;j<m_GpSet.nGpNum;j++)
		{
			strUnitCode.Format("%d#%s",m_GpSet.GpSetInfo[j].setcode,m_GpSet.GpSetInfo[j].Code);
			tmpReqStrMul += strUnitCode;
		}
		tmpCfgStr.Replace("#codeset",tmpReqStrMul);

		tmpCfgStr.Replace("#rownum",IntToStr(m_GpSet.nGpNum));
	}
	else						//单股
	{
		if(m_GpSet.nGpNum==1)
		{
			tmpRepStr.Format("%d#%s",m_GpSet.GpSetInfo[0].setcode,m_GpSet.GpSetInfo[0].Code);
			tmpCfgStr.Replace("#code",m_GpSet.GpSetInfo[0].Code);
			tmpCfgStr.Replace("#scode",tmpRepStr);
		}
		else
		{
			tmpRepStr.Format("%d#%s",JbfxCurSetCode,JbfxCurGPCode);
			tmpCfgStr.Replace("#code",JbfxCurGPCode);
			tmpCfgStr.Replace("#scode",tmpRepStr);
		}
	}
	tmpCfgStr.Replace("#time1",IntToStr(m_CwParam.nCwTimeS));
	tmpCfgStr.Replace("#time2",IntToStr(m_CwParam.nCwTimeE));

	tmpCfgStr.Replace("#flcode",m_BlockCode);			//板块分类代码

	if(m_nClue==-1) 
		return FALSE;

	DeleteListContent();
	if(bRefreshHead||m_bFirstEntry) 
		RefreshHeadFrame();
	m_bFirstEntry = FALSE;

	//读取数据
	try
	{
		DWORD nReqID = atoi(m_strGnID)+1;
		if(tmpCfgStr.GetLength()>0) 
		{
			TD_AsynRemoteSQL_(m_nClue,TRUE,nReqID,"0","0",tmpCfgStr);
			SetListTitle(",数据请求中...");
		}
	}
	catch(...)
	{
		return FALSE;
	}

	return TRUE;
}

#define MAX_LISTSTR_LEN 1024*5 
void	CCWTableUnit::ProcessSet(long nClue)
{
	//插入数据
	int		wantlen = MAX_LISTSTR_LEN-2;
	int		nRet, i;
	float	fRet;
	BOOL	bSuccess;

	DeleteListContent();

	int iii = 0;
	int nCount = TD_GetRowCount_(m_nClue);
	int nCurSel = m_pList->GetCurSel();
	if(nCurSel<0||nCurSel>nCount) nCurSel = 0;

	CCWCalc CWCalc(nClue);

	for(i=0;i<nCount;i++)
	{
		if(!TD_Fetch_(nClue)) 
			break;
		
		char	buf[MAX_LISTSTR_LEN]={0};
		sg_cell_line sl;
		sl.nFlag = 0;
		sl.data = 0;
		for(int j=0;j<m_Cfg.OutNum;j++)
		{
			sg_cell sc;
			sc.stringdata.Empty();
			sc.iImage= -1;//无图标
			sc.iSelectedImage= 0;//选定时有为第一个图标

			char *strTitle = m_Cfg.TitleCfg[j].OutName;
			switch(m_Cfg.TitleCfg[j].OutType) 
			{
			case 'D'://int
				bSuccess=TD_GetIntData_(nClue,m_Cfg.TitleCfg[j].OutName,nRet);
				if(bSuccess)
					sc.data.intdata = nRet;
				break;
			case 'F'://float
				bSuccess=TD_GetFloatData_(nClue,m_Cfg.TitleCfg[j].OutName,fRet);
				if(bSuccess)
					sc.data.doubledata = fRet;
				break;
			case 'C'://string
				wantlen = MAX_LISTSTR_LEN-2;
				bSuccess=TD_GetStringData_(nClue,m_Cfg.TitleCfg[j].OutName,buf,wantlen);
				if(bSuccess)
				{
					buf[wantlen]=0;
					int nLen = strlen(buf);
					if(nLen>0) 
					{
						LPTSTR pstr = sc.stringdata.GetBufferSetLength(nLen+1);
						strcpy(pstr, buf);
					}
				}
				break;
			}

			if(!bSuccess)		//未能直接取到数据,需要二次计算
			{
				//取用代码
				char Code[CODE_SIZE]={0}; short nSetCode=-1;
				//有列代码字段
				char strtest[21]={0};
				strcpy(strtest, m_Cfg.TitleCfg[j].OutCalcCode);
				if(m_Cfg.TitleCfg[j].OutCalcCode[0])
				{
					wantlen = MAX_LISTSTR_LEN-2;
					bSuccess=TD_GetStringData_(nClue,m_Cfg.TitleCfg[j].OutCalcCode,buf,wantlen);
					if(_strnicmp(buf, "SH", 2)==0) 
					{
						nSetCode=1;
						strncpy(Code, &buf[2], CODE_SIZE);
					}
					else if(_strnicmp(buf, "SZ", 2)==0) 
					{
						nSetCode=0;
						strncpy(Code, &buf[2], CODE_SIZE);
					}
					else
					{
						strncpy(Code, buf, CODE_SIZE);
						MemStkInfo *pInfo = g_pStockIO->Get_MemStkInfo(Code);
						if(pInfo) nSetCode = pInfo->setcode;
						else 
						{
						}
					}
				}
				//单股分析
				else if(m_Cfg.bSingle)
				{
					if(m_GpSet.nGpNum==1)
					{
						nSetCode = m_GpSet.GpSetInfo[0].setcode;
						strcpy(Code, m_GpSet.GpSetInfo[0].Code);
					}
					else
					{
						nSetCode = JbfxCurSetCode;
						strcpy(Code, JbfxCurGPCode);
					}
				}
				//多股缺省代码字段
				else if(!m_Cfg.bSingle&&strlen(m_Cfg.CodeColName)>0)
				{
					wantlen = MAX_LISTSTR_LEN-2;
					bSuccess=TD_GetStringData_(nClue,m_Cfg.CodeColName,buf,wantlen);
					if(_strnicmp(buf, "SH", 2)==0) 
					{
						nSetCode=1;
						strncpy(Code, &buf[2], CODE_SIZE);
					}
					else if(_strnicmp(buf, "SZ", 2)==0) 
					{
						nSetCode=0;
						strncpy(Code, &buf[2], CODE_SIZE);
					}
					else
					{
						strncpy(Code, buf, CODE_SIZE);
						MemStkInfo *pInfo = g_pStockIO->Get_MemStkInfo(Code);
						if(pInfo) nSetCode = pInfo->setcode;
						else 
						{
						}
					}
				}
				//计算
				CString strCalcFunc(m_Cfg.TitleCfg[j].OutFormula);
				if(strCalcFunc.CompareNoCase("@NAME")==0)		//填充名称
				{
					if(strlen(Code)>0&&m_Cfg.TitleCfg[j].OutType=='C')
					{
						MemStkInfo *pInfo = g_pStockIO->Get_MemStkInfo(Code);
						if(pInfo)
						{
							CString strRes(pInfo->Name);
							LPTSTR pstr = sc.stringdata.GetBufferSetLength(strRes.GetLength()+1);
							strcpy(pstr, (char*)(LPCSTR)strRes);
						}
					}
				}
				else if(strCalcFunc.CompareNoCase("@CODE")==0)	//填充代码
				{
					if(strlen(Code)>0&&m_Cfg.TitleCfg[j].OutType=='C')
					{
						MemStkInfo *pInfo = g_pStockIO->Get_MemStkInfo(Code);
						if(pInfo)
						{
							CString strRes(pInfo->Code);
							LPTSTR pstr = sc.stringdata.GetBufferSetLength(strRes.GetLength()+1);
							strcpy(pstr, (char*)(LPCSTR)strRes);
						}
					}
				}
				else
				{
					CWCalc.SetConfig(strCalcFunc, &m_Cfg.TitleCfg[j], nSetCode, Code);
					if(CWCalc.ParseCalcString())
					{
						float fRet = CWCalc.GetResult();
						switch(m_Cfg.TitleCfg[j].OutType) 
						{
						case 'D'://int
							sc.data.intdata = fRet;
							break;
						case 'F'://float
							sc.data.doubledata = fRet;
							break;
						case 'C'://string
							{
								CString strRes = FloatToStr(fRet);
								LPTSTR pstr = sc.stringdata.GetBufferSetLength(strRes.GetLength()+1);
								strcpy(pstr, (char*)(LPCSTR)strRes);
							}
							break;
						}
					}
					else
					{
						float fRet = 0.0f;
						switch(m_Cfg.TitleCfg[j].OutType) 
						{
						case 'D'://int
							sc.data.intdata = fRet;
							break;
						case 'F'://float
							sc.data.doubledata = fRet;
							break;
						case 'C'://string
							{
								LPTSTR pstr = sc.stringdata.GetBufferSetLength(1);
								strcpy(pstr, "");
							}
							break;
						}
					}
				}
			}

			sl.line_cell.push_back(sc);

		}
		
		if(i!= nCount-1)
			m_pList->AppendItem(sl,false);
		else
			m_pList->AppendItem(sl,true);
		m_pList->SetItemData(i,i);
    }

	OnRefreshChartFromList(0);	
}

//////////////////////////////////////////////////////////////////////////
BOOL	CCWTableUnit::SaveListHead()
{
	if(strlen(m_Cfg.GnIniFile)>0&&m_Cfg.OutNum>0)
	{
		CFile fp;
		CString dicpath = HomePath+(CString)"CWCache\\";
		CreateDirectory(dicpath, NULL);
		CString inifile = (CString)m_Cfg.GnIniFile;
		inifile.Replace(".ini", ".gdk");
		CString filepath = dicpath+inifile;
		if(fp.Open(filepath, CFile::modeCreate | CFile::modeWrite))
		{
			fp.Write(&m_Cfg.GnIniVer, sizeof(short));
			int nColNum = m_pList->GetHeadCount();
			for(int v=0;v<nColNum;v++)
			{
			}
			fp.Close();
			return TRUE;
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
void	CCWTableUnit::FillGPSet(GP_Set *pGs, short *pStkIndex, int nStkNum)
{
	pGs->nGpNum = 0;
	for(int i=0;i<nStkNum;i++)
	{
		MemStkInfo *pInfo = (*g_pStockIO)[pStkIndex[i]];
		if(pInfo)
		{
			pGs->GpSetInfo[pGs->nGpNum].setcode = pInfo->setcode;
			strcpy(pGs->GpSetInfo[pGs->nGpNum].Code, pInfo->Code);
			pGs->nGpNum++;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
int  CCWTableUnit::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_SIZE:
		UBase::ProcessMsg(message,wParam,lParam);
		ArrangeWnd(*this, FALSE);
		return 1;
	case UM_SET_STOCK:
		{
			UBase::ProcessMsg(message,wParam,lParam);
			if(m_pStkInfo) 
				SetGPCodeAndGetData(m_pStkInfo->setcode,m_pStkInfo->Code,m_pList && m_nClue>=0 && pSwitchBar && m_pTitleDlg);
		}
		break;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

//////////////////////////////////////////////////////////////////////////
//指挥Chart
void	CCWTableUnit::SetStaticDataCell(Chart_Cfg *pCfg,
											int nCol,
											 sg_cell sc,
											 std::vector<std::string>& vStaticXData, 
											 std::vector<std::vector<double> >& vStaticYData)
{
	char *strField = m_Cfg.TitleCfg[nCol].OutName;
	int nStaticNum = pCfg->StaticChart.ynames.size();
	//填充数据了
	//1.Static
	if(stricmp(strField, pCfg->StaticChart.xname.name.c_str())==0)
	{
		pCfg->StaticChart.xname.title = m_Cfg.TitleCfg[nCol].OutTitle;
		if(sc.stringdata.GetLength()>0) 
			vStaticXData.push_back((char*)(LPCSTR)FormatLongString(sc.stringdata, m_Cfg.TitleCfg[nCol].OutDataFormat));
		else vStaticXData.push_back("NULL");
	}
	for(int k=0;k<nStaticNum;k++)
	{
		if(stricmp(strField, pCfg->StaticChart.ynames[k].name.c_str())==0)
		{
			switch(m_Cfg.TitleCfg[nCol].OutType) 
			{
			case 'D':
				vStaticYData[k].push_back((double)sc.data.intdata);
				break;
			case 'F':
				vStaticYData[k].push_back(sc.data.doubledata);
				break;
			default:
				vStaticYData[k].push_back(0.0);
				break;
			}
		}
	}
}

void	CCWTableUnit::SetDynaDataCell(Chart_Cfg *pCfg,
										int nCol,
										   sg_cell sc,
										   std::vector<std::vector<double> >& vDynaData)
{
	char *strField = m_Cfg.TitleCfg[nCol].OutName;
	int nDynaNum = pCfg->DynaChart.DynaChartFields.size();
	//填充数据了
	//2.Dyna
	for(int k=0;k<nDynaNum;k++)
	{
		for(int p=0;p<pCfg->DynaChart.DynaChartFields[k].name.size();p++)
		{
			if(stricmp(strField, pCfg->DynaChart.DynaChartFields[k].name[p].c_str())==0)
			{
				switch(m_Cfg.TitleCfg[nCol].OutType) 
				{
				case 'D':
					vDynaData[k].push_back((double)sc.data.intdata);
					break;
				case 'F':
					vDynaData[k].push_back(sc.data.doubledata);
					break;
				default:
					vDynaData[k].push_back(0.0);
					break;
				}
				CString strTitle = m_Cfg.TitleCfg[nCol].OutTitle;
				CString strxTitle = m_Cfg.TitleCfg[nCol].OutTitle;
				int nzhi = strxTitle.Find("|", 0);
				if(nzhi>=0) strTitle = strxTitle.Right(strxTitle.GetLength()-nzhi-1);
				pCfg->DynaChart.DynaChartFields[k].vtitle[p] = (char*)(LPCSTR)strTitle;
			}
		}
	}
}

void	CCWTableUnit::SetStarDataCell(Chart_Cfg *pCfg,
										int nCol,
										   sg_cell sc,
										   std::vector<std::vector<double> >& vStarXData,
										   std::vector<std::vector<double> >& vStarYData,
										   std::vector<std::string> *vStarMainData)
{
	char *strField = m_Cfg.TitleCfg[nCol].OutName;
	int nStarNum = pCfg->StarChart.StarCharts.size();
	//填充数据了
	//3.Star
	for(int k=0;k<nStarNum;k++)
	{
		if(stricmp(strField, pCfg->StarChart.StarCharts[k].mainname.name.c_str())==0)
		{
			pCfg->StarChart.StarCharts[k].mainname.title = m_Cfg.TitleCfg[nCol].OutTitle;
			if(sc.stringdata.GetLength()>0)
				vStarMainData[k].push_back((char*)(LPCSTR)sc.stringdata);
			else vStarMainData[k].push_back("NULL");
		}
		if(stricmp(strField, pCfg->StarChart.StarCharts[k].xname.name.c_str())==0)
		{
			switch(m_Cfg.TitleCfg[nCol].OutType) 
			{
			case 'D':
				vStarXData[k].push_back((double)sc.data.intdata);
				break;
			case 'F':
				vStarXData[k].push_back(sc.data.doubledata);
				break;
			default:
				vStarXData[k].push_back(0.0);
				break;
			}
			CString strTitle = m_Cfg.TitleCfg[nCol].OutTitle;
			CString strxTitle = m_Cfg.TitleCfg[nCol].OutTitle;
			int nzhi = strxTitle.Find("|", 0);
			if(nzhi>=0) strTitle = strxTitle.Right(strxTitle.GetLength()-nzhi-1);
			pCfg->StarChart.StarCharts[k].xname.title = (char*)(LPCSTR)strTitle;
		}
		if(stricmp(strField, pCfg->StarChart.StarCharts[k].yname.name.c_str())==0)
		{
			switch(m_Cfg.TitleCfg[nCol].OutType) 
			{
			case 'D':
				vStarYData[k].push_back((double)sc.data.intdata);
				break;
			case 'F':
				vStarYData[k].push_back(sc.data.doubledata);
				break;
			default:
				vStarYData[k].push_back(0.0);
				break;
			}
			CString strTitle = m_Cfg.TitleCfg[nCol].OutTitle;
			CString stryTitle = m_Cfg.TitleCfg[nCol].OutTitle;
			int nzhi = stryTitle.Find("|", 0);
			if(nzhi>=0) strTitle = stryTitle.Right(stryTitle.GetLength()-nzhi-1);
			pCfg->StarChart.StarCharts[k].yname.title = (char*)(LPCSTR)strTitle;
		}
	}
}

void	CCWTableUnit::ProcessChartRefresh(VChart *pChart, Chart_Cfg *pCfg)
{
	//准备大量图的数据
	//1.Static
	std::vector<std::string> vStaticXData;
	std::vector<std::vector<double> > vStaticYData;
	int nStaticNum = pCfg->StaticChart.ynames.size();
	std::vector<double> StaticTmpData; StaticTmpData.empty();
	for(int i=0;i<nStaticNum;i++) vStaticYData.push_back(StaticTmpData);
	//2.Dyna
	std::vector<std::vector<double> > vDynaData;
	int nDynaNum = pCfg->DynaChart.DynaChartFields.size();
	std::vector<double> DynaTmpData; DynaTmpData.empty();
	for(i=0;i<nDynaNum;i++) vDynaData.push_back(DynaTmpData);
	//3.Star
	std::vector<std::vector<double> > vStarXData;
	std::vector<std::vector<double> > vStarYData;
	int nStarNum = pCfg->StarChart.StarCharts.size();
	std::vector<std::string> *vStarMainData = new std::vector<std::string>[nStarNum];
	for(i=0;i<nStarNum;i++) vStarMainData[i].clear();
	std::vector<double> StarXTmpData; StarXTmpData.empty();
	std::vector<double> StarYTmpData; StarYTmpData.empty();
	for(i=0;i<nStarNum;i++)
	{
		vStarXData.push_back(StarXTmpData);
		vStarYData.push_back(StarYTmpData);
	}

	int nCount = m_pList->GetItemCount();
	int nCurSel = m_pList->GetCurSel();
	nCurSel = max(nCurSel, 0);
	for(i=0;i<nCount;i++)
	{
		for(int j=0;j<m_Cfg.OutNum;j++)
		{
			sg_cell sc;
			m_pList->GetCellData(sc, j, i);

			SetStaticDataCell(pCfg, j, sc, vStaticXData, vStaticYData);
			if(i==nCurSel) 
				SetDynaDataCell(pCfg, j, sc, vDynaData);
			SetStarDataCell(pCfg, j, sc, vStarXData, vStarYData, vStarMainData);
		}
	}

	//图表
	pChart->SetStaticChart(pCfg->StaticChart);
	pChart->SetDynaChart(pCfg->DynaChart.DynaChartFields);
	pChart->SetStarChart(pCfg->StarChart.StarCharts);

	if(vStaticXData.size()>0)
	{
		pChart->SetStaticXData(vStaticXData);
		for(i=0;i<nStaticNum;i++)
		{
			if(vStaticYData[i].size()>0)
				pChart->SetStaticYData(i, vStaticYData[i],i==nStaticNum-1);
		}
	}

	for(i=0;i<nStarNum;i++)
	{
		if(vStarMainData[i].size()>0)
			pChart->SetStarMainData(i, vStarMainData[i]);
		vStarMainData[i].clear();
		if(vStarXData[i].size()>0)
			pChart->SetStarXData(i, vStarXData[i]);
		if(vStarYData[i].size()>0)
			pChart->SetStarYData(i, vStarYData[i],i==nStarNum-1);
	}
	
	for(i=0;i<nDynaNum;i++)
	{
		if(vDynaData[i].size()>0)
			pChart->SetDynaChartData(i, vDynaData[i],i==nDynaNum-1);
	}
	
	delete []vStarMainData;
}

void	CCWTableUnit::OnRefreshChartFromList(UINT uMsgID)
{
	if(!m_pList) return;

	for(int i=m_vRegChart.size()-1;i>=0;i--)
	{
		if(m_vRegChart[i].pChart)
		{
			if(m_vRegChart[i].bLinked)
			{
				ProcessChartRefresh(m_vRegChart[i].pChart, m_vRegChart[i].pCfg);
			}
			for(int j=0;j<20;j++)
			{
				if(m_vRegChart[i].uMsgID[j]==uMsgID)
				{
					ProcessChartRefresh(m_vRegChart[i].pChart, m_vRegChart[i].pCfg);
				}
			}
		}
	}
}
