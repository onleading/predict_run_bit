#ifndef HEXIN_H_
#define HEXIN_H_

////////////////////////////////////////////////////////////////////////////
//核新的校验

typedef BOOL (WINAPI  *CREATE_XIADAN_WND)(HWND hwndParent, const char *pszProfile);
typedef int (WINAPI  *GET_XIADAN_WNDNUMBER)();
typedef BOOL (WINAPI  *FILTER_DLL_MSG)(LPMSG lpMsg);
typedef void (WINAPI  *PROCESS_DLL_IDLE)();
typedef float (WINAPI  *GET_XIADAN_VERSION)(void*, int);

typedef BOOL (CALLBACK* LPFNDLLFUNC1)(HWND hwndParent, const char *pszProfile);
typedef void (WINAPI *SessionCloseHook)(long lCommID);
typedef BOOL (CALLBACK* LPFNDLLFUNC2)(const TCHAR* pszProfile, SessionCloseHook pHook = NULL);
typedef BOOL (WINAPI *LPFNDLLFUNC3)(char FAR *pszID, char FAR *pszName, BOOL bRestart);

const int WM_CHECK_HEXINWT = WM_USER + 221;

struct CXiaDanDllInfo
{
	HINSTANCE m_hInstance;
	HINSTANCE m_hDLL3;
	HINSTANCE m_hDLL2;
	CREATE_XIADAN_WND m_pfnCreateWnd;
	GET_XIADAN_WNDNUMBER m_pfnGetWndNumber;
	FILTER_DLL_MSG m_pfnFilterDllMsg;
	PROCESS_DLL_IDLE m_pfnProcessDllIdle;
	GET_XIADAN_VERSION  m_pfnGetXiadanDllVersion;
};

extern CXiaDanDllInfo*	m_pXiaDanDllInfo;
extern BOOL DoHexinDLLWt(HWND hWnd,BOOL bVerify=FALSE);
extern BOOL DoHexinEXEVerify(HWND hWnd);
////////////////////////////////////////////////////////////////////////////
//通达信的校验

typedef void (* TDXJY_INIT)(const char *homepath,const char *cfgfile,const char *priIni);
typedef void (* TDXJY_DOTDXJY)(HWND hwnd,long newflag,long jymmflag); //不要CALLBACK和WINAPI

const int WM_CHECK_TDXWT = WM_USER + 222;
const int ID_MOREHEXIN_MSG = 30447;

struct CTDXDllInfo
{
	HINSTANCE m_hInstance;
	TDXJY_INIT m_pInit;
	TDXJY_DOTDXJY m_pDoTDXJy;
};

extern CTDXDllInfo*	m_pTDXDllInfo;
extern void DoTDXDLLWt(HWND hWnd,long newflag=1,long jymmflag=1);
extern BOOL DoTdxExeWt(HWND hWnd);

extern void ExitHexinAndTdxJy();

//本地验证文件
struct LocalVerifyInfo
{
	long exploredate;
	char homepath[100];
	char ipaddress[26];
	long lastverifydate;
	char hqhostnum;
	char unused[1];
};

extern BOOL LocalJyVerify();
extern BOOL MakeJyVerifyFile(char *ipaddress);
extern BOOL Lhzq_JyVerify();
extern BOOL CheckUserViaURL(const char *pUrl,char *Errmsg);

#endif
