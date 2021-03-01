#ifndef UHycj_H_
#define UHycj_H_

//L2��ҵ�ɽ�����
#include "UBase.h"

class UHycj : public UBase
{
	BOOL	m_bEmpty;
	int		m_nZoomFlag;//0:�޷Ŵ�,1:�ϲ��ַŴ� 2:�²��ַŴ�
	short	m_setcode;
	//Զ�̲�����
	BOOL	m_bDataReady;
	CRect	m_HyRect[10],m_HyGPRect[5];
	int		m_nCurHy;
private:
	void DrawPart1(CDC *pDC,int TotalHeight,int TextH,int XWidth,int XFrom[],int YFrom);
	void DrawPart2(CDC *pDC,int TotalHeight,int TextH,int XWidth,int XFrom[],int YFrom);
public:
	UHycj(LPCOMINFO pComInfo);
	~UHycj();

	void ReadData(BOOL bDataReady=TRUE,BOOL bReqData=TRUE);
	void DrawIt(CDC *pDC);

	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};


#endif
