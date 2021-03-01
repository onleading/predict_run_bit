#include "stdafx.h"
#include "AutoupInfo.h"

struct NeedAutoupInfo g_NeedAutoupInfo;

BOOL GetAutoupInfo(struct NeedAutoupInfo *pAutoupInfo)
{
	memset(pAutoupInfo,0,sizeof(struct NeedAutoupInfo));
	char  filename[MAX_PATH];
	sprintf(filename,"%stmp\\needautoup.dat",g_WSXHStr);
	FILE *fp = fopen(filename,"rb");
	if(!fp) return FALSE;
	fread(pAutoupInfo,1,sizeof(struct NeedAutoupInfo),fp);
	pAutoupInfo->AutoupURL[118]=0;
	pAutoupInfo->Descripton[2*1024-2]=0;
	fclose(fp);
	return TRUE;
}

void WriteAutoupInfo(struct NeedAutoupInfo *pAutoupInfo)
{
	char  filename[MAX_PATH];
	sprintf(filename,"%stmp\\needautoup.dat",g_WSXHStr);
	FILE *fp = fopen(filename,"wb");
	if(!fp) return;
	fwrite(pAutoupInfo,1,sizeof(struct NeedAutoupInfo),fp);
	fclose(fp);
}

#include "AutoupDlg.h" 
BOOL ShowAutoupInfo()
{
	BOOL bShow=FALSE;
	if(g_NeedAutoupInfo.bNeedAutoup>1)
	{
		g_GH.AWeekNoHint=FALSE;
		bShow=TRUE;
	}
	else if(g_NeedAutoupInfo.bNeedAutoup==1)
	{
		if(!g_GH.AWeekNoHint)
			bShow=TRUE;
		else if(g_GH.NoHintStartDate!=0)
		{
			CTime NowDate = CTime::GetCurrentTime();		
			CTime LastDate=LongToTime(g_GH.NoHintStartDate);
			CTimeSpan SpanTime=NowDate-LastDate;
			if(SpanTime.GetDays()>=7)
			{
				g_GH.AWeekNoHint=FALSE;
				bShow=TRUE;
			}
		}
	}
	if(bShow)
	{
		CAutoupDlg dlg(g_pMainWin);
		dlg.DoModal();
		return TRUE;
	}
	return FALSE;
}