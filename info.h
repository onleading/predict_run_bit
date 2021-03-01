#ifndef INFO__H_
#define INFO__H_
/////////////////////////////////////////////////////////////////////////////
//	InfoWorkshop 资讯工厂的信息结构
/////////////////////////////////////////////////////////////////////////////

struct iwtag_struct
{
	char	filename[16];	//iw文件名前部,最多15个字节
	long	create_ymd;		//文件创建日期，年月日
	long	create_hms;		//文件创建时间，时分秒 
	long	rec_num;		//文件记录数
	char	unused[4];		//备用
};

struct iw_raw_head
{
	long	create_ymd;		//文件创建日期，年月日
	long	create_hms;		//文件创建时间，时分秒 
	char	unused[32];		//备用
};

struct iw_client_title
{	
	long    rec_id;			//记录的唯一ID
	long	time_ymd;		//发表日期，年月日
	long	time_hms;		//发表时间，时分秒 
	char	title[121];		//标题
	long	type_id;		//分类ID
	char	show_id;		//标记ID
	char	proc_id;		//处理ID
	char	info_src[31];	//信息来源
	char	info_format;	//资讯格式 0:txt 1:html 2:mht 3:网摘
	char	info_url[201];	//网摘URL
};

struct iw_raw_title
{	
	struct	iw_client_title onetitle;		//客户端使用的标题
	char					delflag;		//删除标志
	char					level;			//信息分级
	char					unused[26];		//备用
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
