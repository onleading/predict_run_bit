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
	long GetWeek(long Date);												//����������Чֵ��ΧΪ(0-6)��0��ʾ������
	long DateToDay(long nDate);												//����ת����Ү�ջ�����������(00010101Ϊ1)
	long DayToDate(long nDay);												//Ү�ջ�����������ת��������(1Ϊ00010101)
	long DateOffset(long basedate,long offset,BOOL bBack);					//��ָ�����ڽ���ƫ��,����3:�Ƿ�����ʷƫ��(TRUE=��)
};

#endif