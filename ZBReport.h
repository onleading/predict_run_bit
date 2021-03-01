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
	long		m_lNowDate;				//最近有分析数据的日期
	float		fData[MAXGPNUM][32];	//8个常规栏目+6个输出线*4

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

	void Sleep();				//使这个报表睡眠
	void WakeUp();				//唤醒这个已经激活的报表

	//设置市场并填充lpnUsedStkIndex数组
	void SetDomain(int nBlockFlag,int WhichType);
	//设置处理日期
	void SetDate(int Flag);				
	void SetDate(int Flag,CTime date);
	//初始化报表，计算数据
	void InitReport();
	void GetData();
	void Sort();

	//主画图函数
	void DrawReport(CDC *pDC);  

	CTime GetDate(){ return(LongToTime(m_lNowDate)); }

	void OnSize(UINT nType, int cx, int cy);
	void OnClick(CDC *pDC,CPoint point,BOOL IsRight);
	void OnLButtonDown(UINT nFlags, CPoint point);

	void InsertSwitch(int nSCType);
	int	 ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};

#endif // !defined(AFX_ZBREPORT_H__EC2ED441_A0F5_11D3_B921_0000800104D5__INCLUDED_)
