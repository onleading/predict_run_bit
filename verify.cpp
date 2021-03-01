#include "stdafx.h"
#include "verify.h"

CXiaDanDllInfo*	m_pXiaDanDllInfo = NULL;
CTDXDllInfo*	m_pTDXDllInfo = NULL;

BOOL DoHexinDLLWt(HWND hWnd,BOOL bVerify)
{
	if(!m_pXiaDanDllInfo)
	{
		m_pXiaDanDllInfo = new CXiaDanDllInfo;
		memset(m_pXiaDanDllInfo, 0, sizeof(CXiaDanDllInfo));

		m_pXiaDanDllInfo->m_hDLL3 = AfxLoadLibrary("commdll.dll");
		if(m_pXiaDanDllInfo->m_hDLL3 == NULL) return FALSE;
		LPFNDLLFUNC3 lpfnDllFunc3 = (LPFNDLLFUNC3)GetProcAddress(m_pXiaDanDllInfo->m_hDLL3,"ClxEnumCommProtocol");
		if(lpfnDllFunc3 == NULL) return FALSE;
		char szDllID[80];
		char szDllName[80];
		BOOL bLoop = TRUE, bFirst = TRUE;
		while (bLoop)
		{
			bLoop = lpfnDllFunc3(szDllID, szDllName, bFirst);
			bFirst = FALSE;
		}
		m_pXiaDanDllInfo->m_hDLL2 = AfxLoadLibrary("tcpip.dll");
		if(m_pXiaDanDllInfo->m_hDLL2 == NULL) return FALSE;
		LPFNDLLFUNC2 lpfnDllFunc2 = (LPFNDLLFUNC2)GetProcAddress(m_pXiaDanDllInfo->m_hDLL2,"ClxCommInit");
		if(lpfnDllFunc2 == NULL) return FALSE;
		lpfnDllFunc2("yyy.ini", NULL);

		m_pXiaDanDllInfo->m_hInstance = AfxLoadLibrary("xiadan.dll");
		if(m_pXiaDanDllInfo->m_hInstance == NULL)
		{
			TDX_MessageBox(hWnd,"不能发现交易模块!",MB_OK|MB_ICONERROR);
			return FALSE;
		}
		m_pXiaDanDllInfo->m_pfnCreateWnd = (CREATE_XIADAN_WND)GetProcAddress(m_pXiaDanDllInfo->m_hInstance, "CreateXiaDanWnd");
		m_pXiaDanDllInfo->m_pfnGetWndNumber = (GET_XIADAN_WNDNUMBER)GetProcAddress(m_pXiaDanDllInfo->m_hInstance, "GetXiaDanWndNumber");
		m_pXiaDanDllInfo->m_pfnFilterDllMsg = (FILTER_DLL_MSG)GetProcAddress(m_pXiaDanDllInfo->m_hInstance, "FilterXiaDanDllMsg");
		m_pXiaDanDllInfo->m_pfnProcessDllIdle = (PROCESS_DLL_IDLE)GetProcAddress(m_pXiaDanDllInfo->m_hInstance, "ProcessXiaDanDllIdle");
		m_pXiaDanDllInfo->m_pfnGetXiadanDllVersion = (GET_XIADAN_VERSION)GetProcAddress(m_pXiaDanDllInfo->m_hInstance, "GetXiaDanDllVersion");
	}
	if(m_pXiaDanDllInfo->m_pfnCreateWnd)
		(m_pXiaDanDllInfo->m_pfnCreateWnd)(hWnd, bVerify?("@yyy.ini"):("yyy.ini"));
	return TRUE;
}

BOOL DoHexinEXEVerify(HWND hWnd)
{
	char tmpWtPath[MAX_PATH],tmpParam[MAX_PATH];
	GetPrivateProfileString("EXTERN","WTPATH","",tmpWtPath,MAX_PATH-1,g_WSXHStr+g_strYxhjCfg);
	if(!g_bHexinWtOnlyTXMM)
		sprintf(tmpParam,"%X %s -V",hWnd,HomePath);
	else //如果只要校验通讯密码，按如下
		sprintf(tmpParam,"%X %s -V '1'",hWnd,HomePath);
	return ExecExtern(tmpWtPath,hWnd,tmpParam);	
}

void DoTDXDLLWt(HWND hWnd,long newflag,long jymmflag) //newflag:0表示状态相关,1表示状态无关
{
	if(!m_pTDXDllInfo)
	{
		m_pTDXDllInfo = new CTDXDllInfo;
		memset(m_pTDXDllInfo, 0, sizeof(CTDXDllInfo));
		m_pTDXDllInfo->m_hInstance = AfxLoadLibrary("TdxTradeUser.dll");
		if(m_pTDXDllInfo->m_hInstance == NULL)
		{
			TDX_MessageBox(hWnd,"不能发现交易模块!",MB_OK|MB_ICONERROR);
			return;
		}
		m_pTDXDllInfo->m_pInit = (TDXJY_INIT)GetProcAddress(m_pTDXDllInfo->m_hInstance, "Init");
		m_pTDXDllInfo->m_pDoTDXJy = (TDXJY_DOTDXJY)GetProcAddress(m_pTDXDllInfo->m_hInstance, "DoTDXJy");
		if(m_pTDXDllInfo->m_pInit)
			(m_pTDXDllInfo->m_pInit)(HomePath,g_WSXHStr+g_strYxhjCfg,g_WSXHStr+"newmodem.ini");
	}
	if(m_pTDXDllInfo && m_pTDXDllInfo->m_pDoTDXJy)
		(m_pTDXDllInfo->m_pDoTDXJy)(hWnd,newflag,jymmflag);
	return;
}

BOOL DoTdxExeWt(HWND hWnd)
{
	char tmpWtPath[MAX_PATH],tmpParam[MAX_PATH];
	sprintf(tmpWtPath,"%s%s",HomePath,g_strTdxVerifyFileName);
	sprintf(tmpParam,"-V%X",hWnd);
	return ExecExtern(tmpWtPath,hWnd,tmpParam);	
}

void ExitHexinAndTdxJy()
{
	if(m_pXiaDanDllInfo)
	{
		if(m_pXiaDanDllInfo->m_hInstance)	AfxFreeLibrary(m_pXiaDanDllInfo->m_hInstance);
		if(m_pXiaDanDllInfo->m_hDLL2)		AfxFreeLibrary(m_pXiaDanDllInfo->m_hDLL2);
		if(m_pXiaDanDllInfo->m_hDLL3)		AfxFreeLibrary(m_pXiaDanDllInfo->m_hDLL3);
		TDEL(m_pXiaDanDllInfo);
	}
	if(m_pTDXDllInfo)
	{
		if(m_pTDXDllInfo->m_hInstance)	AfxFreeLibrary(m_pTDXDllInfo->m_hInstance);
		TDEL(m_pTDXDllInfo);
	}
}

//得到加密的日期值
long GetExploreExeDate()
{
	CString tmpStr;
	char path[256];
	GetWindowsDirectory(path,255);
	tmpStr = path;
	if(tmpStr.Right(1) != "\\") tmpStr+="\\";
	try
	{
		CFileStatus FileStatus;
		if( CFile::GetStatus(tmpStr+"EXPLORER.EXE",FileStatus) )
		{	
			time_t tTime=FileStatus.m_mtime.GetTime();
			struct tm * ptm=localtime(&tTime);
			if(ptm==NULL)
				return 0;
			return (ptm->tm_year+1900)*10000+(ptm->tm_mon+1)*100+ptm->tm_mday;
		}
	}
	catch(...)
	{
	}
	return 0;
}

BOOL LocalJyVerify()
{
	BOOL bRet = FALSE;
	CFile theFile;
	if(theFile.Open(g_WSXHStr+"jyverify.dat",CFile::modeRead|CFile::shareDenyNone))
	{
		char *buf = new char[sizeof(struct LocalVerifyInfo)];
		theFile.Read(buf,sizeof(struct LocalVerifyInfo));
		char *Password = "l2wl_verify0";
		CBlowfish  fish((BYTE *)Password,12);
		fish.Decrypt((BYTE *)buf,sizeof(struct LocalVerifyInfo));
		struct LocalVerifyInfo *pInfo = (struct LocalVerifyInfo *)buf;
		pInfo->homepath[98]=0;
		pInfo->ipaddress[25]=0;

		CTime nowTime=LongToTime(g_iToday);
		CTime lastverifyTime=LongToTime(pInfo->lastverifydate);
		CTimeSpan timeSpan=nowTime-lastverifyTime;
		if(GetExploreExeDate()==pInfo->exploredate && timeSpan.GetDays()<g_VipVerify_Info.ReVerifySpan && stricmp(HomePath,pInfo->homepath)==0)
			bRet=TRUE;
		delete buf;
		theFile.Close();
	}
	return bRet;
}

BOOL MakeJyVerifyFile(char *ipaddress)
{
	CFile theFile;
	if(theFile.Open(g_WSXHStr+"jyverify.dat",CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone))
	{
		struct LocalVerifyInfo verifyInfo;
		memset(&verifyInfo,0,sizeof(struct LocalVerifyInfo));
		verifyInfo.exploredate=GetExploreExeDate();
		verifyInfo.lastverifydate=g_iToday;
		strncpy(verifyInfo.homepath,HomePath,98);
		verifyInfo.homepath[98]=0;
		strncpy(verifyInfo.ipaddress,ipaddress,25);
		verifyInfo.ipaddress[25]=0;
		verifyInfo.hqhostnum=g_HQHost.HostNum;

		char *Password = "l2wl_verify0";
		CBlowfish  fish((BYTE *)Password,12);
		fish.Encrypt((BYTE *)(&verifyInfo),sizeof(struct LocalVerifyInfo));
		
		theFile.Write((BYTE *)(&verifyInfo),sizeof(struct LocalVerifyInfo));
		theFile.Close();
		return TRUE;
	}
	return FALSE;
}

BOOL Lhzq_JyVerify()
{
	long lasttime=GetPrivateProfileInt("user","validatetime",20000101,HomePath+"buser.ini");
	if(lasttime<20000101 || lasttime>21000101) 
		return FALSE;
	CTime nowTime=LongToTime(g_iToday);
	CTime lastverifyTime=LongToTime(lasttime);
	CTimeSpan timeSpan=nowTime-lastverifyTime;
	if(timeSpan.GetDays()<20)
		return TRUE;
	return FALSE;
}

BOOL CheckUserViaURL(const char *pUrl,char *Errmsg)
{
	BOOL bRet = FALSE;
	strcpy(Errmsg,"验证失败!");
	CInternetSession	sess;
	CHttpFile *fileGet = (CHttpFile*)sess.OpenURL(pUrl);
	if(fileGet)
	{
		CString strSentence;
		CString strformat,tempstr;
		DWORD	dwStatus;
		DWORD	dwBuffLen = sizeof(dwStatus);
		BOOL	bSuccess = fileGet->QueryInfo(HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwBuffLen);
		
		if( bSuccess && dwStatus>=200 && dwStatus<300 )
		{ 
			fileGet->ReadString(strSentence);
			if(strSentence.Find("<result>1</result>")>=0)
				bRet=TRUE;
		}
		else 
		{
			strSentence.Format("验证失败!打开验证页出错,错误码:%d", dwStatus);
			strcpy(Errmsg,strSentence);
		}
		fileGet->Close();
		delete fileGet;
	}
	else
		strcpy(Errmsg,"验证失败!不能打开验证页");
	sess.Close();
	return bRet;
}
