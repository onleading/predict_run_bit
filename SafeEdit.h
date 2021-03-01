#ifndef __SAFE_EDIT__H__
#define __SAFE_EDIT__H__
#if _MSC_VER > 1000
#pragma once
#endif
#include <afxtempl.h>
#include "SafeEngine.h"

#define ES_EX_WANTTYPES		0x000F		// 允许输入的类型
#define ES_EX_WANTALLCHARS	0x0000		// 允许任意输入
#define ES_EX_WANTNUM		0x0001		// 仅允许数字输入
#define ES_EX_WANTCHAR		0x0002		// 仅允许字符输入
#define ES_EX_WANTALNUM		0x0003		// 仅允许数字和字符输入
#define ES_EX_READONLY		0x0010		// 只读模式
#define ES_EX_PASSWORD		0x0020		// 密码风格
#define ES_EX_UPPERCASE		0x0040		// 全部大写风格
#define ES_EX_LOWERCASE		0x0080		// 全部小写风格
#define ES_EX_HASMENU		0x0100		// 是否显示上下文菜单
#define ES_EX_HASCOPYMENU	0x0200		// 是否显示"复制"菜单
#define ES_EX_HASCUTMENU	0x0400		// 是否显示"剪切"菜单
#define ES_EX_HASPASTEMENU	0x0800		// 是否显示"粘贴"菜单
#define ES_EX_HASBORDER		0x1000		// 是否显示边框
#define ES_EX_GUIFONT		0x2000		// 是否使用GUI字体


class CSafeEdit : public CWnd,public CSafeControl
{
public:
	DECLARE_DYNAMIC(CSafeEdit)
	CSafeEdit(BOOL bSafeMode=TRUE);
	virtual ~CSafeEdit();
	BOOL IsSafeMode() const { return m_bSafeMode; }
	DWORD GetExtendedStyle() const { return m_dwExtendedStyle; }
	BOOL IsReadOnly() const { return (m_dwExtendedStyle&ES_EX_READONLY)!=0; }
	BOOL IsPassword() const { return (m_dwExtendedStyle&ES_EX_PASSWORD)!=0; }
	BOOL IsHasMenu() const { return (m_dwExtendedStyle&ES_EX_HASMENU)!=0; }
	BOOL IsHasCopyMenu() const { return (m_dwExtendedStyle&ES_EX_HASCOPYMENU)!=0; }
	BOOL IsHasCutMenu() const { return (m_dwExtendedStyle&ES_EX_HASCUTMENU)!=0; }
	BOOL IsHasPasteMenu() const { return (m_dwExtendedStyle&ES_EX_HASPASTEMENU)!=0; }
	BOOL IsHasBorder() const { return (m_dwExtendedStyle&ES_EX_HASBORDER)!=0; }
	LPCSTR GetHideMask() const { return m_strHideMask; }
	BOOL Create(DWORD dwStyle,const RECT& rect,CWnd* pParentWnd,UINT nID);
	VOID SetExtendedStyle(DWORD dwExtendedStyle);
	BOOL ModifyExtendedStyle(DWORD dwRemove,DWORD dwAdd,BOOL bRedraw);
	VOID SetHideMask(LPCSTR pszHideMask);
	LONG GetFirstVisible() const;
	VOID GetSel(LONG& nStartChar,LONG& nEndChar) const;
	VOID SetSel(LONG nStartChar,LONG nEndChar,BOOL bNoScroll=FALSE);
	LONG GetCaret() const;
	VOID SetLimitText(LONG nLimit);
	LONG GetLimitText() const;
	LONG GetLengthSafe() const;
	VOID GetTextSafe(CString& rString) const;
	VOID GetTextSafe(char *pStr,int nNum) const;
	VOID SetTextSafe(LPCSTR pszString);
	VOID ResetContent() { SetTextSafe(""); }
	VOID CleanString(CString* pszString);
	VOID PostCharKey(TCHAR cChar);
	VOID SetFeedBack(BOOL bUseFeedback,HWND hFeedback,UINT nFeedbackMsg);
public:
	virtual BOOL IsCharWanted(TCHAR cChar);
	virtual VOID OnSystemNotSafe();
	virtual BOOL HandleKbdEventSafe(LPVOID pUnknownKbdEvent);
protected:
	enum enumCONST
	{	CX_MARGINS=3,
		CY_MARGINS=3,
		MAX_EDITLIMITS=256,
	};
public:
	//{{AFX_VIRTUAL(CSafeEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual VOID PreSubclassWindow();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(CSafeEdit)
	afx_msg VOID OnDestroy();
	afx_msg VOID OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg VOID OnEditCopy();
	afx_msg VOID OnEditCut();
	afx_msg VOID OnEditPaste();
	afx_msg VOID OnEditClear();
	afx_msg VOID OnEditClearAll();
	afx_msg VOID OnEditSelectAll();
	afx_msg VOID OnPaint();
	afx_msg VOID OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg VOID OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg VOID OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg VOID OnSetFocus(CWnd* pOldWnd);
	afx_msg VOID OnKillFocus(CWnd* pNewWnd);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	BOOL RegisterWindowClass(HINSTANCE hInstance,LPCTSTR lpszClassName);
	VOID GenerateWeakKey(LPBYTE pWeakKey,DWORD cbWeakKey);
	VOID XorByWeakKey(LPSTR pszString,LONG nLength,const unsigned char* pWeakKey,DWORD cbWeakKey) const;
	VOID GetPaintTextSafe(CString& rString) const;
	VOID SubPaintText(LPCTSTR pszText,CString& rString,LONG nStart,LONG nCount) const;
	CPoint PosFromCharIndex(LONG nCharIndex);
	LONG CharIndexFromPos(CPoint point);
	LONG CorrectCharIndex(LONG nCharIndex);
	VOID ShowCaretWhenVisible();
	VOID HideCaretWhenVisible();
	VOID GetStringFromClipboard(LPSTR pszString,DWORD cbString);
	VOID SetStringToClipboard(LPCSTR pszString);
protected:
	BOOL		m_bSafeMode;
	DWORD		m_dwExtendedStyle;
	CString		m_strHideMask;
	BYTE		m_acWeakKey[MAX_EDITLIMITS];
	CFont		m_fontEdit;
	LONG		m_nSelStartChar;
	LONG		m_nSelEndChar;
	LONG		m_nFirstVisible;
	BOOL		m_bCaretValid;
	BOOL		m_bCaretVisible;
	BOOL		m_bTrackMouse;
	LONG		m_nTrackFrom;
	LONG		m_nShiftFrom;
	CPoint		m_ptTrackFrom;
	LONG		m_nLength;
	LONG		m_nLengthLimit;
	CHAR		m_szText[MAX_EDITLIMITS];
protected:
	BYTE		m_szKey[MAX_PATH];
	SHORT		m_nKeyLen;
	LPBYTE		m_lpSafeBuf;
	DWORD		m_dwSafeLen;
	DWORD		m_dwStringLen;
	BOOL        m_bUseFeedback;
	HWND		m_hFeedback;
	UINT		m_nFeedbackMsg;
};

//{{AFX_INSERT_LOCATION}}
#endif
