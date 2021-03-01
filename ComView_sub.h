#ifndef COMVIEW_SUB_H_
#define COMVIEW_SUB_H_

//////////////////////////////////////////////////////////////////////////////////////////////////////
//构建分时图或分析图下部分的扩展切换区

//常规More的部分
#define MORE_GLHQ		0
#define MORE_ORDER		1
#define MORE_ZLGJT		2
#define MORE_REPORT		3
#define MORE_GP_PJPAGE	4
#define MORE_HY_PJPAGE	5
#define MORE_GPPAGE		6
#define MORE_HYPAGE		7
#define MORE_ZNPAGE		8
//个股复合资讯用到的
#define MORE_META_RP	9
#define MORE_META_PJ	10
#define MORE_META_HY	11
#define MORE_META_GG	12
#define MORE_META_XW	13
#define MORE_META_GW	14
//交易用到的
#define MORE_VOLIN		15
#define MORE_ZHINFO		16
#define MORE_WTLIST		17
//其它插件
#define MORE_FUNDDIST	18
#define MORE_ZHSORT		19

#define	MAX_SWITCH_MORE_NUM		21

struct Switch_More_Info
{
	char 	pStr[21];
	char 	pTipStr[51];
	int		nMoreValue;
	int		nFlag;
};

extern struct Switch_More_Info	g_SwitchMore[MAX_SWITCH_MORE_NUM];
extern int						g_SwitchMoreNum;


#endif