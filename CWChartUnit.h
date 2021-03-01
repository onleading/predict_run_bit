#ifndef CWChartUnit_H_
#define CWChartUnit_H_

#include "UBase.h"
#include "VChart.h"

#define UM_ASYN_SQL_MSG		WM_USER+1350

class StarChartsType
{
public:
	int type;//reserved
	std::vector<StarChartItem> StarCharts;
};

//动态饼图
class DynaChartFieldsItem
{
public:
	std::vector<string> names;
	string title;
};
class DynaChartsType
{
public:
	int type;//reserved
	std::vector<DynaChartItem> DynaChartFields;//各动态饼字段
};

struct Chart_Cfg
{
	//功能引用的INI
	char	GnIniFile[MAX_PATH];
	short	GnIniVer;

	char	DataSrcFile[MAX_PATH];
	
	//静态图设置
	StaticChartItem StaticChart;
	//星空图
	StarChartsType StarChart;
	//动态饼图
	DynaChartsType DynaChart;
};
//////////////////////////////////////////////////////////////////////////
class VChart;
class CCWChartUCWnd;

class CCWChartUnit : public UBase  
{
public:
	CCWChartUnit(LPCOMINFO pComInfo);
	virtual ~CCWChartUnit();

	BOOL	InitChart(LPCSTR strGnID, LPCSTR strGnIni);
	void	ArrangeWnd(CRect &rc, BOOL bHide);
	void	BeginShowChart();
	void	ShowAll(BOOL bShow);

	char		*GetMyIni() {return m_Cfg.GnIniFile;}
	char		*GetLinkedTableIni() {return m_Cfg.DataSrcFile;}
	VChart		*GetMyChart() {return m_pChart;}
	Chart_Cfg	*GetMyCfg() {return &m_Cfg;}

	int		ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

private:
	void	RefreshColorFontCfg();
	BOOL	GetGnInfo(CString GnPath,CString GnINI);
	BOOL	GetFromGNIni(struct Chart_Cfg *pCfg, const char *PathFile);

private:
	char			m_strGnID[21];
	Chart_Cfg		m_Cfg;
	CCWChartUCWnd	*m_pCUCtrlWnd;
	
	VChart			*m_pChart;
};

#endif
