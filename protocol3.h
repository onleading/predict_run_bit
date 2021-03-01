#ifndef	_FOREX_PROTOCOL_DEFINE_
#define	_FOREX_PROTOCOL_DEFINE_

typedef	struct tagCodeMapInfo
{
	BYTE Market;
	char Code[CODE_SIZE];
}CODEMAPINFO, *PCODEMAPINFO;

struct CodeStrCmp
{
	BOOL operator()(const CODEMAPINFO c1, const CODEMAPINFO c2) const
	{
		if(c1.Market!=c2.Market)
			return c1.Market - c2.Market < 0;
		else return lstrcmp(c1.Code, c2.Code) < 0;
	}
};

/////////////////////////////////////////////////////////////////////////////
//ʱ������,�����������ͬ������

#define MAX_TIMESEC			4
#define MAX_DTFNUM			10

typedef	struct	tagTimeInfo	//	ʱ����������
{
	BYTE	Market;						//	�г����
	char	strMask[4];					//	��������
	char	TimeZero;					//	ʱ��
	short	OpenTimer[MAX_TIMESEC];		//	����ʱ��
	short	CloseTimer[MAX_TIMESEC];	//	����ʱ��
	short	InitTimer;					//	��ʼ��ʱ��
	short	EndTimer;					//	����ʱ��
}TIMEINF,*PTIMEINF;

typedef	struct	tagMktInfo	// �г�����
{
	BYTE	BigMarket;
	char	Name[MRK_NAME_SIZE];
	char	Code[MRK_CODE_SIZE];
	BYTE	MktType;
	char	Jc[MRK_JC_SIZE];
	char	unused1[10];
	short	TimeZero;
	BYTE	DocNum;
	short	DelayHQ;
	char	unused2[7];
}MKTINFO,*PMKTINFO;

typedef struct tagDSStkInfo	// �����Ϣ
{
	BYTE	BigMarket;					//  ������
	BYTE	setcode;					//	�����г����
	BYTE	DocNum;						//	С��λ��
	WORD	Type;						//  Ʒ������
	char	Code[CODE_SIZE];			//	����
	char	Name[NAME_SIZE];			//	���ļ��
	char	SpellCode[SPELLCODE_SIZE];	//	Ӣ�ļ��

	float	Unit;						//  ���������ߵ�λ������,�۹�Ϊ1,�۹�ָ��Ϊ���֮һ,AB�ɰ���Ӧ����
	float	AmoUnit;					//  ���������ߵ�λ����,�۹�Ϊ1,�۹�ָ��Ϊ�����֮һ,AB�ɰ���Ӧ����
	BYTE	IsNeedJustCJL;				//  �Ƿ���Ҫ���ڳɽ���
	BYTE	Reserved;					//  ����

	long	nIDIndex;					//	ȫ��������
	short	TimerID;					//	��Ӧʱ��ID
	
	long	QHVol_BaseRate;				//  �ڻ���ÿ�ֶ�Ӧ��С��λ�ı���(ÿ�ֳ���)
	short	QHPrice_MinRate;			//  �ڻ�����С�䶯��λ(��100)
	char	QHPrice_UPDownRate;			//	��չ�������ͣ���Ʊ���,0��ʾ������,5��ʾ5%
	char	Currency;					//	���ҵ�λ(����)
}STKINFO,*PSTKINFO;


/////////////////////////////////////////////////////////////////////////////
const	int	SERVER_INFO			=	9200;	//	ȡ�÷�����Ϣ
const	int	TIME_INFO			=	9203;	//	��Ʒʱ������
const	int	MARKET_INFO			=	9204;	//	�г���������
const	int	CODE_LIST_INFO_5	=	9205;	//  �����Ϣ,ֻ֧����������̵Ŀͻ��˰汾

const	int	NOW_CURR_DATA		=	9210;	//	���µ�����������
const	int	MUL_CURR_DATA		=	9211;	//	���¶����������
const	int	TICK_DATA			=	9212;	//	Tick��������
const	int	MIN_DATA			=	9213;	//	������������
const	int	HQB_DATA			=	9214;	//	������������,����
const	int	HIS_DATA			=	9215;	//	��ʷ��������
const	int	PUSH_HQB_DATA		=	9216;	//	������������,��
const	int	FJB_DATA			=	9217;	//	�ּ۱�����
const	int	SEAT_DATA			=	9218;	//	ȯ�̶���
const	int	JYHQINFO_DATA		=	9221;	//	ĳ�г������������,ÿ�����������200ֻ
const	int	HISTICK_DATA		=	9222;	//	��ʷtick
const	int	HISMIN_DATA			=	9223;	//	��ʷ������
const	int	HISTICK2_DATA		=	9224;	//	��ʷtick2
const	int	ZHSORT_DS_NREQ		=	9225;	//	�ۺ�����
const	int	MUL_CURR_DATA_HQB	=	9226;	//	���¶����������(�����ר��)

const	int MIN2_DATA			=	9227;	//	������������(С�ṹ)
const	int	HISMIN2_DATA		=	9228;	//	��ʷ������(С�ṹ)
const	int HISFX_DATA			=	9229;	//  ��ʷ��������

const	int	DS_LOGIN_REQ		=	9300;	//��¼��Ϣ
const	int	DS_OTHERINFO_REQ	=	9301;	//�ڲ�������Ϣ
const	int	DS_FILELEN_REQ		=	9304;	//�ļ�����
const	int	DS_FILE_REQ			=	9305;	//�����ļ�

const	int	DS_F10CFG_REQ		=	9400;   //����๦��F10��������Ϣ
const	int	DS_F10TXT_REQ		=	9401;	//����๦��F10��������Ϣ

const	int	DS_SEATQUERY_REQ	=	9420;	//���󾭼�ϯλ�Ĳ�ѯ��Ϣ

const	int	DS_ALLHQCODE_REQ	=	9500;	//�������й�Ʊ�ļ�Լ����
const	int	DS_ALLHQINFO_REQ	=	9501;	//�������й�Ʊ�ļ�Լ����
const	int	DS_HOSTMORE_REQ		=	9502;	//������Ϣ 

/////////////////////////////////////////////////////////////////////////////

typedef	struct	tagReqNull
{
	short	ReqID;
}REQNULL,*PREQNULL;

struct ds_login_req
{
	short	req;
	char	qsid[11];		//	���ÿͻ���ʶ
	char	user[50];		//	�û���
	char	pwd[20];		//	����
	char	productfl;		//	�ͻ�������
	float	clientver;		//	�ͻ��˰汾
	char	istdxuser;		//  �ǲ���ͨ���ŵİ汾�û�
	char	unused[7];
	short	magicshort1;	//  ��һ��MagicShort
};

struct ds_login_req_t
{
	short	req;
	char	user[50];		//	�û���
	char	pwd[20];		//	����
	char	productfl;		//	�ͻ�������
	float	clientver;		//	�ͻ��˰汾
	char	istdxuser;		//  �ǲ���ͨ���ŵİ汾�û�
	char	unused[2];
	short	magicshort1;	//  ��һ��MagicShort
};

struct ds_login_ans
{
	char	Sucflag;			//0:ʧ�� 1:�ɹ�
	char	unused1[2];
	char	errmsg[50];			//������Ϣ
	struct bc_date date;		//����������
	struct bc_time time;		//������ʱ��
	char	ds_qsid[21];		//��վ������
	float	hostver;			//��վ�汾
	char	bMustSameQSID;		//�Ƿ��������ͬ��ȯ��ID
	int 	Authority;
	char	unused2[212];
};

struct ds_otherinfo_req
{
	short req;
};

struct ds_otherinfo_ans
{
	long ClientNum;			//��ǰ��������
	long MaxConnectNum;		//��������������
	long HisMaxClientNum;	//��ʷ�ϳ��ֵ����������
	long UsedClientNum;		//����Ч
	
	char starttime[25];		//����ʼʱ�䴮
	char HostVer[30];
	char ProtocolVer;
	long nCanMaxStockNum;	//֧�ֵ����Ʒ����
	long nStockNum;			//��ǰ��Ʒ����

	long  machine_date;		//����������
	long  machine_time;		//������ʱ��
	char  unused1[10];
	long  nSyncDirNum;		//ͬ��Ŀ¼��
	char  bSSDS;			//�Ƿ���ʵʱվ

	char unused2[224];

	char bCompress;			//ѹ����ʽ 0:��ѹ�� 1:ѹ��
	
	short ProcessNum;		//������
	short ThreadNum;		//�߳���
	
	char  bExceedNumCfg;	//�Ƿ񳬹���Ʊ������

	char  nHKType;			//0��֧�ַ��غ�ָ����1������ʱ�ɽ��� 2:����ʵ���ɽ��� HSI
	long  nHKZSVol;		
	char  nIFType;			//0��֧�ַ���IF��3����1������ʱ�ɽ��� 2:����ʵ���ɽ��� IFL3
	long  nIFLVol;		
	char  nCUType;			//0��֧�ַ���Cu��3����1������ʱ�ɽ��� 2:����ʵ���ɽ��� CUL3
	long  nCULVol;		

	char  nDTFConStatus[MAX_DTFNUM];
							//MAX_DTFNUM·DTF������״̬,0��ʾ��֧�ִ�·,1��ʾ�Ͽ�,2��ʾ����
	char  unused3[4];
};

//////////////////////////////////////////////////////////////////////////
typedef	struct	tagMktInfAns	//	�г���������
{
	short	MktNum;
	MKTINFO	MktInf[1];
}MKTINFANS,*PMKTINFANS;

/////////////////////////////////////////////////////////////////////////////
typedef	struct	tagSrvInfoAns	//	��������Ϣ����
{
	char	QSID[11];
	long	IndexVer;	//	���汾��
	long	TimerVer;	//	ʱ�����ð汾��
	long	IndexNum;	//	�����Ŀ
	long	TimerNum;	//	ʱ��������Ŀ
	long	WorkType;	//	���з�ʽ
}SRVINFOANS,*PSRVINFOANS;

/////////////////////////////////////////////////////////////////////////////
typedef	struct	tagStkInfReq	//	�����Ϣ
{
	short	ReqID;	//	�����
	long	Offset;	//	ƫ��λ��
	short	StkNum;	//	��Ŀ
}STKINFREQ,*PSTKINFREQ;

typedef	struct	tagStkInfAns
{
	long		Offset;
	short		StkNum;
	STKINFO	StkInfo[1];
}STKINFANS,*PSTKINFANS;

/////////////////////////////////////////////////////////////////////////////
typedef	struct	tagTimeInfReq	//	ʱ������
{
	short	ReqID;
	long	Offset;
	short	TimeNum;
}TIMEINFREQ,*PTIMEINFREQ;

typedef	struct	tagTimeInfAns
{
	long		Offset;
	short		TimeNum;
	TIMEINF	TimeInf[1];
}TIMEINFANS,*PTIMEINFANS;

//////////////////////////////////////////////////////////////////////////
typedef	struct	tagICurrDataReq	//	������������
{
	short	ReqID;
	BYTE	Market;
	char	Code[CODE_SIZE];
}ICDREQ,*PICDREQ;

typedef	struct	tagICurrDataExAns
{
	char	buf[1];
}ICDEXANS,*PICDEXANS;

/////////////////////////////////////////////////////////////////////////////
typedef	struct	tagMulIdx
{
	BYTE	Market;
	char	Code[CODE_SIZE];
}MULIDX,*PMULIDX;

typedef	struct	tagMulDataReq	//	�����������
{
	short	ReqID;
	WORD	StkNum;
	MULIDX	MulIdx[1];
}MULDATAREQ,*PMULDATAREQ;

typedef	struct	tagSortReq		//	������������
{
	short	ReqID;
	BYTE	Market;
	short	SortID;
	WORD	From;
	WORD	SortNum;
	WORD	SortType;
}SORTREQ,*PSORTREQ;

typedef	struct	tagMulDataExAns
{
	WORD	StkNum;
	char	buf[1];
}MULDATAEXANS,*PMULDATAEXANS;

/////////////////////////////////////////////////////////////////////////////
typedef	struct	tagTickReq		//	�ֱ���Ϣ
{
	short	ReqID;
	BYTE	Market;
	char	Code[CODE_SIZE];
	DWORD	Offset;
	WORD	TickNum;
}TICKREQ,*PTICKREQ;

typedef	struct	tagTickAns
{
	BYTE	Market;
	char	Code[CODE_SIZE];
	DWORD	Offset;
	WORD	TickNum;
	TICKDATA	iTick[1];
}TICKANS,*PTICKANS;

typedef	struct	tagTick2Ans
{
	BYTE	Market;
	char	Code[CODE_SIZE];
	long	nDate;			//����
	float	fClose;			//ǰ��
	long	Volin;			//�����
	DWORD	Offset;
	WORD	TickNum;
	TICKDATA	iTick[1];
}TICK2ANS,*PTICK2ANS;

/////////////////////////////////////////////////////////////////////////////
typedef	struct	tagHisDataReq	//	��ʷ����
{
	short	ReqID;
	BYTE	Market;
	char	Code[CODE_SIZE];
	WORD	Period;
	WORD	MulNum;
	DWORD	Offset;
	WORD	DataNum;
}HISDATAREQ,*PHISDATAREQ;

typedef	struct	tagHisDataAns
{
	BYTE	Market;
	char	Code[CODE_SIZE];
	WORD	Period;
	WORD	MulNum;
	DWORD	Offset;
	WORD	DataNum;
	ANALYDATA	iData[1];
}HISDATAANS,*PHISDATAANS;
//////////////////////////////////////////////////////////////////////////
typedef	struct	tagHisFxDataReq	//��ʷ��������
{
	short	ReqID;
	BYTE	Market;
	char	Code[CODE_SIZE];
	WORD	Period;
	long	startdate;
	long	enddate;
}HISFXDATAREQ,*PHISFXDATAREQ;

typedef	struct	tagHisFxDataAns
{
	BYTE	Market;
	char	Code[CODE_SIZE];
	WORD	Period;
	WORD	DataNum;
	ANALYDATA	iData[1];
}HISFXDATAANS,*PHISFXDATAANS;
/////////////////////////////////////////////////////////////////////////////
typedef	struct	MinuteData_DS	//	��������
{
	short	Minute;			//	��������ķ�����
	float	Now;			//	�ּ�
	float	Average;		//	����,��̬�����(�ڻ�)
	DWORD	NowVol;			//	����,�ֳɽ����(100Ԫ)
	DWORD	VolInStock;		//	�ֲ���(�ڻ�)
}MINDATA_DS,*PMINDATA_DS;

typedef	struct	tagMinDataReq	//	��������
{
	short	ReqID;
	BYTE	Market;
	char	Code[CODE_SIZE];
}MINDATAREQ,*PMINDATAREQ;

typedef	struct	tagMinDataAns
{
	BYTE		Market;
	char		Code[CODE_SIZE];
	WORD		DataNum;
	char		iData[1];
}MINDATAANS,*PMINDATAANS;

typedef	struct	tagHisTickReq		//	�ֱ���Ϣ
{
	short	ReqID;
	long	nDate;				//����
	BYTE	Market;
	char	Code[CODE_SIZE];
	DWORD	Offset;
	WORD	TickNum;
}HISTICKREQ,*PHISTICKREQ;

typedef	struct	tagHisMinDataReq	//	��ʷ��������
{
	short	ReqID;
	long	nDate;				//����
	BYTE	Market;
	char	Code[CODE_SIZE];
}HISMINDATAREQ,*PHISMINDATAREQ;

typedef	struct	tagHisMinDataAns
{
	BYTE		Market;
	char		Code[CODE_SIZE];
	long		nDate;				//����
	float		fClose;				//ǰ��
	WORD		DataNum;
	char		iData[1];
}HISMINDATAANS,*PHISMINDATAANS;

//////////////////////////////////////////////////////////////////////////
typedef struct tagFJBDataReq
{ 
	short	req;
	BYTE	Market;
	char	Code[CODE_SIZE];
}FJBDATAREQ,*PFJBDATAREQ;

typedef	struct tagFJBDataAns	//�ּ۱���Ϣ
{ 
	BYTE	Market;
	char	Code[CODE_SIZE];
	short num;
	PriceVolTable idx[1];
}FJBDATAANS,*PFJBDATAANS;

//////////////////////////////////////////////////////////////////////////
//��������
struct Seat_Req
{
	short	ReqID;
	BYTE	BSFlag;	//0:����� 1:������
	BYTE	WantNum;
	BYTE	Market;
	char	Code[CODE_SIZE];
};

struct	Seat_Content
{
	char SeatNo[7];
	char SeatStr[31];
};

struct Seat_Ans
{
	BYTE			Market;
	char			Code[CODE_SIZE];
	BYTE			BSFlag;
	BYTE			NumB;
	BYTE			NumS;
	Seat_Content	List[1];
};

struct ds_filelen_req
{  
	short req;
	char  fname[40];
};

struct ds_filelen_ans
{
   DWORD filelen;
   char  usemdstr;
   char	 mdstr[33];
};

struct ds_file_req
{  
	short req;
	DWORD flag;				//��ʶ��������
	DWORD pos;
	char  fname[40];
};

struct ds_wz_ans
{ 
	DWORD flag;				//��ʶ��������
	short num;
	char  buf[1];
};

struct ds_f10cfg_req
{
	short req;
	short setcode;
	char code[SH_CODE_LEN];
	short whichjbm;
	short reserved;
};

struct ds_f10cfg_title
{
	char sTitle[64];
	char sFilePath[80];
	long nOffset;
	long nLength;
};

struct ds_f10cfg_ans
{
  short titlenum;
  struct ds_f10cfg_title list[1];
};

struct ds_f10txt_req
{
	short req;
	short setcode;
	char code[SH_CODE_LEN];
	short which;
	char sFilePath[80];
	long nOffset;
	long nLength;
	short whichjbm;
	short reserved;
};

struct ds_f10txt_ans
{
	short setcode;
	char code[SH_CODE_LEN];
	short which;
	short num;
	char  buf[1];
};

struct ds_hostmore_req
{
	short req;
	char  verflag;	
};

struct ds_hostmore_ans
{
	struct bc_date date;
};
//////////////////////////////////////////////////////////////////////////
//����ϯλ��ѯ
typedef	struct _SEATQUERYINFO 
{
	BYTE	Market;
	char	Code[CODE_SIZE];
	short	nSeatItem;								//ϯλ��
	short	nSeatType;								//ϯλ��λ
	float	fPrice;									//ϯλ�۸�
}SEATQUERYINFO;

typedef	struct _GETSEATQUERY_REQ
{
	short		iRetCode;							//������Ϣ
	int			nReqNum;							//���ض��ϯλ�Ų�ѯ
	short		nSeatItems[1];
}GETSEATQUERY_REQ;

typedef	struct _GETSEATQUERY_ANS 
{
	int				nInfoNum;						//���ظ���,��ʱ��1��
	SEATQUERYINFO	cSeatItem[1];
}GETSEATQUERY_ANS;

/////////////////////////////////////////////////////////////////////////////

//ֱ�ӷ���
#define DS_HQALIVE_ASSISID			40
//ǰһ������Ӧ���ŷ���(100-255)
#define DS_LOGIN_ASSISID			101
#define DS_SERVER_INFO_ASSISID		102
#define DS_CODE_LIST_INFO_ASSISID	103
#define DS_MARKET_TIMER_ASSIDID		104
#define	DS_MKT_INFO_ASSIDID			105
#define DS_REQFILELEN_ASSISID		108
#define	DS_REQFILE_ASSISID			109

#endif
/////////////////////////////////////////////////////////////////////////////
