#ifndef CWCL_H_
#define CWCL_H_

#include "blowfish.h"
#include "dbf.h"

//B01:����
//B02:��ɼ�
//B03:�͹�
//B04:���
#define CWCL_TYPE_DR			1		//��Ȩ��Ϣ
//
//B01:����ǰ��ͨ�ɱ�����ɣ�
//B02:����ǰ�ܹɱ�����ɣ�
//B03:���к���ͨ�ɱ�����ɣ�
//B04:���к��ܹɱ�����ɣ�
#define CWCL_TYPE_SEND2OUT		2		//�͹�����(�ɱ��䶯)
//
//B01:����ǰ��ͨ�ɱ�����ɣ�
//B02:����ǰ�ܹɱ�����ɣ�
//B03:���к���ͨ�ɱ�����ɣ�
//B04:���к��ܹɱ�����ɣ�
#define CWCL_TYPE_IN2OUT		3		//�ڲ�������(�ɱ��䶯)
//
//B01:����ǰ��ͨ�ɱ�����ɣ�
//B02:����ǰ�ܹɱ�����ɣ�
//B03:���к���ͨ�ɱ�����ɣ�
//B04:���к��ܹɱ�����ɣ�
#define CWCL_TYPE_OTHERCAPCHG	5		//�����ɱ��仯(�ɱ��䶯)
//
//B02:�¹ɼ�
//B03:�¹�
#define CWCL_TYPE_NEW			6		//�����¹�
//
//B01:����ǰ��ͨ�ɱ�����ɣ�
//B02:����ǰ�ܹɱ�����ɣ�
//B03:���к���ͨ�ɱ�����ɣ�
//B04:���к��ܹɱ�����ɣ�
#define CWCL_TYPE_BUYBACK		7		//�ع�(�ɱ��䶯)
//
//B01:����ǰ��ͨ�ɱ�����ɣ�
//B02:����ǰ�ܹɱ�����ɣ�
//B03:���к���ͨ�ɱ�����ɣ�
//B04:���к��ܹɱ�����ɣ�
#define CWCL_TYPE_NEW2OUT		8		//�¹�����(�ɱ��䶯)
//
//B01:����ǰ��ͨ�ɱ�����ɣ�
//B02:����ǰ�ܹɱ�����ɣ�
//B03:���к���ͨ�ɱ�����ɣ�
//B04:���к��ܹɱ�����ɣ�
#define CWCL_TYPE_RESERVES2OUT	9		//ת���ɱ�����(�ɱ��䶯)
//
//B01:����ǰ��ͨ�ɱ�����ɣ�
//B02:����ǰ�ܹɱ�����ɣ�
//B03:���к���ͨ�ɱ�����ɣ�
//B04:���к��ܹɱ�����ɣ�
#define CWCL_TYPE_CONVERT2OUT	10		//��תծ����(�ɱ��䶯)
//
//B03:����
#define CWCL_TYPE_INPROCHG		11		//��ͨ������

typedef struct
{
	char	Setcode;	//�°����ݲ��д��ֶ�
	char 	Code[7];
	long    Date;
	char    Type;
	float   B01;
	float   B02;
	float   B03;
	float   B04;
}CWDATA,*LPCWDATA;

typedef struct //relation.dat
{
	short   SetCode;    //�г�
	char    Code[11];   //����
	char    Name[19];   //����
	char	Typestr[11];//���ʹ�;
	DWORD   dwUnUsed;   //UnUsed;
}STKRELATION;

////profile.dat
typedef struct 
{
	char SetCode;
	char Code[7];
	char Name[9];
	long Date;      //������ǰ
	char Reason[43];//ԭ�� ��43��Ϊ���������ṹ��СΪ 64
}TDXSTKOLDNAME;

class Cwcl
{
public:
	Cwcl();
	~Cwcl();
	void	ReadARecord(short RecordNum,CWDATA *CWdata_obj);
	void	Set_Password(BYTE * Password,short len);
	void	CWData_Encrypt(CWDATA *CWdata_obj,short len);
	void	CWdata_Decrypt(CWDATA *CWdata_obj,short len);
	void	ReadLTGB_ZGB(struct MemStkInfo * scode,long * pDate, float * fpLTGB,float * fpZGB,short ItemNum,float fDefaultLTGB,float fDefaultZGB);
	void	ReadLTGB_ZGB_OneDate(struct MemStkInfo * scode,long Date,float * LTGB,float * ZGB,float fDefaultLTGB,float fDefaultZGB);
	void	GetGBBQ_Type(struct MemStkInfo * scode,struct AnalyData * AnalyDatap, char * fpBQtype,short ItemNum);
	void	TQData(struct MemStkInfo * scode,struct AnalyData * AnalyDatap,short nFrom,short nEnd,short nDataNum,short nTQFlag,short nPeriod);
	void	MinTQData(struct MemStkInfo * scode,struct AnalyData * AnalyDatap,short nFrom,short nEnd,short nDataNum,short nTQFlag,short nYearFlag=0);
	short	GetTQInfo(struct MemStkInfo * scode,CWDATA *pCWdata,short nMaxNum,BOOL bOnlyQk=FALSE);
	short	GetRecentGbbqInfo(CWDATA *pOutData,short nMaxNum);

	short	Init_ZGB_LTGB();
	CWDATA * GetGpCwDataStart(char *GpCode,short setcode,short &DataNum); //�ҵ�ĳ��Ʊ�Ĳ��񻺳������
	unsigned long Get_RecordNum(char *File);//�õ������ļ��ļ�¼����
	unsigned long Get_RecordNum();

	void	ResetRLTData(void);
	short	ReadRelationInfo(struct MemStkInfo * scode, STKRELATION *RLTData, short nMaxCount);

	void	ResetFaceData(void);
	long	ReadAllFaceInfo(TDXSTKOLDNAME* &FaceData);
	short	ReadFaceInfo(struct MemStkInfo * scode, TDXSTKOLDNAME *FaceData, short nMaxCount);

private:
	DBH_Head * 		m_CWBB;			//DBF  ͷ�ṹ
	char 			FileName[256];  //���ݿ��ļ������ڼ��ܹ�����ʹ��
	CBlowfish  		* Fish;

	CWDATA			*All_CwData;
	unsigned long   All_DataNum;

	//����
	STKRELATION		*ALL_RLTData;
	short			RLTDataSet;
	short			*RLTSetInfo;
	long			All_RLTDataNum;

	//FaceOff
	TDXSTKOLDNAME	*ALL_FaceData;
	long			ALL_FaceNum;
};

#endif
