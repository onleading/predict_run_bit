// ViewManager.h: interface for the CViewManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWMANAGER_H__C67D43A7_F890_11D1_83BA_0000B43382FE__INCLUDED_)
#define AFX_VIEWMANAGER_H__C67D43A7_F890_11D1_83BA_0000B43382FE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxcoll.h>

class CViewManager  
{
public:
	void OnActivateView(const BOOL bActivate, CView * pView);
	int GetWindowNum();
	void SetViewName(const char * cs, CView * pView);
	void RemoveAll();
	void RemoveView(CView * pView);
	void AddView(const char * csName, CView * pView);
	CViewManager();
	virtual ~CViewManager();

	CPtrArray		arViews;
	CStringArray	arViewTitles;

	int				HQWindowNum;
	int				TxtWindowNum;
	int				NewsWindowNum;
	int				HisWindowNum;
	int				ComWindowNum;
	int				Com2WindowNum;
	int				BigMenuWindowNum;
	int				HomePageWindowNum;
	int				WebInfoWindowNum;
	int				CjzxWindowNum;
	int				JbfxWindowNum;
	int				AdvHqWindowNum;
	int				JJWindowNum;
	int				TvWindowNum;
	int				RssWindowNum;

	CView			*pActiveView;		//当前激活的窗口
	CView			*pOldActiveView;	//上一次激活的窗口
	CView			*pOldActiveView2;	//上上一次激活的窗口

	BOOL			bClosing;
};

#endif // !defined(AFX_VIEWMANAGER_H__C67D43A7_F890_11D1_83BA_0000B43382FE__INCLUDED_)
