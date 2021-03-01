#ifndef VTOOLS_H_
#define VTOOLS_H_

#ifdef	TTOOLS_EXPORT
	#define TTOOLSDLL_API extern "C" __declspec(dllexport)
#else
	#define TTOOLSDLL_API extern "C" __declspec(dllimport)
#endif

#define UM_SOFTKEY_SETINDEX		WM_USER+0x2414
#define UM_SOFTKEY_NOTIFY		WM_USER+0x2415
#define SOFTKEY_VK_CLOSE		98
#define SOFTKEY_VK_RETURN		99

TTOOLSDLL_API char *MakeBuildNo(char *DateStr,char *TimeStr);
TTOOLSDLL_API COLORREF HLS_TRANSFORM(COLORREF rgb, int percent_L, int percent_S);

TTOOLSDLL_API void SoftKey_Close();
TTOOLSDLL_API HWND SoftKey_Switch(HWND hParentWnd,BOOL bStickCtrl,HWND hCtrlHwnd,CRect rcCtrl);

TTOOLSDLL_API void MD5_String(char *p,char *pRet/*32*/);
TTOOLSDLL_API void MD5_Buffer(BYTE *p,int len,char *pRet/*32*/);
TTOOLSDLL_API void MD5BIN_Buffer(unsigned char* lpszBuf,unsigned long nLen,BYTE *m_szMD5/*16*/);

TTOOLSDLL_API char* OpenTdxIni(const char* pFilePath,const char* pCipher);
TTOOLSDLL_API DWORD	GetTDXProfileInt(LPCSTR lpApp,LPCSTR lpKey,int nDefault,LPSTR IniData);
TTOOLSDLL_API DWORD	GetTDXProfileString(LPCSTR lpApp,LPCSTR lpKey,LPCSTR lpDefault,LPSTR pData,DWORD DataSize,LPSTR IniData);
TTOOLSDLL_API void  CloseTdxIni(char* pIniBuf);
TTOOLSDLL_API BOOL  SaveTdxIni(const char* pExistFilePath,const char* pNewFilePath,const char* pCipher);

#endif