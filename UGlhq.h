#ifndef _U_GLHQ_H_
#define _U_GLHQ_H_

#include "Ubase.h"
#include "dde.h"
#include "MyScrollbar.h"
#include "SwitchEx.h"

#define GLHQ_VSCROLL_WID	15
#define DEF_GLHQ_ORDER		3

#define GLHQ_BLOCK_GP		0	//������������
#define GLHQ_ONLYGP			1	//����������

class UGlHq : public UBase
{
	void	SetControlPos();
	void	GetSomeSize(CDC *pDC);
	void	AdjustRowColNum();
	int		GetItemWidth(int ColID);
	int		GetCellLeft(int nCol,int & nWidth);
	LPSTR	GetItemTitle(int ColID);
	void	ToSomeRow(int row);

	void	DrawGrid(CDC *pDC);
	void	DrawTitle(CDC *pDC);

	void	DrawGPName(CDC *pDC,int row,int col,char *lpszStr,COLORREF color,BOOL LeftFlag,BOOL SameWidth,MemStkInfo *pStock);
	void	DrawCellStringText(CDC *pDC,int row,int col,int PosFlag,int PosFlag2,COLORREF color,char *fmt,...);
	void	DrawItem(CDC *pDC,int row,int col,int xsflag,MemStkInfo * pInfo,CurrStockData * hqp,EXCURRDATA *hqpex);
	void	DrawHighLine(CDC *pDC);	

	void	OnClick(CPoint point);
	void	OnLButtonDown(CPoint point);
	float	fGetSortValue(int nSort_Type,short xh,short tag);
	void	Sort();

	void	AfterSetStock();
	void	SubscribleHQ();
	BOOL	IsHideStatus();
public:
	UGlHq(LPCOMINFO pComInfo);
	virtual ~UGlHq();
	BOOL	Create(CWnd *pParentWnd,int nType);
	//
	void	ReadData(BOOL bReqData);
	void	DrawIt(CDC *pDC);
	int		ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

	int		GetGPDomain(short *lpnStkIndex,int nMaxNum);
	void	SetGPDomain(short *lpnStkIndex,int nStkNum);
	short	GetCurrentGPIndex();
private:
	CSize	FontSize;
	int		nTitleHeight;		//�������ĸ߶�
	int		nCellHeight;		//�������ĸ߶�
	int		nXHColWidth;		//������Ŀ��

	CRect	m_GPRect;
	int		m_nType;
	int		m_nSwitchHeight;	//�л����ĸ߶�

	int		nTotalRow;			//���й�Ʊ����
	int     nTotalCol;			//������Ŀ����
	long *	m_aGlHqIdx;			//��Ŀ���

	int		nStartRow;			//��ҳ��һ��  
	int     nRowNum;			//������ʵ������ʾ�Ĺ�Ʊ����
	int		nEndRow;			//��ҳ���һ��
	int     nCurRow;			//��ǰ��

	int		nStartCol;			//��һ��
	int		nColNum;			//����
	int     nEndCol;			//���һ��
	
	int		nCurSortIndex;		//����������ţ����ڵ�ǰ��Ŀ˳��
	BOOL	bAscendSort;		//�Ƿ�����

	short		lpnUsedStkIndex[MAXGPNUM];
	short		Raw_lpnUsedStkIndex[MAXGPNUM];
	
	short		lpnUsedRowIndex[MAXGPNUM];
	short		Raw_lpnUsedRowIndex[MAXGPNUM];

	INBLOCKINFO m_BlockInf[MAX_INBLOCK_NUM];	
	int			m_nBlockNum;
	int			m_nCurOrder;

	CRect		m_rcBlockHome;

	CMyScrollBar	*m_pVScrollBar;	
	CNewZfjsTab		*m_pSwitchBar;
};

#endif
