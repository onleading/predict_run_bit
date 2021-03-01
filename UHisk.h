#ifndef UHIS_H_
#define UHIS_H_

#include "Ubase.h"

//历史分时右侧"数值"

class UHisk : public UBase
{
	float ActiveCapital; //与StkInfo中一样，没有X10000.0
	float TotalCapital;
public:
	UHisk(LPCOMINFO pComInfo);
	~UHisk();

	void ReadData();
	void DrawIt(CDC *pDC);
	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};


#endif