#include "StdAfx.h"
#include "hqcontrol.h"

//���ڷ��ƽ�
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
	//�˴��жϸ���Ȩ��,��Ҫ�������ƽ�
	g_GC.bIsSHLevel2=FALSE;
	g_GC.bIsSZLevel2=FALSE;
	g_GC.bIsSSDS=FALSE;
	g_GC.bHasKQSFunc=FALSE;

	//�жϸ��ֹ���Ȩ�޵�
	//Ȩ֤�����̿�
	if(strstr(g_strUserType,"SDS")>0)
		g_GC.bHasKQSFunc = TRUE;
	//ȱʡΪ�Ϻ�Level2
	if(strlen(g_strUserLimits)==0)
		g_GC.bIsSHLevel2=TRUE;
	else
	{
		//�Ϻ�Level2
		if(strstr(g_strUserLimits,"A")>0 || strstr(g_strUserLimits,"B")>0)
			g_GC.bIsSHLevel2=TRUE;
		//����Level2
		if(strstr(g_strUserLimits,"C")>0 || strstr(g_strUserLimits,"D")>0)
			g_GC.bIsSZLevel2=TRUE;		
		//ʵʱ�۹�
		if(strstr(g_strUserLimits,"H")>0)
			g_GC.bIsSSDS = TRUE;
		//Ȩ֤�����̿�
		if(strstr(g_strUserLimits,"S")>0)
			g_GC.bHasKQSFunc = TRUE;
	}
	//ʯ�ǲƾ��Ķ��ư�
#ifdef CITY10
	if(strstr(g_strUserType,"����")<=0)
		return FALSE;
#endif
	return TRUE;
}

//20����������8�Σ�˵�����粻�ȶ�
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
