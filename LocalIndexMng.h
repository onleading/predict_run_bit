#ifndef __LOCALINDEXMANAGE__H__
#define __LOCALINDEXMANAGE__H__

#define LOCALINDEX_MAXSPACE		200			//���������Զ���Ʒ�ָ���
#define	INDEXSETSPACE_STEP		20			//�Զ�Ʒ�ִ洢���������벽��
//
#define	LCEXTFORMAT_SKIP		0
#define	LCEXTFORMAT_DATE		1
#define	LCEXTFORMAT_OPEN		2
#define	LCEXTFORMAT_HIGH		3
#define	LCEXTFORMAT_LOW			4
#define	LCEXTFORMAT_CLOSE		5
#define	LCEXTFORMAT_VOLUME		6
#define	LCEXTFORMAT_AMOUNT		7

#define	LCEXTDATE__M__D__Y		0			//MM/DD/YYYY
#define	LCEXTDATE__D__M__Y		1			//DD/MM/YYYY
#define	LCEXTDATE__Y__M__D		2			//YYYY/MM/DD
#define	LCEXTDATE_M_D_Y			3			//MM-DD-YYYY
#define	LCEXTDATE_D_M_Y			4			//DD-MM-YYYY
#define	LCEXTDATE_Y_M_D			5			//YYYY-MM-DD
#define	LCEXTDATEYMD			6			//YYYYMMDD

#define	LCEXTVOL_1P1			0
#define	LCEXTVOL_1P10			1
#define	LCEXTVOL_1P100			2
#define	LCEXTVOL_1P1000			3
#define	LCEXTVOL_1P10000		4

#define	LCEXTSPACE_COMMA		0
#define	LCEXTSPACE_SEM			1
#define	LCEXTSPACE_TAB			2
#define	LCEXTSPACE_SPACE		3
//
#define	INDEXSTKSPACE_STEP		50			//�Զ�Ʒ�ֳɷֹɻ������벽��
#define	INDEXDAYSPACE_STEP		300			//�������ݻ������벽��
#define CWDATASPACE_STEP		100			//�������ݻ������벽��

#define LCIDXWEIGHT_FLOWCAP		0			//��ͨ�ɱ���Ȩ
#define LCIDXWEIGHT_ADJUSTCAP	1			//������ͨ�ɱ���Ȩ
#define LCIDXWEIGHT_TOTALCAP	2			//�ܹɱ���Ȩ
#define LCIDXWEIGHT_PRICE_NOTQ	3			//����Ȩ�۸�Ȩ��
#define LCIDXWEIGHT_PRICE_EFTQ	4			//�ȱ�ǰ��Ȩ�۸�Ȩ��
#define LCIDXWEIGHT_PRICE_EBTQ	5			//�ȱȺ�Ȩ�۸�Ȩ��
#define LCIDXWEIGHT_SAME		6			//�ȱȸ�ȨͬȨ��

#define NEWLCIDXSPACE(bRet,pt,spc,req,step,type) {\
	if((req)<=0) (bRet) = FALSE;\
	if((bRet) && ((pt)==NULL || (spc)<(req)))\
	{\
		TDELA(pt);\
		(spc) = 0;\
		(pt) = new type[(req)+(step)];\
		if((pt)==NULL) (bRet) = FALSE;\
		else (spc) = (req)+(step);\
	}\
}

#define CHECKLCIDXSPACE(bRet,pt,tmppt,spc,req,step,type) {\
	if((req)<=0) (bRet) = FALSE;\
	if((bRet) && ((pt)==NULL || (spc)<(req)))\
	{\
		(tmppt) = new type[(req)+(step)];\
		if((tmppt)==NULL) (bRet) = FALSE;\
		else\
		{\
			memset((tmppt),0,((req)+(step))*sizeof(type));\
			if((spc)>0 && (pt)!=NULL) memcpy((tmppt),(pt),(spc)*sizeof(type));\
			TDELA(pt);\
			(pt) = (tmppt);\
			(spc) = (req)+(step);\
		}\
	}\
}

//����ļ�:������Ϣ�ļ�(lcpts.lpi),������Ϣ�ļ�(lcext.lei),�����Ϣ�ļ�(lcidx.lii),��Ϲ�Ʊ��(393XXX.lis)
//����:��ȡlcpts.lpi,lcext.lei��lcidx.lii,������˲����ɶ���Ʒ������,��ʼ��˳���
//����:����:���ۺ������ͷ�����Ϣĩβ���,ͬʱ�ӵ�������ĩβ;ɾ��:Ѱ���ڴ������ͷ�����Ϣ�е�λ��,�ֱ�ɾ��,���ۺ�������ɾ��������˳���
//
typedef struct LocalSecIndex		//			����Ʒ������
{
	long	lType;					//			����Ʒ�����:1:���Ʒ��;2:�ⲿƷ��;3:����Ʒ��
	char	Code[CODE_SIZE];	//			����Ʒ�ִ���
	char	Name[NAME_SIZE];		//			���
	long	lMemStkIdx;				//			����Ʒ���ڴ������е�λ��
	long	lSubIndex;				//			����Ʒ���ڶ��Ʒ����е�λ��
}LCSECIDX,*LPLCSECIDX;
//
typedef struct LocalPtSet			//	320B	����Ʒ�ֻ�����Ϣ
{
	char	Code[CODE_SIZE];	//	7B		����
	char	Name[NAME_SIZE];		//	9B		���
	short	BaseSetcode;			//	2B		ԭʼ�г�
	char	BaseCode[CODE_SIZE];//	7B		ԭʼ����
	char	Reserved[295];			//	295B
}LCPTSET,*LPLCPTSET;
//
typedef struct LocalExtSet			//	320B	�ⲿƷ�ֻ�����Ϣ
{
	char	Code[CODE_SIZE];	//	7B		����
	char	Name[NAME_SIZE];		//	9B		���
	short	nDateFormat;			//	2B		���ڸ�ʽ
	short	nSpaceType;				//	2B		�����
	long	lSkipLines;				//	4B		��������
	short	nFieldFormat[10];		//	20B		�ֶθ�ʽ
	char	Reserved[18];			//	18B
	short	nVolRatio;				//	2B		�ɽ�������
	char	SrcFilePath[256];		//	256B	Դ�ļ�·��
}LCEXTSET,*LPLCEXTSET;
//
typedef struct LocalIndexSet		//	320B	���Ʒ�ֻ�����Ϣ
{
	long	nRefreshedTime;			//	4B		�Ƿ���Ҫˢ��
	char	Code[CODE_SIZE];	//	7B		����
	char	Name[NAME_SIZE];		//	9B		���
	char	Author[32];				//	32B		�����
	long	nHYID;					//			��ҵID
	short	nUnitNum;				//	2B		��Ʒ����
	short	QZType;					//	2B		��Ȩ��ʽ:0-��ͨ��Ȩ��,1-�ܹɱ�Ȩ��,2-�Զ���Ȩ��,3-ƽ��Ȩ�ط�ʽ
	long	BaseDate;				//	4B		��׼ʱ��(ʵ������ʱ��Ӵ��տ�ʼ)
	float	BaseValue;				//	4B		��׼ֵ
	long	lGiveupDays;			//	4B		��Ȩ:ͬ��Ȩ�غ��Զ���Ȩ����Ч
	char	Descript[252];			//	252B	˵��
}LCIDXSET,*LPLCIDXSET;

typedef struct LocalIndexStock		//	16B		����Ʒ�ֳɷ���Ϣ
{
	short	nSetCode;				//	2B		�г�����
	char	Code[CODE_SIZE];	//	7B		����
	float	fWeight;				//	4B		�Զ���Ȩ��
	char	Reserved[3];			//	3B		����
}LCIDXSTK,*LPLCIDXSTK;

typedef struct LocalIndexDayData	//	48B
{
	long	lDate;					//	4B		����
	double	fOpen;					//	4B		��Ȩ����ֵ
	double	fMax;					//	4B		��Ȩ���ֵ
	double	fMin;					//	4B		��Ȩ���ֵ
	double	fClose;					//	4B		��Ȩ����ֵ
	double	fYClose;				//	4B		���������ڽ��յĳ�Ȩ����ֵ(�ȱȳ�Ȩ)
	double	fVolume;				//	4B		�ɽ���
	double	fAmount;				//	4B		�ɽ����
	unsigned short	wAdvance;		//	2B		���Ǽ���
	unsigned short	wDecline;		//	2B		�µ�����
	double	fFlowCap;				//	4B		��ͨ�ɱ�
	double	fTotalCap;				//	4B		�ܹɱ�
	double	fFinalWeight;			//	4B		����Ȩ��:��ͨ�ɱ�/������ͨ�ɱ�/�ܹɱ�/1/1/1/1
}LCIDXDAY,*LPLCIDXDAY;

#include "cwcl.h"
//�Զ���ϴ���ģ��
//1.��������,ɾ��,�����Զ���ϵľ������
//2.�����Զ���ϵ���ֵ����
//3.���Զ���ϵı༭,�ڱ༭���֮����������
//4.���Զ���ϵ���ֵ����,�����������������
//5.ɾ���Զ����ʱ,ͬʱɾ��������ֵ
//6.�޸��Զ����ʱ,���㲢���´�����ֵ
//7.�������ݺ�,��Ҫ���¼���
//8.�Զ�������ݸ���������֤��ָ�����ݸ�������,�������ݼ�����֮����֮����ʱ��ƥ��,�ۼ�.
//9.���ۼӽ����ĳ��Ϊ0,ֱ���޳�

class LcIdxMng
{
protected:
	LPLCSECIDX	m_pLcSecIdx;		//�ۺ�����
	long		m_LcSecIdxNum;
	///////////////////////////////////////////////////////////////////////////
	LPLCPTSET	m_pLcPtSet;			//����Ʒ����Ϣ
	long		m_LcPtSetNum;		//����Ʒ�ָ���
	long		m_LcPtSetSpc;		//����Ʒ����Ϣ����С(sizeof(LCPTSET)Ϊ��λ)
	///////////////////////////////////////////////////////////////////////////
	LPLCEXTSET	m_pLcExtSet;		//�ⲿƷ����Ϣ
	long		m_LcExtSetNum;		//�ⲿƷ�ָ���
	long		m_LcExtSetSpc;		//�ⲿƷ����Ϣ����С(sizeof(LCEXTSET)Ϊ��λ)
	//
	char		*m_pSrcText;		//�����ļ�����
	long		m_lSrcTextLen;		//�����ļ���С
	long		m_lSrcTextSpc;		//�����ļ������С
	///////////////////////////////////////////////////////////////////////////
	//�Զ������Ϣ
	LPLCIDXSET	m_pLcIdxSet;		//�Զ������Ϣ
	long		m_LcIdxSetNum;		//�Զ���ϸ���
	long		m_LcIdxSetSpc;		//�Զ������Ϣ����С(sizeof(LCIDXSET)Ϊ��λ)
	//�Զ���ϳɷ�(����ȫ����ʱ����),�����Զ���Ϲ���
	LPLCIDXSTK	m_pLcIdxStk;		//�Զ���ϳɷֹ���Ϣ
	long		m_LcIdxStkNum;		//�Զ���ϳɷֹɸ���
	long		m_LcIdxStkSpc;		//�Զ���ϳɷֹ���Ϣ��С(sizeof(LCIDXSTKINFO)Ϊ��λ)
	//�Զ���ϼ�����
	LPLCIDXDAY	m_pLcIdxDay;		//�Զ�����ۼӻ�����
	long		m_LcIdxDayNum;		//�����Զ�����������ݸ���
	long		m_LcIdxDaySpc;		//�Զ�����ۼӻ�������С(sizeof(LCIDXDAY)Ϊ��λ)
	//
	LPANALYDATA	m_pIndexAnalyData;	//�����������
	long		m_IndexAnalyNum;	//�����Զ�����������ݸ���
	long		m_IndexAnalySpc;	//�Զ�����ۼӻ�������С(sizeof(LCIDXDAY)Ϊ��λ)
	//�Զ�����м�����(���ɼ�����ֵ,�ۼ���ָ��)
	MemStkInfo	*m_pCurStock;		//��ǰ����Ĺ�Ʊ
	LPLCIDXDAY	m_pLcStkDay;		//������ֵ����������
	long		m_LcStkDayNum;		//�����������ݸ���
	long		m_LcStkDaySpc;		//������ֵ������������С(sizeof(LCIDXDAY)Ϊ��λ)
	//
	LPANALYDATA	m_pStockAnalyData;	//���̸�������
	long		m_StockAnalyNum;	//�����Զ�����������ݸ���
	long		m_StockAnalySpc;	//�Զ�����ۼӻ�������С(sizeof(LCIDXDAY)Ϊ��λ)
	//��Ȩ��Ϣ
	Cwcl		*m_pCwcl;
	LPCWDATA	m_pCWData;
	long		m_lCWDataNum;
	long		m_lCWDataSpc;
	//
	long		m_MaxDays;

	CCriticalSection	m_CalcSec;

public:
	LcIdxMng();
	~LcIdxMng();
	//
	BOOL	LoadAll(BOOL bInit=FALSE);
	BOOL	SaveAll();
	long	GetSecNum();
	long	GetSecIdx(LPLCSECIDX &pLcSecIdx);
	///////////////////////////////////////////////////////////
	//
	BOOL	LoadPtSet(BOOL bInit=FALSE);
	BOOL	SavePtSet();
	//
	BOOL	AddLcPt(LPLCPTSET pLcPtSet);
	BOOL	ChgLcPt(LPLCPTSET pLcPtSet,long lPtNo);
	BOOL	DelLcPt(long lPtNo);
	//
	long	GetPtNum();
	long	GetPtSet(LPLCPTSET &pLcPtSet);
	//
	BOOL	TestPtExist(char *pPtCode,long lPtNo = -1);
	
	LPLCPTSET GetLcPTSet(char *pPtCode);
	///////////////////////////////////////////////////////////
	//
	BOOL	LoadExtSet(BOOL bInit=FALSE);
	BOOL	SaveExtSet();
	BOOL	ReLoad(long lExtNo = -1);
	//
	BOOL	AddLcExt(LPLCEXTSET pLcExtSet);
	BOOL	ChgLcExt(LPLCEXTSET pLcExtSet,long lExtNo);
	BOOL	DelLcExt(long lExtNo);
	//
	long	GetExtNum();
	long	GetExtSet(LPLCEXTSET &pLcExtSet);
	//
	BOOL	TestExtExist(char *pExtCode,long lExtNo = -1);
	//////////////////////////////////////////////////////////
	BOOL	LoadIdxSet(BOOL bInit=FALSE);
	BOOL	SaveIdxSet();
	BOOL	ReCalc(long lIndexNo = -1);
	//
	BOOL	AddLcIndex(LPLCIDXSET pLcIdxSet,LPLCIDXSTK pLcIdxStk,long LcIdxStkNum);
	BOOL	ChgLcIndex(LPLCIDXSET pLcIdxSet,long lIndexNo,LPLCIDXSTK pLcIdxStk,long LcIdxStkNum);
	BOOL	DelLcIndex(long lIndexNo);
	//
	long	GetIdxNum();
	long	GetIdxSet(LPLCIDXSET &pLcIdxSet);
	BOOL	GetIdxStk(LPLCIDXSTK &pLcIdxStk,long &LcIdxStkNum,long &LcIdxStkSpc,long lIndexNo);
	BOOL	SaveIdxStk(LPLCIDXSTK pLcIdxStk,long LcIdxStkNum,LPLCIDXSET pLcIdxSet,LPLCIDXSET pOldLcIdxSet = NULL);
	void	DelIdxStk(long lIndexNo);
	LPLCIDXSET	GetIdxInfo(long lIndexNo)
	{
		if(lIndexNo>=0&&lIndexNo<GetIdxNum())
			return &m_pLcIdxSet[lIndexNo];
		return NULL;
	}
	char	*GetIdxStkCode(long lIndexNo)
	{
		if(lIndexNo>=0&&lIndexNo<GetIdxNum())
			return m_pLcIdxSet[lIndexNo].Code;
		return "";
	}
	//
	BOOL	TestIndexExist(char *pIdxCode,long lIdxNo = -1);
	BOOL	TestLCIndex(char *pIdxCode,long &lIdxNo);
	//
	void	CheckReCalc(CWnd *pView,char *pIdxCode);
	void	SetNeedReCalc();
	
	//
	long	GetLcNum();
protected:
	//
	BOOL	ReLoadExtern(long lExtNo);
	long	GetDate(char *buf,long lDateFormat);
	//
	BOOL	ReCalcIndex(long lIndexNo);
	BOOL	TQData(LPLCIDXSET pLcIdxSet,MemStkInfo *pCurStock,LPLCIDXDAY pLcStkDay,long lLcStkDayNum,LPCWDATA pCWData,long lCWDataNum);
};

extern class LcIdxMng *g_pLcIdxMng;

#endif
