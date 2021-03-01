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

//��̬��ͼ
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
	std::vector<DynaChartItem> DynaChartFields;//����̬���ֶ�
};

struct Chart_Cfg
{
	//�������õ�INI
	char	GnIniFile[MAX_PATH];
	short	GnIniVer;

	char	DataSrcFile[MAX_PATH];
	
	//��̬ͼ����
	StaticChartItem StaticChart;
	//�ǿ�ͼ
	StarChartsType StarChart;
	//��̬��ͼ
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
