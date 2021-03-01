#ifndef STATUSWND_H_
#define STATUSWND_H_

#define	DKBar_Width		50
#define ROLLIcon_Width	16

#include "SwitchEx.h"
#include "SwitcherWnd.h"
#include "PopPreViewTool.h"

struct  MDITabInfo
{
	BOOL	bValid;
	HWND	MDIWnd;
	CString strTitle;
	HICON	hIcon;
	MDITabInfo()
	{
		bValid = 0;
		MDIWnd = NULL;
		strTitle = _T("");
		hIcon = NULL;
	}
};

 
class CStatusWnd : public CWnd
{
	BOOL	RegisterWindowClass(HINSTANCE hInstance,LPCTSTR lpszClassName);

	void	GetItemRect(int nBarID,CRect *pRC);
	void	DrawItem(CDC *pDC,int nBarID);
	void	DrawZSBar(CDC *pDC,CRect rect,int flag);
	void	DrawtheBar(CDC *pDC,CRect rect,int flag,struct CurrStockData *zstmpp,short TotalStkNum);
	void	DrawDKBar(CDC *pDC,CRect rect,struct CurrStockData *shzstmpp,struct CurrStockData *szzstmpp);

	void	RefreshMDITab();

	CRect	m_ToolRectHelp;
	int		m_nZsNum;
	CSwitcherWnd m_wndSwitcher;
	CPopPreViewTool m_PreviewTool;
public:
	CStatusWnd();
	virtual ~CStatusWnd();
protected:
	//{{AFX_MSG(CMarquee)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg void OnTabSelChanged(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	vector<MDITabInfo> m_vTabInfo;
public:
};

#endif
