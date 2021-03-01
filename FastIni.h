/**********************************************************************
	FastINI��

	�����ļ���ȡ���޸�(һ����װ�룬һ����д�룬����Ƶ�����������ļ��ı׶�)
	
**********************************************************************/

#ifndef _FAST_INI_
#define _FAST_INI_

class CFastINI  
{
private:
	//	��ӳ��
	CMapStringToPtr* m_pAppMap;
	//	��������: 0,��ϵͳAPI��ʽ����; 1,��ͨ���ŷ�ʽ����
	int m_iCallType;
	//	�ļ���������·����
	CString	m_strFileName;
	//	�Ƿ���ĳЩ��Ϣ���޸�
	BOOL	m_bHasModFlag;

	//  ����ȱʡֵ
	DWORD DefaultStringValue( LPTSTR lpReturnedString, DWORD nSize, LPCTSTR lpDefault );
	//	ȡɾ����ע���Ժ�ʵ�ʵļ�ֵ(Ŀǰ��֧�ֱ�������"//"�ַ�����ͷ��ע��)
	DWORD GetKeyValueString( LPCTSTR lpKeyValueText, CString& csKeyValue );

	//	ȡ��ʵ�ʼ�ֵ(����ע��)
	int GetKeyValueInt( LPCTSTR csKeyText );
	//	ɾ��Map��
	void  RemoveMAP();
public:
	CFastINI( int iCallType=0 );
	virtual ~CFastINI();

	//	���������ļ�(һ���Խ����ļ��������ݶ��룬�����ú�ӳ���ϵ)
	BOOL LoadIniFile( LPCTSTR lpFileName );
	//	����������Ϣ
	BOOL SaveFile();
	//	���ַ�����������Ϣ
	DWORD GetPrivateProfileString(
		LPCTSTR lpAppName,        // section name
		LPCTSTR lpKeyName,        // key name
		LPCTSTR lpDefault,        // default string
		LPTSTR lpReturnedString,  // destination buffer
		DWORD nSize,              // size of destination buffer
		LPCTSTR lpFileName        // initialization file name
		);

	//	���ַ�����������Ϣ
	DWORD TDX_GetPrivateProfileString(
		LPCTSTR lpAppName,        // section name
		LPCTSTR lpKeyName,        // key name
		LPCTSTR lpDefault,        // default string
		LPTSTR lpReturnedString,  // destination buffer
		DWORD nSize,              // size of destination buffer
		LPCTSTR lpFileName        // initialization file name
		);
	// ��һ�ַ���Ϣ
	int GetPrivateProfileChar(
		LPCSTR lpAppName,
		LPCSTR lpKeyName,
		char nDefault,
		LPCSTR lpFileName);
	//	������������Ϣ
	int GetPrivateProfileInt(
		LPCTSTR lpAppName,  // section name
		LPCTSTR lpKeyName,  // key name
		INT nDefault,       // return value if key name not found
		LPCTSTR lpFileName  // initialization file name
		);
	//	���ṹ��������Ϣ
	BOOL GetPrivateProfileStruct(
		LPCSTR lpszSection,
		LPCSTR lpszKey,
		LPVOID   lpStruct,
		UINT     uSizeStruct,
		LPCSTR szFile
		);
	//	д������Ϣ
	BOOL WritePrivateProfileString(
		LPCTSTR lpAppName,  // section name
		LPCTSTR lpKeyName,  // key name
		LPCTSTR lpString,   // string to add
		LPCTSTR lpFileName  // initialization file
		);
	//	д�ṹ��������Ϣ
	BOOL WritePrivateProfileStruct(
		LPCSTR lpszSection,
		LPCSTR lpszKey,
		LPVOID   lpStruct,
		UINT     uSizeStruct,
		LPCSTR szFile
		);
};

#endif
