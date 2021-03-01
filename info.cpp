#include "stdafx.h"
#include "info.h"
/////////////////////////////////////////////////////////////////////////////
//					资讯工场
/////////////////////////////////////////////////////////////////////////////

struct iw_client_title	g_SwapIWTitle[4][MAX_ONE_IWTITLE_NUM];
long					g_nSwapIWTitleNum[4] = {0};
long					g_nSwapIWTitleTotalNum[4] = {0};
CString					g_strIWTitle="";
CString					g_strIWURL="";
long					g_nObserverACKCnt = 0;

struct iw_client_title	g_ObserverRollInfo[MAX_OBSERVER_ROLL_NUM];
long					g_ObserverRollInfoNum=0;

char					g_strIwContentPath[MAX_PATH]={0};

//读取资讯工场相关配置
void LoadIwConfig()
{
	GetPrivateProfileString("TdxRun","IwContentPath","",g_strIwContentPath,MAX_PATH-1,HomePath+"TdxRun.dat");		
}

//构造资讯工场某些信息的链接
CString GetIWURL(iw_client_title *pTitle)
{
	CString strLink;
	switch(pTitle->info_format)
	{
	case 0:
		strLink.Format("%s%d/%d.txt",g_strIwContentPath,pTitle->time_ymd,pTitle->rec_id);
		break;
	case 1:
		strLink.Format("%s%d/%d.html",g_strIwContentPath,pTitle->time_ymd,pTitle->rec_id);
		break;
	case 2:
		strLink.Format("%s%d/%d.mht",g_strIwContentPath,pTitle->time_ymd,pTitle->rec_id);
		break;
	default:
		if(strstr(pTitle->info_url, "http://"))
			strLink.Format("%s", pTitle->info_url);
		else
			strLink.Format("http://%s", pTitle->info_url);
		break;
	}
	return strLink;
}