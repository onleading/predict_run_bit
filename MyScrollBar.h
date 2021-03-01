#if !defined(AFX_MYSCROLLBAR_H__AC6D5350_7836_11D4_988A_00E04C66AD8E__INCLUDED_)
#define AFX_MYSCROLLBAR_H__AC6D5350_7836_11D4_988A_00E04C66AD8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/////////////////////////////////////////////////////////////////////////////

#define  X_VERTICAL  0 // 竖直
#define  X_HORIZON   1 // 水平

typedef enum{
	MSBS_DEFAULT=0,
	MSBS_SIMPLE,
}MSB_STYLE;

class CMyScrollBar : public CWnd
{
private:
	HWND		m_MsghWnd;
	UINT		m_Msg;
	int			m_WhichUnit;
	int			m_nDrawStyle;		//绘画风格
	int			m_nMouseStat;		//0 down 1 up 
	float		m_step;				//步长(像素单位)
	int			m_movebarwidth;		//滚动条长度
	CRect		m_rcBar;			//滚动条区间
	short m_nLower,m_nUpper;
	int   m_nScrollBarPos;
	int   m_nMinPos,m_nMaxPos;
	COLORREF	m_cor[10];
	MSB_STYLE	m_style;
public:
	CMyScrollBar();
	virtual ~CMyScrollBar();
private:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyScrollBar)
//	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	int  SetScrollPos( int nPos, BOOL bRedraw = TRUE );
	int	 GetScrollPos();
	void Init(int placestyle,HWND hwnd,UINT msg,int whichunit=0);
	void SetScrollColor(COLORREF cor[10]);
	void SetScrollColor(COLORREF bordercolor,int borderwidth,COLORREF  bc,COLORREF fc);
	void SetScrollRange( int nMinPos, int nMaxPos, BOOL bRedraw = TRUE );
	void DrawScrollBar(void);
	void SetStyle(MSB_STYLE msbs=MSBS_DEFAULT){m_style=msbs;};
private:
	void LButtonUp(MSG *pMsg);
	void LButtonDown(MSG *pMsg);
	void MouseMove(MSG *pMsg,int flag);
	void DrawButtonBG(CDC *pDC,CRect rc,BOOL bHorizontal);
	void DrawButtonMark(CDC *pDC,CRect rc,BOOL bHorizontal);
	void DrawLeftArrow(CDC *pDC,CRect rc);
	void DrawRightArrow(CDC *pDC,CRect rc);
	void DrawUpArrow(CDC *pDC,CRect rc);
	void DrawDownArrow(CDC *pDC,CRect rc);
	BOOL RegisterWindowClass(HINSTANCE hInstance,LPCTSTR lpszClassName);

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyScrollBar)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYSCROLLBAR_H__AC6D5350_7836_11D4_988A_00E04C66AD8E__INCLUDED_)
