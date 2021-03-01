/**********************************************************************
	FastINI类

	配置文件读取及修改(一次性装入，一次性写入，避免频繁访问配置文件的弊端)
	
**********************************************************************/

#ifndef _FAST_INI_
#define _FAST_INI_

class CFastINI  
{
private:
	//	域映射
	CMapStringToPtr* m_pAppMap;
	//	调用类型: 0,以系统API方式调用; 1,以通达信方式调用
	int m_iCallType;
	//	文件名（包括路径）
	CString	m_strFileName;
	//	是否有某些信息被修改
	BOOL	m_bHasModFlag;

	//  处理缺省值
	DWORD DefaultStringValue( LPTSTR lpReturnedString, DWORD nSize, LPCTSTR lpDefault );
	//	取删除了注释以后实际的键值(目前仅支持本行内以"//"字符串开头的注释)
	DWORD GetKeyValueString( LPCTSTR lpKeyValueText, CString& csKeyValue );

	//	取得实际键值(忽略注释)
	int GetKeyValueInt( LPCTSTR csKeyText );
	//	删除Map表
	void  RemoveMAP();
public:
	CFastINI( int iCallType=0 );
	virtual ~CFastINI();

	//	载入配置文件(一次性将该文件所有数据读入，并设置好映射关系)
	BOOL LoadIniFile( LPCTSTR lpFileName );
	//	保存配置信息
	BOOL SaveFile();
	//	读字符串型配置信息
	DWORD GetPrivateProfileString(
		LPCTSTR lpAppName,        // section name
		LPCTSTR lpKeyName,        // key name
		LPCTSTR lpDefault,        // default string
		LPTSTR lpReturnedString,  // destination buffer
		DWORD nSize,              // size of destination buffer
		LPCTSTR lpFileName        // initialization file name
		);

	//	读字符串型配置信息
	DWORD TDX_GetPrivateProfileString(
		LPCTSTR lpAppName,        // section name
		LPCTSTR lpKeyName,        // key name
		LPCTSTR lpDefault,        // default string
		LPTSTR lpReturnedString,  // destination buffer
		DWORD nSize,              // size of destination buffer
		LPCTSTR lpFileName        // initialization file name
		);
	// 读一字符信息
	int GetPrivateProfileChar(
		LPCSTR lpAppName,
		LPCSTR lpKeyName,
		char nDefault,
		LPCSTR lpFileName);
	//	读整形配置信息
	int GetPrivateProfileInt(
		LPCTSTR lpAppName,  // section name
		LPCTSTR lpKeyName,  // key name
		INT nDefault,       // return value if key name not found
		LPCTSTR lpFileName  // initialization file name
		);
	//	读结构型配置信息
	BOOL GetPrivateProfileStruct(
		LPCSTR lpszSection,
		LPCSTR lpszKey,
		LPVOID   lpStruct,
		UINT     uSizeStruct,
		LPCSTR szFile
		);
	//	写配置信息
	BOOL WritePrivateProfileString(
		LPCTSTR lpAppName,  // section name
		LPCTSTR lpKeyName,  // key name
		LPCTSTR lpString,   // string to add
		LPCTSTR lpFileName  // initialization file
		);
	//	写结构型配置信息
	BOOL WritePrivateProfileStruct(
		LPCSTR lpszSection,
		LPCSTR lpszKey,
		LPVOID   lpStruct,
		UINT     uSizeStruct,
		LPCSTR szFile
		);
};

#endif
