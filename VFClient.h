#ifndef VFCLIENT_H_
#define VFCLIENT_H_
/*****************************************************************
	ͨ���ſ����������ݽӿ�ģ��

	2008.04.09	֧��ȡ֤ȯƷ�ֵĻ���������Ϣ
******************************************************************/
#pragma pack(push,1) //�����һ�ֽڶ���

typedef struct tagFastHQ
{
	float		Close;						//	4B		ǰ���̼�
	float		Open;						//	4B		���̼�
	float		Max;						//	4B		��߼�
	float		Min;						//	4B		��ͼ�
	float		Now;						//	4B		�ּ�
	DWORD		Volume;						//	4B		����
	float		Buyp[5];					//	20B		�����
	DWORD		Buyv[5];					//	20B		��Ӧ����۵�����	  
	float		Sellp[5];					//	20B		������
	DWORD		Sellv[5];					//	20B		��Ӧ�����۵�����
}FASTHQ;

typedef struct tagFastHQ10
{
	float		Close;						//	4B		ǰ���̼�
	float		Open;						//	4B		���̼�
	float		Max;						//	4B		��߼�
	float		Min;						//	4B		��ͼ�
	float		Now;						//	4B		�ּ�
	DWORD		Volume;						//	4B		����
	float		Buyp[10];					//	40B		�����
	DWORD		Buyv[10];					//	40B		��Ӧ����۵�����	  
	float		Sellp[10];					//	40B		������
	DWORD		Sellv[10];					//	40B		��Ӧ�����۵�����
}FASTHQ_10;

typedef struct
{
	BOOL	bProxy;				//�Ƿ����ô���
	char	nSockType;			//��������:0:SOCK4���� 1:SOCK5���� 2:Http����
	UINT	nProxyPort;			//����������Ķ˿�
	char	strProxyIP[100];	//����������ĵ�ַ
	char	strProxyPass[100];	//������֤�û���
	char	strProxyUser[100];	//������֤����
}FPROXYINFO;

typedef struct 
{	
	UINT	nPort;				//������վ�Ķ˿�
	char	strIPAddress[100];	//������վ�ĵ�ַ	
}FHOSTINFO;

#define		SST_5BS			0
#define		SST_10BS		1

#pragma pack(pop)
#endif