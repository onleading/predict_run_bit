#ifndef UZST_H_
#define UZST_H_

#include "Ubase.h"
#include "HToolTipEx.h"
#include "levin.h"
#include "SwitchEx.h"

#include "DTotalOrder.h"
#include "DCancelOrder.h"
#include "DCjbsStat.h"
#include "DCjtjStat.h"

#define ZST_AUCTIONWIDTH	90
#define ZST_LWIDTH			55
#define ZST_RWIDTH			50
#define ZST_THEIGHT			20
#define ZST_BHEIGHT			18

#define COLORBTN_WIDTH	48
#define COLORBTN_HEIGHT	22

typedef struct 
{
	BYTE	bExist;			//�Ƿ���ѡ�е�
	BYTE	IsMovable;		//����ʱ�ܷ��������ƶ�״̬ ! �����Ҽ�����ʱ����λ 0/1 .
	BYTE	nInRegion;		//���Ǹ�����
	BYTE	nSet;			//����(����,�Ի��ߵ�,��ʱ���ò��ϵ�)
	short	nSelfLineNo;	//ɾ���ߣ��ݼƱ��
	short	nInOnePoint;	//����ĳ������ 0 - �� ,1 - ��һ���� ,2 - �ڶ����� ....
	POINT	nBeginPt;		//�����ƽ�ƣ���¼��λ��
	BYTE	bMoved;			//���δ�ƶ�
}ZSTSELINFO;

struct thunderpos
{
	short	tdxmin;
	short	thundernum;
	short	firstpos;
	short   type;	//0:һ�� 1:ȯ�̷���
	short	minetoken;	//���ױ������
};

#define	DC_DRAW_TEXT(Color,lpString,Offset)\
	size = g_d.DisplayText(pDC,pos,3,VipColor.##Color,_F(lpString)); pos+=size.cx+Offset;

class UZst : public UBase
{
	BOOL		m_bPlayStatus;					//�ǲ����ڷ�ʱ�ز�״̬
	int			m_nCurMinute;					//��ǰ�ķ�����
	int			m_nRealTotalMin;				//һ�����������ݸ���
	int			m_nTodayMin;					//�������е����ݸ���

	BOOL		m_bTotalSect;					//�Ƿ�������
	BOOL		m_bDrawCross;					//�Ƿ�����
	int			yStart;							//��ʼ����ʱͼ��λ��(����)
	int			yBasePrice;						//��ʱͼ�۸��м�λ��(����)
	int			yVol;							//��ʼ���ɽ�����λ��(����)
	int			yEndVol;						//�������ɽ�����λ��(����)
	int			yStartBot;						//��ʼ����չͼ��λ��(�еĻ�)
	int			yEndBot;						//��������չͼ��λ��(�еĻ�)
	int			xAucStart,xStart;				//��ʼ����ʱͼ��λ��(����)
	int			xEnd;							//��������ʱͼ��λ��(����)
	int			nPriceLnum,nVolLnum,nBotLnum;	//�������ڵ��߸���
	double		fPointPerMinute;				//X �����굥λ
	////////////////////////////////////////////////////////////////////////////////
	double		fBasePrice;						//��ʱͼ����۸�
	double		fMaxDelta;						//���ɽ���λ���,������Ĳ�����
	double		fMaxPxOff;						//����λ���(�ɽ�,�����м��Ͼ���)
	double		fMaxLx;							//����������ֵ
	//
	long		m_lMaxVol;						//�������ֵ
	long		m_lMaxVolEx;					//�������ֵ(�ɽ�,�����м��Ͼ���)
	long		m_lMaxQHPos,m_lMinQHPos;		//�ֲ���
	//ָ����
	//1.ֻ��ʾ�ɽ���;2.����/���;3.ί��/ADL;4.���Ͼ���;5.��������;6.�ɽ���ͳ��;7.��ͳ��;8.�ɽ�����ͳ��;9.�󵥱���ͳ��;10.�����ҵ�;11.�ۼƳ���
	double		m_fMaxLB,m_fMinLB;				//����,���(����)�������С
	long		m_lMaxADL;						//��������(ί��),ADL(�ǵ�����)�����
	double		m_lMaxVirtualPxOff;				//���Ͼ�������ƥ���λ���
	long		m_lMaxVirtualVol;				//���Ͼ�������ƥ���������ֵ
	long		m_lMaxBSVol;					//��ί(��/��)���
	long		m_lMaxOaCVol;					//�������ҳ���
	double		m_fMaxCancelSum;				//�ۼƳ���(��/��)���
	long		m_lMaxCjbs;						//�ɽ������ۼ����
	long		m_lMaxLargeCjbs;				//�󵥳ɽ������ۼ����
	double		m_fMaxIOVol;					//�����̳ɽ����ּ�
	double		m_fMaxLargeVol;					//�󵥳ɽ����ۼ����
	double		m_fMaxDDZ;
	double		m_fMinDDZ;
	//����ϵ��
	CurrStockData		m_Hq;
	EXCURRDATA			m_HqEx;
	struct MinuteData	m_MinuteData[1500];
	float				m_LBdata[1500];			//����
	short				m_nLeadData;			//�������ݸ���
	BOOL				m_bLeadReqed;
	NORMALSTAT			m_NormalStat[1500];
	short				m_nNormalStat;
	TICKSTAT			m_TickStat[1500];
	short				m_nTickStat;
	VTLDATA				m_pVtlData[MAX_AUCTIONNUM];
	short				m_nVtlData;
	//����ʱ���
	int					nSectTime[4*MAX_ZST_MANYDAY][2];//���4*MAX_ZST_MANYDAY��
	int					nSectNum;						//����
	int					nNowSect;						//�����ǵڼ���,���԰볡��Ч
	int					nSecLens;						//���зֳ����ܼƷ�����(�ֳ�ʱ��)
	//�Ƿ�������
	BOOL				m_bZsLx;
	//����ʮ�ֹ����
	BOOL				m_bInSwim;
	BOOL				m_bCrossMode;
	BOOL				m_bHasCross;
	CPoint				m_CrossPoint;
	BOOL				m_bKeyCross;
	long				m_nKeyMin;
	CPoint				m_RealMove;
	//����ά��
	long				m_lLastMinVolume;	 //��һ���ӵ���	
	int					m_nLastRefreshMinute;//�ϴμ�¼�ķ�����
	int					m_nLastAuctionMinute;//��һ�θ��¼��Ͼ���ʱ��(����)
	//Զ�̲�����
	BOOL				m_bDataReady;
	//��Ѷ������
	struct ext_info_title  *m_pTitle;
	int						m_nTitle;
	struct ext_ggbw_title  *m_pTitle2;
	int						m_nTitle2;
	struct thunderpos		m_thunderpos[1500];
	int						m_thunderposnum;
	int						m_jumpPos;		 //�ϴ�������λ��
	BOOL					m_bManualPop;
	//��Ʊ����
	short				m_nOverlapNum;	
	MemStkInfo *		m_pOverlapStkInfo[3];//������������ֻ��Ʊ
	CurrStockData		m_OverlapHq[3];
	struct MinuteData	m_OverlapMinuteData[3][1500];
	//���շ�ʱ
	BOOL				m_bManyDayZST;		//��������ͼ
	int					m_nZSTManyDay;		//��������ͼ������
	struct MinuteData	m_ManyDayMinuteData[MAX_ZST_MANYDAY][1500];
	float				m_fManyDayClose[MAX_ZST_MANYDAY];
	long				m_ManyDayDate[MAX_ZST_MANYDAY];
	//�۸�Ԥ����
	float				m_fWarnPriceH, m_fWarnPriceL;
	//����Ҽ�ͳ��
	CPoint				m_ArchorPoint;
	CPoint				m_OrgPoint;
	int					m_nQJPos1;
	int					m_nQJPos2;
	//�������
	CRect				m_QSRelativeRect;				//ȯ�̶�����ҳλ��
	CRect				m_RelativeRect[MAX_CUSWEB_NUM];	//��ҳλ��
	CRect				m_ViewZoomRect;					//���ԭλ��
	CRect				m_ReportBtnRect;				//�о�����ƽ̨�İ�ť
	CRect				m_GubaBtnRect;					//�ɰɰ�ť
	CRect				m_FlushRealInfoRect;			//��˸��λ��
	CRect				m_YMDCfgRect;					//Ԥ��λ��
	CRect				m_TokenOnOffRect;				//��ʾ��ȥ�����µ�ͼ��
	CRect				m_CloseOpenRect;				//���տ�����Ϊ�����λ��
	BOOL				m_bFlushRealInfo;				//�Ƿ���˸��ʱ����
	BOOL				m_bShowLayout;					//�Ƿ������½���ʾ"������ť"
	BOOL				m_bHasSwitch;					//�Ƿ����л���
	
	CRect				m_PeriodRc;
	//�ر�ָ����
	DTotalOrder			m_dTotalOrder;					//��������
	DCancelOrder		m_dCancelOrder;					//���Ҿ���,�����ۼ�
	DCjbsStat			m_dCjbsStat;					//�ɽ������ۼ�,�󵥱����ۼ�
	DCjtjStat			m_dCjtjStat;					//�󵥳ɽ����ۼ�
protected:
	///////////////////////////���߹���
	DRAWLINE			m_NewDrawLine;					//��ӻ���ʱ����ʱ��Ϣ(����֮��ű���)
	ZSTSELINFO			m_SelectInfo;
	BOOL				m_bEvenSelected;
	//
	int					m_nPtNum;
	BOOL				m_bDraw2nd;
	BOOL				m_bDraw3rd;
	BOOL				m_bDrawLine;					//�Ƿ���
	UINT				m_nWay;							//��������
	CRect				m_PtInArea;						//��CRect���棬���Գ�������亯��,�������
	CRect				m_AbsPtInArea;					//��������
	int					m_nSelfLineIn;					//�������ĸ�������(�۸�/�ɽ���/��չ)
	//
	CPoint				m_Point[MAX_LINEPOINTNUM*6];	//һ��ֻ�ò�����MAX_LINEPOINTNUM,������Ϊ������,����������ʼ��
	//
	_inline void	ResetSelfLine()
	{		
		m_bDrawLine		= FALSE;
		m_nWay			= 0;
		m_nPtNum		= 0;
		m_bDraw2nd		= FALSE;
		m_bDraw3rd		= FALSE;
		m_nSelfLineIn	=  0;
		m_AbsPtInArea	= 
		m_PtInArea		= CRect(0,0,0,0);
		m_Point[0]		= 
		m_Point[1]		= 
		m_Point[2]		= 
		m_Point[3]		= 
		m_Point[4]		= CPoint(0,0);
		memset(&m_NewDrawLine,0,sizeof(DRAWLINE));
	};
	//
	void	ShowSelfLine(CDC * pDC);
	BOOL	BeginDrawSelfLine(CDC * pDC,CPoint point );
	void	MoveDrawPanRatio( CDC * pDC,int nNo,BOOL bShow=FALSE,int nDrawMode=R2_XORPEN);
	void	DrawingMouseMove(CDC * pDC,CPoint point,int nNo);
	void	SelfLineChg(CDC * pDC,CPoint point);
	void	TransSelfLine(CPoint point,int nNo,int nWhitch);
	CPoint	UTransToPoint(int nNo,int nWhitch);
	int		SelectSelfLine(CPoint point);
	BOOL	CalcLinear(long xPosS,long xPosE);
	//
	long	GetTimeIndexFromX(long x);				//����x�������õ�ʱ������
	long	GetXFromTimeIndex(long ti);				//����ʱ�������õ�x�������
	//
	float	GetPxValFromY(long y);					//����y�������õ��۸�ֵ
	long	GetYFromPxVal(float fVal);				//���ݼ۸�ֵ�õ�y�������
public:
	UINT	GetSelfLine();
	void	SetSelfLine(UINT nID);
	///////////////////////////���߹��߽���///////////////////////////////
protected:
	BOOL				m_bSelected;					//�Ƿ�ѡ��,ֻ����ZST_MULTI
	BOOL				m_bYestodayFalg;				//�Ƿ���������,�����赥���ж�
	BOOL				m_bUseOpenAxisForIPO;			//�¹��Ƿ���ý�Ϊ����

	//�������µ���ص�
	struct LevinGN_WTCJListANS	*m_pWTList;
	struct LevinGN_GPListANS	*m_pGPList;
	struct MAIDAN				*m_pYmdList;
	long						m_lCanUseVol;	
	
	RECT		m_pWTRect[MAX_WTCJLIST_NUM];
	RECT		m_pYmdRect[MAX_YMD_NUM];
	int			m_nWTNum,m_nGPNum,m_nYmdNum;
	int			m_nCurWT,m_nCurYmd,m_nCurCopyWT;
private:
	//���Ʊ���ӺͶ��շ�ʱͼ��صĺ���
	void	Overlap_ReadCache();
	void	Overlap_ReqData();
	void	Overlap_ReqHis();
	void	ManyDay_ReadCache(long lNowRq);
	void	ManyDay_ReqData(long lNowRq);
	short	GetOverlapGp(MemStkInfo **pInfo, short nMaxGp);
	void	AutoOverlapGP();
	void	ReCalcZSAverage();
	//��������
	void	SetSwitchLabel();
	BOOL	IsSameOpenCloseTimer(MemStkInfo *Info1,MemStkInfo *Info2);
	void	SetBasePrice();
	void	DrawQJLine(CDC *pDC);
	void	GetQzLinkInfo(char *pInfo,MemStkInfo *pStkInfo);
	//
	void	CalcIt();
	void	DTWH_currMinute();

	void	ToNextMin(int nFlag,int nSpace=0);						//���Ҽ�ͷ�ƶ�ʱ����
	void	InSwimArea(CDC *pInDC,CPoint pt,BOOL bInView=TRUE);		//���Ʊ�
	void	LeaveSwimArea(CDC *pInDC);								//�뿪�Ʊ���ʱ
	void	DrawAddInfo(CDC *pInDC,int xPos,BOOL bShowLast);
	void	DrawInfoArea(CDC *pInDC,int xPos,BOOL bInView=TRUE);	//����Ϣ��
	void	LonelyDrawAxis(CDC *pInDC,int nWhich,BOOL bSwim=FALSE,int DrawPos=0);				//��������
	void	DrawAxisArea(CDC *pDC,CPoint RelaPoint,int nWhich,BOOL bSwim=FALSE,int DrawPos=0);	//���ắ��
	void	DrawFrame(CDC *pDC);			//�����

	int		GetTimeFromX(double x);			//����x����õ�����ʱ��
	int		GetNowYFromX(double x);
	int		GetYFromValue(double value);	//���ݼ�λ�õ�Y����
	int		GetYLxFromValue(double value);
	int		GetYVolFromValue(double value,BOOL bVolume=TRUE);
	int		GetYLbFromValue(double value);
	int		GetYMmldFromValue(long value);
	int		GetYDiffFromValue(long value) ;

	int		GetXFromMinute(int minute,int flag=-1);	//���ݾ��Է�������X����
	short	GetNowTMinute();
	int		GetXFromTickNo(int nTickNo);

	void	GetPosInfo(int nPos, BYTE &nHisXh, short &nMinXh);	
public:
	HToolTipEx		m_ToolTip,m_LevinToolTip;
	BOOL			m_bBeginRectangle;
	CNewZfjsTab	*	m_pSwitch;
	short			m_nSwitchBtn;

	UZst(LPCOMINFO pComInfo);
	~UZst();

	void	SetStyle(int style);
	int		GetStyle()				{ return m_nStyle; }
	BOOL	GetDataReady()			{ return m_bDataReady; }
	long	GetRealTotalMinuteNum()	{ return m_nRealTotalMin; }
	long	GetFirstMinute(void)	{ return nSectTime[0][0]; }
	long	GetLastMinute(void)		{ return nSectTime[nSectNum-1][1]; }
	long	Xh2TdxMinute(int nXh)	{ return System2Tdx(nXh, TimeSec); }
	int		GetOverlapNum()			{ return m_nOverlapNum; }

	MemStkInfo *GetOverlapMemStkInfo(int which);

	long		GetDate(int whichHis);
	void		ReadData(BOOL bDataReady=TRUE,BOOL bReqData=TRUE);
	void		DrawIt(CDC *pDC);
	int			ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};

#endif
