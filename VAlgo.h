#ifndef VALGO_H_
#define VALGO_H_

#ifdef	TALGO_EXPORT
	#define TALGODLL_API extern "C" __declspec(dllexport)
#else
	#define TALGODLL_API extern "C" __declspec(dllimport)
#endif

//得到拼音串
TALGODLL_API char *GetPYStr(const char *str,int nMax);

//差分转换算法
TALGODLL_API short BufToDouble ( double &ftmp, char * buf );
TALGODLL_API short DoubleToBuf ( double ftmp, char * buf );

//对窗口进行安全保护
TALGODLL_API int  SCT_EnableProtect(HWND hwndlg);
TALGODLL_API int  SCT_DisableProtect(HWND hwndlg);

//窗口元素XP化
TALGODLL_API BOOL ClassXP(HWND hWnd, BOOL bEnable);

//Base64编码
TALGODLL_API BOOL Base64Encode(CString rawString,CString & base64str);

//将列表内容存为一个格式化文件
TALGODLL_API BOOL ListCtrl2Txt(CListCtrl * pList,const char *txtfile);

//将BMP句柄存为一个文件
TALGODLL_API BOOL SaveToBMPFile(HBITMAP bitmap,LPCTSTR szFileName);

//得到本机的网卡号
TALGODLL_API void GetNetCardStr(char *netcardstr);

#endif