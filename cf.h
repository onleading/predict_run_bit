#ifndef __TDXW__CF__H__
#define __TDXW__CF__H__

class CustomFunc
{
public:
	long TrimStr(char *str);
	void GetFormulaPack();
	void GetFormulaBandStock();
	void SaveFormulaPack();
	void SaveFormulaBandStock();
	long GetWeek(long Date);												//函数返回有效值范围为(0-6)，0表示星期天
	long DateToDay(long nDate);												//日期转换成耶稣基督存活的天数(00010101为1)
	long DayToDate(long nDay);												//耶稣基督存活的天数转换成日期(1为00010101)
	long DateOffset(long basedate,long offset,BOOL bBack);					//对指定日期进行偏移,参数3:是否向历史偏移(TRUE=是)
};

#endif