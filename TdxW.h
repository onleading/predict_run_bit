// TdxW.h : main header file for the TdxW application
//

#if !defined(AFX_TDXW_H__ADD98CE8_82F1_11D1_98A0_0088CC032229__INCLUDED_)
#define AFX_TDXW_H__ADD98CE8_82F1_11D1_98A0_0088CC032229__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

class CHqSrcMgn;
class CTdxWApp : public CWinApp
{
private:
	BOOL SetInitValue();
	void CopySomeFileToT0002();
	void CopyResDLLFromT0002();
	BOOL bExistInstance;

	HANDLE m_hMutex;
	CNewMultiDocTemplate *pBigMenuTemplate,*pHomePageTemplate,*pGridTemplate,*pComTemplate,*pCom2Template,*pTxtTemplate,*pNewsTemplate,*pHisTemplate,*pWebInfoTemplate,*pCjzxTemplate,*pJbfxTemplate,*pAdvHqTemplate,*pJjTemplate,*pTvTemplate,*pRssTemplate;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CTdxWApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTdxWApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CTdxWApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TDXW_H__ADD98CE8_82F1_11D1_98A0_0088CC032229__INCLUDED_)
 
