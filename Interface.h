#ifndef _DTOPVIEWCALC_H
#define _DTOPVIEWCALC_H

#ifdef	CALCDLL_EXPORT
	#define CALCDLL	_declspec(dllexport)
#else
	#define CALCDLL	_declspec(dllimport)
#endif

#define MAX_LINEOUT		50
#define MAX_PARAMNUM	16
#define MAX_PRDNUM		12

#define VAR_LEN			16
#define ZBCODE_LEN		14
#define ZBNAME_LEN		50
#define PASSWORD_LEN	20

#include "OutStruct.h"

#pragma pack(push,1)

#define OUT_RESERVE	2

#define		DRAW_KLINE		1
#define		DRAW_STICKLINE  2
#define		DRAW_ICON		3
#define		DRAW_TEXT		4
#define		DRAW_BAND		5

//�������������Ϣ��
typedef struct tagTYPEX
{
	int		nTypeID;						//���ID
	int		nRootType;						//�����
	int		nFatherType;					//������
	char	lpszName[32];					//���������
	BOOL 	bEnable;						//FALSE����ʾ,TRUE��ʾ,����߼̳���,RESERVED
}TYPEX, *LPTYPEX;

//Index�ṹ�Ľ���
typedef struct tag_ParaInfo   //������Ϣ�Ľṹ����
{
	char		acParaName[VAR_LEN];//����������
	float		nMin;				//������Сȡֵ��Χ
	float		nMax;				//�������ȡֵ��Χ
	float		nStep;				//�������Բ���
	float		nDefault;			//ϵͳ�Ƽ���ȱʡֵ
	float		nValue[MAX_PRDNUM]; //�û������ֵ,enum Period
}TPARAINFO;

enum Period  //12�����ڶ�Ӧ���±�SubScript
{
	MIN1_ST		,
	MIN5_ST		,
	MIN15_ST	,
	MIN30_ST	,
	HOUR_ST		,
	DAY_ST		,
	WEEK_ST		,
	MONTH_ST	,
	MINN_ST		,
	DAYN_ST		,
	SEASON_ST	,
	YEAR_ST		,
};

typedef struct tag_LineInfo   //���߷�����Ϣ�Ľṹ����
{
	char		acLineName[VAR_LEN];   	//����ߵ����ƣ�ȱʡ�ù�ʽǰ������
	int 		nType;					//������ߵĻ��Ʒ���(<10Ϊ����,>10Ϊϵͳ������ɫ)
	DWORD		nColor;					//�Զ���ɫ(-1��ȡnType)
	int			nWidth;					//�ߵĿ��
}TLINEINFO;

typedef struct tag_INDEXINFO
{  
	short		nIndexNo;			//�����ƫ��
	BYTE	 	nSet;				//�����ĸ����� 0 ָ�� 1 ���� 2 ר�� 3 K�� 4 �Զ��庯��(�������һ��.)
	char		acCode[ZBCODE_LEN]; //ָ��Ӣ�ļ��
	char		acName[ZBNAME_LEN]; //ָ����������
	BYTE		nType;       		//ָ������
	DWORD		nDrawMode;			//���߷�ʽ enum DRAWMODE
	BYTE		nParaNum;    		//��������
	TPARAINFO	aPara[MAX_PARAMNUM];//������Ϣ,6����
	BYTE		nLineNum;			//����߸���
	TLINEINFO	aLine[MAX_LINEOUT];	//��������ߵ�����
	BYTE		nAxisType;   		//Y�����������,0Ϊ�Զ�������ʹ��fAxisValue
	BYTE		nAddXAxisNum;		//��Ҫ�������ӵ�Y����Ҫ�ֽ��߸���
	float		afAddXAxis[4];		//�������ӵ�Y����Ҫ�ֽ��ߵķֽ���ֵ
	float		fAxisValue[7];		//�Զ����Y����ֵ
	char		lpszPassword[PASSWORD_LEN];	//��������,Ϊ�ձ�ʾ�������
	char    *	lpszDefTxt;      	//����ָ������㹫ʽ��Ϣ(��������ΪNULL)
	char    *	lpszParamGuy;		//������������
	char    *	lpszCompliedInfo; 	//���������Ժ��ָ�����㹫ʽ��Ϣ
	char    *	lpszHelp;			//ָ��˵��
	DWORD		nDelStatus;         //enum  STATUS
}TINDEXINFO ;

////ָ�����
enum LINETYPE
{
	DEFAULT		= 0,
	VOLSTICK	= 1,	//�ɽ�����
	COLORSTICK	= 2,	//ɭ����
	CIRCLEDOT	= 3,	//Ȧ��ͼ 
	STICK       = 4,
	LINESTICK	= 5,
	ZBCIRCLEDOT = 6,
	ZBCROSSDOT  = 7,
	POINTDOT	= 8,
	NODRAW		= 255,	//��������
};

enum STATUS
{
	SYSTEM_INDEX=  0x00000001,
	USER_INDEX	=  0x00000002,	//�˴������©��
	SECRET_INDEX=  0x00000004,
	BIN_INDEX	=  0x00000008,
	TMP_INDEX	=  0x00000010,
	DEL_INDEX	=  0x00000020,
	ERR_INDEX	=  0x00000040,
	HIDE_INDEX	=  0x00000080,
	UNSTABLE_INDEX= 0x00000100,
	EXPIRABLE_INDEX=0x00000200,
};

//���ʱ�����������Ӧ��ѭ��
enum SYSTEM
{
	ENTERLONG	= 0,		//{��ͷ����}		
	EXITLONG    = 1,		//{��ͷ����} 
	ENTERSHORT  = 2,		//{��ͷ����} 
	EXITSHORT	= 3,		//{��ͷ����} 
};

//�����ĸ�����
enum INDEX_SET
{
	ZB_TYPE = 0,
	TJ_TYPE = 1,
	EXP_TYPE= 2,
	KL_TYPE = 3,
	FUNC_TYPEX = 4,
	UF_TYPE = 5,
	SEPCLINE_TYPE = 6,
};

// ָ������ BYTE
enum INDEX_LX
{
	/*
	INDEX_TREND    ,  //����ָ��*
	INDEX_UNTREND  ,  //������ָ��*
	INDEX_ENERGE   ,  //����ָ��*
	INDEX_VOL      ,  //�ɽ���ָ��*
	INDEX_VOLPRICE ,  //����ָ��
	INDEX_PATH     ,  //ѹ��֧��ָ��*
	INDEX_ALL      ,  //����ָ��*
	INDEX_OBOS     ,  //������ָ��*
	INDEX_SWAY	   ,  //�ڶ�ָ��
	INDX_STAT	   ,  //ͳ��ָ��
	INDEX_INOUT	   ,  //��������Ա�
	INDEX_POWER	   ,  //ǿ��ָ��
	INDEX_OTHER	   ,  //����

	INDEX_MAV,
	INDEX_STYLE,
	INDEX_STOP,
	INDEX_TRADE,
	*/
	INDEX_ZFJS	   ,  //��ɫָ��

	INDEX_ALL      ,  //������*
	INDEX_OBOS     ,  //��������*
	INDEX_TREND    ,  //������*
	INDEX_ENERG    ,  //������*
	INDEX_VOL      ,  //�ɽ�����*
	INDEX_MAV      ,  //������*
	INDEX_STYLE    ,  //ͼ����*
	INDEX_PATH     ,  //·����*
	INDEX_STOP     ,  //ͣ����*
	INDEX_OTHER	   ,  //����

	INDEX_ADDBASE = 100,//�û���ӵ�,��INDEX_ADDBASE֮��
};

enum	TJXG_LX
{
	TJ_ZFJS,

	TJ_ZB,
	TJ_JBM,
	TJ_JSPZ,
	TJ_ZSTZ,
	TJ_XTTZ,
	TJ_OTHER,
	TJ_ADDBASE = 100,//�û���ӵ�,��TJ_ADDBASE֮��
};

// ���߷�ʽ BYTE
enum DRAWMODE
{
	//��ͼ����,������ӵ���ͼ���仯����;��ɾ��,���ɶ�������
	//��TAB��ʽ,����ͼ���л�.
	OnlyMain		=	0x00000001,
	//������ͼ,����ٵ��ӵ���ͼ,��������.������һ���ڸ�ͼ.
	//������ӵ���K�ߵ�,��������е���.
	MainAble		=   0x00000002,			
	//����ʱ,��ԭָ��Ϊ��,�ұ����OnlyMain.
	//����е���,�Ͳ����϶�.  
	//�仯����
	KOverlay		=	0x00000004,	//K��	
	UOverlay		=   0x00000008,	//������
	COverlay		=   0x00000010,	//����վ��
	/////////////////////////
	PUCULINE		=	0x00000020,
	SARLINE			=	0x00000040,
	BASEZERO		=   0x00000080,
};

//���K�� 16ɫ
#define COLORRED                    0 
#define COLORBLUE                   1  
#define COLORGREEN                  2  
#define COLORCYAN                   3 
#define COLORBLACK                  4 
#define COLORMAGENTA                5  
#define COLORBROWN                  6  
#define COLORLIGHTGRAY              7  
#define COLORGRAY                   8  
#define COLORLIGHTBLUE              9  
#define COLORLIGHTGREEN				10  
#define COLORLIGHTCYAN				11  
#define COLORLIGHTRED				12  
#define COLORLIGHTMAGENTA			13  
#define COLORYELLOW					14  
#define COLORWHITE					15  
#define DEFAULTKCOLOR				16

#define	MAXKCOLOR					17

//Calc��Ĵ����
#define CALC_OK			0
#define CALC_SKIP		1
#define CALC_ERROR		-1
#define ASK_ALL			-1

//////////////////////////////////////////////////////////////////////////
/*
Init			_CallDTopViewCalc_0
SetSysLimited	_CallDTopViewCalc_1
RegisterDataIO	_CallDTopViewCalc_2
InputData		_CallDTopViewCalc_3
InputDataInfo	_CallDTopViewCalc_4
InputDataInfo	_CallDTopViewCalc_4
Calc			_CallDTopViewCalc_5
Calc			_CallDTopViewCalc_5
GetTreeInfo		_CallDTopViewCalc_6
GetIndexNum		_CallDTopViewCalc_7
GetIndexInfo	_CallDTopViewCalc_8
GetIndexInfo	_CallDTopViewCalc_8
GetIndexNo		_CallDTopViewCalc_9
GetTypeNum		_CallDTopViewCalc_10
GetTypeName		_CallDTopViewCalc_11
GetTypeInfo		_CallDTopViewCalc_12
Allocfloat		_CallDTopViewCalc_13
Freefloat		_CallDTopViewCalc_14
GetData			_CallDTopViewCalc_15
PopupDlg		_CallDTopViewCalc_16
PopupDlg		_CallDTopViewCalc_16
*/
//////////////////////////////////////////////////////////////////////////

class	AutoCalc;
class  CALCDLL DTopViewCalc
{
public:
	DTopViewCalc();
	virtual ~DTopViewCalc();
public:
	//
	TINDEXINFO	*   operator[](BYTE nSet) const;
	//
	BOOL			_CallDTopViewCalc_0(char *PubDir=".\\",char *PriDir=".\\",char *IniFile=".\\Tdxw.ini",char *RegStr="");
	void			_CallDTopViewCalc_1(BOOL bLimitExport,BOOL bHasCW,long lOpenRq,long Reserved,char *OtherTypeName,char *OtherXGName);
	short			_CallDTopViewCalc_2(PDATAIOFUNC pfn);
	long			_CallDTopViewCalc_3(char * Code,short nSetCode,short DataType,void * pData,short nDataNum,BYTE nTQ=0,unsigned long unused = 0);
	//nDataNum = ASK_ALL ��ʾȫ��
	long			_CallDTopViewCalc_4(char * Code,short nSetCode,short DataType,short nDataNum = ASK_ALL,BYTE nTQ=0,unsigned long unused = 0);
	long			_CallDTopViewCalc_4(char * Code,short nSetCode,short DataType,NTime time1=NOWTIME,NTime time2=NOWTIME,BYTE nTQ=0,unsigned long unused = 0);
	//���ֻ��MAX_LINEOUT �����,��������ΪNULL
	//����ɹ�������CALC_OK�����򷵻�CALC_ERROR!
	short			_CallDTopViewCalc_5(BYTE nSet,long nIndexNo,float * fPut[MAX_LINEOUT]);
	short			_CallDTopViewCalc_5(TINDEXINFO * One,float * fPut[MAX_LINEOUT]);
	short			_CallDTopViewCalc_5(char *strFormula, short nSetCode, char *Code, float * fPutRes[MAX_LINEOUT]);
	long			_CallDTopViewCalc_6(void *ackData, int nAckType, int nSubType=-1);
	long			_CallDTopViewCalc_7(BYTE nSet)	const;					//�õ�����
	//���ʧ�ܣ�����NULL
	TINDEXINFO	*	_CallDTopViewCalc_8(BYTE nSet,long nIndexNo)	const;	//����ʱ��ֱ��ָ��λ��
	TINDEXINFO	*   _CallDTopViewCalc_8(BYTE nSet,char * acCode)	const;			//���Ƽ���,����Ϊ�ղ�������ͼ
	////////////////////
	long			_CallDTopViewCalc_9(BYTE nSet,char *acCode)    const;
	///////////////////
	//���㿪ʼ
	long			_CallDTopViewCalc_10(int nSet) const;
	char		*   _CallDTopViewCalc_11(int nSet, long No)   const;
	BOOL			_CallDTopViewCalc_12(LPTYPEX pTypeInfo, int nSet, long No);
	////���ߺ���
	float		*   _CallDTopViewCalc_13(short nLen);					//һ������һ��������ΪnLen
	void		    _CallDTopViewCalc_14(float * p);					//�ͷ�һ��
	//////��������ָ��
	void		*   _CallDTopViewCalc_15()	const	{	return m_pData;		};
	//�޸ĳɹ����
	BOOL			_CallDTopViewCalc_16(short Subscript = DAY_ST);
	BOOL			_CallDTopViewCalc_16(BYTE nSet,long nIndexNo,short Subscript = DAY_ST);
private:
	BOOL			SaveAll();
	char *			GetLastError();
//	char		*   GetIndexCode(BYTE nSet,long nIndexNo)	const;	
//	BOOL			CopyIndexInfo(TINDEXINFO * pDes,TINDEXINFO * pSrc);

	void			Reset(BYTE nSet=255);
//	BOOL			LoadIndex();
//	BOOL			SaveIndex();
private:
	static          NTime   NOWTIME;
	static			long	m_ObjCalcer;				//ÿ����һ������+1������һ��-1����������ĸ������
	static			BOOL	m_bInited;
	AutoCalc		*m_pAutoCalc;
	void		*   m_pData;
	short			m_nDataType,m_nDataNum,m_nSetCode;
	NTime			m_time[2];
	char			m_strErrorInfo[256],m_Code[8];
};


#pragma pack(pop)
#endif
