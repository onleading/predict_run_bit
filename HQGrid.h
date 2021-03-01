#ifndef _HQGRID_H
#define _HQGRID_H

#include "Grid.h"
#include "DDE.h"

#define GetStatData_And_WriteFloat(ftmp){fStatData = ftmp; if(m_pWriteFile) m_WriteStr+=FloatToStr(ftmp);}

#define GetStatData_And_WriteInt(ltmp)  {fStatData = ltmp; if(m_pWriteFile) m_WriteStr+=IntToStr(ltmp);}

#define DrawEmpty_And_WriteEmpty   {DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);\
									if(m_pWriteFile) m_WriteStr+=MEANLESS_STR;}

enum STAT_TYPE//几种统计方法
{
	STAT_AVER = 0,
	STAT_SUM,
	STAT_MAX,
	STAT_MIN,
	STAT_STD,
	STAT_LASTNULL = 10
};

enum  HEAD_DATA_TYPE
{
	TYPE_STRING=0,	//字符串类型
	TYPE_FLOAT,		//浮点类型
	TYPE_INT,		//整数类型
};

enum HEAD_CALC_TYPE
{
	CALC_STATIC=0,	//载入静态
	CALC_DYNA_TXT,	//实时变化的文本
	CALC_DYNA_NUM,	//由算式得到的实时变化的数字,暂不支持
	CALC_COM,		//综合计算,暂不支持
};

struct DynaHQItem
{
	DynaHQItem()
	{
		DynaID = 0;
		Name[0] = 0;
	};
	DynaHQItem(UINT DynaID_i, char *Name_i)
	{
		DynaID = DynaID_i;
		strncpy(Name, Name_i, 30);
	};
	UINT	DynaID;
	char	Name[31];
};

typedef struct FormatInfo
{
	int v_align;	//垂直方向对齐方式,0:中,1:上,2:下
	int h_align;	//水平方向对齐方式,0:左,1:中,2:右
	COLORREF color;	//文本颜色
	int xsflag;		//若是浮点数,保留的小数位数
	FormatInfo()
	{
		v_align=0;
		h_align=2;
		color=GridColor.Grid_Level_Color;
		xsflag=XS2;
	}
}FormatInfo,*pFormatInfo;

struct ColHeadInfo
{
	int				nHeadID;		//累加,init后固定
	char			strTitle[41];	
	short			width;			//宽度，字幅宽度
	HEAD_DATA_TYPE	dataType;		//数据类型
	HEAD_CALC_TYPE	calcType;		//数据计算类型
	UINT			dynaID;	  		//关联行情字段,0~TOTAL_COL-1为动态行情字段,TOTAL_COL~2*TOTAL_COL-1为静态字段,2*TOTAL_COL~3*TOTAL_COL-1为动态公式字段
	CString strFormula;				//calcType为CALC_DYNA_NUM时记录公式,为CALC_STATIC时记录字段名
	ColHeadInfo()
	{
		nHeadID=0;
		memset(strTitle,0,41);
		width=0;
		dataType=TYPE_STRING;
		calcType=CALC_STATIC;
		strFormula="";
		dynaID=-1;
	}
};

#define DEF_ITEM_WIDTH	9
#define MAX_FYCOL		100
#define MAX_FYONE_LEN	1024

struct cmpstrs
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

class HQGrid : public Grid
{
	float		m_SzzsZaf,m_ShzsZaf;
	int			nSort_Type;					//排序栏目
	int			RightClickWhichCol;			//当前用户在哪个栏目上点击排序

	float		LastNow[MAXGPNUM], LastBuyp[MAXGPNUM], LastSellp[MAXGPNUM];			//上一次刷新的行情
	long		LastVol[MAXGPNUM]; //上次刷新的行情
	int			m_nCurPic;

	//用来实现拖放的成员变量
	BOOL		m_bStartDrag,m_bDragging;
	CPoint		m_DragPoint;
	int			DraggingGPNo;

	BOOL		m_bAutoPage;
	BOOL		m_bInfoConnect;
	
	//风云排行
	int					m_ItemTextFormat[MAX_FYCOL];//用于风云排行时控制每列的文字对齐方式,0:左,1:居中,2:右
	vector<ColHeadInfo>	m_vBaseHeads;				//init后的顺序　
	long				m_FYCol[TOTAL_COL];
	vector<CString>		m_vBaseRowStr;				//数据原始信息
	map<char*, DynaHQItem, cmpstrs> m_DynaIndex;
	int					m_nGPCode;					//股票代码列号,为-1表示所有head都不包含股票代码
	BOOL				m_LoadFyOK;					//加载版面成功,用来控制绘制操作

	//打印区域
	CRect			m_PRect;
	vector<int>		m_PColPerPage;
	int				m_nPRowPerPage;

	BOOL			m_bCanMoveGrid;
	BOOL			m_bMovingGrid;
	int				m_nMoveItem;
	
	//统计数据
	float			m_fAver[2*TOTAL_COL],m_fSum[2*TOTAL_COL],m_fMax[2*TOTAL_COL],m_fMin[2*TOTAL_COL],m_fStd[2*TOTAL_COL]; //统计
	float			m_fQZ[2*TOTAL_COL], m_fStdQZ[2*TOTAL_COL]; //权重
	short			m_nStatistFlag[STAT_LASTNULL];	//统计项目

	LinkedSetInfo	m_LSetInfo;
public:
	long	*	m_lpnCol;			//栏目ID
	int			m_nColNum;			//总栏目数
	float		*m_fpItemWidth;

	short		Sort_lpnUsedStkIndex[MAXGPNUM];		//用于远程版的本地排序
	short		Fy_nTag[MAXGPNUM];					//用于风云榜的定位
	BOOL		m_bRefreshDraw;	
	UINT		m_nGridType;

	BOOL			m_bDDEStatus;	//是否是DDE查看状态
	BOOL			m_bAJHStatus;	//是否是A+H查看状态
	BOOL			m_bJSZJStatus;	//资金
	BOOL			m_bSigStatus;	//信号
	int				m_nConStatus;	//选股
	int				m_nConFiltCount;//个数

public:
	HQGrid(LPCOMINFO pComInfo);
	virtual ~HQGrid();

	void	ReSetRefreshTime();
	void	OnlySetDomain(int nBlockFlag,int WhichType);
	void	ResetRowCol();
	void	SetDomain(int nBlockFlag,int WhichType,BOOL bSort,int SortType,BOOL AscendSort);
	void	InitGrid(BOOL bSort,int SortType,BOOL AscendSort);

	void	DrawRemoteGrid(CDC *pDC);
	void	DrawGrid(CDC *pDC,BOOL bRealDraw=TRUE);	//主画图函数
	void	PrintGrid(CDC *pDC, CPrintInfo* pInfo);

	void	OnUpdateRightClickHead(CCmdUI* pCmdUI);
	void	OnUpdateRank(CCmdUI *pCmdUI);
	void	OnRightClickHead(UINT nID);

	void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void	OnShiftUp();
	void	OnShiftDown();
	void	OnClick(CDC *pDC,CPoint point,BOOL IsRight);
	void	OnLButtonDown(UINT nFlags, CPoint point);
	void	OnLButtonUp(UINT nFlags, CPoint point);
	void	OnLButtonDblClk(UINT nFlags, CPoint point);
	void	OnMouseMove(UINT nFlags, CPoint point);
	void	OnRButtonUp(UINT nFlags, CPoint point);

	void	ProcessHQRefresh();		//处理行情更新
	void	SpecialReDraw(int Flag);
	void	GetDataACK(BOOL bDSData=FALSE);
	void	OnGetDataACK(short nMainID,BOOL bDSData=FALSE);
	void	OnWritetotxtfile();
	BOOL	GetAutoPage(){return m_bAutoPage;}

	BOOL	IsHZTypeNow(int nType)
	{
		if(nType<0||nType>=STAT_LASTNULL) return FALSE;
		return m_nStatistFlag[nType]>0;
	}

	void	SetHZType(int nType)
	{
		if(nType<0||nType>=STAT_LASTNULL) return;
		m_nStatistFlag[nType] = 1-m_nStatistFlag[nType];
	}

	int		GetStaticItemCount()
	{
		int res = 0;
		for(int i=0;i<STAT_LASTNULL;i++)
		{
			if(m_nStatistFlag[i]>0)
				++res;
		}
		return res;
	};

	int		GetNextGP(int nGPIndex, BOOL bNext=TRUE)
	{
		int nAdd = (bNext?1:-1);
		for(int i=0;i<nTotalRow;i++)
		{
			if(lpnUsedStkIndex[i]==nGPIndex)
				return lpnUsedStkIndex[(i+nAdd+nTotalRow)%nTotalRow];
		}
		int allgp = g_pStockIO->GetStockNum(ALL_ALL);
		return ((nGPIndex+nAdd)+allgp)%allgp;
	};
	
	LinkedSetInfo*	GetLinkedSetInfo(void);

	//趋势导航
	void	InitFYInfo(void);
	void	InitDynaMap(void);
	void	GetDynaInfo(char *strIniName, char *strShowName, UINT &nDynaID);
	BOOL	ParseHeadInfo(ColHeadInfo &colinfo,char *colstr);
	int		GetCodeIndex(int nBaseRow);
	void	GetString(char *strValue, int maxlen,int nBaseRow, int nBaseCol);
	float	fGetFyValue(int nBaseRow, int nBaseCol);
	BOOL	GetFYTxt(LPCSTR infoFile,int nIndex,CString &strInfo);
	void	DrawImage(CDC *pDC,int row,int col,int iIndex);
	void	DrawImage2(CDC *pDC,int row,int col,int iIndex1,int iIndex2);
	void	DrawCellStringText(CDC *pDC,int row,int col,int PosFlag,int PosFlag2,COLORREF color,BOOL bTranBig5,char *fmt,...);
	void	TransID2Str(UINT nID,MemStkInfo *pInfo,CurrStockData * hqp,EXCURRDATA *hqpex,LPDDEREPORT pDDEReport,FormatInfo &fi,float &float_out,CString &rs,CString &e_rs);	//根据索引号取动态量(如现价)的字符串形式和浮点形式的值

	//打印
	void	SetPrintRect(CRect rect) {m_PRect = rect;}
	void	CalcPrintPage(CDC* pDC, CPrintInfo* pInfo);

	void	Sort(void);				//排序函数

	void	InsertSwitch(int nSCType);
	int		ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
private:
	inline int		GetItemWidth(int ColID);
	inline LPSTR	GetItemTitle(int ColID);
	void			GetSomeSize(CDC *pDC);
	inline HEAD_DATA_TYPE	GetItemDataType(int ColID);

	BOOL	TestInComView2();	
	void	DrawMineIcon(CDC *pDC,int row,int col,int flag);	
	void	ToGPVip(BOOL bNewView, int flag=0);
	void	ChangeGPType(BOOL bTurn);
	void	PopupCornerMenu(CRect CornerRect);
	void	ShiftGPInBlock(int DragGPNo,CPoint point);
	void	GetData();								//请求行情数据
	void	FiltConFlag();

	void	DrawA2HItem(CDC *pDC,int row,int col,int xsflag,MemStkInfo * pInfo_hk,CurrStockData * hqp_hk,EXCURRDATA *hqpex_hk, MemStkInfo * pInfo,CurrStockData * hqp,EXCURRDATA *hqpex);
	void	DrawItem(CDC *pDC,int row,int col,int xsflag,MemStkInfo * pInfo,CurrStockData * hqp,EXCURRDATA *hqpEx=NULL);
	void	DrawHZ(CDC *pDC);
	void	DrawTitle(CDC *pDC);
	void	DrawBlueRect(CDC *pDC,int row,int col,float f1,int xsflag,BOOL bNarrow=FALSE);

	void	CalcItem(int row,int col,int xsflag,MemStkInfo * pInfo,CurrStockData * hqp,EXCURRDATA *hqpex,float fQZ,int nCalcWhich=0);	//nCalcWhich 0-第一轮 1-第二轮(需用第一轮数据)
	void	DrawHZItem(CDC *pDC,short nHZType,float	*fCellData,int row,int col,int xsflag);

	int		GetCellLeft(int nCol,int & nWidth);		//得到方格的左边位置
	float	fGetSortValue(short xh);				//得到排序Float值(不包括股票名称)
	void	OnCtrlLeft();

	BOOL	HitTestGrid(int &nHitItem, CPoint point);
	void	ChangeGPInGrid(int SrcGPNo,int DestGPNo,BOOL bShift);

	void	DrawSortHighLine(CDC *pDC);
};

#endif
