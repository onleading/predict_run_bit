#ifndef _GRID_H_
#define _GRID_H_

#include "Ubase.h"
#include "MyScrollbar.h"
#include "SwitchEx.h"

#define CREATE_GRIDMEMDC	\
	CBitmap tempBitmap;\
	CDC memDC;\
	memDC.CreateCompatibleDC(pDC);\
	tempBitmap.CreateCompatibleBitmap(pDC,m_DRect.Width (),m_DRect.Height());\
	CBitmap *pOldBitmap = memDC.SelectObject (&tempBitmap);\
	memDC.FillSolidRect(0,0,m_DRect.Width(),m_DRect.Height(),GridColor.GridBackColor);\
	memDC.SetBkMode (TRANSPARENT);\
	CFont *oldfont = g_d.SelectFont(&memDC,m_nInCom2?SMALL2_FONT:LIST_FONT);\
	CPen  *oldpen = g_d.SelectPen(&memDC,GRIDLINE_PEN);

#define DELETE_GRIDMEMDC	\
	g_d.RestorePen(&memDC,oldpen);\
	g_d.RestoreFont(&memDC,oldfont);\
	memDC.SelectObject(pOldBitmap);\
	tempBitmap.DeleteObject();\
	memDC.DeleteDC();

class Grid : public UBase
{
public:
	BOOL		bDrawBlue;			//仅用在HQGrid类中,是否进行蓝色反显
	int 		DrawFlag;			//DrawReport的画法DRAW_FRAME,DRAW_NOREADDATA,DRAW_ALL三种
	int         nTotalRow;			//所有股票个数
	int         nTotalCol;			//所有栏目个数
	int         nCurRow;			//当前行
	int			nStartRow;			//该页第一行  
	int         nStartCol;			//第一、二列固定，从第三个栏目数起
	int         nEndCol;			//最后一列
	int			nEndRow;			//该页最后一行
	int			nColNum;			//列数
	int         nRowNum;			//本窗口实际能显示的股票个数
	
	int			tempRowNum,tempColNum;
	BOOL		tempRowFlag,tempColFlag;

	int			nCurSortIndex;		//排序的索引号（对于当前栏目顺序）
	BOOL		bAscendSort;		//是否增序

	short       lpnUsedStkIndex[MAXGPNUM];	//指向本窗口实际能显示的股票序号数组

	int			m_WhichGPType;		//股票分类
	int			m_nBlockFlag;		//-1:非板块股,分类股; 0-99:用户定义板块股; >=100 < 200 地方板块;>=200 < 300 行业板块;>=300 概念板块

	int			m_nFixedLeadCol;	//起头固定的列数
	BOOL		m_bHasSwitch;	

	BOOL		m_bWriteAllHQ;
	BOOL		m_bFixedGrid;
	int			m_SwitchWidth;
public:
	Grid(int nFixedLeadCol = 2,BOOL bHasLabel = FALSE,LPCOMINFO pComInfo = NULL);
	virtual ~Grid();

	BOOL	Create(CWnd *pParentWnd,CRect rect, int nSCType=0,int whichunit=0);
	void	SetRect(CRect rect, BOOL bRefresh = FALSE);	//设置报表的区域并调整滚动条
	void	GetClientRect(CRect &rect);					//得到窗口大小
	void	SetControlPos(CWnd *pWnd, CRect &rect);		//设置swtchbar等控件的位置
	virtual void InsertSwitch(int nSCType){}
	void	HideSwitchBar(void);

	void	RefreshScrollBar();
	void	SetSwitchBarSel(int whichFl,int subFl=0);

	CString GetBlockCaption(int nBlockFlag);

	void	GetDrawRect(CRect &rect, short nSwitchFlag=0);
	void	GetCornerRect(CRect &rc);

	int		GetBlockFlag(){return m_nBlockFlag;}
	int		GetGPType(){return m_WhichGPType;}
	int		GetTotalRow(){return nTotalRow;}
	void	GetGridTitle(CString &str);

	short	GetCurrentGPIndex();
	short	GetCurrentGPSet(short *pPageGPIndex);
	BOOL	ProcessGPCode(const char *code,short setcode);
	void	ProcessAutoPage();

	int		GetDragItem(CPoint pt);	//得到当前正在拖放的股票序号,-1表示没有得到

	void	ToSomeRow(int row);
	void	ToSomeCol(int col);

	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

	void	OnHScrollMessage(WPARAM wParam,LPARAM lParam);
	void	OnVScrollMessage(WPARAM wParam,LPARAM lParam);

	void	WriteToTxtFile(CString filename,BOOL bWriteExcel=FALSE);

	void	SetPartMode(BOOL bPart);
	BOOL	IsFixedGrid(void);
	void	GetDomain(int &nGPType, int &nBlockFlag);
	void	ReCalcSwitchNeedWidth();
protected:
	CString		m_WriteStr;
	CStdioFile	*m_pWriteFile;
	BOOL		m_bWriteExcel;

	CNewZfjsTab		*pSwitchBar;
	
	BOOL			m_bPartMode;	//局部模式 和 全翻模式

	CString		m_strTitle;
	CRect		m_Rect,m_DRect;		//总区域和画图区域

	sort_struc  lpSort[MAXGPNUM];
	sort_struc2 lpSort2[MAXGPNUM];	//用于股票名称拼音排序
    sort_struc3 lpSort3[MAXGPNUM];	//专用于热门板块分析

	int			nTitleHeight0;		//汇总起始高度
	int			nTitleHeight;		//标题栏高度
	int			nCellHeight;		//方格高度
	int			nSigTitleHeight;	//单格高度
	int			nSigCellHeight;		//单格高度
	int			nCellHeadWidth;		//左固定列宽度
	CSize		FontSize;
	
	void	AdjustRowColNum();
	void	DrawHighLine(CDC *pDC);
	void	DrawCellText(CDC *pDC,int row,int col,int PosFlag,char *lpszStr,COLORREF color,BOOL LeftFlag=FALSE,BOOL SameWidth=FALSE,MemStkInfo *pStock=NULL);
	void	DrawCellText(CDC *pDC,int row,int col,int PosFlag,COLORREF color,char *fmt,...);
	void	DrawCellText(CDC *pDC,int row,int col,int PosFlag,float f1,int xsflag,COLORREF color,int postfix_flag=0);
	void	DrawCellText(CDC *pDC,int row,int col,int PosFlag,float f1,float f2,int xsflag,int postfix_flag=0);

	void	OnUp(CDC *pDC);
	void	OnDown(CDC *pDC);
	void	OnPageDown(CDC *pDC,BOOL bGetData=TRUE);
	void	OnPageUp(CDC *pDC,BOOL bGetData=TRUE);
	void	OnCtrlHome(CDC *pDC);
	void	OnCtrlEnd(CDC *pDC);
	void	OnLeft();
	void	OnRight();
	void	OnHome();
	void	OnCtrlRight();
	void	OnEnd();
	virtual void OnCtrlLeft() { return; } //必须重载
	//计算方格左边位置,Width为返回方格宽度,nCol表示列顺序中绝对序号,虚函数,必须重载
	virtual int	GetCellLeft(int nCol,int & nWidth) { return 0;}
	//设置市场类型
	virtual void SetDomain(int nBlockFlag,int WhichType);
};

#endif
