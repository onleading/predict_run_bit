// NTHelper.h: interface for the CNTHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NTHELPER_H__A4B52B31_5815_11D5_9A1B_00A0CC54942F__INCLUDED_)
#define AFX_NTHELPER_H__A4B52B31_5815_11D5_9A1B_00A0CC54942F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNTHelper  
{
public:
	enum OSType
	{
		WIN_95 = 0,
		WIN_98,
		WIN_NT,
		WIN_2000,
		WIN_ME,
		WIN_XP
	};

	/////////////////////////////////////////////////////////////////////////////
	// Set a registry key value
	// 
	// Note do not need \\ at the beginning of Section Key
	// Example of calling this interface:
	// CNTHelper::SetRegKeyValue( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\ORACLE"), _T("ORACLE_HOME"), strPath );
	static BOOL SetRegKeyValue( HKEY hKey, CString strSection, CString strKey, CString strValue );

	/////////////////////////////////////////////////////////////////////////////
	// Set a registry key value
	//
	// Example of calling this interface:
	// CNTHelper::GetRegKeyValue( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\ORACLE"), _T("ORACLE_HOME"), strPath );
	static BOOL GetRegKeyValue( HKEY hKey, CString strSection, CString strKey, CString& strValue );

	/////////////////////////////////////////////////////////////////////////////
	// Convert a string to floating point
	//
	static double _ttof( CString strValue );

	static OSType GetOSType();
	/////////////////////////////////////////////////////////////////////////////
	// Start File
	//
	// strFile:		File to be opened
	//
	static void StartFile( CString& strFile );

	/////////////////////////////////////////////////////////////////////////////
	// Execute DOS command
	// strCmd:	Command to be executed
	// bSync:	Whether wait for this process or not
	//
	static void ExecuteCmd( CString strCmd, BOOL bSync =FALSE );

	/////////////////////////////////////////////////////////////////////////////
	// Print a string to the default printer, p points to the string buffer, 
	// pSize holds the size information
	//
	static void PrintString(TCHAR *p, DWORD pSize, CString strTitle);

	/////////////////////////////////////////////////////////////////////////////
	// Get system time in the following format
	// YYYYMMDDHH24MISS
	//
	static CString GetSysTimeStamp();

	/////////////////////////////////////////////////////////////////////////////
	// Function to get the system temporary path name
	//
	static CString GetTempFolder();

	/////////////////////////////////////////////////////////////////////////////
	// Return the directory of the current module file
	//
	// Suppose current module file is: E:\Products\DataAnalyst\bin\DataAnalyst.exe
	// It will return E:\Products\DataAnalyst\bin
	//
	static CString GetModuleFileDir();

	/////////////////////////////////////////////////////////////////////////////
	// Popup a window to ask user for file name
	//
	// strFileFilter:		File Filter
	// bSaveOrOpen:			FALSE for Save File, TRUE or Open file caption
	// strFileExtension:	File Name extension
	// strFileName:			The place where the result will be placed
	//
	static BOOL PopWindowForFileName( CString	strFileFilter,
						   BOOL		bSaveOrOpen,
						   CString  strFileExtension,
						   CString&	strFileName
						   );
	static BOOL QueryKeyValue(HKEY hKey, CString strKey, CString& strValue);
	static CString GetNTLangName();
	CNTHelper();
	virtual ~CNTHelper();
};


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CFileDialogEx dialog
//
class CFileDialogEx : public CFileDialog
{
	DECLARE_DYNAMIC(CFileDialogEx)

public:
	CFileDialogEx(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

// Operations
	virtual int DoModal();

protected:
	//{{AFX_MSG(CFileDialogEx)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_NTHELPER_H__A4B52B31_5815_11D5_9A1B_00A0CC54942F__INCLUDED_)
