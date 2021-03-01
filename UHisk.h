#ifndef UHIS_H_
#define UHIS_H_

#include "Ubase.h"

//��ʷ��ʱ�Ҳ�"��ֵ"

class UHisk : public UBase
{
	float ActiveCapital; //��StkInfo��һ����û��X10000.0
	float TotalCapital;
public:
	UHisk(LPCOMINFO pComInfo);
	~UHisk();

	void ReadData();
	void DrawIt(CDC *pDC);
	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};


#endif