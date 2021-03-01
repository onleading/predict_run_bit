// MyStrategy.h: interface for the MyStrategy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYSTRATEGY_H__D3FBB36B_1683_4F32_A108_3D297934DA99__INCLUDED_)
#define AFX_MYSTRATEGY_H__D3FBB36B_1683_4F32_A108_3D297934DA99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)
#pragma warning(disable:4788)
#include <map>
#include <algorithm>
#include <functional>
using namespace std;

#define MAX_NEED_DATA	2
#define MAX_TYPE		10

enum	CalcType
{
	ZB_SEL=0,
	TJ_SEL,
	CWHQ_SEL,
	KL_SEL,
	STYLE_SEL,
	DS_SEL,

	ZB_TYPESEL=10000,
	TJ_TYPESEL,
};

enum	LineType              //线型
{
	K_LINE=0,                 //K线
    CALC_LINE,                //指标条件等公式计算线
	STATIC_LINE,              //静态数据线对应基本信息行情数据
	DS_LINE,                  //内置计算
	NUM_LINE                  //单一数字
};

struct CalcLine               //计算的线
{
	short lineType;           //线型
	short lineIndex;          //线的索引
	float lineValue;          //数值，缺省为0

	Serialize(CArchive &ar);
};

struct SingleCon                   //单一条件
{
	int  mainType;                 //指标类型:(指标:0，条件:1，行情财务:2，K线:3，定式:4)
	int  indexNo;                  //指标系统索引
	short procIndex;               //处理函数索引,无处理函数则为0
	CalcLine mainLine;             //主线
	vector<CalcLine> otherLines;   //其他线
	long endDate;                  //选股时间

	CString sList;				   //ListBox里的说明

	Serialize(CArchive &ar);
};

struct Plans
{
	vector<SingleCon> myCons;          //条件组
	BOOL				IsAndRatio;		//
	CString           sDescript;       //条件说明
};

struct SignStrategy            //条件信号策略：若是交易系统则派生出2个
{
	UINT  pairConIndex;        //对应条件索引
	BOOL  bBuy;                //0:卖出 1:买入
	short typeDo;              //处理方式 0:不再买卖 1:同等买卖 2:按比例买卖
	long  lCount;              //固定买入卖出股数或比例数
	int   nCancelData;         //略过连续信号周期数:缺省为1
};

struct KStrategy              //止损策略
{
	short procIndex;          //处理函数索引
	int   nData;              //周期数
	short nBreadth;           //止损幅度
};

struct TestStrategy
{
	Plans                   myPlans;   //条件组
	vector<SignStrategy>    sighStra;  //信号发出时的对应买卖策略
	vector<KStrategy>       ks;        //止损策略组
};

class MyStrategy  
{
public:
	MyStrategy();
	virtual ~MyStrategy();
	
	CTime ParseCalcDate(void);   
	void CalcEndGap(void);                               //得到选股日期距当前交易周期数
	void InitAllData(void);
	void UnInitAllData(void);
	BOOL GetMyAnalData(void);
	void AddSortCon(int iSort);
	int  GetListStrings(vector<CString> &aListStrings);  //得到ListStrings
	BOOL CompCurCon(CString &tmpStr);					 //比较添加的条件是否重复
	void	PutPlanScript(CString &planScript);			 //存放方案描述
	CString GetPlanScript(void);                         //得到方案描述
	void SLPlan(CArchive &ar, BOOL bUseNewTime = TRUE);
	BOOL IsSort(void);                                   //判断是否有排序条件

	void SetTQFlag(BOOL bTQ);
	BOOL HasSortCon(void);
	void ParseType(void);						//判断该策略是否包含某类条件
	BOOL GetConsInfo(void);                  //判断排序和非排序条件
	void SetCurGpIndex(int curGpIndex);
	void InputCalcDataTime(NTime &time1,NTime &time2, int nPeriod);
	void AddSingleCon(SingleCon &conToAdd);
	void SetIsAndRatio(BOOL bIsAndRatio);
	void DelSingleCon(UINT nToDel);
	BOOL ParsCurCon(int nCurCon);                  
	int  FiltSortRes(int IsAndRadio, int TotalStockNum, int SuccessNum, short *SelectedStk);          //处理排序结果
//////////////////////////////////////////////////////////////////////
//计算接口
public:
	BOOL CalcMyPlan(void);
	void CalcTestStrategy(void);
	BOOL CalcFinalOut(int isAndRadio);
/////////////////////////////////////////////////////////////////////
//各类处理函数
///////////////////////////主条件判断函数
	void CalcEqual(void);
	void CalcGreater(void);
	void CalcLess(void);
	void CalcUp(void);
	void CalcDn(void);
	void CalcUpCross(void);
	void CalcDnCross(void);
	void CalcTurnUp(void);
	void CalcTurnDn(void);
///////////////////////////连续信号判断条件
	void CalcContiNoBuy(void);
	void CalcContiDayNoBuy(int cancelData);
///////////////////////////止损判断条件
	void CalcMaxKS(void);
	void CalcTopKS(void);
//////////////////////////////////////////////////////////////////////
//定势辅助函数
private:	
	short	E_OddK_Style(struct AnalyData *pAnalyData,short nSpan,short nNo,short nUpDown,short nSize,short nShadow,short nWeight,short nMode);
	short	E_TwinK_Style(struct AnalyData *pAnalyData,short nSpan,short nNo,short nKStyle,short nWeight,short nMode);
	short	isTrendK_Style(MemStkInfo *tmpInfo,struct AnalyData *pAnalyData,short nDataNum,short nSpan,short nNo,short nKStyle);
	short	OddK_UpDown(struct AnalyData *pAnalyData,short nNo);
	short	OddK_Size(struct AnalyData *pAnalyData,short nNo);
	short	OddK_Shadow(struct AnalyData *pAnalyData,short nNo);
	short	isTwinK_Style(struct AnalyData *pAnalyData,short nNo,short nKStyle);

protected:
	BOOL	m_bTQ;
	int		m_nConGap;						//各条件周期距最近分析数据周期
	int		m_nEndGap;						//最近分析数据距当前交易日周期	
	int		m_bSort;						//是否有排序
	int		m_nCon;							//条件数
	BOOL	m_bHasType[MAX_TYPE];			//条件索引(该策略是否包含某类条件)
	int		m_curGPIndex;
	int		m_nCalcPeriod;					//分析周期
	long	m_lReadNum;                     //大盘分析数据个数
	long	m_lNeedNum;
	long	m_lMyReadNum;                   //个股分析数据个数
	int		m_nDataNum;						//简单分析数据个数

	AnalyData *m_pRecentData;				//简单分析数据
	LPHISDAT m_pAnalyData;					//历史数据
	MemStkInfo *m_pCurGpInfo;				//基本信息
	CurrStockData *m_pCurGpData;			//行情数据
	NTime  m_startTime;
	NTime  m_endTime;
	short  m_curConIndex;
	float  *m_fDataLines;					//存放分析数据
	long   *m_aTransData;					//存放历史条件结果集
	long   *m_transData;					//存放单一条件历史结果
	long   *m_finalOut;						//多条件综合历史结果
	SingleCon     m_curCalcCon;				//当前计算条件
	Plans         m_curPlans;				//选股策略
	SignStrategy  m_curSignStra;			//当前条件买卖策略
   	KStrategy     m_curCalcKs;				//当前止损策略
	TestStrategy  m_curTestStra;			//策略测试

	vector<int> m_nGpInclude;                            //筛选出的股票
	map<float, int, greater_equal<float> > *m_mapSort;    //所有备选股的指标或行情或财务数据，排序用
	vector<map<float, int, greater_equal<float> > > m_aMapSort;
	vector<int> m_iSort;
/////////////////////////////////////////////////////////////////////初始数据请求
	BOOL EqualGreaterDate(NTime &validDate, NTime &endDate);
	BOOL EqualLessDate(NTime &validDate, NTime &endDate);
	void CalcConGap(long nearTime);                                                       
	void PutCurTransDataToCalc(int nCurCon);
	void PutCurTransDataBack(int nCurCon);
	int  CalcSortCon(short i, SingleCon &curCon, long TotalStockNum, short *sortSelStk);   //排序统计

	void DelSortCon(int nToDel);                    //对排序条件删除特殊处理
	BOOL InputCurConData(SingleCon curCon);   //填充m_fDataLines
	void CalcCurCon(SingleCon curCon);
	void TransWithSign(void);
	void TransWithKS(void);
	
	void RePlaceDate(CString &sList, long nDate);
};

#endif // !defined(AFX_MYSTRATEGY_H__D3FBB36B_1683_4F32_A108_3D297934DA99__INCLUDED_)
