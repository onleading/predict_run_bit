// ViewManager.cpp: implementation of the CViewManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TdxW.h"
#include "ViewManager.h"

#include "TxtView.h"
#include "GridView.h"
#include "BigMenuView.h"
#include "HomePageView.h"
#include "NewsView.h"
#include "ComView.h"
#include "ComView2.h"
#include "IEBrowserView.h"
#include "JbfxView.h"
#include "AdvHqView.h"
#include "JJView.h"
#include "TvView.h"
#include "RssView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CViewManager::CViewManager()
{
	bClosing = FALSE;
	HQWindowNum = 0;
	TxtWindowNum = 0;
	NewsWindowNum = 0;
	HisWindowNum = 0;
	ComWindowNum = 0;
	Com2WindowNum = 0;
	BigMenuWindowNum = 0;
	HomePageWindowNum = 0;
	WebInfoWindowNum = 0;
	CjzxWindowNum = 0;
	JbfxWindowNum = 0;
	AdvHqWindowNum = 0;
	JJWindowNum = 0;
	TvWindowNum = 0;
	RssWindowNum = 0;

	pActiveView = pOldActiveView = pOldActiveView2 = NULL;
}

CViewManager::~CViewManager()
{
	arViews.RemoveAll();
	arViewTitles.RemoveAll();
}

void CViewManager::AddView(const char * csName, CView * pView)
{
	if (bClosing) return;

	CString cs(csName);

	arViews.Add(pView);
	arViewTitles.Add(cs);

	pOldActiveView = pActiveView;
	pActiveView = pView; //当前激活的窗口重新设置

	if(pView->IsKindOf (RUNTIME_CLASS(CGridView)))
		HQWindowNum ++;
	else if(pView->IsKindOf (RUNTIME_CLASS(CTxtView)))
		TxtWindowNum ++;
	else if(pView->IsKindOf (RUNTIME_CLASS(CNewsView)))
		NewsWindowNum ++;
	else if(pView->IsKindOf (RUNTIME_CLASS(CComView)))
		ComWindowNum++;
	else if(pView->IsKindOf (RUNTIME_CLASS(CComView2)))
		Com2WindowNum++;
	else if(pView->IsKindOf (RUNTIME_CLASS(CBigMenuView)))
		BigMenuWindowNum++;
	else if(pView->IsKindOf (RUNTIME_CLASS(CHomePageView)))
		HomePageWindowNum++;
	else if(pView->IsKindOf (RUNTIME_CLASS(CIEBrowserView)))
		WebInfoWindowNum++;
	else if(pView->IsKindOf (RUNTIME_CLASS(CJbfxView)))
		JbfxWindowNum++;
	else if(pView->IsKindOf (RUNTIME_CLASS(CAdvHqView)))
		AdvHqWindowNum++;
	else if (pView->IsKindOf (RUNTIME_CLASS(CJJView)))
		JJWindowNum++;
	else if (pView->IsKindOf (RUNTIME_CLASS(CTvView)))
		TvWindowNum++;
	else if(pView->IsKindOf (RUNTIME_CLASS(CRssView)))
		RssWindowNum++;
}

void CViewManager::RemoveView(CView * pView)
{
	if (bClosing || arViews.GetSize() <= 0) return;

	int t;
	for (t = 0; t < arViews.GetSize(); t++)
	{
		if ((CView *)(arViews.GetAt(t)) == pView)
		{
			if(pView->IsKindOf (RUNTIME_CLASS(CGridView)))
				HQWindowNum--;
			else if(pView->IsKindOf (RUNTIME_CLASS(CTxtView)))
				TxtWindowNum--;
			else if(pView->IsKindOf (RUNTIME_CLASS(CNewsView)))
				NewsWindowNum--;
			else if(pView->IsKindOf (RUNTIME_CLASS(CComView)))
				ComWindowNum--;
			else if(pView->IsKindOf (RUNTIME_CLASS(CComView2)))
				Com2WindowNum--;
			else if(pView->IsKindOf (RUNTIME_CLASS(CBigMenuView)))
				BigMenuWindowNum--;
			else if(pView->IsKindOf (RUNTIME_CLASS(CHomePageView)))
				HomePageWindowNum--;
			else if(pView->IsKindOf (RUNTIME_CLASS(CIEBrowserView)))
				WebInfoWindowNum--;
			else if(pView->IsKindOf (RUNTIME_CLASS(CJbfxView)))
				JbfxWindowNum--;
			else if(pView->IsKindOf (RUNTIME_CLASS(CAdvHqView)))
				AdvHqWindowNum--;
			else if(pView->IsKindOf (RUNTIME_CLASS(CJJView)))
				JJWindowNum--;
			else if(pView->IsKindOf (RUNTIME_CLASS(CTvView)))
				TvWindowNum--;
			else if(pView->IsKindOf (RUNTIME_CLASS(CRssView)))
				RssWindowNum--;

			arViewTitles.RemoveAt(t);
			arViews.RemoveAt(t);

			if(pView==pOldActiveView) //Remove以后,顺便将pOldActiveView,pOldActiveView2赋空
				pOldActiveView=NULL;
			if(pView==pOldActiveView2)
				pOldActiveView2=NULL;
			if(pView==pActiveView)
				pActiveView=NULL;
			return;
		}
	}
}

void CViewManager::RemoveAll()
{
	arViews.RemoveAll();
	arViewTitles.RemoveAll();
}

void CViewManager::SetViewName(const char * cs, CView * pView)
{
	if (bClosing || arViews.GetSize() <= 0) return;

	int t;
	CString csName(cs);

	for (t = 0; t < arViews.GetSize(); t++)
	{
		if ((CView *)(arViews.GetAt(t)) == pView)
		{
			arViewTitles.SetAt(t, csName);
			return;
		}
	}
}

int CViewManager::GetWindowNum()
{
	return arViews.GetSize();
}

void CViewManager::OnActivateView(const BOOL bActivate, CView * pView)
{
	if (bActivate)
	{
		if(pActiveView!=pView)
			pOldActiveView = pActiveView;
		pActiveView = pView; //当前激活的窗口重新设置
	}
}
