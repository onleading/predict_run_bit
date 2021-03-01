#ifndef OUTSTRUCT_H_
#define OUTSTRUCT_H_

extern  BYTE TMEANLESS_DATA[8];
#define TMEANLESS_INT				87654321
#define TSET_MEANLESS(a)	        memset(&(a),0xF8,sizeof(a))
#define TSET_NULL(a,from,num)		memset(a+(from),0xF8,sizeof(float)*(num))
#define TBE_MEANLESS(a)				memcmp(&(a),TMEANLESS_DATA,sizeof(a))

#pragma pack(push,1) //�����һ�ֽڶ���

typedef struct tag_NTime
{
	unsigned short year;
	unsigned char month;
	unsigned char day;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
}NTime;

//�ص�����
typedef long(CALLBACK * PDATAIOFUNC)(char * Code,short nSetCode,short DataType,void * pData/*�ڲ�����*/,short nDataNum,NTime,NTime,BYTE nTQ,unsigned long);
//ʵ�ֹ����л��Ļص�����]
typedef long(CALLBACK * PSWITCHINFOFUNC)(char * Code,short nSetCode,char *buf,long buflen,short FuncType,short Option,unsigned long);


typedef struct tag_HISDAT
{
	NTime Time;			//ʱ��
	float Open;			//��λ���̼�
	float High;			//��λ��߼�
	float Low;			//��λ��ͼ�
	float Close;		//��λ���̼�
	union
	{
		float	Amount;
		DWORD	VolInStock;
	};
	float fVolume;       //�ɽ���
	union 
	{ 
		float YClose;
		struct
		{   
			WORD up;                // ���Ǽ���
			WORD down;              // �µ�����
		}zd;						// ��λǰ���̼�(��Ȩ��)���ڻ��ĳֲ���
	};
}HISDAT,*LPHISDAT;

typedef struct ExDayInfo					//	120B
{
	long			Date;					//	4B		ʱ��
	//
	float			Cjbs;					//	4B		�ɽ�����(���ڴ�����,�ۼ�)
	float			BuyCjbs;				//	4B		����ɽ�����(���ڴ�����,�ۼ�)
	float			SellCjbs;				//	4B		�����ɽ�����(���ڴ�����,�ۼ�)
	float			BigBCjbs;				//	4B		����󵥳ɽ�����(���ڴ�����,�ۼ�)
	float			BigSCjbs;				//	4B		�����󵥳ɽ�����(���ڴ�����,�ۼ�)
	//
	float			ItemNum;				//	4B		��������(���ڴ�����,�ۼ�)
	float			BuyItem;				//	4B		�����������(���ڴ�����,�ۼ�)
	float			SellItem;				//	4B		������������(���ڴ�����,�ۼ�)
	float			BigItem;				//	4B		�󵥲�������(���ڴ�����,�ۼ�)
	float			BigBItem;				//	4B		����󵥲�������(���ڴ�����,�ۼ�)
	float			BigSItem;				//	4B		�����󵥲�������(���ڴ�����,�ۼ�)
	//
	float			BuyVolume;				//	4B		����ɽ���(���ڴ�����,�ۼ�)
	float			SellVolume;				//	4B		�����ɽ���(���ڴ�����,�ۼ�)
	float			BigVolume;				//	4B		�󵥳ɽ���(���ڴ�����,�ۼ�,����)
	float			BigBVolume;				//	4B		�ɽ�������(���ڴ�����,�ۼ�,����)
	float			BigSVolume;				//	4B		�ɽ�������(���ڴ�����,�ۼ�,����)
	//
	float			BigTickBVol;			//	4B		����󵥳ɽ���(���ڴ�����,�ۼ�,���)
	float			BigTickSVol;			//	4B		�����󵥳ɽ���(���ڴ�����,�ۼ�,���)
	//
	float			fBOrder;				//	4B		���ܹҵ�(���ڴ�����,�ۼ�)
	float			fBCancel;				//	4B		���ܳ���(���ڴ�����,�ۼ�)
	float			fSOrder;				//	4B		���ܹҵ�(���ڴ�����,�ۼ�)
	float			fSCancel;				//	4B		���ܳ���(���ڴ�����,�ۼ�)
	float			fTotalBOrder;			//	4B		����ʱί������(���ڴ�����,ȡ���һ��)
	float			fTotalSOrder;			//	4B		����ʱί������(���ڴ�����,ȡ���һ��)
	float			fBuyAvp;				//	4B		����ʱί�����(���ڴ�����,ȡ���һ��)
	float			fSellAvp;				//	4B		����ʱί������(���ڴ�����,ȡ���һ��)
	char			Reserved[12];			//	12B
}EXDAYINFO,*LPEXDAYINFO;

typedef struct tag_REPORTDAT
{
	WORD           ItemNum;              // ��������
	float          Close;                // ǰ���̼�
	float          Open;                 // ���̼�
	float          Max;                  // ��߼�
	float          Min;                  // ��ͼ�
	float          Now;                  // �ּ�
	DWORD          Volume;               // ����
	DWORD          NowVol;               // ����(���ֲ�)
	float          Amount;               // �ܳɽ����
	DWORD          Inside;               // ����
	DWORD          Outside;              // ����
	float          TickDiff;             // ���ǵ�(��λ��)
	BYTE		   InOutFlag;			 // �����̱�־ 0:Buy 1:Sell 2:None
	union
	{
		struct	//����
		{
			float          Buy;			// ��߽����
			float          Sell;        // ��ͽ�����
			float          Buyp[3];		// ���������
			DWORD          Buyv[3];		// ��Ӧ��������۵���������
			float          Sellp[3];	// ����������
			DWORD          Sellv[3];	// ��Ӧ���������۵���������
		}Ggpv;
		struct	//ָ��
		{
			float			LxValue;	// ����ָ��
			float			Yield;		// ������Ȩ��ָ��
			long			UpHome;		// ���Ǽ���
			long			DownHome;	// �µ�����
			float			JJAmount;	// ����ɽ����
			float			ZQAmount;	// ծȯ�ɽ����
			float			GZAmount;	// ��ծ�ɽ����
			float			QZAmount;	// Ȩ֤�ɽ����
			float			QTAmount;	// ָ�������ɽ����
		}Zspv;
	}Other;
}REPORTDAT,*LPREPORTDAT;	

typedef struct tag_STOCKINFO 
{
	char        Name[9];			// ֤ȯ����

	short       Unit;               // ���׵�λ	
	long		VolBase;			// ���ȵĻ���
	float       cdp;				// �����м�
	float       nh;					// ����
	float       nl;					// ���
	float       ah;					// �߼�ͻ�Ƶ�
	float       al;					// �ͼ�ͻ�Ƶ�
 	short       dkflag;				// �ʺ����������
	float       tbp;				// ���ƽ���
	float       stoplost;			// ֹ���
	float       leave;				// �˽��

	float       ActiveCapital;		// ��ͨ�ɱ�
    long        J_start;			//��������
	short       J_addr;				//����ʡ��
	short       J_hy;				//������ҵ
    float       J_zgb;				//�ܹɱ�
    float       J_zjhhy;			//֤�����ҵ
    float       J_oldjly;			//������ھ�����
    float       J_oldzysy;			//���������Ӫ����
    float       J_bg;				//B��
    float       J_hg;				//H��
    float       J_mgsy2;			//����ÿ������ (�Ʊ����ṩ��ÿ������,������Ĳ���)
    float       J_zzc;				//���ʲ�(Ԫ)
    float       J_ldzc;				//�����ʲ�
    float       J_gdzc;				//�̶��ʲ�
    float       J_wxzc;				//�����ʲ�
    float       J_cqtz;				//����Ͷ��
    float       J_ldfz;				//������ծ
    float       J_cqfz;				//���ڸ�ծ
    float       J_zbgjj;			//�ʱ�������
    float       J_jzc;				//�ɶ�Ȩ��(���Ǿ��ʲ�)
    float       J_zysy;				//��Ӫ����
    float       J_zyly;				//��Ӫ����
    float       J_yszk;				//Ӧ���ʿ�
    float       J_yyly;				//Ӫҵ����
    float       J_tzsy;				//Ͷ������
    float       J_jyxjl;			//��Ӫ�ֽ�����
    float       J_zxjl;				//���ֽ�����
    float       J_ch;				//���
    float       J_lyze;				//�����ܶ�
    float       J_shly;				//˰������
    float       J_jly;				//������
    float       J_wfply;			//δ��������
    float       J_tzmgjz;			//����ÿ�ɾ��ʲ� ��������:  ���ʲ�/��������ܹɱ�
	//���Ӳ���
	float		J_jyl;				//������%
	float		J_mgwfp;			//ÿ��δ����
	float		J_mgsy;				//ÿ������
	float		J_mggjj;			//ÿ�ɹ�����
	float		J_mgjzc;			//ÿ�ɾ��ʲ�
	float		J_gdqyb;			//�ɶ�Ȩ���
}STOCKINFO,*LPSTOCKINFO;

typedef struct tag_TICKDAT
{ 
	NTime	Time;
	float   Now;            //�ּ�(Ԫ)
	DWORD   NowVol;         //����(��)
	float 	Amount;		    //�ɽ����(Ԫ)
	short   InOutFlag;      //������־
}TICKDAT,*LPTICKDAT;

typedef struct tag_BSPDAT
{
	NTime	Time;
	float   Now;
	unsigned long NowAmount;	//���ʳɽ����(��Ԫ)
	long	NowVol;
	float   BuyP[3];
	long    BuyV[3];
	float   SellP[3];
	long    SellV[3];
	char    InOutFlag;
	char    MaxFlag;//��߼۱�־ 0����߼���ǰ���ѳ��ֻ���Now��Ϊ��߼�1:��߼۴����Max��
	char    MinFlag;
	float   Max;
	float   Min;
}BSPDAT,*LPBSPDAT;

typedef struct tag_GBInfo	//�ɱ��ܹɱ���Ϣ
{
	float Zgb;
	float Ltgb;
}GBINFO,*LPGBINFO;

typedef struct tag_OtherInfo
{
	short RelativeMin;	//�뿪�е���Է�����
}OTHERINFO,*LPOTHERINFO;

typedef struct tag_ALLGPInfo
{
	short StockNum;			//ÿ�ַ���Ĺ�Ʊ��
	char  buf[1];			//���г�[1],����[6],����[8]˳����
}ALLGPINFO,*LPALLGPINFO;

typedef struct tag_FLInfo
{
	char  FLNum;			//���100������
	char  FLName[100][20];	//������
	short FLStockNum[100];	//ÿ�ַ���Ĺ�Ʊ��
	char  buf[1];			//���г�[1],����[6],����[8]˳����
}FLINFO,*LPFLINFO;

typedef struct tag_TPPrice
{
	float Close;
	float TPTop;
	float TPBottom;
}TPPRICE,*LPTPPRICE;

typedef struct tag_SysInfo
{
	char	bYesterday;
	long	lOpenRq;
	char	bOnLine;
	char	productnbbs[10];
	//
	char	canlevel2;
	char	cMagic1;
	char	cMagic2;
	char	cIsStdVersion;
	char	strUserName[50];
	//
	char	bDSOnLine;
	char	bIsSSDS;
	char	unused[24];
}SYSINFO,*LPSYSINFO;

typedef struct tag_CurFxtZbData
{
	char	setcode;
	char	strCode[20];
	float	fNow;
	float	fBuyp;
	float	fSellp;
	float	fOutLine[6];
}CURFXTZB,*PCURFXTZB;

typedef struct tag_GPInfoData
{
	char	strCode[20];
	char	strName[40];
	short	setcode;
	short	xsflag;
	char	unused[14];
}GPINFO,*PGPINFO;

typedef struct tag_JYInfoData
{
	char	isLogin;
	char	zjzh[41];
	WORD	yybid;
	char	khqx;
	char	vipuserflag;
	char	MngRight;
	char	unused[33];
}JYINFODATA,*PJYINFODATA;

typedef struct tag_WinRectData
{
	RECT	MainFrameRect;
	RECT	ActiveViewRect;
}WINRECT_DAT,*PWINRECT_DAT;

/////���Ǽ���
#define PER_MIN5		0	//5��������,��Ҫ����5��������
#define PER_MIN15		1	//15��������,��Ҫ����5��������
#define PER_MIN30		2	//30��������,��Ҫ����5��������
#define PER_HOUR		3	//1Сʱ����,��Ҫ����5��������
#define PER_DAY			4	//��������,��Ҫ������������
#define PER_WEEK		5	//��������,��Ҫ������������
#define PER_MONTH		6	//��������,��Ҫ������������
#define PER_MIN1		7	//
#define PER_MINN		8	//
#define PER_DAYN		9	//����������,��Ҫ������������
#define PER_SEASON		10	//����,��Ҫ������������
#define PER_YEAR		11	//����,��Ҫ������������

#define	EXDAY_L2		31	//����չ����

#define GBINFO_DAT		103	//�ɱ���Ϣ
#define	REPORT_DAT		104	//��������
#define	STKINFO_DAT		105	//��Ʊ�������

#define ALLGPINFO_DAT	106	//���й�Ʊ�����Ϣ
#define FLGINFO_DAT		107	//�������Ϣ
#define DYGINFO_DAT		108	//���������Ϣ
#define HYGINFO_DAT		109	//��ҵ�����Ϣ
#define GNGINFO_DAT		110	//��������Ϣ
#define ZDGINFO_DAT		111	//�Զ�������Ϣ
#define ZHGINFO_DAT		112	//��ϰ����Ϣ

#define TPPRICE_DAT		121	//�ǵ�ͣ����
#define SYSINFO_DAT		122	//ϵͳ��Ϣ
#define CUR_FXTZB_DAT	123	//��ǰ����ͼָ������
#define CURGPINDEX_DAT	124	//���ص�ǰ�Ĺ�ƱID
#define GPINDEX_DAT		125	//���ݵ�ǰ����õ���ƱID
#define GPINFO_DAT		126	//���ݹ�ƱID ���ص�ǰ����Ϣ
#define JYINFO_DAT		127 //�õ����������Ϣ
#define WINRECT_DAT		128	//���ڵ�λ�úʹ�С

#define OTHER_DAT		130	//����

/////////////
#define ASK_ALL			-1


//�����л�������
#define SWITCH_TOZST			11
#define SWITCH_TOVIP			12
#define SWITCH_ADDBLOCK			13
#define SWITCH_SAVENEWBLOCK		14
#define SWITCH_OPENURL			15		//��URL����
#define SWITCH_REFRESHALLHQ		16		//ˢ����������
#define SWITCH_TOZSGPC			17		//�л������̹�Ʊ��
#define SWITCH_TOWARNWT			18		//Ԥ��ί��
#define SWITCH_JYGETHQ			22		//֪ͨǰ�˸�������Ӧ������ͼ
#define SWITCH_SETKEY			23		//����ݼ���������
#define SWITCH_SHIFTCJZX		24		//�����ƾ���Ѷ������(F7)
#define SWITCH_CLOSECJZX		25		//�رղƾ���Ѷ
#define SWITCH_CLOSEASSISWND	26		//�رո�����
#define SWITCH_REFRESHTITLE		27		//ˢ�����������״̬
#define SWITCH_LOGINEMBWT		28
#define SWITCH_QUITEMBWT		29		//�ر�ί�н�����
#define SWITCH_MAXASSISWND		30		//ί�и��������
#define SWITCH_INZXG			31		//�Ƿ�����������ѡ���� ����0 ��ʾ����
#define SWITCH_ADDTOZXG			32		//��ӵ�������ѡ����
#define SWITCH_OPENEMBWT		33
#define SWITCH_TOGGLEASSISWND	34		//�л�������
#define SWITCH_JY_ISFULLSIZE	35		//�������Ƿ���ȫռ״̬

//////////////////////////////////////////////////////////////////////

typedef struct tag_OP_Scheme_Color
{ 
	COLORREF	BACKCOLOR;			//����
	COLORREF	BACKCOLOR2;			//����2
	COLORREF	TXTCOLOR;			//ͼ������

	//����ͼ���
	COLORREF	AXISCOLOR;			//�������
	COLORREF	TIMECOLOR;			//��������(����ʮ���ߡ��α��)
	
	COLORREF	KPCOLOR;			//�����߼�������
	COLORREF	KNCOLOR;			//������
	COLORREF	DOWNCOLOR;			//�µ���
	COLORREF	LEVELCOLOR;			//ƽ��

	COLORREF	VOLCOLOR;			//�ɽ�����ɫ
	COLORREF	BIGTITLECOLOR;		//�����
	COLORREF	FLOATCURSORCOLOR;	//������ɫ

	//�б����
	COLORREF	Grid_CodeName_Color;//����������ɫ
	COLORREF	GridLineColor;		//�б�ķָ�����ɫ
	COLORREF	GridHighLineColor;	//����ѡ���ߵ���ɫ
	COLORREF	GridTitleTextColor;	//��Ŀ������ɫ
	COLORREF	GridTitleArrowColor;//��Ŀ�������ɫ
	COLORREF	Grid_Spec_Color;	//�ر���ɫ�����������ΪTXTCOLOR

	//��Ѷ��ɫ
	COLORREF	ZxColor;
	COLORREF	ZxTitleColor;
	COLORREF	MineTitleTxtColor;
	COLORREF	MineTitleSelColor;
	COLORREF	MineTitleSpeColor;
	COLORREF	F10TitleColor;
	COLORREF	F10TitleSelColor;
			
	//��ҳ���
	COLORREF	LinkColor;
	COLORREF	ActiveLinkColor;
	COLORREF	VisitedLinkColor;
	COLORREF	WebSpecColor;

	COLORREF	OTHERCOLOR[11];		//������ɫ,����ָ���ʮ��ɶ��ı�ͼ
}SCHEME_COLOR;

typedef struct tag_OP_Scheme_Font
{ 
	char		ListFontName[40];
	short		ListFontSize;
	char		GraphFontName[40];
	short		GraphFontSize;
	char		ZxFontName[40];
	short		ZxFontSize;
}SCHEME_FONT;

typedef struct tag_OP_Scheme_Menu
{ 
	short	nMenuMode; 		//0:��ͳ��� 1:XP���
	short	bExtendXPStyle; //�Ƿ�����ǿ�͵�XP��� 
	long	Reserved;
}SCHEME_MENU;

struct HandleValue//HandleFloat
{
	DWORD	nValidFlag;			//
	DWORD	nHandleType;
	long	nValueLen;			//char
	char	*pValue;			//һ��Ĵ�
	long	nDataNum;			//����ѡ��:1.���㴮�ĸ���:(1-5,����nHandleType�õ�) 2(ѡ��).float��ĸ���:m_nDataNum
	float	*pFloat;			//���ڼ���Ĵ�
	char	**ppValue;			//��̬���ɵĴ�
	
	HandleValue()
	{
		nValidFlag = TMEANLESS_INT;
		nHandleType = 0;
		nValueLen = 0;
		pValue = NULL;
		nDataNum = 0;
		pFloat = NULL;
		ppValue = NULL;
	}
};
//////////////////////////////////////////////////////////////////////


#pragma pack(pop)

#endif
