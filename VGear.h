#ifndef VGEAR_H_
#define VGEAR_H_
#pragma pack(1)

#ifdef	TGEAR_EXPORT
	#define TGEARDLL_API extern "C" __declspec(dllexport)
#else
	#define TGEARDLL_API extern "C" __declspec(dllimport)
#endif

#define UM_SOFTKEY_SETINDEX		WM_USER+0x2414
#define UM_SOFTKEY_NOTIFY		WM_USER+0x2415
#define SOFTKEY_VK_CLOSE		98
#define SOFTKEY_VK_RETURN		99

typedef struct
{
	BOOL	m_bProxy;
	char	m_nSockType;
	UINT	m_nProxyPort;
	char	m_strProxyIP[100];
	char	m_strProxyPass[100];
	char	m_strProxyUser[100];

	BOOL	bTdxProxy;
	char	tdxproxynum;
	char	tdxprimary;
	char	tdxdesc[10][50];
	char	tdxip[10][50];
	UINT	tdxport[10];
}PROXYINFO;

struct sock4req1
{
	char VN;
	char CD;
	unsigned short Port;
	unsigned long IPAddr;
	char other[1];
};

struct sock4ans1
{
	char VN;
	char CD;
};

struct sock5req1
{
	char Ver;
	char nMethods;
	char Methods[255];
};

struct sock5ans1
{
	char Ver;
	char Method;
};

struct sock5req2
{
	char Ver;
	char Cmd;
	char Rsv;
	char Atyp;
	char other[1];
};

struct sock5ans2
{
	char Ver;
	char Rep;
	char Rsv;
	char Atyp;
	char other[1];
};

struct authreq
{
	char Ver;
	char Ulen;
	char Name[255];
	char PLen;
	char Pass[255];
};

struct authans
{
	char Ver;
	char Status;
};

//����������Socket
TGEARDLL_API int SendToSocket(SOCKET sock,char *buf,int len);
//��������
TGEARDLL_API BOOL ReceiveBytes(SOCKET sock,char *buff,short len,int timeout=15);
//��ʼ����(֧�ִ����ͨ������ɫͨ��)
TGEARDLL_API BOOL DirectConnect(SOCKET &m_sock,PROXYINFO proxy,const char *hostname,UINT hostport,long &tick1,char *err_msg/*255*/);

//�õ�������Ϣ
TGEARDLL_API char *MakeBuildNo(char *DateStr,char *TimeStr);
//�õ�HLS��ɫ��Ϣ
TGEARDLL_API COLORREF HLS_TRANSFORM(COLORREF rgb, int percent_L, int percent_S);
//�ر������
TGEARDLL_API void SoftKey_Close();
//�л������
TGEARDLL_API HWND SoftKey_Switch(HWND hParentWnd,BOOL bStickCtrl,HWND hCtrlHwnd,CRect rcCtrl);
//MD5����
TGEARDLL_API void MD5_String(char *p,char *pRet/*32*/);
TGEARDLL_API void MD5_Buffer(BYTE *p,int len,char *pRet/*32*/);
TGEARDLL_API void MD5BIN_Buffer(unsigned char* lpszBuf,unsigned long nLen,BYTE *m_szMD5/*16*/);
//��ͨ���ż���INI��ʽ�Ĵ���
TGEARDLL_API char* OpenTdxIni(const char* pFilePath,const char* pCipher);
TGEARDLL_API DWORD	GetTDXProfileInt(LPCSTR lpApp,LPCSTR lpKey,int nDefault,LPSTR IniData);
TGEARDLL_API DWORD	GetTDXProfileString(LPCSTR lpApp,LPCSTR lpKey,LPCSTR lpDefault,LPSTR pData,DWORD DataSize,LPSTR IniData);
TGEARDLL_API void  CloseTdxIni(char* pIniBuf);
TGEARDLL_API BOOL  SaveTdxIni(const char* pExistFilePath,const char* pNewFilePath,const char* pCipher);

//�õ�ƴ����
TGEARDLL_API char *GetPYStr(const char *str,int nMax);
//���ת���㷨
TGEARDLL_API short BufToDouble ( double &ftmp, char * buf );
TGEARDLL_API short DoubleToBuf ( double ftmp, char * buf );
//�Դ��ڽ��а�ȫ����
TGEARDLL_API int  SCT_EnableProtect(HWND hwndlg);
TGEARDLL_API int  SCT_DisableProtect(HWND hwndlg);
//����Ԫ��XP��
TGEARDLL_API BOOL ClassXP(HWND hWnd, BOOL bEnable);
//Base64����
TGEARDLL_API BOOL Base64Encode(CString rawString,CString & base64str);
//���б����ݴ�Ϊһ����ʽ���ļ�
TGEARDLL_API BOOL ListCtrl2Txt(CListCtrl * pList,const char *txtfile);
//��BMP�����Ϊһ���ļ�
TGEARDLL_API BOOL SaveToBMPFile(HBITMAP bitmap,LPCTSTR szFileName);
//�õ�������������
TGEARDLL_API void GetNetCardStr(char *netcardstr);

#pragma pack()
#endif