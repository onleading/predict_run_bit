#include "stdafx.h"
#include "treeid.h"
#include "resource.h"
#include "MainFrm.h"

struct TreeID_Map g_pTreeIDMap[]=
{
	//直接的Command
	{"HQ",			CMD_DIRECT,	ID_SHOWHQ},				//显示行情
	{"JY",			CMD_DIRECT,	ID_FUNC_WT},			//弹出交易
	{"ZX",			CMD_DIRECT,	IDC_ZXTREE},			//显示资讯树
	{"HOTBLOCK",	CMD_DIRECT,	ID_SHOWHISTORY},		//热门板块分析
	{"SETZXG",		CMD_DIRECT,	ID_BLOCKOPTION},		//板块股操作
	{"MYPAD",		CMD_DIRECT,	IDC_RECENTPAD},			//最近的定制版面
	{"HK",			CMD_DIRECT,	IDM_DS1+31},			//显示港股
	{"QH",			CMD_DIRECT,	IDM_DS1+30},			//显示期货
	{"QZSDS",		CMD_DIRECT,	IDC_QZSDS},				//闪电手
	{"SORT",		CMD_DIRECT,	IDC_6787},				//排行(67+87)
	{"PAD1",		CMD_DIRECT,	IDC_SPEC_CUSTOMPAD1},	//第一个特定版面
	{"PAD2",		CMD_DIRECT,	IDC_SPEC_CUSTOMPAD2},	//第二个特定版面
	{"URGENT",		CMD_DIRECT,	ID_URGENT},				//紧急通告对话框
	{"JJREPORT",	CMD_DIRECT,	ID_JJREPORT},			//基金分析平台
	
	{"SORT67",			CMD_HQ,		67},				//排行(67)
	{"SORT87",			CMD_HQ,		87},				//排行(87)
	//行情窗口
	{"AG",			CMD_HQ,		TYPE_AG+1},				//A股列表
	{"BG",			CMD_HQ,		TYPE_BG+1},				//B股列表
	{"ZXB",			CMD_HQ,		TYPE_ZXB+1},			//中小板列表
	{"CYB",			CMD_HQ,		TYPE_CYB+1},			//创业板列表
	{"QZ",			CMD_HQ,		TYPE_QZ+1},				//权证列表
	{"ZXG",			CMD_HQ,		TYPE_ZXG+1},			//自选股列表	
	{"BLOCKZS",		CMD_HQ,		TYPE_SPECINDEX+1},		//板块指数
	

	//分析窗口
	{"F3",			CMD_COM,	102},					//上证大盘
	{"F4",			CMD_COM,	108},					//深证大盘
	
	//交易对话框
	{"REGUSER",		CMD_TCDLG,	DLG_REGUSER},			//试用用户注册
	{"ACTIVEUSER",	CMD_TCDLG,	DLG_ACTIVEUSER},		//激活试用用户
	{"UPGRADEUSER",	CMD_TCDLG,	DLG_UPGRADEUSER},		//提升级别
	{"LEAVEWORD",	CMD_TCDLG,	DLG_LEAVEWORD},			//显示留言信息
	//
	{"",		0,			0}
};

void CarryOut_WebInfoTreeID(const char *pInfo)
{
	int companyno=0,infotreeid=0,daynum=0;
	//
	char tempContent[255]={0};
	ParseMessageStr(pInfo,tempContent,"_",2);
	companyno=atol(tempContent);
	ParseMessageStr(pInfo,tempContent,"_",3);
	infotreeid=atol(tempContent);
	ParseMessageStr(pInfo,tempContent,"_",4);
	daynum=atol(tempContent);
	
	//打开标题区
	SwitchToView("Cjzx");
	DWORD tmpItemData = infotreeid*100+companyno;
	theViewManager.pActiveView->SendMessage(UM_CJZX_DAY_MESSAGE,tmpItemData,daynum);
}

//执行函数
void TreeID_CarryOut(const char *pIDStr)
{
	CMainFrame *pMainWnd = (CMainFrame *)AfxGetMainWnd();
	if(strnicmp(pIDStr,"info_",5)==0)
	{
		CarryOut_WebInfoTreeID(pIDStr);
		return;
	}
	else if(strnicmp(pIDStr,"gpcode_",7)==0)
	{
		MemStkInfo *tmpInfo=g_pStockIO->Get_MemStkInfo(pIDStr+7);
		if(tmpInfo)	ToZst(tmpInfo->Code,tmpInfo->setcode,FALSE);	
		return;
	}
	else if(strnicmp(pIDStr,"cmdid_",6)==0)
	{
		UINT nID = atol(pIDStr+6);
		pMainWnd->SendMessage(WM_COMMAND, nID, 0);
		return;
	}
	else if(strnicmp(pIDStr,"ietitle/",8)==0)	//对话框打开 http://www.treeid/ietitle/标题http://网址
	{
		CString tmpLine = pIDStr+8;
		int nHttpPos = tmpLine.Find("http");
		if(nHttpPos>0)
		{
			CString tmpIWTitle=tmpLine.Left(nHttpPos);
			char tmpUrlCode[255]={0};
			UrlDecode(tmpIWTitle,tmpUrlCode);
			g_strIWTitle = tmpUrlCode;
			//
			g_strIWURL=tmpLine.Mid(nHttpPos);
			if(pMainWnd)
				pMainWnd->PostMessage(UM_OPENIWCONTENT,1,0);
		}
		return;
	}	
	else if(strnicmp(pIDStr,"http",4)==0)	//外部打开	http://www.treeid/http://.......
	{
		OpenCusWebPage(g_pMainWin,pIDStr,FALSE);
		return;
	}
	int i=0;
	while(strlen(g_pTreeIDMap[i].IDStr)>0)
	{
		if(stricmp(pIDStr,g_pTreeIDMap[i].IDStr)==0)
		{
			switch(g_pTreeIDMap[i].CommandType)
			{
			case CMD_DIRECT:	//直接的Command
				{
					if(g_pTreeIDMap[i].CommandID==IDC_ZXTREE && g_OEM.bOutStandZXGHY)
					{
						OpenCusWebPage(g_pMainWin,g_OEM.strWebButtonURL,g_OEM.bMainWebPageInsideOpen);
						return;
					}
					DWORD tmpID = g_pTreeIDMap[i].CommandID;
					if(tmpID==IDC_SPEC_CUSTOMPAD1)	//特别的第一主页,交易区要弹出来
					{
						tmpID = IDC_CUSTOMPAD1;
						CMainFrame *pMainWnd = (CMainFrame *)AfxGetMainWnd();
						pMainWnd->ShowTabCtrlBar(0,FALSE);
						DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
						if(dwStatus&JY_LOGINED)
							pMainWnd->SwitchToRestSpace(AREA_WT);
					}
					if(tmpID==IDC_SPEC_CUSTOMPAD2)	//特别的第二主页,交易区要隐藏
					{
						tmpID = IDC_CUSTOMPAD2;
						CMainFrame *pMainWnd = (CMainFrame *)AfxGetMainWnd();
						pMainWnd->ShowTabCtrlBar(0,FALSE);
						pMainWnd->ShowTabCtrlBar(1,FALSE);
					}
					CMainFrame *pMainWnd = (CMainFrame *)AfxGetMainWnd();
					if(pMainWnd)
						pMainWnd->PostMessage(WM_COMMAND,tmpID);
				}
				break;
			case CMD_MAIN:		//主窗口
			case CMD_HQ:		//行情窗口
			case CMD_COM:		//分析窗口
			case CMD_JBM:		//基本资料窗口
			case CMD_NEWS:		//公告消息窗口
			case CMD_HIS:		//历史分析窗口
			case CMD_ADVHQ:		//高级行情窗口
			case CMD_JBFX:		//基本分析窗口
			case CMD_JJ:		//基金分析窗口
				{
					CMainFrame *pMainWnd = (CMainFrame *)AfxGetMainWnd();
					if(pMainWnd)
						pMainWnd->OnCommCommand(g_pTreeIDMap[i].CommandType,g_pTreeIDMap[i].CommandID);
				}
				break;
			case CMD_TCDLG:		//交易对话框
				if(TC_GetDlg_)
					TC_GetDlg_(g_pTreeIDMap[i].CommandID);
				break;
			}
			break;
		}
		i++;
	}
}

//IE控件的内部特别链接处理
BOOL BeforeNavigate_Process(char *url)
{
	if(memicmp(url,"HTTP://WWW.ZHIYINGID/",21)==0)
	{
		TreeID_CarryOut(url+21);
		return TRUE;
	}
	else if(memicmp(url,"HTTP://ZHIYINGID/",17)==0)
	{
		TreeID_CarryOut(url+17);
		return TRUE;
	}
	else if(memicmp(url,"HTTP://WWW.TREEID/",18)==0)
	{
		TreeID_CarryOut(url+18);
		return TRUE;
	}
	else if(memicmp(url,"HTTP://TREEID/",14)==0)
	{
		TreeID_CarryOut(url+14);
		return TRUE;
	}
	return FALSE;	
}
