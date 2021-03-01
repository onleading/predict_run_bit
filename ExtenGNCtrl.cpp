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

extern long CALLBACK CalcCallBackFunc(char * Code,short nSetCode,short DataType,void * pData/*内部申请*/,short nDataNum,NTime tFrom,NTime tEnd,BYTE nTQ,unsigned long nReserved);
extern long CALLBACK SwitchCallBackFunc(char * Code,short nSetCode,char *buf,long buflen,short FuncType,short Option,unsigned long nReserved);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExtenGNCtrl::CExtenGNCtrl()
{
	m_nActiveModule = -1;
	m_nWhichSub = -1;
}

CExtenGNCtrl::~CExtenGNCtrl()
{
	for(long i=0;i<m_ModuleEntries.size();i++)
	{
		if(m_ModuleEntries[i].hModule != NULL)
		{
		//	m_ModuleEntries[i].ModuleExit();
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
	if(!iModule->hModule)	
		return FALSE;

	return TRUE;

ErrorReturn:
	AfxFreeLibrary(iModule->hModule);
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
const char *dllstr = "TE_*.dll";

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
					m_ModuleEntries.push_back(iModule);
			}
			done = _findnext(handle,&ffblk);
		}
		_findclose(handle);
	}
	return(m_ModuleEntries.size());
}

IMODULE*	CExtenGNCtrl::GetModuleFromCode(char *Name)
{
	int nModule = m_ModuleEntries.size();
	for(int i=0;i<nModule;i++)
	{
		if(strcmp(Name, m_ModuleEntries[i].iModuleInfo.strModuleName)==0)
			return &m_ModuleEntries[i];
	}
	return NULL;
}

BOOL	CExtenGNCtrl::ShowThis(HWND hParentWnd, long nNo, char *errMsg)
{
	return TRUE;
}

void	CExtenGNCtrl::ResizeThis(CWnd *pWnd,UINT cx,UINT cy)
{
	
}

void	CExtenGNCtrl::ExitThis(CWnd *pWnd)
{
	
}
