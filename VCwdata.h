#ifndef VCWDATA_H_
#define VCWDATA_H_

#ifdef	TCWDATA_EXPORT
	#define TCWDATADLL_API extern "C" __declspec(dllexport)
#else
	#define TCWDATADLL_API extern "C" __declspec(dllimport)
#endif

#pragma pack(1)

#define CW_NAME_LEN			60
#define CW_COMMAND_LEN		8*1024

#define CW_SECURITY_NUM		8000
#define CW_CONST_NUM		2000
#define CW_CONSTVALUE_NUM	3000	
#define CW_TABLE_NUM		2000
#define CW_FIELD_NUM		200
#define CW_SIGCOND_NUM		10
#define CW_FIELD_MAXLEN		500

#define C_CW_CATEGORY		1177
#define C_CW_MARKET			1491
#define C_CW_LISTEDSECTOR	1495
#define C_CW_LISTEDSTATE	1176
#define C_CW_TJJHY			1601
#define C_CW_ZJHHY			1602
#define C_CW_TDXHY			1603

enum C_DataType
{
	C_Short = 3,			//len=2
	C_Int = 4,				//len=4 
	C_LongLong = 5,			//len=8 
	C_Char = 6,				//len=1
	C_Float = 7,			//len=4
	C_Double = 8,			//len=8
	C_Date = 9,				//len=4 (YYYYMMDD)int
	C_DateTime = 10,		//len=8 ((YYYYMMDD)int (HHMMSS)int)
	C_Text = 11,			//�ı���Ϣ
	C_EnumFirst = 1000,		//ö��
	C_EnumEnd = 79999,		
	C_DecimalFirst = 80000,	//
	C_DecimalEnd = 89999,
	C_StrFirst = 90000,		//9****,len=****
	C_StrEnd = 98000,		
};

//�ڴ�֤ȯ����ṹ
typedef struct tagMemSecurityInfo
{
	long	m_nInnerCode;		//֤ȯ�ڲ�����
	long	m_nComCode;			//��˾����	
	char	m_szSymbol[10+1];	//֤ȯ����
	char	m_szSecuAbbr[20+1];	//֤ȯ���
	long	m_nSecuMarket;		//֤ȯ�г�
	long	m_nSecuCategory;	//֤ȯ���
	long	m_nListedDate;		//��������
	long	m_nListedSector;	//���а��
	long	m_nListedState;		//����״̬
	long	m_nTJJHY;			//ͳ�ƾ���ҵ
	long	m_nZJHHY;			//֤�����ҵ
	long	m_nTDXHY;			//ͨ������ҵ
}MemSecurityInfo;

//�ڴ泣���ṹ
typedef struct tagMemConstInfo
{
	long	m_lCID;						//�ӳ���ID
	char	m_szCName[CW_NAME_LEN+1];	//�ӳ�������
}MemConstInfo;

//�ڴ��ṹ
typedef struct tagMemTableInfo
{
	long	lTableId;	
	char	szTableExp[CW_NAME_LEN+1];	// ����(����)
}MemTableInfo;

//�ڴ��ֶνṹ
typedef struct tagMemFieldInfo
{
	long	nId;					// �ֶ�ID(���ֶ�����ΪT???)
	long	nType;					// �ֶ�����
	char	szName[CW_NAME_LEN+1];	// �ֶ�����
	long	nPos;					// �ֶμ�¼�е�λ��
	long	nWid;					// �ֶο��
}MemFieldInfo;

//���ݻص������ķ�ʽ
typedef long(CALLBACK * PDATAIOFUNC)(char * Code,short nSetCode,short DataType,void * pData/*�ڲ�����*/,short nDataNum,NTime,NTime,BYTE nTQ,unsigned long);
//ʵ�ֹ����л��Ļص�����
typedef long(CALLBACK * PSWITCHINFOFUNC)(char * Code,short nSetCode,char *buf,long buflen,short FuncType,short Option,unsigned long);


#pragma pack()
#endif
