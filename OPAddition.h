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

#define ITYPE_CJZX				0	//财经资讯
#define ITYPE_RLMINE			1	//实时信息地雷
#define ITYPE_HSMINE			2	//历史信息地雷
#define ITYPE_F10				3	//分类F10资料

struct ext_info_title
{	
	short	companyno;	//资讯公司编号来源
	short   info_type;	//信息类型
	long    rec_id;		//记录的唯一ID
	short	branch;		//分支机构编号
	long	time_ymd;	//发表日期，年月日
	long	time_hms;	//发表时间，时分秒 
	char	title[64];	//标题
	long	Infotreeid;	//分类栏目ID
	char	infosrc[20];//信息来源
	short	infoformat;	//信息格式
	short	minetoken;	//地雷标记类型
	short	issuetype;	//发表文章类型
	char	author[30];	//作者
	short	author_level;//发表人级别
	long	contentlen;	 //内容长度
	long	modify_ymd;	 //修改日期，年月日
	long	modify_hms;	 //修改时间，时分秒 
};

struct ext_ggbw_title
{	
	long  rec_id;
	char  setcode;
	char  code[SH_CODE_LEN+1];
	long  time_ymd;
	long  time_hms;
	long  climate;		  //当日天气情况 0:晴 1:阴 2:雨 3:雪
	char  title[64];	  //标题
	long  pos;			  //在内容文件中的偏移地址
	long  contentlen;	  //内容长度
	long  modify_ymd;	  //修改日期，年月日
	long  modify_hms;	  //修改时间，时分秒 
};

#endif
