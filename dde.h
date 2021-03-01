#ifndef DDE_H_
#define DDE_H_

//Ʒ�ֵĴ�����
#define TOTAL_TYPE			0	//�ۺ�����
#define ABGP_TYPE			1	//��½AB��
#define HKGP_TYPE			2	//��۹�Ʊ
#define QH_TYPE				3	//�ڻ�Ʒ��
#define FE_TYPE				4	//���Ʒ��
#define IDX_TYPE			5	//����ָ��
#define FOREGP_TYPE			6	//�����Ʊ
#define FY_TYPE				8	//��������
#define DDE_TYPE			9	//DDE����
#define A2H_TYPE			10	//A+H����
#define	JSZJ_TYPE			11	//�ʽ�
#define SIGNAL_TYPE			12	//��ţ���ź�
#define LAST_TYPE			15	//��Ϊ����

//��Ŀֵ
#define ABGP_COL_NUM	67		//����������Ŀ����
#define ABGPHQ_COL_NUM	30		//��������Ŀ����

#define HKGP_COL_NUM	24		//�۹�������Ŀ����
#define QH_COL_NUM		25		//�ڻ�������Ŀ����
#define FE_COL_NUM		12		//���������Ŀ����
#define FOREGP_COL_NUM	21		//���������Ŀ����
#define IDX_COL_NUM		12		//ָ��������Ŀ����
#define DDE_COL_NUM		17		//DDE��Ŀ����
#define A2H_COL_NUM		12		//A+H��Ŀ����
#define GLHQ_COL_NUM	18		//�����������Ŀ
#define	JSZJ_COL_NUM	10
#define	SIGNAL_COL_NUM	32		//�����ź�

#define TOTAL_COL		140		//����ϵͳ֧�ֵ���Ŀ��

//��Ʊ��Ŀ����
const short ZQDM	= 0;		//����
const short ZQJC	= 1;		//֤ȯ��
const short ZRSP	= 2;		//����
const short JRKP	= 3;		//��
const short ZGCJ	= 4;		//���
const short ZDCJ	= 5;		//���
const short ZJCJ	= 6;		//�ּ�
const short ZGJM	= 7;		//�����
const short ZDJM	= 8;		//������
const short CJL		= 9;		//����
const short CJJE	= 10;		//�ܽ��
const short XS		= 11;		//����
const short QRSD	= 12;		//���ǵ�
//const short QBSD	= 13;		//���ǵ�
const short HY_STR	= 13;		//������ҵ
const short ZAF		= 14;		//�Ƿ�
const short ZEF		= 15;		//���
const short JUNJ	= 16;		//����
const short SYL		= 17;		//��ӯ��
const short WTB		= 18;		//ί��
const short LP		= 19;		//����
const short WP		= 20;		//����
const short LWB		= 21;		//�����
const short WLC		= 22;		//ί����
const short BJL1	= 23;		//����һ
const short SJL1	= 24;		//����һ
const short BJ1		= 25;		//���һ
const short SJ1		= 26;		//����һ
const short BJ2		= 27;		//��۶�
const short BJL2	= 28;		//���ֶ�
const short SJ2		= 29;		//���۶�
const short SJL2	= 30;		//���ֶ�
const short BJ3		= 31;		//�����
const short BJL3	= 32;		//������
const short SJ3		= 33;		//������
const short SJL3	= 34;		//������
const short LIANGB	= 35;		//����
const short J_HSL	= 36;		//������
const short J_LTGB	= 37;		//��ͨ�ɱ�
const short J_LTSZ	= 38;		//��ͨ��ֵ
const short J_ZSZ	= 39;		//����ֵ

const short DKPH	= 40;		//���ƽ��
const short DTHL	= 41;		//��ͷ����
const short DTZS	= 42;		//��ͷֹ��
const short KTHB	= 43;		//��ͷ�ز�
const short KTZS	= 44;		//��ͷֹ��
const short QRD		= 45;		//ǿ����
const short ZANGSU  = 46;		//����
const short HYD		= 47;		//��·��
const short MBZL	= 48;		//ÿ�ʾ���
const short MBHSL	= 49;		//ÿ�ʻ���

const short J_GXRQ	= 50;		//��������
const short J_START = 51;		//��������
const short J_ZGB	= 52;		//�ܹɱ�
const short J_GJG   = 53;		//���ҹ�
const short J_OLDJLY= 54;		//������ھ�����
const short J_OLDZYSY=55;		//���������Ӫ����
const short J_BG	= 56;		//B��
const short J_HG	= 57;		//H��
const short J_ZGG	= 58;		//����ÿ������ (�Ʊ����ṩ��ÿ������,������Ĳ���)
const short J_ZZC	= 59;		//���ʲ�(��Ԫ)
const short J_LDZC	= 60;		//�����ʲ�
const short J_GDZC	= 61;		//�̶��ʲ�
const short J_WXZC	= 62;		//�����ʲ�
const short J_CQTZ	= 63;		//����Ͷ��
const short J_LDFZ	= 64;		//������ծ
const short J_CQFZ	= 65;		//���ڸ�ծ
const short J_ZBGJJ	= 66;		//�ʱ�������
const short J_JZC	= 67;		//�ɶ�Ȩ��(���Ǿ��ʲ�)
const short J_ZYSY	= 68;		//��Ӫ����
const short J_ZYLY	= 69;		//��Ӫ����
const short SJL		= 70;		//��ӯ��
const short J_YYLY	= 71;		//Ӫҵ����
const short J_TZSY	= 72;		//Ͷ������
const short J_JYXJL	= 73;		//��Ӫ�ֽ�����
const short J_ZXJL	= 74;		//���ֽ�����
const short J_CH	= 75;		//���
const short J_LYZE	= 76;		//�����ܶ�
const short J_SHLY	= 77;		//˰������
const short J_JLY	= 78;		//������
const short J_WFPLY	= 79;		//δ��������
const short J_TZMGJZ= 80;		//����ÿ�ɾ��ʲ�
const short J_JYL	= 81;		//���ʲ�������
const short J_MGWFP	= 82;		//ÿ��δ����
const short J_MGSY	= 83;		//ÿ������
const short J_MGGJJ	= 84;		//ÿ�ɹ�����
const short J_MGJZC	= 85;		//ÿ�ɾ��ʲ�
const short J_GDQYB	= 86;		//�ɶ�Ȩ���

const short ZBCOL   = 87; 		//ָ��������Ŀ

const short	SPELL_CODE	= 88;	//�����
const short QH_JSJ		= 89;	//�ڻ������
const short QH_YJSJ		= 90;	//�ڻ�ǰ�����

const short SPREAD		= 91;	//�������
const short BSUNIT		= 92;	//������λ
const short CURRENCYNAME= 93;	//���ҵ�λ
const short AVERPRICE	= 94;	//ƽ����
const short YIELDVAL	= 95;	//������
const short HIS_HIGH	= 96;	//�����
const short HIS_LOW		= 97;	//�����
const short IEP			= 98;	//�ο���
const short IEV			= 99;	//�ο���
const short MRKCAP		= 100;	//��ֵ
const short PE			= 101;	//��ӯ��
const short PREMINUM	= 102;	//���%
const short GEARING		= 103;	//���˱���%
const short EXECPRICE	= 104;	//��ʹ��
const short CONVRATIO	= 105;	//��������
const short EXPIREDATE	= 106;	//������
const short TODAYZC		= 107;	//�ֲֲ�
const short TODAYKC		= 108;	//���տ���
const short TODAYPC		= 109;	//����ƽ��

const short DDX_1		= 110;	//����DDX
const short DDY_1		= 111;	//����DDY
const short DDZ_1		= 112;	//����DDZ
const short DDX_60		= 113;	//60��DDX
const short DDY_60		= 114;	//60��DDY
const short DDX_REDNUM	= 115;	//DDX��������
const short DDX_REDIN10	= 116;	//10��DDX��

const short BIGIOAMOUNT	= 117;	//�����ʽ�����
const short BIGIOWEIGHT = 118;	//������Ծ��

const short BIGBUY_RAT	= 119;	//������%
const short BIGSELL_RAT	= 120;	//������%
const short TOTALWTB	= 121;	//����ί��
const short TICK2_JL	= 122;	//��ʾ���

const short H_ZQDM		= 123;	//H�ɴ���
const short H_ZQJC		= 124;	//H������
const short H_ZJCJ		= 125;	//H���ּ�
const short H_ZAF		= 126;	//H���Ƿ�
const short H_CJL		= 127;	//H������
const short H_GB		= 128;	//H�ɹɱ�
const short H_HSL		= 129;	//H�ɻ���
const short H_YJ		= 130;	//���
const short H_YJL		= 131;	//�����

const short JS_INDD		= 132;	//������
const short JS_OUTDD	= 133;	//������
const short JS_INZD		= 134;	//�е�����
const short JS_OUTZD	= 135;	//�е�����
const short JS_INXD		= 136;	//С������
const short JS_OUTXD	= 137;	//С������

const short ON_DXTX		= 138;	//��������
const short ON_ZXTX		= 139;	//��������

/////////////////////////////////////////////////////

extern char		g_strBigMarket[16][11];

extern char		g_lpszAllItemName[TOTAL_COL][18];
extern float *	g_aAllItemWidth;
extern float	g_aAllItemWidth_Def[TOTAL_COL];

extern long		g_aABGPIdx[ABGP_COL_NUM];
extern long		g_aABGPIdx_Def[ABGP_COL_NUM];

extern long		g_aHKGPIdx[HKGP_COL_NUM];
extern long		g_aHKGPIdx_Def[HKGP_COL_NUM];

extern long		g_aFOREGPIdx[FOREGP_COL_NUM];
extern long		g_aFOREGPIdx_Def[FOREGP_COL_NUM];

extern long		g_aFEIdx[FE_COL_NUM];
extern long		g_aFEIdx_Def[FE_COL_NUM];

extern long		g_aQHIdx[QH_COL_NUM];
extern long		g_aQHIdx_Def[QH_COL_NUM];

extern long		g_aIDXIdx[IDX_COL_NUM];
extern long		g_aIDXIdx_Def[IDX_COL_NUM];

extern long		g_aDDEIdx[DDE_COL_NUM];
extern long		g_aDDEIdx_Def[DDE_COL_NUM];

extern long		g_aA2HIdx[A2H_COL_NUM];
extern long		g_aA2HIdx_Def[A2H_COL_NUM];

extern long		g_aJSZJIdx[JSZJ_COL_NUM];
extern long		g_aJSZJIdx_Def[JSZJ_COL_NUM];

extern long		g_aSignalIdx[SIGNAL_COL_NUM];
extern long		g_aSignalIdx_Def[SIGNAL_COL_NUM];

extern long		g_aGlHqIdx_Def[GLHQ_COL_NUM];

#endif
