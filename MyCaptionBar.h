#if !defined(AFX_MYCAPBAR_H__9C64BE29_0473_49A6_9315_9A437B155011__INCLUDED_)
#define AFX_MYCAPBAR_H__9C64BE29_0473_49A6_9315_9A437B155011__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyBitmapBt.h"

#define ID_CAPTIONBAR_MENU0		1000
#define UM_EXITMENULOOP			WM_USER+200

#define	TOPAREA_HEIGHT			20

class CMyCaptionBar : public CDialogBar
{
	void CancelAndNewMenu();
public:
	CMyCaptionBar();
	
// Attributes
public:
	HMENU			m_hmenu;	//当前打开的菜单的句柄
	CPoint			m_pt;	
	BOOL			m_leftbuttondown;
	MyBitmapBt **	m_bt;
	CRect *			m_bt_rect;
	MyBitmapBt *	m_rbt[5];
	CRect			m_rbt_rect[5];
	MyBitmapBt *	m_sysbt[4];
	CRect			m_sysbt_rect[3];

	CRect			m_titlestr_rect;
	CString			m_titlestr;
	UINT			m_nButton;
	CString *		m_pstr;

	CNewMenu		m_menu;			//加载的菜单
	UINT			m_nID_Menu;		//加载的菜单的ID
	BOOL			m_bFirstShow;	//窗口是否已经显示过至少一次.
	//在第一次显示前,框架会修改此窗口的尺寸若干此,从而干扰了子窗口的定位
	//用此标记使相关定位在此窗口至少显示过一次后进行
	CImageList		m_imagelist;
	CPoint			m_icon_point;
// Operations
public:
	BOOL			LoadMenu(UINT nIDMenu);
	void			CancelAndPopNewMenu();

	void			NextBtRect(CRect &rc,int offset,int xLimit);
	void			ChangeParentWndSize(UINT sc);

	virtual void	OnMenuButton(UINT nIndex);
	virtual void	ModifyDC(CDC *pDC);
	void			SetTitleStr(LPCSTR str);
	void			GetTitleStr(CString &str);
protected:
	BOOL			ProcessMenu(LPVOID pNMenu,UINT nIndex);
	HICON			m_hIcon;
	int				m_nHeadIconWid;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyCaptionBar)
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyCaptionBar();
	CNewMenu m_tmpMenu;
	CNewMenu m_WebMenu,m_WebMenu2,m_AdvFuncMenu,m_DSMenu;
	// Generated message map functions
protected:
	//{{AFX_MSG(CMyCaptionBar)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif // !defined(AFX_MYDOCKBAR_H__9C64BE29_0473_49A6_9315_9A437B155011__INCLUDED_)
