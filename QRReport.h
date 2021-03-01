// QRReport.h: interface for the QRReport class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QRREPORT_H__EC2ED441_A0F5_11D3_B921_0000800104D5__INCLUDED_)
#define AFX_QRREPORT_H__EC2ED441_A0F5_11D3_B921_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Grid.h"

class QRReport : public Grid
{
	long	m_lNowDate;			//最近有分析数据的日期

	float	fData[MAXGPNUM][12];	//7个常规栏目+5个历史数据
	float	fBaseData[12];		//基准的数据

	int		BaseGPNo;			//基准股票的序号，为-1表示没有基准
	int		OldBaseGPNo;		//上一次基准股票的序号，为-1表示没有基准
								//OldBaseGPNo主要用于恢复fData数组
private:
	void	GetSomeSize(CDC *pDC);
	int		GetCellLeft(int nCol,int & nWidth);
	float   fGetSortValue(short xh);
	void	DrawTitle(CDC *pDC);
	void	DrawItem(CDC *pDC,int row,int col,int xsflag,MemStkInfo * scodetmp,CurrStockData * hqp);
	void    OnCtrlLeft();
public:
	QRReport();

	void	Sleep();			//使这个报表睡眠
	void	WakeUp();			//唤醒这个已经激活的报表

	//设置市场并填充lpnUsedStkIndex数组
	void	SetDomain(int nBlockFlag,int WhichType);

	void	SetBase(int GPNo);
	void	SetItBase();

	void	InitReport();		//初始化报表，计算数据
	void	BaseCalc();
	void	GetSingleGPData(int GPNo,float (*Data)[12]);
	void	GetData();
	void	Sort();
	
	void	DrawReport(CDC *pDC); //主画图函数

	void	OnSize(UINT nType, int cx, int cy);
	void	OnClick(CDC *pDC,CPoint point,BOOL IsRight);
	void	OnLButtonDown(UINT nFlags, CPoint point);

	void	InsertSwitch(int nSCType);
	int		ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};

#endif // !defined(AFX_QRREPORT_H__EC2ED441_A0F5_11D3_B921_0000800104D5__INCLUDED_)
