// ExtenGNCtrl.cpp: implementation of the CExtenGNCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tdxw.h"
#include "ExtenGNCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

DWORD					l_GlobalUnitID=0;

extern long CALLBACK CalcCallBackFunc(char * Code,short nSetCode,short DataType,void * pData/*内部申请*/,short nDataNum,NTime tFrom,NTime tEnd,BYTE nTQ,unsigned long nReserved);
extern long CALLBACK SwitchCallBackFunc(char * Code,short nSetCode,char *buf,long buflen,short FuncType,short Option,unsigned long nReserved);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExtenGNCtrl::CExtenGNCtrl()
{

}

CExtenGNCtrl::~CExtenGNCtrl()
{
	for(long i=0;i<m_ModuleEntries.size();i++)
	{
		if(m_ModuleEntries[i].hModule != NULL)
		{
			AfxFreeLibrary(m_ModuleEntries[i].hModule);
			m_ModuleEntries[i].hModule = NULL;
		}
	}
	m_ModuleEntries.clear();
}

//////////////////////////////////////////////////////////////////////////
BOOL	CExtenGNCtrl::LoadModuleDll(LPCSTR  DllName, int nModule, IMODULE*iModule)	//	导入各模块
{
	char	InfoString[256]={0};
	iModule->hModule = AfxLoadLibrary(DllName);
	DWORD errCode = GetLastError();
	if(iModule->hModule==NULL)	return(FALSE);
/////////////////////////////////////////////////////////////////////////////
	_TDXWEXTGN_GETMODULEINFO	GetModuleInfo = (_TDXWEXTGN_GETMODULEINFO)
		GetProcAddress(iModule->hModule,"TDXWEXTGN_GetModuleInfo");
	_TDXWEXTGN_REGISTERCALLBACK RigisterCallBack = NULL;
	if(GetModuleInfo==NULL)	goto ErrorReturn;
	GetModuleInfo(&iModule->iModuleInfo);
/////////////////////////////////////////////////////////////////////////
//下面进行Module合法性校验
//////////////////////////////////////////////////////////////////////////
	RigisterCallBack = (_TDXWEXTGN_REGISTERCALLBACK)
		GetProcAddress(iModule->hModule, "TDXWEXTGN_RegisterCallBack");
	if(RigisterCallBack==NULL) goto ErrorReturn;
	RigisterCallBack((PDATAIOFUNC)CalcCallBackFunc,(PSWITCHINFOFUNC)SwitchCallBackFunc);
/////////////////////////////////////////////////////////////////////////////
	iModule->ModuleInit = (_TDXWEXTGN_INITMODULE)
		GetProcAddress(iModule->hModule,"TDXWEXTGN_InitModule");
	if(iModule->ModuleInit==NULL) goto ErrorReturn;
//////////////////////////////////////////////////////////////////////////
	iModule->ModuleScheme = (_TDXWEXTGN_Scheme)
		GetProcAddress(iModule->hModule,"TDXWEXTGN_Scheme");
	if(iModule->ModuleScheme==NULL) goto ErrorReturn;
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
	iModule->ModuleShow = (_TDXWEXTGN_SHOWMODULE)
		GetProcAddress(iModule->hModule,"TDXWEXTGN_ShowModule");
	if(iModule->ModuleShow==NULL) goto ErrorReturn;
//////////////////////////////////////////////////////////////////////////
	iModule->ModuleDataTrans = (_TDXWEXTGN_SETVALUE)
		GetProcAddress(iModule->hModule,"TDXWEXTGN_SetValue");
	if(iModule->ModuleDataTrans==NULL) goto ErrorReturn;
//////////////////////////////////////////////////////////////////////////
	iModule->ModuleResize = (_TDXWEXTGN_ARRANGEWND)
		GetProcAddress(iModule->hModule,"TDXWEXTGN_ArrangeWnd");
	if(iModule->ModuleResize==NULL) goto ErrorReturn;
//////////////////////////////////////////////////////////////////////////
	iModule->ModuleExit = (_TDXWEXTGN_EXITMODLE)
		GetProcAddress(iModule->hModule,"TDXWEXTGN_ExitModule");
	if(iModule->ModuleExit==NULL) goto ErrorReturn;

	return(TRUE);
ErrorReturn:
	AfxFreeLibrary(iModule->hModule);
	return(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
const char *dllstr = "module_*.dll";

long	GetExternFileCount(LPCSTR lpFormat)
{
	char	FilePath[MAX_PATH];
	memset(FilePath,0,MAX_PATH);
	sprintf(FilePath,"%s%s",HomePath,lpFormat);
	struct	_finddata_t	ffblk;
	long	handle = _findfirst(FilePath,&ffblk),done=0,nCount=0;
	if(handle==-1)	return(0);
	while(done==0)
	{
		if(ffblk.name[0]!='.'&&!(ffblk.attrib&_A_SUBDIR))	nCount++;
		done = _findnext(handle,&ffblk);
	}
	_findclose(handle);
	return(nCount);
}

long	CExtenGNCtrl::LoadExternModule(void)	//	导入模块
{
	long	DllNum = GetExternFileCount(dllstr),handle,done=0;
	if(DllNum==0)	return(0);
	m_ModuleEntries.clear();
	int nStartSubXH = 0;
	char	DllPath[MAX_PATH]={0};
	sprintf(DllPath,"%s%s",HomePath,dllstr);
	struct	_finddata_t	ffblk;
	handle = _findfirst(DllPath,&ffblk);
	if(handle!=-1)
	{
		while(done==0)
		{
			if(ffblk.name[0]!='.'&&!(ffblk.attrib&_A_SUBDIR))
			{
				sprintf(DllPath,"%s%s",HomePath,ffblk.name);
				IMODULE iModule={0};
				int nModule = m_ModuleEntries.size();
				if(LoadModuleDll(DllPath,nModule,&iModule))
				{
					iModule.nSubStartID = nStartSubXH;
					nStartSubXH += iModule.iModuleInfo.nSubModuleNum;
					m_ModuleEntries.push_back(iModule);
				}
			}
			done = _findnext(handle,&ffblk);
		}
		_findclose(handle);
	}
	return(m_ModuleEntries.size());
}
//////////////////////////////////////////////////////////////////////////
BOOL	CExtenGNCtrl::LoadModule(int nUnitStyle, int &nSubModule, IMODULE* &pModule)
{
	for(int i=0;i<m_ModuleEntries.size();i++)
	{
		if(nUnitStyle>=m_ModuleEntries[i].nSubStartID&&
			nUnitStyle<m_ModuleEntries[i].nSubStartID+m_ModuleEntries[i].iModuleInfo.nSubModuleNum)		//找到模块了
		{	
			nSubModule = nUnitStyle - m_ModuleEntries[i].nSubStartID;
			if(nSubModule<0||nSubModule>=m_ModuleEntries[i].iModuleInfo.nSubModuleNum) return FALSE;
			if(m_ModuleEntries[i].StatusFlag==0)						//尚未初始化
				m_ModuleEntries[i].ModuleInit(HomePath,g_WSXHStr);
			m_ModuleEntries[i].StatusFlag = 1;
			pModule = &m_ModuleEntries[i];
			return TRUE;
		}	
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////

BOOL	CExtenGNCtrl::ShowThis(HWND hParentWnd, long nNo, char *errMsg)
{
	short nModuleIndex = nNo/100;
	short nWhichSub = nNo%100;
	if(nModuleIndex<0||nModuleIndex>=m_ModuleEntries.size()) return FALSE;
	if(nWhichSub<0||nWhichSub>=m_ModuleEntries[nModuleIndex].iModuleInfo.nSubModuleNum) return FALSE;

	if(m_ModuleEntries[nModuleIndex].StatusFlag==0)						//尚未初始化
		m_ModuleEntries[nModuleIndex].ModuleInit(HomePath,g_WSXHStr);
	
	m_ModuleEntries[nModuleIndex].StatusFlag = 1;

	if(m_ModuleEntries[nModuleIndex].SubStatusFlag[nWhichSub])				//此功能没有Exit
	{
		strcpy(errMsg, "请退出相同模块功能!");
		return FALSE;
	}
	m_ModuleEntries[nModuleIndex].SubStatusFlag[nWhichSub]=1;

	//配色
	SCHEME_COLOR tmpColor={0};
	SCHEME_FONT  tmpFont={0};
	SCHEME_MENU  tmpMenu={0};
	Set_Scheme(tmpColor, tmpFont, tmpMenu);
	m_ModuleEntries[nModuleIndex].ModuleScheme(tmpColor, tmpFont, tmpMenu);

	if(m_ModuleEntries[nModuleIndex].ModuleShow(-1, nWhichSub, hParentWnd, "", "", errMsg))
		m_ModuleEntries[nModuleIndex].ParentWnd[nWhichSub] = hParentWnd;
	else
	{
		AfxMessageBox(errMsg);
		return FALSE;
	}

	return TRUE;
}

void	CExtenGNCtrl::ResizeThis(CWnd *pWnd,UINT cx,UINT cy)
{
	if(!pWnd) return;
	HWND hWnd = pWnd->GetSafeHwnd();	

	for(int i=0;i<m_ModuleEntries.size();i++)
	{
		for(int j=0;j<m_ModuleEntries[i].iModuleInfo.nSubModuleNum;j++)
		{
			if(m_ModuleEntries[i].SubStatusFlag[j]==1&&m_ModuleEntries[i].ParentWnd[j]&&m_ModuleEntries[i].ParentWnd[j]==hWnd)
				m_ModuleEntries[i].ModuleResize(-1, j, cx, cy);
		}
	}

}

void	CExtenGNCtrl::ExitThis(CWnd *pWnd)
{
	if(!pWnd) return;
	HWND hWnd = pWnd->GetSafeHwnd();

	for(int i=0;i<m_ModuleEntries.size();i++)
	{
		for(int j=0;j<m_ModuleEntries[i].iModuleInfo.nSubModuleNum;j++)
		{
			if(m_ModuleEntries[i].ParentWnd[j]&&m_ModuleEntries[i].ParentWnd[j]==hWnd)
			{
				m_ModuleEntries[i].ModuleExit(j, -1, "", "");
				m_ModuleEntries[i].ParentWnd[j]=NULL;
				m_ModuleEntries[i].SubStatusFlag[j]=0;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
