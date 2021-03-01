// MyStrategy.h: interface for the MyStrategy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYSTRATEGY_H__D3FBB36B_1683_4F32_A108_3D297934DA99__INCLUDED_)
#define AFX_MYSTRATEGY_H__D3FBB36B_1683_4F32_A108_3D297934DA99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)
#pragma warning(disable:4788)
#include <map>
#include <algorithm>
#include <functional>
using namespace std;

#define MAX_NEED_DATA	2
#define MAX_TYPE		10

enum	CalcType
{
	ZB_SEL=0,
	TJ_SEL,
	CWHQ_SEL,
	KL_SEL,
	STYLE_SEL,
	DS_SEL,

	ZB_TYPESEL=10000,
	TJ_TYPESEL,
};

enum	LineType              //����
{
	K_LINE=0,                 //K��
    CALC_LINE,                //ָ�������ȹ�ʽ������
	STATIC_LINE,              //��̬�����߶�Ӧ������Ϣ��������
	DS_LINE,                  //���ü���
	NUM_LINE                  //��һ����
};

struct CalcLine               //�������
{
	short lineType;           //����
	short lineIndex;          //�ߵ�����
	float lineValue;          //��ֵ��ȱʡΪ0

	Serialize(CArchive &ar);
};

struct SingleCon                   //��һ����
{
	int  mainType;                 //ָ������:(ָ��:0������:1���������:2��K��:3����ʽ:4)
	int  indexNo;                  //ָ��ϵͳ����
	short procIndex;               //����������,�޴�������Ϊ0
	CalcLine mainLine;             //����
	vector<CalcLine> otherLines;   //������
	long endDate;                  //ѡ��ʱ��

	CString sList;				   //ListBox���˵��

	Serialize(CArchive &ar);
};

struct Plans
{
	vector<SingleCon> myCons;          //������
	BOOL				IsAndRatio;		//
	CString           sDescript;       //����˵��
};

struct SignStrategy            //�����źŲ��ԣ����ǽ���ϵͳ��������2��
{
	UINT  pairConIndex;        //��Ӧ��������
	BOOL  bBuy;                //0:���� 1:����
	short typeDo;              //����ʽ 0:�������� 1:ͬ������ 2:����������
	long  lCount;              //�̶��������������������
	int   nCancelData;         //�Թ������ź�������:ȱʡΪ1
};

struct KStrategy              //ֹ�����
{
	short procIndex;          //����������
	int   nData;              //������
	short nBreadth;           //ֹ�����
};

struct TestStrategy
{
	Plans                   myPlans;   //������
	vector<SignStrategy>    sighStra;  //�źŷ���ʱ�Ķ�Ӧ��������
	vector<KStrategy>       ks;        //ֹ�������
};

class MyStrategy  
{
public:
	MyStrategy();
	virtual ~MyStrategy();
	
	CTime ParseCalcDate(void);   
	void CalcEndGap(void);                               //�õ�ѡ�����ھ൱ǰ����������
	void InitAllData(void);
	void UnInitAllData(void);
	BOOL GetMyAnalData(void);
	void AddSortCon(int iSort);
	int  GetListStrings(vector<CString> &aListStrings);  //�õ�ListStrings
	BOOL CompCurCon(CString &tmpStr);					 //�Ƚ���ӵ������Ƿ��ظ�
	void	PutPlanScript(CString &planScript);			 //��ŷ�������
	CString GetPlanScript(void);                         //�õ���������
	void SLPlan(CArchive &ar, BOOL bUseNewTime = TRUE);
	BOOL IsSort(void);                                   //�ж��Ƿ�����������

	void SetTQFlag(BOOL bTQ);
	BOOL HasSortCon(void);
	void ParseType(void);						//�жϸò����Ƿ����ĳ������
	BOOL GetConsInfo(void);                  //�ж�����ͷ���������
	void SetCurGpIndex(int curGpIndex);
	void InputCalcDataTime(NTime &time1,NTime &time2, int nPeriod);
	void AddSingleCon(SingleCon &conToAdd);
	void SetIsAndRatio(BOOL bIsAndRatio);
	void DelSingleCon(UINT nToDel);
	BOOL ParsCurCon(int nCurCon);                  
	int  FiltSortRes(int IsAndRadio, int TotalStockNum, int SuccessNum, short *SelectedStk);          //����������
//////////////////////////////////////////////////////////////////////
//����ӿ�
public:
	BOOL CalcMyPlan(void);
	void CalcTestStrategy(void);
	BOOL CalcFinalOut(int isAndRadio);
/////////////////////////////////////////////////////////////////////
//���ദ����
///////////////////////////�������жϺ���
	void CalcEqual(void);
	void CalcGreater(void);
	void CalcLess(void);
	void CalcUp(void);
	void CalcDn(void);
	void CalcUpCross(void);
	void CalcDnCross(void);
	void CalcTurnUp(void);
	void CalcTurnDn(void);
///////////////////////////�����ź��ж�����
	void CalcContiNoBuy(void);
	void CalcContiDayNoBuy(int cancelData);
///////////////////////////ֹ���ж�����
	void CalcMaxKS(void);
	void CalcTopKS(void);
//////////////////////////////////////////////////////////////////////
//���Ƹ�������
private:	
	short	E_OddK_Style(struct AnalyData *pAnalyData,short nSpan,short nNo,short nUpDown,short nSize,short nShadow,short nWeight,short nMode);
	short	E_TwinK_Style(struct AnalyData *pAnalyData,short nSpan,short nNo,short nKStyle,short nWeight,short nMode);
	short	isTrendK_Style(MemStkInfo *tmpInfo,struct AnalyData *pAnalyData,short nDataNum,short nSpan,short nNo,short nKStyle);
	short	OddK_UpDown(struct AnalyData *pAnalyData,short nNo);
	short	OddK_Size(struct AnalyData *pAnalyData,short nNo);
	short	OddK_Shadow(struct AnalyData *pAnalyData,short nNo);
	short	isTwinK_Style(struct AnalyData *pAnalyData,short nNo,short nKStyle);

protected:
	BOOL	m_bTQ;
	int		m_nConGap;						//���������ھ����������������
	int		m_nEndGap;						//����������ݾ൱ǰ����������	
	int		m_bSort;						//�Ƿ�������
	int		m_nCon;							//������
	BOOL	m_bHasType[MAX_TYPE];			//��������(�ò����Ƿ����ĳ������)
	int		m_curGPIndex;
	int		m_nCalcPeriod;					//��������
	long	m_lReadNum;                     //���̷������ݸ���
	long	m_lNeedNum;
	long	m_lMyReadNum;                   //���ɷ������ݸ���
	int		m_nDataNum;						//�򵥷������ݸ���

	AnalyData *m_pRecentData;				//�򵥷�������
	LPHISDAT m_pAnalyData;					//��ʷ����
	MemStkInfo *m_pCurGpInfo;				//������Ϣ
	CurrStockData *m_pCurGpData;			//��������
	NTime  m_startTime;
	NTime  m_endTime;
	short  m_curConIndex;
	float  *m_fDataLines;					//��ŷ�������
	long   *m_aTransData;					//�����ʷ���������
	long   *m_transData;					//��ŵ�һ������ʷ���
	long   *m_finalOut;						//�������ۺ���ʷ���
	SingleCon     m_curCalcCon;				//��ǰ��������
	Plans         m_curPlans;				//ѡ�ɲ���
	SignStrategy  m_curSignStra;			//��ǰ������������
   	KStrategy     m_curCalcKs;				//��ǰֹ�����
	TestStrategy  m_curTestStra;			//���Բ���

	vector<int> m_nGpInclude;                            //ɸѡ���Ĺ�Ʊ
	map<float, int, greater_equal<float> > *m_mapSort;    //���б�ѡ�ɵ�ָ��������������ݣ�������
	vector<map<float, int, greater_equal<float> > > m_aMapSort;
	vector<int> m_iSort;
/////////////////////////////////////////////////////////////////////��ʼ��������
	BOOL EqualGreaterDate(NTime &validDate, NTime &endDate);
	BOOL EqualLessDate(NTime &validDate, NTime &endDate);
	void CalcConGap(long nearTime);                                                       
	void PutCurTransDataToCalc(int nCurCon);
	void PutCurTransDataBack(int nCurCon);
	int  CalcSortCon(short i, SingleCon &curCon, long TotalStockNum, short *sortSelStk);   //����ͳ��

	void DelSortCon(int nToDel);                    //����������ɾ�����⴦��
	BOOL InputCurConData(SingleCon curCon);   //���m_fDataLines
	void CalcCurCon(SingleCon curCon);
	void TransWithSign(void);
	void TransWithKS(void);
	
	void RePlaceDate(CString &sList, long nDate);
};

#endif // !defined(AFX_MYSTRATEGY_H__D3FBB36B_1683_4F32_A108_3D297934DA99__INCLUDED_)
