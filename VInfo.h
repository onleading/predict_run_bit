#ifndef VINFO_H_
#define VINFO_H_

#ifdef	TINFO_EXPORT
	#define TINFODLL_API extern "C" __declspec(dllexport)
#else
	#define TINFODLL_API extern "C" __declspec(dllimport)
#endif

#pragma pack(1)

struct PopupWnd_Info
{
	COLORREF rgb1;
	COLORREF rgb2;
	COLORREF txtbk;
	COLORREF txtcolor;
	short	 cx;
	short	 cy;
	short	 stayseconds;
	char	 wndtype;
	char	 hasverifybtn;
	char	 verifyinfo[51];
	char	 unused[51];
};

#define UM_VERIFY_MESSAGE		WM_USER+0x2421	//������֤����Ϣ
#define UM_SEARCH_GPGRADE		WM_USER+5018	//����ĳֻ��Ʊ������

//���ݻص������ķ�ʽ
typedef long(CALLBACK * PDATAIOFUNC)(char * Code,short nSetCode,short DataType,void * pData/*�ڲ�����*/,short nDataNum,NTime,NTime,BYTE nTQ,unsigned long);
//ʵ�ֹ����л��Ļص�����
typedef long(CALLBACK * PSWITCHINFOFUNC)(char * Code,short nSetCode,char *buf,long buflen,short FuncType,short Option,unsigned long);

//������Ϣ��(����ʹ��)
TINFODLL_API void	I_PopupWnd(CWnd *pMainWnd,struct PopupWnd_Info Info,const char *Title,const char *Txt,const char *Url);
//��ʼ������
TINFODLL_API BOOL	I_Init_Environment(const char *PubDir,const char *PriDir,const char *CfgFile,long reserved);
//ע��������������ص�����
TINFODLL_API void	I_RegisterCallBack(PDATAIOFUNC pDatafn,PSWITCHINFOFUNC pSwitchfn);
//�ж��Ƿ��л�������������������
TINFODLL_API BOOL	I_HasJGData(const char *Code,short setcode,long reserved);
//����ĳֻ��Ʊ�Ļ������ݴ���
TINFODLL_API void	I_ShowJGDialog(long nGPID,long reserved);
//�������ܴ���
TINFODLL_API void	I_ShowFunc(long nType,long reserved);
//������Ϣ
TINFODLL_API int	I_ProcessMsg(HWND hWnd,UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
//����ʱ����
TINFODLL_API int	I_Exit();

#pragma pack()
#endif
