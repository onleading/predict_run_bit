#ifndef HToolTipEx_H_
#define HToolTipEx_H_

enum {
	TTS_NORMAL,
	TTS_WITHTITLE
};
/////////////////////////////////////////////Define Tip
// CTipWindow window
class HToolTipEx : public CToolTipCtrl
{	
// Construction
public:
	HToolTipEx();

// Attributes
private:
	CWnd	* m_pWnd;
public:
	BOOL	m_Activate;
	BOOL    m_NeedZoom;

	CRect	m_title_rect;
	CRect	m_text_rect;
	CString	m_title_str;
	CString	m_text_str;
	COLORREF	m_title_bkcor1,m_title_bkcor2,m_title_textcor;
	COLORREF	m_text_bkcor,m_text_textcor;
	UINT	m_style;
// Operations
public:
	void Activate( BOOL bActivate );
	BOOL IsActive();
	void TrackPosition(int xPos, int yPos);
	void TrackPosition(LPPOINT lppt);
	void TrackActivate(BOOL bActivate, LPTOOLINFO lpti);
	int  SetMaxTipWidth(int nWidth);
	RECT SetMargin(LPRECT lpRect);
	int  GetMaxTipWidth();
	void GetMargin(LPRECT lpRect);
	int  GetDelayTime(DWORD dwType);
	int  SetDelayTime(DWORD dwType, int nTime);
	COLORREF GetTextColor();
	COLORREF GetBkColor();
	COLORREF SetTextColor(COLORREF crColor);
	COLORREF SetBkColor(COLORREF crColor);

	void UpdateTipText(LPCTSTR lpszText, CWnd* pWnd, UINT nIDTool = 0);
	void UpdateTipText(UINT nIDText, CWnd* pWnd, UINT nIDTool = 0);
	virtual void DrawIt();
	void SetStyle(UINT nStyle);
	void SetTitleBkColor(COLORREF crColor1,COLORREF crColor2);
	void SetTitleTextColor(COLORREF crColor);
	void DealText(LPCSTR lpszText,UINT style);
	BOOL AddTool(CWnd* pWnd, UINT nIDText, LPCRECT lpRectTool = NULL,UINT nIDTool = 0);
	BOOL AddTool(CWnd* pWnd, LPCTSTR lpszText = LPSTR_TEXTCALLBACK,LPCRECT lpRectTool = NULL, UINT nIDTool = 0);

	void InitToolTip(CWnd *pWnd,UINT nStyle);
	void UpdateToolTip(CWnd *pWnd,CString strTipInfo);
	void PopToolTip();

	void AddToolTip(CWnd *pWnd,CString strTipInfo,LPCRECT pRect,UINT nID);
	void DelToolTip(CWnd *pWnd);
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(HToolTipEx)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	HToolTipEx(CWnd *pParent);
	virtual ~HToolTipEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(HToolTipEx)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif
