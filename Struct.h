#ifndef   STRUC_H
#define   STRUC_H

#include "Interface.h"
#include "info.h"

typedef struct
{
	float Price;
	long  Vol;
}PRICE_VOLSTRUCT; 

struct tag_FindResult
{
	char which;
	char path[256];
};

struct tag_FindResult2
{
	char whichtype;		//����
	char title[256];	//������ļ���
	char whichindex;	//�ڼ���
};

struct GpDomain_Struct
{
	short flag;		//flag,0:����� 1:����� 2:��ҵ�� 3.������ 4.��ϰ�� 5:�Զ�����
	short which;	
	char  Name[10];
};

struct ZHB_BlockInfo_Struct
{
	char	zhmc[11];	//�������
	short	nStockNum;
	short   nContent[MAXGPNUM];	//���USER_SIZEֻ��Ʊ
};

struct BlockInfo_Struct
{
	int tag;
	char Name[100];
	char KeyGuyCode[50];
};

typedef struct BlockColor
{
	char		BlockName[100];	//�����
	COLORREF	clrBlock;		//�����ɫ
	DWORD		unnsed;			//����
}BLOCKCOLOR,*LPBLOCKCOLOR;

struct IndexSortInfo_Struct	//֧�����������
{
	int Index;
	int Cycle;
	int Param[MAX_PARAMNUM];
};

struct CONDITIONINFO
{
	char acCode[ZBCODE_LEN];//ָ��Ӣ������
	int  ParamNum;
	int  ParamValue[MAX_PARAMNUM];
	int  CalcCycle;				//�������� 0:5������ 1:15���� ...
};

struct FILEHEAD
{
	int		ConditionNum;
	BOOL	m_IsAndRadio;
	long	nDate;
	short	nRefreshTime;
	DWORD	Flag[3];
};

typedef struct
{
	int		nTjIndexNo;
	BOOL	bUsed;			//�Ƿ����Ԥ��
	char	acCode[50]; 	//ָ��Ӣ������
	float	aTjParam[6];	//��������
	int		nPeriod;		//��������
}TJ_WARN_OLD;

typedef struct
{
	int		nTjIndexNo;
	BOOL	bUsed;			//�Ƿ����Ԥ��
	char	acCode[50]; 	//ָ��Ӣ������
	float	aTjParam[MAX_PARAMNUM];	//��������
	int		nPeriod;		//��������
}TJ_WARN;

//����Ԥ��ϵͳ�Ľṹ
typedef struct
{	int			nLB;             	//����ͻ�Ƶ�
	BOOL		bLB;				//�Ƿ�����Ԥ��
	float		fUpRatio;			//�Ƿ���ͻ��
	BOOL		bUpRatio;			//�Ƿ��Ƿ���ͻ��
	float		fDownRatio;			//������ͻ��
	BOOL		bDownRatio;			//�Ƿ������ͻ��
	float		fTickAmount;		//���쳣(�ʽ��)
	BOOL		bTickAmount;		//�Ƿ���쳣
	float		fTickDiff;			//�����쳣(������)
	BOOL		bTickDiff;			//�Ƿ񵥱��쳣
	float		fTickVol;			//�����쳣(�ʳɽ���)
	BOOL		bTickVol;			//�Ƿ�����쳣
	BOOL		bLowHigh;			//�Ϳ�����
	BOOL		bHighLow;			//�߿�����
	BOOL		bStopClose;			//�Ƿ����ͣ��Ԥ��
	BOOL		bFzZF;				//�Ƿ���з�������
	BOOL        bNewMaxWarn;		//���¸߱�������
	BOOL        bNewMinWarn;		//���µͱ�������
	BOOL        bHasBigBuy;			//�д���
	BOOL        bHasBigSell;		//�д�����
	BOOL		bPopup;				//�Ƿ񵯳�Ԥ���Ի���;
	BOOL		bTickHugeAmo;		//�Ƿ��������Ԥ��
	int			nTickAmoMinute;		//�������Ԥ��ʱ���
	int			nTickAmoTime;		//�������Ƶ��
	int			nBeepFlag;			//0:������ 1:ϵͳ���� 2:�Զ�������
	char		WavFile[80];		//Wav�ļ�·��
}WARNGLOBALPARAM;

typedef struct 
{
	BOOL		bTJPopup;			//�Ƿ񵯳�Ԥ���Ի���;
	int			nTJBeepFlag;		//0:������ 1:ϵͳ���� 2:�Զ�������
	char		TJWavFile[80];		//Wav�ļ�·��
	char		nWarnFlag;			//0:ȱʡȫ�г���1:��ѡ��
	char		ununsed[99];		//����
}WARNGLOBALPARAM2;

typedef struct
{
	SYSTEMTIME   time;
	short   nStockNo;
	float   fPrice;
	long	nVol;
	short	InOutFlag;	
	short   nReason;
	short	nStockType;	//nStockType = 0:ϵͳԤ����Ʊ,1:ָ��Ԥ����Ʊ
} WARN_STRU;

typedef struct						//��λͻ�Ƶ�������
{
	char	code[7];               //��Ʊ���
	char	setcode;
	float   fHighPrice;             //ͻ�Ƽ�
	float   fLowPrice;			 	//���Ƽ�
	char	bDoLevin1;				//�����µ�1 0:�޹��� 1:���� 2:����
	char	bDoLevin2;				//�����µ�2 0:�޹��� 1:���� 2:����
	char	bEverDo1;				//ÿ����ʾ1
	char	bEverDo2;				//ÿ����ʾ2
	unsigned short	ETFAPRate;		//ETF��/�����
	unsigned short	TurnOverRate;	//������
} PWARNINGCND;

struct FirstLevel
{
	char Text[30];
	int  SonNum;
};

struct sort_struc
{
	float fValue;
    short nIndex;
	short tag;
};

struct sort_struc2
{
	char StrValue[21];
	short nIndex;
	short tag;
};

struct sort_struc3
{
	float fValue;
	short nItemNum;
    short nIndex;
	short tag;
};

//Grid��ɫ�ṹ
struct GRIDCOLOR
{
	COLORREF GridBackColor;
	COLORREF GridLineColor;
	COLORREF GridHighLineColor;
	COLORREF GridTitleTextColor;
	COLORREF GridTitleArrowColor;
	COLORREF Grid2GridColor;

	COLORREF Grid_Up_Color;
	COLORREF Grid_Down_Color;
	COLORREF Grid_Level_Color;
	COLORREF Grid_CodeName_Color;
	COLORREF Grid_Volume_Color;
	COLORREF Grid_Other_Color;
};

struct VIPCOLOR
{
	COLORREF			BACKCOLOR;	//����
	COLORREF			AXISCOLOR;  //�������
	COLORREF			TIMECOLOR;	//��������(����ʮ���ߡ��α��)
	COLORREF			KPCOLOR;	//�����߼�������
	COLORREF			KNCOLOR;    //������
	COLORREF			LEVELCOLOR; //ƽ��
	COLORREF			DOWNCOLOR;  //�µ���
	COLORREF			BIGTITLECOLOR;//�����
	COLORREF			USCOLOR;//������
	COLORREF			TXTCOLOR;//ͼ������
	COLORREF			FLOATCURSORCOLOR;//������ɫ
	COLORREF			DRAWLINETOOLCOLOR; //����TOOL��ɫ//���߹��ߵ���ɫ
	COLORREF			ZBCOLOR[6];		//һ�㲻�����5��ָ��
	COLORREF			VOLCOLOR;		//�ɽ���
	COLORREF			COMVIEWINFOBC;	//����ͼ����ͼ�Ϸ���Ϣ��
	COLORREF			UNDEFINE1;
	COLORREF			UNDEFINE2;
};

struct TXTCOLOR
{
	COLORREF TxtForeColor;
	COLORREF TxtBackColor;
};

struct TREECOLOR
{
	COLORREF TreeForeColor;
	COLORREF TreeBackColor;
};

struct MENUCOLOR //����Ĳ˵���ɫ�����ǵ������ԣ���������
{
	COLORREF MenuBackColor;
	COLORREF MenuTitleColor;
	COLORREF MenuColor1;
	COLORREF MenuColor2;
	COLORREF MenuColor3;
	COLORREF MenuColor4;
};

//ϵͳ��ɫ�ṹ��,�����ĸ��ӽṹ
struct SYSTEMCOLOR
{
	struct VIPCOLOR		VipColor;
	struct GRIDCOLOR	GridColor;
	struct TXTCOLOR		TxtColor;
	struct TREECOLOR	TreeColor;
};

//KeyGuy�еĽṹ
struct KeyGuyContent
{
	int Type;	//0:��Ʊ 1:ָ�� 2:����
	char Content[19];
};

//ϵͳ���õ���ɫItem�ṹ
struct SchemeItemContent
{
	char str[30];
	int  id;
};

#define   SH_CODE_LEN      6
#define   NAME_LEN         8

struct CmdTable
{
    char    Name[40];
    char    Code[8];
    WPARAM  wParam;
    LPARAM  lParam;	   
};

struct DynaCmdTable
{
	char EnName[16];
	char ChName[40];
	char Author[30];		//�����
	char Descript[200];		//����
	char HYString[HY_STRING_LEN];	//��ҵ��
	BOOL ShowShortCut;		//�Ƿ���ʾ���
	DWORD DefPadSet;
	DWORD DefPadCmdID;
	CmdTable CmdInfo;
	int nCmdNum;			//��������
	int	nNextDefNum;		//��ȥ���ÿ������һ��ȱʡ���ÿ����
};

//
typedef struct TickStat					//	122B	��TMN��
{
	WORD		Minute;					//	2B		ͳ��ʱ��
	float		fAmount[3][5];			//	60B		��/��/��:����/����/����/����/δ֪����
	float		fVol[3][5];				//	60B		��/��/��:����/����/����/����/δ֪����
	DWORD		VolNum[3][5];			//	60B		��/��/��:����/����/����/����/δ֪����
}TICKSTAT,*LPTICKSTAT;

typedef struct PzCalcRes 
{
	time_t		CalcTmie;					//����ʱ��
	long		nDataNum;					//���ݸ���
	int			nCalcOKNum[MAX_LINEOUT];	//���������ĸ���
	float		fOut[MAX_LINEOUT];			//������
}PZCALCRES, *LPPZCALCRES;

struct DiskStkInfo         // ������.INF�ļ��ж�λ����ʱ���ݽṹ
{
  char           Code[SH_CODE_LEN+1];   // ֤ȯ����
  char           tp_flag;               // ͣ�Ʊ�־
  short          Index;                 // ��.IHQ�е����
  short          Unit;                  // ���׵�λ
  float          ActiveCapital;         // ��ͨ�ɱ�
  char           Name[NAME_LEN+1];      // ֤ȯ����
  char           fl_flag;               // �����־
  long	    	 VolBase;               // ���ȵĻ���(ǰ5�������)
  short          IndexOfHQ;            // ��Z_HQ�е����
  short          IndexOfMMP;           // ��Z_MMP�е����
};

struct MemStkInfo           // ��.INF�ļ��ж�������ڴ��е����ݽṹ
{
	char        Code[CODE_SIZE];   // ֤ȯ����
	char        tp_flag;               // ͣ�Ʊ�־
	short       Index;                 // ��.IHQ�е����
	float       Unit;                  // ���׵�λ
	float		AmoUnit;
	float       ActiveCapital;         // ��ͨ�ɱ�
	char        Name[NAME_SIZE];       // ֤ȯ����
	char        fl_flag;               // �����־,�������ڱ��mark
	long		VolBase;               // ���ȵĻ���
	float       cdp;                   // �����м�
	float       nh;                    // ����
	float       nl;                    // ���
	float       ah;                    // �߼�ͻ�Ƶ�
	float       al;                    // �ͼ�ͻ�Ƶ�
 	short       dkflag;                // �ʺ����������
	float       tbp;                   // ���ƽ���
	float       stoplost;              // ֹ���
	float       leave;                 // �˽��

	short       J_addr;         //����ʡ��
	short       J_hy;           //������ҵ
    long        J_gxrq;         //��������
	long		J_start;		//��������
    float       J_zgb;          //�ܹɱ�
    float       J_zjhhy;        //֤�����ҵ
    float       J_oldjly;       //������ھ�����
    float       J_oldzysy;      //���������Ӫ����
    float       J_bg;           //B��
    float       J_hg;           //H��
    float       J_mgsy2;        //����ÿ������ (�Ʊ����ṩ��ÿ������,������Ĳ���)
    float       J_zzc;          //���ʲ�(��Ԫ)
    float       J_ldzc;         //�����ʲ�
    float       J_gdzc;         //�̶��ʲ�
    float       J_wxzc;         //�����ʲ�
    float       J_cqtz;         //����Ͷ��
    float       J_ldfz;         //������ծ
    float       J_cqfz;         //���ڸ�ծ
    float       J_zbgjj;        //�ʱ�������
    float       J_jzc;          //�ɶ�Ȩ��(���Ǿ��ʲ�)
    float       J_zysy;         //��Ӫ����
    float       J_zyly;         //��Ӫ����
    float       J_yszk;         //Ӧ���ʿ�
    float       J_yyly;         //Ӫҵ����
    float       J_tzsy;         //Ͷ������
    float       J_jyxjl;        //��Ӫ�ֽ�����
    float       J_zxjl;			//���ֽ�����
    float       J_ch;			//���
    float       J_lyze;         //�����ܶ�
    float       J_shly;         //˰������
    float       J_jly;          //������
    float       J_wfply;        //δ��������
    float       J_tzmgjz;       //����ÿ�ɾ��ʲ� ��������:  ���ʲ�/��������ܹɱ�
	float		J_HalfYearFlag;		//ȫ������Ϊ����Ϊ��λ

	//��������ͳ�����ݵĸ���
	WORD		nImgStatLastSeconds;	//�˹��ϴ�����ͳ�����ݵĸ���ʱ��
	WORD		nTckStatLastSeconds;	//�˹��ϴ�����ͳ�����ݵĸ���ʱ��
	//����Ʊ��Ԥ��ϵͳ�Ļ�׼����
	short		BuySellFlag;			//�������̵ı�־ 1�� -1��
	short		CloseStopFlag;			//ͣ���־ 0,1,2
	long		nWarnCount;				//��������
	float		zangsu;					//�ڻ��������в�������
	//������Ϣ,����Զ��
	short		BaseFreshCount;			//�������ϵĸ��´���
	short		GbbqFreshCount;			//�ɱ���Ǩ�ĸ��´���
	float		Close;
	//����setcode,��ʶ���г�
	char		setcode;				//ע����setcode,��Ҫ����Զ�̷���
	//����bigmarket,��ʶ��Ʒ����
	char		bigmarket;
	BYTE		DocNum;					//������������,������
	short		TimerID;
	char		SpellCode[SPELLCODE_SIZE];

	TICKSTAT	its;
	PzCalcRes	CalcRes1;
	PzCalcRes	CalcRes2;
	PzCalcRes	CalcRes3;
	time_t		NearHqTime;
};

typedef	struct CurrStockData     // ��.IHQ�ļ��ж�������ݽṹ Modified 2003.9.29
{
  DWORD          ItemNum;               // ��������
  float          Close;                 // ǰ���̼�,ָ��
  float          Open;                  // ���̼�,ָ��
  float          Max;                   // ��߼�,ָ��
  float          Min;                   // ��ͼ�,ָ��
  float          Now;                   // �ּ�,���ָ��
  union
  {
	  DWORD          RefreshNum;        // ˢ����,������ͨ������Ч
	  DWORD          OpenVolume;        // ������
  };
  float			 Lead;					// ����ָ��
  DWORD          Volume;                // ����
  DWORD          NowVol;                // ����(���ֲ�)
  float          Amount;                // �ܳɽ����
  DWORD          Inside;                // ����
  DWORD          Outside;               // ����
  union
  {
		float	TickDiff;				// ������(��λ��)
		long	VolInStockDiff;			// �ֲֲ�,�����ڻ�����
  };
  union
  {      
	  float Yield;           // ��ӯ��1,������Ȩ��ָ��
	  DWORD VolInStock;      // �ֲ���
  }Yield_VolInStock;
  float          Buyp[5];               // ��������
					// ����ɽ����:Buyp[0]
					// ծȯ�ɽ����:Buyp[1]
					// ��ծ�ɽ����:Buyp[2]
  DWORD          Buyv[5];               // ��Ӧ�������۵��������
					// ���Ǽ���:Buyv[0]
  float          Sellp[5];              // ���������
					// Ȩ֤�ɽ����:Sellp[0]
					// �ڻ���ֲ���:Sellp[1]
					// ָ�������ɽ����:Sellp[1]
					// �ڻ�������:Sellp[2]
					// ָ���򲻺���Ȩָ��:Sellp[2]
  DWORD          Sellv[5]; // ��Ӧ��������۵��������
					// �µ�����:Sellv[0]
					//	�ڻ������������־:Sellv[1]
					//  �ڻ������п���Sellv[2] 
					//	�ڻ������п���Sellv[3]
  union
  {
      struct
      {
	    WORD     InOutFlag:2;           // �����̱�־
	    short    sdunit:4;              // ��������λ(���ڼ�������ָ��)
	    WORD     RestVol:7;             // ���
	    WORD     Unused:3;              // δ�ñ�־
      }          Flag;                  // ��־λ�ṹ
      struct
      {
        WORD     InOutFlag:2;           // �����̱�־
        WORD     YQSY:14;               // ��ծԤ������
      }          GZFlag;
  };
  union
  {
	  DWORD         CJBS;				// ��λ���ӵ��ֶΣ��ɽ�����
	  float			ClearPrice;			// �ڻ������
	  char			HKTTFlag;			//	�۹ɷֱ�������
  };
  union
  {
  	float        Jjjz;
	DWORD		 Reserved;
  };
}CURRDATA,*PCURRDATA;

typedef	struct ExCurrData
{
	float	AverPrice;						//	ƽ����
	union
	{
			float	PE;						//	��ӯ��
			float	PreClear;				//  ǰ����
			float	HK_Dividend;			//	�۹�ÿ����Ϣ
			float	GdsyNowSyl;				//	�̶�����ĵ�������������
	};
	union
	{
			float	YieldVal;				//	������
			long	lSanRatio;				//  �ڻ��ֱʶ࿪����
			float	HK_Profit;				//	�۹�ÿ��ӯ��
	};
	union
	{
			float	MrkCap;					//	��ֵ
			DWORD	PreVolIn;				//	ǰ�ֲ�
			float	HK_StockSum;			//	�۹ɹɱ�
			DWORD	GdsyYjlx;				//	�̶�����Ӧ����Ϣ
	};
	float	HisHigh;						//	��ʷ��߼�
	float	HisLow;							//	��ʷ��ͼ�
	union
	{
			float	PreDelta;				//	����ʵ��
			float	IEP;					//	���̲ο�����
	};
	union
	{
			float	NowDelta;				//	����ʵ��
			float	IEV;					//	���̲ο��ɽ���
	};
	union
	{
		float	PreMinum;					//	��ۣ�
		DWORD	OutOpen;					//  ���̿�	
	};
	union
	{
		float	Gearing;					//	�ܸ˱���
		DWORD	OutClose;					//	����ƽ
	};
	union
	{
		float	ExecPrice;					//	��ʹ��
		DWORD	InOpen;						//	���̿�
	};
	union
	{
		float	ConvRatio;					//	�Ϲ�����
		DWORD	InClose;					//	����ƽ
	};
	char	ExpireDate[16];					//	��Ȩ������
	WORD	BuySeatNum[5];					//  �����
	WORD	SellSeatNum[5];					//  ������			
	BYTE	HasLink;						//	��û����ع�Ʊ
	float	HSpread;						//	���������
	float	LSpread;						//	���������
	DWORD	PreVol;							//  ÿ�ֹ���
	long    HqDate;							//  ������������
	long	HqTime;							//	��������ʱ��
	float	fZhangsu;						//	����
	long	VolBase;						//  ���ȵĻ���
	char	Info1[13];
	char	Info2[13];
	BYTE	Reserved[1];					//	��ʾ����Դ
}EXCURRDATA, *PEXCURRDATA;

struct CurrStockDataEx
{
	char		SetCode;
	char        Code[SH_CODE_LEN+1];
	struct CurrStockData hq;		   // ԭʼ����
	char        dkflag;				   // 
	float       tbp;                   // ���ƽ���
	float       stoplost;              // ֹ���
	float       leave;                 // �˽��
	float		zangsu;			   // ����
	unsigned short	nWarnCount;			   // (��Ծ��)��������
};

typedef struct TickData      // ��.TIC�ļ��ж�������ݽṹ
{ 
	short          Minute;              // ��������ķ�����
	long           Now;                 // �ּ�*1000
	long		   NowVol;              // ����
	long           VolInStock_dif;      // �ֲ�������
	union
	{
		short      InOutFlag;             // �����ڻ�,��ֵ���ֱ�ʾ���в���ռNowVol��?/%%-1 +10000*InOutFlag
		struct
		{
					char HKTTFlag;
					char HKInOutFlag;
		} HKFlag;
	};
} TICKDATA,*PTICKDATA;

typedef struct VirtualData					//	16B		���Ͼ���
{
	short		Minute;						//	2B		ʱ��
	float		fPrice;						//	4B		����ƥ��۸�
	DWORD		dwVolume;					//	4B		����ƥ��ɽ�
	long		dwLeaveQty;					//	4B		����δƥ����(Ϊ��ʱ,��ʾ��һ��,Ϊ����ʾ����һ��)
	char		reserved;					//	1B
	char		Second;						//	1B		����
}VTLDATA,*LPVTLDATA;

typedef struct TickDataOfMtc      // ��.TIC�ļ��ж�������ݽṹ
{ 
	short       Minute;             // ��������ķ�����
	long		Now;                // �ּ�*1000
	long		NowVol;             // ����
	long        VolInStock_dif;     // �ֲ�������
	short       InOutFlag;          // ������־
	long        Lead;               // ָ������ָ��
	DWORD		Buyv;				// �����������Ǽ�����������������
	DWORD		Sellv;				// ���������µ���������������������
	float       NowAmount;
} TICKDATAOFMTC;

typedef struct  MinuteData                      // ��.IFZ�ļ��ж�������ݽṹ1
{
	short Minute;                         // ��������ķ�����
	float Now;           // �ּ�   ��ָ��          �ּ�        White Line
	float Average;       // ���� ����Ȩָ��        ��̬�����  Yellow Line 
	DWORD NowVol;        // ���� �ֳɽ����(100Ԫ) ����
	DWORD Buyv;          //�����������Ǽ�����������������
	DWORD Sellv;         //���������µ���������������������
	union
	{
	  DWORD VolInStock;   //       �ֲ���(�ڻ�)
	  float Lead;         //       ����ָ��(ָ��)
	}VolInStock_Lead;
}MINDATA,*PMINDATA;

union long_short
{         
	long    Date;               // ����YYYYMMDD(����)
    struct
    {
		WORD Day:11;			//����
		WORD Year:5;			//��, ��2004������
		WORD Minute:16;         // ��������ķ�����
    }Daye;
};

typedef struct AnalyData	// ��.IFZ�ļ��ж������ʷ���ݼ�¼
{
  union long_short Time;				 // ʱ��,��ʷԭ��, �浽���ϵ����ݶ��� *100 �Ժ�� DWORD ������
  union { DWORD l_Open;  float Open;  }; // ��λ���̼�
  union { DWORD l_High;  float High;  }; // ��λ��߼�
  union { DWORD l_Low;   float Low;   }; // ��λ��ͼ�
  union { DWORD l_Close; float Close; }; // ��λ���̼�
  union {float Amount;  DWORD QHVolInStock;}; 
										 // ��λ�ɽ����(����/ָ��)
  DWORD Volume;                          // ��λ�ɽ���,��λ�ɽ����(ָ��,100Ԫ)
  union 
  { 
	  float YClose;
	  struct
	  {   WORD up;                  // ���Ǽ���
	      WORD down;                // �µ�����
	  }zd;							// ��λǰ���̼�(��Ȩ��)���ڻ��ĳֲ���
  };								// ָ��,�ǵ�����
}ANALYDATA,*LPANALYDATA;

/////////////////////////////////////////////////////////////////////////////////
//��չ�����̺�
#define EXDAYBLOCK_LEN				3072

typedef struct ExdayHead					//	3072B	�ļ�ͷ:���Ϊ������,������Ϊ������
{
	long lStockNum;							//	4B		��Ʊ����
	long lIndexSpc;							//	4B		��Ʊ�������ռ�(ĿǰΪ6000)
	long lBlockNum;							//	4B		��ǰ�ܵ�Block����
	char Reserved[3060];					//	3060B	�����������С
}EXDAYHEAD,*LPEXDAYHEAD;

typedef struct ExDayIndex					//	3072B	�̺���չ���ݹ�Ʊ����
{
	char	Code[16];						//	16B		��Ʊ����
	long	lDataNum;						//	4B		���ݸ���
	long	lDataBlockNum;					//	4B		���ݿ�ʹ�ø���
	long	lDataBlockNo[762];				//	3048B	���ݿ���
}EXDAYINDEX,*LPEXDAYINDEX;
//
typedef struct ExDayData					//	96B
{
	long			Time;					//	4B		ʱ��
	//
	unsigned long	Cjbs;					//	4B	L2	�ɽ�����
	unsigned short	BuyCjbsRatio;			//	2B	L2	����ɽ�����/�ɽ�����*50000,ȡ��
	unsigned short	SellCjbsRatio;			//	2B	L2	�����ɽ�����/�ɽ�����*50000,ȡ��
	unsigned short	BigBuyCjbsRatio;		//	2B	L2	����󵥳ɽ�����/����ɽ�����*50000,ȡ��
	unsigned short	BigSellCjbsRatio;		//	2B	L2	�����󵥳ɽ�����/�����ɽ�����*50000,ȡ��
	//
	unsigned long	ItemNum;				//	4B		��������
	unsigned short	BuyItemRatio;			//	2B		���������������*50000,ȡ��
	unsigned short	BigItemRatio;			//	2B		�󵥲�����������*50000,ȡ��
	unsigned short	BigBuyItemRatio;		//	2B		����󵥲�������ռ�󵥱���*50000,ȡ��
	//
	unsigned long	Volume;					//	4B		�ɽ���
	unsigned short	OutSideRatio;			//	2B		����ɽ���/�ɽ���*50000,ȡ��(L1:����;L2:���)
	unsigned short	BigVolRatio;			//	2B		�󵥳ɽ���/�ɽ���*50000,ȡ��(����)
	unsigned short	BigBuyVolRatio;			//	2B		����󵥳ɽ���/�󵥳ɽ���*50000,ȡ��(����)
	unsigned short	BigBuyTckRatio;			//	2B		����󵥳ɽ���/�ɽ���*50000,ȡ��(���)
	unsigned short	BigSellTckRatio;		//	2B		�����󵥳ɽ���/�ɽ���*50000,ȡ��(���)
	//
	float			fBOrder;				//	4B	L2	Stock:�򾻹ҵ�(����:Index:��ί��)
	float			fBCancel;				//	4B	L2	Stock:�򾻳���
	float			fSOrder;				//	4B	L2	Stock:�����ҵ�(����:Index:��ί��)
	float			fSCancel;				//	4B	L2	Stock:��������
	float			fBuyAvp;				//	4B	L2	Stock:ί�����
	float			fSellAvp;				//	4B	L2	Stock:ί������
	//
	char			Code[13];				//	13B		��Ʊ����
	char			bTransed;				//	1B		�Ƿ��Ѿ�ת��
	char			Reserved[18];			//	18B		����
}EXDAYDATA,*LPEXDAYDATA;
//Level2�������ݽṹ
typedef struct CurrStockData_L2				//	96B		��.IH2�ļ��ж�������ݽṹ
{
	float		Buyp[5];					//	20B		�����
	DWORD		Buyv[5];					//	20B		��Ӧ����۵�����	  
	float		Sellp[5];					//	20B		������
	DWORD		Sellv[5];					//	20B		��Ӧ�����۵�����
	float		BuyAvp;						//	4B		ί�����
	float		SellAvp;					//	4B		ί������
	DWORD		TotalBuyv;					//	4B		��ί����
	DWORD		TotalSellv;					//	4B		��ί����
}CURRSTOCKDATA_L2;

typedef struct CurrQueue					//  216B
{
	DWORD			RefreshNum;				//	4B		ˢ����
	float			Buyp;					//	4B		ί���
	float			Sellp;					//	4B		ί����
	unsigned short	buy1num;				//	2B		��һ����ϸ�ҵ�����
	unsigned short	sell1num;				//	2B		��һ����ϸ�ҵ�����
	unsigned short	buy1vol[50];			//	100B	��һ����
	unsigned short	sell1vol[50];			//	100B	��һ����
}CURRQUEUE;

typedef struct MarkInfo						//	4B		��.MRK�ж�������ݽṹ
{
	DWORD		L2TicNum;					//	4B		L2��ʳɽ���
}MARKINFO,*LPMARKINFO;

typedef struct TickData_L2					//	12B		��.TI2�ļ��ж�������ݽṹ
{
	WORD		nSecond;					//	2B		����6������������
	DWORD		Now;						//	4B		�ּ�*10000
	DWORD		NowVol;						//	4B		����
	union
	{
		WORD Rest;
		struct
		{
			char	RestVol;				//	1B		���
			char	InOutFlag;				//	1B		�����̱�־
		};
	};
}TICKDATA_L2;

typedef struct NormalStat					//	46B		����ͳ����Ϣ
{
	WORD		Minute;							//	2B		ͳ��ʱ��
	float		Now;							//	4B		���ռ۸�
	float		BuyAvp;							//	4B		���
	DWORD		TotalBuyv;						//	4B		����
	float		SellAvp;						//	4B		����
	DWORD		TotalSellv;						//	4B		����
	union {DWORD NetBOrder;	float fBOrder;};	//	4B		�򾻹ҵ�.����:DWORD;�ۼ�:float
	union {DWORD NetBCancel;float fBCancel;};	//	4B		�򾻳���.����:DWORD;�ۼ�:float
	union {DWORD NetSOrder;	float fSOrder;};	//	4B		�����ҵ�.����:DWORD;�ۼ�:float
	union {DWORD NetSCancel;float fSCancel;};	//	4B		��������.����:DWORD;�ۼ�:float
}NORMALSTAT,*LPNORMALSTAT;

//
typedef struct SumTckStat				//	126B	��TST��
{
	WORD		Minute;					//	2B		ͳ��ʱ��
	float		fVol[3][5];				//	60B		��/��/��:����/����/����/����/δ֪����
	DWORD		VolNum[3][5];			//	60B		��/��/��:����/����/����/����/δ֪����
	DWORD		CJBS;					//	4B
}SUMTCKSTAT,*LPSUMTCKSTAT;

typedef struct SumImgStat				//	52B		��IST��
{
	long			RefreshNum;
	float			LargeVol;			//	4B		������(Դ�ڿ���,����tick)
	float			LargeInVol;			//	4B		���������(Դ�ڿ���,����tick)
	float			LargeOutVol;		//	4B		����������(Դ�ڿ���,����tick)
	unsigned long	InItemNum;			//	4B		����Tick����(Դ�ڿ���,����tick)
	unsigned long	OutItemNum;			//	4B		����Tick����(Դ�ڿ���,����tick)
	unsigned long	LargeItemNum;		//	4B		��Tick����(Դ�ڿ���,����tick)
	unsigned long	LargeInItemNum;		//	4B		������Tick����(Դ�ڿ���,����tick)
	unsigned long	LargeOutItemNum;	//	4B		������Tick����(Դ�ڿ���,����tick)
	float			fBOrder;			//	4B	L2	�򾻹ҵ�
	float			fBCancel;			//	4B	L2	�򾻳���
	float			fSOrder;			//	4B	L2	�����ҵ�
	float			fSCancel;			//	4B	L2	��������
}SUMIMGSTAT,*LPSUMIMGSTAT;
//
typedef struct DDEReport
{
	float		fDDX;					//		����DDX
	float		fDDY;					//		����DDY
	float		fDDZ;					//		����DDZ
	float		fDDX59;					//		ǰ59��DDX
	float		fDDX60;					//		60��DDX
	float		fDDY59;					//		ǰ59��DDY
	float		fDDY60;					//		60��DDY
	short		DDXRed59;				//		60����,֮ǰ���������DDX����
	short		DDXRed9;				//		֮ǰ9����DDX�������
	short		DDXRed;					//		DDX��������
	short		DDXRed10;				//		DDX���10��������
	float		fBigBuyRate;
	float		fBigSellRate;
	float		fBOrderRate;
	float		fAveTradeVol;
	float		fBigIOAmount;
	float		fBigIOWeight;
}DDEREPORT,*LPDDEREPORT;

//������
typedef struct TractorObj										//	14B
{
	BYTE	WarnFlag;											//	1B		��������
	float	Price;												//	4B		�������ۼ�
	BYTE	OrderNum;											//	1B		������������
	long	FirstQty;											//	4B		��һ�ڴ�С
	long	DiffQty;											//	4B		ǰ����,��һ�ʼ�ȥǰһ��
}TRACTOROBJ,*LPTRACTOROBJ;
//ί���춯
typedef struct OrderShockObj									//	14B
{
	BYTE	WarnFlag;											//	1B		����:1:����;2:����;3:������;4:������
	float	Amount;												//	4B		����ί�н��
	float	Volume;												//	4B		����ί������
	char	Reserved[5];										//	5B
}ORDERSHOCKOBJ,*LPORDERSHOCKOBJ;
//������
typedef struct LargeVolObj
{
	BYTE	WarnFlag;											//	1B		����:0:������;1:������
	float	Price;												//	4B		
	float	Volume;												//	4B		
	char	Reserved[5];										//	5B		
}LARGEVOLOBJ,*LPLARGEVOLOBJ;
//�ǵ�����
typedef	struct QuickChgObj
{
	BYTE	WarnFlag;											//	1B		����:0:�۸�����;1:�۸��µ�
	float	QuickChgRate;										//	4B		�����ǵ�����
	char	Reserved2[9];										//	9B
}QUICKCHGOBJ,*LPQUICKCHGOBJ;
//�۸�ת
typedef	struct ReversalObj
{
	BYTE	WarnFlag;											//	1B		����:0:�۸񷴵�;1:�۸���ˮ
	float	ReversalRate;										//	4B		�۸�ת����
	char	Reserved2[9];										//	9B
}REVERSALOBJ,*LPREVERSALOBJ;
//���Ӽ۸��춯
typedef struct PxShockObj
{
	BYTE	WarnFlag;											//	1B		����:0:�۸�����;1:�۸�����
	float	PxShockRate;										//	4B		�۸�5����ͻ�Ʒ���
	char	Reserved2[9];										//	9B
}PXSHOCKOBJ,*LPPXSHOCKOBJ;
//�����춯
typedef struct TickShockObj
{
	BYTE	Reserved1;											//	1B
	float	TickPxRate;											//	4B		���ʼ۸�ƫ�����
	char	Reserved2[9];										//	9B
}TICKSHOCKOBJ,*LPTICKSHOCKOBJ;
//���з���
typedef struct VolShockObj
{
	BYTE	Reserved1;											//	1B
	float	VolumeRate;											//	4B		��������ӵķ������
	char	Reserved2[9];										//	9B
}VOLSHOCKOBJ,*LPVOLSHOCKOBJ;

typedef struct WarnAtom											//	32B
{
	short		SetCode;										//	2B
	char		Code[SH_CODE_LEN+1];							//	7B
	short		WarnType;										//	2B		Ԥ������
	DWORD		WarnID;											//	4B		Ԥ��ID
	union Detail												//	14B
	{
		TRACTOROBJ		uTractor;
		ORDERSHOCKOBJ	uOrderShock;
		LARGEVOLOBJ		uLargeVol;
		QUICKCHGOBJ		uQuickChg;
		REVERSALOBJ		uReversal;
		PXSHOCKOBJ		uPxShock;
		TICKSHOCKOBJ	uTickShock;
		VOLSHOCKOBJ		uVolShock;
	}uDetail;
	char		Hour;											//	1B		hh
	short		MinSec;											//	2B		mmdd
}WARNATOM,*LPWARNATOM;

#define DWARN_SPACE		1000

typedef struct DWarnInf
{
	long		nWarnNum;
	long		nSZIDPos;
	long		nSHIDPos;
	LPWARNATOM	pWarnAtom;
	long		nWarnSpace;
}DWARNINF,*LPDWARNINF;

//���½ṹΪ.mtl�ļ��Ĺ���
struct WTRankings							//
{
	char	Code[SH_CODE_LEN+1];			//			֤ȯ����
	double	TotalOrderQty;					//			ί����
	double	TotalVolumeTraded;				//			�ɽ���
	double	TotalValueTraded;				//			�ɽ���
};

struct HYRankings							//
{
	char	IndustryCode[8];				//			��ҵ����
	double	IndustryTotalValueTraded;		//			��ҵ�ܳɽ���
};

struct HYGPRankings							//
{
	char	Code[SH_CODE_LEN+1];			//			֤ȯ����
	double	TotalValueTraded;				//			�ɽ���
};

struct SingleCancelRankings					//
{
	char	Code[SH_CODE_LEN+1];			//			֤ȯ����
	double	OrderEntryTime;					//			ί��ʱ��(DateTime)
	double	Quantity;						//			ί��ʱ��
	float	Price;							//			�۸�
};

struct TotalCancelRankings					//
{
	char	Code[SH_CODE_LEN+1];			//			֤ȯ����
	double	TotalWithdrawnQty;				//			�ܳ�����
};

struct MarketTotalInfo											//
{
	struct WTRankings			WTBuyRank[12][10];				//			��ί������
	struct WTRankings			WTSellRank[12][10];				//			��ί������
	double						MarketTotalValueTraded;			//			�г��ܳɽ���
	struct HYRankings			HYRank[10];						//			��ҵ����
	struct HYGPRankings			HYGPRank[10*5];					//			��ҵ�ڹ�Ʊ����
	struct SingleCancelRankings	SingleCancelBuyRank[12][10];	//			�����򳷵�����
	struct SingleCancelRankings	SingleCancelSellRank[12][10];	//			�����򳷵�����
	struct TotalCancelRankings	TotalCanceBuyRank[12][10];		//			��������������
	struct TotalCancelRankings	TotalCanceSellRank[12][10];		//			��������������
};

struct CurrStockData_Code
{
	char   SetCode;
	char   Code[SH_CODE_LEN+1];
	struct CurrStockData	hq;		// ԭʼ����
	struct CurrStockData_L2 hql2;	// Level2����
};

struct CurrQueue_Code
{
	char   SetCode;
	char   Code[SH_CODE_LEN+1];
	struct CurrQueue	queue;		// ��������
};

/////////////////////////////////////////////////////////////////////////////////

struct  PriceVolTable
{ 
	DWORD Price;
	DWORD Volume;
	DWORD Kcl;
	DWORD Pcl;
};

struct fltype               // ��Ʊ�����������λ�ṹ
{  
	WORD     astk:1;    //A��
	WORD     bstk:1;    //B��
	WORD     jj:1;      //����
	WORD     gz:1;      //��ծ
	WORD     qiz:1;     //��ҵծȯ
	WORD     qz:1;      //��,���Ȩ֤
	WORD     qtstk:1;   //����
	WORD	 zxbstk:1;	//��С��ҵ
	WORD	 sb:1;		//����
	WORD	 addcode:1;	//���۱���003
	WORD     cybstk:1;  //��ҵ��
	WORD     unused:5;  //
};

union   flunion 
{
	struct	fltype ftype;
	WORD	fshort;
};

struct bc_date
{
    unsigned short da_year; // Year - 1980
    char  da_day;  // Day of the month
    char  da_mon;  // Month (1 = Jan)
};

struct bc_time
{
    char  ti_min;  // Minutes
    char  ti_hour; // Hours
    char  ti_hund; // Hundredths of seconds
    char  ti_sec;  // Seconds
};

struct date 
{
  int da_year;     /* current year */
  char da_day;     /* day of the month */
  char da_mon;     /* month (1 = Jan) */
};

struct time 
{
  unsigned char  ti_min;   /* minutes */
  unsigned char  ti_hour;  /* hours */
  unsigned char  ti_hund;  /* hundredths of seconds */
  unsigned char  ti_sec;   /* seconds */
};

typedef struct
{
	char   DocPath[256];	//��ʷ����·��
	short  GPFZNUM;			//��Ʊ������
	short  QHFZNUM;			//�ڻ�������
	short  JbmNum;			//������·������
	char   Jbmname[3][10];	//��������Դ����
	char   SZBASE[3][100];	//���ڻ�����·��
	char   SHBASE[3][100];	//�Ϻ�������·��
} CFGINFO ;

typedef struct
{	long 	Date;     			// ����
	long 	TimeFlag;           // ʱ���
	float 	Close;              // ����
	struct 	TickData *pTick;    // Tick ��Ż�����
	short 	nTickNum;           // Tick ��Ŀ
}FBTICKINFO;

typedef struct
{   long 	Date;               // ����
	short   nTickNum;           // Tick ��Ŀ
}DATEANDNUM;

////////////////////////////////////////////////////////
typedef struct 
{
	char	topic[40];
	short	MsgNo;
	short	Status;
	short	type;
}CLE_CONTABLE;

typedef struct
{
	unsigned int Cxg_flag:1;   //���¹ɱ�־
	unsigned int P_flag:2;
	//���̹ɱ�־ =0
	//���̹ɱ�־ =1
	//С�̹ɱ�־ =2
	unsigned int J_flag:2;
	//���Źɱ�־ = 0
	//��ƽ�ɱ�־ = 1
	//����ɱ�־ = 2
	//����ɱ�־ = 3
	unsigned int Jzc_GD_flag:1;
	//���ʲ��� = 1
	//���ʲ��� = 0
	unsigned int Gjj_G_flag:1;
	//������� = 1
	unsigned int Gclr_G_flag:1;
	//��������߱�־
	unsigned int BIAS_Buy_flag:1;	 //BIAS����ָʾ��־
	unsigned int SAR_Buy_flag:1;     //SAR����ָʾ��־
	unsigned int KDJ_Buy_flag:1;     //KDJ����ָʾ��־
	unsigned int RSI_Buy_flag:1;     //RSI����ָʾ��־
	unsigned int MACD_Buy_flag:1;    //MACD����ָʾ��־
	unsigned int MTM_Buy_flag:1;     //MTM����ָʾ��־
	unsigned int SCNX_flag:1;        //�ϴ����߱�־
	unsigned int Free_Bit:17;		 //����bit
}CLE_OTHERXG_RES;

typedef struct
{
	long Xz_sign;//С��,���̷ֽ�  4500���
	long Dz_sign;//����,���̷ֽ�  15000���

	float Yp_sign;//����,��ƽ�ֽ�  ÿ��˰������ÿ�����棩>= 0.60Ԫ
	float Pc_sign;//��ƽ,����ֽ�  ÿ��˰������ÿ�����棩<  0.60Ԫ ����
							   //ÿ��˰������ÿ�����棩>= 0.10Ԫ
	float Ks_sign;//�����־

	short Cxg_Day_L;//���¹�����ʱ����±�־

	float Jzc_G_sign;//���ʲ��߱�־
	float Jzc_D_sign;//���ʲ��ͱ�־

	float Gjx_G_sign;//������߱�־
	float Gclr_G_sign;//��������߱�־
	short DMI_Day;	  //DMI�� 14��DMI
	short RSI_Day;    //������RSI��12��RSI
	short WVAD_Day;	  //24��WVAD
} CLESIGN;

typedef struct
{
	char	topic[30];	//����
	short	MsgNo;		//������
	short	ConType;	//���飬������,����ָ��ֵ�ȼ���
	short	ValueType;	//><= �Ƿ� ѡ���
	char	UnitStr[10];//��λ�ִ�
} CUSTXG_CONTABLE;

typedef struct
{
	short	MsgNo;		//������
	int		PreFix;
	union
	{
		float	fValue;
		DWORD	nValue;
		DWORD	nSel;
	}Value;
	char	ConStr[50];
} USER_CONS;

typedef struct
{
	short	MsgNo;		//������
	int		PreFix;
	union
	{
		float	fValue;
		DWORD	nValue;
		DWORD	nSel;
	}Value;
} REMOTE_USER_CONS;

typedef struct
{   
	char     Flag[8];
	struct
	{	char Version:4;
		char Compressed:1;
		char Reserved:3;
	}Info;
	short FieldNum;
	char reveser[5];
}ATTHEADER,*LPATTHEADER;

typedef struct
{
	char 	Source[40];
	char 	Title[100];
	char 	URL[194];
	short 	Type;
	long 	pos;
	long 	len;
}ATTBODY,*LPATTBODY;

struct NotifyFile
{
	char cardno[30];
	char serno[11];
	char cardname[30];
	char version[13];
	short status;
};

typedef struct 
{
	short Minute;
	float Now;
	DWORD NowVol;
	float BuyP[5];
	DWORD BuyV[5];
	float SellP[5];
	DWORD SellV[5];		

	float NowAverage;		//��ǰ�ɽ�����
	DWORD CJBSDIF;

	short InOutFlag;     //���ڶ�������ʱ����Tic��������־
}TICK_MMP;

enum
{
	NORMAL_AXIS	=   0,
	SAMESCALE_AXIS,	
	SAMESECT_AXIS,
	R100_AXIS,
	LOG10_AXIS ,
	GOLDEN_AXIS ,
};

struct UFjContent
{
	long  price;	//��λ*1000
	long  volume;
	long  buyvol;
	long  sellvol;
	short rate;		//����*1000
	float buyrate;	//������
};

struct CacheFile_Head
{
	short dataflag;
	short nDaynNum;//���Զ�������Ч,����Ϊ0
};

//////////////////////////////////////////////////////////////////

typedef struct tag_InfoDlg
{
	int	 style;
	char name[NAME_SIZE];
	char time1[20];
	char value[20];
	char open[20];
	char max[20];
	char min[20];
	char close[20];
	char volume[20];
	char amount[20];
	char volinstk[20];
	char junja[20];
	char zhangdie[20];
	char zhangfu[20];
	char zhenfu[20];
	char lastclose[20];
	char hsl[20];
	char ltgb[20];
	int	 datanum;
}INFODLG,*LPINFODLG;

typedef struct tag_CbInfo
{
	struct AnalyData *m_pData;
	int	 m_nDataNum;
	int  m_nWhichPeriod;
	int  m_nCurr;
	int  m_nY;
	int	 m_nYC;	
	int	 m_nBottom;
	int	 m_nTop;
	float	m_fNowVal;
	double  m_dMaxPrice;
	double  m_dMinPrice;
	MemStkInfo	* pStk;
}CBINFO,*LPCBINFO;

//���߹��߲���
//�汾:
//0:��ԭʼ�İ汾
//1:��������ע�͵��ֺż���ɫ��ʼ����
//2:�޸�ʱ���ʽ,���ӻ��ߵ��߿�,����,��ɫ����
//3:��ʱ����Ŀǰ�Ծ��Է����������ʶ,���������
typedef struct DRAWLINE
{
	char		SetCode;
	char		Code[8];		//֤ȯ����
	char		acCode[10];  	//ָ��Ӣ�ļ��
	short		nPeriod;		//��������
	UINT		nWay;			//��������
	long		TimeS;			//��ʼʱ��
	float		fPriceS;		//�۸�Ķ�λ
	long		lLineNo;		//���ߵı��(��0��,����ʱ�Լ���ɾ(��ʱ�ӵ�ĩβ,ɾʱ��Ϊ-1,�˳�����ʱ���ɾ����)ʱά��,����ʱ���ı�)
	union
	{
		struct
		{
			long        TimeE;
			float       fPriceE;
			long        TimeR[10];			//�����ֶ�
			float		fPriceR[10];
			DWORD		lversion;			//���������ϰ汾����
			long		lLineWidth;			//�߿�
			long		lLineType;			//����:ʵ��/����/�㻮��
			COLORREF	lcrf;				//��ɫ
		};
		struct
		{
			char		szBuffer[216];			//����
			char		tReserve[20];
			DWORD		tversion;				//���������ϰ汾��ɫ
			int			width;
			int			height;
			int			FontSize;				//�����С//��ʵ��С:-1*(FontSize+6)
			COLORREF	tcrf;					//һ��DWORD�����ɫ
			int			nUsePoint;
		};
		BYTE		bUpDown;
	};
	BYTE		bDelete;					//���ɾ�������ҷ��ţ��������˳��ٴ��������м������
}DRAWLINE,*LPDRAWLINE;

typedef struct FormulaPack				//��ʽ��ģ��
{
	char	aPackName[32];				//����
	short	lRectNum;					//�ɼ�����
	short	nPeriod;					//����,��������ΪnPeriod+1
	char	aFormular[10][32];			//��ʽ����(ȫ��)
}FMLPACK,*LPFMLPACK;

typedef struct FormularToStock			//���ɰ󶨵�ģ��
{
	short	setcode;
	char	code[13];
	char	aPackName[32];				//����
}FML2STK,*LPFML2STK;

////////////////////////////////////////////////////////////////////////
//			Զ�̽ṹ
////////////////////////////////////////////////////////////////////////

struct ManyHostInfo
{
	char strHostName[21];
	char bWebPage;
	char strURL[100];
	char NotifyStr[91];
};

typedef struct
{
	int		Num;
	char	SetCode[20];
	char	Code[20][7];
	float	Now[20];
	float	Other[20];
}ZHRESULT,*LPZHRESULT;

typedef struct
{
	int  HostNum;
	int  PrimaryHost;
	char HostName[150][50];
	char IPAddress[150][50];
	unsigned short Port[150];
	int  weight[150];							//Ȩ��
	char szYybids[150][100];
}TDXWHOST;

typedef struct MinCacheHead
{
	char	suc;	//��û�л���
	long	pos;
}MinCacheHead;

typedef struct CacheHisZst
{
	long	date;
	float	Close;
	struct MinuteData mindata[250];
}CACHEHISZST,*LPCACHEHISZST;

struct HisTickCacheHead
{
	long	nGPIndex;
	long	nDate;
	long	nTickNum;
	long	nStartPos;
};


//////////////////////////��ͳ��///////////////////////////
typedef struct
{   
	char            Name[80] ;         // �Ӳ˵���
    unsigned long	NextMenuID;        // ��һ���˵���ID
	unsigned long	ItemID;			   // �˲˵����ID,Ϊ-1��ʾ������,�����ڲ�ͬ�Ĳ˵��б�ʾ��һ��
}MENUITEMTDX;

struct SYS_INFO
{
	char content[256];//ϵͳ��Ϣ�����ݣ��255���ַ�
	char url[256];//���ָ��ĳһ�����ӣ���ӵ�ַ������url[0]=='\0';
};
struct NLBInfo
{
	BOOL bProcTest;
	long ClientNum;
	long MaxConnectNum;
	long Diff_TickCnt;
};

struct TdxJyHQ
{
	float   Close;
	float   Now;
	float   Buyp[5];
	DWORD   Buyv[5];
	float   Sellp[5];
	DWORD   Sellv[5];
	float	gzlx;
	DWORD	Volume;
	float   unused[4];
};

struct ExternHQ
{
	//�ּۣ����̣�ǰ���̣���ߣ����
	long last_price;
	long open_price;
	long close_price;
	long high_price;
	long low_price;
	//�ɽ����ɽ���
	long business_balance;
	long business_amount;
	//�����
	long buy_price1;
	long buy_price2;
	long buy_price3;
	long buy_price4;
	long buy_price5;	
	//������
	long sale_price1;
	long sale_price2;
	long sale_price3;
	long sale_price4;
	long sale_price5;
	//������
	long buy_amount1;
	long buy_amount2;
	long buy_amount3;
	long buy_amount4;
	long buy_amount5;
	//������
	long sale_amount1;
	long sale_amount2;
	long sale_amount3;
	long sale_amount4;
	long sale_amount5;
	//��Ʊ����
	char Logo_Stock_Code[11];
};

struct ETInfo
{
	char GPSetCode;
	char GPCode[SH_CODE_LEN+1];
	char Ready;
	char IsTdxJyHQ;
	HWND HWnd;
	struct ExternHQ HQ;
	struct TdxJyHQ JyHQ;
};

#define GUEST_LOGIN		0
#define REGISTER_LOGIN	1
#define JY_LOGIN		2

typedef struct tag_LoginInfo
{
	char Has_GuestLogin;
	char GuestLogin_Str[20];
	char Has_RegisterLogin;
	char RegisterLogin_Str[20];
	char Has_JyLogin;
	char JyLogin_Str[20];

	char bDialogRegister;
	char bCanDelMySelf;
	char bMiscJyStyle;
	char bHasAutoRelation;
	char bHasWebRelation;
	BOOL bOnlyWebUser;
	BOOL bEnableUserName;
	char Web_RegisterURL[200];
	char Web_UserInfoURL[200];
}LOGININFO;

struct RollInfo_Habit
{
	BOOL		bRollOpen[2];		//�ǲ��Ǵ�����״̬
	int			nScrollType[2];		//0,����������1������һ��ʱ��ͣ
	int			nInfoType[2];		//0,ֻ��ʾϵͳ��Ϣ��1,ֻ��ʾ��ѡ��
	int			nMovingSpaceTime[2];//�������ʱ��,Խ��Խ��
	int			nMovingStopTime[2];	//����ֹͣʱ��,Խ��ֹͣʱ��Խ��
	COLORREF	crColor[2];			//ȱʡ�Ĺ�����ɫ

	BOOL		bShow;				//�Ƿ���ʾ
	BOOL		bFloat;				//�Ƿ��Ǹ�����
	BOOL		bTopMost;			//�������Ƿ�������ǰ
};

struct Global_Habit
{
	BOOL AWeekNoHint;
	long NoHintStartDate;
	BOOL NoXHCol;
	BOOL NoVScroll;
	int  nShowJUNJ;
	int	 nCurBlockFlag;
	BOOL bRealUPK;
	BOOL bDrawMainZb;			//�Ƿ���ͼָ��
	long nLastHintAutoupTime;	//�ϴ���ʾ�鿴�Զ�����������
	BOOL bPillarBSChart;
	BOOL bBigHq;
	BOOL bBoldZSTLine;
	BOOL bLeftHQInfo;
	BOOL bBigStatusBar;
	BOOL bDynaTJBlock;
	
	int  nPerScale;
	int  nPerSect;
	
	BOOL bLocalData;
	int	 nInitKNum;
	int	 nInitKNum2;
	BOOL bOnlyDownAGData;
	BOOL bShowKGap;
	BOOL bShowMmpChg;
	
	BOOL bFullScreenNotify;
	BOOL bAddBlockPrompt;
	BOOL bLockHQSort;
	BOOL bZSTLevel2Face;
	BOOL bFXTLevel2Face;
	BOOL bTick2Priority;
	
	BOOL bShowBS;
	int	 StatBigVol;
	BOOL bPinAssistPad;
	BOOL bCodeCanSort;
	BOOL bShowTPPriceInZST;
	BOOL bDiffVolColor;

	long lCheckUrgentDate;
	BOOL bCheckUrgent;
	
	long lUseCount;

	BOOL bEmptyZSTRight;
	BOOL bZsShowAverage;
	BOOL bZsShowLX;
	BOOL bHq10Bottom;
	BOOL bDenseZSTAxis;

	UINT nDefPeriod;

	BOOL bHlight_HighLow;

	BOOL bCheckRisk;

	BOOL bKQS_Use10;
	BOOL bKQS_RelationGP;

	BOOL bShowHomePage;

	struct RollInfo_Habit Roll;
};

struct Global_Video
{
	short	nVideoFlag;
	char	strVideoURL[MAX_PATH];
	short	nWndcx;
	short	nWndcy;
};

struct Global_EmbOem
{
	BOOL bEmbedWTVersion;
	BOOL bZszqEmbedWTVersion;	
	BOOL bZxzqEmbedWTVersion;
	BOOL bDgzqEmbedWTVersion;
	BOOL bGxzqEmbedWTVersion;
	BOOL bStdEmbedWTVersion;
	BOOL bYhzqEmbedWTVersion;
	BOOL bGtjaEmbedWTVersion;
	BOOL bShzqEmbedWTVersion;
	BOOL bHyzqEmbedWTVersion;
	BOOL bXyzqEmbedWTVersion;
	BOOL bGdzqEmbedWTVersion;
	BOOL bZxjtEmbedWTVersion;
	BOOL bHlzqEmbedWTVersion;

	BOOL bWebUseInputZjzh;			//����ҳ���Ƿ�����û�������ʺ�
	BOOL bHasTryout;				//�Ƿ�������������վ�Ĺ���
	BOOL bHasOutStandTryOut;		//�Ƿ���ͻ��������������վ�Ĺ���
	BOOL bNoHelpBtnInLogo;			//��¼�Ի������Ƿ��а�����ť
	BOOL bHasSoftKeyCaption;		//�Ƿ�������̵���ʾ
	char SysTestCaption[40];		//ϵͳ��ⰴť�ı���
	char Sel1Caption[40];			//����+���׵İ�ť�ı���
	char Sel2Caption[40];			//��������İ�ť�ı���
	char Sel3Caption[40];			//�������׵İ�ť�ı���
	BOOL bWritePreNameInLogo;		//�Ƿ��ڱ�������д����
	BOOL bWriteVersionInLogo;		//�Ƿ��ڱ�������д�汾��
	BOOL bMediumFontInLogo;			//�ڶԻ����ϲ��ý�С������
	BOOL bCustomQsZXTree;			//�Ƿ���ȯ��������Ѷ��
	char CustomQsZxTreeName[40];	//ȯ��������Ѷ���ı���
	BOOL bCustomQsZxPosSecond;		//ȯ��������Ѷ���ı����ڵڶ�������λ��
	BOOL bStatusInfoLeftAlign;		//��¼�Ի����״̬��Ϣ�����
	char ResDllName[40];			//��ԴDLL�ļ���
	char SpecialCfgFileName[40];	//�Զ���cfg�ļ���
	BOOL bNoExpandTitleInQsZXTree;	//��ȯ���Զ��������Զ�չ�����еı���
	BOOL bOutStandCusWebPageBack;	//�Ƿ���ͻ���ĸ��ɹ����ִ��ı���
	char SSLAqfsName[40];			//ͨ����SSL������
	char AuthPassAqfsName[40];		//��֤���������
	char TdxCAAqfsName[40];			//CA֤�������
	char SmsAqfsName[40];			//���ſ���
	char TokenCardAqfsName[40];		//���ƿ�
	char UKeyAqfsName[40];			//EPASS������½
	BOOL bOppositeMngRight;			//�Ƿ����෴��Ȩ��,�����5,�����0
	BOOL bFormatAndUseYYBID;		//�Ƿ���Ӫҵ�����
	BOOL bHasRelativeInfo;			//�ڷ�ʱͼ���Ƿ��������Ѷ
	BOOL bRealInfoZstFlush;			//ʵʱ��Ѷ�ڷ�ʱͼ���Ƿ������˸
	char ComboBoxKHWTName[40];		//�г��б�����ʾ�Ŀͻ��ŵ�����
	char ComboBoxSHHYName[40];		//�г��б�����ʾ���ʽ��ʺŵ�����
	char DescKHWTName[40];			//�����Ŀͻ��ŵ�����
	char DescSHHYName[40];			//�������ʽ��ʺŵ�����
	BOOL bGgxxTopInZxTree;			//������Ϣ�Ƿ������Ѷ�����ϲ�
	BOOL bMainWebPageInsideOpen;	//���˵�����ҳ�����ڲ���
	BOOL bHasQsZXOnlyLoginJy;		//���û�е�¼���ף���û��ȯ�̵���Ѷ��
	BOOL bHasMetaCJZX;				//�Ƿ�֧�ָ��ϵĲƾ���Ѷ
	BOOL bMetaUseWebProfile;		//Meta��Ϣ����ʾ������ҳ��ʽ
	BOOL bHintCannotUseMeta;		//�Ƿ����ز���ʹ�õ�META��
	int	 nHintCannotUseMinMetaRight;//����META���еĲ����õ���С����
	BOOL bTopShowMetaRightInfo;		//�ڶ�����ʾ������Ѷ��Ȩ����Ϣ
	BOOL bUrgentUseQSIcon;			//�����Ի����Ƿ���ȯ�̵�ͼ��
	
	BOOL bWebButtonInCaption;		//�Ƿ������˵��ϻ���һ���������ַ����(��res*.dll֧��)
	BOOL bRightWebBtn;				//������ⰴť�Ƿ����ұ�
	char strWebButtonURL[100];		//����������ַ����
	char strCaptionQSWebURL[100];	//���Ͻ�����ȯ��ҳ��ĵ�ַ
	char strCaptionHQURL[100];		//���Ͻ����鰴ťҳ��ĵ�ַ

	char strVsatHttp[100];			//ά������ҳ��ַ(����ȯ��ʹ��)
	BOOL bShowPreNameInTopCenter;	//�����˵����м���ʾ������

	BOOL bNoSwitchInLogo;			//�ڵ�¼���治֧���л��������飬�������׵�
	BOOL bHasTdxTend;				//�Ƿ�֧��ͨ���ŵ����Ƶ�����Ϣ
	BOOL bWhiteForeColor;			//�Ƿ�����ɫ��ǰ��ɫ
	BOOL bWhiteBtnForeColor;		//�Ƿ�����ɫ��ǰ��ɫ(���ڵ�¼�����ϵİ�ť)
	BOOL bHideCloseBt;				//�Ƿ���ȥ��¼���ϵĹرհ�ť
	BOOL bUseBtn2Back;				//ͨѶ����,ϵͳ���Ͱ����Ƿ���õڶ��װ�ť���
	char TendName[40];				//���Ƶ���������,�����Ϊ��,���������Ƶ���
	BOOL OnlyJYTend;				//������¼���ײ��й�Ʊ�ع���
	
	int	 nL2Cfg_Shift_left;			//L2���ð�ť��λ��ƫ��
	int	 nL2Cfg_Shift_right;
	int	 nL2Cfg_Shift_top;
	int	 nL2Cfg_Shift_bottom;

	BOOL bF9WTUseF12Key;			//F9ί����ʹ��F12��
	BOOL bF12SelectWT;				//ʹ��F12ʱ��ѡ��ί��
	BOOL bHideJYControl;			//���ز���ʹ�õ�ί�пؼ��� 

	int	 nLogoMaxPassordLen;		//��¼�����������볤��
	int	 nLogoMaxAuthPassLen;		//��¼����������֤�����

	BOOL bHasQSRun;					//�Ƿ����QSRun.dllģ��
	BOOL bDSMustJy;					//��ʱ��չ��վʹ���ǽ����û�����ʹ��
	BOOL bShowTwoHostName;			//��״̬����ͬʱ��ʾ������վ�� Զ�̵�/���ص�
	BOOL bAlwaysSoftKey;			//�Ƿ�һֱ�������

	BOOL bOutStandZXGHY;			//����ѡ�ɺͷ���ı�ǩ����ͻ����ʾ
	BOOL bNoAdvert;					//����ʾ�ڲ���һЩ���
	//������TC6���е�
	BOOL bCanTryoutUser;			//�Ƿ���ע���û�����
	BOOL bCanActiveUser;			//�Ƿ��м����û�����
	BOOL bCanUpgradeUser;			//�Ƿ����û���������
	BOOL bMetaRightFromZXCookie;	//������Ѷ��Ȩ��������zxcookie.dat�ļ�
	BOOL bFirstHideWTFace;			//��һ�ν�������ؽ��׽���
	BOOL bFirstOpenZXTree;			//��һ�ν����ǿ�ƴ���Ѷ��
};

struct Global_Cfg
{
	BOOL bLocalData;
	BOOL bShowVIPCfg;

	BOOL bHasLevel2Engine;
	char Level2_AutoupId[11];
	char Level2_SepcComte[21];
	BOOL bHasTdxInfo;
	BOOL bHasTdxAdv;
	BOOL bMustTdxLevel2;
	BOOL bAutoupFromLevel1;
	BOOL bHasQzSDS;
	BOOL bZHFromExtern;
	char Extern_LoginName[50];
	char Extern_Password[50];
	
	BOOL bIsInsideVersion;	
	BOOL bIsHKVersion;
	BOOL bIsSHLevel2;
	BOOL bIsSZLevel2;
	BOOL bIsSSDS;
	BOOL bHasKQSFunc;
	BOOL bHasZxTree23;
	BOOL bSupportStatusRoll;
	BOOL bSupportSlowStatusRoll;
	BOOL bNoAutoup;
	BOOL bShowHQNameInStatusBar;
	BOOL bNoUrgent;
	long WhichAutoUpInfo;
	char productnbbs[10];
	
	char version_title[40];
	BOOL nShareMode;
	char strHabitPath[100];
	
	char OtherFileName[40];
	char AlongPathFile[80];
	BOOL bAlongJy;
	BOOL bAlongOpenWT;
	BOOL bAlongOpenHXInfo;
	BOOL bOnlyHK;
	
	char HKey_Path[180];		//ע�����
	char HKey_KeyString[180];	//ע����ֵ
	char HKey_RunParam[180];	//ע����������ʱ�Ĳ���
	char HKey_MenuName[40];		//����Ĳ˵�����
	char HKey_ErrorInfo[180];	//���г���ʱ������Ϣ

	BOOL bShortRefreshHQ;
	BOOL bHasGPGrade;
	BOOL bHasTend;

	struct Global_Video Video;
};

struct Global_Url
{
	char EmailStr[50];;
	char EmailURL[256];;
	char HelpWebStr[50];
	char HelpWebURL[256];
	char ForumURL[256];
	char Corp[50];
	char CorpWeb[256];
	char HelpURL[256];
	char AboutURL[256];
	char WebInfoURL[256];
	char AddZxStr[50];
	char AddZxURL[256];
	char LogoHelpStr[50];
	char LogoHelpURL[256];
};

struct Global_SysInfo
{
	long Program_StartYmd;
	long Program_StartHms;
	long ConnectHQ_LastYmd;
	long ConnectHQ_LastHms;
	long ConnectDSHQ_LastYmd;
	long ConnectDSHQ_LastHms;
	long ConnectZx_LastYmd;
	long ConnectZx_LastHms;
};

struct Global_DSCfg
{
	char	DSOEMName[50];
	BOOL	DSTabFirst;
	BOOL	HasHKMarket;
	BOOL	HasHKYSInfo;
	BOOL	MustInGUI;
};

struct AntiHack_Cfg
{
	BOOL  bJudgeExpireDate;
	BOOL  bUserHostEverBOOL;
	BOOL  bPassorted;
	BOOL  bLogo;
	BOOL  bDSLogo;
	char  cLevel2XORChar_Queue;		//����
	char  cLevel2XORChar_TickL2;	//���
	char  cLevel2XORChar_ComboHQ;	//���������
	char  cLevel2XORChar_ComboHQ2;	//���������
};

struct NetParam_Cfg
{
	BOOL  bNoPassPort;
};

//////////////////����ƽ̨////////////////////////////////////

struct tag_VitalBtn
{
	int BtnNum;
	char BtnStr[20][50];
	char BtnType[20];	//0:���س��� 1:��Ѷϵͳ 2:����ί�� 3:�������� 4:������� ...
	char BtnExecFile[20][180];
	UINT BtnID[20];
};

struct tag_OP_AssisLevel
{
	char Text[80];
	short GnNO;
	short SonNum;
};

struct tag_OP_SubAssisLevel
{
	char Text[80];
	short GnNO;
};

//��WEB�йصĽṹ

typedef	struct	tagIEInputData
{
	char	FieldName[64];
	char	SetValue[64];
	BYTE	InputType;
}IEINPUTDATA;

typedef struct tag_ChatArea
{
	int		ChatAreaNum;		//����������Ŀ,���ܳ���20��
	char	ChatAreaStr[20][50];//����������
	char	AreaType[20];		//����������
	char    SwitchRefresh[20];	//�л�ʱ�Ƿ�ˢ��,��Щ��ҳ�л�ʱ��ˢ��
	char	ChatURL[20][200];	//��������URL(�������������ҳ�Ļ�,����Ϊ��)
	int		URLDataNum[20];		//URL�ĵ�¼���������ܳ���10��
	IEINPUTDATA URLData[20][10];//URL�ĵ�¼����
}CHATAREA;

typedef struct
{
	short infotree_num;
	short infotreeid[100];
	char  infotreestr[100][50];
	char  infotreeurl[100][200];
	short infoopenie[100];
	char  infoautodown[100][50];
	short infotreeqx[100];
}INFOTREEID;

typedef struct
{
	char	description[80];
	short	m_nSelHY;
	short	m_nSelDY;
	char	m_setcode;
	char	m_code[SH_CODE_LEN+1];
	short	m_zxcompanynum;
	short	m_zxcompany[30];
	char	m_search_type;
	long	m_time_ymd[2];
	short	m_hy_relation;
	short	m_dy_relation;
	char	m_author[30];
	char	m_keywordstr[100];
}RECENTSEARCH;	//������ViewThem.DLL�б���һ��

//////////////////////////////////////adde by iwld 2003.4.23�Զ�ָ��
typedef struct
{
	char Code[SH_CODE_LEN+1];
	char Name[NAME_LEN+1];
	NTime StartTime;		//ָ����ʼʱ��
	float fBaseValue;		//��ָ
	float fBaseScale;		//�ͻ�ָ�ıȼ�
	int nStart;				//���ɼ�������
	UINT CalcProp;			//����Ȩ�ؼ��㷽ʽ
	BOOL bRealTime;			//�Ƿ�ʵʱˢ��
	int nGp;				//������
}SelfZSHd;

typedef struct
{
	int nIndex;			//��Ʊ����
	float fBasePrice;	//��ʼ��
	float fPropValue;	//��ռȨֵ,δ����ʱ��ͣ��ʱΪ0
}SelfZSUnit;

typedef	struct 
{
	char BigsetName[32];
	long MarketSet[MAX_MKT_NUM];
	char MarketName[MAX_MKT_NUM][MRK_NAME_SIZE];
}GUIMARKET;

enum
{
	HQ_LINE=0,			//	�ָ���
	HQ_BJ_FLOAT,		//	�ǵ���ɫ��ʾfloat
	HQ_PLUS_FLOAT,		//	ͬ0�Ƚϵ�float
	HQ_PLUS_FLOAT_PER,	//	ͬ0�Ƚϵ�float(�ٷֱ�)
	HQ_VOL_LONG,		//	��ʾ�ɽ���
	HQ_RED_LONG,		//	��ʾ��ɫ
	HQ_GREEN_LONG,		//	��ʾ��ɫ
	HQ_VOL_PLONG,		//  ��ʾ�ɽ���(��ȷ��ʾ����)
	HQ_FLOAT,			//	��ͨfloat
	HQ_FLOAT_PER,		//	�ٷֱ���ʾfloat
	HQ_LONG,			//	��ͨlong
	HQ_STRING,			//	�ַ���
	HQ_PRICE_SEAT,		//  ��λ+ϯλ��
	HQ_AMOUNT_FLOAT,	//	��ʾ���
	HQ_NULL
};

typedef	struct	tagHqItem
{
	char	lpScript[13];
	BYTE	Type;
	BYTE	Pos;
	long	lData;
	float	fData;
	LPSTR	lpString;
}HQITEM,*PHQITEM;

typedef	struct tagCusWebPage
{
	char	PageUrl[MAX_PATH];
	char	PageName[64];
}CUSWEBPAGE, *PCUSWEBPAGE;

enum
{
	ABDEF_ITEM,
	DSOPT_ITEM,
	SELF_ITEM
};

typedef	struct tagGridCatItem
{
	BYTE	FlagType;			//
	char	lpTitle[13];
	UINT	msg;
	WPARAM	wParam;
	LPARAM	lParam;
	BOOL	bMenu;
	long	lData;
	DWORD	Reserved;
}GRIDCATITEM, *PGRIDCATITEM;

typedef	struct
{
	UINT		ItemID;
	char		ItemString[50];
	COLORREF	Color;
}SPECMENUCOLOR;

struct ETF_Info
{
	short setcode;
	char  Code[SH_CODE_LEN+1];
	char  Desc[20];
	char  IOPVCode[SH_CODE_LEN+1];
	char  IndexCode[SH_CODE_LEN+1];
};

struct Status_GP
{
	char	lpScript[20];
	BYTE	setcode;
	char    Code[SH_CODE_LEN+1];
};

struct News_Title
{
	short   info_type;	//��Ϣ����
	long    rec_id;		//��¼��ΨһID
	long	time_ymd;	//�������ڣ�������
	long	time_hms;	//����ʱ�䣬ʱ���� 
	char	title[64];	//����
	char	infosrc[20];//��Ϣ��Դ
	short	infoformat;	//��Ϣ��ʽ
	char	author[30];	//����
};

struct MineTitleCache
{
	long  RefreshHms;
	short TitleNum;
	struct ext_info_title *pList;
};

typedef struct			//��ͼ��ʷ����ģʽ
{
	char KeyString[10];
	char InfoString[20];
}MAINLINE;

struct VipVerify_Info
{
	char RemoteVerify;
	char ReVerifySpan;	//��Ҫ������֤������,ȱʡΪ80��
	char DenyInfo;		//����֤�ǲ��ǲ��ܵ�¼��Ѷ
};

struct FY_GROUP_INFO
{
	CString			strGroupName;
	int				groupAttribute;	//group��������
	int				belong;			//��group�����
	vector<UINT>	subGroup;		//��group�����
	vector<CString> strItemName;	
	vector<CString> strShowTitle;	
	vector<int>		itemAttribute;	//ָ���Ӱ����������,��ȷ����ǩ������ɫ�ȵ�
	UINT nCurSel;					//���鴦�ڼ���״̬��item
	UINT nIndex;					//��switchbar�е����(��1Ϊ����)
	UINT nIndex_file;				//��Ӧ���ļ����(��1Ϊ����)
	BOOL bEncrypted;				//�Ƿ����
	BOOL bActive;					//ʵ��group���ڴ�״̬
	int  nType;						//����:0��ʾ�б� 1��ʾ�ı�
	FY_GROUP_INFO()
	{
		nIndex=-1;
		groupAttribute=0;
		belong=-1;
		nCurSel=0;
		bEncrypted=FALSE;
		nIndex_file=0;
		bActive=FALSE;
		nType=0;
	}
};

struct _FY_GROUP_INFO
{
	CString strGroupName;
	CString	parentName;
	int nIndex;		//��Ӧvector<FY_GROUP_INFO>�е����
	int nSubIndex;	//���Ƕ���group,���ӦFY_GROUP_INFO��subGroup�����
};

struct FY_LOCATION
{
	int		nGroup;
	int		nItem;
	short	nRow;
};

typedef struct ADVERTBUTTON
{
	char btTxt[21];
	char url[200];
	COLORREF txtCor;
	UINT advdlg_cx;
	UINT advdlg_cy;
	CRect rect;
}ADVERTBUTTON,*PADVERTBUTTON;

struct ADVERT_INFO
{
	char F10AdvertStr1[256];
	char F10AdvertStr2[256];
	short nAdvBt;
	ADVERTBUTTON AdvButton[5];
};

struct NewJy_CFG
{
	char	CaptionWeb1[200];
	char	RelativeInfo[200];
	BOOL	bHasQSRelativeInfo;
	char	QSRelativeName[20];
	char	QSRelativeInfo[200];

	char	AddedWT_Name[20];
	char	AddedWT1_Name[20];
	char	AddedWT1_Path[200];
	char	AddedWT2_Name[20];
	char	AddedWT2_Path[200];

	BOOL	bAlwaysStatusRoll;

	char	ZXHomePage[200];
};

struct LinkedSetInfo
{
	BOOL	bUsed;
	
	int		WhichGpMarket;
	short	ColType;
	int		nStartRow;
	int		nTolRow;
	int		nValidRow;
	int		nVisualRow;
	int		SortType;
	CWnd	*pMsgRtnWnd;
	short	lpnUsedStkIndex[MAXGPNUM];
};

//////////////////////////////////////////////////////////////////////////
//Ԥ����ؽṹ

struct WarnHQCache
{
	short			nLastCalcMin[MAX_TJWARN];
	CurrStockData	iHq;
};

//���ӵ���Ϣ
struct OverlapInfo
{
	char		nAutoType;			//�Զ���������,Ϊ0��ʾ�����Զ�����
	short		nAxis;				//��������
	short		nNum;				//�ֶ����Ӹ���
	short		StkSetCode[3];
	char		StkCode[3][SH_CODE_LEN+1];
	char		bOverlapHisIndex;
	MemStkInfo	*pNowStk;			//��ǰ����ɵ�ָ��
	MemStkInfo	*pOverlapStk[3];	//�ֶ����ӹɵ�ָ��
	DWORD		unused[100];
};

//����������̾���
struct KeyGuyCNInfo 
{
	CWnd	*pOwner;
	long	nKeyGuyCNing;
	char	strKeyGuyCN[20];
};

//////////////////////////////////////////////////////////////////////////
//���ư��漰��֮��
struct cmpstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

class UBase;
struct UnitNodeInfo
{
	int NodeID;				//�ڵ�ID,��ڵ㣽��ID*2,�ҽڵ㣽��ID*2+1,���ڵ㣽0
	int splitType;			//�ָ�����:-1���� 1���� 0������Ԫ
	float toSplit;			//�ָ�ٷֱ�,���ֵܽڵ㹲100
	CRect rect;				//�ָ�������
	int unitType;			//Unit����,���ӽڵ���Ϊ-1
	BOOL bContrallUnit;		//�Ƿ�����Unit
	UBase*	pUnit;			//Unit��ָ��,���ӽڵ���ΪNULL
	short	style;			//����Unit�ķ��,���ӽڵ���Ϊ-1
	short	var_GpIndex;	//Unit�����Ĺ�Ʊ,���ӽڵ���Ϊ-1, ��Ϊ������Ϣ
	BOOL	bLocked;		//�Ƿ�����
	long	nJHMin[2];		//�Ƿ񼸺�������0--width 1--height
};

struct UnitTreeNode
{
	UnitNodeInfo* pCurUnit;
	UnitTreeNode* pParent;
	UnitTreeNode* pLeftChild;
	UnitTreeNode* pRightChild;
};

struct SplitManInfo
{
	int splitType;
	CRect cursorRt;
	COLORREF borderColor;
	UnitTreeNode *node;
};

struct SplitActInfo
{
	int nDepUnit;
	int nSplitType;
	float fToSplit;
	UnitTreeNode* pNode;
};


//�۹����ݿ���ҵ����
struct	FL_CodeName 
{
	char	SectorCode[21];
	char	SectorName[21];
};

//���������Ϣ
typedef struct FUNDBASE
{
	char szSymbol[7];	//�������
	char szName[21];	//������
	char chType;		//�������ͣ�0������ʽ��1�����ʽ,2:����ʽ�����ͻ���
	
	char szCompanyAName[31]; //�������������
	char szInvestStyle[21];	 //����Ͷ�ʷ��
	char szInvestStylePro[21];//����Ͷ�ʷ������
	
	long  lNetDate;		//��ֵ����
	float fNet;			//�������¾�ֵ
	float fLastNet;		//�������ھ�ֵ
	float fCumNet;		//���������ۼƾ�ֵ
	float fFundShare;	//���𼾱��ݶ�
	float fTotalNet;	//���𼾱���ֵ�ܶ�
	float fLastTotalNet;//��һ�ڼ�������ֵ�ܶ�
	float fStockTotal;	//���¼�����Ʊ��ֵ
	float fStockRate;	//��ƱͶ��ռ��ֵ����
}FUNDBASE,*pFUNDBASE;

//����ֹ���Ϣ
struct MYSTOCKINFO
{
	char szSymbol[7];	//��Ʊ����
	float fAcount;		//��������
	float fMarketValue;	//������ֵ
	float fLastAcount;	//������ֵ
};

typedef struct FUNDHOLDSTOCK
{
	char szSymbol[7];	//�������
	short nNum;			//��Ʊ��Ŀ
	MYSTOCKINFO stockinfo[10];//��Ʊ�ṹ����
}FUNDHOLDSTOCK,*pFUNDHOLDSTOCK;

struct Seat_Code2Com 
{
	short	setCode;
	long	ComID;
};

struct Seat_Code2Com_Ex
{
	long	SeatID;
	short	setCode;
	long	ComID;
};

struct Seat_Com2Name 
{
	char	ComNameS[21];
	char	ComNameL[61];
};

struct Seat_Com2Name_Ex
{
	long	ComID;
	char	ComNameS[21];
	char	ComNameL[61];
};

struct ShortCut_Cfg
{
	char	setcode;
	char	RawCode[SH_CODE_LEN+1];
};

///////////////////////////////////////////////////////////////////////////
//���
#include "CMLDPlugInInterface.h"
struct	ClintPluginInfo
{
	ICMLDPlugIn_Client		*g_pClientInterface;
	CNewMenu				*g_pCmldAppMenu;
	CString					g_MeneItemString;
	
	ClintPluginInfo()
	{
		g_pClientInterface = NULL;
		g_pCmldAppMenu = NULL;
		g_MeneItemString = _T("");
	}
};

//////////////////////////////////////////////////////////////////////////
//���淽��
struct Scheme_King
{
	char	QSID[11];			//��ͬ�����ͻ�
	char	VersionString[21];	//�汾��
	int		nVersion;			//�汾���

	int		Scheme_Hold_No;		//��ɫ���������

	UINT	LoginDB_ID;
	UINT	MainMenuID;
	UINT	ToolBarID;
	UINT	ToolBarID_Hot;
	UINT	ToolBarID_Disable;
	char	PopFileUrl[MAX_PATH];
	short	nLup[32];

	BOOL	bShowToolBar;
	CString	HomePage;
	int		nVertiOff;
	int		nParaOff;
	CString	StartDownFile;
	CString	YxhjPath;
	BOOL	bStatusSwitch;

	Scheme_King()
	{
		memset(nLup, 0, sizeof(nLup));
	}
};
/////////////////////////////////////////////////////////////////////////////
#endif
