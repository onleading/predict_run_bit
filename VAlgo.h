#ifndef VALGO_H_
#define VALGO_H_

#ifdef	TALGO_EXPORT
	#define TALGODLL_API extern "C" __declspec(dllexport)
#else
	#define TALGODLL_API extern "C" __declspec(dllimport)
#endif

//�õ�ƴ����
TALGODLL_API char *GetPYStr(const char *str,int nMax);

//���ת���㷨
TALGODLL_API short BufToDouble ( double &ftmp, char * buf );
TALGODLL_API short DoubleToBuf ( double ftmp, char * buf );

//�Դ��ڽ��а�ȫ����
TALGODLL_API int  SCT_EnableProtect(HWND hwndlg);
TALGODLL_API int  SCT_DisableProtect(HWND hwndlg);

//����Ԫ��XP��
TALGODLL_API BOOL ClassXP(HWND hWnd, BOOL bEnable);

//Base64����
TALGODLL_API BOOL Base64Encode(CString rawString,CString & base64str);

//���б����ݴ�Ϊһ����ʽ���ļ�
TALGODLL_API BOOL ListCtrl2Txt(CListCtrl * pList,const char *txtfile);

//��BMP�����Ϊһ���ļ�
TALGODLL_API BOOL SaveToBMPFile(HBITMAP bitmap,LPCTSTR szFileName);

//�õ�������������
TALGODLL_API void GetNetCardStr(char *netcardstr);

#endif