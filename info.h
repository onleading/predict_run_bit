#ifndef INFO__H_
#define INFO__H_
/////////////////////////////////////////////////////////////////////////////
//	InfoWorkshop ��Ѷ��������Ϣ�ṹ
/////////////////////////////////////////////////////////////////////////////

struct iwtag_struct
{
	char	filename[16];	//iw�ļ���ǰ��,���15���ֽ�
	long	create_ymd;		//�ļ��������ڣ�������
	long	create_hms;		//�ļ�����ʱ�䣬ʱ���� 
	long	rec_num;		//�ļ���¼��
	char	unused[4];		//����
};

struct iw_raw_head
{
	long	create_ymd;		//�ļ��������ڣ�������
	long	create_hms;		//�ļ�����ʱ�䣬ʱ���� 
	char	unused[32];		//����
};

struct iw_client_title
{	
	long    rec_id;			//��¼��ΨһID
	long	time_ymd;		//�������ڣ�������
	long	time_hms;		//����ʱ�䣬ʱ���� 
	char	title[121];		//����
	long	type_id;		//����ID
	char	show_id;		//���ID
	char	proc_id;		//����ID
	char	info_src[31];	//��Ϣ��Դ
	char	info_format;	//��Ѷ��ʽ 0:txt 1:html 2:mht 3:��ժ
	char	info_url[201];	//��ժURL
};

struct iw_raw_title
{	
	struct	iw_client_title onetitle;		//�ͻ���ʹ�õı���
	char					delflag;		//ɾ����־
	char					level;			//��Ϣ�ּ�
	char					unused[26];		//����
};

#define MAX_ONE_IWTITLE_NUM		80
#define MAX_OBSERVER_ROLL_NUM	80

extern struct iw_client_title	g_SwapIWTitle[4][MAX_ONE_IWTITLE_NUM];
extern long						g_nSwapIWTitleNum[4];
extern long						g_nSwapIWTitleTotalNum[4];
extern CString					g_strIWTitle;
extern CString					g_strIWURL;
extern long						g_nObserverACKCnt;

extern struct iw_client_title	g_ObserverRollInfo[MAX_OBSERVER_ROLL_NUM];
extern long						g_ObserverRollInfoNum;

extern char						g_strIwContentPath[MAX_PATH];

extern void		LoadIwConfig();
extern CString	GetIWURL(iw_client_title *pTitle);

#endif
