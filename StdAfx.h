// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__4B5350E7_8BB1_11D3_B921_0000800104D5__INCLUDED_)
#define AFX_STDAFX_H__4B5350E7_8BB1_11D3_B921_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#pragma warning(disable:4786) 
#pragma warning(disable:4788) 
#endif // _MSC_VER > 1000

//#define WINVER 0X0501
#define WINVER 0x5000
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <afxhtml.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <AFXPRIV.h>
#include <afxrich.h>		// MFC rich edit classes
#include <afxcview.h>		// For ListView access
#include <wsipx.h>
#include <afxsock.h>
#include <afxmt.h>
#include <comdef.h>

#include <afxtempl.h>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <afxinet.h>
#include <algorithm>
#include <strstream>
using namespace std;

#import "msxml3.dll" named_guids
#import "C:\\Program Files\\Common Files\\System\\ado\\msado15.dll" no_namespace rename("EOF", "adoEOF")

typedef CMenu	CNewMenu;
typedef CMultiDocTemplate	CNewMultiDocTemplate;
typedef CMDIFrameWnd	CNewMDIFrameWnd;
typedef CMDIChildWnd	CNewMDIChildWnd;

#include "Appface.h"

#include "string.h"
#include "io.h"
#include "math.h"
#include "stdlib.h"
#include "fcntl.h"
#include "share.h"
#include "ctype.h"
#include "malloc.h"
#include "process.h"
#include "stdio.h"
#include "direct.h"
#include "sys\stat.h"

#include "dbfile.h"
#include "macro.h"
#include "ddrawlinestruct.h"
#include "EnMarco.h"
#include "dde.h"
#include "xgsys.h"
#include "OutStruct.h"
#include "DataIo.h"
#include "LocalIndexMng.h"
#include "struct.h"
#include "ExDayMng.h"
#include "gdi.h"
#include "VGear.h"
#include "protocol.h"
#include "protocol3.h"
#include "Registry.h"
#include "constmng.h"
#include "GlobalVar.h"
#include "func.h"
#include "func_ini.h"
#include "func_comm.h"
#include "loaddll.h"
#include "jy.h"
#include "levin.h"
#include "info.h"
#include "MyInterface.h"
#include "treeid.h"

#include <MMSystem.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Imm32.lib")

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__4B5350E7_8BB1_11D3_B921_0000800104D5__INCLUDED_)
