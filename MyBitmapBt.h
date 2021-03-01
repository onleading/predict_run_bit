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
	Disable		//��ʱδʹ��
};

enum ButtonType
{
	NormalBt,	//��ť�ṹ����:����,����
	TitleBt,	//��ť�ṹ����:����,���Ļ���״̬�ı�λ�õ�ͼ��,����
	SwitchLabelBt,	//��ΪCMySwitchBar�ؼ���Ԫ��
	MenuBt,		//ģ��˵�,�ο�CMyCaptionBar�����ش���ʹ��
};

enum TextStyle
{
	TS_Normal=0x00,
	TS_Bold=0x01,
	TS_Italic=0x02,
	TS_Underline=0x4,
};

const UINT UM_MYBITMAPBUTTON=WM_USER+1001;		//�������,����ɿ�ʱ����һ�δ���Ϣ
const UINT UM_MYBITMAPBUTTON_ONMOUSEMOVE=WM_USER+1002;		//��m_bSel==TRUEʱ�ɼ�ʱ�����ʹ���Ϣ

typedef void(WINAPI*MBB_OwnerDraw)(CWnd *pWnd,CDC *pDC,CRect rcWnd,ButtonState btState,BOOL bFocus);	//�Ի�ӿں���

class MyBitmapBt : public CWnd
{
// Construction
public:
	MyBitmapBt();	//��Ҫ����Init��ʼ��
	MyBitmapBt(UINT id,LPCSTR text,ButtonType bt,UINT nFormat=DT_BOTTOM|DT_CENTER,BOOL useWndRgn=FALSE,COLORREF corMask=RGB(0,255,0));
	virtual ~MyBitmapBt();
// Attributes
protected:
	MBB_OwnerDraw m_pfOwnerDraw;	//���˺���ָ�벻Ϊ��,���ڻ��Ʊ�������ô˺����Ի� 
	UINT m_id;//��ťid,��������ͬ�İ�ť
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
	UINT m_nFormat;		//��ť���ֶ��뷽ʽ:DT_LEFT,DT_CENTER,DT_RIGHT��
	BOOL m_UseWndRgn;	//�����˴���RGN
	COLORREF m_corMask;	//�����򴰿ڱ߽�͸��ɫ
	CString m_strTooltip;	//��꾭��ʱ��ʾ��tooltip�ı�
	CToolTipCtrl m_ToolTip;
	BOOL m_bFocus;	//�Ƿ������뽹��
	CPoint m_offset[3];
	COLORREF m_cor1_menu,m_cor2_menu;	//MenuBt���ʱ,����ť�����º󻭵ı߿����ɫ
// Operations
public:
	void Init(UINT id,LPCSTR text,ButtonType bt,UINT nFormat=DT_BOTTOM|DT_CENTER,BOOL useWndRgn=FALSE,COLORREF corMask=RGB(0,255,0));
	void SetSel(BOOL sel);	//����ѡ��״̬
	BOOL GetSel();		//�ж�ѡ��״̬
	void SetBK(long bk[9]);	//��ͨ,����,��껬��,3��״̬,ÿ��״̬��������3��λͼƴ�ӳ�����	
	void SetBK(LPCTSTR bk[9]);
	void SetBK(CBitmap* bk[9]);
	void SetText(LPCSTR txt);
	void SetTextCor(COLORREF cor_text_n,COLORREF cor_text_d=0,COLORREF cor_text_o=0);	//���������ڲ�ͬ״̬�µ���ɫ
	void SetTextStyle(TextStyle style_n,TextStyle style_d,TextStyle style_o);	//���������ڲ�ͬ״̬�µķ��(����,б���)
	void SetTextHeight(int height_n,int height_d=-1,int height_o=-1);
	void SetTextOffset(int x_n,int y_n,int x_d,int y_d,int x_o=0,int y_o=0);	//���ò�ͬ״̬ʱ���ֵ�ƫ��,NormalBt,SwitchLabelBt���ʱ��Ч
	void SetTooltip(LPCSTR str);
	void SetCenterImage(long ID_CENTER);//��ť����ΪTitleBTʱ,������������λͼ
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
