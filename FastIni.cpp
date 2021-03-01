#include "stdafx.h"
#include "FastIni.h"

CFastINI::CFastINI(int iCallType /*=0*/ )
{
	m_iCallType = iCallType;
	m_bHasModFlag = FALSE;
	m_pAppMap = NULL;
}

CFastINI::~CFastINI()
{
	if( m_bHasModFlag && 1==m_iCallType )
	{
		SaveFile();
	}
	RemoveMAP();
}

BOOL CFastINI::LoadIniFile(LPCTSTR lpFileName)
{
	if(0==m_iCallType) 
		return FALSE;
	m_strFileName = lpFileName;
	if(m_pAppMap)
		RemoveMAP();
	m_pAppMap = new CMapStringToPtr;
	if( NULL==m_pAppMap )
		return FALSE;
	m_pAppMap->InitHashTable(100);
	
	TRY
	{
		CFile	fINI;
		if(!fINI.Open( lpFileName, CFile::modeRead|CFile::shareDenyNone ))
			return FALSE;
		DWORD dwFileLen = fINI.GetLength();
		BYTE*	lpBuffer = new BYTE[dwFileLen+10];
		if( NULL==lpBuffer )
		{
			RemoveMAP();
			return FALSE;
		}
		dwFileLen=fINI.ReadHuge( (void*)lpBuffer, dwFileLen );
		lpBuffer[dwFileLen]='\r';
		lpBuffer[dwFileLen+1]='\n';
		lpBuffer[dwFileLen+2]=0;
		fINI.Close();

		CString	csLine, csKey, csAppName, csKeyName, csKeyValue;
		csLine = csKey = csAppName = csKeyName = csKeyValue = _T("");
		int	i=0;
		CMapStringToString*	pKeyMap = NULL;
		while( lpBuffer[i] )
		{
			if( '\n'==lpBuffer[i] || '\0'==lpBuffer[i] ||
				('\r'==lpBuffer[i] && '\n'==lpBuffer[i+1]) )
			{
				csLine.TrimLeft();csLine.TrimRight();
				if( csLine!=_T("") )
				{
					int iIdx1 = csLine.Find('[');
					int iIdx2 = csLine.Find(']');
					int iIdx3 = csLine.Find('=');
					if( iIdx1==0 && iIdx2==(csLine.GetLength()-1) )
					{
						csAppName = csLine.Mid(iIdx1+1,iIdx2-iIdx1-1);
						if( csAppName!=_T("") )
						{
							csAppName.MakeUpper();
							pKeyMap = NULL;
							pKeyMap = new CMapStringToString;
							if( pKeyMap )
							{
								pKeyMap->InitHashTable(1000);
								m_pAppMap->SetAt( csAppName, pKeyMap );
							}
						}
					}
					else if( iIdx3>0 )
					{
						if( pKeyMap )
						{
							csKeyName = csLine.Left( iIdx3 );
							if( csKeyName!=_T("") )
							{
								csKeyName.TrimRight();csKeyName.MakeUpper();
								if( (csLine.GetLength()-iIdx3-1)>0 )
								{
									csKeyValue = csLine.Right( csLine.GetLength()-iIdx3-1 );
									csKeyValue.TrimLeft();
								}
								else
									csKeyValue = _T("");
								csKey.Format( "%s", csKeyName );
								pKeyMap->SetAt( csKey, csKeyValue );
							}
						}
					}
					csLine = _T("");
				}
				if( 0==lpBuffer[i] || '\n'==lpBuffer[i] )
					i++;
				else
					i+=2;
			}
			else
			{
				csLine += lpBuffer[i];
				i++;
			}
		}
		delete lpBuffer;
	}
	CATCH( CFileException, e )
	{
		RemoveMAP();
		return FALSE;
	}
	END_CATCH

	return TRUE;
}

BOOL CFastINI::SaveFile()
{
	CString csAppKey, csKeyName, csKeyValue;
	CString	csText = "";
	CMapStringToString*	pKeyMap;
	POSITION pos = m_pAppMap->GetStartPosition();
	while ( pos )
	{
		pKeyMap = NULL;
		m_pAppMap->GetNextAssoc( pos, csAppKey, (void*&)pKeyMap );
		if( pKeyMap )
		{
			//	写入域名
			if( csText!="" )
				csText +="\r\n";
			csText += "[" + csAppKey + "]\r\n";
			//	搜索域中所有键并写入
			POSITION posKey = pKeyMap->GetStartPosition();
			while( posKey )
			{
				pKeyMap->GetNextAssoc( posKey, csKeyName, csKeyValue );
				csText += csKeyName + "=" + csKeyValue + "\r\n";
			}
		}
	}
	TRY
	{
		CFile	fIni;
		if(!fIni.Open( m_strFileName, CFile::modeCreate|CFile::modeWrite
			|CFile::shareDenyNone ))
			return FALSE;
		fIni.WriteHuge( (void*)(LPCSTR)csText, csText.GetLength() );
		fIni.Close();
	}
	CATCH( CFileException, e )
	{
		return FALSE;
	}
	END_CATCH
	
	m_bHasModFlag = FALSE;
	
	return TRUE;
}

void CFastINI::RemoveMAP()
{
	if(!m_pAppMap) return;
	POSITION pos=m_pAppMap->GetStartPosition();
	while(pos)
	{
		CMapStringToString* pStrMap;
		CString str;
		m_pAppMap->GetNextAssoc(pos,str,(void*&)pStrMap);
		if(pStrMap!=NULL)
		{
			pStrMap->RemoveAll();
			delete pStrMap;
		}
	}
	m_pAppMap->RemoveAll();
	delete m_pAppMap;
	m_pAppMap=NULL;
}

DWORD CFastINI::DefaultStringValue( LPTSTR lpReturnedString, DWORD nSize, LPCTSTR lpDefault )
{
	if(!lpDefault)
	{
		lpReturnedString[0]=0;
		return 0;
	}
	DWORD	dwSize;
	if( strlen(lpDefault)>=nSize )
	{
		strncpy( lpReturnedString, lpDefault, nSize-1 );
		lpReturnedString[nSize-1] = 0;
		dwSize = nSize-1;
		return dwSize;
	}
	else
	{
		strcpy( lpReturnedString, lpDefault );
		lpReturnedString[strlen(lpDefault)] = 0;
		dwSize = strlen(lpDefault);
		return dwSize;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////

DWORD CFastINI::GetKeyValueString( LPCTSTR lpKeyValueText, CString& csKeyValue )
{
	CString	csKeyText = lpKeyValueText;
	csKeyValue = lpKeyValueText;
	
	int iIdx = csKeyText.Find( "//" );
	TCHAR	cBefore;
	if( iIdx>=0 )
	{
		cBefore = csKeyText.GetAt( iIdx-1 );
		if( ' '==cBefore || '\t'==cBefore )
		{
			csKeyValue = csKeyText.Left( iIdx );
			csKeyValue.TrimRight();
		}
	}
	return csKeyValue.GetLength();
}

int CFastINI::GetKeyValueInt( LPCTSTR lpKeyText )
{
	CString	csKeyText = lpKeyText;
	int iIdx = csKeyText.Find( "//" );
	if( iIdx>0 )
	{
		CString cskeyValue = csKeyText.Left( iIdx );
		cskeyValue.TrimRight();
		csKeyText = cskeyValue;
	}
	return atoi(csKeyText);
}

int CFastINI::GetPrivateProfileInt(
								   LPCTSTR lpAppName,  // section name
								   LPCTSTR lpKeyName,  // key name
								   INT nDefault,       // return value if key name not found
								   LPCTSTR lpFileName  // initialization file name
								   )
{
	if( m_iCallType==0 )
		return ::GetPrivateProfileInt( lpAppName, lpKeyName, nDefault, lpFileName );
	else if(m_pAppMap)
	{
		CString csAppKey, csKeyName, csKeyValue;
		csAppKey = lpAppName;	csAppKey.MakeUpper();
		csKeyName = lpKeyName;	csKeyName.MakeUpper();
		CMapStringToString*	pKeyMap = NULL;
		if( m_pAppMap->Lookup( csAppKey, (void*&)pKeyMap ) )
		{
			if( pKeyMap )
			{
				if( pKeyMap->Lookup( csKeyName, csKeyValue ) )
				{
					if( csKeyValue!=_T("") )
						return GetKeyValueInt( csKeyValue );
				}
			}
		}
	}
	return nDefault;
}

DWORD CFastINI::GetPrivateProfileString(
										LPCTSTR lpAppName,        // section name
										LPCTSTR lpKeyName,        // key name
										LPCTSTR lpDefault,        // default string
										LPTSTR lpReturnedString,  // destination buffer
										DWORD nSize,              // size of destination buffer
										LPCTSTR lpFileName        // initialization file name
										)
{
	DWORD	dwSize = 0;
	if( m_iCallType==0 )
		return ::GetPrivateProfileString( lpAppName, lpKeyName, lpDefault
		, lpReturnedString, nSize, lpFileName );
	else if(m_pAppMap)
	{
		CString csAppKey, csKeyName, csKeyValue;
		csAppKey = lpAppName;
		csAppKey.TrimLeft();csKeyName.TrimRight();csAppKey.MakeUpper();
		csKeyName = lpKeyName;
		csKeyName.TrimLeft();csKeyName.TrimRight();csKeyName.MakeUpper();
		if( _T("")==csAppKey || _T("")==csKeyName )
			return 0;
		CMapStringToString*	pKeyMap = NULL;
		if( m_pAppMap->Lookup( csAppKey, (void*&)pKeyMap ) )
		{
			if( pKeyMap )
			{
				if( pKeyMap->Lookup( csKeyName, csKeyValue ) )
				{
					if( csKeyValue==_T("") )
					{
						lpReturnedString[0] = 0;
						return 0;
					}
					else
					{
						CString csTrueKeyValue;
						DWORD dwKeyValueLen = GetKeyValueString( csKeyValue, csTrueKeyValue );
						if( dwKeyValueLen>=nSize )
						{
							strncpy( lpReturnedString, csTrueKeyValue.LockBuffer(), nSize-1 );
							csTrueKeyValue.UnlockBuffer();
							lpReturnedString[nSize-1] = 0;
							return nSize-1;
						}
						else
						{
							strcpy( lpReturnedString, csTrueKeyValue.LockBuffer() );
							csTrueKeyValue.UnlockBuffer();
							lpReturnedString[csTrueKeyValue.GetLength()] = 0;
							return dwKeyValueLen;
						}
					}
				}
			}
		}
	}
	return DefaultStringValue( lpReturnedString, nSize, lpDefault );
}

DWORD CFastINI::TDX_GetPrivateProfileString(
										LPCTSTR lpAppName,        // section name
										LPCTSTR lpKeyName,        // key name
										LPCTSTR lpDefault,        // default string
										LPTSTR lpReturnedString,  // destination buffer
										DWORD nSize,              // size of destination buffer
										LPCTSTR lpFileName        // initialization file name
										)
{
	DWORD nRet=CFastINI::GetPrivateProfileString(lpAppName,lpKeyName,lpDefault,lpReturnedString,nSize,lpFileName);
	_FL(lpReturnedString);
	return nRet;	
}

BOOL CFastINI::GetPrivateProfileStruct(
										LPCSTR lpszSection,
										LPCSTR lpszKey,
										LPVOID   lpStruct,
										UINT     uSizeStruct,
										LPCSTR szFile
										)
{
	if( m_iCallType==0 )
		return ::GetPrivateProfileStruct( lpszSection, lpszKey, lpStruct, uSizeStruct,szFile );
	else if(m_pAppMap)
	{
		if(m_bHasModFlag)
			SaveFile();
		return ::GetPrivateProfileStruct( lpszSection, lpszKey, lpStruct, uSizeStruct,szFile );
	}
	return FALSE;	
}

int CFastINI::GetPrivateProfileChar(LPCSTR lpAppName,LPCSTR lpKeyName,char nDefault,LPCSTR lpFileName)
{
	char tmpChar[20];
	char DefaultChar[2];
	DefaultChar[0]=nDefault;
	DefaultChar[1]=0;
	CFastINI::GetPrivateProfileString(lpAppName,lpKeyName,DefaultChar,tmpChar,19,lpFileName);
	AllTrim(tmpChar);
	return tmpChar[0];	
}

BOOL CFastINI::WritePrivateProfileString(
										 LPCTSTR lpAppName,  // section name
										 LPCTSTR lpKeyName,  // key name
										 LPCTSTR lpString,   // string to add
										 LPCTSTR lpFileName  // initialization file
										 )
{
	if( m_iCallType==0 )
		return ::WritePrivateProfileString( lpAppName, lpKeyName, lpString, lpFileName );
	else if(m_pAppMap)
	{
		CString csAppKey, csKeyName, csKeyValue;
		csAppKey = lpAppName;
		csAppKey.TrimLeft();csKeyName.TrimRight();csAppKey.MakeUpper();
		csKeyName = lpKeyName;
		csKeyName.TrimLeft();csKeyName.TrimRight();csKeyName.MakeUpper();
		if( _T("")==csAppKey || _T("")==csKeyName )
			return FALSE;
		csKeyValue = lpString;
		CMapStringToString*	pKeyMap = NULL;
		if( m_pAppMap->Lookup( csAppKey, (void*&)pKeyMap ) )
		{
			CString	csKeyText;
			if( pKeyMap->Lookup( csKeyName, csKeyText ) )
			{
				int iIdx = csKeyText.Find( "//" );
				TCHAR	cBefore;
				if( iIdx>=0 )
				{
					CString	csNote = "";
					cBefore = csKeyText.GetAt( iIdx-1 );
					int iCount = 0;
					while( ' '==cBefore || '\t'==cBefore )
					{
						csNote = cBefore + csNote;
						iCount++;
						cBefore = csKeyText.GetAt( iIdx-1-iCount );
					}
					if( csNote.GetLength()>0 )
					{
						csNote += csKeyText.Right(csKeyText.GetLength()-iIdx);
						csKeyValue += csNote;
					}
				}
			}
			pKeyMap->SetAt( csKeyName, csKeyValue );
			m_bHasModFlag = TRUE;
			return TRUE;
		}
		else
		{
			pKeyMap = new CMapStringToString;
			if( pKeyMap )
			{
				m_pAppMap->SetAt( csAppKey, (void*)pKeyMap );
				pKeyMap->SetAt( csKeyName, csKeyValue );
				m_bHasModFlag = TRUE;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CFastINI::WritePrivateProfileStruct(
											LPCSTR lpszSection,
											LPCSTR lpszKey,
											LPVOID   lpStruct,
											UINT     uSizeStruct,
											LPCSTR szFile
										)
{
	if( m_iCallType==0 )
		return ::WritePrivateProfileStruct( lpszSection, lpszKey, lpStruct, uSizeStruct,szFile );
	else if(m_pAppMap)
	{
		if(m_bHasModFlag)
			SaveFile();
		BOOL bRet=::WritePrivateProfileStruct( lpszSection, lpszKey, lpStruct, uSizeStruct,szFile );
		if(bRet)
			bRet=LoadIniFile(szFile);
		return bRet;
	}
	return FALSE;
}
