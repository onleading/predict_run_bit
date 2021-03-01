#include "StdAfx.h"
#include "hqcontrol.h"

//用于反破解
extern HANDLE UserThreadHandle;
BOOL HQControl::IsValidProgram1()
{
	return TRUE;
}

BOOL HQControl::IsValidProgram2()
{
	return TRUE;
}

BOOL HQControl::JudgeAdvHQRights()
{
	//此处判断各种权限,需要防各种破解
	g_GC.bIsSHLevel2=FALSE;
	g_GC.bIsSZLevel2=FALSE;
	g_GC.bIsSSDS=FALSE;
	g_GC.bHasKQSFunc=FALSE;

	//判断各种功能权限等
	//权证高速盘口
	if(strstr(g_strUserType,"SDS")>0)
		g_GC.bHasKQSFunc = TRUE;
	//缺省为上海Level2
	if(strlen(g_strUserLimits)==0)
		g_GC.bIsSHLevel2=TRUE;
	else
	{
		//上海Level2
		if(strstr(g_strUserLimits,"A")>0 || strstr(g_strUserLimits,"B")>0)
			g_GC.bIsSHLevel2=TRUE;
		//深圳Level2
		if(strstr(g_strUserLimits,"C")>0 || strstr(g_strUserLimits,"D")>0)
			g_GC.bIsSZLevel2=TRUE;		
		//实时港股
		if(strstr(g_strUserLimits,"H")>0)
			g_GC.bIsSSDS = TRUE;
		//权证高速盘口
		if(strstr(g_strUserLimits,"S")>0)
			g_GC.bHasKQSFunc = TRUE;
	}
	//石城财经的定制版
#ifdef CITY10
	if(strstr(g_strUserType,"机构")<=0)
		return FALSE;
#endif
	return TRUE;
}

//20秒内重连了8次，说明网络不稳定
long HQControl::UnStable_LastTime = 0;
long HQControl::UnStable_Cnt = 0;
BOOL HQControl::TestUnStableNetWorkStatus()
{
	if(UnStable_LastTime==0)
		UnStable_LastTime=time(NULL);
	if((time(NULL)-UnStable_LastTime)<20 && UnStable_Cnt>8)
		return TRUE;
	UnStable_Cnt++;
	if((time(NULL)-UnStable_LastTime)>20)
	{
		UnStable_LastTime = 0;
		UnStable_Cnt = 0;
	}
	return FALSE;
}
