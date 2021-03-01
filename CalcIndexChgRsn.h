#ifndef __CALCINDEXCHANGEREASON__H__
#define __CALCINDEXCHANGEREASON__H__

//��֤�ۺ�ָ��:�Ͻ���ȫ�����й�Ʊ,����A�ɺ�B��,��1990��12��19��Ϊ����,�Ը��յ��м���ֵΪ����(Ȩ��Ϊ�ܹɱ�)
//��֤�ۺ�ָ��:���ȫ�����й�Ʊ,Ȩ��Ϊ�ܹɱ�
//��֤�ɷ�ָ��:���40֧������Ʊ,Ȩ��Ϊ��ͨ�ɱ�
//����300ָ��:�300֧������Ʊ,Ȩ��Ϊ�ܹɱ�
//���׶�=INDEXC*(N-C)*V/��CV,����CΪ�������̼۸�,VΪ�ܹɱ�������ͨ�ɱ�,N,C,VΪͬһ��Ȩģʽ�µ���ֵ,NΪ�ּ�,CΪ�ο�����,V��Դ��Base.dbf

typedef struct StkWeight
{
	short	SetCode;										//�г�����
	char	StkCode[14];									//֤ȯ����
	float	fClose;											//���̼۸�
	float	fNow;
	float	fCapital;										//�ɱ�(��ͨ������,����ָ���㷨����)
	double	dWeight;										//Ȩ��
	float	fResult;										//���׶�
}STKWEIGHT,*LPSTKWEIGHT;

class TCalcIndexChgReason
{
public:
	TCalcIndexChgReason();
	~TCalcIndexChgReason();
	BOOL SetInitData(char *szIndexName,float fIndexC,int nMaxGpNum = 6000);	//����ָ�����,ָ������,Ȩ��ģʽ
	BOOL AddOne(short SetCode,char *StkCode,float fNow,float fClose,float fCapital);	//���óɷֹ�Ʊ,��Ʊ�ɱ�,��Ʊ����
	BOOL ReCalcWeight();													//����Ȩ��
	//���ù�Ʊ�ּ�,���㹱�׶�
	BOOL UpdateResult(short SetCode,char *StkCode,float fNow);
	//ȡ�ù�Ʊ���׶�
	BOOL GetOneResult(short SetCode,char *StkCode,float &fResult);
	int GetAllResult(LPSTKWEIGHT &pStkWeight);
private:
	BOOL		m_bInited;					//�Ѿ���ʼ��ָ����Ϣ
	BOOL		m_bWeightCaled;				//�Ѿ�����ø���Ȩ��
	//
	char		m_IndexName[26];			//ָ������
	float		m_IndexClose;				//ָ������
	//
	StkWeight	*m_pStkWeight;				//������Ϣ
	int			m_nStkWeight;				//���ɸ���
	int			m_nMaxStkNum;				//����Ʊ����
	int			m_nStkSpace;				//������Ϣ�ռ��С
};

#endif
