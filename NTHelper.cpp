// NTHelper.cpp: implementation of the CNTHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NTHelper.h"
#ifdef _UNICODE
	#include "afxpriv.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNTHelper::CNTHelper()
{

}

CNTHelper::~CNTHelper()
{

}

/////////////////////////////////////////////////////////////////////////////
// Get Windows NT Language Name in standard short name such as
// ENU, JPN, etc
//
// return:		Language Name.
//
// Note: if API call failed, it will return default value ENU.
//
CString CNTHelper::GetNTLangName()
{
	TCHAR		strLangName[10];
	
	memset( strLangName, 0, 10 );
	if ( GetLocaleInfo(	GetSystemDefaultLangID(), 
					LOCALE_SABBREVLANGNAME, 
					strLangName, 
					sizeof( strLangName ) ) )			// ENU, JPN etc.
	{
		return strLangName;
	}
	return _T("ENU");
}

BOOL CNTHelper::QueryKeyValue(HKEY hKey, CString strKey, CString& strValue)
{
	DWORD	dwType;
	LPBYTE	pBuffer = 0;
	DWORD	bufSize = 0;

	strValue = _T("");
	LONG nResult = RegQueryValueEx(	hKey, 
									strKey, 
									0,
									&dwType,
									pBuffer,
									&bufSize);

	if (ERROR_SUCCESS != nResult)
	{
		return FALSE;
	}

	if (bufSize == 0 )
	{
		strValue = _T("");
		return TRUE;
	}
	// bufSize should now tell us how much space we need...
    pBuffer = (LPBYTE)malloc(bufSize);

	nResult = RegQueryValueEx(	hKey, 
								strKey, 
								0,
								&dwType,
								pBuffer,
								&bufSize);

	if (ERROR_SUCCESS == nResult)
	{
		strValue = (LPCTSTR)pBuffer;
		free(pBuffer);
		return TRUE;
	}
	else
	{
		free(pBuffer);
		return FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Popup a window to ask user for file name
//
// strFileFilter:		File Filter
// bSaveOrOpen:			FALSE for Save File, TRUE or Open file caption
// strFileExtension:	File Name extension
// strFileName:			The place where the result will be placed
//
BOOL CNTHelper::PopWindowForFileName( CString	strFileFilter,
						   BOOL		bSaveOrOpen,
						   CString  strFileExtension,
						   CString&	strFileName
						   )
{
	CFileDialogEx theFileDialog(	bSaveOrOpen,
								strFileExtension,
								NULL,
								OFN_HIDEREADONLY,
								strFileFilter);
	if (theFileDialog.DoModal() == IDOK) 
	{
		strFileName = theFileDialog.GetPathName();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Return the directory of the current module file
//
// Suppose current module file is: E:\Products\DataAnalyst\bin\DataAnalyst.exe
// It will return E:\Products\DataAnalyst\bin
//
CString CNTHelper::GetModuleFileDir()
{
	DWORD	dwLength, dwSize;
	TCHAR	szFileName [MAX_PATH];
	CString	strFileName;
	int		nPos;
	
	dwSize = sizeof (szFileName) / sizeof (szFileName [0]);
	dwLength = ::GetModuleFileName (AfxGetInstanceHandle(), szFileName, dwSize);
	if (dwLength <= 0) 
	{
		return _T("");
	}

	strFileName = szFileName;
	nPos = strFileName.ReverseFind( '\\' );
	return strFileName.Left( nPos );
}

/////////////////////////////////////////////////////////////////////////////
// Function to get the system temporary path name
//
CString CNTHelper::GetTempFolder()
{
	
	TCHAR buf[255];
	GetTempPath( 255, buf );
	CString strTmp( buf );
	
	return strTmp;
}

/////////////////////////////////////////////////////////////////////////////
// Get system time in the following format
// YYYYMMDDHH24MISS
//
CString CNTHelper::GetSysTimeStamp()
{
	CTime		t = CTime::GetCurrentTime();
	CString		strTime;

	strTime = t.Format("%Y%m%d%H%M%S");
	return strTime;
}

/////////////////////////////////////////////////////////////////////////////
// Print a string to the default printer, p points to the string buffer, 
// pSize holds the size information
//
void CNTHelper::PrintString(TCHAR *p, DWORD pSize, CString strTitle)
{
    CDC             dc;
    CPrintDialog    printDlg(FALSE);
    CRect           r;
    int             nHeight;
    
    // ask the user to select a printer
    if (printDlg.DoModal() == IDCANCEL)
        return;

    // For one line long string, it wraps the last word. If we add this, 
	// it will not wrap the last line.
	p[pSize] = '\n';
	pSize++;
	
    // Attach a printer DC
    dc.Attach(printDlg.GetPrinterDC());
    dc.m_bPrinting = TRUE;

    // use Textmappingmode, that's easiest to map the fontsize
    dc.SetMapMode(MM_TEXT);
    
    // setup font specifics
    LOGFONT LogFont;
    
    CFont       aFont, *oldFont;
    
    LogFont.lfHeight = -MulDiv(10, GetDeviceCaps(dc, LOGPIXELSY), 72);
    LogFont.lfWidth = 0;
    LogFont.lfEscapement = 0;
    LogFont.lfOrientation = 0;
    LogFont.lfWeight = 0;
    LogFont.lfItalic = false;
    LogFont.lfUnderline = 0;
    LogFont.lfStrikeOut = 0;
    LogFont.lfCharSet = DEFAULT_CHARSET;					// So that it can deal with Chinese or Japanese
    LogFont.lfOutPrecision = OUT_TT_PRECIS;
    LogFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    LogFont.lfQuality = DEFAULT_QUALITY;
    LogFont.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
    lstrcpy (LogFont.lfFaceName, _T("Courier New"));
    dc.SetBkMode(OPAQUE);
    aFont.CreateFontIndirect ( &LogFont );
    // ok, we've build the font, now use it
    oldFont = dc.SelectObject( &aFont );        
        
    // Initialise print document details    
    DOCINFO di;
    ::ZeroMemory ((void*)&di, sizeof (DOCINFO)); 
    di.cbSize = sizeof (DOCINFO);
    // application title appears in the spooler view
    di.lpszDocName = strTitle;
    
    // Begin a new print job
    BOOL bPrintingOK = dc.StartDoc( &di );
        
    // Get the printing extents and store in the m_rectDraw field of a 
    // CPrintInfo object
    CPrintInfo Info;
    int w = dc.GetDeviceCaps(HORZRES);
    int h = dc.GetDeviceCaps(VERTRES);
    Info.m_rectDraw.SetRect(100,100, w, h-200);
    
    TCHAR *pStartAt = p;
    long nTotalDone = 0;
    long nLengthToGo = pSize;
    
	dc.SelectObject( &aFont );
    for (UINT page = Info.GetMinPage(); bPrintingOK && nTotalDone < nLengthToGo; page++)
	{
        // begin new page
        dc.StartPage();
        Info.m_nCurPage = page;
        
        // calc how much text fits on one page
        r = Info.m_rectDraw;
        r.bottom = r.top;
		long nLow = 0;
		long nHigh = nLengthToGo - nTotalDone;
		long nCount = nLengthToGo - nTotalDone;
		r = Info.m_rectDraw;

		// Use binary search algorithm to quickly find out the size of each page
		while( nLow < nHigh )
        {
            r.right = Info.m_rectDraw.right;
            nHeight = dc.DrawText(pStartAt, nCount, r, DT_CALCRECT|DT_WORDBREAK|DT_NOCLIP|DT_EXPANDTABS);

			if ( nHeight < h )
			{
				nLow = nCount;
			}
			if ( nHeight > h )
			{
				nHigh = nCount;
			}
			if ( nLow == nHigh-1 )
			{
				nLow = nHigh;
			}
			if ( nLow < nHigh )
			{
				nCount = nLow + ( nHigh - nLow ) / 2;
			}
        }
        // go one back to assure correct height
        if (r.bottom >= Info.m_rectDraw.bottom)
		{
            nCount--;
		}
        
        // print that text
        dc.DrawText(pStartAt, nCount, r, DT_WORDBREAK|DT_NOCLIP|DT_EXPANDTABS);
        
        // go to next page
        pStartAt += nCount;
        nTotalDone += nCount;
        
        // end page
        bPrintingOK = (dc.EndPage() > 0);
    }
    
    // end a print job
    if (bPrintingOK)
	{
        dc.EndDoc();
	}
    else
	{
        // abort job.
        dc.AbortDoc();
	}
    
    // restore font
    dc.SelectObject(oldFont);

    // free font memory
    aFont.DeleteObject();

    // detach the printer DC
    dc.Detach();
}

/////////////////////////////////////////////////////////////////////////////
// Execute DOS Command
//
// strCmd:	Command to be executed
// bSync:	Whether sync or not
//
void CNTHelper::ExecuteCmd(CString strCmd, BOOL bSync /*=FALSE*/)
{
	BOOL				bRetCode;
	STARTUPINFO			StartupInfo;
	GetStartupInfo( &StartupInfo );
	PROCESS_INFORMATION	Process_Information;

	bRetCode = CreateProcess(	NULL,
								(LPTSTR)(LPCTSTR)strCmd,
								NULL,
								NULL,
								FALSE,
								CREATE_NO_WINDOW,
								NULL,
								NULL,
								&StartupInfo,
								&Process_Information );		
	if ( bSync )
	{
		WaitForSingleObject( Process_Information.hProcess, INFINITE );
	}
}

/////////////////////////////////////////////////////////////////////////////
// Start File
//
// strFile:		File to be opened
//
void CNTHelper::StartFile(CString &strFile)
{
	CString		strCmd;
	strCmd.Format( _T("cmd /c \"%s\""), strFile );
	ExecuteCmd( strCmd );
}

CNTHelper::OSType CNTHelper::GetOSType()
{
	OSVERSIONINFO vinfo;
	
	vinfo.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	::GetVersionEx( &vinfo );

	if( vinfo.dwPlatformId != VER_PLATFORM_WIN32_NT &&
		vinfo.dwMajorVersion == 4					&&
		vinfo.dwMinorVersion == 0	)
	{
		return WIN_95;
	}
	if( vinfo.dwPlatformId != VER_PLATFORM_WIN32_NT &&
		vinfo.dwMajorVersion >= 4					&&
		vinfo.dwMinorVersion > 0					&&
		!( vinfo.dwMajorVersion == 4 && vinfo.dwMinorVersion == 90 ) )
	{
		return WIN_98;
	}
	if( vinfo.dwPlatformId != VER_PLATFORM_WIN32_NT &&
		vinfo.dwMajorVersion == 4					&&
		vinfo.dwMinorVersion == 90 )
	{
		return WIN_ME;
	}
	if( vinfo.dwPlatformId == VER_PLATFORM_WIN32_NT &&
		vinfo.dwMajorVersion == 4					&&
		vinfo.dwMinorVersion == 0 )
	{
		return WIN_NT;
	}
	if( vinfo.dwPlatformId == VER_PLATFORM_WIN32_NT &&
		vinfo.dwMajorVersion == 5					&&
		vinfo.dwMinorVersion == 0 )
	{
		return WIN_2000;
	}
	if( vinfo.dwPlatformId == VER_PLATFORM_WIN32_NT &&
		vinfo.dwMajorVersion == 5					&&
		vinfo.dwMinorVersion == 1 )
	{
		return WIN_XP;
	}

	// Default always return Windows NT
	return WIN_NT;
}

/////////////////////////////////////////////////////////////////////////////
// Convert string to double
//
// strValue:		String to be converted
// return:			Double data type
//
double	CNTHelper::_ttof( CString strValue )
{
#ifdef _UNICODE
	USES_CONVERSION;
	return atof( W2A( strValue ) );
#else
	return atof( strValue );
#endif
}


/////////////////////////////////////////////////////////////////////////////
BOOL AFXAPI AfxUnhookWindowCreateEx()
{
	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
#ifndef _AFXDLL
	if (afxContextIsDLL && pThreadState->m_hHookOldCbtFilter != NULL)
	{
		::UnhookWindowsHookEx(pThreadState->m_hHookOldCbtFilter);
		pThreadState->m_hHookOldCbtFilter = NULL;
	}
#endif
	if (pThreadState->m_pWndInit != NULL)
	{
		pThreadState->m_pWndInit = NULL;
		return FALSE;   // was not successfully hooked
	}

	return TRUE;
}

void AFXAPI AfxHookWindowCreateEx(CWnd* pWnd)
{
	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	if (pThreadState->m_pWndInit == pWnd)
		return;

	if (pThreadState->m_hHookOldCbtFilter == NULL)
	{
		/*
		pThreadState->m_hHookOldCbtFilter = ::SetWindowsHookEx(WH_CBT,
			_AfxCbtFilterHook, NULL, ::GetCurrentThreadId());
		if (pThreadState->m_hHookOldCbtFilter == NULL)
			AfxThrowMemoryException();
		*/
	}
	ASSERT(pThreadState->m_hHookOldCbtFilter != NULL);
	ASSERT(pWnd != NULL);
	ASSERT(pWnd->m_hWnd == NULL);   // only do once

	ASSERT(pThreadState->m_pWndInit == NULL);   // hook not already in progress
	pThreadState->m_pWndInit = pWnd;
}

/////////////////////////////////////////////////////////////////////////////
// CFileDialogEx

IMPLEMENT_DYNAMIC(CFileDialogEx, CFileDialog)

CFileDialogEx::CFileDialogEx(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
}


BEGIN_MESSAGE_MAP(CFileDialogEx, CFileDialog)
	//{{AFX_MSG_MAP(CFileDialogEx)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CFileDialogEx::DoModal()
{
	ASSERT_VALID(this);
	ASSERT(m_ofn.Flags & OFN_ENABLEHOOK);
	ASSERT(m_ofn.lpfnHook != NULL); // can still be a user hook

	// zero out the file buffer for consistent parsing later
	ASSERT(AfxIsValidAddress(m_ofn.lpstrFile, m_ofn.nMaxFile));
	DWORD nOffset = lstrlen(m_ofn.lpstrFile)+1;
	ASSERT(nOffset <= m_ofn.nMaxFile);
	memset(m_ofn.lpstrFile+nOffset, 0, (m_ofn.nMaxFile-nOffset)*sizeof(TCHAR));

	// WINBUG: This is a special case for the file open/save dialog,
	//  which sometimes pumps while it is coming up but before it has
	//  disabled the main window.
	HWND hWndFocus = ::GetFocus();
	BOOL bEnableParent = FALSE;
	m_ofn.hwndOwner = PreModal();
	AfxUnhookWindowCreateEx();
	if (m_ofn.hwndOwner != NULL && ::IsWindowEnabled(m_ofn.hwndOwner))
	{
		bEnableParent = TRUE;
		::EnableWindow(m_ofn.hwndOwner, FALSE);
	}

	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	ASSERT(pThreadState->m_pAlternateWndInit == NULL);

	if (m_ofn.Flags & OFN_EXPLORER)
		pThreadState->m_pAlternateWndInit = this;
	else
		AfxHookWindowCreateEx(this);

	m_ofn.Flags = 2060;
	int nResult;
	if (m_bOpenFileDialog)
		nResult = ::GetOpenFileName(&m_ofn);
	else
		nResult = ::GetSaveFileName(&m_ofn);

	pThreadState->m_pAlternateWndInit = NULL;

	// WINBUG: Second part of special case for file open/save dialog.
	if (bEnableParent)
		::EnableWindow(m_ofn.hwndOwner, TRUE);
	if (::IsWindow(hWndFocus))
		::SetFocus(hWndFocus);

	PostModal();
	return nResult ? nResult : IDCANCEL;
}

/////////////////////////////////////////////////////////////////////////////
// Set Registry Key Value
// hKey:			Registry Key group
// strSection:		Registry Key Section
// strKey:			Registry Key
// strValue:		Registry Value
//
BOOL CNTHelper::SetRegKeyValue(HKEY hKey, CString strSection, CString strKey, CString strValue)
{
	HKEY			hSecKey;
	DWORD			dwResult;
	
	// Step 1. Open registry key
	if ( ::RegOpenKeyEx( hKey, strSection, 0, KEY_ALL_ACCESS, &hSecKey ) != ERROR_SUCCESS )
	{
		if ( ::RegCreateKeyEx( hKey, strSection, 0, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hSecKey, &dwResult ) != ERROR_SUCCESS )
		{
			// Can not create registry key
			return FALSE;
		}
	}

	// Step 2. Set registry key value
#ifdef _UNICODE 
	USES_CONVERSION;
	RegSetValueEx( hSecKey, strSection, 0, REG_SZ, (unsigned char*)W2A( strValue ), strValue.GetLength() );
#else
	TCHAR			szValue[1024];
	sprintf( szValue, "%s", strValue );
	RegSetValueEx( hSecKey, strSection, 0, REG_SZ, (LPBYTE)szValue, strValue.GetLength() );
#endif

	// Step 3. Close registry key
	RegCloseKey( hSecKey );
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Set Registry Key Value
// hKey:			Registry Key group
// strSection:		Registry Key Section
// strKey:			Registry Key
// strValue:		Registry Value
//
BOOL CNTHelper::GetRegKeyValue(HKEY hKey, CString strSection, CString strKey, CString& strValue)
{
	HKEY			hSecKey;
	DWORD			dwResult;
	
	// Step 1. Open registry key
	if ( ::RegOpenKeyEx( hKey, strSection, 0, KEY_ALL_ACCESS, &hSecKey ) != ERROR_SUCCESS )
	{
		if ( ::RegCreateKeyEx( hKey, strSection, 0, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hSecKey, &dwResult ) != ERROR_SUCCESS )
		{
			// Can not create registry key
			return FALSE;
		}
	}

	// Step 2. Set registry key value
#ifdef _UNICODE 
	USES_CONVERSION;
#endif
	QueryKeyValue( hSecKey, strKey, strValue );

	// Step 3. Close registry key
	RegCloseKey( hSecKey );
	return TRUE;
}
