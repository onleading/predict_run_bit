#include "stdafx.h"
#include "tdxw.h"
#include "Plugin_gn.h"
#include "custompad.h"

//用于定制版面存盘
char g_GNPlug_TypeStr[MAX_GNPLUGIN_NUM][41]={0};

CGNPlugin::CGNPlugin()
{
}

CGNPlugin::~CGNPlugin()
{
	for(long i=0;i<GetPluginNum();i++)
	{
		if(m_Modules[i].hModule)
		{
			m_Modules[i].UnInitModule(0);
			AfxFreeLibrary(m_Modules[i].hModule);
			m_Modules[i].hModule = NULL;
		}
	}
	m_Modules.clear();
}

BOOL	CGNPlugin::GetOnePluginInfo(int iNo, GNPluginInfo * pInfo)
{
	if(iNo<0||iNo>=GetPluginNum()) 
		return FALSE;
	memcpy(pInfo, &m_Modules[iNo].iInfo, sizeof(GNPluginInfo));
	return TRUE;
};

BOOL	CGNPlugin::AdjustPlugin(GNPluginInfo *pInfo)
{
	if(!pInfo) return FALSE;
	if(pInfo->nModuleType<GNPLUG_TYPE_EMBED || pInfo->nModuleType>GNPLUG_TYPE_VIEW)
		return FALSE;
	pInfo->strModuleCode[40] = 0;
	pInfo->strModuleDesc[40] = 0;
	pInfo->strProvider[40] = 0;
	pInfo->strVersion[40] = 0;
	if(pInfo->nModuleType==GNPLUG_TYPE_EMBED)
	{
		pInfo->nUnitID += EXTERN1_UNIT;
		if(pInfo->nUnitID<EXTERN1_UNIT || pInfo->nUnitID>=EXTERN100_UNIT)
			return FALSE;
	}
	else if(pInfo->nModuleType==GNPLUG_TYPE_VIEW)
	{
		if(pInfo->nViewID<0 || pInfo->nViewID>=100)
			return FALSE;
	}
	int i;
	for(i=0;i<m_Modules.size();i++) 
	{
		if(stricmp(pInfo->strModuleCode,m_Modules[i].iInfo.strModuleCode)==0)
			break;
		else if(pInfo->nModuleType==m_Modules[i].iInfo.nModuleType)
		{
			if(pInfo->nModuleType==GNPLUG_TYPE_EMBED)
			{
				if(pInfo->nUnitID==m_Modules[i].iInfo.nUnitID)
					break;
			}
			else if(pInfo->nModuleType==GNPLUG_TYPE_VIEW)
			{
				if(pInfo->nViewID==m_Modules[i].iInfo.nViewID)
					break;
			}
		}
	}
	if(i<m_Modules.size())
		return FALSE;
	return TRUE;
};

//导入单个模块
BOOL	CGNPlugin::LoadPluginDll(LPCSTR DllName, IGNPLUGIN* iPlugin)
{
	if(!iPlugin)
		return FALSE;
	iPlugin->hModule = AfxLoadLibrary(DllName);
	if(!iPlugin->hModule)	
		return FALSE;
	//
	_TE_GetModuleInfo		GetModuleInfo;
	//查询模块基本信息
	GetModuleInfo = (_TE_GetModuleInfo)GetProcAddress(iPlugin->hModule,"TE_GetModuleInfo");
	if(!GetModuleInfo)	
		goto ErrorReturn;
	GetModuleInfo(&iPlugin->iInfo);
	if(!AdjustPlugin(&iPlugin->iInfo))
		goto ErrorReturn;
	//注册回调函数
	iPlugin->RigisterCallBack = (_TE_RegisterCallBack)GetProcAddress(iPlugin->hModule, "TE_RegisterCallBack");
	if(!iPlugin->RigisterCallBack)	
		goto ErrorReturn;
	iPlugin->RigisterCallBack((PDATAIOFUNC)CalcCallBackFunc,(PSWITCHINFOFUNC)SwitchCallBackFunc);
	//设置配色和字体等
	iPlugin->SetScheme = (_TE_SetScheme)GetProcAddress(iPlugin->hModule, "TE_SetScheme");
	if(!iPlugin->SetScheme)	
		goto ErrorReturn;
	//设置初始化信息
	iPlugin->InitModule = (_TE_InitModule)GetProcAddress(iPlugin->hModule,"TE_InitModule");
	if(!iPlugin->InitModule) 
		goto ErrorReturn;
	iPlugin->InitModule(HomePath,g_WSXHStr,0);

	//下面进行Module合法性校验
	iPlugin->ShowFunc = (_TE_ShowFunc)GetProcAddress(iPlugin->hModule,"TE_ShowFunc");
	if(!iPlugin->ShowFunc) 
		goto ErrorReturn;

	iPlugin->ShowModule = (_TE_ShowModule)GetProcAddress(iPlugin->hModule,"TE_ShowModule");
	if(!iPlugin->ShowModule) 
		goto ErrorReturn;

	iPlugin->ArrangeWnd = (_TE_ArrangeWnd)GetProcAddress(iPlugin->hModule,"TE_ArrangeWnd");
	if(!iPlugin->ArrangeWnd) 
		goto ErrorReturn;

	iPlugin->ProcMsg = (_TE_ProcMsg)GetProcAddress(iPlugin->hModule,"TE_ProcMsg");
	if(!iPlugin->ProcMsg) 
		goto ErrorReturn;

	iPlugin->ExitModule = (_TE_ExitModule)GetProcAddress(iPlugin->hModule,"TE_ExitModule");
	if(!iPlugin->ExitModule) 
		goto ErrorReturn;

	iPlugin->GetCurrentGPInfo = (_TE_GetCurrentGPInfo)GetProcAddress(iPlugin->hModule,"TE_GetCurrentGPInfo");
	if(!iPlugin->GetCurrentGPInfo) 
		goto ErrorReturn;

	iPlugin->SetCurrentGPCode = (_TE_SetCurrentGPCode)GetProcAddress(iPlugin->hModule,"TE_SetCurrentGPCode");
	if(!iPlugin->SetCurrentGPCode) 
		goto ErrorReturn;

	iPlugin->UnInitModule = (_TE_UnInitModule)GetProcAddress(iPlugin->hModule,"TE_UnInitModule");
	if(!iPlugin->UnInitModule) 
		goto ErrorReturn;
	
	return TRUE;

ErrorReturn:
	AfxFreeLibrary(iPlugin->hModule);
	iPlugin->hModule = NULL;
	return FALSE;
}

long	CGNPlugin::GetGNPluginNum(LPCSTR lpFormat)
{
	char	FilePath[MAX_PATH] = {0};
	sprintf(FilePath,"%sGNPlugins\\%s",HomePath,lpFormat);
	struct	_finddata_t	ffblk;
	int		nCount = 0;
	long	handle = _findfirst(FilePath,&ffblk),done=0;
	if(handle==-1)	
		return 0;
	while(done==0)
	{
		if(ffblk.name[0]!='.'&&!(ffblk.attrib&_A_SUBDIR))	
			nCount++;
		done = _findnext(handle,&ffblk);
	}
	_findclose(handle);
	return nCount;
}

//导入所有外部可用模块
//需要检查有没有重复的strModuleCode和nUnitID(如果是nModuleType的话,nUnitID必须在60-160之间
long	CGNPlugin::LoadGNPlugin(void)
{
	long DllNum = GetGNPluginNum("TE_*.dll");
	if(DllNum==0)	
		return 0;

	m_Modules.clear();

	char	DllPath[MAX_PATH]={0};
	sprintf(DllPath,"%sGNPlugins\\%s",HomePath,"TE_*.dll");
	long	handle,done = 0,nCnt = 0;
	struct	_finddata_t	ffblk;
	handle = _findfirst(DllPath,&ffblk);
	if(handle!=-1)
	{
		while(done==0 && nCnt<MAX_GNPLUGIN_NUM)
		{
			if(ffblk.name[0]!='.'&&!(ffblk.attrib&_A_SUBDIR))
			{
				sprintf(DllPath,"%sGNPlugins\\%s",HomePath,ffblk.name);
				IGNPLUGIN iPlugin={0};
				int nModule = m_Modules.size();
				if(LoadPluginDll(DllPath,&iPlugin))
				{
					m_Modules.push_back(iPlugin);
					nCnt++;
				}
			}
			done = _findnext(handle,&ffblk);
		}
		_findclose(handle);
	}
	return m_Modules.size();
}

//根据内部代码得到模块信息
IGNPLUGIN*	CGNPlugin::GetPluginFromModuleCode(char *ModuleCode)
{
	int nModule = GetPluginNum();
	for(int i=0;i<nModule;i++)
	{
		if(strcmp(ModuleCode, m_Modules[i].iInfo.strModuleCode)==0)
			return &m_Modules[i];
	}
	return NULL;
}

char*		CGNPlugin::GetModuleCodeFromUnitID(UINT nUnitID)
{
	int nModule = GetPluginNum();
	for(int i=0;i<nModule;i++)
	{
		if(m_Modules[i].iInfo.nModuleType==GNPLUG_TYPE_EMBED && m_Modules[i].iInfo.nUnitID==nUnitID)
			return m_Modules[i].iInfo.strModuleCode;
	}
	return NULL;
}

IGNPLUGIN*	CGNPlugin::GetPluginFromTypeOrder(int iOrder,int nType)
{
	int nModule = GetPluginNum();
	int nCnt = 0;
	for(int i=0;i<nModule;i++)
	{
		if(m_Modules[i].iInfo.nModuleType==nType)
		{
			if(nCnt==iOrder) 
				return &m_Modules[i];
			nCnt++;
		}
	}
	return NULL;
}

IGNPLUGIN*	CGNPlugin::GetPlugin(int iNo)
{
	int nModule = GetPluginNum();
	if(iNo<0 || iNo>=nModule)
		return NULL;
	return &m_Modules[iNo];
}
