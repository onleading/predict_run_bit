/*
* Copyright (c) 2002,通达信开发部
* All rights reserved.
* 
* 文件名称：UCb.h
* 文件标识：
* 摘    要：完成移动筹码、火焰山功能.
*/

#ifndef UCB_H_
#define UCB_H_

#ifdef _DEBUG
#pragma warning(disable:4786) 
#endif

#include "Ubase.h"


const	int	NPRICE	= 1000;
const	int	CACHEA	= 100;
const	int	MAX_MON = 6;
const	int	CHNUM	= 50;
struct	POINTCOST					//分别处理不同周期的成本图
{	
	int					N;			//参数。
	double			*	pTabVol;	//按照不同梯度分配，存放对应价位的成交量。
	double			*	pDrawVol;	//用来画的
	double				dCost;		//占的成本。
	POINTCOST()
	{
		N = 0;
		pTabVol = new double[NPRICE];
		pDrawVol= new double[NPRICE];
		memset(pTabVol,0,NPRICE*sizeof(double));
		memset(pDrawVol,0,NPRICE*sizeof(double));
	}
	void	Reset()
	{
		memset(pDrawVol,0,NPRICE*sizeof(double));
		memset(pTabVol,0,NPRICE*sizeof(double));
		dCost = 0;
	}
	void	ResetDraw()
	{
		memset(pTabVol,0,NPRICE*sizeof(double));
		memset(pDrawVol,0,NPRICE*sizeof(double));
	}
	~POINTCOST()
	{
		if ( pTabVol )
			delete [] pTabVol;
		if ( pDrawVol )
			delete [] pDrawVol;
	}
};
enum	COSTWAY				//用来均匀成本的方法
{
	AVERAGE,				//平均
	TRIANGLE,				//▲
	NORMAL,					//正态
};
struct	ENVIORSETUP			//环境设置
{
	float		fWeakK;			//历史换手衰减系数。//衰减系数 0.1 ~ 10
	COSTWAY		Way;			//当日成本算法
	BOOL		IsPrecious;		//是否精确计算
	float		fdHSL;			//缺省欢送
	float		fJZD;			//集中度
	short		nLine;			//多少线
	short		nSkip[2];			//多少个筹码叠加图
	short		aN[2*MAX_MON];	//有固定上限
	DWORD		reserve[3];		//
};
struct	DISPINFO
{
	long		Date;			//显示日期
	double		dGotRate;		//获利比例:以收市价为参考点
	float		fPrice;			//每个纵向价格
	double		dGot;			//每个纵向价格的获利盘
	float		dPJPrice;		//50%筹码处的价格
	float		p90[2];			//90%筹码的两个价格
	double		j90;			//90%筹码的集中度
	float		p70[2];
	double		j70;
};
struct	COSTFILE
{
	long		num;		//多少个double
	DWORD		offset;		//文件开始处
	DWORD		end;		//文件
};

class UCb : public UBase
{
protected:
	int						m_nAmple;			//放大比例，可调(因为基金太小)
	CRect					m_aRect[4];			//四个可以点击的区域
	double					m_dMaxVol;			//最大成交量
	int						m_FxBottom,m_FxTop;
	float				*	m_fLTGB;
	struct AnalyData	*	m_pData;			//数据源
	short					m_nDataNum;			//
	short					m_anCache[CHNUM];	//缓存到第几天
	double	*				m_aVolCache[CHNUM];	//为了减少计算量而保存
	double	*				m_aVolNow;			//直接使用的
	double	*				m_aDrawVol;			//画图时截取的部分筹码分布表	
	POINTCOST				m_aInCost[MAX_MON];	//×日内的成本
	POINTCOST				m_aOutCost[MAX_MON];//×日前的成本
	POINTCOST			*	m_pIn[MAX_MON];		//排序好的序列
	POINTCOST			*	m_pOut[MAX_MON];
	ENVIORSETUP				m_Setup;		
	long					m_nMinPrice;	//保存移动筹码分布最低价
	long					m_nMaxPrice;    //保存移动筹码分布最高价
	double					m_dMaxPrice;	//K线图中的最值
	double					m_dMinPrice;	//K线图中的最值
	long					m_nPGap;		//价格跳跃
	long					m_nDotSpan;		//拉开间隔。
	DISPINFO				m_DispInfo;		//显示信息
	BOOL					m_bValid;		//如果是指数，把进行过的变为无效
	int						m_nCurr;		//当前时间点(如果数据变化，立刻废除)
	POINT					m_nGivePt;		//定位Y向用。
	short					m_nSwitch;		// 0 -普通 1－日前  2－日内
	double					m_dRatio;		////y =Bottom - [(Bottom - YC)/(Close-dMin)]*(P-dMin);
	double					m_dPRatio;		////P = i * (dMax-dMin)/nNewSize + dMin;
	float					m_fCBPrice;		//50%成本均价
public:
	UCb(LPCOMINFO pComInfo);
	virtual	~UCb();
	void	ReadData();
	void	DrawEx(CDC *pDC);
	int		ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
private:
	//工具函数
	inline	void	DispInfo(CDC * pDC);
	inline	void	DrawIt(CDC *pDC);
	void	DeleteIt();
	void	Reset();
	void	GetGotLostRate();
	void	BeforeCalc();
	//如果Cache不好，重新计算
	void	MainCalc();
	//针对每个计算
	void	CalcEach();
	//平均成交量的方法
	inline	void	GetPerVol(double * pVol,double	dVol,long P1,long P2);
	inline	void	SubPerVol(double * pVol,double dVol,long H,long L);
	inline	int		ReadCache();
	inline	void	WriteCache(int	nPosNo);
	inline	double	GetHLP(float fPrice);
	void	LoadSetup();
	void	SaveSetup();
	double	GetHSL(int no);
	inline	void	SortIt();
};


#endif