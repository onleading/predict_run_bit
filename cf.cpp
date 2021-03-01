
#include "StdAfx.h"
#include "cf.h"
#include "Comview.h"

long CustomFunc::TrimStr(char *str)
{
	long i = 0;
	while(' '==str[i] || '\n'==str[i] || '\r'==str[i] || '\t'==str[i]) i++;
	if(i) memmove(str,str+i,strlen(str)+1-i);					//结尾'\0'也move过去
	i = strlen(str) - 1;
	while((i) && (' '==str[i] || '\n'==str[i] || '\r'==str[i] || '\t'==str[i])) str[i--] = 0;
	return strlen(str);
}

void CustomFunc::GetFormulaPack()
{
	memset(g_FmlPack,0,MAX_FMLPACK_NUM*sizeof(FMLPACK));
	g_nFmlPack = 0;
	if(g_pCalc == NULL) 
		return;

	CString strFilePath = g_WSXHStr+"PriPack.dat";
	//如果没有PriPack.dat,试试PriPack_def.dat系统缺省文件
	if(!TestFileExist(strFilePath))
		::CopyFile(g_WSXHStr+"PriPack_def.dat",strFilePath,FALSE);
	long lFileLen = 0,nPackNum = 0;
	int	 i = 0,j = 0;
	FILE * fpt = fopen(LPCSTR(strFilePath),"rb");
	if(fpt == NULL) return;
	lFileLen = on_filelength(fpt);
	if(lFileLen<=0 || lFileLen%sizeof(FMLPACK)!=0)
	{
		fclose(fpt);
		return;
	}
	nPackNum = lFileLen / sizeof(FMLPACK);
	if(nPackNum>MAX_FMLPACK_NUM) 
		nPackNum = MAX_BANDSTOCK_NUM;
	fseek(fpt,0,SEEK_SET);
	fread(g_FmlPack,nPackNum*sizeof(FMLPACK),1,fpt);
	fclose(fpt);
	for(i=0;i<nPackNum;i++)
	{
		g_FmlPack[i].aPackName[31] = '\0';
		if(TrimStr(g_FmlPack[i].aPackName)==0) continue;		//无名
		for(j=0;j<i;j++)
		{
			if(stricmp(g_FmlPack[i].aPackName,g_FmlPack[j].aPackName)==0) break;
		}
		if(j<i) continue;										//重名
		if(g_FmlPack[i].lRectNum<0 || g_FmlPack[i].lRectNum>10) continue;
		for(j=0;j<g_FmlPack[i].lRectNum;j++)
		{
			g_FmlPack[i].aFormular[j][31] = '\0';
			if(TrimStr(g_FmlPack[i].aFormular[j])==0)
			{
				if(j!=0) break;									//可以没有主图,但是副图缺就说不过去了
				else continue;
			}
			if(g_pCalc->_CallDTopViewCalc_8(ZB_OBJECT,g_FmlPack[i].aFormular[j])==NULL)
				break;
		}
		if(j<g_FmlPack[i].lRectNum) continue;
		if(g_nFmlPack<i)
			memcpy(&(g_FmlPack[g_nFmlPack]),&(g_FmlPack[i]),sizeof(FMLPACK));
		g_FmlPack[g_nFmlPack].nPeriod--;
		g_nFmlPack++;
	}
	if(g_nFmlPack<MAX_FMLPACK_NUM) memset(&(g_FmlPack[g_nFmlPack]),0,(MAX_FMLPACK_NUM-g_nFmlPack)*sizeof(FMLPACK));
	g_CF.SaveFormulaPack();
}

void CustomFunc::GetFormulaBandStock()
{
	memset(g_Fml2Stk,0,MAX_BANDSTOCK_NUM*sizeof(FML2STK));
	g_nFml2Stk = 0;

	CString strFilePath;
	FILE	*fpt	= NULL;
	long	lFileLen= 0,nBandStockNum = 0;
	int		i = 0,j = 0;
	strFilePath = g_WSXHStr+"PriBand.dat";
	fpt = fopen(LPCSTR(strFilePath),"rb");
	if(fpt == NULL) return;
	lFileLen = on_filelength(fpt);
	if(lFileLen<=0 || lFileLen%sizeof(FML2STK)!=0)
	{
		fclose(fpt);
		return;
	}
	nBandStockNum  = lFileLen / sizeof(FML2STK);
	if(nBandStockNum>MAX_BANDSTOCK_NUM) 
		nBandStockNum = MAX_BANDSTOCK_NUM;
	fseek(fpt,0,SEEK_SET);
	fread(g_Fml2Stk,nBandStockNum*sizeof(FML2STK),1,fpt);
	fclose(fpt);
	for(i=0;i<nBandStockNum;i++)
	{
		g_Fml2Stk[i].code[12] = '\0';
		if(TrimStr(g_Fml2Stk[i].code)==0) continue;				//无代码名
		g_Fml2Stk[i].aPackName[31] = '\0';
		if(TrimStr(g_Fml2Stk[i].aPackName)==0) continue;		//无模板名
		for(j=0;j<g_nFmlPack;j++)								//查找模板
		{
			if(stricmp(g_Fml2Stk[i].aPackName,g_FmlPack[j].aPackName)==0) break;
		}
		if(j==g_nFmlPack) continue;
		if(g_nFml2Stk<i)
			memcpy(&(g_Fml2Stk[g_nFml2Stk]),&(g_Fml2Stk[i]),sizeof(FML2STK));
		g_nFml2Stk++;
	}
	if(g_nFml2Stk<MAX_BANDSTOCK_NUM) 
		memset(&(g_Fml2Stk[g_nFml2Stk]),0,(MAX_BANDSTOCK_NUM-g_nFml2Stk)*sizeof(FML2STK));
	g_CF.SaveFormulaBandStock();
}

void CustomFunc::SaveFormulaPack()
{
	CString strFilePath;
	FILE	*fpt	= NULL;
	strFilePath = g_WSXHStr+"PriPack.dat";
	int i=0;
	fpt = fopen(LPCSTR(strFilePath),"wb");
	if(fpt == NULL) return;
	if(g_nFmlPack > 0)											//存个盘
	{
		for(i=0;i<g_nFmlPack;i++) g_FmlPack[i].nPeriod++;
		fwrite(g_FmlPack,g_nFmlPack*sizeof(FMLPACK),1,fpt);
		for(i=0;i<g_nFmlPack;i++) g_FmlPack[i].nPeriod--;
	}
	fclose(fpt);
}

void CustomFunc::SaveFormulaBandStock()
{
	CString strFilePath;
	FILE	*fpt	= NULL;
	strFilePath = g_WSXHStr+"PriBand.dat";
	fpt = fopen(LPCSTR(strFilePath),"wb");
	if(fpt == NULL) return;
	if(g_nFml2Stk > 0)
		fwrite(g_Fml2Stk,g_nFml2Stk*sizeof(FML2STK),1,fpt);
	fclose(fpt);
}

long  CustomFunc::GetWeek(long Date)
{
	char days_of_mon[12]  = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};	
	short  i,nYear,nMonth,nDay;
	long AllDays = 0;			// 从1980年至当日共有多少天
	nYear = Date / 10000;
	nMonth = (Date % 10000) / 100;
	nDay = (Date % 10000) % 100;
	if(nYear<1980 || nYear>4000 || nMonth<=0 || nMonth>12 || nDay<=0 || nDay>31) return (AllDays % 7);
	for(i = 1980;i < nYear;i++)
	{
		AllDays += 365;
		if(i % 400 == 0 || (i % 4 == 0 && i % 100 != 0)) AllDays++;
	}
	for(i = 1;i < nMonth;i++)
	{
		AllDays += days_of_mon[i-1];
		if(i == 2 && (nYear % 400 == 0 || (nYear % 4 ==0 && nYear % 100 != 0))) AllDays++;
	}
	AllDays += nDay + 1;		//1980年1月1日为星期二
	return (AllDays % 7);
}

long CustomFunc::DateToDay(long nDate)
{
	int year=1990,month=12,day=19;
	int date=0;
	int i=0;
	int days=0;
	int yeardays=365;
	int monthdays=31;
/////////////////////////////////////////////////////
	year=nDate/10000;
	month=(nDate%10000)/100;
	day=nDate%100;
//控制日期的有效性:1.时间跨度;2.月份的有效性;3.日数的有效性
	if(nDate<10101||nDate>30001231)	return 0;
	if(month<1||month>12)	return 0;
	if((month<8 && month%2==1) ||(month>7 && month%2==0))		monthdays=31;
	else if(month==4 || month==6 || month==9 || month==11)		monthdays=30;
	else if(month==2)
	{
		if((year%400==0)||(year%100!=0 && year%4==0)) monthdays=29;
		else monthdays=28;
	}
	if(day<1 || day>monthdays)	return 0;
//以年为单位
	for(i=1;i<year;i++)
	{
		if((i%400==0)||(i%100!=0 && i%4==0)) yeardays=366;
		else yeardays=365;
		days+=yeardays;
	}
//月	
	for(i=1;i<month;i++)
	{
		switch(i)
		{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:monthdays=31;break;
		case 2:
			{
				if((year%400==0)||(year%100!=0 && year%4==0)) monthdays=29;
				else monthdays=28;
				break;
			}
		case 4:
		case 6:
		case 9:
		case 11:monthdays=30;break;
		}
		days+=monthdays;
	}
	days+=day;
	return days;
}

long CustomFunc::DayToDate(long nDay)
{
	int year=1,month=1,day=1;
	int i=0,j=0;
	long nDate=19901219;
	int yeardays=365;
	if(nDay<=0||nDay>1095457) return 0;
//年内的单独算
	while(1)
	{
		if((year%400==0)||(year%100!=0 && year%4==0)) yeardays=366;
		else yeardays=365;
//年
		if(nDay>yeardays)
		{
			year++;
			nDay-=yeardays;
			continue;
		}
		break;
	}
//月日
	for(j=1;j<=12;j++)
	{
		switch(j)
		{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			if(nDay>31)
			{
				month++;
				nDay-=31;
			}
			else day=nDay;
			break;
		case 2:
			if(nDay>yeardays-337)
			{
				month++;
				nDay-=(yeardays-337);
			}
			else day=nDay;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			if(nDay>30)
			{
				month++;
				nDay-=30;
			}
			else day=nDay;
			break;
		}
		if(month<=j) break;
	}
	nDate = year*10000+month*100+day;
	return nDate;
}

long CustomFunc::DateOffset(long basedate,long offset,BOOL bBack)
{
	if(basedate<17000101) return 0;
	if(offset<0)
	{
		offset = -1*offset;
		bBack=!bBack;
	}
	if(offset>300000) return 0;
	long nDay = DateToDay(basedate);
	if(bBack) nDay-=offset;
	else nDay+=offset;
	return DayToDate(nDay);
}
