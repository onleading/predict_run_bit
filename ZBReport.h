// ZBReport.h: interface for the ZBReport class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZBREPORT_H__EC2ED441_A0F5_11D3_B921_0000800104D5__INCLUDED_)
#define AFX_ZBREPORT_H__EC2ED441_A0F5_11D3_B921_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Grid.h"

class ZBReport : public Grid
{
	long		m_lNowDate;				//����з������ݵ�����
	float		fData[MAXGPNUM][32];	//8��������Ŀ+6�������*4

	struct AnalyData *pZsAnalyData;
	int		ZsAnalyDataNum;
private:
	int   GetCellLeft(int nCol,int & nWidth);
	float fGetSortValue(short xh);
	void  GetSomeSize(CDC *pDC);

	void  DrawTitle(CDC *pDC);
	void  DrawItem(CDC *pDC,int row,int col,int xsflag,MemStkInfo * scodetmp,CurrStockData * hqp);
	void  OnCtrlLeft();
public:
	ZBReport();
	~ZBReport();

	void Sleep();				//ʹ�������˯��
	void WakeUp();				//��������Ѿ�����ı���

	//�����г������lpnUsedStkIndex����
	void SetDomain(int nBlockFlag,int WhichType);
	//���ô�������
	void SetDate(int Flag);				
	void SetDate(int Flag,CTime date);
	//��ʼ��������������
	void InitReport();
	void GetData();
	void Sort();

	//����ͼ����
	void DrawReport(CDC *pDC);  

	CTime GetDate(){ return(LongToTime(m_lNowDate)); }

	void OnSize(UINT nType, int cx, int cy);
	void OnClick(CDC *pDC,CPoint point,BOOL IsRight);
	void OnLButtonDown(UINT nFlags, CPoint point);

	void InsertSwitch(int nSCType);
	int	 ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};

#endif // !defined(AFX_ZBREPORT_H__EC2ED441_A0F5_11D3_B921_0000800104D5__INCLUDED_)
