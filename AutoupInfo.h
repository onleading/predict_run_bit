#ifndef AutoupInfo_h_
#define AutoupInfo_h_

struct NeedAutoupInfo
{
	char bNeedAutoup;			//�Ƿ���Ҫ���� 0:�������� 1:������ 2:��ǿ������
	float ThisVer;				//�ͻ��˰汾
	char  AutoupURL[120];		//��������ַĿ¼
	char  Descripton[2*1024-1];	//����˵��
	char  bAutoupFromHQHost;	//�Ƿ����վ��������
};

extern struct NeedAutoupInfo g_NeedAutoupInfo;

extern BOOL  GetAutoupInfo(struct NeedAutoupInfo *pAutoupInfo);
extern void  WriteAutoupInfo(struct NeedAutoupInfo *pAutoupInfo);
extern BOOL  ShowAutoupInfo();
//////////////////////////////////////////////////////

#endif