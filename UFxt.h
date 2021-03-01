/*
* Copyright (c) 2002,ͨ���ſ�����
* All rights reserved.
* 
* �ļ����ƣ�UFxt.h
* �ļ���ʶ��
* ժ    Ҫ�����K��ͼ��ʾ��ָ�껭�ߡ�ƽ�Ƶȹ���.
*/
#ifndef UFXT_H_
#define UFXT_H_

#include "Ubase.h"
#include "Interface.h"
#include "InfoDlg.h"
#include "HToolTipEx.h"
#include "SwitchEx.h"

#define MAX_CWDATA		500
#define MAX_FCDATA		100

#define FXT_BHEIGHT		18
#define MAXDOTLINE		50
#define MAXOBJECT		100

#define TABSWITCH_WID	0	

#define MIN_SHOWNUM		20

#define TEXTHIGH		16
#define DMAXVAL			-99999999
#define DMINVAL			999999999999	

//�����ĸ����� 0 ָ�� 1 ���� 2 ר�� 3 K�� 4 �Զ��庯��(�������һ��.)
#define OBJECTTOP 30
enum 
{
	//ָ����
	ZB_OBJECT = ZB_TYPE,
	//����ѡ��ָʾ
	TJ_OBJECT = TJ_TYPE,
	//ר��ϵͳָʾ
	EXP_OBJECT= EXP_TYPE,
	//���K��ָʾ
	KL_OBJECT = KL_TYPE,
	//�û��Զ��������
	UF_OBJECT = UF_TYPE,
	//���⻭������
	SPECLINE_OBJECT = SEPCLINE_TYPE,
	//���߹����е��Ի���
	SELFLINE_OBJECT = OBJECTTOP,
	//�ɱ���Ǩָʾ
	GBZS_OBJECT	,
	//����
	THUN_OBJECT ,
	//���ɱ���
	GGBW_OBJECT	,
	//��������
	FC_OBJECT	,
	//��ʷ����
	HIS_DAT		,	
}; 

enum //�����Ҽ����º�Ӧ��ѡ�������
{
	ID_ANY = 0,
	ID_MAIN = 1,
	ID_SELECTNORMAL = 2,
	ID_MOVE = 3,
	ID_AXIS = 4,
	ID_PERIOD = 5,
	ID_DRAWLINE = 6,
	ID_DELOVERLAPK = 7,
	ID_SELECTEXPZS = 100,
	ID_SELECTKZS = 101
};

////////////////////////////////////////////////////////////////
//		�꺯�������ڵ���
////////////////////////////////////////////////////////////////

#define ProcessMouse(pfunc)	\
do{		\
	CPoint pt(LOWORD(lParam),HIWORD(lParam));	\
	if ( !PtInRect(pt) || pt.x>=right )	return 1;	\
	CDC *pDC = m_pView->GetDC();			\
	pfunc(pDC,pt);							\
	m_pView->ReleaseDC(pDC);				\
}while(0)

#define PushLastInfo(p)		\
do{		\
	if ( p->nSet == ZB_OBJECT )	\
	{	m_aLastInfo[m_nLastInfo].nGSIndex = p->nGSIndex;	\
		m_nLastInfo = (++m_nLastInfo)%MAX_ZBT_NUM;			\
	}\
}while(0);

#define PopLastInfo(p)		\
do{	static	nNo = 0;			\
	nNo = (++nNo)%11;			\
	long nZB = 0;		\
	if ( m_nLastInfo>0  )			\
	{	if ( m_aLastInfo[m_nLastInfo-1].nSet == ZB_OBJECT )		\
			nZB = m_aLastInfo[m_nLastInfo-1].nGSIndex ,			\
			p = AddObjectByIndex(0,nZB);		\
		memset(m_aLastInfo+m_nLastInfo-1,0,sizeof(ZBTINFO));	\
		--m_nLastInfo;											\
	}		\
	else p = AddObjectByName(ZB_OBJECT,m_strFavUseZb[nNo]);		\
}while(0)

#define ChgAxis(AXIS)	\
{	\
	m_nMainAxisStatus = AXIS;	\
	m_bChgRatio = TRUE;	\
	ProcessMsg(WM_PAINT);	\
}

//��������
#define Lock_K()	\
{	\
	if ( m_bMouseCross )	\
		m_KLock = m_nLeftNo + GetItemNoFromX(m_OldMousePoint.x);	\
	if ( m_bKeyCross )	\
		m_KLock = m_nLeftNo + GetItemNoFromX(m_OldKeyPoint.x);	\
}
#define Lock_KData()	\
{	if ( m_KLock > 0 )	\
		memcpy(&m_KAnalyData,m_pObject->pHisDay+m_KLock,sizeof(AnalyData));	\
}

//����ʮ�ֹ��
#define RejustCross()	\
if ( m_KLock != -1 && m_KLock < m_nDataNum )			\
{	CDC  * ptmpDC = m_pView->GetDC();					\
	DrawLastState(ptmpDC);								\
	m_OldKeyPoint = GetPointFromItem(m_KLock-m_nLeftNo);\
	DrawXRule(ptmpDC,m_OldKeyPoint.x,m_OldKeyPoint.y);	\
	DrawYRule(ptmpDC,m_OldKeyPoint.x,m_OldKeyPoint.y);	\
	Disp(ptmpDC,m_OldKeyPoint.x,m_OldKeyPoint.y);		\
	DrawLastState(ptmpDC);		\
	m_pView->ReleaseDC(ptmpDC);	\
}

//��λָʾ��λ��
//�����߲��ֿ���
#define LocateZS(pDC,Who,Icon,i)	\
do{	\
	(pDC)->DrawIcon(size.cx-8,m_aRegion[i].Area.top,Icon);	\
	Who##.rect.left =  size.cx;		Who##.rect.right =  size.cx+16;	\
	Who##.rect.top  =  m_aRegion[i].Area.top;			\
	Who##.rect.bottom= m_aRegion[i].Area.top+32;		\
	size.cx += 16;		\
}while(0)

#define AppendRg0(pIdx)	\
do{		\
	TOBJECT * pObj = NULL;		\
	for ( int i=0;i<m_aRegion[0].nObject;i++ )	\
		if ( m_aRegion[0].aIndexInfo[i]->nSet == pIdx->nSet && m_aRegion[0].aIndexInfo[i]->nGSIndex == pIdx->nIndexNo )	\
			break;	\
	if ( i == m_aRegion[0].nObject )	\
	{	pObj = AddObjectByIndex(pIdx->nSet,pIdx->nIndexNo);		\
		m_aRegion[0].aIndexInfo[m_aRegion[0].nObject] = pObj;	\
		RejustOneObject(pObj,m_nDataNum);						\
		CalcOne(pObj);							\
		CalcOneGSRatio(pObj,(m_nLastNo+1)-m_nDrawNum);			\
		CalcOneRegionRatio(0);									\
		m_aRegion[0].nObject++;									\
		m_bChgRatio = TRUE;					\
		ProcessMsg(WM_PAINT);				\
	}	\
}while(0)


//�����ڵ�һ��BYTE֮��0xFF
#define EXTRA_Y		0x00000100			//�ж���Y��
#define DEFINE_Y	0x00000200			//�Զ���Y������
#define PUCU_LINE   0x00000400
#define SAR_LINE	0x00000800	
#define BASE_ZERO	0x00001000

////////////////////////////////////////////////////////////////
//		UFXTʹ�õĽṹ
////////////////////////////////////////////////////////////////

//�����ĸ����� 0 ָ�� 1 ���� 2 ר�� 3 K�� 4 �Զ��庯��(�������һ��.)
//��K������Ҳ��һ������  30 -- K ������
//ע:��ʹ�����⻭��ʱ,ֻ��nGSIndex����,������ʾ����һ�ֻ�������,�����ͳͳ����,��ɾ���˶���ʱ
//Ҳ��Ҫ������Ĵ���
typedef struct  TOBJECT
{
	short	nCurGP;				//	��Ʊ���
	BYTE	nSet;				//	ָ�ꡢָʾ
	long	nGSIndex;			//  ��ʽ��ID��
	char	acCode[ZBCODE_LEN]; //  ���ڿ��ٲ�������
	BYTE	nPeriod;			//  ����
	BYTE	nRefTimes;			//  �����õĴ���
	WORD	nArrayLen;			//  ���鳤�� != nDataNum
	BYTE	nOutNum;			//	�������ֵ
	union	
	{	struct 
		{
			struct AnalyData * pHisDay;	//	��ʷ����Ҳ����һ������
			short  nMM[2];	
			short  nMaxPos[4];				//  �����������ֵλ��(��������nDrawNum����)
			short  nMinPos[4];
		};
		float  *pCalcData[MAX_LINEOUT];		//  ����õ�����,û�е���Ϊ��
	};
	double  dMaxVal;		//  �����������ֵ(��������nDrawNum����)
	double  dMinVal;
	DWORD	Property;		//  ����(���ֽڱ�ʾnDrawMode,����λ����)
	DWORD	Reserved;		//  ����(0 )
	TOBJECT* next;
}TOBJECT;

typedef struct RULERINFO
{
	BYTE	bValid;
	long	nZoom;
	double	dFrom;
	double	dStep;
	double	dDelta;
}RULERINFO;

struct  RECTINFO
{
	WORD	thObj;
	BYTE	thOut;
	RECT	aRect;
};

typedef struct  
{
	RECT	Area;			//  ��ʾ����,��������Ŀ̶��ɴ�Rgion����
	RECT    DrawRc;			//  ��������
	WORD	nObject;		//  ���ٸ�����
	double  dMaxVal;		//  ��ͼʱ�����õ�������ÿ�������ֲ�ͬ
	double  dMinVal;
	double  dPerUnit;
	double  dMaxValEx;		//  ��ͼʱ�����õ�������ÿ�������ֲ�ͬ
	double  dMinValEx;
	double  dPerUnitEx;
	RULERINFO RulerInfo;
	//  ÿ���������ڲ�ָ������,ÿ���������� MAXOBJECT ������			
	TOBJECT* aIndexInfo[MAXOBJECT];
	//  ����Ϊ�㻭�߱��������
	BYTE	nDotNum;
	BYTE	nBaseDot;
	WORD	nDotInfo[MAXDOTLINE];
	WORD	nRect;
	struct  RECTINFO aRct[58];
}TREGION;

typedef struct 
{
	BYTE	bExist;			//�Ƿ���ѡ�е�
	BYTE	IsMovable;		//����ʱ�ܷ��������ƶ�״̬ ! �����Ҽ�����ʱ����λ 0/1 .
	BYTE	nInRegion;		//���Ǹ�����
	BYTE	nSet;			//����(����,�Ի��ߵ�)
	union
	{
		//����
		struct
		{	short nInObject;	//���Ǹ�����
			long  nGSIndex;
			BYTE  nOutNo;		//���������
			BYTE  bAdd;
		};
		//�Ի���
		struct 
		{	short nSelfLineNo;	//ɾ���ߣ��ݼƱ��
			short nInOnePoint;	//����ĳ������ 0 - �� ,1 - ��һ���� ,2 - �ڶ����� ....
			POINT nBeginPt;		//�����ƽ�ƣ���¼��λ��
			BYTE  bMoved;		//���δ�ƶ�
		};
	};
}FXTSELINFO;

typedef struct
{
	BYTE	nSet;
	short   nCurGP;
	long    nGSIndex;
}ZBTINFO;

typedef struct 
{
	RECT		rect;
	TOBJECT*	p;
	char		strInfo[40];
}FXZSINFO;//ָʾ��Ϣ

class UFxt : public UBase
{
	BOOL			m_bSelected;					//�Ƿ�ѡ��,ֻ����FXT_MULTI
	CurrStockData   m_Hq; 
	EXCURRDATA		m_HqEx;

	BOOL			m_bDataReady;					//Զ�̲�����
	BOOL			m_bReqing;
	BOOL			m_bIsHead;
public:
	///////////////////////////��ʾ
	HToolTipEx		m_ToolTip;
	CInfoDlg *		m_pInfoDlg;
	BOOL			m_InfoDlgCreating;
	CNewZfjsTab	*	m_pSwitch;
	CNewZfjsTab	*	m_pTabSwitch;
	int				m_TabSel;
	///////////////////////////�����߸����ж�����
	MemStkInfo *	m_pLastStkInfo;
	int				m_LastVolume;
	float			m_LastAmount;
private:
	int				m_nValidFreqZB;					//��Ч�ĳ���ָ��
	int				m_nOverlapPenPos,m_nOverlapBrushPos;
	POINT			m_TipArchor;
	CString			m_TipInfo;						//ToolTip��ʾ����Ϣ
	BOOL			m_bOverStk;						//�Ƿ��й�Ʊ����
	int				m_nSpecProcDraw;				//���⴦��K�� 0-���� 1-�ǵ��仯K�� 2-OX
	BOOL			m_bKeepOver;
	BOOL			m_bXS3;
	CRect			m_last_rect;
private:
	int				m_nTitle;
	int				m_nTitle2;

	int				m_nFilter;						//ΪTCPIP���õı���:�ǵ������ݸ���
	///////////////////////////�����߼�
	short			m_nChgMouse;					//��귢���仯
	CRect			m_MainYRc;
	CRect			m_PeriodRc;						//���������ʾ��λ��
	POINT			m_nDownPoint;
	int				m_nAtItem;
	int				m_nAtRegion;
	double			m_dNowVal;
	//
	char			m_strZbMain[ZBNAME_LEN];
	char			m_strFavUseZb[11][ZBNAME_LEN];	//��ѡָ��ֲ���
	//
	char			m_strCom2ZbMain[ZBNAME_LEN];
	char			m_strCom2FavUseZb[11][ZBNAME_LEN];
	long			m_Com2WinNum;
	BOOL			m_Com2DrawMain;					//���ư����Ƿ���ͼָ��
	//
	short			m_NormalRegion;					//��ָͨ����ɼ��������
	BOOL			m_bUseFmlPack;					//�Ƿ��а�ָ����
	BOOL			m_bDrawMainZb;					//�Ƿ���ʾ��ͼָ��
	long			m_lBandPackNo;					//�Զ���ʱ��ģ���
	long			m_lCurPackNo;					//ѡ��ָ��ģ��ʱ��ģ���
	//
	TINDEXINFO*		m_pColorKIndex;					//���K��
	TINDEXINFO*		m_pExpertIndex;					//����ϵͳ
	TINDEXINFO*		m_pDPFXIndex;					//���̷��򣬶��صĹ���
	//
	int				m_PeriodKeyOrder;				//�������ڵı���,����"]" "[" F8��
	BOOL			m_bNotRedraw;					//��������ʱ��ˢ��
	BYTE			m_nCommonPeriod;				//��ͨ����(��ģ��)
	BYTE			m_nPeriod;						//��ǰ����
	BYTE			m_nCalcPeriod;					//��ǰ���ڶ�ӦVlaue�����ĸ�
	TOBJECT			*m_pObject;						//ָ�ꡢָʾ��һ�ಿ��
	short			m_nRegion;						//���ٸ�����
	short			m_nOldRegion;					
	short			m_nOldSelRegion;
	BOOL			m_bDbClickFT;					//˫����һ�θ�ͼ
	char			m_strDbSaveZb[15];				//�Ŵ�ʱ�õ�ָ��
	short			m_nDataNum;						//�ж��ٸ�����
	TREGION			m_aRegion[MAX_ZBT_NUM];
	float			m_aRatio[RATIONNUM];			//��Ų�ͬͼ�ı�����ϵ
													//��i��(i��1��ʼ) Start=i*(i-1)/2  Num=i
	BOOL			m_bEvenSelected;				//����ߡ�ָʾ������ѡ�й���
	FXTSELINFO		m_SelectInfo;					//��ϸ��ѡ����Ϣ��
	short			m_nSelRegion;					//Ŀǰ�����������Ϣ��
	short			m_nSelRegionBak;
	ZBTINFO         m_aLastInfo[MAX_ZBT_NUM];		//���仯��ָ��ȵļ�¼������ʱ�����ȿ��ǡ�
	short			m_nLastInfo;					//��¼���ڼ�����
	///////////////////////////ָʾ��Ϣ
	BOOL			m_bExistZS;
	FXZSINFO		m_ExpZS;
	FXZSINFO		m_KLZS;
	///////////////////////////��ͼ�������
	int				m_nNowMine;
	int				m_nWillNum;				//�û��ƻ���ʾ�����������ȵı�׼��
	int				m_nDrawNum;				//ʵ�ʻ��˶��ٸ��������ݲ���ʱ��ȡnDataNum + 3
	int				m_nTodayNum;			//��̬ά��
	int				m_nLeftNo;				//�صڼ�����ʼ
	int				m_nLastNo;				//��ǰ��������һ��,��0��ʼ��
	int				m_nTmpLeftNo;
	int				m_nTmpLastNo;
	double			m_dWidth;				//K�ߵȵĿ��
	double			m_dDistance;
	long		*	m_pPosOfX;				//һ���Լ���õ�X�����Ϣ
	long		*	m_pAllPosOfX;
	CPoint		*	m_pPoint;
	int				m_nXAxis;
	long		*	m_pXRule;
	int				m_YDy;					//��ʾY��Ŀ̶�ʱ��Ϊ����������¼
	float			m_fYVal;
	///////////////////////////����K�߹���
	AnalyData	    m_KAnalyData;
	int				m_KLock;				// -1 û��������>=0 �����ڵڼ�������0���𣬰���m_nLeftNo��������m_nDataNum��Ч��
											//��ÿ�ζ�������ʱ��������´����ݣ��Է�������Ч������Ʊͬ����ˡ�
	long			m_nToLeft;				//���㵽��ͷ�Ĵ�����
	long			m_nToRight;
	///////////////////////////
	BOOL			m_bKColor;				//�Ƿ����K��
	BOOL			m_bNowOverlap;			//�Ƿ����
	int				m_nOverlapNum;			//���ӵĹ�Ʊ����
	///////////////////////////��ʮ�ֹ����Ϣ
	POINT			m_RealMove;				//ȷ���������ƶ���
	BOOL			m_bMouseCross;
	POINT			m_OldMousePoint;
	BOOL			m_bKeyCross;
	POINT			m_OldKeyPoint;
	int				m_nKeyNo;
	/////////////////////////////��ѡ�п�
	BOOL			m_bLArchor;				//�����ʼ���	
	BOOL			m_bRArchor;				//�Ҽ���ʼ���
	BOOL			m_bBeginRectangle;
	POINT			m_ArchorPoint,m_OrgPoint;
	int				m_nRightRgn;			//�Ҽ�����
	///////////////////////////���߹���״̬��
	DRAWLINE		m_NewDrawLine;			//��ӻ���ʱ����ʱ��Ϣ(����֮��ű���)
	//
	BOOL			m_bLineValid;			//��ʱ����,�жϻ��߶�λ�����Ƿ��ܼ���õ�
	//
	int				m_nPtNum;
	BOOL			m_bDraw2nd;
	BOOL			m_bDraw3rd;
	BOOL			m_bDrawLine;			//�Ƿ���
	UINT			m_nWay;					//��������
	CRect			m_PtInArea;				//��CRect���棬���Գ�������亯����
	CRect			m_AbsPtInArea;
	int				m_nSelfLineIn;			//�������ĸ�������

	CPoint			m_Point[MAX_LINEPOINTNUM*6];	//һ��ֻ�ò�����MAX_LINEPOINTNUM,������Ϊ������,����������ʼ��

	struct AnalyData *  m_pAnalyData;		//�õ�ʱ����Ϣ,�۸��
	struct AnalyData *  m_pOrignData;
	HISDAT			 *	m_pHisData;
	struct AnalyData *  m_pTmpAnalyData;	//ת����������ʱ��������
	int					m_nTmpDataNum;		//ת����������ʱ������������
	int					m_nSHOWNUM;
	//////////////////////
	int				m_nMainAxisStatus;		//��ͼ������״̬
	double          m_dR100Max;				//�ٷֱ�������Ϣ
	int				m_bR100Dot;				//�Ƿ���С����
	///////////////////////////�����Ż��ı���
	BOOL			m_bChgRegion;			//�Ƿ���¼��������С
	BOOL			m_bChgRatio;			//�Ƿ�ı�������������ֵ��(m_nWillNum�����)
	///////////////////////////���ƽ�ƻ���
	long			m_nMousePan;					
	CWDATA			CWData[MAX_CWDATA];
	int				m_nCwNum;
	TDXSTKOLDNAME	FCData[MAX_FCDATA];
	short			m_nFCNum;

	BYTE			m_ResetZSFlag;
	int				m_nQJPos1,m_nQJPos2; 
	
	CRect			m_ViewZoomRect;
	
	struct
	{
		BOOL	bAtEage;	//ȷ���Ƿ��ڱ�Ե
		POINT	EagePoint;
		int		nWhitch;
	}m_EageInfo;
	struct			
	{
		double	dTopPercent;
		double	dTipPercent;
		double	dRatio;
	}m_R100_AXIS;
	_inline void	ResetSelfLine()
	{
		m_bDrawLine		= FALSE;
		m_nWay			= 0;
		m_nPtNum		= 0;
		m_bDraw2nd		= FALSE;
		m_bDraw3rd		= FALSE;
		m_nSelfLineIn	= 0;
		m_AbsPtInArea	= 
		m_PtInArea		= CRect(0,0,0,0);
		m_Point[0]		= 
		m_Point[1]		= 
		m_Point[2]		= 
		m_Point[3]		= 
		m_Point[4]		= CPoint(0,0);
		memset(&m_NewDrawLine,0,sizeof(DRAWLINE));
	};
	_inline void	ResetData()
	{
		m_nDataNum = g_RMSp;
		m_nWillNum  = 0;
		m_nDrawNum  = 0;
		m_bIsHead = FALSE;
		m_bDataReady = FALSE;
		TDEL(m_pOrignData);
	}
private:
	void	GetLastValue();
	void	SaveLastValue();
	BOOL	IsFxtShowTab();
	//�л�����غ���
	void	SetFormulaPack();
	void	SetFrequentZB();
	void	SetSwitchSel();
	void	ResetAllSwitch();
	//
	void	SavePriv();
	BOOL	LoadPriv();
	//��Ϣ����غ���
	void	CreateInfoDlg();
	void	DeleteInfoDlg();
	void	FillDlgInfo(int x);
	void	FillCbInfo(int nY,int nNo);
	void	PopupInfoDlg();
	//
	void	SaveSelectMany(void);
	void	GetSelectMany(void);
	//
	BOOL	GetTipInfo(int x,int y);
	//���Ʊ������صĺ���
	void	OverlapGP();
	void	AutoOverlapGP(BOOL bToOverlap);
	short	GetOverlapGp(MemStkInfo **pInfo, short nMaxGp);
	//����SUB����
	BOOL	UseLocaldata();
	void	ModifyProperty(TOBJECT *p);
	//
	void	DrawFrame(CDC *pDC);
	////////////////////////////////////ά����������
	TOBJECT* AddObjectByName(BYTE nSet,char * acCode,short nGP=-1 );
	TOBJECT* AddObjectByIndex(BYTE nSet,long nGSIndex,short nGP=-1);
	TOBJECT* AddObjectNameCode(BYTE nSet,char * acCode,char *Code=NULL,short setcode=-1);
	void	RegionDelOne(int nRegionNo,TOBJECT	* p,BOOL bKillSelf=FALSE);
	BOOL	SubObject(TOBJECT*);
	void	RejustOneObject(TOBJECT *,int nArrayLen,BOOL bTickCount=FALSE);
	void	RejustAllObject(int nArrayLen);
	void	DelObject();
	void	ChangGS(int nRegoinNo,int nGSIndex,BYTE nSet = ZB_OBJECT,int bReCalc = TRUE);
	//
	BOOL	CalcOne(TOBJECT * p);
	BOOL	CalcAllGS();
	BOOL	XPan(int ndNo,BOOL bNotQuick=FALSE);
	void	CalcR100_AXIS(int nRegion);
	BOOL	CalcOneRegionRatio(int nRegionNo);
	//
	BOOL	CalcOneGSRatio(TOBJECT * p,int Off);
	BOOL	CalcAllRatio(BOOL bRegion=TRUE,BOOL bKeepKPos=FALSE);
	TOBJECT* LocateObject(BYTE nSet,long nGSIndex,short nGP);
	////////////////////////////////��λȡֵ����
	int		GetItemNoFromX(int x);
	int		GetAbsItemNoFromX(int x);
	int		GetItemNoFromTime(long lTime);
	POINT	GetPointFromItem(int No);
	int		GetYPosR100_AXIS(int nRegionNo,double fY,int nNo=0);
	int		GetYPosOfRegion(int nRegionNo,double fY,int nNo=0);
	double	GetValFromY(int nRegionNo,int y);
	///////////////////////////
	void	InitRegion();
	_inline double GetRegEndRatio(int nNo,int nRegion);
	/////////////////////////////
	void	CalcRuleDrawAxis();
	void	CalcOneAxis(int nRegion);
	void	Disp(CDC * pDC,int nX = -1,int nY =-1,int IsMemDC=0);
	void	InnerDisp(CDC * pMDC,int nRegion,int ItemNo);
	void	DrawYRule(CDC * pDC,int nX=-1,int nY=-1);
	void	InnerDrawY(CDC * pDC,CDC * pMDC,int nRegion,int nX=-1,int nY=-1);
	void	DrawXRule(CDC * pDC,int nX=-1,int nY=-1);
	void	DrawXGrid(CDC * pDC);
	void	DrawKGap(CDC * pDC);
	void	MarkInfo(CDC * pDC);
	void	DrawZBLine(CDC * pDC,int nRegionNo,int nObject,int nOutNo,DWORD nColor, int nWidth);
	void	DrawQJLine(CDC *pDC);
	void	DrawHisLine(CDC * pDC, int nRegionNo,int nObject,int nDrawMode=0,int nFixRegion=-1,BYTE nCurrPen=MAX_PEN_NUM,BYTE nCurrBrush = MAX_BRUSH_NUM);
	void	DrawStick(CDC * pDC,int nRegionNo,int nObject,int nOutNo, DWORD nColor, int nWidth);
	void	DrawColorStick(CDC * pDC,int nRegionNo,int nObject,int nOutNo);
	void	DrawVolStick(CDC * pDC,int nRegionNo,int nObject,int nOutNo);
	void	DrawLineStick(CDC * pDC,int nRegionNo,int nObject,int nOutNo,DWORD nColor, int nWidth);
	void	DrawCircleDot(CDC *pDC, int nRegion, int nObject, int nOutNum, DWORD nColor, int nWidth);
	void	DrawCrossDot(CDC *pDC, int nRegion, int nObject, int nOutNum, DWORD nColor, int nWidth);
	void	DrawPointDot(CDC *pDC, int nRegion, int nObject, int nOutNum, DWORD nColor, int nWidth);
	void	DrawKLine(CDC *pDC, int nRegion, int nObject, int nOutNum, int nWidth);
	void	DrawKLineEx(CDC *pDC, int nRegion, int nObject, int nOutNum, int nWidth, BOOL bSelfColor, DWORD nColor);
	void	DrawPartLine(CDC * pDC,int nRegionNo,int nObject,int nOutNo,DWORD nColor, int nWidth);
	void	DrawSpecialLine(CDC *pDC, int nRegionNo,int nObject);

	void	FillBar(CDC *pDC,int left,int top,int right,int bottom,COLORREF nColor);
	void	DrawStickLine(CDC *pDC, int nRegion, int nObject, int nOutNo, DWORD nColor, int nWidth);
	void	DrawIcons(CDC *pDC, int nRegionNo, int nObject, int nOutNo);
	void	DrawIconsEx(CDC *pDC, int nRegionNo, int nObject, int nOutNo);
	void	DrawTextLine(CDC *pDC, int nRegionNo, int nObject, int nOutNo, DWORD nColor);
	void	DrawTextLineEx(CDC *pDC, int nRegionNo, int nObject, int nOutNo, DWORD nColor);
	void	DrawNumLine(CDC *pDC, int nRegionNo, int nObject, int nOutNo, DWORD nColor);
	void	DrawBand(CDC *pDC, int nRegionNo, int nObject, int nOutNo);

	COLORREF	GetColor(AnalyData * pAnaly, int& nWhichBrush);
	void		DrawOX(CDC *pDC,short col,float val1,float val2,short flag,double yunit,COLORREF color);
	float		GetAllVolume(int num);
	void		DrawPUCU(CDC * pDC,int nRegionNo,int nObject);
	void		DrawSAR(CDC * pDC,int nRegionNo,int nObject);
	//////////////////////////
	void	DrawLastState(CDC * pDC);
	void	ResetCross(CDC * pDC);
	BOOL	QuickSelect(CPoint point);
	int		IsAtEage(CPoint point);
	void	SetMyAxis();
	///////////////////////////��꺯��
	int		MouseMove(CDC * pDC,CPoint point);
	int		LButtonDown(CDC * pDC,CPoint point);
	int		LButtonUp(CDC * pDC,CPoint point);
	int		RButtonDown(CDC * pDC,CPoint point);
	int		LButtonDblClk(CDC * pDC,CPoint point);
	int		RButtonDblClk(CDC * pDC,CPoint point);
	///////////////////////////���߹��ߺ���
	//������һ�������¼���ȫ�����,�м����ȫ��������
	void	ShowSelfLine(CDC * pDC);
	BOOL	BeginDrawSelfLine(CDC * pDC,CPoint point );
	void	MoveDrawPanRatio( CDC * pDC , int nNo , BOOL bShow = FALSE,int nDrawMode = R2_XORPEN);
	void	DrawingMouseMove(CDC * pDC,CPoint point,int nNo );
	void	SelfLineChg(CDC * pDC,CPoint point);
	void	TransSelfLine(CPoint point , int nNo,int nWhitch);	//nNo=-1ʱ,��ʾ�»���
	CPoint	UTransToPoint(int nNo,int nWhitch);
	int		SelectSelfLine(int nRegion,CPoint point);
	BOOL	CalcLinear(long xPosS,long xPosE);
	///////////////////////////����ά������
	void	ReadSomeStk(TOBJECT * p);
	void	DuiQiStk(TOBJECT * p);
	void	UpdateData(BOOL bForceCalcZB=FALSE);
	////ָʾ
	void	SetZSInfo(BYTE nSet,long nGSIndex);
	BOOL	SelecZS(int nRegion,int nNo,int nItem,CPoint point);
	//
	void	TQdata(int flag);
	void	SelectMany(UINT nID);
	void	AdjustParam();
	void	ChangeMenuZB(int nID,BOOL bFromMenu=TRUE);
	void	ChangeSelfColor();
	void	DelZs();
	///
	void	TransAnalyData(BYTE nTransFlag);
	void	ProcessZBInRegion(UINT	uiMsg);
public:
	UFxt(LPCOMINFO pComInfo);
	~UFxt();

	void	WriteToTxtFile(CString filename,BOOL bWriteExcel=FALSE);
	//
	BOOL	CheckReqing()		{ return (OnLine && m_bReqing); }
	void	DelOverStk();
	BOOL	ExistZS();
	void	SetStyle(int style);
	int		GetStyle()	const	{ return m_nStyle; }
	int		GetSelected() const { return m_bSelected; }
	int		GetDrawNum() const	{ return m_nDrawNum; }
	void	SetSelfLine(UINT nID);
	UINT	GetSelfLine();
	//
	void	SetXPanState(int nType);
	int		GetXPanState() const{ return m_nMousePan; };
	//
	BOOL	ReadData(BOOL bDataReady=TRUE);
	void	DrawIt(CDC *pDC);
	void	SetPeriod(BYTE nPeriod,BOOL bReadData=TRUE,BOOL bReDraw=TRUE,BOOL bCommon=TRUE);
	void	SetCom2Fml(LPFMLPACK pCom2Fml,BOOL bDrawMain);
	void	GetCom2Fml(LPFMLPACK pCom2Fml);
	int		ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
	//ֱ�Ӳ��뺯���壬��ʡƵ������ջ��ʱ��
	_inline void DrawOneRegion(CDC * pDC,int nRegion);
	_inline BYTE GetCurrPeriod() const
	{
		return m_nPeriod;
	};
	_inline int GetWindowNum()	const
	{
		return m_nRegion;
	};
	_inline int GetAsixType()  const
	{
		return m_nMainAxisStatus;
	}
	_inline void SetSpecProcDraw(int nType) 
	{
		m_nSpecProcDraw = nType;
	}
	_inline int GetSpecProcDraw()  const
	{
		return m_nSpecProcDraw;
	}
	void	JumpToZB(char *ZBCode,int nTarRegion);
private:
	int		RButtonUp(CDC *pDC, CPoint point);
	int		ChoiceType(CPoint point);
	void	ProcessCommand(WPARAM,LPARAM);
};

#endif
