// QJReport.h: interface for the QJReport class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QJREPORT_H__EC2ED441_A0F5_11D3_B921_0000800104D5__INCLUDED_)
#define AFX_QJREPORT_H__EC2ED441_A0F5_11D3_B921_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define FLOAT_VALUE		0
#define FLOAT2_VALUE	1
#define VOLUME_VALUE	2
#define AMOUNT_VALUE	3

#include "Grid.h"

class QJReport: public Grid
{
	long		m_lQJBgnDate;				//起始日期
	long		m_lQJEndDate;				//结束日期

	struct AnalyData *pZsAnalyData;
	int		ZsAnalyDataNum;

	struct QJData_Struct
	{
		double	f1;
		int		f1Type;
		double	f2;
		int		f2Type;
	}fData[MAXGPNUM][11];				//区间数据结构

public:
	int			CurQJType;           //类型,不同区间分析表的标志字段
									 //为0表示区间分析之涨跌幅度比较表
									 //为1表示区间分析之换手率排名比较表
									 //为2表示区间分析之量变幅度动比较表
									 //为3表示区间分析之震荡幅度比较表
private:
	void	GetSomeSize(CDC *pDC);
	int		GetCellLeft(int nCol,int & nWidth);
	float   fGetSortValue(short xh);

	void	DrawTitle(CDC *pDC);
	void	DrawItem(CDC *pDC,int row,int col,int xsflag,MemStkInfo * scodetmp,CurrStockData * hqp);

	void    OnCtrlLeft();

	float	QJMax			(struct AnalyData  *pAnalyData,short nBgnNum,short nEndNum);	//求区间最高值的函数
	long	QJMaxDate		(struct AnalyData  *pAnalyData,short nBgnNum,short nEndNum);	//求区间最高值所在天数的函数
	float	QJMin			(struct AnalyData  *pAnalyData,short nBgnNum,short nEndNum);	//求区间最低值的函数
	long	QJMinDate		(struct AnalyData  *pAnalyData,short nBgnNum,short nEndNum);	//求区间最低值所在天数的函数
	int		GetColIndex(int col);	//得到实际栏目
public:
	QJReport();
	~QJReport();

	void Sleep();				//使这个报表睡眠
	void WakeUp();				//唤醒这个已经激活的报表

	//设置市场并填充lpnUsedStkIndex数组
	void	SetDomain(int nBlockFlag,int WhichType);
	void	SetQJType(int type);	//设定分析类型
	int		GetQJType(){return CurQJType;}
	void	SetDateSection(CTime date1,CTime date2);	//设置起讫日期
	CTime	GetDate(BOOL bFirst) 
	{
		if(bFirst) return(LongToTime(m_lQJBgnDate));
		else return(LongToTime(m_lQJEndDate));
	}

	//初始化报表，计算数据
	void	InitReport();
	void	GetData();
	void	Sort();
	//主画图函数
	void	DrawReport(CDC *pDC); 

	void	OnSize(UINT nType, int cx, int cy);
	void	OnClick(CDC *pDC,CPoint point,BOOL IsRight);
	void	OnLButtonDown(UINT nFlags, CPoint point);

	void	InsertSwitch(int nSCType);
	int		ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};

#endif // !defined(AFX_QJREPORT_H__EC2ED441_A0F5_11D3_B921_0000800104D5__INCLUDED_)