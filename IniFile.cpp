// IniFile.cpp: implementation of the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IniFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIniFile::CIniFile()
{

}

CIniFile::CIniFile(CString strFileName)
{
	m_strFileName=strFileName;
}

CIniFile::~CIniFile()
{

}

void CIniFile::SetFileName(CString strFileName)
{
     m_strFileName=strFileName;
}

void CIniFile::SetAppName(CString strAppName)
{
   m_strAppName=strAppName;
}

int CIniFile::GetInt(LPCSTR pKeyName,int def,LPCSTR pAppName)
{
  CString strAppName;
   if(pAppName==NULL)
  {
	  strAppName=m_strAppName;
  }
  else
  {
	  strAppName=pAppName;
  }
  return GetPrivateProfileInt(strAppName,pKeyName,def,m_strFileName); 
}

void CIniFile::PutInt(int data, LPCSTR pKeyName, LPCSTR pAppName)
{
   CString strAppName;
   if(pAppName==NULL)
   {
	  strAppName=m_strAppName;
   }
   else
   {
	  strAppName=pAppName;
   }
   CString strdata;
   strdata.Format("%d",data);
   ::WritePrivateProfileString(strAppName,pKeyName,strdata,m_strFileName);
}

void CIniFile::GetString(CString &strData, LPCSTR pKeyName, LPCSTR def, LPCSTR pAppName)
{
    CString strAppName,strDefStr;
	if(pAppName==NULL)
	{
		strAppName=m_strAppName;
	}
	else
	{
		strAppName=pAppName;
	}
    if(def)
	{
		strDefStr=def;
	}
    GetPrivateProfileString(strAppName,pKeyName,strDefStr,strData.GetBuffer(1000),1000,m_strFileName);
	strData.ReleaseBuffer();
}
 
void CIniFile::PutString(LPCSTR pdata, LPCSTR pKeyName, LPCSTR pAppName)
{
    CString strAppName;
	if(pAppName==NULL)
	{
		strAppName=m_strAppName;
	}
	else
	{
		strAppName=pAppName;
	}
	WritePrivateProfileString(strAppName,pKeyName,pdata,m_strFileName);
}
