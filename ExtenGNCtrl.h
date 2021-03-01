#ifndef EXTERNGNCTRL_H_
#define EXTERNGNCTRL_H_

struct ModuleInfo
{
	UINT 	nModuleType;		//��MODULE_STOCK, MODULE_ETF, MODULE_FUTURES�Ⱥ�
	char 	strModuleName[41];	//ģ����,��Ϊ����������֦��
	char 	strProvider[41];	//ģ���Ȩ
	float 	fVersion;			//��ģ��İ汾��
	char	strSerialNo[33];	//���к�
	char	unUsed[222];
};

//ȡģ����Ϣ
typedef void	(*_TE_GetModuleInfo)(struct ModuleInfo *pInfo);
//����ģ��ص������ĵ�ַ
typedef void	(*_TE_RegisterCallBack)(PDATAIOFUNC pDataFn,PSWITCHINFOFUNC pFuncFn);
//��ʼ��ģ��
typedef BOOL	(*_TE_InitModule)(const char *PubDir=".\\",const char *PriDir=".\\", DWORD unUsed=0); 
//�ڿ����������ʾģ��Ľ���
typedef BOOL	(*_TE_ShowModule)(HWND hVessel,char *errMsg,UINT nCtrlId);
//��С�ı仯����Ӧ
typedef void 	(*_TE_ArrangeWnd)(UINT cx,UINT cy,UINT nCtrlId);
//��Ӧ��Ϣ
typedef int		(*_TE_ProcMsg)(UINT message, WPARAM wParam, LPARAM lParam,UINT nCtrlId);
//�˳�ģ��
typedef void	(*_TE_ExitModule)(UINT nCtrlId);

typedef	struct	tagModuleIndex
{
	HMODULE					hModule;				//	��̬���ӿ���
	ModuleInfo				iModuleInfo;			//	ģ����Ϣ

	_TE_InitModule			ModuleInit;
	_TE_ShowModule			ModuleShow;
	_TE_ArrangeWnd			ModuleArrangeWnd;
	_TE_ProcMsg				ModuleProcMsg;
	_TE_ExitModule			ModuleExit;
}IMODULE;

class CExtenGNCtrl
{
public:
	CExtenGNCtrl();
	virtual ~CExtenGNCtrl();

	long	LoadExternModule(void);
	BOOL	ShowThis(HWND hParentWnd, long nNo, char *errMsg);
	void	ResizeThis(CWnd *pWnd,UINT cx,UINT cy);
	void	ExitThis(CWnd *pWnd);

	short	CExtenGNCtrl::GetModuleNum(void)
	{
		return m_ModuleEntries.size();
	};

	BOOL	CExtenGNCtrl::GetModuleInfo(short nModule, ModuleInfo &iModuleInfo)
	{
		if(nModule<0||nModule>=m_ModuleEntries.size()) return FALSE;
		memcpy(&iModuleInfo, &m_ModuleEntries[nModule].iModuleInfo, sizeof(ModuleInfo));
		return m_ModuleEntries[nModule].hModule!=NULL;
	};

	IMODULE*	GetModuleFromCode(char *Name);

private:
	BOOL		LoadModuleDll(LPCSTR  DllName, int nModule, IMODULE *iModule);
private:
	vector<IMODULE> m_ModuleEntries;
	long			m_nActiveModule;
	long			m_nWhichSub;
};

#endif
