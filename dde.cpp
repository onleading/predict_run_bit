#include "stdafx.h"
#include "dde.h"

//�����͵�����
char  g_strBigMarket[16][11]={"�ۺ�","����","���","�ڻ�","���","ָ��","���","����","����","DDE","","","","","","����"};

//������Ŀ������
char  g_lpszAllItemName[TOTAL_COL][18] =
{
	 "����   ", "����", "ǰ��", "��", "���",
	 "���","����", "ί���", "ί����", "����", 
	 "�ܶ�", "����","�ǵ�", "��ҵ����", "�Ƿ�",
	 "���", "����", "��ӯ��","ί��", "����", 
	 "����", "�����", "ί��","ί����", "ί����", 
	 "���һ", "����һ","��۶�", "������", "���۶�",
	 "������","�����", "������", "������", "������",
	 "����",  "������","��ͨ�ɱ�","��ͨ��ֵ", "AB������ֵ",

	 "���ƽ��","��ͷ����","��ͷֹ��","��ͷ�ز�","��ͷֹ��","ǿ����%%","����","��Ծ��","ÿ�ʾ���","ÿ�ʻ���",

	 "��������","��������","�ܹɱ�(��)","���ҹ�","����ͬ��","����ͬ��",
	 "B��(��A)","H��","ְ����","���ʲ�(��)","�����ʲ�",
	 "�̶��ʲ�","�����ʲ�","����Ͷ��","������ծ","���ڸ�ծ",
	 "������","���ʲ�","��Ӫ����","��Ӫ����","�о���",
	 "Ӫҵ����","Ͷ������","��Ӫ�ֽ���","���ֽ���","���",
	 "�����ܶ�","˰������","������","δ������","��������",
 
	 "������%%","ÿ��δ����","ÿ������","ÿ�ɹ���","ÿ�ɾ���","Ȩ���%%",

	 ""	,
	 "�����",
	 _Settle,_Prev_Settle,_Spread,_Lot_Size,_Currency,
	 _Aver_Price,_Yield,_Year_High,_Year_Low,"�ο���",
	 "�ο���",_MrkCap,_PE_Ratio,_PreMinum,_Gearing,
	 _ExecPrice,_ConvRatio,_Expire_Date,_Contract_Diff,_OpenContract,_CloseContract,
	 "����DDX","����DDY","����DDZ","60��DDX","60��DDY","DDX����","10�պ�DDX","SUP������","������Ծ%%","������%%","������%%","����ί��","��ʾ���",
	 "H�ɴ���","H������","H�ּ�","H�Ƿ�%%","H����","H�ɱ�","H����%%","���(��)","�����%%",
	 "������","������","�е�����","�е�����","С������","С������",
	 "��������","��������",
};

//������Ŀ�Ŀ��
float *g_aAllItemWidth=NULL;
float g_aAllItemWidth_Def[TOTAL_COL]={6,8,8,8,8,8,8,7,7,7,
						9,6,8,15,7,7,8,8,7,7,
						9,7,7,7,7,8,8,7,7,7,
						7,7,7,7,7,6,7,11,10,11,

						9,9,9,9,9,9,7,7,9,9,

						9,9,11,10,9,9,9,10,9,12,12,
						12,12,12,12,12,12,12,12,12,9,
						11,11,11,11,11,10,10,10,10,11,

						8,11,9,9,9,8,
						12,
						8,
						8,9,8,8,8,
						8,8,8,8,8,
						9,9,8,9,9,
						8,9,11,8,8,8,
						9,9,9,9,9,9,10,11,10,10,10,9,9,
						8,17,9,9,9,8,9,8,9,
						9,9,9,9,9,9,
						9,9};

//AB����Ŀ
long g_aABGPIdx[ABGP_COL_NUM];

long g_aABGPIdx_Def[ABGP_COL_NUM]={ZQDM,ZQJC,ZAF,LIANGB,J_HSL,
					ZRSP,JRKP,ZGCJ,ZDCJ,ZJCJ,
					CJL,CJJE,XS,ZANGSU,QRSD,
					ZEF,JUNJ,SYL,J_LTGB,J_LTSZ,HY_STR,
					WP,LP,LWB,ZGJM,BJL1,
					ZDJM,SJL1,WTB,WLC,

					J_GXRQ,J_START,J_ZGB,J_GJG,J_OLDJLY,J_OLDZYSY,
					J_BG,J_HG,J_ZGG,J_ZZC,J_LDZC,
					J_GDZC,J_WXZC,J_CQTZ,J_LDFZ,J_CQFZ,
					J_ZBGJJ,J_MGGJJ,J_JZC,J_ZYSY,J_ZYLY,SJL,
					J_YYLY,J_TZSY,J_JYXJL,J_ZXJL,J_CH,
					J_LYZE,J_SHLY,J_JLY,J_WFPLY,J_MGJZC,J_TZMGJZ,
					J_JYL,
					J_MGWFP,J_MGSY,J_GDQYB};


//�۹���Ŀ
long  g_aHKGPIdx[HKGP_COL_NUM];

long  g_aHKGPIdx_Def[HKGP_COL_NUM]={ZQDM,ZQJC,ZAF,ZANGSU,ZJCJ,BJ1,
					SJ1,XS,LIANGB,CJL ,JRKP,ZGCJ,
					ZDCJ,CJJE,ZEF,QRSD,ZRSP,
					BSUNIT,HIS_HIGH,HIS_LOW,
					IEP,IEV,MRKCAP,PE};

//�����Ŀ
long  g_aFOREGPIdx[FOREGP_COL_NUM];

long  g_aFOREGPIdx_Def[FOREGP_COL_NUM]={ZQDM,ZQJC,ZAF,ZJCJ,BJ1,
					SJ1,XS ,CJL ,JRKP,ZGCJ,
					ZDCJ,CJJE,ZEF,QRSD,ZRSP,
					JUNJ,WTB,LP,WP,
					LWB ,WLC};

//�����Ŀ
long  g_aFEIdx[FE_COL_NUM];

long  g_aFEIdx_Def[FE_COL_NUM]={ZQDM,ZQJC,ZJCJ,
					QRSD,ZAF,ZEF,ZGCJ,
					ZDCJ,JRKP,ZRSP,BJ1,SJ1};

//�ڻ���Ŀ
long  g_aQHIdx[QH_COL_NUM];

long  g_aQHIdx_Def[QH_COL_NUM]={ZQDM,ZQJC,ZAF,ZANGSU,ZJCJ,BJ1,SJ1,XS,LIANGB,WTB,BJL1,SJL1,
					CJL,QRSD,SYL,TODAYZC,QH_JSJ,JRKP,ZGCJ,ZDCJ,QH_YJSJ,
					TODAYKC,TODAYPC,LP,WP};

//ָ����Ŀ
long  g_aIDXIdx[IDX_COL_NUM];

long  g_aIDXIdx_Def[IDX_COL_NUM]={ZQDM,ZQJC,ZAF,ZJCJ,ZANGSU,
					QRSD,ZEF,ZGCJ,CJJE,
					ZDCJ,JRKP,ZRSP};

//DDE��Ŀ
long  g_aDDEIdx[DDE_COL_NUM];
long  g_aDDEIdx_Def[DDE_COL_NUM]={ZQDM,ZQJC,ZJCJ,ZAF,
								DDX_1,DDY_1,DDZ_1,DDX_60,DDY_60,DDX_REDNUM,DDX_REDIN10,BIGIOAMOUNT,BIGIOWEIGHT,
								BIGBUY_RAT,BIGSELL_RAT,TOTALWTB,TICK2_JL};

//A2H��Ŀ
long  g_aA2HIdx[A2H_COL_NUM];
long  g_aA2HIdx_Def[A2H_COL_NUM]={ZQDM, ZQJC, ZJCJ, ZAF, J_HSL, 
								H_ZQDM, H_ZQJC, H_ZJCJ, H_ZAF, H_HSL, 
								H_YJ, H_YJL};

//�����������Ŀ
long g_aGlHqIdx_Def[GLHQ_COL_NUM]={ZQJC,ZAF,ZJCJ,ZANGSU,LIANGB,
								QRSD,ZGJM,ZDJM,CJL,J_HSL,
								XS,JRKP,ZRSP,ZGCJ,ZDCJ,
								CJJE,ZEF,J_LTSZ};

//ZJ��Ŀ
long  g_aJSZJIdx[JSZJ_COL_NUM];
long  g_aJSZJIdx_Def[JSZJ_COL_NUM]={ZQDM,ZQJC,ZJCJ,ZAF,JS_INDD,JS_OUTDD,JS_INZD,JS_OUTZD,JS_INXD,JS_OUTXD};

//�ź���Ŀ
long g_aSignalIdx[SIGNAL_COL_NUM];

long g_aSignalIdx_Def[SIGNAL_COL_NUM]={ZQDM,ZQJC,ZAF,ON_DXTX,ON_ZXTX,LIANGB,J_HSL,
					ZRSP,JRKP,ZGCJ,ZDCJ,ZJCJ,
					CJL,CJJE,XS,ZANGSU,QRSD,
					ZEF,JUNJ,SYL,J_LTGB,J_LTSZ,HY_STR,
					WP,LP,LWB,ZGJM,BJL1,
					ZDJM,SJL1,WTB,WLC};
