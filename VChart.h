#ifndef CVCHART__H__H
#define CVCHART__H__H
#include <afxwin.h>
#include <vector>
#include <string>

//动态饼图
class DynaChartItem
{
public:
	//动态饼图的各分块的字段
	std::vector<std::string> name;
	//动态饼图的各分块的名字
	std::vector<std::string> vtitle;
	//动态饼图的各分块的颜色
	std::vector<COLORREF> color;
	//饼图的标题
	std::string title;
};


//静态图表
//for StaticYItemType, nType
#define TCSS_YITEM_LINE	0X00000000//此y轴显示为折线,同一个静态图中各个y轴可以显示为折线或柱,同一个y轴也可以在同一个图中同时显示为折线和柱
#define TCSS_YITEM_POLE	0X00000001//此y轴显示为拄装,同一个静态图中各个y轴可以显示为折线或柱,同一个y轴也可以在同一个图中同时显示为折线和柱
#define TCSS_YITEM_MASK	0X00000001//掩码
//for StaticYItemType, nType,左右轴的刻度单位可能不同
#define TCSS_AXIS_LEFT	0X00000000//此y轴使用左坐标轴刻度值,如果一个静态图的所有y轴都使用右轴,则左轴不会被画出
#define TCSS_AXIS_RIGHT 0X00000010//此y轴使用右坐标轴刻度值,如果一个静态图的所有y轴都使用右轴,则左轴不会被画出
#define TCSS_AXIS_MASK	0X00000010//掩码

//静态图表的定义,一个x轴,多个y轴
class StaticChartItem
{
public:
	class StaticXType
	{
	public:
		std::string name;
		std::string title;
	};
	class StaticYItemType
	{
	public:
		StaticYItemType(){}		
		StaticYItemType(const std::string& _name, int _nType,
						const COLORREF& _color, const std::string& _caketitle, 
						const std::string& _poletitle, const std::string& _title)
		{
			name	  = _name;
			nType	  = _nType;
			color	  = _color;
			caketitle = _caketitle;
			poletitle = _poletitle;
			title	  = _title; 

		}
		StaticYItemType(const std::string& _name, int _nType,
						const COLORREF& _color, const std::string& _caketitle,
						const std::string& _poletitle)
		{
			name	  = _name;
			nType	  = _nType;
			color	  = _color; 
			caketitle = _caketitle;
			poletitle = _poletitle;
		}		
		UINT		nType;		//柱线选项,左右轴的情况
		COLORREF    color;
		std::string name;
		std::string title;		//如果为柱线或并柱，y轴标题
		std::string caketitle;	//如果为饼,饼的标题
		std::string poletitle;	//如果为并柱，并柱的标题
	};

	UINT							nFlag;
	StaticXType						xname;
	std::vector<StaticYItemType>	ynames;
};


//星空图,它有3维;x,y轴(StarColumnType)都为数值;然后每一点又有一个名字(StarMainType)
class StarChartItem
{
public:
	class StarColumnType
	{
	public:
		std::string name;
		std::string title;
		COLORREF	color;
	};
	class StarMainType
	{
	public:
		std::string name;
		std::string title;
		COLORREF	color;
	};

	int				type;
	StarColumnType	xname;   //x轴
	StarColumnType	yname;	//y轴
	StarMainType	mainname;//z轴
};

#define TDX_DLLCHART_CLR_NUM	20	//图例颜色

struct ChartColorFont
{
	COLORREF BkColor;				//系统背景颜色
	COLORREF TitleTextColor;		//系统标题颜色
	COLORREF ChartBkColor;			//图表背景颜色
	COLORREF ChartTxtColor;			//图表文本颜色
	COLORREF AxisTxtColor;			//轴文本颜色
	COLORREF LightColor;			//图表虚线条颜色
	COLORREF AxisColor;				//边线的颜色
	COLORREF BrushColor[TDX_DLLCHART_CLR_NUM];		//图例的颜色

	LOGFONT	 TitleFont;				//标题字体
	LOGFONT	 DescFont;				//图例使用的字体
	LOGFONT	 ChartFont;				//坐标轴等用的字体
};

//支持的三大类图表
#define TCS_STATIC					0X00000001	//图表为静态表
#define TCS_DYNA					0X00000002	//动态表
#define TCS_STAR					0X00000004	//星空图
#define TCS_STYLE					0X00000007	//三大类的掩码

//静态表类的三小类
#define TCS_CAKE					0X00000000	//饼图
#define TCS_PARA					0X00000010	//并列坐标类,独立坐标系
#define TCS_ALLINONE				0X00000020	//综合类,都画在同一坐标系

#define TCS_STATIC_STYLE			0X00000030	//三小类的掩码，ChangeStyle()时可能用到
#define TCS_STATIC_STYLE_CHANGEABLE	0X00000040	//如果是静态饼，指明可否变为并列或综合，如果为并列或综合，指明是否可变为饼图

#define TCS_VERT					0X00000080	//对于PARA和ALLINONE，是否颠倒xy坐标

//图表风格
#define TCS_HGRID					0X00000100	//是否显示横向网格线
#define TCS_VGRID					0X00000200	//是否显示纵向网格线
#define TCS_3D						0X00000400	//图形是否支持3d效果
#define TCS_XFLOATINGBOX			0X00001000	//是否在显示x轴游标
#define TCS_YFLOATINGBOX			0X00002000	//是否在显示y轴游标
#define TCS_FLOATINGBOX				(TCS_XFLOATINGBOX|TCS_YFLOATINGBOX)

#define TCS_TOOLTIP_AUTOWANYI		0X00004000	//ToolTip是否自动用万亿表示法

#define TCS_SYSCLR_DYNA				0X00010000	//动态图并使用缺省的颜色
#define TCS_SYSCLR_STATIC			0X00020000	//静态图并使用缺省的颜色

//关于静态图综合图的图例
//1:图例位置
#define TCS_ALLINONE_EXAMPLE_TOP		0X04000000	//在坐标图上可移动
#define TCS_ALLINONE_EXAMPLE_RIGHT		0X08000000	//固定在坐标图之右
#define TCS_ALLINONE_EXAMPLE_POS		0X0C000000	//两种位置的掩码
//2:图例显示模式
#define TCS_ALLINONE_EXAMPLE_1Row		0X00000000	//显示为一行
#define TCS_ALLINONE_EXAMPLE_1Col		0X10000000	//显示为一列
#define TCS_ALLINONE_EXAMPLE_2Col		0X20000000	//显示为两列
#define TCS_ALLINONE_EXAMPLE_2Row		0X30000000	//显示为两行
#define TCS_ALLINONE_EXAMPLE_ROW_COL	0X30000000	//显示模式掩码

//向图表控件中添加各图表的步骤:1:添加图表;2:向图表中添加数据
class VChart: public CWnd
{
public:
	//接口
	virtual DWORD GetStyle(DWORD dwMask = 0XFFFFFFFF) = 0;
	virtual BOOL  ChangeStyle(DWORD dwRevmove, DWORD dwAdd = 0, UINT nFlag = 0) = 0;

	//当设数据时，如果想刷新使用参数bRefresh=TRUE，而不要使用Invalidate();
	//设置第index(0 based)星空图的名字坐标轴，x坐标轴，y坐标轴
	virtual BOOL SetStarMainData(int index, const std::vector<std::string>& maindata, BOOL bRefresh = FALSE) = 0;
	virtual BOOL SetStarXData(int index, const std::vector<double>& pData, BOOL bRefresh = FALSE) = 0;
	virtual BOOL SetStarYData(int index, const std::vector<double>& pData, BOOL bRefresh = FALSE) = 0;

	//设置静态图的x坐标轴和第index个y坐标轴数据
	virtual BOOL SetStaticXData(const std::vector<std::string>& xdata, BOOL bRefresh = FALSE) = 0;
	virtual BOOL SetStaticYData(int index, const std::vector<double>& pData, BOOL bRefresh = FALSE) = 0;

	//设置动态饼的数据
	virtual BOOL SetDynaChartData(int index, const std::vector<double>& data, BOOL bRefresh= FALSE)= 0;

	//添加删除修改星空图
	virtual int  AddStarChart(const StarChartItem& item) = 0;
	virtual BOOL DeleteStarChart(int index = -1) = 0;
	virtual BOOL ModifyStarChart(int index, const StarChartItem& item) = 0;

	//添加删除修改静态图表项
	virtual int  AddStaticYChart(const StaticChartItem::StaticYItemType& item) = 0;
	virtual BOOL DeleteStaticYChart(int index = -1) = 0;
	virtual BOOL ModifyStaticYChart(int index, StaticChartItem::StaticYItemType& item)= 0;
	virtual BOOL ModifyStaticXChart(StaticChartItem::StaticXType& xname) = 0;

	//添加删除修改动态图表项
	virtual int  AddDynaChart(const DynaChartItem& item) = 0;
	virtual BOOL DeleteDynaChart(int index = -1) = 0;
	virtual BOOL ModifyDynaChart(int index, const DynaChartItem& item) = 0;
	
	//构造好各图表,直接设给控件
	virtual void  SetStarChart(const std::vector<StarChartItem>& StarChart) = 0;
	virtual const std::vector<StarChartItem>& GetStarChart() const = 0;
	virtual void  SetStaticChart(const StaticChartItem& StaticChart) = 0;
	virtual const StaticChartItem& GetStaticChart() const = 0;
	virtual void  SetDynaChart(const std::vector<DynaChartItem>& DynaCharts) = 0;
	virtual const std::vector<DynaChartItem>& GetDynaChart() const = 0;

	//设置颜色等
	virtual void  SetChartColorFont(const ChartColorFont& mgt) = 0;
	virtual const ChartColorFont& GetChartColorFont() const = 0;

	//静态图表调用，设置选中数据
	virtual void  SetStaticSel(int index,int nSel = -1,COLORREF rgb=RGB(128,128,128)) = 0;

	//图表发生变化时(如数据)，先RefreshChart()然后再Invalidate();
	virtual void RefreshChart() = 0;
};

#ifdef  IMPL_TDX_CCHARTD
	#define TDX_CCHARTD_DLL __declspec(dllexport)
#else
	#define TDX_CCHARTD_DLL __declspec(dllimport)
#endif

//图表控件的创建和删除使用下面两个函数,而不是使用new delete
TDX_CCHARTD_DLL VChart* CreateNewTChart(DWORD dwStyle, HWND hWnd, CRect rect, UINT nID);
TDX_CCHARTD_DLL void DeleteTChart(VChart** ppVChart);

#endif
