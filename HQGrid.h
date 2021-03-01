#ifndef _HQGRID_H
#define _HQGRID_H

#include "Grid.h"
#include "DDE.h"

#define GetStatData_And_WriteFloat(ftmp){fStatData = ftmp; if(m_pWriteFile) m_WriteStr+=FloatToStr(ftmp);}

#define GetStatData_And_WriteInt(ltmp)  {fStatData = ltmp; if(m_pWriteFile) m_WriteStr+=IntToStr(ltmp);}

#define DrawEmpty_And_WriteEmpty   {DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);\
									if(m_pWriteFile) m_WriteStr+=MEANLESS_STR;}

enum STAT_TYPE//����ͳ�Ʒ���
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
	TYPE_STRING=0,	//�ַ�������
	TYPE_FLOAT,		//��������
	TYPE_INT,		//��������
};

enum HEAD_CALC_TYPE
{
	CALC_STATIC=0,	//���뾲̬
	CALC_DYNA_TXT,	//ʵʱ�仯���ı�
	CALC_DYNA_NUM,	//����ʽ�õ���ʵʱ�仯������,�ݲ�֧��
	CALC_COM,		//�ۺϼ���,�ݲ�֧��
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
	int v_align;	//��ֱ������뷽ʽ,0:��,1:��,2:��
	int h_align;	//ˮƽ������뷽ʽ,0:��,1:��,2:��
	COLORREF color;	//�ı���ɫ
	int xsflag;		//���Ǹ�����,������С��λ��
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
	int				nHeadID;		//�ۼ�,init��̶�
	char			strTitle[41];	
	short			width;			//��ȣ��ַ����
	HEAD_DATA_TYPE	dataType;		//��������
	HEAD_CALC_TYPE	calcType;		//���ݼ�������
	UINT			dynaID;	  		//���������ֶ�,0~TOTAL_COL-1Ϊ��̬�����ֶ�,TOTAL_COL~2*TOTAL_COL-1Ϊ��̬�ֶ�,2*TOTAL_COL~3*TOTAL_COL-1Ϊ��̬��ʽ�ֶ�
	CString strFormula;				//calcTypeΪCALC_DYNA_NUMʱ��¼��ʽ,ΪCALC_STATICʱ��¼�ֶ���
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
	int			nSort_Type;					//������Ŀ
	int			RightClickWhichCol;			//��ǰ�û����ĸ���Ŀ�ϵ������

	float		LastNow[MAXGPNUM], LastBuyp[MAXGPNUM], LastSellp[MAXGPNUM];			//��һ��ˢ�µ�����
	long		LastVol[MAXGPNUM]; //�ϴ�ˢ�µ�����
	int			m_nCurPic;

	//����ʵ���Ϸŵĳ�Ա����
	BOOL		m_bStartDrag,m_bDragging;
	CPoint		m_DragPoint;
	int			DraggingGPNo;

	BOOL		m_bAutoPage;
	BOOL		m_bInfoConnect;
	
	//��������
	int					m_ItemTextFormat[MAX_FYCOL];//���ڷ�������ʱ����ÿ�е����ֶ��뷽ʽ,0:��,1:����,2:��
	vector<ColHeadInfo>	m_vBaseHeads;				//init���˳��
	long				m_FYCol[TOTAL_COL];
	vector<CString>		m_vBaseRowStr;				//����ԭʼ��Ϣ
	map<char*, DynaHQItem, cmpstrs> m_DynaIndex;
	int					m_nGPCode;					//��Ʊ�����к�,Ϊ-1��ʾ����head����������Ʊ����
	BOOL				m_LoadFyOK;					//���ذ���ɹ�,�������ƻ��Ʋ���

	//��ӡ����
	CRect			m_PRect;
	vector<int>		m_PColPerPage;
	int				m_nPRowPerPage;

	BOOL			m_bCanMoveGrid;
	BOOL			m_bMovingGrid;
	int				m_nMoveItem;
	
	//ͳ������
	float			m_fAver[2*TOTAL_COL],m_fSum[2*TOTAL_COL],m_fMax[2*TOTAL_COL],m_fMin[2*TOTAL_COL],m_fStd[2*TOTAL_COL]; //ͳ��
	float			m_fQZ[2*TOTAL_COL], m_fStdQZ[2*TOTAL_COL]; //Ȩ��
	short			m_nStatistFlag[STAT_LASTNULL];	//ͳ����Ŀ

	LinkedSetInfo	m_LSetInfo;
public:
	long	*	m_lpnCol;			//��ĿID
	int			m_nColNum;			//����Ŀ��
	float		*m_fpItemWidth;

	short		Sort_lpnUsedStkIndex[MAXGPNUM];		//����Զ�̰�ı�������
	short		Fy_nTag[MAXGPNUM];					//���ڷ��ư�Ķ�λ
	BOOL		m_bRefreshDraw;	
	UINT		m_nGridType;

	BOOL			m_bDDEStatus;	//�Ƿ���DDE�鿴״̬
	BOOL			m_bAJHStatus;	//�Ƿ���A+H�鿴״̬
	BOOL			m_bJSZJStatus;	//�ʽ�
	BOOL			m_bSigStatus;	//�ź�
	int				m_nConStatus;	//ѡ��
	int				m_nConFiltCount;//����

public:
	HQGrid(LPCOMINFO pComInfo);
	virtual ~HQGrid();

	void	ReSetRefreshTime();
	void	OnlySetDomain(int nBlockFlag,int WhichType);
	void	ResetRowCol();
	void	SetDomain(int nBlockFlag,int WhichType,BOOL bSort,int SortType,BOOL AscendSort);
	void	InitGrid(BOOL bSort,int SortType,BOOL AscendSort);

	void	DrawRemoteGrid(CDC *pDC);
	void	DrawGrid(CDC *pDC,BOOL bRealDraw=TRUE);	//����ͼ����
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

	void	ProcessHQRefresh();		//�����������
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

	//���Ƶ���
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
	void	TransID2Str(UINT nID,MemStkInfo *pInfo,CurrStockData * hqp,EXCURRDATA *hqpex,LPDDEREPORT pDDEReport,FormatInfo &fi,float &float_out,CString &rs,CString &e_rs);	//����������ȡ��̬��(���ּ�)���ַ�����ʽ�͸�����ʽ��ֵ

	//��ӡ
	void	SetPrintRect(CRect rect) {m_PRect = rect;}
	void	CalcPrintPage(CDC* pDC, CPrintInfo* pInfo);

	void	Sort(void);				//������

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
	void	GetData();								//������������
	void	FiltConFlag();

	void	DrawA2HItem(CDC *pDC,int row,int col,int xsflag,MemStkInfo * pInfo_hk,CurrStockData * hqp_hk,EXCURRDATA *hqpex_hk, MemStkInfo * pInfo,CurrStockData * hqp,EXCURRDATA *hqpex);
	void	DrawItem(CDC *pDC,int row,int col,int xsflag,MemStkInfo * pInfo,CurrStockData * hqp,EXCURRDATA *hqpEx=NULL);
	void	DrawHZ(CDC *pDC);
	void	DrawTitle(CDC *pDC);
	void	DrawBlueRect(CDC *pDC,int row,int col,float f1,int xsflag,BOOL bNarrow=FALSE);

	void	CalcItem(int row,int col,int xsflag,MemStkInfo * pInfo,CurrStockData * hqp,EXCURRDATA *hqpex,float fQZ,int nCalcWhich=0);	//nCalcWhich 0-��һ�� 1-�ڶ���(���õ�һ������)
	void	DrawHZItem(CDC *pDC,short nHZType,float	*fCellData,int row,int col,int xsflag);

	int		GetCellLeft(int nCol,int & nWidth);		//�õ���������λ��
	float	fGetSortValue(short xh);				//�õ�����Floatֵ(��������Ʊ����)
	void	OnCtrlLeft();

	BOOL	HitTestGrid(int &nHitItem, CPoint point);
	void	ChangeGPInGrid(int SrcGPNo,int DestGPNo,BOOL bShift);

	void	DrawSortHighLine(CDC *pDC);
};

#endif
