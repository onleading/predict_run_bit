#ifndef LEVIN_H_
#define LEVIN_H_

#define LEVINGN_BUYSELL			1	//�µ�
#define LEVINGN_CANCEL			2	//����

#define LEVINGN_WTLIST			3	//��ί���б�
#define LEVINGN_CJLIST			4	//��ɽ��б�
#define LEVINGN_GPLIST			5	//�����е�¼�ʻ��Ĺ�Ʊ
#define LEVINGN_ZHINFO			6	//���ѵ�¼���ʻ�

struct LevinGN_BuySell
{
	char	YybID[6];
	char	Zjzh[31];

	short	Setcode;	//�г����� 0:���� 1:�Ϻ� ������δ��
	char	Code[9];	//֤ȯ����

	short	nBSFlag;	//0:��	 1:��
	short	nKPFlag;	//0:���� 1:ƽ��
	short	nWTFS;		//�м۷�ʽ,������г���һ��
	
	float	fWtPrice;	//
	long	lVol;		//��Сί�е�λ
};

struct LevinGN_Cancel
{
	char	YybID[6];
	char	Zjzh[31];

	char	Wtbh[31];
};

struct LevinGN_BuySellCancelANS
{
	char	bSuccess;		//ί���Ƿ�ɹ� 1��ʾ�ɹ�
	char	Errmsg[100];	//���successΪ0,errmsgΪ������Ϣ

	char	YybID[6];
	char	Zjzh[31];
	char	Wtbh[20];
};

typedef struct LevinGN_ListREQ
{
	short	Setcode;		//���setcodeΪ-1,CodeΪ��,�������е�ί�л�ɽ���ѯ
	char	Code[9];
	char	bTodayReq;		//�Ƿ��ǵ��ղ�ѯ
	long	lStartDate;
	long	lEndDate;
}LEVINGN_LISTREQ;

typedef struct LevinGN_WTCJListANS
{
	char	YybID[6];
	char	Zjzh[31];
	char	Gddm[31];
	char	Wtbh[31];

	short	Setcode;
	char	Code[9];
	
	short	nBSFlag;	//0:��	 1:��
	short	nKPFlag;	//0:���� 1:ƽ��
	short	nWTFS;		//�м۷�ʽ,������г���һ��
	long	lWtDate;	//ί������,���ǵ��ղ�ѯ,Ϊ0

	long	lSeconds;		//�������������
	float	fWtPrice;		//ί�м�
	float	fCjPrice;		//�ɽ���
	long	lCjVol;			//�ɽ����� ����ǳ�,��Ϊ��ֵ
	long	lWtVol;			//ί������ ����ǳ�,��Ϊ��ֵ
}LEVINGN_WTCJLISTANS;

typedef struct LevinGN_GPZhInfoREQ
{
	short unused;
}LEVINGN_GPZHINFOREQ;

typedef struct LevinGN_GPListANS
{
	char	YybID[6];
	char	Zjzh[31];
	char	Gddm[31];
	short	Setcode;
	char	Code[9];
	float	fCbj;		//�ɱ���
	long	lTotalVol;	//��Ʊ���
	long	lCanUseVol;	//���ù�Ʊ	
}LEVINGN_GPLISTANS;

typedef struct LevinGN_ZhInfoANS
{
	char	YybID[6];
	char	Zjzh[31];
	char	ZhType;		//0:��Ʊ,1:�ڻ�

	double	dCanUseRmb;	//���������
	double	dCanUseMy;	//������Ԫ
	double	dCanUseGy;	//���ø�Ԫ
}LEVINGN_ZHINFOANS;

struct Extern_LevinCfg
{
	BOOL	bCanDbClickPrice;	//�Ƿ�˫����ʽ
	BOOL	bTradeAllWtl;		//�Ƿ�ɨ��
	BOOL	bGenSell;			//�򵥳ɽ���������ʾ�û���������
	float   fGenSellRate;		//�����۸�Ϊ�ּ۵Ķ���()%
	BOOL    bGenBuy;			//�����ɽ���������ʾ�û�������
	float   fGenBuyRate;		//�򵥼۸�Ϊ�ּ۵Ķ���()%
	char	bStop;				//����ֹ��ֹӮ��,
	float	fStopLossRate;		//ֹ�����
	float	fStopEarnRate;		//ֹӮ����
	BOOL	bNoShowWhenCJOver;	//�ɽ���ί��ͼ�겻����ʾ
}EXTERN_LEVINCFG; 

typedef struct MAIDAN						//Ԥ��
{
	char	zqdm[7];						//��Ʊ����
	char	bsflag;							//������־
	char	gddm[20];						//�ɶ�����
	short	wttype;							//ί������
	long	wtsl;							//ί������  
	int     bjfs;							//���۷�ʽ
	float	wtjg;							//ί�м۸�
	long	wtrq;							//������
	int		jydw;							//0--��  1--��  2--��
	char	buf[6];							//��Ϊ8��������
}MaiDan;

enum
{
	LF_NORMAIL,			//��������
	LF_SENDYMD,			//����Ԥ��
	LF_NEWORDER,		//�����µ�
	LF_MMGSORDER,		//�������浥
	LF_STOPLOSSORDER,	//ֹ��
	LF_STOPEARNORDER	//ֹӯ��
};

enum
{
	FEEDBACK_NEWWT,
	FEEDBACK_NEWCJ,
	FEEDBACK_NEWCANCEL,
	FEEDBACK_YMDCHANGE,
	FEEDBACK_ZJZHCHANGE,
	FEEDBACK_CFGCHANGE
};

enum
{
	TOKEN_BUY,
	TOKEN_SELL,
	TOKEN_BUYOK,
	TOKEN_SELLOK,
	TOKEN_YMDBUY,
	TOKEN_YMDSELL,
	TOKEN_BUYPARTOK,
	TOKEN_SELLPARTOK,
	TOKEN_YMDCFG,
	TOKEN_ONOFF
};	

enum
{
	WTSTATUS_NULL,		//�Ƿ���
	WTSTATUS_NOCJ,		//û�ɽ�
	WTSTATUS_PARTCJ,	//���ֳɽ�
	WTSTATUS_ALLCJ		//ȫ���ɽ�
};

#define MAX_WTCJLIST_NUM	3000	//
#define MAX_GPLIST_NUM		500		//
#define MAX_YMD_NUM			200		//

extern  struct Extern_LevinCfg	g_ExternLevinCfg;
extern  CImageList	g_LevinImage;
extern BOOL g_bHasAdvJy;
extern BOOL g_bShowToken;

extern BOOL IsSameWTBH(struct LevinGN_WTCJListANS *pList1,struct LevinGN_WTCJListANS *pList2); //�Ƿ�����ͬ��ί�б��
extern BOOL IsSameZjZh(struct LevinGN_ZhInfoANS *pList1,struct LevinGN_GPListANS *pList2); //�Ƿ�����ͬ���ʽ��ʻ�
extern BOOL GetWTList(struct MemStkInfo *pStkInfo,int & nWTNum,struct LevinGN_WTCJListANS *pWTList);
extern BOOL GetGPList(int & nGPNum,struct LevinGN_GPListANS *pGPList);
extern BOOL GetYmdList(struct MemStkInfo *pStkInfo,int & nYmdNum,struct MAIDAN *pYmdList);
extern void SendLevinCancel(CWnd *pView,struct LevinGN_WTCJListANS *pOne);


extern BOOL GetAllCJList(int & nCJNum,struct LevinGN_WTCJListANS *pCJList);
extern void DetectLevinCJ(CWnd *pMainWnd);

extern void ExitLevinCJ();

#endif
