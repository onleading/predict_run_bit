#ifndef AutoupInfo_h_
#define AutoupInfo_h_

struct NeedAutoupInfo
{
	char bNeedAutoup;			//是否需要升级 0:无需升级 1:需升级 2:需强制升级
	float ThisVer;				//客户端版本
	char  AutoupURL[120];		//升级的网址目录
	char  Descripton[2*1024-1];	//升级说明
	char  bAutoupFromHQHost;	//是否从主站进行升级
};

extern struct NeedAutoupInfo g_NeedAutoupInfo;

extern BOOL  GetAutoupInfo(struct NeedAutoupInfo *pAutoupInfo);
extern void  WriteAutoupInfo(struct NeedAutoupInfo *pAutoupInfo);
extern BOOL  ShowAutoupInfo();
//////////////////////////////////////////////////////

#endif