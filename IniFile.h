// IniFile.h: interface for the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIFILE_H__B074750E_FFAA_4FDB_857E_F245478E1FF3__INCLUDED_)
#define AFX_INIFILE_H__B074750E_FFAA_4FDB_857E_F245478E1FF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIniFile  
{
public:
	void PutString(LPCSTR pdata,LPCSTR pKeyName,LPCSTR pAppName=NULL);
	void GetString(CString &strData,LPCSTR pKeyName,LPCSTR def=NULL,LPCSTR pAppName=NULL);
 	void PutInt(int data,LPCSTR pKeyName,LPCSTR pAppName=NULL);
	int GetInt(LPCSTR pKeyName,int def=0,LPCSTR pAppName=NULL);
	void SetAppName(CString strAppName);
	void SetFileName(CString strFileName);
	CIniFile();
	CIniFile(CString strFileName);
	virtual ~CIniFile();
private:
	CString m_strFileName;
	CString m_strAppName;

};

#endif // !defined(AFX_INIFILE_H__B074750E_FFAA_4FDB_857E_F245478E1FF3__INCLUDED_)
