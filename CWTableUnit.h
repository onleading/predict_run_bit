#ifndef CWTABLE_UNIT_H
#define CWTABLE_UNIT_H

#include	"Ubase.h"
#include	"SwitchEx.h"
#include	"TableTitleDlg.h"
#include	"CWChartUnit.h"

#define UM_ASYN_SQL_MSG		WM_USER+1350
#define	UM_LIST_SELCHANGE	WM_USER+1351
#define UM_LIST_COLSORT		WM_USER+1352
#define UM_LIST_ROWGNPOP	WM_USER+1353

class VChart;
//////////////////////////////////////////////////////////////////////////
//股票集
struct GP_Info
{
	short	setcode; //0:深圳 1:上海
	char	Code[7];
};

struct Domain_Info
{
	short	nBlockFlag;
	short	WhichType;
	DWORD	Reserved;
};

#define	TABLE_MAXGP_NUM	2000
struct GP_Set
{
	short	nGpNum;
	GP_Info GpSetInfo[TABLE_MAXGP_NUM];
};

struct CwParam
{
	long	nCwTimeS;			//财务起始期
	long	nCwTimeE;			//财务终止期
};

struct	Chart_RegInfo 
{
	VChart		*pChart;
	Chart_Cfg	*pCfg;
	BOOL		bLinked;			//完全联动
	UINT		uMsgID[20];			//部分联动
};
//////////////////////////////////////////////////////////////////////////	
class VList;
class CCWTableUCWnd;

class CCWTableUnit : public UBase
{
public:
	CCWTableUnit(LPCOMINFO pComInfo);
	virtual ~CCWTableUnit();

	BOOL	InitTable(LPCSTR strGnID, LPCSTR strGnIni);
	void	SetGPSetAndGetData(GP_Set *pGp, BOOL bGetData);
	void	SetParamAndGetData(TableBGQ *pParam, BOOL bGetData);
	void	SetGPCodeAndGetData(short setcode,CString GPCode, BOOL bGetData);
	void	SetBlockCodeAndGetData(CString BlockCode, BOOL bGetData);

	void	BeginShowList();
	void	SetShowStyle(int nShowStyle) 
	{ 
		m_nShowStyle = nShowStyle;
		BeginShowList();
	}
	void	RegOneChart(VChart *pChart, Chart_Cfg *pCfg, UINT uMsgID, BOOL bLinked = FALSE)
	{
		if(!pChart||!pCfg) return;
		Chart_RegInfo cri={0};
		cri.pChart = pChart;
		cri.pCfg = pCfg;
		if(bLinked) 
		{
			cri.bLinked = bLinked;
			memset(cri.uMsgID, 0, sizeof(cri.uMsgID));
			m_vRegChart.push_back(cri);
			return;
		}
		else
		{
			for(int i=0;i<20;i++)
			{
				if(cri.uMsgID[i]==0)
				{
					cri.uMsgID[i] = uMsgID;
					return;
				}
			}
		}
	}
	void	RemoveReggedMsg(VChart *pChart, UINT uMsgID)
	{
		if(!pChart) return;
		for(int i=0;i<m_vRegChart.size();i++)
		{
			if(m_vRegChart[i].pChart==pChart)
			{
				for(int j=0;j<20;j++)
				{
					if(m_vRegChart[i].uMsgID[j]==uMsgID)
					{
						m_vRegChart[i].uMsgID[j] = 0;
						return;
					}
				}
			}
		}
	}

	char*	GetMyIni() {return m_Cfg.GnIniFile;}

	void	ShowAll(BOOL bShow = TRUE);
	void	ArrangeWnd(CRect &rc, BOOL bHide);
	BOOL	GetData(BOOL bRefreshHead=FALSE);

	void	OnGetDataAck(WPARAM wParam,LPARAM lParam);

	void	OnZHTabCmd(UINT nID);
	void	OnUpdateZHTabCmd(CCmdUI *pCmdUI);
	void	OnTabList(WPARAM wParam,LPARAM lParam);
	void	OnTabListCmd(UINT nID);
	void	OnUpdateTabListCmd(CCmdUI *pCmdUI);
	void	OnTabListBlockCmd(UINT nID);
	void	OnUpdateTabListBlockCmd(CCmdUI *pCmdUI);

	void	OnListRowGnPop();

	int		ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

	void	OnRefreshChartFromList(UINT uMsgID);

private:
	void	RefreshColorFontCfg();
	void	ShowTabSwitch(char *SwitchString);
	void	ChangeTabSwitch(char *SwitchString);
	void	SetListTitle(char *strExtMsg);
	void	RefreshHeadFrame();
	void	DeleteListContent()
	{
		m_pList->DeleteItem(-1, TRUE);
	}

	short	GetTitleHeight()
	{	
		if(!strstr(m_Cfg.SQLstatement, "#time")) return 0;
		return 27;
	}

	void	InitNewTabList(char *SwitchString);

	BOOL	InitConnect();	
	BOOL	InitConnectInfo();
	BOOL	GetGnInfo(CString GnPath,CString GnINI);
	BOOL	GetFromGNIni(struct Table_GN_Cfg *pCfg, const char *PathFile);

	void	ProcessSet(long nClue);

	BOOL	SaveListHead();
	
	//Tool
	void	FillGPSet(GP_Set *pGs, short *pStkIndex, int nStkNum);

	void	ProcessChartRefresh(VChart *pChart, Chart_Cfg *pCfg);

	void	SetStaticDataCell(Chart_Cfg *pCfg,
		int nCol,
		sg_cell sc,
		std::vector<std::string>& vStaticXData, 
		std::vector<std::vector<double> >& vStaticYData);

	void	SetDynaDataCell(Chart_Cfg *pCfg,
		int nCol,
		sg_cell sc,
		std::vector<std::vector<double> >& vDynaData);

	void	SetStarDataCell(Chart_Cfg *pCfg,
		int nCol,
		sg_cell sc,
		std::vector<std::vector<double> >& vStarXData,
		std::vector<std::vector<double> >& vStarYData,
		std::vector<std::string> *vStarMainData);

//数据成员
private:
	CCWTableUCWnd	*m_pTUCtrlWnd;

	//下分类属性页
	CNewZfjsTab		*pSwitchBar;
	int				m_SwitchWidth;
	int				m_SwitchHeight;
	CRect			m_rcSwitch;
	map<long, CString, greater<long> > m_MapTabList;
	int				m_MainItemNum[100];
	UINT			m_nZHTabCmdID, m_nTabListCmdID, m_nTabListBlockCmdID;

	CTableTitleDlg	*m_pTitleDlg;

	char			m_strGnID[21];
	BOOL			m_bFirstEntry;

	//功能配置
	Table_GN_Cfg	m_Cfg;

	short			m_nShowStyle;						//0:常规风格 1:行情风格
	
	//各种参数
	GP_Set			m_GpSet;
	CString			m_BlockCode;
	TableBGQ		m_CwParam;

	long			m_nClue;
	VList			*m_pList;

	vector<Chart_RegInfo>	m_vRegChart;
};

#endif
