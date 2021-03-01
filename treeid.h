#ifndef TREEID_H_
#define TREEID_H_

#define CMD_MAIN	0		//主窗口
#define CMD_HQ		1		//行情窗口
#define CMD_COM		2		//分析窗口
#define CMD_JBM		3		//基本资料窗口
#define CMD_NEWS	4		//公告消息窗口
#define CMD_HIS		5		//历史分析窗口
#define CMD_ADVHQ	6		//高级行情窗口
#define CMD_JBFX	7		//基本分析窗口
#define CMD_JJ		8		//基金分析窗口

#define CMD_DIRECT	50		//直接的Command
#define CMD_TCDLG	51		//交易对话框

struct TreeID_Map
{
	char	IDStr[20];
	DWORD	CommandType;
	DWORD	CommandID;
};

extern struct TreeID_Map g_pTreeIDMap[];

extern void TreeID_CarryOut(const char *pIDStr);
extern BOOL BeforeNavigate_Process(char *url);

#endif