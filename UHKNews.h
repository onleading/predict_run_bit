// UHKNews.h: interface for the UHKNews class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UHKNEWS_H__0CE1FBC0_7299_4818_BB4C_8C7B1E63BA03__INCLUDED_)
#define AFX_UHKNEWS_H__0CE1FBC0_7299_4818_BB4C_8C7B1E63BA03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UBase.h"
#include "ListBoxEx.h"
#include "MyRichEdit.h"
#include "MyScrollBar.h"
#include "SwitchEx.h"
#include "HKNewsContentDlg.h"

class UHKNews : public UBase  
{
public:
	UHKNews(LPCOMINFO pComInfo);
	virtual ~UHKNews();

	int  GetStyle() { return m_nStyle; }
	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
	void KillDlg(void);
	
private:
//	void DrawSwitchCtrl(CDC *pDC);

	void OnVScrollMessage(WPARAM wParam,LPARAM lParam);
	void Resize();
	void RefreshScheme();

	int  HitTest(CPoint point);
	void FillRealTitleList();
	void FillRealTypeList();
	void FillRealContent();
	
	void FillList();
	void FillContent();
private:
	CListBoxEx		*m_pListType, *m_pListTitle;
	CMyRichEdit		*m_pContentEdit;
	CHARFORMAT		cfm;
	
	CHKNewsContentDlg	*m_pContentDlg;
	CMyScrollBar	*pVScrollBarType, *pVScrollBarTitle;
	CNewZfjsTab		*m_pSwitch;

	CFont			NewFont;
	int				m_nCurSwitch, m_nCurSelType, m_nCurSelTitle, TextH;
};

#endif // !defined(AFX_UHKNEWS_H__0CE1FBC0_7299_4818_BB4C_8C7B1E63BA03__INCLUDED_)
