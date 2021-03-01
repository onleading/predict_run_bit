#include "stdafx.h"
#include "ComView_sub.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
//构建分时图或分析图下部分的扩展切换区

Switch_More_Info	g_SwitchMore[]=
{
	{"关联报价",	"关联的个股和板块报价",			MORE_GLHQ,0},
	{"涨跌异动",	"深沪当日和五分钟涨跌幅排行",	MORE_ZHSORT,1},
//	{"综合资讯",	"个股新闻,公告,评析和研究",		MORE_GPPAGE,1},

};
int						g_SwitchMoreNum = 2;
