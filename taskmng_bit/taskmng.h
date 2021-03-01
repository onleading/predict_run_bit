// taskmng.h : main header file for the TASKMNG application
//

#if !defined(AFX_TASKMNG_H__5511C293_FD75_4B4C_890A_CFB44B967614__INCLUDED_)
#define AFX_TASKMNG_H__5511C293_FD75_4B4C_890A_CFB44B967614__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTaskmngApp:
// See taskmng.cpp for the implementation of this class
//

class CTaskmngApp : public CWinApp
{
public:
	CTaskmngApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskmngApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTaskmngApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKMNG_H__5511C293_FD75_4B4C_890A_CFB44B967614__INCLUDED_)
