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
	BOOL		bDrawBlue;			//������HQGrid����,�Ƿ������ɫ����
	int 		DrawFlag;			//DrawReport�Ļ���DRAW_FRAME,DRAW_NOREADDATA,DRAW_ALL����
	int         nTotalRow;			//���й�Ʊ����
	int         nTotalCol;			//������Ŀ����
	int         nCurRow;			//��ǰ��
	int			nStartRow;			//��ҳ��һ��  
	int         nStartCol;			//��һ�����й̶����ӵ�������Ŀ����
	int         nEndCol;			//���һ��
	int			nEndRow;			//��ҳ���һ��
	int			nColNum;			//����
	int         nRowNum;			//������ʵ������ʾ�Ĺ�Ʊ����
	
	int			tempRowNum,tempColNum;
	BOOL		tempRowFlag,tempColFlag;

	int			nCurSortIndex;		//����������ţ����ڵ�ǰ��Ŀ˳��
	BOOL		bAscendSort;		//�Ƿ�����

	short       lpnUsedStkIndex[MAXGPNUM];	//ָ�򱾴���ʵ������ʾ�Ĺ�Ʊ�������

	int			m_WhichGPType;		//��Ʊ����
	int			m_nBlockFlag;		//-1:�ǰ���,�����; 0-99:�û��������; >=100 < 200 �ط����;>=200 < 300 ��ҵ���;>=300 ������

	int			m_nFixedLeadCol;	//��ͷ�̶�������
	BOOL		m_bHasSwitch;	

	BOOL		m_bWriteAllHQ;
	BOOL		m_bFixedGrid;
	int			m_SwitchWidth;
public:
	Grid(int nFixedLeadCol = 2,BOOL bHasLabel = FALSE,LPCOMINFO pComInfo = NULL);
	virtual ~Grid();

	BOOL	Create(CWnd *pParentWnd,CRect rect, int nSCType=0,int whichunit=0);
	void	SetRect(CRect rect, BOOL bRefresh = FALSE);	//���ñ�������򲢵���������
	void	GetClientRect(CRect &rect);					//�õ����ڴ�С
	void	SetControlPos(CWnd *pWnd, CRect &rect);		//����swtchbar�ȿؼ���λ��
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

	int		GetDragItem(CPoint pt);	//�õ���ǰ�����ϷŵĹ�Ʊ���,-1��ʾû�еõ�

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
	
	BOOL			m_bPartMode;	//�ֲ�ģʽ �� ȫ��ģʽ

	CString		m_strTitle;
	CRect		m_Rect,m_DRect;		//������ͻ�ͼ����

	sort_struc  lpSort[MAXGPNUM];
	sort_struc2 lpSort2[MAXGPNUM];	//���ڹ�Ʊ����ƴ������
    sort_struc3 lpSort3[MAXGPNUM];	//ר�������Ű�����

	int			nTitleHeight0;		//������ʼ�߶�
	int			nTitleHeight;		//�������߶�
	int			nCellHeight;		//����߶�
	int			nSigTitleHeight;	//����߶�
	int			nSigCellHeight;		//����߶�
	int			nCellHeadWidth;		//��̶��п��
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
	virtual void OnCtrlLeft() { return; } //��������
	//���㷽�����λ��,WidthΪ���ط�����,nCol��ʾ��˳���о������,�麯��,��������
	virtual int	GetCellLeft(int nCol,int & nWidth) { return 0;}
	//�����г�����
	virtual void SetDomain(int nBlockFlag,int WhichType);
};

#endif
