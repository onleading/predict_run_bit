#ifndef OPADDITION_H_
#define OPADDITION_H_

struct setcode_and_stockcode
{
 	char setcode;
	char stockcode[6];
};

struct stock_mine_info
{
 	char setcode;
	char stockcode[6];
	char count;
};

#define ITYPE_CJZX				0	//�ƾ���Ѷ
#define ITYPE_RLMINE			1	//ʵʱ��Ϣ����
#define ITYPE_HSMINE			2	//��ʷ��Ϣ����
#define ITYPE_F10				3	//����F10����

struct ext_info_title
{	
	short	companyno;	//��Ѷ��˾�����Դ
	short   info_type;	//��Ϣ����
	long    rec_id;		//��¼��ΨһID
	short	branch;		//��֧�������
	long	time_ymd;	//�������ڣ�������
	long	time_hms;	//����ʱ�䣬ʱ���� 
	char	title[64];	//����
	long	Infotreeid;	//������ĿID
	char	infosrc[20];//��Ϣ��Դ
	short	infoformat;	//��Ϣ��ʽ
	short	minetoken;	//���ױ������
	short	issuetype;	//������������
	char	author[30];	//����
	short	author_level;//�����˼���
	long	contentlen;	 //���ݳ���
	long	modify_ymd;	 //�޸����ڣ�������
	long	modify_hms;	 //�޸�ʱ�䣬ʱ���� 
};

struct ext_ggbw_title
{	
	long  rec_id;
	char  setcode;
	char  code[SH_CODE_LEN+1];
	long  time_ymd;
	long  time_hms;
	long  climate;		  //����������� 0:�� 1:�� 2:�� 3:ѩ
	char  title[64];	  //����
	long  pos;			  //�������ļ��е�ƫ�Ƶ�ַ
	long  contentlen;	  //���ݳ���
	long  modify_ymd;	  //�޸����ڣ�������
	long  modify_hms;	  //�޸�ʱ�䣬ʱ���� 
};

#endif
