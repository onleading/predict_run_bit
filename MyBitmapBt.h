#if !defined(AFX_MYBITMAPBT_H__FCA23411_6CB9_4171_9417_D6D2E6502FBA__INCLUDED_)
#define AFX_MYBITMAPBT_H__FCA23411_6CB9_4171_9417_D6D2E6502FBA__INCLUDED_

//#include "struct.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyBitmapBt.h : header file
//
#include "stdafx.h"
#include "MyInterface.h"

/////////////////////////////////////////////////////////////////////////////
// MyBitmapBt window
enum ButtonState
{
	Normal=0,
	MouseOver,
	LMouseDown,
	Disable		//暂时未使用
};

enum ButtonType
{
	NormalBt,	//按钮结构包括:背景,文字
	TitleBt,	//按钮结构包括:背景,中心会随状态改变位置的图标,文字
	SwitchLabelBt,	//作为CMySwitchBar控件的元素
	MenuBt,		//模拟菜单,参考CMyCaptionBar里的相关代码使用
};

enum TextStyle
{
	TS_Normal=0x00,
	TS_Bold=0x01,
	TS_Italic=0x02,
	TS_Underline=0x4,
};

const UINT UM_MYBITMAPBUTTON=WM_USER+1001;		//当被点击,左键松开时发送一次此消息
const UINT UM_MYBITMAPBUTTON_ONMOUSEMOVE=WM_USER+1002;		//当m_bSel==TRUE时由计时器发送此消息

typedef void(WINAPI*MBB_OwnerDraw)(CWnd *pWnd,CDC *pDC,CRect rcWnd,ButtonState btState,BOOL bFocus);	//自绘接口函数

class MyBitmapBt : public CWnd
{
// Construction
public:
	MyBitmapBt();	//需要调用Init初始化
	MyBitmapBt(UINT id,LPCSTR text,ButtonType bt,UINT nFormat=DT_BOTTOM|DT_CENTER,BOOL useWndRgn=FALSE,COLORREF corMask=RGB(0,255,0));
	virtual ~MyBitmapBt();
// Attributes
protected:
	MBB_OwnerDraw m_pfOwnerDraw;	//若此函数指针不为空,则在绘制背景后调用此函数自绘 
	UINT m_id;//按钮id,用于区别不同的按钮
	BOOL bFirst;
	BOOL m_LBtDown;
	CBitmap *m_TitleBtBK[9];
	COLORREF m_cor_text_n,m_cor_text_d,m_cor_text_o;
	TextStyle m_text_style_n,m_text_style_d,m_text_style_o;
	int m_text_height_n,m_text_height_d,m_text_height_o;
	CBitmap m_bmp;
	BOOL m_drawflag;
	CBitmap *m_pBMCenter;
	ButtonType m_bt;
	ButtonState m_BtState;
	CRect m_wndRect;
	CString m_wndText;
	BOOL m_bSel;
	UINT m_nFormat;		//按钮文字对齐方式:DT_LEFT,DT_CENTER,DT_RIGHT等
	BOOL m_UseWndRgn;	//设置了窗口RGN
	COLORREF m_corMask;	//不规则窗口边界透明色
	CString m_strTooltip;	//鼠标经过时显示的tooltip文本
	CToolTipCtrl m_ToolTip;
	BOOL m_bFocus;	//是否是输入焦点
	CPoint m_offset[3];
	COLORREF m_cor1_menu,m_cor2_menu;	//MenuBt风格时,当按钮被按下后画的边框的颜色
// Operations
public:
	void Init(UINT id,LPCSTR text,ButtonType bt,UINT nFormat=DT_BOTTOM|DT_CENTER,BOOL useWndRgn=FALSE,COLORREF corMask=RGB(0,255,0));
	void SetSel(BOOL sel);	//设置选中状态
	BOOL GetSel();		//判断选中状态
	void SetBK(long bk[9]);	//普通,按下,鼠标滑过,3种状态,每种状态由左中右3张位图拼接出背景	
	void SetBK(LPCTSTR bk[9]);
	void SetBK(CBitmap* bk[9]);
	void SetText(LPCSTR txt);
	void SetTextCor(COLORREF cor_text_n,COLORREF cor_text_d=0,COLORREF cor_text_o=0);	//设置文字在不同状态下的颜色
	void SetTextStyle(TextStyle style_n,TextStyle style_d,TextStyle style_o);	//设置文字在不同状态下的风格(粗体,斜体等)
	void SetTextHeight(int height_n,int height_d=-1,int height_o=-1);
	void SetTextOffset(int x_n,int y_n,int x_d,int y_d,int x_o=0,int y_o=0);	//设置不同状态时文字的偏移,NormalBt,SwitchLabelBt风格时有效
	void SetTooltip(LPCSTR str);
	void SetCenterImage(long ID_CENTER);//按钮类型为TitleBT时,用来设置中心位图
	void SetCenterImage(LPCTSTR resName);
	void SetCenterImage(CBitmap *pbm);
	void SetCustomFunc(MBB_OwnerDraw pFunc);
	void SetFrameCor_MenuStyle(COLORREF cor1,COLORREF cor2);
	static void GetTextStyle(TextStyle style,int &weight,BOOL &bItalic,BOOL &bUnderline);
	void GetOriginalWndRgn(CRgn &rgn);
	void GetTextFont(CFont &font);
	UINT GetID();
protected:
	void OnMouseLeave(WPARAM w,LPARAM l); 
	void OnMouseEnter(WPARAM w,LPARAM l);
	COLORREF _SetTextCor(CDC *pDC);
	void _SetWindowRgn();
	void _ModifyState(ButtonState &state);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MyBitmapBt)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	// Generated message map functions
protected:
	//{{AFX_MSG(MyBitmapBt)
	afx_msg void OnPaint();
	afx_msg void OnMove(int x, int y);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
//CMyStatic
class CMyStatic
{
public:
	CMyStatic();
	CRect m_rc;
	CString m_str;
	COLORREF m_cor_text;
	CWnd *m_pParentWnd;
	void SetParent(CWnd * pParentWnd);
	void SetWindowText(LPCSTR str);
	void GetWindowText(CString &str);
	void SetRect(CRect rc);
	void GetRect(CRect &rc);
	void SetTextColor(COLORREF cor_text);
	COLORREF GetTextColor();
	void DrawText(CDC *pDC,UINT nFormat=DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	void DrawText(CDC *pDC,UINT nFormat,COLORREF cr,UINT size,LPCSTR type=SONG_FONT);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYBITMAPBT_H__FCA23411_6CB9_4171_9417_D6D2E6502FBA__INCLUDED_)
