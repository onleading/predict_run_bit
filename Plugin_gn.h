#ifndef _PLUGIN_GN_H_
#define _PLUGIN_GN_H_

#include "OutStruct.h"

/////////////////////////////////////////////////////////////////////////////
//					�ⲿ���ܲ���ӿ�
/////////////////////////////////////////////////////////////////////////////

#define MAX_GNPLUGIN_NUM	100

#define GNPLUG_TYPE_EMBED	0			//Unit�����
#define GNPLUG_TYPE_DIALOG	1			//�Ի������
#define GNPLUG_TYPE_VIEW	2			//View�����

//��Ҫ�Զ�����Ϣ�ӿ�
#define UM_UNIT_BROADCAST			WM_USER+8000	//�㲥
#define UM_EXOPENBLOCKHQ			WM_USER+8001	//�򿪰��

struct GNPluginInfo
{
	UINT 	nModuleType;		//Unit�����,�Ի������,View�����
	char 	strModuleCode[41];	//ģ���ڲ���
	char 	strModuleDesc[41];	//ģ������
	char 	strProvider[41];	//ģ���Ȩ
	char	strVersion[41];		//�汾��Ϣ
	short	nUnitID;			//�����Unit������Ļ�,UnitID��0-99֮��
	short	nGNLevel;			//���ܼ��� 1-2
	char	bFirstRefreshAllHQ;	//����ǰ�Ƿ�Ҫˢ����������
	short	nViewID;			//�����View������Ļ�,nViewID��0-99֮��
	char	bbPlugin_GPRel;		//������Ƿ�������
	char	bOnlyInCom;			//����֧�����ض���������ʾ(�����ڶ��ư��������,����Unit�������Ч)
	char	RightChar;			//Ȩ���ַ���
	char	unUsed[167];		//����
};

//ȡģ����Ϣ
typedef void	(*_TE_GetModuleInfo)(struct GNPluginInfo *pInfo);
//����ģ��ص������ĵ�ַ
typedef void	(*_TE_RegisterCallBack)(PDATAIOFUNC pDataFn,PSWITCHINFOFUNC pFuncFn);
//������ɫ�������
typedef void	(*_TE_SetScheme)(SCHEME_COLOR color,SCHEME_FONT font,BOOL bRedraw);
//��ʼ��ģ��
typedef BOOL	(*_TE_InitModule)(const char *PubDir=".\\",const char *PriDir=".\\", DWORD unUsed=0); 

//-----���¿�����Unit�������View�����
//��ʾĳ�ֹ���
typedef BOOL	(*_TE_ShowFunc)(CWnd *pParent,UINT nFuncType,UINT nFuncOption,long reserved);

//-----����ֻ����Unit�����
//ע��nCtrlId,�����ж��,0��ʾ����,����Ctrl��1��ʼ
//�ڿ����������ʾģ��Ľ���
typedef BOOL	(*_TE_ShowModule)(HWND hVessel,char *errMsg,UINT nCtrlId);
//��С�ı仯����Ӧ
typedef void 	(*_TE_ArrangeWnd)(UINT cx,UINT cy,UINT nCtrlId);
//�˳���������
typedef void	(*_TE_ExitModule)(UINT nCtrlId);

//-----���¿�����Unit�������View�����
//��Ӧ��Ϣ
typedef int		(*_TE_ProcMsg)(UINT message, WPARAM wParam, LPARAM lParam,UINT nCtrlId);
//�õ���ǰ������֤ȯ��Ϣ
typedef void	(*_TE_GetCurrentGPInfo)(short & setcode,char *strCode,UINT nCtrlId);
//���õ�ǰ������֤ȯ��Ϣ
typedef BOOL	(*_TE_SetCurrentGPCode)(short setcode,const char *strCode,UINT nCtrlId);

//�˳�ģ��
typedef void	(*_TE_UnInitModule)(DWORD unUsed=0); 

typedef	struct	tagGNPluginIndex
{
	HMODULE					hModule;			//	��̬���ӿ���
	GNPluginInfo			iInfo;				//	�����Ϣ

	_TE_RegisterCallBack	RigisterCallBack;
	_TE_SetScheme			SetScheme;
	_TE_InitModule			InitModule;

	_TE_ShowFunc			ShowFunc;
	_TE_ShowModule			ShowModule;
	_TE_ArrangeWnd			ArrangeWnd;
	_TE_ProcMsg				ProcMsg;
	_TE_ExitModule			ExitModule;
	_TE_GetCurrentGPInfo	GetCurrentGPInfo;
	_TE_SetCurrentGPCode	SetCurrentGPCode;

	_TE_UnInitModule		UnInitModule;
}IGNPLUGIN;


extern char g_GNPlug_TypeStr[MAX_GNPLUGIN_NUM][41];

/////////////////////////////////////////////////////////////////////////////
//					CGNPlugin �ⲿ�ӿڴ�����
/////////////////////////////////////////////////////////////////////////////

class CGNPlugin
{
public:
	CGNPlugin();
	virtual ~	CGNPlugin();

	long		LoadGNPlugin();
	short		GetPluginNum()	{ return m_Modules.size(); }
	BOOL		GetOnePluginInfo(int iNo, GNPluginInfo * pInfo);
	IGNPLUGIN*	GetPluginFromModuleCode(char *ModuleCode);
	char*		GetModuleCodeFromUnitID(UINT nUnitID);

	IGNPLUGIN*	GetPluginFromTypeOrder(int iOrder,int nType);
	IGNPLUGIN*	GetPlugin(int iNo);

private:
	long		GetGNPluginNum(LPCSTR lpFormat);
	BOOL		AdjustPlugin(GNPluginInfo *pInfo);
	BOOL		LoadPluginDll(LPCSTR  DllName, IGNPLUGIN *iPlugin);

private:
	vector<IGNPLUGIN> m_Modules;

};

#endif
