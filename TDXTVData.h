#ifndef __TDX__TOPVIEW__DATA__H__
#define __TDX__TOPVIEW__DATA__H__

#pragma pack(push,1)
#pragma warning(disable:4200)

#define COMMON_MARKETCODE_MAXLEN		11
typedef char SCommonMarketCode[COMMON_MARKETCODE_MAXLEN];

#define TDP_DAY			(BYTE(1))
#define TDP_WEEK		(BYTE(2))
#define TDP_MONTH		(BYTE(3))

//#define TDT_SEAT		(WORD(0000))	//ϯλ��Ϣ
#define TDT_0301		(WORD(301))		//����/������
#define TDT_1013		(WORD(1013))	//����/�ֹ�/�����˻�
#define TDT_1001		(WORD(1001))	//����/�ֹ�/����
#define TDT_0401		(WORD(401))		//����/����/Ӫҵ������
#define TDT_0801		(WORD(801))		//Ӫҵ��/����/�г�
#define TDT_0101		(WORD(101))		//����/�ɽ�
#define TDT_0501		(WORD(501))		//����/����/�����ʻ�
#define TDT_0502		(WORD(502))		//�г�/����/�����ʻ�

#define TDT_SHHQ000001_0401	(WORD(9999))		//��ָ֤��/����/Ӫҵ������
#define TDT_ZXW_EX		(WORD(9900))	// _TopView_ZXW

struct STDCommonHead
{
	SCommonMarketCode	marketCode;
	DWORD	dwDate;
	char	data[0];
};

struct STDHoriHead
{
	SCommonMarketCode	marketCode;
	char	data[0];
};

struct STDVertHead
{
	DWORD	dwDate;	
	char	data[0];
};

// �ϲ��ı���������Ϣ
struct SUnionDateItem
{
	DWORD			dwDate;			// ����
	DWORD			dwType;			// ����    TDP_DAY, TDP_WEEK, TDP_MONTH
};

//1.	_TopView_BS_XXX���������̽���ͳ����Ϣ03_01
//����������--�ա��ܡ��¡���Ӧ03_01
struct STD0301BaseItem
{
	float	fTotAmt;			//ͳ�������ڵ��ܳɽ����
	float	fBuyAmt;			//ͳ�������ڵ����̳ɽ����	ʵΪ����
	float	fSellAmt;			//ͳ�������ڵ����̳ɽ����  ʵΪ����
};
struct STD0301Item
{
	SCommonMarketCode	marketCode;
	DWORD	dwDate;
	STD0301BaseItem	base;
};
//2.	_TopView_C_XXX  A�ɹɶ������ʻ��ֹ�ͳ��10_13
//����������--�ա��ܡ��¡���Ӧ10_13
//�ʻ���4��--A��B��D��F�����庬��ÿһ���ʻ��ĺ������ʳ¿��࣬��Ҳ�������
struct STD1013BaseItem
{
	float	fMktVol;			//��ͨA�ɹɱ�������ͨ��
	int		nTotAcctCount;		//��ͨA���ʻ����������ɶ�����
	float	fAHold;				//A���ʻ��ֹ�����
	int		nAAcctCount;		//A���ʻ���
	float	fBHold;				//B���ʻ��ֹ�����
	int		nBAcctCount;		//B���ʻ���
	float	fDHold;				//D���ʻ��ֹ�����
	int		nDAcctCount;		//D���ʻ���
	float	fFHold;				//F���ʻ��ֹ�����
	int		nFAcctCount;		//F���ʻ���
};
struct STD1013Item
{
	SCommonMarketCode	marketCode;
	DWORD	dwDate;
	STD1013BaseItem	base;
};
//3.	_TopView_I_XXX  A�ɹɶ��ֹ�����ṹͳ��10_01
//����������--�ա��ܡ��¡���Ӧ10_01
struct STD1001BaseItem 
{
	float	fMktVol;			//��ͨA�ɹɱ�������ͨ��
	int		nTotAcctCount;		//��ͨA���ʻ����������ɶ�����
	int		nK1AcctCount;		//�ֹ���<1k�ɵ��ʻ���
	float	fK1Hold;			//�ֹ���<1k�ɵ��ʻ��ĳֹ�����
	int		nK10AcctCount;		//�ֹ�����[1k��10k]�ɵ��ʻ���
	float	fK10Hold;			//�ֹ�����[1k��10k]�ɵĳֹ�����
	int		nK50AcctCount;		//�ֹ�����[10k��50k]�ɵ��ʻ���
	float	fK50Hold;			//�ֹ�����[10k��50k]�ɵĳֹ�����
	int		nK100AcctCount;		//�ֹ�����[50k��100k]�ɵ��ʻ���
	float	fK100Hold;			//�ֹ�����[50k��100k]�ɵĳֹ�����
	int		nK500AcctCount;		//�ֹ�����[100k��500k]�ɵ��ʻ���
	float	fK500Hold;			//�ֹ�����[100k��500k]�ɵĳֹ�����
	int		nM1AcctCount;		//�ֹ�����[500k��1M]�ɵ��ʻ���
	float	fM1Hold;			//�ֹ�����[500k��1M]�ɵĳֹ�����
	int		nM5AcctCount;		//�ֹ�����[1M��5M]�ɵ��ʻ���
	float	fM5Hold;			//�ֹ�����[1M��5M]�ɵĳֹ�����
	int		nM10AcctCount; 		//�ֹ�����[5M��10M]�ɵ��ʻ���
	float	fM10Hold;			//�ֹ�����[5M��10M]�ɵĳֹ�����
	int		nGt10MAcctCount; 	//�ֹ�����[10M����]�ɵ��ʻ���
	float	fGt10MHold;			//�ֹ�����[10M����]�ɵĳֹ�����
};
struct STD1001Item 
{
	SCommonMarketCode	marketCode;
	DWORD	dwDate;
	STD1001BaseItem base;
};
//4.	_TopView_S_XXX����������Top10Ӫҵ��ͳ��04_01
//����������--�ա��ܡ��¡���Ӧ04_01
struct STD0401BaseItem
{
	BYTE	rankType;		//��������-1:�����2:������3:�������4:���������
#define TOPVIEW_SEAT_RANKTYPE_Buy			1	// ����
#define TOPVIEW_SEAT_RANKTYPE_Sell			2	// ����
#define TOPVIEW_SEAT_RANKTYPE_NetBuy		3	// ������
#define TOPVIEW_SEAT_RANKTYPE_NetSell		4	// ������
	BYTE	Rank;			//���
	DWORD	dwSeatId;		//Ӫҵ��ID
	float	fTotAmt;		//��Ʊ�ܳɽ���
	float	fBuyAmt;		//Ӫҵ��������
	float	fSellAmt;		//Ӫҵ���������
};
struct STD0401Item
{
	SCommonMarketCode	marketCode;
	DWORD	dwDate;
	STD0401BaseItem	base;
};

//ת��֮�󣬴�С����60��
struct STD0401BaseBSItemEx
{
	DWORD	dwSeatId:24;		//Ӫҵ��ID
	float	fBuyAmt;		//Ӫҵ��������
	float	fSellAmt;		//Ӫҵ���������
};
struct STD0401BaseItemEx
{
	float	fTotAmt;		//��Ʊ�ܳɽ���
	STD0401BaseBSItemEx		bs[40];	//�ֱ���10�����룬10��������10�������룬10��������
};
struct STD0401ItemEx
{
	SCommonMarketCode	marketCode;
	DWORD	dwDate;
	STD0401BaseItemEx	base;
};

//5.	_TopView_S_Seat Ӫҵ�����Ʊ�
//Ӫҵ�����Ʊ�û��ͳ���������ԣ�_TopView_S_XXX��_TopView_Seat_Trade_XXX���õ���SeatId��Ӧ�ù��������
struct STDSeatItem
{
	DWORD	dwSeatId;			//Ӫҵ��ID��ָ�����ڲ�����
	char	szSeatCode[21];		//Ӫҵ����ţ���֤����Ϣ��˾����
	char	szSeatName[101];	//Ӫҵ������
};


struct STDSeatExItem
{
	DWORD	dwSeatId;			//Ӫҵ��ID��ָ�����ڲ�����
	char	szSeatCode[21];		//Ӫҵ����ţ���֤����Ϣ��˾����
	char	szSeatName[101];	//Ӫҵ������
	char	szMemberCode[21];	//��ԱCode
};

//6.	_TopView_Seat_Trade_XXX Ӫҵ������ͳ��08_01
//����������--�ա��ܡ��¡���Ӧ08_01

struct STD0801BaseItem
{ 	
	float	fABuy;			//Ӫҵ��A��������
	float	fASell;			//Ӫҵ��A���������
	float	fBBuy;			//Ӫҵ��B��������
	float	fBSell;			//Ӫҵ��B���������
	float	fFondBuy;		//Ӫҵ�������������
	float	fFondSell;		//Ӫҵ�������������
	float	fTbondBuy;		//Ӫҵ����ծ������
	float	fTbondSell;		//Ӫҵ����ծ�������
	float	fCbondBuy;		//Ӫҵ����ҵծ������
	float	fCbondSell;		//Ӫҵ����ҵ�������
	float	fCvtBuy;		//Ӫҵ����תծ������
	float	fCvtSell;		//Ӫҵ����תծ�������
	float	fBondBuy;		//Ӫҵ��ծȯ������
	float	fBondSell;		//Ӫҵ��ծȯ�������
	float	fRepurBuy;		//Ӫҵ���ع�������
	float	fRepurSell;		//Ӫҵ���ع��������
};

struct STD0801Item
{ 	
	SCommonMarketCode	szSeatId;//marketCode;
						//Ϊ�������ݽṹһ���ԣ�������ĳɴ��� dwSeatId=atol(szSeatId),
						//szSeatId = Format("%d",dwSeatId);
//	DWORD	dwSeatId;			//Ӫҵ��ID
 	DWORD	dwDate;
	STD0801BaseItem	base;
};

//7.	_TopView_Stk_Trade_XXX ���ɳɽ�ͳ��01_01
//����������--�ա��ܡ��¡���Ӧ01_01
struct STD0101BaseItem
{ 
	int		nTradeCount;	//��Ʊͳ�������ڵ��ܳɽ�����
	float	fTq;			//A����ͨ��
	float	fVol;			//A�ɳɽ���
	float	fAmt;			//A�ɳɽ���
};
struct STD0101Item
{ 
	SCommonMarketCode	marketCode;
	DWORD	dwDate;
	STD0101BaseItem	base;
};
//10.	_TopView_Stk_C_Trade_XXX ���ɷ����ʻ�����ͳ��
//����������--�ա��ܡ��¡���Ӧ05_01
//�ʻ���4��--A��B��D��F�����庬��ÿһ���ʻ��ĺ������ʳ¿��ࡣ
struct STD0501BaseItem
{
	float	fVol;			//ͳ���ڼ��ܳɽ���
	float	fAmt;			//ͳ���ڼ��ܳɽ���
	float	fABuyVol;		//A���ʻ���������
	float	fABuyAmt;		//A���ʻ���������
	float	fASellVol;		//A���ʻ���������
	float	fASellAmt;		//A���ʻ����������
	float	fBBuyVol;		//B���ʻ���������
	float	fBBuyAmt;		//B���ʻ���������
	float	fBSellVol;		//B���ʻ���������
	float	fBSellAmt;		//B���ʻ����������
	float	fDBuyVol;		//D���ʻ���������
	float	fDBuyAmt;		//D���ʻ���������
	float	fDSellVol;		//D���ʻ���������
	float	fDSellAmt;		//D���ʻ����������
	float	fFBuyVol;		//F���ʻ���������
	float	fFBuyAmt;		//F���ʻ���������
	float	fFSellVol;		//F���ʻ���������
	float	fFSellAmt;		//F���ʻ����������
};
struct STD0501Item
{
	SCommonMarketCode	marketCode;
	DWORD	dwDate;
	STD0501BaseItem	base;
};

//11.	_TopView_IDX_C_Trade_XXX �����г������ʻ�����ͳ��
//����������--�ա��ܡ��¡���Ӧ05_02
//�ʻ���4��--A��B��D��F�����庬��ÿһ���ʻ��ĺ������ʳ¿��ࡣ
struct  STD0502BaseItem
{
	float	fATotVol;		//A���ʻ������г��ܳɽ���
	float	fATotAmt;		//A���ʻ������г��ܳɽ���
	float	fABuyVol;		//A���ʻ������г���������
	float	fABuyAmt;		//A���ʻ������г���������
	float	fASellVol;		//A���ʻ������г���������
	float	fASellAmt;		//A���ʻ������г����������
	float	fBTotVol;		//B���ʻ������г��ܳɽ���
	float	fBTotAmt;		//B���ʻ������г��ܳɽ���
	float	fBBuyVol;		//B���ʻ������г���������
	float	fBBuyAmt;		//B���ʻ������г���������
	float	fBSellVol;		//B���ʻ������г���������
	float	fBSellAmt;		//B���ʻ������г����������
	float	fDTotVol;		//D���ʻ������г��ܳɽ���
	float	fDTotAmt;		//D���ʻ������г��ܳɽ���
	float	fDBuyVol;		//D���ʻ������г���������
	float	fDBuyAmt;		//D���ʻ������г���������
	float	fDSellVol;		//D���ʻ������г���������
	float	fDSellAmt;		//D���ʻ������г����������
	float	fFTotVol;		//F���ʻ������г��ܳɽ���
	float	fFTotAmt;		//F���ʻ������г��ܳɽ���
	float	fFBuyVol;		//F���ʻ������г���������
	float	fFBuyAmt;		//F���ʻ������г���������
	float	fFSellVol;		//F���ʻ������г���������
	float	fFSellAmt;		//F���ʻ������г����������
};
struct  STD0502Item
{
	SCommonMarketCode	marketCode;
	DWORD	dwDate;
	STD0502BaseItem	base;
};

struct STDRightItem
{
	DWORD dwTypeId;			//��չ��Ϣ���� �� DB_TextInfo.dbo._InfoServer_ExpandInfo ���ж���
	DWORD dwClientType;	    //�ͻ�������
	DWORD dwProductId;      //��ƷID
	DWORD dwDelayDay;		//���ڸߵ�����,�����ӳ�ʱ��
	DWORD dwDataType;		//�ߵ���������
};


struct STDZXWExBaseItem
{
	float		fZXWBuy;			// ��ϯλ������
	float		fZXWSell;			// ��ϯλ�������
	float		fFundBuy;			// ���������
	float		fFundSell;			// ����������
	float		fCompanyBuy;		// Ӫҵ�������
	float		fCompanySell;		// Ӫҵ��������
	float		fOthersBuy;			// �ʹ������
	float		fOthersSell;		// �ʹ�������
	float		fDaPanJC;			// ���������ʽ����
};
struct STDZXWExItem
{
	SCommonMarketCode	marketCode;
	DWORD				dwDate;
	STDZXWExBaseItem	base;
};

#pragma warning(default:4200)	//#pragma warning(disable:4200)
#pragma pack(pop)

interface	IGeneric
{
	virtual	void*	CALLBACK	QueryInterface(DWORD dwID)=0;
};

interface IZxwDataManager : public IGeneric
{
	// ���з������ݾ�Ϊ��������
	// ��Ϊ���к��������̰߳�ȫ
	// ���ܣ�ȡ��ָ�����͡�ָ�����ڡ�ָ�����ڵ��������ݣ���ֻ��Ʊͬһ������ݣ�
	// ���룺dwDataType����������    dwCycleType����������   dwDate��������Ϣ  
	// �����void*�������б���ת������Ӧ���ͣ�������ʾ�����º�����ͬ��
	// STDCommonHead
	// STD0301Item
	// STD1013Item
	// STD1001Item
	// STD0401ItemEx
	// STD0801Item
	// STD0101Item
	// STD0501Item
	// STD0502Item
	// pdwCount����ΪNULL����NULLʱ��Ϊ�����б������ݸ���
	// pdwReportDate�����Դ�NULL����NULLʱ������ʵ�ʷ��ص��������ڣ�������dwDate����
	// IsGetLast ��ʾ��ǰ����û������ʱ,�Ƿ��ȡ��һ������
	virtual void* GetData(DWORD dwDataType, DWORD dwCycleType, DWORD dwDate, DWORD *pdwCount, BOOL IsGetLast = FALSE) = 0;

	// ���ܣ�ȡ��ָ�����͡�ָ�����ڡ�ָ����Ʊ���������ݣ�һֻ��Ʊ�Ķ������ݣ�
	// ���룺dwDataType����������    dwCycleType����������   pszCode����Ʊ��MarketCode  
	// �����void*�������б���ת������Ӧ����    pdwCount����ΪNULL����NULLʱ��Ϊ�����б������ݸ���
	// ע��  ���dwDataTypeΪDATA_SEAT_TRADE����pszCode��Ҫת��ΪϯλID����dwSeatID = atol(pszCode)
	virtual void* GetData(DWORD dwDataType, DWORD dwCycleType, const char *pszMarketCode, DWORD *pdwCount) = 0;

	// ���ܣ�ȡ��ָ�����͡�ָ�����ڡ�ָ����Ʊ��ָ�����ڵ����ݣ�һֻ��Ʊ��ĳ�����ݣ�
	// ���룺dwDataType����������    dwCycleType����������   pszCode����Ʊ��MarketCode   dwDate��������Ϣ
	// �����void*�������б���ת������Ӧ����    pdwCount����ΪNULL����NULLʱ��Ϊ�����б������ݸ���
	//       pdwReportDate�����Դ�NULL����NULLʱ������ʵ�ʷ��ص��������ڣ�������dwDate����
	// ע��  ���dwDataTypeΪDATA_SEAT_TRADE����pszCode��Ҫת��ΪϯλID����dwSeatID = atol(pszCode)
	// IsGetLast ��ʾ��ǰ����û������ʱ,�Ƿ��ȡ��һ������
	virtual void* GetData(DWORD dwDataType, DWORD dwCycleType, const char *pszMarketCode, DWORD dwDate, BOOL IsIsGetLast = FALSE) = 0;

	// ���ܣ�ȡ��ָ�����͡�ָ�����ڵ����ݱ���֮�����б�
	// ���룺dwDataType����������    dwCycleType����������   pdwCount����ΪNULL����NULLʱ��Ϊ�����б������ݸ���
	// ����������б�
	virtual DWORD* GetReportList(DWORD dwDataType, DWORD dwCycleType, DWORD *pdwCount) = 0;

	// ���ܣ�����ָ��ϯλID��ȡ��ϯλ����
	// ���룺pszSeatId��ϯλID
	// �����ϯλ����
	//		 FALSE��ʾϯλId����
	virtual BOOL GetSeatName(const char *pszSeatId,char* pszSeatName,int nMaxSeatNameLen) = 0;

#define SEAT_GENERAL				0		// ��ͨϯλ
#define SEAT_FUND					1		// ����ϯλ
#define SEAT_REST					2		// ����ϯλ
	// ���ܣ��ж�ָ��ID��ϯλ��ϯλ����
	// ���룺pszSeatId��ϯλID
	// �����ϯλ����
	virtual BYTE GetSeatType(const char *pszSeatId) = 0;

	// ���ܣ�ȡ��ָ�����͵��������ݱ���֮���ڣ��п��������£��п��������£��п��������£�
	// ���룺dwDataType����������
	// �����������������
	//		���صĽ������ת����SUnionDateItem�ṹ
	virtual void* GetUnionReportList(DWORD dwDataType, DWORD *pdwCount) = 0;
};

typedef BOOL (CALLBACK *DllInit)(char* szInstallPath,DWORD dwPersonId,char* szPassword,char* szServerIp,DWORD dwServerPort,DWORD dwProvinceId);
typedef IZxwDataManager* (CALLBACK *DllGetZXWDataInterface)();
typedef void (CALLBACK *DllReceiveData)();
typedef void (CALLBACK	*DllRelease)();

//1��zxwdata.dll��tdxtvdata.dll������һ��Ŀ¼�¡�
//2��LoadLibrary(tdxtvdata.dll)
//3������DllInit��ʼ����
//	szInstallPath��Ҫ��\\����β�������ݱ���Ŀ¼,
//	dwPersonId ָ������Ŀ���,
//	szPassword�����룬
//	szServerIp,dwServerPort��ָ�����������IP���˿�,
//	dwProvinceId��ʡ����Ϣ���������ݵ�ʱ���ο���������գ���ʡ��Id����
// 		pcUndefine		=0,		//����
// 		pcBeiJing		=1,		//����
// 		pcShangHai		=2,		//�Ϻ�
// 		pcTianJin		=3,		//���
// 		pcChongQing		=4,		//����
// 		pcAnHui			=5,		//����
// 		pcFuJian		=6,		//����
// 		pcGanSu			=7,		//����
// 		pcGuangDong		=8,		//�㶫
// 		pcGuangXi		=9,		//����
// 		pcGuiZhou		=10,	//����
// 		pcHaiNan		=11,	//����
// 		pcHeBei			=12,	//�ӱ�
// 		pcHeNan			=13,	//����
// 		pcHeiLongJiang	=14,	//������
// 		pcHuBei			=15,	//����
// 		pcHuNan			=16,	//����
// 		pcJiLin			=17,	//����
// 		pcJiangSu		=18,	//����
// 		pcJiangXi		=19,	//����
// 		pcLiaoNing		=20,	//����
// 		pcNeiMeng		=21,	//����
// 		pcNingXia		=22,	//����
// 		pcQingHai		=23,	//�ຣ
// 		pcShanDong		=24,	//ɽ��
// 		pcShan1Xi		=25,	//ɽ��
// 		pcShan3Xi		=26,	//����
// 		pcSiChuan		=27,	//�Ĵ�
// 		pcXinJiang		=28,	//�½�
// 		pcXiZang		=29,	//����
// 		pcYunNan		=30,	//����
// 		pcZheJiang		=31		//�㽭
//4������DllReceiveData��������
//5��DllGetZXWDataInterfaceȡ�����ӿ�
//6���˳�ʱ��Ҫ����DllRelease

#endif
